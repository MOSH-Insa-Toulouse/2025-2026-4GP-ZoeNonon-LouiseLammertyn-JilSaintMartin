#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// --- Constantes Capteur ---
const int flexPin = A0;
float VCC = 5.0;
const float R_DIV = 47000.0;
float R3 = 100000;
float R5 = 10000;
float R1 = 100000;
float R_potentiometre;
float Rf; //Résistance globale 
int pos = 128;
float Rflex_min;


int condition = 0;
int etat_bouton = 0;
int fin_calib = 0;

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
#define encoder0PinA  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define encoder0PinB  4  //DT Output B
#define Switch 5 // Switch connection if available
volatile int encoder0Pos = 1;

// --- Menu ---
int menuIndex = 0;        // 0=Calibration, 1=Mesure, 2=mesure2
bool modeChoisi = false;  // true si appui pour valider
bool switchLastState = HIGH;

// --- Fonctions Capteur ---
float lecture_tension() {
  float Vadc = analogRead(flexPin);
  return (Vadc * VCC) / 1023.0;
}
float tension_AO;

float calculateFlexResistance() {
  float Vadc = lecture_tension();
  float R_potentiometre = R_DIV * (VCC / Vadc - 1.0);

  Rf = ((1+(R3 / R_potentiometre))*(VCC/Vadc))*R1-R1-R5;
  return Rf;
}

void ecriture_potentiometre(uint8_t cmd, uint8_t data, uint8_t ssPin) {
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  digitalWrite(ssPin, LOW);
  SPI.transfer(cmd);
  SPI.transfer(data);
  digitalWrite(ssPin, HIGH);
  SPI.endTransaction();
}

void angle

// --- OLED ---
void init_oled() {
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,0);
  ecranOLED.setTextSize(1);
  ecranOLED.println("Bienvenue!");
  ecranOLED.display();
}

void oled_calibration() {
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,0);
  ecranOLED.setTextSize(1);
  ecranOLED.println("Calibration en cours...");
  /*
  if (fin_calib == 1){
    ecranOLED.clearDisplay();
    ecranOLED.setCursor(0,0);
    ecranOLED.println("Calibration terminee!");
    ecranOLED.println();
    ecranOLED.println("Tension calibree: ");
    ecranOLED.println(tension_AO);
    Rflex_min = calculateFlexResistance();
    ecranOLED.println("Potentiometre calibre position: ");
    ecranOLED.println(pos);
    ecranOLED.println("Resistance au repos: ");
    ecranOLED.println(Rf);
    
  }*/
  ecranOLED.display();
}

void affichage_ecran_tension() {
  float tension = lecture_tension();
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2);
  ecranOLED.setCursor(0,0);
  ecranOLED.println("Tension capteur:");
  ecranOLED.println();
  ecranOLED.println(tension);
  ecranOLED.display();
}

void affichage_ecran_resistance() {
  float resistance = calculateFlexResistance();
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2);
  ecranOLED.setCursor(0,0);
  ecranOLED.println("Résistance capteur:");
  ecranOLED.println();
  ecranOLED.println(resistance);
  ecranOLED.display();
}

void affichage_ecran_angle() {
  float angle = 0;
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2);
  ecranOLED.setCursor(0,0);
  ecranOLED.println("Angle capteur:");
  ecranOLED.println();
  ecranOLED.println(angle);
  ecranOLED.display();
}

// --- Affichage menu ---
void Afficher_menu() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println("Choix du mode:");
  ecranOLED.println();
  if(condition == 0) ecranOLED.println("> Tension");
  else ecranOLED.println("  Tension");
  if(condition == 1) ecranOLED.println("> Resistance");
  else ecranOLED.println("  Resistance");
  if(condition == 2) ecranOLED.println("> Angle");
  else ecranOLED.println("  Angle");
  if(condition == 3) ecranOLED.println("> Calibration");
  else ecranOLED.println("  Calibration");
  ecranOLED.println();
  ecranOLED.println("Appuyez pour valider");
  ecranOLED.display();
}

// --- Calibration ---
void calibration() {
  float tension_cible = 2.5;
  float diff1, diff2;

  tension_AO = lecture_tension();

  fin_calib = 0;

  diff1 = abs(tension_AO - tension_cible);

  do {
    Serial.println("Tension mesurée");
    Serial.println(tension_AO);
    Serial.println("Position du potentiomètre");
    Serial.println(pos);
    ecriture_potentiometre(MCP_WRITE, pos, ssMCPin);
    delay(500);
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

  fin_calib = 1;

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

void validation_pot(){
   if (modeChoisi == true){
    if (condition == 0){
      //fonction  tension
      affichage_ecran_tension();
    }
    if (condition == 1){
      // fonction resitance
      affichage_ecran_resistance();
    }
    if (condition == 2){
      //fonction angle
      affichage_ecran_angle();
    }
    if (condition == 3){
      //fonction angle
      oled_calibration();
      calibration();
      oled_calibration();
    }
    modeChoisi = false;
  }
}

void encoder_setup(){
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       

  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);       

  attachInterrupt(0, doEncoder, RISING); 
}

void doEncoder() {
  if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==HIGH) {
    encoder0Pos++;
  } else if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==LOW) {
    encoder0Pos--;
  }
  if (encoder0Pos == 29){
    encoder0Pos = 0;
  }
  if (encoder0Pos == -1){
    encoder0Pos = 28;
  }
  //Serial.println (encoder0Pos, DEC);  //Angle = (360 / Encoder_Resolution) * encoder0Pos
}

void position_encoder(){
  if ((encoder0Pos<=7 && 0 <encoder0Pos)){
    condition = 0;
  }
  else if ((encoder0Pos<=14 && 7<encoder0Pos)){
    condition = 1;
  }
  else if ((encoder0Pos<=21 && 14<encoder0Pos)){
    condition = 2;
  }
  else if ((encoder0Pos<=28 && 21<encoder0Pos)){
    condition = 3;
  }
  //Serial.println("Condition:");
  //Serial.println(condition);
}


// --- Setup ---
void setup() {
  pinMode(ssMCPin, OUTPUT); 
  digitalWrite(ssMCPin, HIGH);
  SPI.begin(); 

  encoder_setup();

  Serial.begin(9600);

  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("Échec OLED"));
    for(;;);
  }
  ecranOLED.setTextColor(SSD1306_WHITE);
  ecranOLED.setTextWrap(true);

  init_oled();
  delay(1000);
  oled_calibration();
  //calibration();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Afficher_menu();
  Serial.println("Calibration terminée");
}

// --- Loop ---
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
