/** \file covid_tracker.ino
 *
 * \page arduino Fichier principal covid_tracker.ino
 * \tableofcontents
 *
 * \section intro Introduction
 *
 * Ce fichier contient le code principal du projet. Il initialise les différents modules et définit les fonctions de configuration (setup) et de boucle (loop).
 *
 * \section modules Modules
 *
 * Ce fichier inclut les modules suivants :
 * - MyCreds.h : Gestion des variables globales, notamment identifiants et mots de passe
 * - MyDebug.h : Débogage
 * - MyWiFi.h : Configuration et gestion du WiFi
 * - MySPIFFS.h : Système de fichiers Flash
 * - MyAdafruitIO.h : Gestion de la communication MQTT avec Adafruit IO
 * - MyWebServer.h : Serveur Web
 * - MyBLE.h : Communication Bluetooth Low Energy (BLE)
 * - MyOTA.h : Mise à jour du firmware Over The Air (OTA)
 * - MyCore0.h : Utilisation du Core 0 (pour les processeurs multi-coeurs)
 * - MyTicker.h : Gestion des tâches périodiques (tickers)
 *
 * \section setup Configuration (SETUP)
 *
 * La fonction setup() initialise tous les modules nécessaires au fonctionnement du projet. Voici les étapes effectuées dans cette fonction :
 *
 * - Initialisation du débogage
 * - Initialisation du système de fichiers SPIFFS
 * - Initialisation du WiFi + synchronisation NTP
 * - Initialisation du serveur Web
 * - Initialisation du Core 0 pour faire tourner le serveur Web
 * - Initialisation de la communication MQTT avec Adafruit IO et du ticker envoyant l'état de santé à intervalle régulier
 * - Initialisation du serveur puis du client BLE
 * - Initialisation du mode Over The Air (OTA)
 * - Impression des identifiants et mots de passe (pour le débogage)
 *
 * \section loop Boucle Principale (LOOP)
 *
 * La fonction loop() exécute en boucle les tâches nécessaires au fonctionnement du projet sur le coeur principal (Core1) :
 *
 * - Gestion de la communication MQTT avec Adafruit IO
 * - Gestion du client BLE pour scanner les identifiants à proximité
 * - Gestion du mode OTA pour les mises à jour du firmware
 * - Pause de 10 ms pour permettre au processeur de passer à d'autres tâches si nécessaire
 *
 * La fonction loop1() exécute en boucle les tâches nécessaires au fonctionnement du projet sur le coeur secondaire (Core0) :
 *
 * - Gestion du serveur Web
 */

#define MYDEBUG 1

// Variables globales, accessibles depuis tous les fichiers
int i = 0; // pour la loop

// ------------------------------------------------------------------------------------------------
// MODULES
#include "MyCreds.h"
#include "MyDebug.h"      // Debug
#include "MyWiFi.h"       // WiFi
#include "MySPIFFS.h"     // Flash File System
#include "MyAdafruitIO.h" // Adafruit MQTT
#include "MyWebServer.h"  // Serveur Web
#include "MyDeepSleep.h"  // Deep Sleep
#include "MyBLE.h"        // BLE
#include "MyOTA.h"        // Over the air
#include "MyCore0.h"      // Utilisation du Core 0
#include "MyTicker.h"
//  ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// SETUP
// ------------------------------------------------------------------------------------------------

void setup()
{
  setupDebug();
  MYDEBUG_PRINTLN("------------------- SETUP");
  setupSPIFFS();          // Initialisation du système de fichiers
  setupWiFi();            // Initialisation du WiFi
  getNTP();               // Récupère l'heure via NTP
  setupWebServer();       // Initialisation du Serveur Web
  setupMyCore0();         // Initialisation du Core 0
  setupAdafruitIO();      // Initialisation Adafruit MQTT
  setupTicker();          // Initialisation du Ticker lié à l'envoi régulier de l'état de santé à Adafruit
  setupDeepSleepTicker(); // Initialisation d'un ticker pour DeepSleep
  setupBLEServer();       // Initialisation du serveur BLE pour publier un ID
  setupBLEClient();       // Initialisation du client BLE pour scanner les ID à proximité
  setupOTA();             // Initialisation du mode Over The Air
  print_creds();          // Affiche les crédentials liés au réseau
}

// ------------------------------------------------------------------------------------------------
// LOOP
// ------------------------------------------------------------------------------------------------

void loop()
{
  loopAdafruitIO();
  loopBLEClient();
  loopOTA();
  delay(10); // Délai pour que le CPU puisse passer à d'éventuelles autres tâches
}
