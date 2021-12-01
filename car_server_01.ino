/* Bluetooth Controlled Arduino Car
    Server part of the code - the car itself
    Main purpose: - get the encoded message
                  - interpret it
                  - act as asked

   Data is encoded on an array "data" of 3 integers, each index representing a different property:
        data[0] - speed       -   a value between [0,255]   0 - minimum           255 - maximum
        data[1] - direction   -   1 - go forward            0 - go backwards
        data[2] - wheel       -   0 - go left               1 - go straight       2- go right

  TDOD: implement go functions - if possible, make use of enable pins too

*/

#include <SPI.h>
#include <RH_NRF24.h>

///Singleton instance of the radio driver
RH_NRF24 nrf24;

///Pins referring to the left motor
int ENABLE_LEFT = 6;
int MOTOR_LEFT_1 = 2;
int MOTOR_LEFT_2 = 3;

///Pins referring to the right motor
int ENABLE_RIGHT = 5;
int MOTOR_RIGHT_1 = 4;
int MOTOR_RIGHT_2 = 7;

int SPEED = 255;
void setup()
{
  Serial.begin(9600);      //for using the serial monitor

  ///Some error checking provided by the RadioHead sample program "rf24_server"
  while (!Serial)
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

  ///Setting the pin modes for pins
  pinMode(ENABLE_LEFT, OUTPUT);
  pinMode(MOTOR_LEFT_1, OUTPUT);
  pinMode(MOTOR_LEFT_2, OUTPUT);
  pinMode(ENABLE_RIGHT, OUTPUT);
  pinMode(MOTOR_RIGHT_1, OUTPUT);
  pinMode(MOTOR_RIGHT_2, OUTPUT);
}

int speedValue;
int directionValue;
int wheelValue;

void goForwardLeft()
{
  return;
}
void goForwardStraight()
{
  return;
}
void goForwardRight()
{
  return;
}
void goBackwardsLeft()
{
  return;
}
void goBackwardsStraight()
{
  return;
}
void goBackwardsRight()
{
  return;
}

void loop()
{
  if (nrf24.available())
  {
    uint8_t data[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(data);

    if (nrf24.recv(data, &len)) //if we got the message
    {
      ///decompose the data for easier understanding - also easier to change if needed
      speedValue = data[0];
      directionValue = data[1];
      wheelValue = data[2];

      ///Print on serial monitor, for debugging purposes
      Serial.print("got request: SPEED | DIRECTION | STEERING : ");
      Serial.print(speedValue);
      Serial.print("  |  ");
      Serial.print(directionValue);
      Serial.print("  |  ");
      Serial.println(wheelValue);

      if (speedValue <= 50) //if speed is not high enough to move the car, don't even try
      {
        //stop motor
      }
      else
      {
        if (directionValue = 1)       //if we go forward
        {
          switch (wheelValue)
          {
            case 0:
              goForwardLeft();
              break;
            case 1:
              goForwardStraight();
              break;
            case 2:
              goForwardRight();
              break;
            default:
              Serial.println("Unknown wheel value! (1)");
              break;
          }
        }
        else if (directionValue = 0)  //if we go backwards
        {
          switch (wheelValue)
          {
            case 0:
              goBackwardsLeft();
              break;
            case 1:
              goBackwardsStraight();
              break;
            case 2:
              goBackwardsRight();
              break;
            default:
              Serial.println("Unknown wheel value! (2)");
              break;
          }
        }
        else                          //if we get bad data
        {
          Serial.println("Unknown direction value!");
        }
      }
    }
    else
    {
      Serial.println("No message, an error occuded!");
    }
  }
}
