#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))  // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/4, /*tx =*/5);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// pins
#define RX 16
#define TX 17
#define interruptPin 4

// settings/constants
const unsigned long debounceTime = 200;  // Ignore next interupt for short time [ms]
const unsigned long switchOffTime = 60 * 1000; // ask for swithitchin off after a time [ms]
const int readySoundId = 0;
const int switchOfSoundId = 121; 
// language {'cs': 0, 'en': 1, 'de': 2,  'fr': 3, 'es': 4}
const int lang_id = 1; 

// variables
long int count = 0;
long int lastCount = 0;
unsigned long lastInterruptTime = 0;     // storege for time of last interup (drop)

void setup_serial(void) {
  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }
}

void setup_player(void) {
#if (defined ESP32)
  FPSerial.begin(9600, SERIAL_8N1, RX, TX);
#else
  FPSerial.begin(9600);
#endif

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(25);  //Set volume value. From 0 to 30
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);

}

void setup_interupt() {
  // interupt
  pinMode(interruptPin, INPUT_PULLUP);                                            // set as input with pull-up rezistor
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, CHANGE);  // connet interupt rutine
}

void handleInterrupt() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceTime) {
    count++;
    lastInterruptTime = currentTime; 
  }
}

void playSound(short int langId, short int soundId, bool debug = true) {
  myDFPlayer.playLargeFolder(langId, soundId);

  if (debug) {
    if (myDFPlayer.available()) {
      Serial.print(F("lang:"));
      Serial.print(langId);
      Serial.print(F(", sound:"));
      Serial.print(soundId);
      Serial.print(F(", status:"));
      printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
    }
  }
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void setup() {
  setup_serial();
  setup_player();
  delay(600);
  playSound(lang_id, readySoundId);  // ready
  delay(300);
  lastInterruptTime  = millis();
  setup_interupt();  
}


void loop() {
  noInterrupts();
  long int countCopy = count;
  interrupts();

  if (lastCount != countCopy) {
    playSound(lang_id, countCopy);

    // if (myDFPlayer.available()) {
    //   printDetail(myDFPlayer.readType(), myDFPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
    // }
    Serial.println(countCopy);
    delay(50);
    lastCount = countCopy;
  }
  long currentTime = millis();
  if (currentTime - lastInterruptTime > switchOffTime) { 
    lastInterruptTime = currentTime;
    Serial.print(F("last count:")); Serial.println(countCopy);
    delay(1000); 
    playSound(lang_id, switchOfSoundId);
    delay(1000); 
  }
}
