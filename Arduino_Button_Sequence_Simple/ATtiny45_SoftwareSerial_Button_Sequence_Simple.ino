#include <SoftwareSerial.h>
#include <Arduino.h>
#include <stdint.h>

SoftwareSerial ScooterSerial(3,4);
#define BUTTON_PIN 2
#define RESEND 4

int speed = 30;
int presses=5;

uint16_t calculateChecksum(uint8_t *data)
{
  uint8_t len = data[0] + 2;
  uint16_t sum = 0;
  for (int i = 0; i < len; i++)
    sum += data[i];

  sum ^= 0xFFFF;
  return sum;
}


void setSpeed(const int speed) { //Setze Maximalgeschwindigkeit in km/h
  uint8_t data[] = {
    0x55, 0xAA, 0x04, 0x22, 0x01, 0xF2,
    0, 0, //rpm
    0, 0, //checksum
  };

  *(uint16_t *)&data[6] = (speed * 252) / 10;
  *(uint16_t *)&data[8] = calculateChecksum(data + 2);

  for (int i=0; i<=RESEND; i++) {
    if (i != 0) {
      delay(150);
    }
    ScooterSerial.write(data, sizeof(data) / sizeof(uint8_t));
  }

}

bool waitwhilepress() {
          int i=0;
           while(digitalRead(BUTTON_PIN) == LOW) {
              delay(50);
              if(i>2) return false;
              i++;
            }
            return true;
}
bool waitforpress() {
          int i=0;
           while(digitalRead(BUTTON_PIN) == HIGH) {
              delay(50);
              if(i>2) return false;
              i++;
            }
            return true;
}

bool checkCode(int presses) {
  unsigned long time=0;
  unsigned long temp=0;

      if(digitalRead(BUTTON_PIN) == HIGH) return false;
         time = millis();
         for(int i=0; i<(presses+1); i++) {
                   if(!waitwhilepress()) return false;
                   delay(50);
                   if(!waitforpress()) return false;
                   delay(50);
         }

         temp = millis();
         if((temp-time) < 1650 && (temp-time) > 500) return true;

}

void setup()
{
  pinMode(BUTTON_PIN, INPUT);
  ScooterSerial.begin(115200);


}

void loop()
{
           if(checkCode(presses)) {
            setSpeed(speed);
            presses=3;
            speed=20;
           }
           delay(50);
}