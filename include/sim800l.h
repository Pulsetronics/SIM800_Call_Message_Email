
#ifndef __SIM800_H
#define __SIM800_H

#include "Arduino.h"
  
#define bufferSize   200

typedef struct SIM800_t {
       int      pointer;
       int      numbersOfReceivedByte;
       uint8_t  buffer[bufferSize];
       uint8_t  phoneNumbers[3][15];
       uint8_t  textMessage[bufferSize];

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


#endif 