//---------------------------------
//---BIBLIOTHEQUES ET CONSTANTES---
//---------------------------------

//---Bibliothèques---
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <SoftwareSerial.h>


//--- Constantes capteur ---
const int Graphite_sensor = A0;  //Pin pour mesurer la tension aux bornes du capteur graphite
const int flex_sensor = A1;   //Pin pour mesurer la tension aux bornes du flex sensor

float VCC = 5.0;    //Tension d'alimentation

const float R_DIV = 47000.0;    //Résistance du pont diviseur de tension (capteur flex)
//Résistance de l'amplificateur transimpédance (cf. schéma LTSpice)
float R3 = 100000;
float R5 = 10000;
float R1 = 100000;

float Rf;   //Résistance globale du capteur graphite

float R_potentiometre;    //Résistance du potentiomètre digital
int pos = 128;    //Position du pas de départ du potentiomètre digital
int  maxPositions = 256;
const long rAB = 92500;   //Résistance du potentiomètre entre les points A et B 
const byte rWiper = 125;    //125 ohms pot wiper resistance

float Rflex_min;
float value_flex;


//---Communication bluetooth---
#define rxPin 9 //Broche rx
#define txPin 10 //Broche tx
#define baudrate 9600
SoftwareSerial mySerial(rxPin ,txPin); 

//--- Potentiomètre digital ---
#define MCP_NOP     0b00000000
#define MCP_WRITE   0b00010001
#define MCP_SHTDWN  0b00100001
const int ssMCPin = 8; 

//--- Écran OLED ---
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_RESET  -1
#define adresseI2CecranOLED     0x3C
Adafruit_SSD1306 ecranOLED(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

//--- Encodeur rotatoire---
int etat_bouton = 0;    //Pour lire l'état du bouton de l'encoder rotatoire
#define BUTTON_PIN  5
#define encoder0PinA  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define encoder0PinB  4  //DT Output B
#define Switch 5 // Switch connection if available
volatile int encoder0Pos = 1;

//--- Menu ---
int condition = 0;    //Pour gérer quel mode du menu utiliser (0=Tension, 1=Résistance, 2=Calibration, 3=Changement de capteur)
bool modeChoisi = false;  //True si on appuie pour valider
bool switchLastState = HIGH;
int x_graph = 0;

//---Changement de capteur---
bool cond_capteur = true; //True = capteur graphite et False = Flex sensor
char capt[30];


//------------------------------
//---DEFINITION DES FONCTIONS---
//------------------------------

//---Lecture tension---
float lecture_tension() {
  float Vadc;
  if (cond_capteur){ //Lecture de la tension pour le capteur graphite
    Vadc = analogRead(Graphite_sensor);
  }
  else{ //Lecture de la tension pour le flex sensor
    Vadc = analogRead(flex_sensor);
  }
  return (Vadc * VCC) / 1023.0;
}

//---Calcul de la résistance---
float calcul_resistance() {
  float tension = lecture_tension();
   if (cond_capteur){ //Pour le capteur graphite
    R_potentiometre = ((rAB * pos) / maxPositions ) + rWiper; //Résistance du potentiomètre
    Rf = ((1 + (R3 / R_potentiometre)) * (VCC / tension)) * R1 - R1 - R5;
    return Rf;
  }
  else{ //Pour le flex sensor
    float Rflex = R_DIV * (VCC / tension - 1.0); 
    return Rflex;
  }
}

//---Variables globales pour les mesures---
float tension = lecture_tension();
float resistance = calcul_resistance();

//---Changement de capteur---
void changement_capteur(){
  if (cond_capteur == true){
    strcpy(capt, "Capteur graphite");
  }
  else{
  
    strcpy(capt, "Flex sensor");
}
}

//---Ecriture dans le potentiomètre digital---
void ecriture_potentiometre(uint8_t cmd, uint8_t data, uint8_t ssPin) {
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  digitalWrite(ssPin, LOW);
  SPI.transfer(cmd);
  SPI.transfer(data);
  digitalWrite(ssPin, HIGH);
  SPI.endTransaction();
}

//---Calibration---
void calibration() {
  float tension_cible = 2.5;
  float diff1, diff2;

  diff1 = abs(tension - tension_cible);
  do {
    //Affichage sur l'écran de l'ordinateur des paramètres de calibration
    Serial.println(F("Tension mesurée"));
    Serial.println(tension);
    Serial.println(F("Position du potentiomètre"));
    Serial.println(pos);
    ecriture_potentiometre(MCP_WRITE, pos, ssMCPin);
    delay(200);
    tension = lecture_tension();
    diff2 = abs(tension - tension_cible);

    if (diff2 > diff1) {
      if (pos == 255) pos = 1;
      else pos++;
    } else {
      if (pos == 0) pos = 255;
      else pos--;
    }
    diff1 = abs(tension - tension_cible);

  } while ((tension < 2.4 || tension > 2.6));
  //On calcule la différence entre la tension mesurée et la tension initiale avant er après
  //le changement de position. On les compare et on incrémente le pas en fonction.

  //Affichage sur l'écran de l'oridnateur des paramètres retenus
  Serial.print(F("Tension calibrée: "));
  Serial.println(tension);
  resistance = calcul_resistance();
  Serial.print(F("Pot calibré position: "));
  Serial.println(pos);
  Serial.print(F("Resistance au repos: "));
  Serial.println(resistance);
}

//_______________________________
//---Ecriture sur l'écran OLED---

//---Message de bienvenue---
void affichage_initialisation() {
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,0);
  ecranOLED.setTextSize(1);
  ecranOLED.println(F("Bienvenue!"));
  ecranOLED.display();
}

