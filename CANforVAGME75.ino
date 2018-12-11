#include <mcp_can.h>
#include <SPI.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);         

byte buf[2];
unsigned int RPM = 0;
unsigned int RPMfordash = 0;
byte RPMtimer = 0;
byte canSendTimer = 0;
byte canSendTimer2 = 1;
byte canSendTimer3 = 2;
byte loop25 = 0;
unsigned long genericTimer = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
  {
      Serial.println("CAN BUS Shield init fail");
      Serial.println(" Init CAN BUS Shield again");
      delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
  delay(200);
  unsigned int danceRPM = 8000;
  danceRPM = danceRPM * 4;
  byte rpmLo = lowByte(danceRPM);
  byte rpmHi = highByte(danceRPM);
  //Serial.println(RPMfordash);
  byte stmp[8] = {0x09, 0x22, rpmLo, rpmHi, 0x20, 0x00, 0x22, 0x22};
  for(unsigned long i = millis(); (millis() - i) < 1500; i = i){
    if((millis() % 10) == 0){CAN.sendMsgBuf(0x280, 0, 8, stmp);}
  }
  stmp[2] = 0x00;
  stmp[3] = 0x00;
  CAN.sendMsgBuf(0x280, 0, 8, stmp);
  for(unsigned long i = millis(); (millis() - i) < 1000; i = i){
    if(Serial.available()){ Serial.read();}
  }
}

void loop() {
  // put your main code here, to run repeatedly:
    if(RPMtimer == 50){
      Serial.print('%');
      if (Serial.available()){
        byte packetSize = Serial.available();
        for (int i = 0; i < 2; i++){
          buf[i] = Serial.read();
        }
        for(int i = 0; i < 2; i++){
          Serial.print(i);Serial.print(":");
          Serial.println(buf[i]);
        }
      }
      else{
        Serial.println("No recieve!");
      }
      Serial.println(word(buf[0], buf[1]));
      RPMtimer = 0;
    }
   
    if (canSendTimer == 10){
      RPM = word(buf[0], buf[1]);
      RPMfordash = RPM*4;
      byte rpmLo = lowByte(RPMfordash);
      byte rpmHi = highByte(RPMfordash);
      //Serial.println(RPMfordash);
      byte stmp[8] = {0x09, 0x22, rpmLo, rpmHi, 0x20, 0x00, 0x22, 0x22};
      CAN.sendMsgBuf(0x280, 0, 8, stmp);
      
      canSendTimer = 0;
  }
  if (canSendTimer2 == 10){
    CAN.sendMsgBuf(0x3D0, 0, 8, (0, 0x80, 0, 0, 0, 0, 0, 0));
    CAN.sendMsgBuf(0xDA0, 0, 8, (0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00));
    CAN.sendMsgBuf(0x5A0, 0, 8, (0xFF, 0x1A, 0x00, 0x04, 0x00, 0x05, 0x06, 0xad));
    canSendTimer2 = 0;
  }
  if (canSendTimer3 == 10){
    CAN.sendMsgBuf(0x1A0, 0, 8, (0x18, 0x00, 0x15, 0x00, 0xfe, 0xfe, 0x00, 0xff));
    CAN.sendMsgBuf(0x050, 0, 8, (0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00));
    canSendTimer3 = 0;
  }
  if (loop25 = 25){
      CAN.sendMsgBuf(0x320, 0, 8, (0x00, 0x2A, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00));
  }
  TimerFunction();
}

void TimerFunction(){
  if(millis()- genericTimer != 0){
    canSendTimer++;
    RPMtimer++;
    loop25++;
    genericTimer = millis();
  }
  
}

