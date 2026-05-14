# 2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin
### Introduction

Dans le cadre de la formation en Génie Physique de l'INSA Toulouse, les étudiants de 4ème année doivent réaliser un capteur low-tech à base de graphite et en faire une analyse critique. Le but final étant de répondre à la question suivante:  

<p align="center">
  <strong>Un capteur graphite low-tech peut-il remplacer un flexeur commercial ?</strong>
</p>

Ce capteur graphitte se base sur les travaux de:

  - [1]	K.-H. Müller, J. Herrmann, B. Raguse, G. Baxter, and T. Reda, “Percolation model for electron conduction in films of metal nanoparticles linked by organic molecules,” Phys. Rev. B, vol. 66, no. 7, p. 075417, Aug. 2002, doi: 10.1103/PhysRevB.66.075417.
  
  - [2]	C.-W. Lin, Z. Zhao, J. Kim, and J. Huang, “Pencil Drawn Strain Gauges and Chemiresistors on Paper,” Sci. Rep., vol. 4, no. 1, p. 3812, Jan. 2014, doi: 10.1038/srep03812.
  
  - [3]	K.-H. Müller, J. Herrmann, G. Wei, B. Raguse, and L. Wieczorek, “Electron transport in nanoparticle assemblies,” in 2010 International Conference on Nanoscience and Nanotechnology, Feb. 2010, pp. 316–318. doi: 10.1109/ICONN.2010.6045236.



### Description du projet 
Le capteur de contrainte en graphite repose sur un réseau conducteur de particules de graphite déposées sur un substrat en papier.
Lorsque l'on trace un trait au crayon sur du papier, les particules de graphite adhèrent aux fibres, formant ainsi un mince film conducteur.
Ce film se comporte comme un système granulaire désordonné où la conduction électrique s'effectue par le biais des contacts entre les particules de graphite.
La déformation mécanique affecte ce réseau :
  - Sous contrainte de traction, la distance entre les particules augmente, ce qui réduit les contacts électriques et augmente la résistance.
  - Sous contrainte de compression, les particules sont rapprochées, ce qui améliore la connectivité et diminue la résistance.

La réponse du capteur est donc régie par les variations du réseau et les effets de tunnel entre particules voisines.
En raison de sa structure, le capteur est très sensible aux petites déformations, mais il est également influencé par des paramètres de fabrication tels que la densité du graphite, la dureté du crayon et les conditions environnementales (humidité, température).

**La variation de résistance qui en résulte peut être mesurée à l'aide d'un circuit à transimpédance, afin d'obtenir un signal analogique.**

Ce projet se divise en plusieurs étapes décrites ci-dessous. 



### Livrables
Pour valider ce projet nous devons proposer: 
- Un circuit imprimé (PCB) pour créer les circuits électroniques reliant nos composants
- Un code arduino qui permet de contrôler un ARDUINO afin de mesurer la résistance du capteur, faire nos tests et contrôler des éléments tels que le bluetooth, l'écran ou l'encodeur rotatoire
- Une datasheet présentant les caractéristiques du capteur graphite fabriqué
- Une application ANDROID permettant de recevoir les données par bluetooth et les afficher sur un graphique



### Matériel
Afin de réaliser ce capteur, nous utilisons le matériel suivant: 

- Une carte Arduino UNO
- Un écran OLED (I2C)
- Un potentiomètre numérique MCP41050
- Un encodeur rotatoire
- Un capteur de déformation en papier low tech
- Un module bluetooth


Pour réaliser l'amplificateur transimpédance, nous avons utilisé: 

- Un amplificateur opérationnel LTC1050
- Deux condensateurs de 100nF
- Un condensateur de 1µF
- Deux résistances de 100kΩ
- Deux résistances de 1kΩ
- Une résistance de 10kΩ

