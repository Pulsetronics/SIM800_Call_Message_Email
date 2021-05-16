#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <SoftwareSerial.h>>
#include "sim800l.h"

#define RESET_PIN     13
#define GAS_SENSE_PIN 12
#define PIR_SENSE_PIN 11
#define BUZZER_PIN    10


SoftwareSerial SIM(2,3);
SIM800_t sim800;

void SIM800_begin (int baudrate) {
     SIM.begin(baudrate);
     Serial.begin(baudrate);
     Serial.println("Program Started");
}

void SIM800_readSerial ( int noOfByte ) {
     int16_t timeout = 0;
     sim800.pointer  = 0;
     memset(sim800.buffer, 0, bufferSize - 1);
     while(sim800.pointer < noOfByte) {
       if (SIM.available() > 0) {
          sim800.buffer[sim800.pointer++] = SIM.read();
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

  // Disbale Echo
  while (strcmp((char*) sim800.buffer, "OK") != 0) {
      memset(sim800.buffer, 0, bufferSize-1);
      SIM.print("ATE0\r\n");
      SIM800_readSerial(2);
  }
  Serial.print("ATE0    -> ");
  Serial.println((const char*)sim800.buffer);

  // wait for the module response
  while (strcmp((char*) sim800.buffer,"OK") != 0 ){
    memset(sim800.buffer, 0, bufferSize-1);
    SIM.print(F("AT\r\n"));
    SIM800_readSerial(2);
  }
  Serial.print("AT      -> ");
  Serial.println((const char*)sim800.buffer);
  
   // Set Text Mode
   while (strcmp((char*) sim800.buffer, "OK") != 0) {
      memset(sim800.buffer, 0, bufferSize-1);
      SIM.print("AT+CMGF=1\r\n");
      SIM800_readSerial(2);
  }

   // Call a number for Demo
   while (strcmp((char*) sim800.buffer, "OK") != 0) {
      memset(sim800.buffer, 0, bufferSize-1);
      SIM.print("ATD+2348029266470;\r\n");
      SIM800_readSerial(2);
  }

  Serial.print("Call No -> ");
  Serial.println((const char*)sim800.buffer);
   
   while(1) {
       SIM800_readSerial(4);
       Serial.println((const char*)sim800.buffer);
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
   //  _buffer=_readSerial();
   //  Response in case of data call, if successfully connected 
   //  if ( (_buffer.indexOf("OK") )!=-1 ) return true;  
   //  else return false;
  return false;
}


bool SIM800_callNumber(char* number){
  memset(sim800.buffer, 0, bufferSize-1);
  SIM.print (F("ATD"));
  SIM.print (number);
  SIM.print(F(";\r\n"));
  SIM800_readSerial(2);

  if(strcmp((char*) sim800.buffer, "OK") == 0) return true;
  else return false;
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


bool SIM800_sendSms(char* number,char* text){

    memset(sim800.buffer, 0, bufferSize-1);
    while (strcmp((char*) sim800.buffer, "OK") != 0) {
      SIM.print("AT+CMGF=1\r\n");
      SIM800_readSerial(2);
    }
    memset(sim800.buffer, 0, bufferSize-1);
    while(strcmp((char*) sim800.buffer, ">") != 0) {
    SIM.print (F("AT+CMGS=\""));  // command to send sms
    SIM.print (number);           
    SIM.print(F("\"\r"));     
    SIM800_readSerial(1);  
    }

    SIM.print (text);
    SIM.print ("\r\n"); 
    SIM.print((char)26);
    SIM800_readSerial(5);

     if(strcmp((char*) sim800.buffer, "+CMGS") == 0) return  true;
     else return false;
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

bool SIM800_GPRSConfig(void) {
     char result = 0;

     /* Set the connection type to GPRS */

     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n"));
     SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;

     /* Set the APN to to “www” since i am using a Vodafone SIM card 
        It might be different for you, and depends on the network */

     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+SAPBR=3,1,\"APN\",\"www\"\r\n"));
     SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;

     /* Enable the GPRS */
     
     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+SAPBR=1,1\r\n"));
     SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;

     /* Query if the connection is setup properly, if we get back a 
        IP address then we can proceed */

     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+SAPBR=2,1\r\n"));
     SIM800_readSerial(6);
     if (strcmp((char*) sim800.buffer, "+SAPBR") == 0) result++;

    if(result == 4) return true;
    else return false;
}

bool SIM800_sendEmail
(    char* subject,
     char* text, 
     char* stmp_address, 
     char* smtp_port, 
     char* s_emailAddress, 
     char* r_emailAddress, 
     char* s_password, 
     char* senderName, 
     char* receiverName
) 
{
     char result = 0;

    /* Start by setting up the Email bearer profile identifier */

     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+EMAILCID=1\r\n"));
     SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;


     /* Set the timeout value within which you will enter the email data to be sent, 
        Here i have set it to 30 seconds */

     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+EMAILTO=30\r\n"));
     SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;


     /* Set the SMTP server address and port number */

     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+SMTPSRV="));
     SIM.print(stmp_address);
     SIM.print(",");
     SIM.print(smtp_port);
     SIM.print(F("\r\n"));
     SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;

 
     /* Set the email authentication information */

     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+SMTPAUTH=1,\""));
     SIM.print(s_emailAddress);
     SIM.print("\",\"");
     SIM.print(s_password);
     SIM.print(F("\"\r\n"));
     SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;
 

    /* Set the senders email address and Name */

     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+SMTPFROM=\""));
     SIM.print(s_emailAddress);
     SIM.print("\",\"");
     SIM.print(senderName);
     SIM.print(F("\"\r\n"));
     SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;

    
    /* Set the To email address and Name */
    
     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+SMTPRCPT=0,0,\""));
     SIM.print(r_emailAddress);
     SIM.print("\",\"");
     SIM.print(receiverName);
     SIM.print(F("\"\r\n"));
     SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;


   /* Set the Cc email address and Name */

     // AT+SMTPRCPT=1,0,”olayiwola.ayinde@eulermotors.com”,”nerdHardware”


   /* Set the Bcc email address and Name */

     // AT+SMTPRCPT=2,0,”microprogrammer@ymail.com”,”Olayiwola”



   /* Set the email Subject, You can set whatever you want. I have set it as MySubject */

     memset(sim800.buffer, 0, bufferSize-1);
     SIM.print(F("AT+SMTPSUB=\""));
     SIM.print(subject);
     SIM.print(F("\"\r\n"));
     SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;

 
   /* Time to enter the email content. Enter the below command and you will get a > prompt 
      after which you should type content and press Ctrl+Z when you are done or send 0x1A 
      hex character.
   */
    memset(sim800.buffer, 0, bufferSize-1);
    SIM.print(F("AT+SMTPBODY\r\n"));
    SIM800_readSerial(1);
    while(strcmp((char*) sim800.buffer, ">") != 0) {
      memset(sim800.buffer, 0, bufferSize-1);
      SIM.print(F("AT+SMTPBODY\r\n"));
      SIM800_readSerial(1);
    }
    memset(sim800.buffer, 0, bufferSize-1);
    SIM.print(text);
    SIM.print((char)26);
    SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;


    /* Send the email by entering the below command, if you get  the URC 
       +SMTPSEND: 1 then email is successfully sent
    */

    memset(sim800.buffer, 0, bufferSize-1);
    SIM.print("AT+SMTPSEND\r\n");
    SIM.print((char)26);
    SIM800_readSerial(2);
     if (strcmp((char*) sim800.buffer, "OK") == 0) result++;
    SIM800_readSerial(12);
     if (strcmp((char*) sim800.buffer, "+SMTPSEND: 1") == 0) result++;

    if(result == 10) return true;
    else return false;
}


    /*********************************************************************************************
     *  If someone is at the door detecting using PIR sensors, the device calls, sends sms and   *
     *  email to the predefined numbers (owner numbers), the owner can then call or send SMS to  *
     *  open the door.                                                                           *                 
     *********************************************************************************************/

