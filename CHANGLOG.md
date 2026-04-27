# Changelog

## [2.0.0] - Migration vers STM32 pour intégration SpO2 (en cours)

### Ajouté
- Migration du projet vers un controleur STM32.
- Préparation de l’intégration SpO2 avec lecture des signaux RED et IR.
- Préparation de buffers SpO2 adaptés à une carte disposant de plus de mémoire RAM.

### Modifié
- Changement de cible matérielle principale : passage de l’Arduino Uno vers STM32.
- Adaptation prévue de la configuration PlatformIO.
- Adaptation de certaines variable propre à l'arduino.

### Raison de la migration
L’intégration de la SpO2 nécessite de stocker simultanément plusieurs échantillons des signaux RED et IR.

Lors des premiers essais sur Arduino Uno, PlatformIO indiquait une utilisation mémoire largement supérieure à la capacité disponible.

---

## [1.3.1] – Amélioration cohérence globale

### Objectif
Amélioration global du code et implémentation de garde-fou.
### Ajouts
- Détection de doigt plus robuste

### Modifications
- Amélioration de la cohérence globale du code
- Ajustement des conditions de validation du signal

### État
- BPM fonctionnel et stable
- Optimisation en cours

---

## [1.3.0] – Stabilisation avancée du BPM (historique de code indisponible avant cette version)

### Ajouts
- Moyenne pondérée du BPM (priorité aux valeurs récentes)

### Modifications
- Amélioration de la réactivité du BPM
- Nettoyage et cohérence du code

### Corrections
- Ajout de sécurités contre les valeurs aberrantes

### État
- BPM fonctionnel et stable

---

## [1.2.0] – Stabilisation initiale du BPM

### Ajouts
- Moyenne des valeurs BPM
- Rejet des battements aberrants

### Modifications
- Amélioration de la fiabilité globale

### État
- BPM fonctionnel mais encore sensible au bruit

---

## [1.1.0] – Calcul du BPM

### Ajouts
- Intégration de l'algorithme de détection des battements cardiaques natif de chez SparkFun
- Mesure de l’intervalle entre battements
- Calcul d'un BPM brut

### État
- Fonctionnel mais instable

---

## [1.0.0] – Initialisation capteur

### Ajouts
- Initialisation du MAX30102
- Lecture du signal IR
- Validation du fonctionnement du capteur

### État
- Base technique uniquement