### LTspice
La première étape de se projet est de simuler le circuit d'amplification transimpédance du capteur sur LTspice. 
Le  montage est un amplificateur à transimpédance (TIA) basé sur l’AOP LTC1050.

Le principe du montage est de convertir le courant traversant le capteur en une tension exploitable. L’amplificateur opérationnel, configuré en transimpédance, permet d’obtenir une tension de sortie proportionnelle à la valeur de la résistance.

Un filtre passe-bas est ajouté en sortie afin de réduire le bruit et de fournir un signal stable, compatible avec une entrée analogique.

Dans la suite du projet, R2 sera remplacée par un potentiomètre numérique, afin de permettre la calibration de l’Arduino sur une plage de tension comprise entre 0 et 5 V.

<p align="center">
<img width="885" height="337" alt="capteur" src="https://github.com/user-attachments/assets/771fa828-8ae2-4b6b-a42e-7ae31a5f30cd" />
</p>
<p align="center">
  <em>Circuit d'amplification transimpédance sur LTspice</em>
</p>


Pour consulter les différents fichiers, aller à [2.LTSpice](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/2.LTSpice)

### Kicad

Une fois la simulation validée sur LTspice, l'étape suivante consiste à transformer le schéma théorique en un objet physique fonctionnel : le Circuit Imprimé (PCB). 
Pour ce faire, nous utilisons la suite logicielle KiCad.

Cette phase de conception se décompose en trois étapes majeures :

**Saisie schématique et empreinte**

La première étape consiste à reproduire le circuit de l’amplificateur transimpédance.
- Intégration des composants: Nous y plaçons l'AOP LTC1050, les composants passifs (résistances, condensateurs) et les connecteurs nécessaires à l'Arduino Uno, l'écran OLED et l'encodeur rotatif.
- Association des empreintes: Chaque symbole schématique est lié à une empreinte physique réelle. Cette étape est cruciale pour assurer que les composants pourront être soudés correctement sur le cricuit final.


<p align="center">
 <img width="852" height="602" alt="shm" src="https://github.com/user-attachments/assets/e6f07f1a-847f-4858-b204-001d56c1e0d7" />
</p>
<p align="center">
  <em>Visualisation de la schématique du montage sur KiCad</em>
</p>

**Routage et placement**

Le passage au routage permet de définir la disposition physique des éléments sur la carte de cuivre.
- Placement des composants et routages: Nous avons disposé les composants afin de pouvoir router par la suite le plus efficacement possible, sans que les composants et les pistes ne se superposent.
- Plan de masse: Un plan de masse a été intégré pour stabiliser les potentiels et réduire le bruit sur le circuit analogique.

<p align="center">
<img width="755" height="602" alt="pcb" src="https://github.com/user-attachments/assets/fbcbb2ab-9b43-4ea8-ba69-89117455dc1d" />  
</p>
<p align="center">
  <em>Visualisation du routage du montage sur KiCad</em>
</p>

**Visualisation 3D et vérification**

Enfin, KiCad permet une modélisation 3D du projet. Cet outil nous a permis de:
- Vérifier l'encombrement mécanique des composants.
- Valider la cohérence globale avant la fabrication du PCB.

<p align="center">
  <img width="2162" height="1256" alt="image" src="https://github.com/user-attachments/assets/fb70385a-7eba-4ba1-b248-b9d658bd53f9" />
</p>
<p align="center">
  <em>Visualisation 3D du montage global sur KiCad</em>
</p>


Pour consulter les différents fichiers, aller à [3.KiCad](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/3.KiCad)

### Fabrication PCB et soudage

<p align="center">
  <img width="423" height="370" alt="image" src="https://github.com/user-attachments/assets/f2a7d98e-83db-400a-ab24-7c4c89ce1e0a" />
</p>
<p align="center">
  <em>Fichier généré par KiCad pour l'impression du PCB</em>
</p>

