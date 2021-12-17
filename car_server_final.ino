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
 *  Server part of the code - this code is uploaded on the arduino on the car itself
 *  
 *  Tasks: - get the encoded message from the controller
 *         - interpret the message, drive the DC motors correctly
 *         - gather data from parking sensor
 *         - send the data to the controller to display it nicely
 *  
 *  Received data is encoded on the array dataGet, on 3 integers, each index representing a different property:
 *       dataGet[0] - speed       -   a value between [0,255]   0 - minimum           255 - maximum
 *       dataGet[1] - direction   -   1 - go forward            0 - go backwards
 *       dataGet[2] - wheel       -   0 - go left               1 - go straight       2- go right
 *  
 *  Data sent is encoded on the array dataSend, on 2 integers:
 *       dataSend[0] - current degree of servomotor
 *       dataSend[1] - distance of (possible) object from ultrasonic sensor
 *  
 *  For an orientative description of assembly, check the images from the presentation
*/


#include <SPI.h>        //for communication
#include <RH_NRF24.h>   //for communication
#include <Servo.h>      //for servomotor


RH_NRF24 nrf24;   //instance of the radio driver needed for communication

const int ENABLE = 5;       //enable pin, for both DC motors
const int MOTOR_LEFT = 2;   //control pin, for left DC motor
const int MOTOR_RIGHT = 3;  //control pin, for right DC motor

const int trigPin = 7;  //pins for
const int echoPin = 6;  //ultrasonic sensor

Servo myServo;    //a servo object for controlling the servo motor


void setup()
{
  //for using the serial monitor, in debugging phase
  Serial.begin(9600);      

  //some error checking provided by the RadioHead sample program "rf24_server"
  if (!nrf24.init())
    Serial.println("init failed");
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

  //setting the pin modes for pins
  pinMode(ENABLE, OUTPUT);
  pinMode(MOTOR_LEFT, OUTPUT);
  pinMode(MOTOR_RIGHT, OUTPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  //define on which pin is the servomotor attached
  myServo.attach(9);  
}


int speedValue;     //will store
int directionValue; //received
int wheelValue;     //values 

long duration;  //will store values
int distance;   //that will be sent


//calculates the distance measured by the ultrasonic sensor
int calculateDistance()
{
  digitalWrite(trigPin, LOW);     //set trigger low for 2 microseconds
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);    //set trigger high for 10 microseconds
  delayMicroseconds(10);
  
  digitalWrite(trigPin, LOW);     //set trigger back to low
  
  duration = pulseIn(echoPin, HIGH);  //read the echoPin, return the sound wave travel time in microseconds
  distance = duration * 0.034 / 2;    //formula used to calculate distance based on travel time
  
  return distance;
}


/*  The next functions implement the logic of giving the control of DC motors,
 *   the values written are more or less experimental values from testing: by moving
 *   wheels in opposite directions, we can achieve rotations even with fixed wheels
 */
void goForwardLeft()
{
  Serial.println("forwardLeft");
  
  digitalWrite(MOTOR_LEFT, LOW);
  digitalWrite(MOTOR_RIGHT, HIGH);
  
  return;
}

void goForwardStraight()
{
  Serial.println("forwardStraight");

  digitalWrite(MOTOR_LEFT, HIGH);
  digitalWrite(MOTOR_RIGHT, HIGH);
  
  return;
}

void goForwardRight()
{
  Serial.println("forwardRight");

  digitalWrite(MOTOR_LEFT, HIGH);
  digitalWrite(MOTOR_RIGHT, LOW);

  return;
}

void goBackwardsLeft()
{
  Serial.println("backwardLeft");

  digitalWrite(MOTOR_LEFT, HIGH);
  digitalWrite(MOTOR_RIGHT, LOW);

  return;
}

void goBackwardsStraight()
{
  Serial.println("backwardStraight");

  digitalWrite(MOTOR_LEFT, LOW);
  digitalWrite(MOTOR_RIGHT, LOW);

  return;
}

void goBackwardsRight()
{
  Serial.println("backwardRight");

  digitalWrite(MOTOR_LEFT, LOW);
  digitalWrite(MOTOR_RIGHT, HIGH);

  return;
}


