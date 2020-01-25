/*------------------------------------------

  Master of Zumo Cooperation Swarm.
  --------------------------------
  \description:
    One of three Zumo Cooperation Swarm 
    projects. This one applays both sides, 
    master and slave. You have posiibility 
    to choose if this robot is master of 
    slave before pushing program to zumos. 
    The other settings are scenario and 
    number of commands.
*///------------------------------------------
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <Wire.h>
#include <ZumoShield.h>

////////////////////////////////////////////////////////////
///
/// Some needed stuff before settings. Setting are below.
///
///---------------------------------------------------------

// Available roles
enum roles 
{
  Leader = 0, 
  Student = 1
};

// Available commands
enum commands
{
  GoForward = 0,
  GoBackward = 1,
  TurnRight = 2,
  TurnLeft = 3  
};

////////////////////////////////////////////////////////////
///
/// The place where is all the stuff You must set up 
/// before uploading this software to Your Zumo
///
///---------------------------------------------------------
///
/// The role of Your Zumo shield
///
///------------------------------
/// One of Zumos needs to be a leader and the second 
/// must be a Student.

roles role { roles::Student };

///---------------------------------------------------------
///
/// Number of commands in scenario to do
///
///------------------------------
/// This is number of commands to do with scenario.
/// After exceeding this value by state of scenario.
/// State is changed to zero and scenario is started
/// from beginning

const unsigned commandsAmount { 4 };

///---------------------------------------------------------
///
/// Scenario of moving
///
///------------------------------
/// This is a scenario which the Leader is sending to the 
/// Student and executed by both of them.
/// First of arguments is direction, second speed and the 
/// last is duration of moving.
///
/// To change easily by user:
/// First size of array. It is number of comments in 
/// scenario. Second is number of properties of each 
/// scenario and it should stay the same for You. Code do 
/// not predicts more arguments for this.
/// 
/// Max value of direction argument is 9, of speed is 999 
/// and duriation 999.
/// Min value of speed is 201 because of shield.

const int scenario[commandsAmount][3] = {
  {GoForward, 600, 600}, 
  {TurnLeft, 300, 700},
  {TurnRight, 500, 900},
  {GoBackward, 400, 200}};

///
////////////////////////////////////////////////////////////

/// Size of strings used to sending and receiving messages
/// orders etc.
const unsigned sizeOfString = 32;

/// State of scenario
unsigned scenarioState{};

/// -------- Some other stuff --------
ZumoMotors motors;
RF24 radio (7, 8);
const byte rxAddr[6] = "00001";
char * recivedMessage[sizeOfString]{};
char * sendedMessage[sizeOfString]{};
unsigned order{};
///-----------------------------------

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

//brief\ Reads from radio to message and console
void read()
{
  Serial.println("System: read()");
  radio.read(recivedMessage, sizeof(*recivedMessage));
  Serial.print("My friend: ");
  Serial.println(*recivedMessage);
}

//brief\ Converts message to order when student gets 
//       order in a message
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
  const unsigned speed = (order % 1000000) / 1000;
  const unsigned duration = order % 1000;
  Serial.print("System: command: ");
  Serial.print(command);
  Serial.print(" speed: ");
  Serial.print(speed);
  Serial.print(" duration: ");
  Serial.print(duration);
  Serial.print(" order: ");
  Serial.print(order, DEC);
  Serial.print(" message: ");
  Serial.println(*recivedMessage);
  
  if (command == commands::GoForward)
  {
    Serial.println("System: I'm going forward");
    motors.setLeftSpeed(speed);
    motors.setRightSpeed(speed);
  }
  else if (command == commands::GoBackward)
  {
    Serial.println("System: I'm going backward");
    motors.setLeftSpeed(-speed);
    motors.setRightSpeed(-speed);
  }
  else if (command == commands::TurnRight)
  {
    Serial.println("System: I'm turning to the right");
    motors.setLeftSpeed(speed);
  }
  else if (command == commands::TurnLeft)
  {
    Serial.println("System: I'm turning to the left");
    motors.setRightSpeed(speed);
  }
  
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
  Serial.println("System: radio setup done");
  Serial.print("System: role: ");
  Serial.println(role);

  // Leader setup and synchronization
  if (role == Leader)
  {
    Serial.println("System: I'm a leader. Sending first message");
    // Sending first message as a leader
    radio.stopListening();
    radio.openWritingPipe(rxAddr);
    *sendedMessage = "My turn to be a leader!";
    writeMessage();
    delay(5000);
    
    Serial.println("System: Be ready to send first command");
    // Be ready to give first command
    Serial.println("System: End of setup");
    // That's all for setup of Leader
  }
  // Student setup and synchronization
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

//brief\ Main loop in program
void loop() 
{
  if (role == Leader)
  {
    Serial.println("System: Sending order as a leader");
    
    // Sending next order
    order = scenario[scenarioState][0] * 1000000 + scenario[scenarioState][1] * 1000 
            + scenario[scenarioState][2];
    writeOrder();
    Serial.print("Success: ");
    Serial.println( radio.txStandBy(1000));
    
    // Incrementing state
    ++scenarioState;
    if (scenarioState > commandsAmount - 1)
    {
      scenarioState = 0;
    }
    delay(200);
    
    Serial.println("System: Start to execute my own command");
    // Start executing
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
