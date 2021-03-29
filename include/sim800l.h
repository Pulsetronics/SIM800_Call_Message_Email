
#ifndef __SIM800_H
#define __SIM800_H

#include "Arduino.h"
  
#define bufferSize   200

typedef struct SIM800_t {
       int      pointer;
       int      numbersOfReceivedByte;
       uint8_t  buffer[bufferSize];
       uint8_t  phoneNumbers[3][15];
       uint8_t  Message[bufferSize];
       uint8_t  receiverEmailAddress[50];
       uint8_t  senderEmailAddress[50];
       uint8_t  senderEmailPassword[32];
       uint8_t  senderName[32];
       uint8_t  receiverName[32];
       uint8_t  smtpServerAddress[50];
       uint8_t  smtpServerPort[10];
       uint8_t  subject[50];


}SIM800_t;

void    SIM800_begin(int baudrate);
void    SIM800_readSerial ( int noOfByte );
void    SIM800_reset(void); 

bool    SIM800_answerCall(void); 	
bool    SIM800_callNumber(char* number);
bool    SIM800_hangoffCall(void);
uint8_t SIM800_getCallStatus(void);   

bool   SIM800_sendSms(char* number,char* text);	 
void   SIM800_readSms(uint8_t index);       //return all the content of sms
void   SIM800_getNumberSms(uint8_t index);  //return the number of the sms..   
bool   SIM800_delAllSms(void);              //return :  OK or ERROR .. 

void SIM800_signalQuality(void);
void SIM800_setPhoneFunctionality(void);
void SIM800_activateBearerProfile(void);
void SIM800_deactivateBearerProfile(void);
bool SIM800_GPRSConfig(void);
bool SIM800_sendEmail( char* subject, char* text, char* stmp_address, char* smtp_port, char* s_emailAddress, char* r_emailAddress, char* s_password, char* senderName, char* receiverName); 
void CheckGasLeakage(void);
#endif 