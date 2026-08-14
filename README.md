# Lumi
🤖 Lumi - Robot Compagnon Connecté & IA
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
