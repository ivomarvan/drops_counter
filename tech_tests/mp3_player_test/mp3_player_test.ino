#include <Arduino.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

HardwareSerial mySerial(1);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }
  
  Serial.println("Starting DFPlayer Mini...");

  mySerial.begin(9600, SERIAL_8N1, 16, 17); // RX, TX pins
  if (!myDFPlayer.begin(mySerial)) { // Use hardware serial to communicate with MP3
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1. Please recheck the connection!"));
    Serial.println(F("2. Please insert the SD card!"));
    while (true);
  }
  
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(30);  // Set volume value. From 0 to 30
  delay(2000); // Wait for DFPlayer to stabilize
  
  Serial.println("Playing test file...");
  myDFPlayer.playFolder(0, 1); // Attempt to play the file 001.mp3 in folder 000
  Serial.println("Test file play command sent.");
}

void loop() {
  // Attempt to read player state multiple times
  for (int i = 0; i < 3; i++) {
    int state = myDFPlayer.readState();
    Serial.print("Player State: ");
    switch (state) {
      case 0: Serial.println("Stopped"); break;
      case 1: Serial.println("Playing"); break;
      case 2: Serial.println("Paused"); break;
      default: Serial.println("Unknown"); break;
    }
    
    int volume = myDFPlayer.readVolume();
    Serial.print("Current Volume: ");
    Serial.println(volume);
    
    delay(1000);
  }

  // Read and print current EQ setting
  int eq = myDFPlayer.readEQ();
  Serial.print("Current EQ: ");
  switch (eq) {
    case 0: Serial.println("Normal"); break;
    case 1: Serial.println("Pop"); break;
    case 2: Serial.println("Rock"); break;
    case 3: Serial.println("Jazz"); break;
    case 4: Serial.println("Classic"); break;
    case 5: Serial.println("Bass"); break;
    default: Serial.println("Unknown"); break;
  }

  // Read and print the number of files on the SD card
  int fileCounts = myDFPlayer.readFileCounts();
  Serial.print("Total Files on SD: ");
  Serial.println(fileCounts);

  // Read and print the current file number being played
  int currentFile = myDFPlayer.readCurrentFileNumber();
  Serial.print("Current File Number: ");
  Serial.println(currentFile);

  // Read and print the number of files in folder 0
  int filesInFolder = myDFPlayer.readFileCountsInFolder(0);
  Serial.print("Files in Folder 0: ");
  Serial.println(filesInFolder);

  // Read and print the number of folders
  int folderCounts = myDFPlayer.readFolderCounts();
  Serial.print("Total Folders: ");
  Serial.println(folderCounts);

  // Add a delay to avoid spamming the serial output
  delay(2000);
}
