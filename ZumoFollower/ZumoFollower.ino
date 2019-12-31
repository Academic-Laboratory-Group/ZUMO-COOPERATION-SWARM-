#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

#include <Wire.h>
#include <ZumoShield.h>

enum roles 
{
  Leader = 0, 
  Student = 1
};

enum commands
{
  GoForward = 0,
  GoBackward = 1,
  TurnRight = 2,
  TurnLeft = 3  
};

roles role { roles::Leader };
const int sizeOfString = 40;
const int speed = 100;

ZumoMotors motors;
RF24 radio (7, 8);
const byte rxAddr[6] = "00001";
char * recivedMessage[sizeOfString]{};
char * sendedMessage[sizeOfString]{};
unsigned order{};

//brief\ Writes message to radio and console
void writeMessage()
{
  Serial.println("System: writeMessage");
  radio.write(sendedMessage, sizeof(*sendedMessage));
  Serial.print("Me: ");
  Serial.println(*sendedMessage);
}


//brief\ Writes order to radio and console
void writeOrder()
{
  Serial.println("System: writeOrder");
  radio.write(&order, sizeof(order));
  Serial.print("Me: ");
  Serial.println(order);
}

//brief\ Reads  from radio to message and console
void read()
{
  Serial.println("System: read()");
  radio.read(recivedMessage, sizeof(*recivedMessage));
  Serial.print("My friend: ");
  Serial.println(*recivedMessage);
}

//brief\ Converts message to order when student gets order in a message
void message2Order()
{
  Serial.println("System: message2Order()");
  Serial.println("System: recivedMessage: ");
  Serial.print(*recivedMessage);
  order = ((unsigned)*recivedMessage);
  Serial.print(" order: ");
  Serial.println(order);
}

//brief\ Executes command from message
void execute()
{
  Serial.println("System: execute()");
  const unsigned command = order / 1000000;
  const unsigned duration = order % 1000000;
  Serial.println("System: command: ");
  Serial.print(command);
  Serial.print(" duration: ");
  Serial.print(duration);
  Serial.print(" message: ");
  Serial.print(*recivedMessage);
//  if (command == commands::GoForward)
//  {
//    Serial.println("System: I'm going forward");
//    motors.setLeftSpeed(speed);
//    motors.setRightSpeed(speed);
//  }
//  else if (command == commands::GoBackward)
//  {
//    Serial.println("System: I'm going backward");
//    motors.setLeftSpeed(speed);
//  }
//  else if (command == commands::TurnRight)
//  {
//    Serial.println("System: I'm turning to the right");
//    motors.setLeftSpeed(speed);
//  }
//  else if (command == commands::TurnLeft)
//  {
//    Serial.println("System: I'm turning to the left");
//    motors.setRightSpeed(speed);
//    motors.setLeftSpeed(speed);
//  }
  motors.setLeftSpeed(speed);
  motors.setRightSpeed(speed);
  delay(duration);
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);
  Serial.println("System: End of executing");
}

//brief\ Setups radio, roles, etc.
void setup() 
{
  while(!Serial);
  Serial.begin(9600);

  radio.begin();
  radio.setRetries(15, 15);
  radio.openReadingPipe(0, rxAddr);
  radio.startListening(); 
  Serial.println("System: radio setup done");
  delay(5000);
  Serial.println("System: Delay after setup done");
  Serial.print("System: role: ");
  Serial.println(role);
  
  if (role == Leader)
  {
    Serial.println("System: I'm a leader. Sending first message");
    // Sending first message as a leader
    radio.stopListening();
    radio.openWritingPipe(rxAddr);
    *sendedMessage = "My turn to be a leader!";
    writeMessage();
    delay(10000);
    
    Serial.println("System: Be ready to send first command");
    // Be ready to give first command
    Serial.println("System: End of setup");
    // That's all for setup of Leader
  }
  else if (role == Student)
  {
    radio.openReadingPipe(0, rxAddr);
    radio.startListening();

    while (1)
    {
      if(radio.available())
      {
        Serial.println("System: Something catched");
        read();
        break;
      }
    }
    Serial.println("System: Be ready to first command. End of setup");
    // Be ready to first command 
  }
  else
  {
    Serial.println("Error! 159");
    delay(10000000);      
  }
}

void loop() 
{
  if (role == Leader)
  {
    Serial.println("System: Sending order as a leader");
    // Sending order
    order = commands::GoBackward * 1000000 + 20000;
    writeOrder();
    delay(200);
    Serial.println("System: Start to execute my own command");
    // Despite the fact of content of message start executing
    execute();
    
    Serial.println("System: Be ready to give next command. End of this loop as a Leader");
    // Be ready to give next command
  }
  else if (role == Student)
  {
    Serial.println("System: Waiting for an command as a student");
    if(radio.available())
    {
      Serial.println("System: Gotten sth let's read this");
      // Reading command
      read();
      
      if (*recivedMessage == "My turn to be a leader!")
      {
        Serial.println("Wait for him");   
      }
      else
      {  
        Serial.println("System: Convert message to order");
        // Convert message to order
        message2Order();
        
        Serial.println("System: Execute order");
        // Execute order
        execute();

        Serial.println("System: Be ready to get next order as a Student");
        // Be ready to get next order as a Student
      }
    } 
    else
    {
      delay(1000);
    }
  }
  else
  {
    Serial.println("Error! 211");
    delay(10000000);      
  }
  delay(200);
}
