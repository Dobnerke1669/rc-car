/* Bluetooth Controlled Arduino Car
 *  Client part of the code - the remote control
 *  Main purpose: - read the desired speed and direction
 *                - encode it 
 *                - send it to the server (to the car)
 *                
 * Data is encoded on an array "data" of 3 integers, each index representing a different property:
 *      data[0] - speed       -   a value between [0,255]   0 - minimum           255 - maximum
 *      data[1] - direction   -   1 - go forward            0 - go backwards
 *      data[2] - wheel       -   0 - go left               1 - go straight       2- go right
 * 
 * Assembly: - connect potentiometers to ground and vcc, put output at A0 and A2
 *           - get a cable from vcc, go through a switch, put it at A1
 * 
 * Tested individually (without the bluetooth device)
 */

#include <SPI.h>        //for communication between two controllers
#include <RH_NRF24.h>   //a RadioHead library for NRF24L01 transciever

///Singleton instance of the radio driver
RH_NRF24 nrf24;

///Definition of pins
int SPEED = 14;          //potentiometer input for speed  - A0
int DIRECTION = 15;      //switch input for direction     - A1
int WHEEL = 16;          //potentiometer input for wheel  - A2


void setup() 
{
  Serial.begin(9600);   //for using the serial monitor

  ///Some error checking provided by the RadioHead sample program "rf24_client"
  if (!nrf24.init())
    Serial.println("init failed");
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

  ///Setting the pin modes for pins
  pinMode(SPEED, INPUT);      
  pinMode(DIRECTION, INPUT);  
  pinMode(WHEEL, INPUT);      
}

int speedValue;
int directionValue;
int wheelValue;

void loop()
{
  ///Read the data from the sensors
  speedValue = analogRead(SPEED);
  directionValue = digitalRead(DIRECTION);
  wheelValue = analogRead(WHEEL);


  ///Declare array for encoded message
  uint8_t data[3];


  ///Interpret the data
  data[0] = map(speedValue, 0, 1023, 0, 255); //map the value of speed to be between 0 and 255

  data[1] = directionValue; //since read digitally, we already have an analog to digital conversion

  if (wheelValue <= 340)
    data[2] = 0;                //for values [0,340] we go left
   else if (wheelValue >= 684)
    data[2] = 2;                //for values [684,1024] we go right
   else
    data[2] = 1;                //for values (340,684) we go straight
  

  ///Print on serial monitor, for debugging purposes
  Serial.print("Speed: ");
  Serial.print(data[0]);
  Serial.print(" Direction: ");
  Serial.print(data[1]);
  Serial.print(" Wheel: ");
  Serial.println(data[2]);

  
  ///Send the message
  nrf24.send(data, sizeof(data));
  nrf24.waitPacketSent();
}