//---Message de calibration---
void affichage_calibration() {
  if (cond_capteur){
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,0);
  ecranOLED.setTextSize(1);
  ecranOLED.println(F("Calibration en cours..."));
  ecranOLED.display();
  calibration();
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Calibration terminee!"));
  ecranOLED.println();
  ecranOLED.println(F("Tension calibree: "));
  ecranOLED.print(tension);
  ecranOLED.println(F("Position potentiometre: "));
  ecranOLED.print(pos);
  ecranOLED.println(F("Resistance au repos: "));
  ecranOLED.print(Rf);
    
  }
  else{
    ecranOLED.clearDisplay();
    ecranOLED.setCursor(0,0);
    ecranOLED.setTextSize(1);
    ecranOLED.println(F("La calibration n'est pas necessaire pour le flex sensor."));
  }
  ecranOLED.display();
}

//---Affichage de la tension---
void affichage_ecran_tension() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Tension capteur:"));
  ecranOLED.println();
  ecranOLED.println(tension);
  ecranOLED.println();
  ecranOLED.println("V");

  //Fonction pour afficher en bas à droite de l'écran le capteur utilisé
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

//---Affichage de la résistance---
void affichage_ecran_resistance() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Resistance capteur:"));
  ecranOLED.println();
  ecranOLED.println(resistance);
  ecranOLED.println();
  ecranOLED.println("ohm");

  //Fonction pour afficher en bas à droite de l'écran le capteur utilisé
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

//---Affichage du choix du capteur---
void affichage_capteur() {
  changement_capteur();
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Capteur:"));
  ecranOLED.println();
  ecranOLED.println(capt);
  ecranOLED.display();
}

//--- Affichage du menu---
void affichage_menu() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Choix du mode:"));
  ecranOLED.println();

  if(condition == 0) ecranOLED.println(F("> Tension"));
  else ecranOLED.println(F("  Tension"));

  if(condition == 1) ecranOLED.println(F("> Resistance"));
  else ecranOLED.println(F("  Resistance"));

  if(condition == 2) ecranOLED.println(F("> Calibration"));
  else ecranOLED.println(F("  Calibration"));

  if(condition == 3) ecranOLED.println(F("> Capteur"));
  else ecranOLED.println(F("  Capteur"));

  //Fonction pour afficher en bas à droite de l'écran le capteur utilisé
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

//---Bouton de l'encodeur---
void checkButton() {
  bool switchState = digitalRead(BUTTON_PIN);

  if (switchState == LOW && switchLastState == HIGH) {
    delay(50); 

    if (digitalRead(BUTTON_PIN) == LOW) {
      if (modeChoisi == false) {
        modeChoisi = true;
      } else {
        modeChoisi = false;
        affichage_menu();
      }
    }
  }
  switchLastState = switchState;
}

