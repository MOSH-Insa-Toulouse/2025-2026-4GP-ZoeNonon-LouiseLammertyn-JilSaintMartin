# 2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin
### Introduction

Dans le cadre de la formation en Génie Physique de l'INSA Toulouse, les étudiants de 4ème année doivent réaliser un capteur low-tech à base de graphite et en faire une analyse critique. Le but final étant de répondre à la question suivante:  

*Ce capteur peut-il remplacer un flexeur commercial?*

Ce capteur low-tech se base sur les travaux de 


Description du projet 
Le capteur de contrainte en graphite repose sur un réseau conducteur de particules de graphite déposées sur un substrat en papier.
Lorsqu'on trace un trait au crayon sur du papier, les particules de graphite adhèrent aux fibres, formant ainsi un mince film conducteur.
Ce film se comporte comme un système granulaire désordonné où la conduction électrique s'effectue par le biais des contacts entre les particules de graphite.
La déformation mécanique affecte ce réseau :
  - Sous contrainte de traction, la distance entre les particules augmente, ce qui réduit les contacts électriques et augmente la résistance.
  - Sous contrainte de compression, les particules sont rapprochées, ce qui améliore la connectivité et diminue la résistance.

La réponse du capteur est donc régie par les variations du réseau  et les effets de tunnel entre particules voisines.
En raison de sa structure, le capteur est très sensible aux petites déformations, mais il est également influencé par des paramètres de fabrication tels que la densité du graphite, la dureté du crayon et les conditions environnementales (humidité, température).

La variation de résistance qui en résulte peut être mesurée à l'aide d'un circuit à transimpédance, afin d'obtenir un signal analogique.


Ce projet se divise en plusieurs étapes décrites ci-dessous. 

### Livrables
Pour valider ce projet nous devons proposé : 
- Un pcb pour faire les circuits éléectroniques de tous nos composants
- Un code arduino, qui permet de mesure la resistance et de faire nos tests
- un datasheet, présentant les caractérique de notre capteur graphique 
- une interface python pour visualiser en direct nos résultats

### Matériel
Afin de valide l'utilisation de ce capteur, nos utilisons un PCB composé : 

Une carte Arduino UNO
Un écran OLED (I2C)
Un potentiomètre numérique MCP41050
Un encodeur rotatif
Un capteur de déformation en papier low tech 


Pour réaliser l'amplificateur transimpédance, nous avons utilisé

- Un amplificateur opérationnel LTC1050
- Deux condensateurs de 100nF
- Un condensateur de 1uF
- Deux résistances de 100k
- Deux résistances de 1k
- Une résistance de 10k

### LTspice
La première étape de se projet simuler le circuit de capteur sur LT spice. 
Le  montage est un amplificateur à transimpédance (TIA) basé sur l’AOP LTC1050.

Le principe du montage est de convertir le courant traversant le capteur en une tension exploitable. L’amplificateur opérationnel, configuré en transimpédance, permet d’obtenir une tension de sortie proportionnelle à la valeur de la résistance.

Un filtre passe-bas est ajouté en sortie afin de réduire le bruit et de fournir un signal stable, compatible avec une entrée analogique.

Dans la suite du projet, R2 sera remplacée par un potentiomètre numérique, afin de permettre la calibration de l’Arduino sur une plage de tension comprise entre 0 et 5 V.
<img width="885" height="337" alt="capteur" src="https://github.com/user-attachments/assets/771fa828-8ae2-4b6b-a42e-7ae31a5f30cd" />



Aller à [2.LTSpice](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/2.LTSpice)

### Kicad

Une fois la simulation validée sur LTspice, l'étape suivante consiste à transformer le schéma théorique en un objet physique fonctionnel : le Circuit Imprimé (PCB). 
Pour ce faire, nous utilisons la suite logicielle  KiCad.

Cette phase de conception se décompose en trois étapes majeures :

Saisie Schématique et Empreinte
La première étape consiste à reproduire le circuit de l’amplificateur transimpédance.
- Intégration des composants : Nous y plaçons l'AOP LTC1050, les composants passifs (résistances, condensateurs) et les connecteurs nécessaires à l'Arduino Uno, l'écran OLED et l'encodeur rotatif.
- Association des empreintes : Chaque symbole schématique est lié à une empreinte physique réelle. Cette étape est cruciale pour assurer que les composants pourront être soudés correctement sur le cricuit final.

<img width="852" height="602" alt="shm" src="https://github.com/user-attachments/assets/e6f07f1a-847f-4858-b204-001d56c1e0d7" />

 Routage et Placement 
Le passage au routage permet de définir la disposition physique des éléments sur la carte de cuivre.
Placement des composants et routages : Nous avons disposé les composants afin de pouvoir router par la suite le plus afficacement possible, sans que les composants et les pistes ce superposent.
.
Plan de masse : Un plan de masse a été intégré pour stabiliser les potentiels et réduire le bruit  sur le circuit analogique.

<img width="755" height="602" alt="pcb" src="https://github.com/user-attachments/assets/fbcbb2ab-9b43-4ea8-ba69-89117455dc1d" />

Visualisation 3D et Vérification
Enfin, KiCad permet une modélisation 3D du projet. Cet outil nous a permis de :
Vérifier l'encombrement mécanique des composants.
Valider la cohérence globale avant la fabrication du PCB.

<img width="2162" height="1256" alt="image" src="https://github.com/user-attachments/assets/fb70385a-7eba-4ba1-b248-b9d658bd53f9" />



Aller à [3.KiCad](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/3.KiCad)

### Fabrication PCB - Souadage
<img width="423" height="370" alt="image" src="https://github.com/user-attachments/assets/f2a7d98e-83db-400a-ab24-7c4c89ce1e0a" />

Une fois le routage validé sous KiCad et le fichier de perçage généré, nous avons pu imprimer notre circuit électronique (merci Cathy). Ensuite, nous avons percé et soudé le montage complet. Avant de passer au code Arduino, nous avons vérifié que tout fonctionnait bien à l'aide d'un multimètre :
- Vérification que chaque piste relie bien les bons points du schéma.

- Absence totale de court-circuit entre le 5V et le GND.
- 
<img width="1500" height="1256" alt="image" src="https://github.com/user-attachments/assets/c728011d-2ce3-461f-a145-05f94b24305f" />


![IMG_3698](https://github.com/user-attachments/assets/c728011d-2ce3-461f-a145-05f94b24305f)
![IMG_3697](https://github.com/user-attachments/assets/a1ab8def-40c3-416e-80f5-0c1f6ff07c94)




### Arduino
Aller à [4.Arduino](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/4.Arduino)
Ensuite nous avons mis en place le code arduino, pour ce faire nous avons utilisée fait : 
- un fonction de calibration
- une fonction de mesure de la resitance
- une fonction pour gérer l'encododeur rotatoire pour gerer un menu affiché sur l'écran
- 
### Application
Aller à [5.Application](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/5.Application)

### Banc de tests
Aller à [6.Banc de tests](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/6.Banc%20de%20tests)

### Datasheet
Aller à [7.Datasheet](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/7.Datasheet)

### Conclusion (répondre à la question: est ce que ce capteur peut remplacer un flexeur?)

### Prise de notes
Ecran OLED fonctionne, utiliser le programme OLED dans folder Arduino
