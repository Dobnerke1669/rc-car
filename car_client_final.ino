/*  Bluetooth Controlled Arduino Car
 *   
 *  A Project Assembled by the team of Nasty/Automatas
 *      @Bálint Roland
 *      @Dobner Szabolcs
 *      @Sasu-Simon Kinga
 *      @Varga Zoltán
 *  
 *  12/18/2021
 *  
 *  Client part of the code - this code is uploaded on an arduino serving as a controller for the car
 *  
 *  Tasks: - interaction with user
 *           - read desired speed and direction indicators 
 *           - send to serial monitor the values from server to display it nicely (radar)
 *         - encode data about speed and direction and send it to the server
 *  
 *  Data send data is encoded on the array dataGet, on 3 integers, each index representing a different property:
 *       dataSend[0] - speed       -   a value between [0,255]   0 - minimum           255 - maximum
 *       dataSend[1] - direction   -   1 - go forward            0 - go backwards
 *       dataSend[2] - wheel       -   0 - go left               1 - go straight       2- go right
 *  
 *  Data received is encoded on the array dataSend, on 2 integers:
 *       dataGet[0] - current degree of servomotor
 *       dataGet[1] - distance of (possible) object from ultrasonic sensor
 *  
 *  For an orientative description of assembly, check the images from the presentation
 *  
 *  NOTE: When using radar display, no other use of serial monitor is allowed!
*/

#include <SPI.h>        //for communication
#include <RH_NRF24.h>   //for communication


RH_NRF24 nrf24;   //instance of the radio driver needed for communication

const int SPEED = 14;          //potentiometer input for speed  - A0
const int DIRECTION = 15;      //switch input for direction     - A1
const int WHEEL = 16;          //potentiometer input for wheel  - A2


void setup() 
{
  //for using the serial monitor
  Serial.begin(9600);

  //some error checking provided by the RadioHead sample program "rf24_client"
  if (!nrf24.init())
    Serial.println("init failed");
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

  //setting the pin modes for pins
  pinMode(SPEED, INPUT);      
  pinMode(DIRECTION, INPUT);  
  pinMode(WHEEL, INPUT);    
}


int speedValue;     //will store
int directionValue; //values
int wheelValue;     //to send


void loop()
{
  //read the data from the sensors
  speedValue = analogRead(SPEED);
  directionValue = analogRead(DIRECTION);
  wheelValue = analogRead(WHEEL);

  //declare array to send message
  uint8_t dataSend[3];
 
  //interpret and encode input
  dataSend[0] = map(speedValue, 0, 1023, 0, 255); //we want speed in range [0,255], for DC motor

  if (directionValue > 700)   //for high values, we go forwards
    dataSend[1] = 1; 
  else                        //for low values, we go backwards
    dataSend[1] = 0;

  if (wheelValue <= 340)        //for values [0,340] we go left
    dataSend[2] = 0;                
   else if (wheelValue >= 684)  //for values [684,1024] we go right
    dataSend[2] = 2;                
   else                         //for values (340,684) we go straight
    dataSend[2] = 1;                
  

/*
    //Print on serial monitor, for debugging purposes
    Serial.print("Speed: ");
    Serial.print(dataSend[0]);
    Serial.print(" Direction: ");
    Serial.println(dataSend[1]);
    Serial.print(" Wheel: ");
    Serial.println(dataSend[2]);
*/

  //send the message
  nrf24.send(dataSend, sizeof(dataSend));
  nrf24.waitPacketSent();

  //receive message
  uint8_t dataGet[2];
  if (nrf24.available())
  {
    if (nrf24.recv(dataGet, sizeof(dataGet)))
    {
      /*  If everything goes well, we get to this point
       *   - print the degree and distance in this format
       *   - instead of opening serial monitor, run radar.pde 
       *  Note: this file probably needs its own directory aswell
       */
      Serial.print(dataGet[0]);
      Serial.print(",");
      Serial.print(dataGet[1]);   
      Serial.print(".");
    }
  }
}
