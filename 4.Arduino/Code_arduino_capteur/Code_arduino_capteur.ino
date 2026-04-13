#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// --- Configuration Pins ---
const int pinGraphite = A0;
const int pinFlex = A1;
const int ssMCPin = 8;     // CS du potentiomètre digital
const int BUTTON_PIN = 5;  // Switch encodeur
const int encoder0PinA = 2; // CLK
const int encoder0PinB = 4; // DT

// --- Constantes Électroniques ---
float VCC = 5.0;
const float R_DIV = 47000.0; // Résistance fixe pour le pont diviseur (Flex)
const float R1 = 100000.0, R3 = 100000.0, R5 = 10000.0; // Pour calcul Graphite

// --- Variables Globales ---
float Rflex_min = 30000.0; // Valeur de repos (à calibrer)
int pos_pot = 250;         // Position du potentiomètre digital
volatile int encoder0Pos = 0;
int condition = 0;         // Index du menu
bool modeChoisi = false;
bool switchLastState = HIGH;
bool utiliser_graphite = true; // Flag pour basculer entre les deux capteurs

// --- Écran OLED ---
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
Adafruit_SSD1306 ecranOLED(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

// --- FONCTIONS DE CALCUL (La "Méthode") ---

// 1. Lecture de la tension brute
float lireTension() {
  int pin = utiliser_graphite ? pinGraphite : pinFlex;
  return (analogRead(pin) * VCC) / 1023.0;
}

// 2. Calcul de la résistance selon le capteur choisi
float calculerResistance() {
  float Vadc = lireTension();
  if (Vadc < 0.1) Vadc = 0.1; // Éviter division par zéro

  if (utiliser_graphite) {
    // Formule complexe (Ampli-OP + Potentiomètre Digital)
    // R_potentiometre calculée selon la position actuelle du MCP41xxx
    float R_pot_actuelle = (48000.0 * pos_pot / 256.0) + 125.0; 
    return ((1.0 + (R3 / R_pot_actuelle)) * (VCC / Vadc)) * R1 - R1 - R5;
  } else {
    // Formule Pont Diviseur simple (Flex Sensor)
    return R_DIV * (VCC / Vadc - 1.0);
  }
}

// 3. Calcul de l'angle
float calculerAngle() {
  float R_actuelle = calculerResistance();
  // Interpolation : 0° = Rflex_min, 90° = Rflex_min * 2.5 (à ajuster)
  float angle = map(R_actuelle, Rflex_min, Rflex_min * 2.5, 0, 90);
  return constrain(angle, 0, 90);
}

// --- COMMUNICATION POTENTIOMETRE DIGITAL ---
void ecrirePotentiometre(uint8_t data) {
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  digitalWrite(ssMCPin, LOW);
  SPI.transfer(0x11); // Commande d'écriture
  SPI.transfer(data);
  digitalWrite(ssMCPin, HIGH);
  SPI.endTransaction();
}

// --- INTERRUPTION ENCODEUR ---
void doEncoder() {
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) encoder0Pos++;
  else encoder0Pos--;
}

// --- AFFICHAGE OLED ---
void afficherMenu() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.print("CAPTEUR: "); ecranOLED.println(utiliser_graphite ? "GRAPHITE" : "FLEX");
  ecranOLED.println("---------------------");
  
  char* labels[] = {"Tension", "Resistance", "Angle", "Calibration", "Changer Capteur"};
  for(int i=0; i<5; i++) {
    if(condition == i) ecranOLED.print("> "); else ecranOLED.print("  ");
    ecranOLED.println(labels[i]);
  }
  ecranOLED.display();
}

// --- LOGIQUE DE CALIBRATION ---
void executerCalibration() {
  float cible = 2.5;
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,20);
  ecranOLED.println("Calib. en cours...");
  ecranOLED.display();

  for (int i = 0; i < 255; i++) {
    pos_pot = i;
    ecrirePotentiometre(pos_pot);
    delay(20);
    if (abs(lireTension() - cible) < 0.1) break; 
  }
  
  Rflex_min = calculerResistance(); // Enregistre la valeur au repos
  modeChoisi = false;
}

// --- SETUP ---
void setup() {
  Serial.begin(9600);
  pinMode(ssMCPin, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(encoder0PinA, INPUT_PULLUP);
  pinMode(encoder0PinB, INPUT_PULLUP);
  
  SPI.begin();
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, RISING);
  
  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while(1);
  ecranOLED.setTextColor(SSD1306_WHITE);
  
  ecrirePotentiometre(pos_pot);
  afficherMenu();
}

// --- LOOP ---
void loop() {
  // 1. Gestion de l'encodeur pour la sélection
  condition = abs(encoder0Pos / 2) % 5; 

  // 2. Gestion du bouton
  bool currentButton = digitalRead(BUTTON_PIN);
  if (currentButton == LOW && switchLastState == HIGH) {
    delay(200);
    if (condition == 4) { // Changer Capteur
       utiliser_graphite = !utiliser_graphite;
    } else {
       modeChoisi = !modeChoisi;
    }
    afficherMenu();
  }
  switchLastState = currentButton;

  // 3. Affichage des modes actifs
  if (modeChoisi) {
    ecranOLED.clearDisplay();
    ecranOLED.setCursor(0,0);
    ecranOLED.setTextSize(1);
    ecranOLED.println(utiliser_graphite ? "MODE GRAPHITE" : "MODE FLEX");
    ecranOLED.setTextSize(2);
    ecranOLED.setCursor(0, 30);

    switch(condition) {
      case 0: ecranOLED.print(lireTension()); ecranOLED.print(" V"); break;
      case 1: ecranOLED.print(calculerResistance()/1000.0, 1); ecranOLED.print(" kOhm"); break;
      case 2: ecranOLED.print(calculerAngle()); ecranOLED.write(247); break;
      case 3: executerCalibration(); break;
    }
    ecranOLED.display();
  } else {
    static int lastCond = -1;
    if(condition != lastCond) {
      afficherMenu();
      lastCond = condition;
    }
  }
  delay(50);
}