void CheckTheDoor(void) {

     if(digitalRead(PIR_SENSE_PIN) == HIGH) {  
          
       /* Send SMS to the Numbers */
       strcpy((char*) sim800.Message, "Person Detected");
       while (SIM800_sendSms((char*) sim800.phoneNumbers[0], (char*) sim800.Message) == false); 
       while (SIM800_sendSms((char*) sim800.phoneNumbers[1], (char*) sim800.Message) == false); 
       while (SIM800_sendSms((char*) sim800.phoneNumbers[2], (char*) sim800.Message) == false); 
      
       /* Call all Numbers */
       while (SIM800_callNumber((char*) sim800.phoneNumbers[0]) == false);
       delay(5000);
       while (SIM800_callNumber((char*) sim800.phoneNumbers[1]) == false);
       delay(5000);
       while (SIM800_callNumber((char*) sim800.phoneNumbers[2]) == false);
       delay(5000);

       /* Send Emails */
       strcpy((char*) sim800.subject,              "MyProject");
       strcpy((char*) sim800.Message,              "Person or Intruder Detected !!!");
       strcpy((char*) sim800.smtpServerAddress,    "smtp.gmail.com");
       strcpy((char*) sim800.smtpServerPort,       "465");
       strcpy((char*) sim800.senderEmailAddress,   "ayindeolayiwola361@gmail.com");
       strcpy((char*) sim800.receiverEmailAddress, "olayiwola_ayinde@yahoo.com");
       strcpy((char*) sim800.senderEmailPassword,  "xxxxxx");
       strcpy((char*) sim800.senderName,           "project Device");
       strcpy((char*) sim800.receiverName,         "myEmail");
       
       while(SIM800_sendEmail((char*) sim800.subject,
                              (char*) sim800.Message,
                              (char*) sim800.smtpServerAddress,
                              (char*) sim800.smtpServerPort,
                              (char*) sim800.senderEmailAddress,
                              (char*) sim800.receiverEmailAddress,
                              (char*) sim800.senderEmailPassword,
                              (char*) sim800.senderName,
                              (char*) sim800.receiverName
                             ) == false);
    }
}


