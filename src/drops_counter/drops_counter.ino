#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))  // Using a software serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(4, 5);  // RX, TX pins
#define FPSerial softSerial
#else
#define FPSerial Serial1  // Use hardware serial if not AVR UNO or ESP8266
#endif

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// Pins
#define RX 16
#define TX 17
#define INTERRUPT_PIN 4

// Settings/Constants
const unsigned long DEBOUNCE_TIME = 200;  // Ignore subsequent interrupts within this time frame [ms]
const unsigned long SWITCH_OFF_TIME = 60 * 1000; // Time before suggesting switch off [ms]
const int READY_SOUND_ID = 0;
const int SWITCH_OFF_SOUND_ID = 121;
// Languages {'cs': 0, 'en': 1, 'de': 2,  'fr': 3, 'es': 4}
const int LANG_ID = 1;

// Variables
volatile long count = 0;
long lastCount = 0;
unsigned long lastInterruptTime = 0;  // Storage for the time of the last interrupt (drop)

void setup_serial() {
  Serial.begin(115200);
  while (!Serial) {
    // Wait for serial port to connect. Needed for some boards.
  }
}

void setup_player() {
#if defined(ESP32)
  FPSerial.begin(9600, SERIAL_8N1, RX, TX);
#else
  FPSerial.begin(9600);
#endif

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true)) {  // Use serial to communicate with MP3 player.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to handle ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(25);  // Set volume value. From 0 to 30
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
}

void setup_interrupt() {
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);  // Set as input with pull-up resistor
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), handleInterrupt, CHANGE);  // Connect interrupt routine
}

void handleInterrupt() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > DEBOUNCE_TIME) {
    count++;
    lastInterruptTime = currentTime;
  }
}

void playSound(short langId, short soundId, bool debug = true) {
  myDFPlayer.playLargeFolder(langId, soundId);

  if (debug) {
    if (myDFPlayer.available()) {
      Serial.print(F("lang:"));
      Serial.print(langId);
      Serial.print(F(", sound:"));
      Serial.print(soundId);
      Serial.print(F(", status:"));
      printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
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
  setup_interrupt();
  playSound(LANG_ID, READY_SOUND_ID);  // Play ready sound
  lastInterruptTime = millis();
}

void loop() {
  noInterrupts();
  long int countCopy = count;
  interrupts();

  if (lastCount != countCopy) {
    playSound(LANG_ID, countCopy);
    // if (myDFPlayer.available()) {
    //   printDetail(myDFPlayer.readType(), myDFPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
    // }
    Serial.println(countCopy);
    delay(50);
    lastCount = countCopy;
  }

  long currentTime = millis();
  if (currentTime - lastInterruptTime > SWITCH_OFF_TIME) {
    lastInterruptTime = currentTime;
    Serial.print(F("last count:")); Serial.println(countCopy);
    delay(1000);
    playSound(LANG_ID, SWITCH_OFF_SOUND_ID);
    delay(1000);
  }
}
