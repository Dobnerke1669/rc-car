#include <SPI.h>
#include <RH_NRF24.h>

// Singleton instance of the radio driver
RH_NRF24 nrf24;
int ENALEFT = 6;
int MOTORLEFT1 = 2;
int MOTORLEFT2 = 3;


int ENARIGHT = 5;
int MOTORRIGHT1 = 4;
int MOTORRIGHT2 = 7;

int SPEED = 255;
void setup() 
{
  Serial.begin(9600);
  pinMode(MOTORLEFT1,OUTPUT);
  pinMode(MOTORLEFT2,OUTPUT);
  pinMode(ENALEFT,OUTPUT);
  pinMode(ENARIGHT,OUTPUT);
  pinMode(MOTORRIGHT1,OUTPUT);
  pinMode(MOTORRIGHT2,OUTPUT);
  
  
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    
}

void loop()
{
  if (nrf24.available())
  {
      // Should be a message for us now   
      uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (nrf24.recv(buf, &len))
      {

        // console log
        Serial.print("got request: SPEED | DIRECTION | STEERING : "); Serial.print(buf[0]); Serial.print("  |  "); Serial.print(buf[1]); Serial.print("  |  "); Serial.print(buf[2]); Serial.println("  |  ");
        
        // straight mode
        if(buf[2] == 1)
        {
          // direction control
          if(buf[1] == 0)
          {
            digitalWrite(MOTORLEFT1,LOW);
            digitalWrite(MOTORLEFT2,HIGH);
  
            digitalWrite(MOTORRIGHT1,LOW);
            digitalWrite(MOTORRIGHT2,HIGH);

             Serial.println("@");
          }else 
          {
             digitalWrite(MOTORLEFT1,HIGH);
             digitalWrite(MOTORLEFT2,LOW);
  
             digitalWrite(MOTORRIGHT1,HIGH);
             digitalWrite(MOTORRIGHT2,LOW);
            
            
          }
           // if you go forward-left then its the same as going back and right and vice-versa
        }else if((buf[1] == 1 && buf[2] == 0)|| (buf[1] ==0 && buf[2] == 2)){
            digitalWrite(MOTORLEFT1,HIGH);
            digitalWrite(MOTORLEFT2,LOW);

            digitalWrite(MOTORRIGHT1,LOW);
            digitalWrite(MOTORRIGHT2,HIGH);
        }
        else if ((buf[1] == 0 && buf[2] == 2) || (buf[1] == 1 && buf[2] == 0))
        {
       
              digitalWrite(MOTORLEFT1,LOW);
              digitalWrite(MOTORLEFT2,HIGH);

              digitalWrite(MOTORRIGHT1,HIGH);
              digitalWrite(MOTORRIGHT2,LOW);
        }


        
        // if the motor needs to operate on low speed it better stops
        if(buf[0] <= 50)
        {
         
          digitalWrite(MOTORLEFT1,LOW);
          digitalWrite(MOTORLEFT2,LOW);

          digitalWrite(MOTORRIGHT1,LOW);
          digitalWrite(MOTORRIGHT2,LOW);
        }

// buf[0] speed
//         *
// buf[1] 1-forward, 0-reverse
//           *             
// buf[2] 0-left,1-straight,2-right
//                 *           *  
        
        analogWrite(ENALEFT,buf[0]);
        analogWrite(ENARIGHT,buf[0]);
        
      }
    else
    {
      Serial.println("recv failed");
    }
  }else{
    //Serial.println(".");
  }

}
