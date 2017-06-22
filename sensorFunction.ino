/*
*  This is a program to automate and control the parameters of a hydroponic garden.
*  Code was designed to be run with an Arduino Mega 2560
*  The project was completed by research students at San Antonio College
*/

#include <SoftwareSerial.h>                           // Include the SoftwareSerial library for DO sensor
#include "DHT.h"                                      // Include the DHT library for DHT22 sensor
#include <Adafruit_RGBLCDShield.h>

#define DHTTYPE DHT22   // DHT 22  (AM2302), (AM2321)
#define RX 11
#define TX 10
#define DHTPIN 4                                      // Define D/O pin for DHT sensor

DHT dht(DHTPIN, DHTTYPE);                             // Define how the DHT sensor is going to work
SoftwareSerial Serial4(RX, TX);                       // Define how the soft serial port is going to work
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();


/*<<<SENSORS WILL REQUIRE A POINTER AND A COMMAND FOR SERIAL COMMUNICATION>>>
POINTERS
RTD - Water Temperature
EC - Electroconductivity Sensor
PH - pH Sensor
DO - Dissolved Oxygen Sensor
DHT - Temperature and Humidity Sensor
Directions: Type the pointer using the serial monitor and press enter.  Type your command and press enter.
<<<FOR COMMANDS PLEASE REFER TO ATLAS SCIENTIFIC MANUALS IN THE GITHUB REPOSITORY>>>*/


float waterTemp;                                      // Creates a floating point number that is the water temperature
float conductivity;                                   // Creates a floating point number that is the nutrient conductivity
float pH;                                             // Creates a floating point number that is the pH
float oxygen;                                         // Creates a floating point number that is the dissolved oxygen
float humidity;                                       // Creates a floating point number that is the humidity
float airTempC;                                       // Creates a floating point number that is the air temperature (Celsius)
float airTempF;                                       // Creates a floating point number that is the air temperature (Fahrenheit)

int seconds = 5000;                                   // LCD scroll delay in milliseconds (x1000)


void setup() {                                        // *****SETUP FUNCTION***** //
  Serial.begin(9600);                                 // Set baud rate for the hardware serial port_0 to 9600
  Serial1.begin(9600);                                // Set baud rate for the hardware serial port_1 to 9600
  Serial2.begin(9600);                                // Set baud rate for the hardware serial port_2 to 9600
  Serial3.begin(9600);                                // Set baud rate for the hardware serial port_3 to 9600
  Serial4.begin(9600);                                // Set baud rate for the software serial port_4 to 9600


  Serial.println("Collecting Data...");               // Display start of data collection
  dht.begin();                                        // Start DHT sensor //a.k.a. the Humidity and Air Temperature Sensor
  lcd.begin(16, 2);                                   // Start LCD screen
  lcd.print("Collecting data...");
  uint8_t i = 0;
}

void loop() {                                         // *****LOOP FUNCTION***** //  
                            //serialEvent ();
  sensorCommunication();

  printData();
}