/*  Receive data and decompose it
 *    - in different function for better visibility
 *    - dir parameter needed to "trick" radar to work faster
 *      - my guess is that passing by parameter is faster than 
 *        waiting for the whole switch to run
 */
void receiveData(int &dir)
{
  //standard procedure of getting message using RadioHead library  
  if (nrf24.available())
  {
    uint8_t dataGet[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(dataGet);
    if (nrf24.recv(dataGet, &len))
    {
      //if everything goes well, we should reach here
      speedValue = dataGet[0];
      directionValue = dataGet[1];
      wheelValue = dataGet[2];

      dir = directionValue;

      /*
      //print on serial monitor, for debugging purposes
      Serial.print("Speed: ");
      Serial.print(dataGet[0]);
      Serial.print(" Direction: ");
      Serial.println(dataGet[1]);
      Serial.print(" Wheel: ");
      Serial.println(dataGet[2]);
      */

      /*  Control the DC motors
       *  - if speed is too low, we stop the motor - it could not move the car anyways
       *  - rotation needs excess power, so when changing direction from left/right to
       *      straight, we got a big burst, making the car harder to control. For this
       *      reason, we decreased ENABLE (pin which provides the speed of the motors)
       *      by 100 when going straight
       */
      if (speedValue <= 50) 
      {
        analogWrite(ENABLE, 0);
      }
      else
      {
        if (directionValue == 1)//forwards
        {
          switch (wheelValue)
          {
            case 0:
              analogWrite(ENABLE, speedValue);
              goForwardLeft();
              break;
            case 1:
              analogWrite(ENABLE, speedValue - 100);  
              goForwardStraight();
              break;
            case 2:
              analogWrite(ENABLE, speedValue);
              goForwardRight();
              break;
            default:
              Serial.println("Unknown wheel value! (1)");
              break;
          }
        }
        else if (directionValue == 0)//backwards
        {
          switch (wheelValue)
          {
            case 0:
              analogWrite(ENABLE, speedValue);
              goBackwardsLeft();
              break;
            case 1:
              analogWrite(ENABLE, speedValue - 100);
              goBackwardsStraight();
              break;
            case 2:
            analogWrite(ENABLE, speedValue);
              goBackwardsRight();
              break;
            default:
              Serial.println("Unknown wheel value! (2)");
              break;
          }
        }
        else
        {
          Serial.println("Unknown direction value!");
        }   
      }
    }
  }
}


uint8_t dataSend[2];
int dirRec = 1;       //direction received - for storing return value of receiveData

void loop()
{
  receiveData(dirRec);

  //we only move the servo motor ("waving" the ultrasonic sensor around) if we go backwards
  int stopServo = 1;
  if (dirRec == 0)//backwards
    stopServo = 0;

  //rotate the servomotor from 15 to 165 degrees
  for (int i = 60; i <= 160 && stopServo == 0; i++)
  {
    receiveData(dirRec);

    if (dirRec == 1)
    { 
      stopServo = 1;    //if in the meantime the direction 
      break;            // changed, go out of the the loop
    }
    else 
    {
      stopServo = 0;    //continue moving
    }
    
    myServo.write(i);                   //move
    delay(30);                          //wait
    distance = calculateDistance();     //get distance

    dataSend[0] = i;          //put away current degree
    dataSend[1] = distance;   //put away distance value

    //send the message
    nrf24.send(dataSend, sizeof(dataSend));
    nrf24.waitPacketSent();
  }
  
  //same as previous loop, but for degrees 165 to 15
  for (int i = 160; i > 60 && stopServo == 0; i--)
  {
    receiveData(dirRec);

    if (dirRec == 1)
    {
      stopServo = 1;
      break;
    }
    else
    {
      stopServo = 0;
    }
    
    myServo.write(i);
    delay(30);
    distance = calculateDistance();
    
    dataSend[0] = i; 
    dataSend[1] = distance;
     
    nrf24.send(dataSend, sizeof(dataSend));
    nrf24.waitPacketSent();
  }
}
