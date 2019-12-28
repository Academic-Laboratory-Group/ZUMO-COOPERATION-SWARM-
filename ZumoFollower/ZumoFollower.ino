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

roles role{};
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
  if (command == GoForward)
  {
    Serial.println("System: I'm going forward");
    motors.setSpeeds(speed, speed);
  }
  else if (command == GoBackward)
  {
    Serial.println("System: I'm going backward");
    motors.setSpeeds(-speed, -speed);
  }
  else if (command == TurnRight)
  {
    Serial.println("System: I'm turning to the right");
    motors.setLeftSpeed(speed);
  }
  else if (command == TurnLeft)
  {
    Serial.println("System: I'm turning to the left");
    motors.setRightSpeed(speed);
  }
  delay(duration);
  motors.setSpeeds(0, 0);
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
  
  // Listening in case that other Zumo was first turn on
  // If other zumo will not send anything until i = 1000
  // then this is Leader
  for (unsigned i = 0; i < 1000; ++i)
  {
    if (radio.available())
    {
      Serial.println("System: I'm a student");
      read();
      role = Student;
      break;
    }
    role = Leader;
    i++;
  }

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
    delay(2000);
    
    Serial.println("System: Waiting for agree");
    // Waiting for agree
    radio.openReadingPipe(0, rxAddr);
    radio.startListening();
    while(1)
    {
      if(radio.available())
      {
        Serial.println("System: Something catched");
        read();
        break;
      }
      else
      {
        radio.stopListening();
        radio.openWritingPipe(rxAddr);
        *sendedMessage = "My turn to be a leader!";
        writeMessage();
        delay(200);
        radio.openReadingPipe(0, rxAddr);
        radio.startListening();
      }
    }
    Serial.println("System: Be ready to send first command");
    // Be ready to give first command
    radio.stopListening();
    radio.openWritingPipe(rxAddr);
    Serial.println("System: End of setup");
    // That's all for setup of Leader
  }
  else if (role == Student)
  {
    Serial.println("System: Sending agreement to be a student");
    // Our agreement to be a student in this turn
    while(1)
    {
      if(!radio.available())
      {
        radio.stopListening();
        radio.openWritingPipe(rxAddr);
        *sendedMessage = "Ok Buddy! Let's go!";
        writeMessage();
        delay(200);
        radio.openReadingPipe(0, rxAddr);
        radio.startListening();
      }
      else
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
    Serial.println("Error! 104");
    delay(10000000);      
  }
}

void loop() 
{
  if (role == Leader)
  {
    Serial.println("System: Sending order as a leader");
    // Sending order
    order = GoForward * 1000000 + 200;
    writeOrder();
    delay(1000);
    Serial.println("System: Start listening if order is accepted");
    // Waiting for acceptation of order
    radio.openReadingPipe(0, rxAddr);
    radio.startListening();
    while(1)
    {
      if(radio.available())
      {
        Serial.println("System: Something catched");
        read();
        break;
      }
    }
    Serial.println("System: Start to execute my own command");
    // Despite the fact of content of message start executing
    execute();

    // Check what a message I got before to by synchronized with my buddy
    Serial.println("System: Check what a message I got before to by synchronized with my buddy");
    if (*recivedMessage == "Ok! Let's go!")
    {
      Serial.println("System: It was Ok! Let's go! so I must wait if he has done");
      // Waiting for Student to finish his executing and get an response
      while(1)
      {
        if(radio.available())
        {
          // Message probably about finished command
          Serial.println("System: He has send sth so he's done probably");
          read();
          break;
        }
      }  
    }
    else if (*recivedMessage == "Done!")
    {
      //Leader was a bit late with executting but it doesn't matter. Start next loop!
      Serial.println("I was a bit late!");
    }
    else
    {
      Serial.println("Error! 139");
      delay(10000000);      
    }
    
    Serial.println("System: Be ready to give next command. End of this loop as a Leader");
    // Be ready to give next command
    radio.stopListening();
    radio.openWritingPipe(rxAddr);
  }
  else if (role == Student)
  {
    Serial.println("System: Waiting for an command as a student");
    if(radio.available())
    {
      Serial.println("System: Gotten sth let's read this");
      // Reading command
      read();

      Serial.println("System: Ok I've got it. Give him an answer that I have it");
      // Giving an answer that wi will do this command
      radio.stopListening();
      radio.openWritingPipe(rxAddr);
      *sendedMessage = "Ok! Let's go!";
      writeMessage();

      Serial.println("System: Start executing given comman to me but before conversion");
      // Executing given command but before convert message to be an order
      message2Order();
      execute();

      Serial.println("System: I've done it, send Leader a message about it");
      // Sending message that we've done what we've done
      *sendedMessage = "Done!";
      writeMessage();
      delay(1000);

      Serial.println("System: Be ready t next command. End of this loop as a Student");
      // Be ready to next command
      radio.openReadingPipe(0, rxAddr);
      radio.startListening();
    } 
  }
  else
  {
    Serial.println("Error! 172");
    delay(10000000);      
  }
}
