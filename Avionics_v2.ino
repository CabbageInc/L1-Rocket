#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SD.h>

#define seaLevelPressure_hPa 1013.25

//File Format:
// timestamp,temperature,pressure,altitude,seaLevelPressure,realAltitude

// Random Number
long randnum;

// Data filename
String filename = "fd";

// Count filename
const String countFileName = "count.txt";
String count_str;
int count = 1;
char c;

// Digital pin for SD card
const int chipSelect = 4;

// Instance for BMP085 object
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);

  // Initialize BMP180 sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP180 sensor.");
    return;
  }
  Serial.println("BMP180 sensor discovered and initialized.");

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card intialization failed.");
    return;
  }else{
    Serial.println("SD card initialization successful");
  }

  if SD.exists(countFileName){
    File countFile = SD.open(countFileName, FILE_READ);
    if(countFile){
      while(countFile.available())){
        c = countFile.read();
        if(isPrintable(c)){
          count_str.concat(c);
        }
      }
      if(count_str.length() >= 1){
        count = count_str.toInt();
      }
      countFile.close();
    }else{
      Serial.println("Unable to read from count file");
    }
  }

  filename = filename + count;
  filename = filename + ".txt";
  filename = filename.c_str();
  Serial.println(filename);

  count++;

  countFile = SD.open(countFilename, FILE_WRITE);
  if(countFile){
    countFile.print(count);
    countFile.close();
  }else{
    Serial.println("Unable to write to count file");
  }
  
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.print("timestamp,temperature,pressure,altitude,seaLevelPressure,realAltitude");
    dataFile.close();
    Serial.println("Data written to SD card.");
  }else {
    Serial.print("Error writing to ");
    Serial.println(filename);
  }
  Serial.println();
}

void loop() {
  // Read BMP180 sensor data
  float timestamp = millis()*0.001;
  float temperature = bmp.readTemperature();
  int32_t pressure = bmp.readPressure();
  float altitude = bmp.readAltitude();
  int32_t seaLevelPressure = bmp.readSealevelPressure();
  float realAltitude = bmp.readAltitude(seaLevelPressure_hPa * 100);

  // Print sensor data to serial monitor
  Serial.print("Time = ");
  Serial.print(timestamp);
  Serial.println("s");
  
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" Pa");

  Serial.print("Altitude = ");
  Serial.print(altitude);
  Serial.println(" meters");

  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.print(seaLevelPressure);
  Serial.println(" Pa");

  Serial.print("Real altitude = ");
  Serial.print(realAltitude);
  Serial.println(" meters");

  // Write sensor data to SD card
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.print(",");
    dataFile.print(timestamp);
    dataFile.print(",");
    dataFile.print(temperature);
    dataFile.print(",");
    dataFile.print(pressure);
    dataFile.print(",");
    dataFile.print(altitude);
    dataFile.print(",");
    dataFile.print(seaLevelPressure);
    dataFile.print(",");
    dataFile.print(realAltitude);
    
    dataFile.close();
    Serial.println("Data written to SD card.");
  } else {
    Serial.print("Error writing to ");
    Serial.println(filename);
  }
  Serial.println();

  delay(2000); // Delay for readability
}
