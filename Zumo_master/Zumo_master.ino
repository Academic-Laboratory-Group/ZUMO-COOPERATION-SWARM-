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
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(rxAddr);
  radio.stopListening();
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
  const char go[] = "Go";
  radio.write(&go, sizeof(go));
  Serial.print("Master: ");
  Serial.println(go);
  delay(2000);
  gozumo();
  const char back[] = "Back";
  radio.write(&back, sizeof(back));
  Serial.print("Master: ");
  Serial.println(back);
  delay(2000);
  backzumo();
}
