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
float R_potentiometre;
float Rf; //Résistance globale 
int pos = 128;
float Rflex_min;
float value_flex;

//---Communication bluetooth---
#include <SoftwareSerial.h>
#define rxPin 9 //Broche 9
#define txPin 10 //Broche 10 
#define baudrate 9600
SoftwareSerial mySerial(rxPin ,txPin); 

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
#define adresseI2CecranOLED     0x3C
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
int x_graph = 0;

// choix du capteur
bool cond_capteur = true; //True = capteur graphite et False = Flex sensor
char capt[30];

// -Lecture tension 
float lecture_tension() {
  float Vadc;
  if (cond_capteur){
    Vadc = analogRead(Graphite_sensor);
  }
  else{
    Vadc = analogRead(flex_sensor);
  }
  return (Vadc * VCC) / 1023.0;
}



float calculateFlexResistance() {
  float Vadc = lecture_tension();

  // calcul de la resistance en fonction du capteur choisi
   if (cond_capteur){
    Rf = ((1 + (R3 / R_potentiometre)) * (VCC / Vadc)) * R1 - R1 - R5;
    return Rf;
  }
  else{
    float Rflex = R_DIV * (VCC / Vadc - 1.0); // pour le flex sensor
    return Rflex;
  }
  
}
// angle

float calculer_angle(){

  if (cond_capteur) {
  return 0;
  }
  else{
  float current_R = calculateFlexResistance();

  float r_flat = Rflex_min;
  float r_90 = Rflex_min * 2.5;

  float angle = map(current_R, r_flat, r_90, 0.0, 90.0);

  if (angle < 0) angle = 0;
  if (angle > 90) angle = 90;

  return angle;}
}

//---Variables globales---
float tension = lecture_tension();
float resistance = calculateFlexResistance();
float angle = calculer_angle();

void changement_capteur(){
  if (cond_capteur == true){
    strcpy(capt, "Capteur graphite");
  }
  else{
  
    strcpy(capt, "Flex sensor");
   
}
}

// potentiometre
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
  ecranOLED.println(F("Bienvenue!"));
  ecranOLED.display();
}

void oled_calibration() {
  if (cond_capteur = true){
  float tension_AO;
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,0);
  ecranOLED.setTextSize(1);
  ecranOLED.println(F("Calibration en cours..."));
  
  if (fin_calib == 1){
    ecranOLED.clearDisplay();
    ecranOLED.setCursor(0,0);
    ecranOLED.println(F("Calibration terminee!"));
    ecranOLED.println();
    ecranOLED.println(F("Tension calibree: "));
    tension_AO= lecture_tension();
    ecranOLED.print(tension_AO);
    Rflex_min = calculateFlexResistance();
    ecranOLED.println(F("Position potentiometre: "));
    ecranOLED.print(pos);
    ecranOLED.println(F("Resistance au repos: "));
    ecranOLED.print(Rf);
    
  }}
  else{
    ecranOLED.clearDisplay();
    ecranOLED.setCursor(0,0);
    ecranOLED.setTextSize(1);
    ecranOLED.println(F("La calibration n'est pas nécessaire pour le flex sensor."));
  }
  ecranOLED.display();
}

void affichage_ecran_tension() {
  
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Tension capteur:"));
  ecranOLED.println();
  ecranOLED.println(tension);

  if (cond_capteur == true){
   ecranOLED.setCursor(32, 56);
   ecranOLED.println(F("Capteur graphite"));
  }
  else{
    ecranOLED.setCursor(62, 56);
    ecranOLED.println(F("Flex sensor"));
  }

  ecranOLED.display();
}

void affichage_ecran_resistance() {
  
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Résistance capteur:"));
  ecranOLED.println();
  ecranOLED.println(resistance);

  if (cond_capteur == true){
   ecranOLED.setCursor(32, 56);
   ecranOLED.println(F("Capteur graphite"));
  }
  else{
    ecranOLED.setCursor(62, 56);
    ecranOLED.println(F("Flex sensor"));
  }

  ecranOLED.display();
}

void affichage_ecran_angle() {
  
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Angle capteur:"));
  ecranOLED.println();
  ecranOLED.println(angle);

  if (cond_capteur == true){
   ecranOLED.setCursor(32, 56);
   ecranOLED.println(F("Capteur graphite"));
  }
  else{
    ecranOLED.setCursor(62, 56);
    ecranOLED.println(F("Flex sensor"));
  }
  
  ecranOLED.display();
}

void afficher_capteur() {
  changement_capteur();
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Capteur:"));
  ecranOLED.println();
  ecranOLED.println(capt);
  ecranOLED.display();
}

// --- Affichage menu ---
void Afficher_menu() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Choix du mode:"));
  ecranOLED.println();

  if(condition == 0) ecranOLED.println(F("> Tension"));
  else ecranOLED.println(F("  Tension"));

  if(condition == 1) ecranOLED.println(F("> Resistance"));
  else ecranOLED.println(F("  Resistance"));

  if(condition == 2) ecranOLED.println(F("> Angle"));
  else ecranOLED.println(F("  Angle"));

  if(condition == 3) ecranOLED.println(F("> Calibration"));
  else ecranOLED.println(F("  Calibration"));

  if(condition == 4) ecranOLED.println(F("> Capteur"));
  else ecranOLED.println(F("  Capteur"));



  if (cond_capteur == true){
   ecranOLED.setCursor(32, 56);
   ecranOLED.println(F("Capteur graphite"));
  }
  else{
    ecranOLED.setCursor(62, 56);
    ecranOLED.println(F("Flex sensor"));
  }

  ecranOLED.display();
}

