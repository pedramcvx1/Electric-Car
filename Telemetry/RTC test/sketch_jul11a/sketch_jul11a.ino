#include <virtuabotixRTC.h>   // Real-Time Clock Library


// For RTC
int lastRead = 0;

// Creation of the Real Time Clock Object
//SCLK -> 6 
//DATA -> 7 
//RST -> 8

virtuabotixRTC myRTC(6, 7, 8);

void setup() {
  Serial.begin(9600); 
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  myRTC.setDS1302Time(00, 24, 11, 2, 22, 8, 2017);

}

void loop() 
{
  myRTC.updateTime();
  if (myRTC.seconds != lastRead) // Each sec save the data into the file
  {
     Serial.print("Şuanki Tarih / Saat : ");
    Serial.print(myRTC.dayofmonth);
    Serial.print("/");
    Serial.print(myRTC.month);
    Serial.print("/");
    Serial.print(myRTC.year);
    Serial.print(" ");
    Serial.print(myRTC.hours);
    Serial.print(":");
    Serial.print(myRTC.minutes);
    Serial.print(":");
    Serial.println(myRTC.seconds);  
}
  lastRead = myRTC.seconds;
  dataString = "";
}


