#include <DS3231_Simple.h>    //Real Time Clock(RTC) library
#include <Dusk2Dawn.h>        //Library to lookup sunrise sunset times of a location by its geographical co-ordinate
#include "cie1931.h"          //Lookup table for PWM Duty Cycles for linear brightness control of Sunrise LED lights

DS3231_Simple Clock;
float latitude = 0.0000000;           // Replace your co-ordinates and timezone
float longitude = 0.000000;
float timezone = 0.0;

Dusk2Dawn MyCity(latitude, longitude, timezone);     // Location and time-zone.
DateTime MyDateAndTime;                              // Create a variable to hold date-time data

void setup() 

   {
      Serial.begin(9600);
      Clock.begin();

      Serial.println(); 
      pinMode(8, OUTPUT);     //UPS relay control pin.
      pinMode(6, OUTPUT);     //Sunrise Alarm PWM Pin, Frequency 1kHz.  
   }

  

void loop() 
{  
   long Now,j,alarm_start, alarm_stop;     //Now(Time in Minutes from midnight)
   double Now_In_Seconds,i,x;              //Now_In_Seconds(Time in seconds from midnight)
   int Year,Month,Day;
   int Sunrise;                                            
   int Sunset;
  
   while(1)
   {
      MyDateAndTime = Clock.read();     // Ask the DS3231 RTC clock module for the data.
      Day=MyDateAndTime.Day;                               
      Month=MyDateAndTime.Month;
      Year=2000+MyDateAndTime.Year;
      Sunrise=MyCity.sunrise(Year, Month, Day, false);    //Today Sunrise in Minutes from Midnight
      Sunset=MyCity.sunset(Year, Month, Day, false);      //Today Sunset in Minutes from Midnight


      Now=((MyDateAndTime.Hour*60)+MyDateAndTime.Minute);      //Now(Time in Minutes from midnight)
      Now_In_Seconds=((Now*60)+MyDateAndTime.Second);          //Now_In_Seconds(Time in seconds from midnight)
    

      // Displays some data over serial monitor in Arduino IDE to ensure proper functioning
      Serial.print("Today Sunrise: "); Serial.println(Sunrise);  
      Serial.print("Today Sunset: ");  Serial.println(Sunset);
      Serial.print("Now TIme(In Minutes): "); Serial.println(Now);
      Serial.print("Now TIme(In Seconds): "); Serial.println(Now_In_Seconds);


      if(Now<=(Sunset-75)&&Now>=(Sunrise-105))     //Inverter Control.
         digitalWrite(8, HIGH); 
      else
         digitalWrite(8, LOW);

      alarm_start=Sunrise-90;       //Alarm Start time.
      alarm_stop=alarm_start+60;    //1hr alarm.
     
      x=alarm_start-alarm_stop;
      
      if(Now>=alarm_start&&Now<=alarm_stop)
         i=(((-255*Now_In_Seconds)/60)+(255*alarm_start))/x;   //Equation gives i=0 at alarm start and i=255 at alarm stop.
      else
         i=0;                                                     

      j=round(i);
      
      analogWrite(6,(255-cie[j]));     //Inverted Output to get correct pin state for circuit.(Led Are Lit When PWM Output is Low and Vice-versa)
                                       //0 means 0% Duty cycle and 255 is 100% Duty Cycle)

      delay(1000);                     // 1 second delay
   }
}
