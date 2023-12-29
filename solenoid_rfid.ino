// I2C LCD
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


// RFID
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);


// Define the list of UIDs
byte knownUIDs[][4] = {
  {0x83, 0x56, 0xD7, 0x0F}, // card
  {0x23, 0xF1, 0xB0, 0x0D}  // keychain
};


// For Solenoid Lock
#define lock_relay 3

// Buzzer
#define buzzer 10

void setup() {
  // initialize serial monitor
  Serial.begin(9600);
  SPI.begin();      

  // initialize the LCD 
  lcd.init();       
  lcd.backlight();
  // lcd.setCursor(0, 0);          // set the cursor to the top-left corner
  // lcd.print("RFID Reader");   // print "Hello, World!" to the LCD
  // lcd.setCursor(3, 1);
  // lcd.print("Initialized...");
  // delay(1500);
  // lcd.clear();

  // initialize RFID Reader
  mfrc522.PCD_Init();
  Serial.println("RFID Reader Initialized");

  // solenoid & relay 
  pinMode(lock_relay, OUTPUT);

  // buzzer
  pinMode(buzzer, OUTPUT);
}


void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Locked...");

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    
    if (isKnownUID(mfrc522.uid.uidByte)) {
      unlocked();
    } else {
      unknown();
    }
  }

  //delay(1000);
}


// check if the scanned UID matches any of the known UIDs
bool isKnownUID(byte *uid) {
  // Compare the scanned UID with each known UID
  for (byte i = 0; i < sizeof(knownUIDs) / sizeof(knownUIDs[0]); i++) {
    if (compareUID(uid, knownUIDs[i], sizeof(knownUIDs[i]))) {
      return true; // UID matches a known UID
    }
  }
  return false; // UID doesn't match any known UID
}


// to compare two UIDs byte by byte
bool compareUID(byte *uid, byte *knownUID, byte size) {
  for (byte i = 0; i < size; i++) {
    if (uid[i] != knownUID[i]) {
      return false; // UID doesn't match
    }
  }
  return true; // UID matches
}


// registered UID 
void unlocked(){
  lcd.clear();
  lcd.print("Unlocked...");
  Serial.println("UID registered");

  digitalWrite(lock_relay, HIGH);

  tone(buzzer, 500);
  delay(300);
  noTone(buzzer);

  delay(2000);
  digitalWrite(lock_relay, LOW);

  // restarts the lock
  asm volatile ("jmp 0");
}


// unknown UID
void unknown(){
  lcd.clear();
  lcd.print("WARNING!!!");
  lcd.setCursor(0, 1);
  lcd.print("ID unknown!");
  Serial.println("UID unknown");

  tone(buzzer, 2000);
  delay(3000);
  noTone(buzzer);
}