void CheckGasLeakage(void) {

     if(digitalRead(GAS_SENSE_PIN) == HIGH) {  
          
       /* Activate Buzzer */
       digitalWrite(BUZZER_PIN, HIGH);

       /* Send SMS to the Numbers */
       strcpy((char*) sim800.Message, "Gas Leakage Detected");
       while (SIM800_sendSms((char*) sim800.phoneNumbers[0], (char*) sim800.Message) == false); 
       while (SIM800_sendSms((char*) sim800.phoneNumbers[1], (char*) sim800.Message) == false); 
       while (SIM800_sendSms((char*) sim800.phoneNumbers[2], (char*) sim800.Message) == false); 
      
       /* Call all Numbers */
       while (SIM800_callNumber((char*) sim800.phoneNumbers[0]) == false);
       delay(5000);
       while (SIM800_callNumber((char*) sim800.phoneNumbers[1]) == false);
       delay(5000);
       while (SIM800_callNumber((char*) sim800.phoneNumbers[2]) == false);
       delay(5000);

       /* Send Emails */
       strcpy((char*) sim800.subject,              "MyProject");
       strcpy((char*) sim800.Message,              "Gas Leakage Detected !!!");
       strcpy((char*) sim800.smtpServerAddress,    "smtp.gmail.com");
       strcpy((char*) sim800.smtpServerPort,       "465");
       strcpy((char*) sim800.senderEmailAddress,   "ayindeolayiwola361@gmail.com");
       strcpy((char*) sim800.receiverEmailAddress, "olayiwola_ayinde@yahoo.com");
       strcpy((char*) sim800.senderEmailPassword,  "xxxxxx");
       strcpy((char*) sim800.senderName,           "project Device");
       strcpy((char*) sim800.receiverName,         "myEmail");
       
       while(SIM800_sendEmail((char*) sim800.subject,
                              (char*) sim800.Message,
                              (char*) sim800.smtpServerAddress,
                              (char*) sim800.smtpServerPort,
                              (char*) sim800.senderEmailAddress,
                              (char*) sim800.receiverEmailAddress,
                              (char*) sim800.senderEmailPassword,
                              (char*) sim800.senderName,
                              (char*) sim800.receiverName
                            ) == false);
      digitalWrite(BUZZER_PIN, LOW);
    }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(RESET_PIN, 1);      // Output
  pinMode(PIR_SENSE_PIN, 0);  //Input
  pinMode(GAS_SENSE_PIN, 0);  //Input
  pinMode(BUZZER_PIN, 1);     //Input
  
  SIM800_begin(9600); 
  delay(5000);

  // configure GSM
  SIM800_reset();
  while(SIM800_GPRSConfig() == false) SIM.print("Configuring...");

  //Sample Phone Numbers
  strcpy((char*) sim800.phoneNumbers[0], "+918447819324");
  strcpy((char*) sim800.phoneNumbers[1], "+919319643241");
  strcpy((char*) sim800.phoneNumbers[2], "+919873113817");


}

void loop() {
        CheckTheDoor();
        CheckGasLeakage(); 
  }
