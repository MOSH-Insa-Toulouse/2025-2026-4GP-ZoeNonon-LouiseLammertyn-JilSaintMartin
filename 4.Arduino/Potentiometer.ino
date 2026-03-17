#include <SPI.h>

// Commande MCP41xxx
#define MCP_WRITE 0b00010001

// Pin CS
const int POT_CS_PIN = 10;

// Position cible (~17 kΩ)
const int POT_POSITION = 128;

// Fonction pour écrire dans le potentiomètre
void setDigitalPot(uint8_t position) {
  position = constrain(position, 0, 255);

  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));

  digitalWrite(POT_CS_PIN, LOW);
  SPI.transfer(MCP_WRITE);
  SPI.transfer(position);
  digitalWrite(POT_CS_PIN, HIGH);

  SPI.endTransaction();
}

void setup() {
  Serial.begin(9600);

  // Initialisation SPI
  pinMode(POT_CS_PIN, OUTPUT);
  digitalWrite(POT_CS_PIN, HIGH);
  SPI.begin();

  Serial.println("Reglage du potentiometre a ~17 kOhm");

  // 👉 Fixe la résistance
  setDigitalPot(POT_POSITION);

  Serial.print("Position envoyee : ");
  Serial.println(POT_POSITION);
}

void loop() {
  // Rien à faire → résistance fixe
}