Une fois le routage validé sous KiCad et le fichier de perçage généré, nous avons pu imprimer notre circuit électronique (merci Cathy <3). Ensuite, nous avons percé et soudé le montage complet. Avant de passer au code Arduino, nous avons vérifié que tout fonctionnait bien à l'aide d'un multimètre, en effectuant les étapes suivantes:
- Vérification que chaque piste relie bien les bons points du schéma.
- Absence totale de court-circuit entre le 5V et le GND.

### Arduino
Il a ensuite été nécessaire de réaliser un code arduino afin de pouvoir contrôler la carte. Ce code permet notamment d'effectuer les mesures sur le capteur graphite et de contrôler les différents composants tels que le module bluetooth, l'écran OLED et l'encodeur rotatoire. 

**Mesure**

La première partie du code est dédiée à la mesure de la tension aux bornes du capteur. A l'aide d'une fonction, la tension mesurée permet de déduire la résistance du capteur. 

Une fonction de calibration a aussi été créee afin de calibrer la tension aux bornes du capteur. Comme l'arduino UNO permet de délivrer une tension entre 0V et 5V, il est intéressant de régler le potentiomètre numérique afin que la tension aux bornes du capteur lorsque le capteur n'est pas tordu, soit de 2,5V. Ce potentiomètre numérique remplace dans le montage électronique la résistance R2.

Notre montage permet aussi d'utiliser un flexor commercial. Pour cela, une fonction au sein du code permet de mesurer la tension différement et d'en déduire la résistance. La calibration n'est cependant pas nécessaire sur ce capteur. 


<p align="center">
  <img width="123.6" height="281.2" alt="Flexor" src="https://github.com/user-attachments/assets/aa4db0c4-3b84-4619-b7e6-af96fb5bf3de" />
</p>
<p align="center">
  <em>Flexor commercial</em>
</p>

La lecture de tension aux bornes du capteur graphite ou du flexor commercial s'effectue toutes les 100ms. 

**Composants pour l'utilisateur**
<p align="center">
  <img width="852" height="647" alt="Schéma_composants" src="https://github.com/user-attachments/assets/c32d6844-e4c0-4008-b94f-47c23bf2a484" />
</p>
<p align="center">
  <em>Schéma des différents composants</em>
</p>

Le reste du code permet de gérer le reste des composants. Une fonction permet de gérer l'affichage sur l'écran OLED. Sur cet écran, l'affichage d'un menu permet d'intéragir avec l'utilisateur. Une fonction gère l'encodeur rotatoire qui permet à l'utilisateur de se déplacer sur le menu. Enfin, une dernière fonction permet d'envoyer les données mesurées à l'aide du module bluetooth sur un téléphone connecté. 

<p align="center">
  <img width="422" height="234" alt="Menu" src="https://github.com/user-attachments/assets/2f5d6a8d-2e0d-4d96-b905-f57ed69e53e4" />
</p>
<p align="center">
  <em>Menu affiché sur l'écran OLED</em>
</p>

Pour consulter les différents fichiers, aller à [4.Arduino](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/4.Arduino)

### Application
Aller à [5.Application](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/5.Application)

### Banc de tests
Aller à [6.Banc de tests](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/6.Banc%20de%20tests)

Pour cette partie, nous avons utilisé un banc de test conçu par les groupes précédents.
Afin de vérifier les résultats obtenus, nous avons tracé la variation relative de la résistance en fonction de la déformation mécanique. La résistance relative est mesurée pour chaque courbure appliquée au banc de test.
La déformation peut être calculée à partir de la géométrie du dispositif selon la relation suivante :
<img width="195" height="77" alt="deformation" src="https://github.com/user-attachments/assets/36a6837b-f27e-4d52-a047-6e39a1ebdb71" />
 />

 Ensuite, à l’aide du code Arduino, nous avons mesuré la résistance pour quatre types de crayons. Une mesure de référence à plat a d’abord été réalisée, puis des mesures ont été effectuées pour différentes courbures du banc de test, en compression et en tension.
