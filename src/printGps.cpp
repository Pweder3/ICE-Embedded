#include <SparkFun_I2C_GPS_Arduino_Library.h> 
#include <TinyGPS++.h> 
#include <FS.h>
#include <SPI.h>
#include <SD.h>

int sck = 36;
int miso = 37;
int mosi = 35;
int cs = 10;

void readFile(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

I2CGPS myI2CGPS; 
TinyGPSPlus gps; 

void setup() {
  Serial.begin(115200);
  delay(1000); // needs delay beore starting so that the serial port has time before sending data so it isnt lost
  
  if (myI2CGPS.begin(Wire,400000) == false) // running at 400 khz
  {
    Serial.println("Module failed to respond. Please check wiring.");
    while (1); 
  }
  Serial.println("GPS module found!");

  myI2CGPS.enableDebugging(Serial);
  
  // String coldStartPacket = myI2CGPS.createMTKpacket(103,"");
  // myI2CGPS.sendMTKpacket(coldStartPacket);

  // SPI.begin(sck,miso,mosi,cs);
  // if (!SD.begin(cs)){
  //   Serial.println("Sd Card did not initalize");
  // }

  // int i  = 0;
  // String bName = "dataFile_";
  // String fName;
  // do{
  //   i+=1;
  //   fName = bName + String(i);  
  //   Serial.println(fName);
  // } while(SD.exists(fName));

}

void loop() {
  while ((myI2CGPS.available())){
    gps.encode(myI2CGPS.read()); // wish i could have this on another thread
  }
  Serial.print(">Lat:");
  Serial.println(gps.location.lat(),12);

  Serial.print(">Long:");
  Serial.println(gps.location.lng(), 12);

  Serial.print(">satNum:");
  Serial.println(gps.satellites.value());

}
