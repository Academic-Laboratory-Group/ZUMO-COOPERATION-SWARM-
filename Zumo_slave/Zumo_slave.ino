#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

#include <Wire.h>
#include <ZumoShield.h>

ZumoMotors motors;

RF24 radio (7, 8);
const byte rxAddr[6] = "00001";

void setup()
{
  while(!Serial);
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, rxAddr);
  radio.startListening();
}

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

void loop()
{
  if(radio.available())
 {
   char text[32] = {0};
   radio.read(&text, sizeof(text));

   Serial.print("Slave: ");
   Serial.println(text);
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
