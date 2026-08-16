# R-BOT / Lumi — V1
🤖 Lumi - Robot Compagnon Connecté & IA

![alt tag](https://github.com/RaphaelGrj/Lumi-Project/blob/main/images/lumi-render.png)

📝 Description du projet

Lumi est un petit robot compagnon de bureau DIY et open-source. Plus qu'un simple gadget, Lumi est doté d'une véritable personnalité évolutive. Animé par l'intelligence artificielle (Gemini) et conçu autour d'un écosystème de microcontrôleurs (ESP32-S3 et ESP32-CAM), il interagit de manière organique avec son environnement et son utilisateur.

Capable de voir, de ressentir la température, de réagir au toucher et de discuter de vive voix, Lumi s'intègre également à Home Assistant pour faire le pont entre la robotique de compagnie et la domotique. L'ensemble de son boîtier est conçu sur mesure pour l'impression 3D.

📋 Cahier des Charges & Fonctionnalités

🧠 Intelligence & Personnalité
Intégration Gemini : Lumi utilise l'IA pour discuter, réfléchir et développer sa propre personnalité au fil des interactions.

Moteur d'Humeur (State Machine) : Le comportement de Lumi n'est pas scripté de manière rigide, mais géré par une boucle d'état (State Machine) qui calcule son humeur selon le temps et ses capteurs.

Exemples : S'il ne voit personne pendant un long moment, il s'ennuie. S'il est trop secoué, il devient agacé.

Échange Vocal : Discussion naturelle et interactive avec l'utilisateur via son haut-parleur intégré.

👁️ La Vue (ESP32-CAM)
Suivi du Regard (Eye-Tracking) : Utilisation du Motion Tracking en temps réel. La caméra détecte le mouvement et ordonne aux yeux gérés par l'ESP32-S3 de se verrouiller sur la personne.

Reconnaissance Faciale & Animale : Identification des membres du foyer (humains) et des animaux de compagnie (pour faire la différence entre son créateur et le chat de la maison, par exemple).

🌡️ Perception Thermique (Capteur BME280)
Lumi ressent son environnement climatique et l'exprime via ses écrans/yeux :

S'il fait froid : Animations de grelottement ou apparition de stalactites.

S'il fait chaud : Apparition de gouttes de sueur ou yeux lourds de fatigue.

✋ Perception Physique (Capteur MPU6050)
Grâce à son accéléromètre/gyroscope, Lumi a conscience de son propre corps :

Détection des secousses : Les yeux deviennent désorientés s'il est bousculé.

Fonction "Tap to Cancel" : Une simple petite tape sur son crâne permet d'annuler son écoute ou d'interrompre une action.

🏠 Écosystème & Connectivité
Home Assistant : Intégration complète pour que Lumi puisse interagir avec la maison connectée (remontée d'états, déclenchement de scénarios).

Portail Captif "Privacy First" : Configuration du réseau local (Wi-Fi) via une interface web propre. Aucun identifiant n'est codé en dur, garantissant un code source sûr à partager.

🛠️ Matériel (Hardware Core)
Cerveau Principal : ESP32-S3 (N16R8) pour gérer l'IA, le moteur d'humeur et l'affichage.

Module Vision : ESP32-CAM dédié au traitement d'images.

Capteurs : BME280 (Température/Humidité/Pression) et MPU6050 (Mouvements/Chocs).

Audio : Haut-parleur pour le retour vocal.

Châssis : Fichiers STL sur mesure optimisés pour l'impression 3D (FDM/Résine).

![alt tag](https://github.com/RaphaelGrj/Lumi-Project/blob/main/images/lumi-breadboard.png)

### 🛒 Bill of Materials (BOM) - Matériel Requis

Voici la liste des composants nécessaires pour assembler Lumi.

| Catégorie | Composant | Rôle & Remarques |
| :--- | :--- | :--- |
| **Microcontrôleurs** | ESP32-S3 (N16R8) | Cerveau principal. Les 16MB de Flash et 8MB de PSRAM sont indispensables pour le mode IA et la gestion fluide de l'affichage (State Machine). |
| | ESP32-CAM | Module de vision dédié au Motion Tracking (Eye-tracking) et à la reconnaissance visuelle (humains/animaux). |
| **Capteurs (Sensors)**| BME280 | Perception thermique : mesure de la température et de l'humidité pour déclencher les animations de confort (grelotte, sueur). |
| | MPU6050 | Perception physique : Accéléromètre/Gyroscope pour détecter les bousculades et la fonction "Tap to Cancel". |
| | Microphone I2S (ex: INMP441) | Indispensable pour capturer la voix de l'utilisateur de manière claire et l'envoyer à Gemini. |
| **Affichage & Audio** | Écrans pour les yeux | (Ex: 2x écrans LCD ronds GC9A01 ou écrans TFT) Pour l'animation du regard et des expressions. |
| | Haut-parleur (Speaker) | Mini haut-parleur pour la voix de Lumi (idéalement avec un mini ampli I2S comme le MAX98357A pour un son net). |
| **Alimentation** | Alimentation ou Batterie | Alimentation filaire 5V robuste, ou module de charge (ex: TP4056) + Batterie LiPo si le robot doit être autonome. |
| **Structure** | Pièces imprimées en 3D | Boîtier sur mesure. Parfaitement réalisable en FDM pour la structure interne robuste, avec possibilité d'utiliser de l'impression résine pour un rendu lisse sur les éléments de façade ou les détails. |
| **Connectique** | Câbles, fils et visserie | Fils de câblage souples, connecteurs Dupont (ou soudure directe), et petite visserie (M2/M3) pour assembler les éléments dans le châssis. |
