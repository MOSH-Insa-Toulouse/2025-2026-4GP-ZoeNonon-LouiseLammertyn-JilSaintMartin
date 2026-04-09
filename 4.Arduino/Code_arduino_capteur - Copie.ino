#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

//---Constantes---
const int flexPin = A0; //Pin de sortie de l'amplificateur
float VCC = 5.0;
const float R_DIV = 47000.0; //Résistance pour le pont diviseur de tension
float Rflex_min; //Résistance au repos calibrée

//---Constantes pour le potentiomètre digital---
#define MCP_NOP 0b00000000
#define MCP_WRITE 0b00010001
#define MCP_SHTDWN 0b00100001
const int ssMCPin = 8; 

//---Initialisation de l'écran OLED---
#define nombreDePixelsEnLargeur 128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur 64          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED         -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED     0x3C
Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);

//---Initialisation de l'encodeur rotatoire---
#define encoder0PinA  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define encoder0PinB  4  //DT Output B
#define Switch 5 // Switch connection if available
volatile unsigned int encoder0Pos = 0;


//Fonction qui lit la tension en sortie de l'amplificateur 
float  lecture_tension(){
  float Vadc = analogRead(flexPin);
  return (Vadc*VCC)/ 1023.0;
}

//Fonction qui lit la résistance du capteur à l'aide d'un pont diviseur de tension
float calculateFlexResistance() {
  float Vadc = lecture_tension();
  return R_DIV * (VCC / Vadc - 1.0);
}

void ecriture_potentiometre(uint8_t cmd, uint8_t data, uint8_t ssPin) 
{
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0)); 
  digitalWrite(ssPin, LOW); 
  SPI.transfer(cmd);        
  SPI.transfer(data);       
  digitalWrite(ssPin, HIGH);
  SPI.endTransaction();
}

void calibration(){
  float tension_AO = lecture_tension();
  float tension_cible = 2.5;
  float tolerance = 0.05;
  int pos = 128;
  float diff1;
  float diff2;

  diff1 = abs((tension_AO-tension_cible));

  do{
      Serial.println (tension_AO);
      Serial.println (pos);
      ecriture_potentiometre(MCP_WRITE, pos, ssMCPin);
      delay(100);
      tension_AO = lecture_tension();
      diff2 = abs((tension_AO-tension_cible));

      if (diff2 > diff1) {
        if (pos == 255){
          pos = 1;
        }
        else{
        pos +=1;
        }
      }
      else
      {
        if (pos == 0){
          pos = 254;
        }
        else{
        pos -=1;
        }
      }
      diff1 = abs((tension_AO-tension_cible));

    } while ((tension_AO < 2.4 || tension_AO > 2.6));

  Serial.println ("CALIBRATION: La tension aux bornes de l'amplificateur vaut ");
  Serial.println (tension_AO);
  Rflex_min = calculateFlexResistance();
  Serial.print("Pot calibré pour 0° à la position : ");
  Serial.println(pos);
  Serial.print("Resistance au repos : ");
  Serial.println(Rflex_min);
}

void affichage_ecran(){
  float tension;

  ecranOLED.clearDisplay();
  
  ecranOLED.setTextSize(2); 
  ecranOLED.setCursor(0, 0);
  ecranOLED.println("Tension capteur:"); 
  ecranOLED.println();

  ecranOLED.setTextSize(2); 
  tension=lecture_tension();
  ecranOLED.println(tension);

  ecranOLED.display();
}

void doEncoder() {

  if (digitalRead(encoder0PinB)==HIGH) {
    if (encoder0Pos == 30){
      encoder0Pos = 0;
    }
    else{
    encoder0Pos++;
    }
  } else {
    if (encoder0Pos ==0){
      encoder0Pos = 30;
    }
    else{
    encoder0Pos--;
    }
  }
}

void setup() {
  //---Initialisation du potentiomètre---
  pinMode (ssMCPin, OUTPUT); 
  digitalWrite(ssMCPin, HIGH);
  SPI.begin(); 

  //---Baud rate---
  Serial.begin(9600);

  //---Initialisation de l'écran OLED
  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED)) {
    Serial.println(F("Échec de l'initialisation de l'écran OLED"));
    for(;;);
    }
  ecranOLED.setTextColor(SSD1306_WHITE);
  ecranOLED.setTextWrap(true);

  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,0);
  ecranOLED.setTextSize(1);
  ecranOLED.println("Bienvenue, calibration en cours");
  ecranOLED.display();
  delay(2000);

  //---Initialisation de l'encodeur rotatoire---
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       
  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);     
  attachInterrupt(0, doEncoder, RISING); 

  //---Calibration de la tension aux bornes du capteur
  calibration();
}


void loop() {
  float tension;
  tension=lecture_tension();
  Serial.println(tension);
  affichage_ecran();

  Serial.print("Position:");
  Serial.println (encoder0Pos, DEC);

  delay(500);
}
