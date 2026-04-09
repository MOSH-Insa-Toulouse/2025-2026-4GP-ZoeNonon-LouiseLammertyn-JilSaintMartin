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

Un amplificateur opérationnel LTC1050
Deux condensateurs de 100nF
Un condensateur de 1uF
Deux résistances de 100k
Deux résistances de 1k
Une résistance de 10k

### LTspice
La première étape de se projet simuler le circuit de capteur sur LT spice. 
Dans un premiere temps nous avons simuluer le à transimpédance ci dessous afin de pourvoir lire la résitance : 
<img width="885" height="337" alt="capteur" src="https://github.com/user-attachments/assets/771fa828-8ae2-4b6b-a42e-7ae31a5f30cd" />


Aller à [2.LTSpice](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/2.LTSpice)

### Kicad
Aller à [3.KiCad](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/3.KiCad)

### Fabrication PCB

### Arduino
Aller à [4.Arduino](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/4.Arduino)

### Application
Aller à [5.Application](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/5.Application)

### Banc de tests
Aller à [6.Banc de tests](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/6.Banc%20de%20tests)

### Datasheet
Aller à [7.Datasheet](https://github.com/MOSH-Insa-Toulouse/2025-2026-4GP-ZoeNonon-LouiseLammertyn-JilSaintMartin/tree/main/7.Datasheet)

### Conclusion (répondre à la question: est ce que ce capteur peut remplacer un flexeur?)

### Prise de notes
Ecran OLED fonctionne, utiliser le programme OLED dans folder Arduino