// --- Calibration ---
void calibration() {
  float tension_cible = 2.5;
  float diff1, diff2;
  float tension_AO=lecture_tension();

  fin_calib = 0;

  diff1 = abs(tension_AO - tension_cible);

  do {
    
    Serial.println(F("Tension mesurée"));
    Serial.println(tension_AO);
    Serial.println(F("Position du potentiomètre"));
    Serial.println(pos);
    ecriture_potentiometre(MCP_WRITE, pos, ssMCPin);
    delay(200);
    tension_AO = lecture_tension();
    diff2 = abs(tension_AO - tension_cible);

    if (diff2 > diff1) {
      if (pos == 255) pos = 1;
      else pos++;
    } else {
      if (pos == 0) pos = 255;
      else pos--;
    }
    diff1 = abs(tension_AO - tension_cible);

  } while ((tension_AO < 2.4 || tension_AO > 2.6));

  fin_calib = 1;

  Serial.print(F("Tension calibrée: "));
  Serial.println(tension_AO);
  Rflex_min = calculateFlexResistance();
  Serial.print(F("Pot calibré position: "));
  Serial.println(pos);
  Serial.print(F("Resistance au repos: "));
  Serial.println(Rflex_min);
}



// --- Bouton du potentiometre  ---
void checkButton() {
  bool switchState = digitalRead(BUTTON_PIN);
  

  if (switchState == LOW && switchLastState == HIGH) {
    delay(50); // debounce

    if (digitalRead(BUTTON_PIN) == LOW) {

      if (modeChoisi == false) {
        modeChoisi = true;
      } else {
        modeChoisi = false;
        Afficher_menu();
      }

    }
  }

  switchLastState = switchState;
}



void validation_pot() {
  if (modeChoisi == true) {

    if (condition == 0) {
      do{
      tension = lecture_tension();
      affichage_ecran_tension();
      loop_bluetooth(tension);
      delay(50);
      checkButton();
      position_encoder(); 
      if (modeChoisi == false) break;
      }while(condition == 0);
      
    }
    else if (condition == 1) {
      do{
      resistance = calculateFlexResistance();
      affichage_ecran_resistance();
      loop_bluetooth(resistance);
      delay(50);
      checkButton();
      position_encoder(); 
      if (modeChoisi == false) break;
      }while(condition == 1);
    }
    else if (condition == 2) {
      do{
      angle = calculer_angle();
      affichage_ecran_angle();
      loop_bluetooth(angle);
      delay(50);
      checkButton();
      position_encoder(); 
      if (modeChoisi == false) break;
      }while(condition == 2);
    }
    else if (condition == 3) {
      do{
      oled_calibration();
      calibration();
      oled_calibration();
      checkButton();
      if (modeChoisi == false) break;
      }while(condition == 3);
    }
    else if (condition == 4){
      if (cond_capteur==true){
        cond_capteur = false;
      }
      else {
        cond_capteur = true;
      }
      do{
      afficher_capteur();
      checkButton();
      if (modeChoisi == false) break;
      }while(condition == 4);
   
      }
}
}
    
    //if (condition != 0 && condition != 1 && condition != 2 && condition != 3) {
      //modeChoisi = false;
      //Afficher_menu();
   // }

// plus utilisé

void validation_pot2(){
   if (modeChoisi == true){
    if (condition == 0){
      //fonction  tension
      do{
      affichage_ecran_tension();
      }while(condition == 0);
    }
    if (condition == 1){
      // fonction resitance
      
      do{
      affichage_ecran_resistance();
      }while(condition == 1);
    }
    if (condition == 2){
      //fonction angle
      do{
      affichage_ecran_angle();
      }while(condition == 2);
    }
    if (condition == 3){
      //fonction calibration
      do{
      oled_calibration();
      calibration();
      oled_calibration();
      }while(condition == 3);
      
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
  if ((encoder0Pos<=6 && 0 <encoder0Pos)){
    condition = 0;
  }
  else if ((encoder0Pos<=12 && 6<encoder0Pos)){
    condition = 1;
  }
  else if ((encoder0Pos<=20 && 12<encoder0Pos)){
    condition = 2;
  }
  else if ((encoder0Pos<=26 && 20<encoder0Pos)){
    condition = 3; //calibration
  }
   else if ((encoder0Pos<=60 && 26<encoder0Pos)){
    condition = 4; //capteur
  } 
  //Serial.println("Condition:");
  //Serial.println(condition);
}

//---Communication bluetooth---
void setup_bluetooth(){
  pinMode(rxPin,INPUT);
  pinMode(txPin,OUTPUT);
    
  mySerial.begin(baudrate);
}



void loop_bluetooth(float variable){
  
  mySerial.print(variable);
  mySerial.print(",");
  mySerial.print(condition);
  mySerial.print(",");
  mySerial.print(cond_capteur ? 1 : 0); //envoie 1 si true et 0 si false
  mySerial.print("\n");
}


// --- Setup ---
void setup() {
  pinMode(ssMCPin, OUTPUT); 
  digitalWrite(ssMCPin, HIGH);
  SPI.begin(); 

  encoder_setup();

  Serial.begin(9600);

  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED))
    while(1);  
  ecranOLED.setTextColor(SSD1306_WHITE);
  ecranOLED.setTextWrap(true);

  init_oled();
  delay(1000);
  oled_calibration();
  //calibration();
  setup_bluetooth();
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