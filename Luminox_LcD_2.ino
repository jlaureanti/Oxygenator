/*
Receives from software serial port, sends to hardware serial port.
 * RX is digital pin 10 (connect to TX of level shifter: pin B1)
 * TX is digital pin 11 (connect to RX of level shifter: pin B2)
The code was written for Arduino Uno Rev. 3 and Arduino IDE ver. 1.6.1. 
*/

#include <SoftwareSerial.h>                  //incorporate software-serial library
#include <LiquidCrystal.h>

SoftwareSerial myserial(9, 10);            //enable software serial port to RX,TX
LiquidCrystal lcd(7,8,2,4,12,13);


long delayPeriod;     //introduce “delayPeriod”
String Luminoxstring = "";                  //string to hold incoming data from Luminox-O2 sensor
boolean Luminox_stringcomplete = false;    //were all data from Luminox-O2 sensor received? Check
float LuminoxFloat;
float LuminoxPPM;

void setup()
{                                                                        
  Serial.begin(9600);                           //set baud rate for Arduino serial port to 9600
  myserial.begin(9600);                         //set baud rate for software serial port to 9600
  Luminoxstring.reserve(41);                 //set aside 41 bytes for receiving data from Luminox-O2 sensor
  Serial.print("I'm Here");

  lcd.begin(16,2);
  lcd.print("I'm awake");
}

void serialEvent()
{
   Serial.print("Still here");
   String inchar = "";
   while (Serial.available() > 0) {             //while a char is held in software serial buffer
      inchar += (char)Serial.read();            //grab that char
      if (inchar == "s")
      delayPeriod = 10000;     //if the char is m, set delay to 5 minutes – change this if necessary
      if (inchar == "m")
      delayPeriod = 5000;     //otherwise, delay is set to 1 second – change this if necessary
      if (inchar == "f")
      delayPeriod = 1000;     //otherwise, delay is set to 1 second – change this if necessary  
   }
}

void loop()
{
  {                                                //start of loop sequence             
    while (myserial.available()) {              //while a char is held in software serial buffer
      char inchar = (char)myserial.read();      //grab that char
      Luminoxstring += inchar;                  //add the received char to LuminoxString
      if (inchar == '\r') {                                   //if the incoming character is a <term>, reset
        Luminox_stringcomplete = true;          
      }           
    }
    if (Luminox_stringcomplete){                //was a complete string received from the Luminox sensor? 
      Luminoxstring.remove(41);                 //remove any serial string overruns 
      Serial.print(Luminoxstring);              //use the Arduino serial port to send that data to CoolTerm
      Luminoxstring.remove(0, 27);
      Luminoxstring.remove(6);
      Serial.println(Luminoxstring);
      
      lcd.begin(16, 2);
      lcd.setCursor(0,0);
      lcd.print(Luminoxstring); lcd.print(" % O2");

      LuminoxFloat = Luminoxstring.toFloat();
      LuminoxPPM = ((LuminoxFloat / 100) * (1000000));
      
      lcd.setCursor(0,1);
      lcd.print(LuminoxPPM); lcd.print(" ppm O2");
      
      Luminoxstring = "";                         //clear the Luminoxstring:
      Luminox_stringcomplete = false;           
      delay(delayPeriod);                       //5 minutes(300000 millis); or 1 second (1000 millis)     
    }
  }
}

