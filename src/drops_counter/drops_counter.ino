long int count = 0;
long int lastCount = 0;
const int interruptPin = 4;
const int ledPin = 2;
unsigned long lastInterruptTime = 0; // Uchovává čas posledního přerušení
const unsigned long debounceTime = 300; // Ignorování přerušení do 300 ms

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }
  
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP); // Nastaví pin jako vstupní s pull-up rezistorem
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, CHANGE); // Připojí přerušení
  
  Serial.println("ready");
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
}

void handleInterrupt() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceTime) {
    count++;
    lastInterruptTime = currentTime; // Aktualizace času posledního přerušení
  }
}

void loop() {
  noInterrupts();
  long int countCopy = count;
  interrupts();

  if (lastCount != countCopy) {
    Serial.println(countCopy);
    delay(100);
    lastCount = countCopy;
  }
}
