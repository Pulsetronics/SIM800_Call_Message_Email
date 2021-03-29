### Case 1. 
If someone is at the door, it should call, send sms and email to the predefined numbers (owner numbers)

The owner then call or send SMS to open the door.

#### Note: 

* if the number that's calling or sent sms is part of the predefined numbers. SMS showing "CORRECT NUMBER CALLING" or correct number sent message" depending in whether call or sms should be sent to the numbers and same message should show on serial monitor and the door should open.

* if the number that's calling or that sent sms is not part of the predefined numbers. SMS showing "WRONG NUMBER CALLING" or "wrong number sent message" should be sent to the numbers depending on whether call or sms and same message should show on serial monitor and the door should not open.

### Case 2:
If there is a gas leakage, 
* A buzzer should be activated
* call the predefined number.
* Email showing gas leakage should be sent.
* SMS showing "GAS Leakage" sent to the predefined number, 1st, 2nd and 3rd time. 


### Note:

If any of the predefined numbers made call or sent SMS to open the door after the 1st SMS (counter =1). Then the door should open and no further action is required.

If any of the predefined numbers did not make a call or send message to open after the 1st SMS is sent, another message should be sent (counter=2)

Same should happen until counter =3.

If counter=3 and no call or SMS is sent to open the door, the door should open by itself.