/**
 * \file MyWiFi.h
 *
 * \page Wifi_NTP
 * \tableofcontents
 *
 * \section wifintp_intro Introduction
 *
 * Ce fichier contient les déclarations et les définitions des fonctions pour la gestion du WiFi et du NTP (Network Time Protocol).
 *
 * \section wifintp_functions Fonctions
 *
 * Ce fichier définit les fonctions suivantes :
 * - setupWiFi : Configuration du WiFi en mode Access Point et Station
 * - TryWifiConnexion : Tentative de connexion au réseau WiFi avec ou sans identifiants
 * - setupNTP : Configuration et initialisation du client NTP
 * - getNTP : Mise à jour du temps depuis le serveur NTP
 *
 * \section wifintp_variables Variables Globales
 *
 * Pour fonctionner, ce fichier a besoin des variables suivantes, déclarées dans le fichier MyCreds.h :
 * - wan_ssid : SSID du réseau servant à accéder à Internet
 * - wan_pswd : Mot de passe du réseau servant à accéder à Internet
 * - lan_ssid : SSID du réseau interne (utilisé par le serveur web)
 * - lan_pswd : Mot de passe du réseau interne
 * - local_ip, gateway, subnet : définissants les adresses utilisées par le réseau interne. Ces variables peuvent être laissées à celle par défaut, mais il faut veiller à ce que deux appareils sur le même WAN n'utilisent pas le même sous-réseau pour leur LAN.
 *
 * Ce fichier définit les variables globales suivantes :
 * - c_delay : Délai pour la tentative de connexion au WiFi
 * - ntpUDP : Objet WiFiUDP pour la communication avec le serveur NTP
 * - timeClient : Objet NTPClient pour la gestion du temps
 */

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "esp_wpa2.h"

// Variables
// pour le mode STATION
int c_delay = 5000;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void getNTP()
{
  timeClient.update();
}

void setupNTP()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    MYDEBUG_PRINTLN("-NTP : Impossible de récupérer l'heure (pas de connexion internet)");
    return;
  }
  timeClient.begin();
  timeClient.setTimeOffset(7200);
  getNTP();
}

bool TryWifiConnexion(String ssid = "", String pswd = "")
{
  if (ssid == "")
  {
    MYDEBUG_PRINTLN("No SSID");
    return false;
  }

  WiFi.begin(ssid, pswd);
  MYDEBUG_PRINT("-WIFI : Connexion au réseau ");
  MYDEBUG_PRINT(ssid);

  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i < c_delay)
  {
    delay(500);
    MYDEBUG_PRINT(".");
    i += 500;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    setupNTP();
  }
  else if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect();
  }

  MYDEBUG_PRINTLN("");
  return WiFi.status() == WL_CONNECTED;
}

void setupWiFi()
{
  MYDEBUG_PRINTLN();
  MYDEBUG_PRINTLN("-WIFI : Configuration");

  // Configuration de la carte en mode Access Point ET Station
  WiFi.mode(WIFI_AP_STA);

  // Démarrage du mode Access Point
  WiFi.softAP(lan_ssid, lan_pswd);
  WiFi.softAPConfig(local_ip, gateway, subnet);

  // J'affiche l'adresse IP de mon point d'accès
  MYDEBUG_PRINT("-WIFI : Access Point mis à disposition : ");
  MYDEBUG_PRINTLN(WiFi.softAPIP());

  // Démarrage du mode Station
  if (TryWifiConnexion(wan_ssid, wan_pswd))
  {
    MYDEBUG_PRINT("-WIFI : connecté en mode Station avec l'adresse IP : ");
    MYDEBUG_PRINTLN(WiFi.localIP());
  }
  else
  {
    MYDEBUG_PRINTLN("-WIFI : aucune connexion");
  }
}
