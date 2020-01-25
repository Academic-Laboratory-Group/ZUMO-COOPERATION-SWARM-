#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <Wire.h>
#include <ZumoShield.h>

/*------------------------------------------

  Slave of Zumo Cooperation Swarm.
  --------------------------------
  \description:
    One of three Zumo Cooperation Swarm 
    projects. This one doesn't need any 
    settings and should be pushed to one 
    of the robots. This one gets and 
    executes orders without checking 
    how it goes on the master side.
    This is one of the easiest way but
    it doesn't work at every time we have
    trying.
*///------------------------------------------

// Some variables, objects etc.
ZumoMotors motors;
RF24 radio (7, 8);
const byte rxAddr[6] = "00001";


//Setup for Serial and radio
//-----------------------------------------
//brief\ it sets serial and radio
//       and checks if it is setted well
//-----------------------------------------
void setup()
{
  while(!Serial);
  Serial.begin(9600);

  radio.begin();                       // Attempt to re-configure the radio with defaults
  radio.openReadingPipe(1,rxAddr);
  radio.startListening();

  Serial.print("isChipConnected(): ");
  Serial.println(radio.isChipConnected());
  Serial.print("radio.isValid(): ");
  Serial.println(radio.isValid());
}

//gozumo
//-----------------------------------------
//brief\ Zumo should move forward with 
//       400 speed value with proper 
//       acceleration
void gozumo()
{
  for (int speed = 0; speed <= 400; speed++)
  {
    motors.setSpeeds(speed,speed);
    delay(2);
  }

  for (int speed = 400; speed >= 0; speed--)
  {
    motors.setSpeeds(speed,speed);
    delay(2);
  }
}

//backzumo
//-----------------------------------------
//brief\ Zumo should move back with 
//       400 speed value with proper 
//       acceleration
void backzumo()
{
   for (int speed = 0; speed >= -400; speed--)
  {
    motors.setSpeeds(speed,speed);
    delay(2);
  }

  for (int speed = -400; speed <= 0; speed++)
  {
    motors.setSpeeds(speed,speed);
    delay(2); 
  }
}

//loop
//-----------------------------------------
//brief\ Main loop in program. Controls
//       radio by getting orders, calls 
//       executing functions
void loop()
{
  //Checking if it is any order
  Serial.println("Checking radio. ");
  if(radio.available())
 {
   //Variable for order
   char text[32] = {0};
   //Readeing order
   radio.read(&text, sizeof(text));
   
   //In case it can help
   delay (1000);
   //Infos for user
   Serial.print("Slave: ");
   Serial.println(text);

   //Executing
   if (text == "Go")
   {
      gozumo();
   }
   if (text == "Back")
   {
      backzumo();
   }
 }
 delay (1000);
}