La résistance relative est définie par :
 
<img width="218" height="77" alt="Rrelative" src="https://github.com/user-attachments/assets/a958a8b9-3cbb-4617-836b-28bab4d56c62" />

### Résulat
Nous avons obtenu les courbes suivantes :
<img width="2827" height="1425" alt="Résultat_compression" src="https://github.com/user-attachments/assets/d60e430e-702e-48a4-a128-1575c572bf66" /> 
<img width="3167" height="1595" alt="resultat_flexion" src="https://github.com/user-attachments/assets/cd6efece-dfbd-4445-902f-7a0a0d65fcdf" />

On observe que les résultats obtenus en flexion présentent une relation globalement linéaire entre la déformation et la variation de résistance. Le capteur est donc particulièrement adapté à la mesure de flexion. Cependant la durée de vie du capteur est courte 
les valeurs sont tres vraibale lorsque que celui ci a deja été pliée.
On retrouve bien les résultats attendus concernant la sensibilité du capteur en fonction du grade du crayon. Les crayons "tendres" (ex : 6B, à forte concentration en graphite) présentent une sensibilité plus faible que les crayons plus "durs" (ex : HB). 

Cela s’explique par la structure du réseau conducteur : une plus forte densité de graphite rend le chemin de conduction plus stable, ce qui réduit l’amplitude relative des variations de résistance lors des déformations.
En revanche, les résultats en compression sont moins concluants. Bien que les courbes restent globalement affines, la variation de résistance n’est pas entièrement exploitable : le signal tend à revenir vers la résistance au repos lorsque le capteur est courbé.

Ce comportement peut s’expliquer par un phénomène mécanique : en compression, la couche de graphite entre en contact avec le banc de test, entraînant une perte de matière  et une modification irréversible du réseau conducteur.

Enfin, nous n’avons pas réussi à obtenir de mesures exploitables avec les crayons H et 2H, car le signal électrique généré était trop faible pour être détecté par le système de mesure.

Aller à [7.Datasheet](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/7.Datasheet)

### Conclusion (répondre à la question: est ce que ce capteur peut remplacer un flexeur?)

### Erreur rencontré
RAM -> ecran oled
Perte de contacte soudure
Kicad mauvais branchemement



//Bluetooth
#include <SoftwareSerial.h>
#define rxPin 0 // Correspondant à la broche tx du module bluetooth
#define txPin 1 // Correspondant à la broche Rx du module bluetooth
#define baudrate 9600 
SoftwareSerial mySerial(rxPin ,txPin); //Definition du software serial

// --- FONCTION D'ENVOI BLUETOOTH ---
void envoyerDonneeBluetooth(float valeur) { // <--- AJOUT BLUETOOTH
  // Ton appli attend un "Unsigned 1 Byte Number" (0-255)
  // On mappe la valeur pour qu'elle rentre dans un octet si nécessaire
  // Ici on envoie la valeur brute/4 ou une valeur d'angle
  byte aEnvoyer = (byte)constrain(valeur, 0, 255); 
  mySerial.write(aEnvoyer); 
}


// OLED
void affichage_ecran_tension() {
  float tension = lecture_tension();
  envoyerDonneeBluetooth(tension * 50); // <--- AJOUT BLUETOOTH (ex: 2.5V -> 125)

  void affichage_ecran_resistance() {
  float resistance = calculateFlexResistance();
  // On divise la résistance par un facteur pour que l'appli puisse l'afficher (0-255)
  envoyerDonneeBluetooth(resistance / 1000); // <--- AJOUT BLUETOOTH

  void affichage_ecran_angle() {
  float angle = calculer_angle();
  envoyerDonneeBluetooth(angle); // <--- AJOUT BLUETOOTH (0-90 deg)

  void setup() {
  mySerial.begin(9600); // <--- AJOUT BLUETOOTH




