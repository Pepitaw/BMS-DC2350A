// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13


#include <SPI.h>
#include "mcp_can.h"
#include <Timer.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

unsigned char buff[8];
unsigned char msg[8];
unsigned long canId;
unsigned int voltage[4];
unsigned int battery[18];
unsigned int data[6];
unsigned char len = 0;
int p=0;
Timer request;

void setup()
{
    Serial.begin(115200);
    //request.every(5000,request_msg);

START_INIT:

    if(CAN_OK == CAN.begin(CAN_500KBPS,2))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}

void request_msg(){
  CAN.sendMsgBuf(8, 0, 8, msg);
  if(canId==9){
          if(buff[0]==1){
            Serial.println(F("Over Voltage > 3.73 !"));
          }
          if(buff[1]==1){
            Serial.println(F("Under Voltage < 2.5 !"));
          }
          if(buff[2]==1){
            Serial.println(F("Over Average Voltage !"));
          }
          if(buff[3]==1){
            Serial.println(F("Under Average Voltage !"));
          }
          if(buff[4]==1){
            Serial.println(F("Over maximum Voltage difference !"));
          }
   }
   for (int i = 0; i < 18; i++)
        {
          if (battery[i]*0.0001 > 3.73)
          {
            Serial.print("IC");
            Serial.print(buff[6]);
            Serial.print(" -> ");
            Serial.print(i+1);
            Serial.print("  ");
            Serial.println(F("Over Voltage > 3.73 !"));
          }
          if (battery[i]*0.0001 < 2.5)
          {
            Serial.print("IC");
            Serial.print(buff[6]);
            Serial.print(" -> ");
            Serial.print(i+1);
            Serial.print("  ");
            Serial.println(F("Under Voltage < 2.5 !"));
          }
        }
}
void loop()
{
    request.update();
    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buff);    // read data,  len: data length, buf: data buf
        unsigned long canId = CAN.getCanId();
        
        /*Serial.println("-----------------------------");
        Serial.print("get data from ID: ");
        Serial.println(canId, HEX);

        for(int i = 0; i<len; i++)    // print the data
        {
            Serial.print(buff[i]);
            Serial.print("\t");
        }
        Serial.println();*/

        
        for(int i=0;i<3;i++){
          voltage[i]=(buff[2*i+1]<< 8)+buff[i*2];
          /*Serial.print(voltage[i]);
          Serial.print("  ");*/
        }
        //Serial.println(voltage[0],BIN);

        switch(canId){
          case 0 :
            for(int i=0;i<3;i++){
              battery[i]=voltage[i];
            }
            break;
          case 1 :
            for(int i=3;i<6;i++){
              battery[i]=voltage[i-3];
            }
            break;
          case 2 :
            for(int i=6;i<9;i++){
              battery[i]=voltage[i-6];
            }
            break;
          case 3 :
            for(int i=9;i<12;i++){
              battery[i]=voltage[i-9];
            }
            break;
          case 4 :
            for(int i=12;i<15;i++){
              battery[i]=voltage[i-12];
            }
            break;
          case 5 :
            for(int i=15;i<18;i++){
              battery[i]=voltage[i-15];
            }
            break;
          case 6 :
            for(int i=0;i<3;i++){
              data[i]=voltage[i];
            }
            break;
          case 7 :
            for(int i=0;i<3;i++){
              data[i]=voltage[i];
            }
            break;
          default :
            break;
        }
        if(canId==6){
          Serial.print(F("Sum voltage of IC "));
          Serial.print(buff[6]);
          Serial.print(F(" : "));
          Serial.print(data[0]*0.001,4);
          Serial.print(F("   "));
          Serial.print(F("Average voltage of IC "));
          Serial.print(buff[6]);
          Serial.print(F(" : "));
          Serial.println(data[1]*0.0001,4);
          Serial.print(F("MAX difference Voltage_"));
          Serial.print(F("IC "));
          Serial.print(buff[6]);
          Serial.print(F(" : "));
          Serial.print(data[2]*0.0001, 4);
          Serial.print(F("   "));
        }
        if(canId==7){
          Serial.print(F("Highest Voltage:"));
          Serial.print(data[0]*0.0001, 4);
          Serial.print(F("   "));
          Serial.print(F("Lowest Voltage:"));
          Serial.println(data[1]*0.0001, 4);
        }
        if(canId==5){
          Serial.print("IC");
          Serial.print(buff[6]);
          Serial.print(" -> ");
          for(int i=0;i<18;i++){
            Serial.print("C");
            Serial.print(i+1);
            Serial.print(": ");
            Serial.print(battery[i]*0.0001,4);
            Serial.print(", ");
          }
          Serial.println();
        }
        /*for(int i=0;i<6;i++){
          Serial.print(data[i]);
          Serial.print(" ");
        }
        Serial.println();*/
    }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
