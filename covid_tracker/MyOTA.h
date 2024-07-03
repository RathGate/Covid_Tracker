/**
 * \file MyOTA.h
 *
 * \page OTA
 * \tableofcontents
 *
 * \section ota_intro Introduction
 *
 * Ce fichier contient les déclarations et les définitions des fonctions pour la mise à jour Over-The-Air (OTA) et le débogage à distance.
 *
 * A noter que la librarie de Remote Debug est incluse, mais aucune fonction n'en a été utilisée pour rendre le débug visible en telnet.
 *
 * \section ota_functions Fonctions
 *
 * Ce fichier définit les fonctions suivantes :
 * - setupOTA : Configuration et démarrage des services OTA et Remote Debug
 * - loopOTA : Boucle de gestion des demandes de téléversement OTA et des messages de débogage à distance
 *
 * \section ota_enums Énumérations
 *
 * Ce fichier définit l'énumération suivante pour les types de débogage :
 * - DebugType : VERBOSE, DEBUG, INFO, WARNING, ERROR
 *
 * \section ota_variables Variables Globales
 *
 * Pour fonctionner, il a besoin des variables suivantes, déclarées dans le fichier MyCreds.h :
 * - bt_hostname : Identifiant de l'utilisateur, visible par les autres appareils via Bluetooth
 *
 * Ce fichier définit les variables globales suivantes :
 * - Debug : Instance de RemoteDebug pour le débogage à distance
 * - debugTicker : Ticker pour générer des messages de débogage à intervalles réguliers
 *
 */

#include <ArduinoOTA.h>
#include <RemoteDebug.h>

RemoteDebug Debug;
Ticker debugTicker;

enum DebugType
{
  VERBOSE,
  DEBUG,
  INFO,
  WARNING,
  ERROR
};

void generateDebugLog(String content, DebugType debugType)
{
  // Todo
}

void setupOTA()
{
  ota_hostname = generate_service_ID("_OTA");
  // On a besoin d'une connexion WiFi !
  if (WiFi.status() != WL_CONNECTED)
  {
    setupWiFi(); // Connexion WiFi
  }

  // Démarrage d'OTA
  MYDEBUG_PRINTLN("-OTA : Démarrage");
  ArduinoOTA.setHostname((char *)ota_hostname.c_str()); // Nommage pour l'identification
  ArduinoOTA.setPassword((char *)ota_password.c_str()); // Mot de passe pour les téléversements
  ArduinoOTA.begin();                                   // Initialisation de l'OTA

  // Initialisation de la librairie RemoteDebug
  Debug.begin(ota_hostname);
  Debug.setPassword(ota_password); // Mot de passe pour les connexions telnet et Web app
  Debug.setResetCmdEnabled(true);  // Pour permettre le Reset par telnet
  Debug.showColors(true);          // Un peu de couleurs pour faire joli
  // Debug.setSerialEnabled(true);       // Pour activer un écho des logs sur le port série (si branché)
}

void loopOTA()
{
  ArduinoOTA.handle(); // Gestion des demandes de téléversement
  Debug.handle();      // Gestion des messages de remote debug
}
