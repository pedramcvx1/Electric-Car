
#include <genieArduino.h>     //Library for the 4D LCD
#include <SPI.h>              // For SD Card, uses SPI comm
#include <SD.h>               // SD Card Library
#include <virtuabotixRTC.h>   // Real-Time Clock Library
#define RESETLINE 2  //reset pin for LCD screen to sync w/ arduino
Genie genie;
#define autonomousStop     3
#define autonomousStart    4

int autonomousStarted = 0;
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

// this definitions for recvWithEndMarker() and showNewData()
boolean newData = false;
String comingData = "";

//  For data Logger
String dataString = "";

// For RTC
int lastRead = 0;

/* analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 51
 ** MISO - pin 50
 ** CLK - pin 52
 ** CS - pin 53
*/

// Creation of the Real Time Clock Object
//SCLK -> 6 
//DATA -> 7 
//RST -> 8

virtuabotixRTC myRTC(6, 7, 8);
File dataFile;

void setup() {
  Serial.begin(9600); 
  

  Serial2.begin(9600);  // Serial 2 9600 baud rate
  genie.Begin(Serial2);   // Use Serial2 for talking to the Genie Library, and to the 4D Systems display
  
  pinMode(autonomousStart,INPUT_PULLUP);
  pinMode(autonomousStop,INPUT_PULLUP);
  pinMode(RESETLINE, OUTPUT);  // Set D4 on Arduino to Output
  digitalWrite(RESETLINE, 0);  // Reset the Display via D4
  delay(100);
  digitalWrite(RESETLINE, 1);  // unReset the Display via D4
  delay (3500); //let the display start up after the reset (This is important)
  genie.WriteContrast(15);// contrast 0-15,

  Serial1.begin(115200); // RF 
 

  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(00, 55, 2, 6, 15, 7, 2018);

    // see if the card is present and can be initialized:
 pinMode(53,OUTPUT);

 if (!SD.begin(53)) {
     Serial.println("initialization failed!");
     return;
   }
  Serial.println("card initialized.");
  genie.WriteObject(GENIE_OBJ_USER_LED, 0, 0);
}

void loop() {
 recvWithEndMarker(); // REceive string through serial port 0 using RF
 showNewData();      // Show the data on the Screen
 dataLogger(); // Save the data to SD card every Sec. 
delay(2);
}


void recvWithEndMarker() {


  while (Serial1.available()) {
          comingData  = Serial1.readStringUntil('\r\n');
            Serial.println(comingData);
              if ((comingData.charAt(0) == 'V')) 
                {
                newData = true;
                // Led indicator for RF data
                genie.WriteObject(GENIE_OBJ_USER_LED, 1, 1);
                }
    
  }
  genie.WriteObject(GENIE_OBJ_USER_LED, 1, 0);
}


void showNewData() {
  myRTC.updateTime(); // This allows for the update of variables for time or accessing the individual elements.

 
  /* Indicators for String */
// V ---> Voltage
// A ---> Current
// S ---> Speed
// T ---> Temperature
// X ---> Reserved 

  if (newData == true) {      // If there is incoming data, then do the process.
     String temp;                // temporary string
    unsigned int integerTemp;   // temporary integer

    int indexOfVoltage = comingData.indexOf('V');
    int indexOfCurrent = comingData.indexOf('A');
    int indexOfSpeed = comingData.indexOf('S');
    int indexOfTemperature = comingData.indexOf('T');
    int indexOfAutonomous = comingData.indexOf('L');
    int indexOfEndMarker = comingData.indexOf('E');

    
    // Get the voltage value from the received string.
    // Arrange and send it to the screen
    temp  = comingData.substring(indexOfVoltage+1,indexOfCurrent ) ;
 //   Serial.println(temp);
    dataString = comingData.substring(indexOfVoltage,indexOfCurrent) + String(",") ;
    integerTemp = temp.toInt();
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 1, integerTemp);

    //Get the current value from the received string.
    //Arrange and send it to the screen
    temp = comingData.substring(indexOfCurrent+1, indexOfSpeed);
 //   Serial.println(temp);
    dataString += comingData.substring(indexOfCurrent, indexOfSpeed) + String(",");
    integerTemp = temp.toInt();
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 2, integerTemp);


    //Get the speed value from the received string.
    //Arrange and send it to the screen
    temp = comingData.substring(indexOfSpeed+1, indexOfTemperature);
  //  Serial.println(temp);
    dataString += comingData.substring(indexOfSpeed, indexOfTemperature) + String(",");
    integerTemp = temp.toInt();
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, integerTemp);


    //Get the temperature value from the received string.
    //Arrange and send it to the screen
    temp = comingData.substring(indexOfTemperature+1, indexOfAutonomous);
 //   Serial.println(temp);
    integerTemp = temp.toInt();
    dataString += comingData.substring(indexOfTemperature, indexOfAutonomous) + String(",");
    genie.WriteObject(GENIE_OBJ_THERMOMETER, 0, integerTemp);


    //Get the energy value from the received string.
    //Arrange and send it to the screen
    temp = comingData.substring(indexOfAutonomous+1, indexOfEndMarker);
    Serial.println(temp);
    integerTemp = temp.toInt();
    dataString += comingData.substring(indexOfAutonomous, indexOfEndMarker) + String(",");
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 3, integerTemp);

    
    dataString += myRTC.year + String("/") + myRTC.month + String("/") +  myRTC.dayofmonth + String(",");
    dataString += myRTC.hours + String(":") + myRTC.minutes + String(":") + myRTC.seconds;
    newData = false;
  }
}

void dataLogger()
{
  if (myRTC.seconds != lastRead) // Each sec save the data into the file
  {
  //Serial.println(dataString);
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
    }
  }
  lastRead = myRTC.seconds;
  dataString = "";

}


