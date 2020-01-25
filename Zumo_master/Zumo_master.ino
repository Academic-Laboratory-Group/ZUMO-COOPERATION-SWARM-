/*------------------------------------------

  Master of Zumo Cooperation Swarm.
  --------------------------------
  \description:
    One of three Zumo Cooperation Swarm 
    projects. This one doesn't need any 
    settings and should be pushed to one 
    of the robots. This one gives and 
    executes orders without checking 
    how it goes on the slave side.
    This is one of the easiest way but
    it doesn't work at every time we have
    trying.
*/------------------------------------------
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <Wire.h>
#include <ZumoShield.h>

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
  //Setting serial
  while(!Serial);
    Serial.begin(115200);

  //Setting radio 
  radio.begin();                       // Attempt to re-configure the radio with defaults
  radio.openWritingPipe(rxAddr);       // Re-configure pipe addresses
  
  radio.stopListening();

  //Checking radio
  Serial.println("System: ");
  Serial.println("printDetails(): ");
  radio.printDetails();
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
//       radio by sending orders, calls 
//       executing functions
void loop()
{
  //Order
  const char go[] = "Go";
  //Serial printing for user
  Serial.print("Master: ");
  Serial.println(go);
  Serial.print("System: Success?: ");
  //Sending order
  radio.write(&go, sizeof(go));
  //Waiting for sending all from radio's fifo
  Serial.print(radio.txStandBy(1000)); 

  delay(2000);
  //Executing 
  gozumo();

  //Order
  const char back[] = "Back";
  //Serial printing for user
  Serial.print("Master: ");
  Serial.println(back);
  Serial.print("System: Success?: ");
  //Sending order
  radio.write(&back, sizeof(back));

  //Waiting for sending all from radio's fifo
  Serial.print(radio.txStandBy(1000)); 
  delay(2000);
  //Executing 
  backzumo();
}
