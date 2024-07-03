/** \file MyCreds.h
 *
 * \page credentials Credentials
 *
 * \section credentials_intro Introduction
 *
 * Ce fichier contient les déclarations des variables et les définitions des fonctions liées aux credentials des différents services utilisés par le programme.
 *
 * Dans la mesure où une partie des librairies rend impossible le fait de rendre la totalité de ces variables dynamique, elles ont été regroupées dans un même fichier par simplicité de modification.
 *
 * \section credentials_variables Variables
 *
 * Ce fichier contient la déclaration d'un grand nombre de variables globales liées à différents services utilisés par l'appareil.
 *
 * Variables liées aux noms des appareils (utilisés dans un grand nombre de fichiers) :
 * - CTDevice_ID : ID de l'appareil, correspondant au nom du groupe des feeds liés à l'utilisateur
 * - Other_CDevice_ID : ID de l'autre appareil, correspondant au nom du groupe des feeds qui lui sont liés
 *
 * Variables liées au réseau WAN (MyWifi.h) :
 * - wan_ssid : SSID du réseau servant à accéder à Internet
 * - wan_pswd : Mot de passe du réseau servant à accéder à Internet
 *
 * Variables liées au réseau LAN (MyWifi.h, MyWebServer.h) :
 * - lan_ssid : SSID du réseau interne (utilisé par le serveur web)
 * - lan_pswd : Mot de passe du réseau interne
 * - local_ip, gateway, subnet : définissants les adresses utilisées par le réseau interne. Ces variables peuvent être laissées à celle par défaut, mais il faut veiller à ce que deux appareils sur le même WAN n'utilisent pas le même sous-réseau pour leur LAN.
 *
 * Variables liées à Adafruit (MyAdafruit.h) :
 * - IO_USERNAME : Identifiant de l'utilisateur lié à l'appareil sur Adafruit.io
 * - IO_KEY : Clé liée à l'utilisateur sur Adafruit.io
 * - IO_OTHER_USERNAME : Identifiant de l'utilisateur lié à l'autre appareil sur Adafruit.io
 *
 * Variables liées à l'Over The Air (MyOTA.h) :
 * - ota_hostname : Identifiant de l'appareil pour y accéder via l'Over the Air
 * - ota_password : Mot de passe pour pouvoir téléverser un programme via l'Over The Air et telnet
 *
 * Variables liées au Bluetooth (MyBLE.h) :
 * - bt_hostname : Identifiant de l'utilisateur, visible par les autres appareils via Bluetooth
 *
 * Variables liées au Covid Tracker (MyBLE.h, MyAdafruit.h, MySPIFFS.h):
 * - user_health_status : Etat de santé de l'utilisateur
 * - tracker_distance : Distance de détection minimale en mètres pour qu'un appareil soit considéré comme contact
 * - tracker_time : Temps de contact minimal en secondes pour que l'état de santé d'un utilisateur suivi soit tracké
 *
 */

String CTDevice_ID = "tony";
String Other_CDevice_ID = "marianne";

// Variables réseau : WAN
String wan_ssid;
String wan_pswd;

// Variables réseau : LAN
String lan_ssid;
String lan_pswd;
IPAddress local_ip(192, 168, 3, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// Variables Adafruit
#define IO_USERNAME "todedo"
#define IO_KEY ""
#define IO_OTHER_USERNAME "mariannecorbel"

// Variables Over The Air
String ota_hostname;
String ota_password = "password";

// Variables BLE
String bt_hostname;

// Variables Tracker
int tracker_distance; // distance de detection en metre
int tracker_time;     // temps après contact en minute
String user_health_status;

// ------------------------------------------- //
// FONCTIONS UTILITAIRES --------------------- //
// ------------------------------------------- //

// Met l'état de santé de l'utilisateur a jour
void set_user_health_status(String status)
{
    if (status == "MALADE" || status == "CONTACT")
    {
        user_health_status = status;
    }
    else
    {
        user_health_status = "SAIN";
    }
}

// Met à jour le nom de l'appareil
void set_device_ID(String name)
{
    CTDevice_ID = name;
}

// Retourne le nom complet de l'appareil
String get_device_name(bool generate_if_missing = false)
{
    return "CT_" + CTDevice_ID;
}

// DEBUG : Affiche la totalité des variables dans la console
void print_creds()
{
    Serial.println("Device name : " + get_device_name());
    Serial.println("WAN - SSID : " + wan_ssid + " password : " + wan_pswd);
    Serial.println("LAN - SSID : " + lan_ssid + " password : " + lan_pswd);
    Serial.println("OTA - Hostname : " + ota_hostname + " password : " + ota_password);
    Serial.println("BT - Hostname : " + String(bt_hostname));
}
