# Lumi
🤖 Lumi - Robot Compagnon Connecté & IA
📝 Description du projet
Lumi est un robot compagnon de bureau DIY, open-source et interactif. Conçu autour d'un microcontrôleur ESP32-S3, il allie conception 3D sur mesure et intelligence artificielle embarquée. Le projet a pour but de créer un petit assistant autonome, capable d'interagir avec son environnement grâce à la vision, tout en offrant une interface de contrôle locale, sécurisée et facile à configurer.

Lumi est pensé pour la communauté des makers : l'ensemble des pièces du boîtier sont optimisées pour l'impression 3D, et le code est conçu de manière modulaire.

📋 Cahier des Charges
🛠️ Matériel (Hardware)
Cerveau : ESP32-S3 (version N16R8) pour disposer d'assez de puissance et de mémoire pour le "mode IA boost".

Vision : Intégration d'un module caméra pour le traitement d'image local et le streaming.

Châssis : Boîtier 100% sur mesure, pensé pour l'impression 3D, avec une intégration propre de l'électronique.

Affichage : Intégration d'animations visuelles (comme l'animation de démarrage "Lumi glitch").

💻 Logiciel & Fonctionnalités (Software)
Configuration "Privacy First" (Portail Captif) : Lumi intègre un gestionnaire de connexion réseau via une interface web. Les identifiants Wi-Fi ne sont jamais codés en dur dans le firmware, garantissant que le code peut être partagé en open-source sans fuite de données personnelles.

Accès Local Simplifié : Interface de gestion et de paramétrage accessible facilement sur le réseau local via mDNS (idéalement via [http://lumi.local](http://lumi.local)).

Retour Vidéo (LUMI_VISION) : Streaming en direct du flux caméra, consultable depuis un navigateur pour surveiller l'environnement du robot.

Intelligence Artificielle Embarquée : Modules de reconnaissance visuelle avec pour objectif d'identifier l'utilisateur ainsi que les animaux de compagnie (reconnaissance spécifique du chat).

🚀 Roadmap
[x] Initialisation du code et configuration de l'ESP32-S3.

[x] Mise en place de la page web de connexion sécurisée et anonymisée.

[ ] Stabilisation du flux vidéo et de la résolution réseau mDNS.

[ ] Déploiement des modèles IA de reconnaissance (humain/chat).

[ ] Publication des fichiers STL (impression 3D) finaux.

🤝 Contribution
(À venir) Les contributions sont les bienvenues ! Les schémas de câblage et les instructions de flashage seront bientôt ajoutés pour vous permettre de construire votre propre Lumi.
