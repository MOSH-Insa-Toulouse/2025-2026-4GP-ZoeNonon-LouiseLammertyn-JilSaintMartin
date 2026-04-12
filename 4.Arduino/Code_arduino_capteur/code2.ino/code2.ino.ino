#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// --- Constantes Capteur ---
const int flexPin = A0;
float VCC = 5.0;
const float R_DIV = 47000.0;
float Rflex_min;

// --- Potentiomètre digital ---
#define MCP_NOP     0b00000000
#define MCP_WRITE   0b00010001
#define MCP_SHTDWN  0b00100001
const int ssMCPin = 8; 

// --- Écran OLED ---
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_RESET  -1
#define OLED_ADDR   0x3C
Adafruit_SSD1306 ecranOLED(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

// --- Encodeur rotatif ---
#define BUTTON_PIN  5
const int encoderPinA = 2;   // CLK encodeur
const int encoderPinB = 4;   // DT encodeur

// --- Constantes Électroniques  ---
float VCC = 5.0;
const float R1 = 100000.0;   // 100k
const float R3 = 100000.0;   // 100k
const float R5 = 10000.0;    // 10k



// --- Menu ---
int menuIndex = 0;        // 0=Calibration, 1=Mesure, 2=mesure2
bool modeChoisi = false;  // true si appui pour valider
bool switchLastState = HIGH;

// --- Fonctions Capteur ---
float lecture_tension() {
  float Vadc = analogRead(flexPin);
  return (Vadc * VCC) / 1023.0;
}

float calculateFlexResistance() {
  float Vadc = lecture_tension();
  return R_DIV * (VCC / Vadc - 1.0);
}

void ecriture_potentiometre(uint8_t cmd, uint8_t data, uint8_t ssPin) {
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  digitalWrite(ssPin, LOW);
  SPI.transfer(cmd);
  SPI.transfer(data);
  digitalWrite(ssPin, HIGH);
  SPI.endTransaction();
}

// --- OLED ---
void init_oled() {
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,0);
  ecranOLED.setTextSize(1);
  ecranOLED.println("Bienvenue !");
  ecranOLED.display();
}

void affichage_ecran() {
  float tension = lecture_tension();
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2);
  ecranOLED.setCursor(0,0);
  ecranOLED.println("Tension capteur:");
  ecranOLED.println();
  ecranOLED.println(tension);
  ecranOLED.display();
}

// 
void MesureAngle() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2);
  ecranOLED.setCursor(0,0);
  ecranOLED.println("Mode Test");
  ecranOLED.display();
  Serial.println("Mode Test activé !");
  delay(2000); 
}

// --- Affichage menu ---
void Afficher_menu() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println("Choix du mode:");
  ecranOLED.println();
  if(menuIndex == 0) ecranOLED.println("> Calibration");
  else ecranOLED.println("  Calibration");
  if(menuIndex == 1) ecranOLED.println("> Mesure");
  else ecranOLED.println("  Mesure");
  if(menuIndex == 2) ecranOLED.println("> Test resitance");
  else ecranOLED.println("  Test");
  ecranOLED.println();
  ecranOLED.println("Appuyez pour valider");
  ecranOLED.display();
}

// --- Calibration ---
void calibration() {
  float tension_AO = lecture_tension();
  float tension_cible = 2.5;
  float diff1, diff2;
  int pos = 128;

  diff1 = abs(tension_AO - tension_cible);

  do {
    ecriture_potentiometre(MCP_WRITE, pos, ssMCPin);
    delay(100);
    tension_AO = lecture_tension();
    diff2 = abs(tension_AO - tension_cible);

    if (diff2 > diff1) {
      if (pos == 255) pos = 1;
      else pos++;
    } else {
      if (pos == 0) pos = 254;
      else pos--;
    }
    diff1 = abs(tension_AO - tension_cible);

  } while ((tension_AO < 2.4 || tension_AO > 2.6));

  Serial.print("Tension calibrée: ");
  Serial.println(tension_AO);
  Rflex_min = calculateFlexResistance();
  Serial.print("Pot calibré position: ");
  Serial.println(pos);
  Serial.print("Resistance au repos: ");
  Serial.println(Rflex_min);
}

// --- Bouton du potentiometre  ---
void checkButton() {
  bool switchState = digitalRead(BUTTON_PIN);
  if (switchState == LOW && switchLastState == HIGH) {
    delay(50); // debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      // valider le choix actuel
      modeChoisi = true;
    }
  }
  switchLastState = switchState;
}

// --- Setup ---
void setup() {
  pinMode(ssMCPin, OUTPUT); 
  digitalWrite(ssMCPin, HIGH);
  SPI.begin(); 

  Serial.begin(9600);

  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("Échec OLED"));
    for(;;);
  }
  ecranOLED.setTextColor(SSD1306_WHITE);
  ecranOLED.setTextWrap(true);

  init_oled();

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Afficher_menu();
}

// --- Loop ---
void loop() {
  checkButton();

  if(modeChoisi) {
    if(menuIndex == 0) calibration(); // Calibration
    else if(menuIndex == 1) affichage_ecran(); // Mesure
    else if(menuIndex == 2) modeTest(); // Test

    modeChoisi = false; 
    Afficher_menu();
  }

  
  delay(50);
}
