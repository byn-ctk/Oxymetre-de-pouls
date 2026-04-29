# Changelog

## [1.4.0] - Portage sur STM32 | 2026-04-29


### Modifié

* Portage du projet sur **STM32F401RE Nucleo-64**.
* Support de l’environnement **PlatformIO** pour STM32.
* Mise en place de la base matérielle pour les futurs développements.


### Notes

* Cette version correspond à une première étape de migration vers STM32.

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
