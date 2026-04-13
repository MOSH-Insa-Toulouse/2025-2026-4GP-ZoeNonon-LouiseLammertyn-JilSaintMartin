#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// --- Constantes Capteur ---
const int Graphite_sensor = A0;
const int flex_sensor = A1;

float VCC = 5.0;
const float R_DIV = 47000.0;

float R3 = 100000;
float R5 = 10000;
float R1 = 100000;

float R_potentiometre = 47000; // pour le flex sensor
float Rf;
float Rflex_min;

// choix du capteur
bool utiliser_graphite = true;

// --- Variables menu ---
int condition = 0;
bool modeChoisi = false;
bool switchLastState = HIGH;

// --- Potentiomètre digital ---
#define MCP_WRITE 0b00010001
const int ssMCPin = 8;

// --- OLED ---
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define adresseI2CecranOLED 0x3C
Adafruit_SSD1306 ecranOLED(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

// --- Encodeur ---
#define BUTTON_PIN 5
#define encoder0PinA 2
#define encoder0PinB 4

volatile int encoder0Pos = 0;

// lecteur de la tension sur la PINA0 ou PINA1
float lecture_tension() {

  float Vadc;

  if (utiliser_graphite) {
    Vadc = analogRead(Graphite_sensor);
  } else {
    Vadc = analogRead(flex_sensor);
  }

  return (Vadc * VCC) / 1023.0;
}

// Calcul de la résitance -
float calculateFlexResistance() {

  float Vadc = lecture_tension();

  if (utiliser_graphite) {
    Rf = ((1 + (R3 / R_potentiometre)) * (VCC / Vadc)) * R1 - R1 - R5; // pour le graphite sensor
    return Rf;
  } else {
    float Rflex = R_DIV * (VCC / Vadc - 1.0); // pour le flex sensor
    return Rflex;
  }
}

// angle
float calculer_angle(){

  if (utiliser_graphite) return 0;

  float current_R = calculateFlexResistance();

  float r_flat = Rflex_min;
  float r_90 = Rflex_min * 2.5;

  float angle = (current_R - r_flat) * 90 / (r_90 - r_flat);

  if (angle < 0) angle = 0;
  if (angle > 90) angle = 90;

  return angle;
}

// ----------------------
// POTENTIOMETRE
// ----------------------
void ecriture_potentiometre(uint8_t cmd, uint8_t data, uint8_t ssPin) {
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  digitalWrite(ssPin, LOW);
  SPI.transfer(cmd);
  SPI.transfer(data);
  digitalWrite(ssPin, HIGH);
  SPI.endTransaction();
}

// ----------------------
// OLED
// ----------------------
void affichage_ecran_tension() {
  float tension = lecture_tension();

  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2);
  ecranOLED.setCursor(0,0);
  ecranOLED.println("Tension:");
  ecranOLED.println(tension);
  ecranOLED.display();
}

void affichage_ecran_resistance() {
  float resistance = calculateFlexResistance();

  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2);
  ecranOLED.setCursor(0,0);
  ecranOLED.println("Resistance:");
  ecranOLED.println(resistance);
  ecranOLED.display();
}

void affichage_ecran_angle() {
  float angle = calculer_angle();

  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2);
  ecranOLED.setCursor(0,0);
  ecranOLED.println("Angle:");
  ecranOLED.println(angle);
  ecranOLED.display();
}

// menu 
void Afficher_menu() {

  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);

  ecranOLED.println("Choix du mode:");
  ecranOLED.println();

  ecranOLED.println(condition==0?"> Tension":"  Tension");
  ecranOLED.println(condition==1?"> Resistance":"  Resistance");
  ecranOLED.println(condition==2?"> Angle":"  Angle");
  ecranOLED.println(condition==3?"> Calibration":"  Calibration");
  ecranOLED.println(condition==4?"> Capteur":"  Capteur");

  ecranOLED.display();
}

// ----------------------
// CHOIX CAPTEUR (AJOUT)
// ----------------------
void choix_capteur() {

  if (encoder0Pos % 2 == 0) {
    utiliser_graphite = true;
  } else {
    utiliser_graphite = false;
  }

  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,0);

  ecranOLED.println("Capteur:");

  if (utiliser_graphite) {
    ecranOLED.println("> Graphite");
    ecranOLED.println("  Flex");
  } else {
    ecranOLED.println("  Graphite");
    ecranOLED.println("> Flex");
  }

  ecranOLED.display();
}

// ----------------------
// CALIBRATION (inchangé)
// ----------------------
void calibration() {

  float tension_cible = 2.5;
  float tension_AO = lecture_tension();

  while (abs(tension_AO - tension_cible) > 0.1) {

    ecriture_potentiometre(MCP_WRITE, 128, ssMCPin);
    delay(200);

    tension_AO = lecture_tension();
  }

  Rflex_min = calculateFlexResistance();
}

// ----------------------
// BOUTON
// ----------------------
void checkButton() {

  bool switchState = digitalRead(BUTTON_PIN);

  if (switchState == LOW && switchLastState == HIGH) {
    delay(50);

    if (digitalRead(BUTTON_PIN) == LOW) {
      modeChoisi = !modeChoisi;
    }
  }

  switchLastState = switchState;
}

// ----------------------
// ENCODEUR
// ----------------------
void doEncoder() {
  if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==LOW)
    encoder0Pos++;
  else
    encoder0Pos--;
}

// ----------------------
// POSITION MENU
// ----------------------
void position_encoder(){

  int pos = encoder0Pos % 5;
  if (pos < 0) pos += 5;

  condition = pos;
}

// ----------------------
// VALIDATION (MODIFIÉ)
// ----------------------
void validation_pot(){

  if (modeChoisi == true){

    if (condition == 0)
      affichage_ecran_tension();

    else if (condition == 1)
      affichage_ecran_resistance();

    else if (condition == 2)
      affichage_ecran_angle();

    else if (condition == 3){
      calibration();
      modeChoisi = false;
    }

    else if (condition == 4){
      choix_capteur();
    }
  }
}

// ----------------------
// SETUP
// ----------------------
void setup() {

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ssMCPin, OUTPUT);

  SPI.begin();

  pinMode(encoder0PinA, INPUT_PULLUP);
  pinMode(encoder0PinB, INPUT_PULLUP);
  attachInterrupt(0, doEncoder, RISING);

  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED))
    while(1);

  ecranOLED.setTextColor(SSD1306_WHITE);

  Afficher_menu();
}

// ----------------------
// LOOP
// ----------------------
void loop() {

  checkButton();

  int ancienneCondition = condition;
  position_encoder();

  if (condition != ancienneCondition) {
    Afficher_menu();
  }

  validation_pot();

  delay(100);
}
