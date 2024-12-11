

#include <FS.h>
#include <SPI.h>
#include <SD.h>

#include <SparkFun_I2C_GPS_Arduino_Library.h> 
I2CGPS myI2CGPS; 

#include <TinyGPS++.h> 
TinyGPSPlus gps; 


unsigned int flickerDelayT = 500;
 
const int sck = 36;
const int miso = 37;
const int mosi = 35;
const int cs = 10;

const int buttonPin = 6;
const int ledPin = 9;

int lastButtonState = 0;
int btnState =1; 
unsigned long lastDebounceTime;

unsigned long tempMillis = millis();
int ledState = false;

String data[3];


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


char* generateFileName(fs::FS &fs){

  static char buff[15];
  char* bName = "dataFile_0";
  int numFiles = 0;

  File root = fs.open("/");
  File file = root.openNextFile();
  while(file){
    if (!file.isDirectory()){
      numFiles +=1;
    }
    file = root.openNextFile();
  }
  snprintf(buff, sizeof(buff), "%s%i",bName,numFiles);
  return buff;
}




void setup() {
  Serial.begin(115200);
  delay(1000);

  if (myI2CGPS.begin() == false)
  {
    Serial.println("Module failed to respond. Please check wiring.");
    while (1); 
  }
  Serial.println("GPS module found!");
  

  pinMode(buttonPin,INPUT_PULLUP);
  pinMode(ledPin,OUTPUT);

  for(int i =0; i< 5; i++){
    digitalWrite(ledPin,HIGH);
    delay(50);
    digitalWrite(ledPin,LOW);
    delay(50);
  }


  Serial.println("-------STARTING PRORGRAM-------");
  SPI.begin(sck,miso,mosi,cs);
  if (!SD.begin(cs)){
    Serial.println("Sd Card did not initalize");
  }


  listDir(SD,"/",0);
  Serial.println(SD.exists("dataFile_1"));

  char* fName = generateFileName(SD);

  Serial.println(fName);

  // writeFile(SD,fName,"X,Y,M");
  
  

}



void loop() {

  // while (myI2CGPS.available()) 
  // {
  //   gps.encode(myI2CGPS.read()); 
  // }

  // data[0] = gps.location.lat();
  // data[1] = gps.location.lng();


  int reading = digitalRead(buttonPin);

  // Serial.println()

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > 50) {
    
    if (reading != btnState) {
      btnState = reading;
      

      if (btnState == LOW) {
        flickerDelayT = 50; //debounce logic 
        Serial.println("Flaged");
        
        // data[2] = 1;

      }
    }
  } 

  lastButtonState  = reading;

  
  if (!reading){
    flickerDelayT  = 50;
  }else {
    flickerDelayT = 500;
  }


  if (millis() -tempMillis  > flickerDelayT){
    ledState = !ledState;
    digitalWrite(ledPin,ledState);
    tempMillis = millis();
    
  }

  // if (!gps.location.isValid()){
  //   digitalWrite(ledPin,LOW);
  // }

  // char foo[32];

  // for (int i = 0; i < 3; i++) {

  // index += snprintf(foo + index, sizeof(foo) - index, "%d", arr[i]);
  
  // if (i < 2) {
  //   index += snprintf(foo + index, sizeof(foo) - index, ",");
  //  }
  // }

  // appendFile(SD,fName,foo);
  
  //Comented out because Demo  will not use GPS

}