//---Communication bluetooth---
void setup_bluetooth(){
  pinMode(rxPin,INPUT);
  pinMode(txPin,OUTPUT);
    
  mySerial.begin(baudrate);
}

//---Boucle de communication bluetooth---
void loop_bluetooth(float variable){
  if (isinf(variable)) {
  mySerial.print("inf"); // Envoie une valeur plafond au lieu de "inf"
  } else {
  mySerial.print(variable);
  }
  mySerial.print(",");
  mySerial.print(condition);
  mySerial.print(",");
  mySerial.print(cond_capteur ? 1 : 0); //envoie 1 si true et 0 si false
  mySerial.print("\n");
}

//---Encodeur rotatoire---
void encodeur() {
  if (modeChoisi == true) {

    if (condition == 0) { //Mesure de tension
      do{
      tension = lecture_tension();
      affichage_ecran_tension();
      loop_bluetooth(tension);
      delay(50);
      checkButton();
      position_encodeur(); 
      if (modeChoisi == false) break;
      }while(condition == 0);
      
    }
    else if (condition == 1) { //Mesure de résistance
      do{
      resistance = calcul_resistance();
      affichage_ecran_resistance();
      loop_bluetooth(resistance);
      delay(50);
      checkButton();
      position_encodeur(); 
      if (modeChoisi == false) break;
      }while(condition == 1);
    }
    else if (condition == 2) { //Calibration
      affichage_calibration();
      do{
      checkButton();
      if (modeChoisi == false) break;
      }while(condition == 2);
    }
    else if (condition == 3){ //Changement de capteur
      if (cond_capteur==true){
        cond_capteur = false;
      }
      else {
        cond_capteur = true;
      }
      do{
      affichage_capteur();
      checkButton();
      if (modeChoisi == false) break;
      }while(condition == 3);
    }
}
}

//---Setup de l'encodeur rotatoire---
void encodeur_setup(){
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       

  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);       

  attachInterrupt(0, do_encodeur, RISING); 
}

//---Incrémentation du pas de l'encodeur---
void do_encodeur() {
  if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==HIGH) {
    encoder0Pos++;
  } else if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==LOW) {
    encoder0Pos--;
  }

  //Si on a fait un tour complet, le compteur repart à 0
  if (encoder0Pos == 30){
    encoder0Pos = 0;
  }
  if (encoder0Pos == -1){
    encoder0Pos = 30;
  }
}

//---lecture la position de l'encodeur---
void position_encodeur(){
  if ((encoder0Pos<=7 && 0 <encoder0Pos)){
    condition = 0; //Tension
  }
  else if ((encoder0Pos<=14 && 7<encoder0Pos)){
    condition = 1; //Résistance
  }
  
  else if ((encoder0Pos<=21 && 14<encoder0Pos)){
    condition = 2; //Calibration
  }
   else if ((encoder0Pos<=30 && 21<encoder0Pos)){
    condition = 3; //Capteur
  }
}

//---Setup---
void setup() {
  //Initialisation du potentiomètre
  pinMode(ssMCPin, OUTPUT); 
  digitalWrite(ssMCPin, HIGH);
  SPI.begin(); 

  //Initialisation de l'encodeur
  encodeur_setup();

  //Baud rate
  Serial.begin(9600);

  //Initialisation de l'écran OLED
  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED))
    while(1);  
  ecranOLED.setTextColor(SSD1306_WHITE);
  ecranOLED.setTextWrap(true);
  affichage_initialisation();
  delay(2000);

  //Calibration du capteur graphite
  affichage_calibration();

  //Initialisation du bluetooth
  setup_bluetooth();
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  affichage_menu();
  Serial.println("Calibration terminée");
}

// --- Loop ---
void loop() {
  //Lecture de l'état du bouton de l'encodeur rotatoire
  checkButton();
  
  int ancienneCondition = condition;
  position_encodeur();

  //Affichage du menu
  if (condition != ancienneCondition) {
    affichage_menu();
  }

  encodeur();
  delay(100);
}