void sensorCommunication() {                      // *****SENDING COMMANDS TO ATLAS SENSORS***** //

  boolean pointerStringComplete = false;                // Flags to signal that a string has been assigned to a variable
  boolean commandStringComplete = false;
  boolean sensorStringRTDComplete = false;
  boolean sensorStringECComplete = false;
  boolean sensorStringPHComplete = false;
  boolean sensorStringDOComplete = false;

  String pointerString = "";                      // Variables to hold the strings
  String commandString = "";
  String sensorStringRTD = "";                         
  String sensorStringEC = "";                           
  String sensorStringPH = "";                           
  String sensorStringDO = "";                           

  pointerString.reserve(10);                      // How many bytes of data should be reserved for each string?
  commandString.reserve(10);
  sensorStringRTD.reserve(30);
  sensorStringEC.reserve(30);
  sensorStringPH.reserve(30);
  sensorStringDO.reserve(30);

                                    // AIR TEMPERATURE & HUMIDITY (DHT)           
  humidity = dht.readHumidity();                      // Read humidity as percentage
  airTempC = dht.readTemperature();                 // Read temperature as Celsius (the default)
  airTempF = dht.readTemperature(true);               // Read temperature as Fahrenheit (isFahrenheit = true) 

  if (isnan(humidity) || isnan(airTempC) || isnan(airTempF)) {        // Check if any DHT reads failed and exit early (to try again).
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  sensorStringRTD = Serial1.readStringUntil(13);            // Read water temperature sensor
  waterTemp = sensorStringRTD.toFloat();
  waterTemp = waterTemp*1.8 + 32;                     
  sensorStringRTD = "";

  sensorStringEC = Serial2.readStringUntil(13);           // Read conductivity sensor
  conductivity = sensorStringEC.toFloat();            
  sensorStringEC = "";

  sensorStringPH = Serial3.readStringUntil(13);           // Read pH sensor
  pH = sensorStringPH.toFloat();                      
  sensorStringPH = "";

  sensorStringDO = Serial4.readStringUntil(13);           // Read disolved oxygen sensor
  oxygen = sensorStringDO.toFloat();                  
  sensorStringDO = "";

  /*if (Serial4.available() > 0) {                      // If we see that the Atlas Scientific DO has sent a character
    char inchar = (char)Serial4.read();               // Get the char we just received
    sensorStringDO += inchar;                         // Add the char to the var called sensorstring
    oxygen = sensorStringDO.toFloat();          // Convert the string to a floating point number so it can be evaluated by the Arduino  
  }*/

  if (Serial.available() > 0) {                   // Sending commands from serial monitor
    pointerString = Serial.readStringUntil(13);           // Read the string until we see a <CR> and assign it to pointerString
    if (pointerString.length() > 0) {               // If something has been assigned to pointerString
      pointerStringComplete = true;               // Change flag to true
      Serial.print("You have entered: ");             // Show user value of pointerString
      Serial.println(pointerString);
      Serial.println("Please enter your command");        // Ask user for command
    }

    while (commandStringComplete == false) {            // While the commandString remains empty
      delay(50);                          // Wait for user to enter command
      commandString = Serial.readStringUntil(13);         // Read the string until we see a <CR>
      if (commandString.length() > 0) {             // If something has been assigned to commandString
        commandStringComplete = true;             // Change flag to true
      }
    }
    Serial.print("You have entered: ");               // Show user value of commandString
    Serial.println(commandString);
    Serial.println("Sending...");
  }

  if (pointerStringComplete == true && commandStringComplete == true) {   // Once the pointer and command are ready, send the command to the correct sensor
                                    
    if (pointerString == "RTD") {                 // SEND TO WATER TEMPERATURE SENSOR
      Serial1.print(commandString);
      Serial1.print('\r');
      while (sensorStringRTDComplete == false) {
        sensorStringRTD = Serial1.readStringUntil(13);
        if (sensorStringRTD.length() > 0) {
          sensorStringRTDComplete = true;
        }
      }
      Serial.print("Response from sensor: ");
      Serial.println(sensorStringRTD);
      sensorStringRTD = "";
      sensorStringRTDComplete = false;
    }

    else if (pointerString == "EC") {               // SEND TO CONDUCTIVITY SENSOR
      Serial2.print(commandString);
      Serial2.print('\r');
      while (sensorStringECComplete == false) {
        sensorStringEC = Serial2.readStringUntil(13);
        if (sensorStringEC.length() > 0) {
          sensorStringECComplete = true;
        }
      }
      Serial.print("Response from sensor: ");
      Serial.println(sensorStringEC);
      sensorStringEC = "";
      sensorStringECComplete = false;
    }

    else if (pointerString == "PH") {               // SEND TO PH SENSOR
      
      Serial3.print(commandString);
      Serial3.print('\r');
      while (sensorStringPHComplete == false) {
 //       if (Serial3.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
 //           char inchar = (char)Serial3.read();              //get the char we just received
 //           sensorStringPH += inchar;                           //add the char to the var called sensorstring
 //           if (inchar == '\r') {                             //if the incoming character is a <CR>
 //           sensorStringPHComplete = true;                  //set the flag
 //           }
        sensorStringPH = Serial3.readStringUntil(13);
        if (sensorStringPH.length() > 0) {
          sensorStringPHComplete = true;
        }
      }
      Serial.print("Response from sensor: ");
      Serial.println(sensorStringPH);
      sensorStringPH = "";
      sensorStringPHComplete = false;
    }

    else if (pointerString == "DO") {               //SEND TO DISSOLVED OXYGEN SENSOR
      Serial4.print(commandString);
      Serial4.print('\r');
      while (sensorStringDOComplete == false) {
        //delay(50);
        sensorStringDO = Serial4.readStringUntil(13);
        if (sensorStringDO.length() > 0) {
          sensorStringDOComplete = true;
        }
      }
      Serial.print("Response from sensor: ");
      Serial.println(sensorStringDO);
      sensorStringDO = "";
      sensorStringDOComplete = false;
    }

    pointerString = "";                       // Clear the strings
    commandString = "";

    pointerStringComplete = false;                  // Reset the flags
    commandStringComplete = false;
  }
}




void printData() {                                   // *****FUNCTION TO PRINT DATA TO SERIAL MONITOR AND LCD***** //
  Serial.print("Water Temperature: ");                // Display Water Temperature
  Serial.print(waterTemp);
  Serial.print(" *F");
  Serial.print("\t");

  Serial.print("Conductivity: ");                     // Display Conductivity
  Serial.print(conductivity);
  Serial.print("\t\t");

  Serial.print("pH: ");                               // Display pH
  Serial.print(pH);
  if (pH >= 7.0) {                                  // If the pH is greater than or equal to 7.0
    Serial.print(" (high)");                        // Print "high" this is demonstrating that the Arduino is evaluating the pH as a number and not as a string
    Serial.print("\t\t");
  }
  if (pH <= 6.999 && pH >= 5.001) {                 // If the pH is 5.0 and 7.0
    Serial.print(" (normal)");                      // Print "low" this is demonstrating that the Arduino is evaluating the pH as a number and not as a string
    Serial.print("\t\t");
  }
  if (pH <= 5.000) {                                // If the pH is less than or equal to 5.0
    Serial.print(" (low)");                         // Print "low" this is demonstrating that the Arduino is evaluating the pH as a number and not as a string
    Serial.print("\t\t");
  }

  Serial.print("Dissolved Oxygen: ");                 //Display Dissolved Oxygen
  Serial.print(oxygen);
  Serial.print("\t\t");

  Serial.print("Humidity: ");                         //Display Humidity
  Serial.print(humidity);
  Serial.print(" %\t");

  Serial.print("Temperature: ");                      // Display Air Temperature
  Serial.print(airTempF);
  Serial.println(" *F");

  uint8_t buttons = lcd.readButtons();                //  ????????????????? Has a button been pushed?

  lcd.clear();                                        // Clear contents of LCD
  lcd.setCursor(0, 0);                                // Set the cursor to column 0, line 0 
  int wtemp = waterTemp;                              // Change floating point numbers to integers
  int atemp = airTempF;
  lcd.print("H20 ");                                  //Print data to LCD screen
  lcd.print(wtemp);
  lcd.print("F ");
  lcd.print("AIR ");
  lcd.print(atemp);
  lcd.print("F");
  lcd.setCursor(0, 1);
  lcd.print("PH ");
  lcd.print(pH);
  lcd.print(" ");
  lcd.print("EC ");
  lcd.print(conductivity);
  lcd.print("");

  delay(seconds);                                     // Pause before displaying more information

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HUMIDITY ");
  lcd.print(humidity);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print("DO ");
  lcd.print(oxygen);
  lcd.print(" ");
  lcd.print("CO2 ");
  lcd.print("N/A");
  lcd.print("");

  delay(seconds);                                    // Pause before displaying more information

  if (buttons) {                                      // If a button is pushed...
    lcd.clear();                                      // Clear the screen and reset the cursor
    lcd.setCursor(0, 0);

    if (buttons & BUTTON_UP) {                        // Top button instructions
      lcd.print("WATER TEMP ");
      lcd.print(waterTemp);
      lcd.print(" F");
      lcd.setCursor(0, 1);
      lcd.print("AIR TEMP");
      lcd.print(airTempF);
      lcd.print(" F");
      delay(seconds);
    }

    if (buttons & BUTTON_DOWN) {                      // Down button instructions
      lcd.print("DOWN ");
    }
    if (buttons & BUTTON_LEFT) {                      // Left button instructions
      lcd.print("LEFT ");
    }
    if (buttons & BUTTON_RIGHT) {                     // Right button instructions
      lcd.print("RIGHT ");
    }
    if (buttons & BUTTON_SELECT) {                    // Select button instructions
      lcd.print("SELECT ");
    }
  }
}