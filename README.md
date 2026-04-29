# Oxymètre de pouls
> À partir de la version 1.4.0, le projet migre vers STM32 afin de permettre l’intégration de la SpO2. L’Arduino Uno reste une base fonctionnelle pour la mesure BPM, mais sa mémoire SRAM est trop limitée pour l’intégration complète BPM + SpO2.

## Description

Ce projet implémente un oxymètre de pouls embarqué capable de mesurer :
- la fréquence cardiaque (BPM)
- la saturation en oxygène du sang (SpO₂)

L’objectif est d’exploiter pleinement le potentiel du capteur MAX30102 en améliorant la bibliothèque SparkFun d'origne.

Le travail actuel se concentre principalement sur la fiabilité de la mesure de la fréquence cardiaque (BPM), avec une gestion plus robuste du signal et des conditions réelles d’utilisation.

## État actuel du projet
Fonctionnel pour :
- Détection du doigt
- Lecture du signal infrarouge (IR)
- Détection des battements cardiaques
- Calcul, filtration et stabilisation du BPM

Le calcul SpO₂ est présent dans la bibliothèque mais n’est pas encore exploité dans le programme principal.

## Améliorations apportées à la bibliothèque SparkFun
- Réorganisation et modification des fichiers internes
- Meilleure gestion des lectures capteur
- Base pour traitement du signal plus avancé

## Fonctionnement

### 1. Détection du doigt
Le système vérifie la présence du doigt en analysant la variation du signal IR :
- seuil d’activation
- seuil de désactivation

### 2. Calibration automatique
Une baseline IR est calculée au démarrage pour stabiliser les mesures :
- moyenne sur plusieurs échantillons
- permet de mieux détecter les variations réelles

### 3. Détection des battements
Les battements sont détectés à partir du signal IR via :
- analyse des variations du signal
- mesure du temps entre deux battements

### 4. Calcul du BPM
Le BPM est calculé à partir de l’intervalle entre battements :
- conversion intervalle → BPM
- rejet des valeurs incohérentes
- limitation des intervalles minimum / maximum

### 5. Stabilisation du BPM
Pour éviter les valeurs instables :
- moyenne glissante (fenêtre de plusieurs mesures)
- tolérance entre valeurs successives
- rejet des mesures aberrantes

### 6. Gestion des erreurs
- reset automatique en cas d’absence de doigt
- rejet des mesures incohérentes
- limitation du nombre d’erreurs consécutives

## Structure du projet

- `main.cpp`  
  Logique principale (lecture capteur, BPM, gestion état)

- `MAX30105.*`  
  Driver capteur modifié (base SparkFun)

- `heartRate.*`  
  Détection des battements cardiaques

- `spo2_algorithm.*`  
  Algorithme SpO₂ (non utilisé actuellement)

## Matériel
- MAX30102
- Arduino (évolution possible vers STM32F401 selon l'écran qui va être inclus au projet)
- Connexion I2C

## Limitations actuelles
- SpO₂ non intégré dans le main
- Sensible aux mouvements
- Dépend fortement du positionnement du doigt

## Objectif
Améliorer progressivement :
- la stabilité des mesures
- le traitement du signal
- l’intégration du calcul SpO₂
- la robustesse globale pour aller vers un système fiable

## Avertissement
Ce projet n’est pas un dispositif médical certifié.
