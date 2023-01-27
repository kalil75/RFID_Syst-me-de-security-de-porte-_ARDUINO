#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

// Instance de la classe MFRC522
MFRC522 rfid(SS_PIN, RST_PIN);

//Créer un objet LCD : Utilisez 0x27 si 0x3F ne fonctionne pas
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Tableau d'initialisation qui stockera le nouveau NUID
byte nuidPICC[4];

// Mettez votre NUID d'accès ici
byte master[4] = {0x49, 0xA1, 0xDE, 0x6E};

// Broche connectée au signal de relais de verrouillage
int lockOutput = 2;

int greenLED = 3;
int redLED = 4;
int buzzerPin = 5;


void setup() {
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  // Configurer l'écran LCD avec rétroéclairage et initialiser
  lcd.init();
  lcd.backlight();
  printWelcomeMessage();

  // Définir les broches de SORTIE
  pinMode(lockOutput, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {

  //Réinitialiser la boucle si pas de nouvelle carte présente sur le capteur/lecteur
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Vérifiez si le NUID a été lu
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  // Stocker NUID dans le tableau nuidPICC
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  // Vérifie si l'étiquette scannée est autorisée
  if (getAccessState(master, nuidPICC) == true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Access Granted!");

    // Turn on relay & green LED for 5 seconds
    digitalWrite(lockOutput, HIGH);
    digitalWrite(greenLED, HIGH);
    delay(4000);
    digitalWrite(lockOutput, LOW);
    digitalWrite(greenLED, LOW);
    
    delay(25);  //Pour résoudre le problème (LCD, solénoïde)
    printWelcomeMessage();

  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Access Denied!");

    digitalWrite(redLED, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzerPin, LOW);

    printWelcomeMessage();

  }
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

}

bool getAccessState(byte accessCode[], byte newCode[]) {
  for (byte i = 0; i < 4; i++) {
    if (newCode[i] != accessCode[i]) {
      return false;
    }
  }
  return true;
}

void printWelcomeMessage() {
  lcd.clear();
  lcd.print("<Access Control>");
  lcd.setCursor(0, 1);
  lcd.print(" Scan Your Tag!");
}
