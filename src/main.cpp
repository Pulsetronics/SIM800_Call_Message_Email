#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "sim800l.h"

#define SIM           Serial
#define RESET_PIN     13
#define GAS_SENSE_PIN 12
#define PIR_SENSE_PIN 11



SIM800_t sim800;

void SIM800_begin (int baudrate) {
     SIM.begin(baudrate);
}

void SIM800_readSerial ( int noOfByte ) {
     int16_t timeout = 0;
     sim800.pointer  = 0;
     memset(sim800.buffer, 0, 199);
     
     while(sim800.pointer < noOfByte) {
       if (Serial.available() > 0) {
          sim800.buffer[sim800.pointer++] = Serial.read();
       }
       if(timeout++ > 1000) break;
       delay(1);
     }
}
    
void SIM800_reset(void) {

  digitalWrite(RESET_PIN,1);
  delay(1000);
  digitalWrite(RESET_PIN,0);
  delay(1000);

  // wait for the module response
  memset(sim800.buffer, 0, 199);
  while (strcmp((char*) sim800.buffer,"OK") != 0 ){
    SIM.print(F("AT\r\n"));
    SIM800_readSerial(2);
  }
  
  //wait for sms ready
  while (strcmp((char*) sim800.buffer, "SMS") != 0) {
      SIM800_readSerial(2);
  }
 
}

void SIM800_setPhoneFunctionality(){
  /*AT+CFUN=<fun>[,<rst>] 
  Parameters
  <fun> 0 Minimum functionality
  1 Full functionality (Default)
  4 Disable phone both transmit and receive RF circuits.
  <rst> 1 Reset the MT before setting it to <fun> power level.
  */
  SIM.print (F("AT+CFUN=1\r\n"));
}


void SIM800_signalQuality(){
/*Response
+CSQ: <rssi>,<ber>Parameters
<rssi>
0 -115 dBm or less
1 -111 dBm
2...30 -110... -54 dBm
31 -52 dBm or greater
99 not known or not detectable
<ber> (in percent):
0...7 As RXQUAL values in the table in GSM 05.08 [20]
subclause 7.2.4
99 Not known or not detectable 
*/
  SIM.print (F("AT+CSQ\r\n"));
  // Serial.println(sim800.buffer);
}


void SIM800_activateBearerProfile(){
  // SIM.print (F(" AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" \r\n" ));_buffer=_readSerial();  // set bearer parameter 
  // SIM.print (F(" AT+SAPBR=3,1,\"APN\",\"internet\" \r\n" ));_buffer=_readSerial(); // set apn  
  // SIM.print (F(" AT+SAPBR=1,1 \r\n"));delay(1200);_buffer=_readSerial();// activate bearer context
  // SIM.print (F(" AT+SAPBR=2,1\r\n "));delay(3000);_buffer=_readSerial(); // get context ip address
}


void SIM800_deactivateBearerProfile(){
  SIM.print (F("AT+SAPBR=0,1\r\n "));
  delay(1500);
}



bool SIM800_answerCall(){
   SIM.print (F("ATA\r\n"));
   // _buffer=_readSerial();
   //Response in case of data call, if successfully connected 
  //  if ( (_buffer.indexOf("OK") )!=-1 ) return true;  
  //  else return false;
  return false;
}


void  SIM800_callNumber(char* number){
  SIM.print (F("ATD"));
  SIM.print (number);
  SIM.print(F(";\r\n"));
}



uint8_t SIM800_getCallStatus(){
/*
  values of return:
 
 0 Ready (MT allows commands from TA/TE)
 2 Unknown (MT is not guaranteed to respond to tructions)
 3 Ringing (MT is ready for commands from TA/TE, but the ringer is active)
 4 Call in progress
*/
  SIM.print (F("AT+CPAS\r\n"));
  //_buffer=_readSerial();  
  // return _buffer.substring(_buffer.indexOf("+CPAS: ")+7,_buffer.indexOf("+CPAS: ")+9).toInt();
   return 0;
}



bool SIM800_hangoffCall(){
  SIM.print (F("ATH\r\n"));
  // _buffer=_readSerial();
  //if ( (_buffer.indexOf("OK") ) != -1) return true;
  // else return false;
  return false;
}






bool SSIM800_sendSms(char* number,char* text){

    SIM.print (F("AT+CMGF=1\r")); //set sms to text mode  
    // _buffer=_readSerial();
    SIM.print (F("AT+CMGS=\""));  // command to send sms
    SIM.print (number);           
    SIM.print(F("\"\r"));       
    // _buffer=_readSerial(); 
    SIM.print (text);
    SIM.print ("\r"); 
	//change delay 100 to readserial	
    // _buffer=_readSerial();
    SIM.print((char)26);
    // _buffer=_readSerial();
    //expect CMGS:xxx   , where xxx is a number,for the sending sms.
    // if (((_buffer.indexOf("CMGS") ) != -1 ) ){
    //   return true;
    // }
    // else {
    //   return false;
    // }
    return false;
}


void SIM800_getNumberSms(uint8_t index){
  // _buffer=readSms(index);
  // Serial.println(_buffer.length());
  // if (_buffer.length() > 10) //avoid empty sms
  // {
  //   uint8_t _idx1=_buffer.indexOf("+CMGR:");
  //   _idx1=_buffer.indexOf("\",\"",_idx1+1);
  //   return _buffer.substring(_idx1+3,_buffer.indexOf("\",\"",_idx1+4));
  // }else{
  //   return "";
  // }

}



void SIM800_readSms(uint8_t index){
  SIM.print (F("AT+CMGF=1\r")); 
  // if (( _readSerial().indexOf("ER")) ==-1) {
  //   SIM.print (F("AT+CMGR="));
  //   SIM.print (index);
  //   SIM.print("\r");
  //   _buffer=_readSerial();
  //   if (_buffer.indexOf("CMGR:")!=-1){
  //     return _buffer;
  //   }
  //   else return "";    
  //   }
  // else
  //   return "";
}


bool SIM800_delAllSms(){ 
  // SIM.print(F("at+cmgda=\"del all\"\n\r"));
  // _buffer=_readSerial();
  // if (_buffer.indexOf("OK")!=-1) {return true;}else {return false;}
  return false;
}


    /*********************************************************************************************
     *  If someone is at the door detecting using PIR sensors, the device calls, sends sms and   *
     *  email to the predefined numbers (owner numbers), the owner can then call or send SMS to  *
     *  open the door.                                                                           *                 
     *********************************************************************************************/

void CheckTheDoor(void) {

     if(digitalRead(PIR_SENSE_PIN) == HIGH) {  
          
          // Send SMS to the Numbers

          
    }
}
`
void setup() {
  // put your setup code here, to run once:
  pinMode(RESET_PIN, 1);      // Output
  pinMode(PIR_SENSE_PIN, 0);  //Input
  pinMode(GAS_SENSE_PIN, 0);  //Input

  // configure GSM
  SIM.print("AT+CMGF=1\r\n");
  //Sample Phone Numbers
  strcpy((char*) sim800.phoneNumbers[0], "+918447819324");
  strcpy((char*) sim800.phoneNumbers[1], "+919319643241");
  strcpy((char*) sim800.phoneNumbers[2], "+919873113817");

  SIM800_begin(9600); 

}

void loop() {
        CheckTheDoor();



  }
