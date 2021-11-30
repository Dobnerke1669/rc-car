//Client part of the code - basically, the remote control
//Version 1, the one we ran together

#include <SPI.h>
#include <RH_NRF24.h>

//data[0] - speed
//data[1] - 1 forward  0 reverse
//data[2] - 0 left 1 straight 2 right

RH_NRF24 nrf24;

void setup() 
{
  Serial.begin(9600);
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

  pinMode(A5, INPUT); //POTENCIOMETER INPUT - SPEED
  pinMode(A4, INPUT); //POTENCIOMETER INPUT - WHEEL
  pinMode(A3, INPUT); //SWITCH INPUT - DIRECTION
}

int speedValue;
int wheelValue;
int directionValue;

void loop()
{
  speedValue = analogRead(A5);
  directionValue = analogRead(A3);
  wheelValue = analogRead(A4);

  uint8_t data[3];

  data[0] = map(speedValue, 0, 1023, 0, 255);
  
  if (directionValue > 500)
    data[1] = 1;
  else
    data[1] = 0;
    
    Serial.println(directionValue);

  if (wheelValue <= 340)
    data[2] = 0;
   else if (wheelValue >= 684)
    data[2] = 2;
   else
    data[2] = 1;
  
  
  
  
  Serial.print("Speed: ");
  Serial.print(data[0]);
  Serial.print(" Direction: ");
  Serial.print(data[1]);
  Serial.print(" Wheel: ");
  Serial.println(data[2]);
  
  // Send a message to nrf24_server
  nrf24.send(data, sizeof(data));
  nrf24.waitPacketSent();
  //delay(500);

}
