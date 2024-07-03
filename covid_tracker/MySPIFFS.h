/**
 * \file MySPIFFS.h
 *
 * \page SPIFFS
 * \tableofcontents
 *
 * \section spiffs_intro Introduction
 *
 * Ce fichier contient les déclarations et les définitions des fonctions pour la gestion du système de fichiers SPIFFS.
 *
 * \section spiffs_functions Fonctions
 *
 * Ce fichier définit les fonctions suivantes :
 * - setupSPIFFS : Montage du système de fichiers SPIFFS et récupération des données de configuration et d'historique
 * - PrintSPIFFSFiles : Affichage dans le terminal série de la liste des fichiers du SPIFFS
 * - PrintFileContent : Affichage dans le terminal série du contenu d'un fichier
 * - getConfig : Récupération et enregistrement des données de configuration depuis le fichier config.json
 * - getHistory : Récupération et retour des données d'historique depuis le fichier history.json
 * - updateHistory : Mise à jour des données d'historique avec le temps passé en contact avec un autre appareil
 * - update_contact_health : Mise à jour de l'état de santé d'un contact dans l'historique
 * - updateNetwork : Mise à jour de la configuration réseau dans le fichier de configuration
 * - updateDeviceConfig : Mise à jour d'une valeur de configuration de l'appareil dans le fichier de configuration
 * - read_json_file : Lecture et désérialisation d'un fichier JSON
 * - write_json_file : Sérialisation et écriture d'un document JSON dans un fichier
 * - add_to_history : Ajout du temps passé en contact avec un autre appareil dans l'historique
 *
 * \section spiffs_variables Variables Globales
 *
 * Ce fichier définit les variables globales suivantes :
 * - strConfigFile : Chemin du fichier de configuration (config.json)
 * - strHistoryFile : Chemin du fichier d'historique (history.json)
 * - configFile : Instance du fichier de configuration
 * - historyFile : Instance du fichier d'historique
 * - isSPIFFSSetup : Indicateur du statut du système de fichiers SPIFFS (monté ou non)
 *
 */

#include "SPIFFS.h"
#include <ArduinoJson.h>

String strConfigFile("/config.json");
String strHistoryFile("/history.json");
File configFile;
File historyFile;

bool isSPIFFSSetup = false;

// Affiche dans le terminal série la liste des fichiers du SPIFFS.
void PrintSPIFFSFiles()
{
  if (!isSPIFFSSetup)
  {
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  Serial.println("FILE LIST:");

  while (file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file = root.openNextFile();
  }
  file.close();
}

// Affiche le contenu du fichier dans le terminal série.
void PrintFileContent(String filename)
{
  MYDEBUG_PRINTLN("LECTURE DU FICHIER " + filename);
  File test = SPIFFS.open(filename, "r");
  while (test.available())
  {
    Serial.write(test.read());
  }
  test.close();
  MYDEBUG_PRINTLN("-----------");
}

void save_health_status()
{
  if (SPIFFS.exists(strConfigFile))
  {
    // Ouverture du fichier de config en lecture
    configFile = SPIFFS.open(strConfigFile, "r");
    // Désérialisation du document JSON
    DynamicJsonDocument jsonDocument(8192);
    DeserializationError error = deserializeJson(jsonDocument, configFile);
    // Fermeture du fichier de config
    configFile.close();

    // Modification de la valeur de la config
    jsonDocument["health"] = user_health_status;

    // Ouverture du fichier de config en écriture
    configFile = SPIFFS.open(strConfigFile, "w");
    // Sérialisation du document JSON pour sauvegarder la valeur modifiée.
    serializeJson(jsonDocument, configFile);
    // Fermeture du fichier de config
    configFile.close();
  }
}

// Récupère et enregistre les données de configuration présentes
// dans le fichier config.json.
void getConfig(bool get_health_status = false)
{
  String temp_id;
  MYDEBUG_PRINTLN("OUVERTURE DU FICHIER DE CONFIGURATION");
  if (SPIFFS.exists(strConfigFile))
  {
    configFile = SPIFFS.open(strConfigFile, "r");
    // PrintFileContent(strConfigFile); // Afficher le contenu du fichier

    DynamicJsonDocument jsonDocument(8192);
    // Désérialisation du document JSON lu
    DeserializationError error = deserializeJson(jsonDocument, configFile);
    if (error)
    {
      MYDEBUG_PRINTLN("-SPIFFS : Impossible de parser le JSON");
      return;
    }

    wan_ssid = (const char *)jsonDocument["network"]["wan"]["ssid"];
    wan_pswd = (const char *)jsonDocument["network"]["wan"]["pswd"];
    lan_ssid = generate_service_ID("_AP");
    lan_pswd = (const char *)jsonDocument["network"]["lan"]["pswd"];
    tracker_distance = (const int)jsonDocument["device"]["tracker_distance"];
    tracker_time = (const int)jsonDocument["device"]["tracker_time"];
    String temp = ((const char *)jsonDocument["health"]);
    set_user_health_status(temp);
    save_health_status();
    MYDEBUG_PRINTLN("Health temp : " + temp + " | Health : " + user_health_status);
  }
  else
  {
    // TODO : Créer le JSON de config
    MYDEBUG_PRINTLN("-SPIFFS : Fichier de configuration manquant");
  }
}

DynamicJsonDocument getHistory()
{
  MYDEBUG_PRINTLN("OUVERTURE DU FICHIER D'HISTORIQUE");
  DynamicJsonDocument jsonDocument(8192);
  if (SPIFFS.exists(strHistoryFile))
  {
    historyFile = SPIFFS.open(strHistoryFile, "r");
    // Désérialisation du document JSON lu
    DeserializationError error = deserializeJson(jsonDocument, historyFile);
    historyFile.close();
    if (error)
    {
      // TODO : Créer le JSON de config
      MYDEBUG_PRINTLN("-SPIFFS : Impossible de parser le JSON");
    }
  }
  else
  {
    // TODO : Créer le JSON de config
    MYDEBUG_PRINTLN("-SPIFFS : Fichier d'historique manquant");
  }

  return jsonDocument;
}

void updateHistory(int index, String key, String value)
{
  if (SPIFFS.exists(strHistoryFile))
  {
    // Ouverture du fichier de config en lecture
    historyFile = SPIFFS.open(strHistoryFile, "r");
    // Désérialisation du document JSON
    DynamicJsonDocument jsonDocument(8192);
    DeserializationError error = deserializeJson(jsonDocument, historyFile);
    // Fermeture du fichier de config
    historyFile.close();

    // Modification de la valeur de la config
    JsonArray historyArray = jsonDocument.as<JsonArray>();
    historyArray[index][key] = value;

    // Ouverture du fichier de config en écriture
    historyFile = SPIFFS.open(strHistoryFile, "w");
    // Sérialisation du document JSON pour sauvegarder la valeur modifiée.
    serializeJson(jsonDocument, historyFile);
    // Fermeture du fichier de config
    historyFile.close();
  }
}

DynamicJsonDocument read_json_file(String filename)
{
  DynamicJsonDocument jsonDocument(8192);
  if (SPIFFS.exists(filename))
  {
    File temp_file = SPIFFS.open(filename);
    DeserializationError error = deserializeJson(jsonDocument, temp_file);
    temp_file.close();
  }
  return jsonDocument;
}

void write_json_file(String filename, DynamicJsonDocument jsonDocument)
{
  if (!SPIFFS.exists(filename))
  {
    return;
  }
  // Ouverture du fichier de config en écriture
  File temp = SPIFFS.open(filename, "w");
  // Sérialisation du document JSON pour sauvegarder la valeur modifiée.
  serializeJson(jsonDocument, temp);
  // Fermeture du fichier de config
  temp.close();
}

int add_to_history(String other_ID, int time_in_sec, String health = "")
{
  if (!SPIFFS.exists(strHistoryFile))
  {
    return -1;
  }

  // Ouverture du fichier de config en lecture
  historyFile = SPIFFS.open(strHistoryFile, "r");
  // Désérialisation du document JSON
  DynamicJsonDocument jsonDocument(8192);
  DeserializationError error = deserializeJson(jsonDocument, historyFile);
  // Fermeture du fichier de config
  historyFile.close();

  JsonArray history_arr = jsonDocument.as<JsonArray>();
  bool found = false;
  for (JsonObject elem : history_arr)
  {
    if (elem["name"] == other_ID.c_str())
    {
      found = true;
      time_in_sec += elem["time_spent"].as<int>();
      elem["time_spent"] = time_in_sec;
      elem["last_encountered"] = timeClient.getFormattedTime();
      break;
    }
  }
  if (!found)
  {
    JsonObject obj = jsonDocument.createNestedObject();
    obj["name"] = other_ID;
    obj["time_spent"] = time_in_sec;
    obj["last_encountered"] = timeClient.getFormattedTime();
    obj["health_status"] = "En attente...";
  }
  write_json_file(strHistoryFile, jsonDocument);

  return time_in_sec;
}

void update_contact_health(String other_ID, String status)
{
  // Ouverture du fichier de config en lecture
  historyFile = SPIFFS.open(strHistoryFile, "r");
  // Désérialisation du document JSON
  DynamicJsonDocument jsonDocument(8192);
  DeserializationError error = deserializeJson(jsonDocument, historyFile);
  // Fermeture du fichier de config
  historyFile.close();

  JsonArray history_arr = jsonDocument.as<JsonArray>();
  for (JsonObject elem : history_arr)
  {
    if (elem["name"] == other_ID.c_str())
    {
      elem["health_status"] = status;
    }
  }
  write_json_file(strHistoryFile, jsonDocument);
}

// Fonction d'update de la configuration réseau.
// networkType: wan/lan
void updateNetwork(String networkType, String ssid, String password)
{
  if (SPIFFS.exists(strConfigFile))
  {
    // Ouverture du fichier de config en lecture
    configFile = SPIFFS.open(strConfigFile, "r");
    // Désérialisation du document JSON
    DynamicJsonDocument jsonDocument(8192);
    DeserializationError error = deserializeJson(jsonDocument, configFile);
    // Fermeture du fichier de config
    configFile.close();

    // Modification de la valeur de la config
    jsonDocument["network"][networkType]["ssid"] = ssid;
    jsonDocument["network"][networkType]["pswd"] = password;

    // Ouverture du fichier de config en écriture
    configFile = SPIFFS.open(strConfigFile, "w");
    // Sérialisation du document JSON pour sauvegarder la valeur modifiée.
    serializeJson(jsonDocument, configFile);
    // Fermeture du fichier de config
    configFile.close();
  }

  getConfig();
}

void updateDeviceConfig(String key, String value)
{
  if (SPIFFS.exists(strConfigFile))
  {
    // Ouverture du fichier de config en lecture
    configFile = SPIFFS.open(strConfigFile, "r");
    // Désérialisation du document JSON
    DynamicJsonDocument jsonDocument(8192);
    DeserializationError error = deserializeJson(jsonDocument, configFile);
    // Fermeture du fichier de config
    configFile.close();

    // Modification de la valeur de la config
    jsonDocument["device"][key] = value;

    // Ouverture du fichier de config en écriture
    configFile = SPIFFS.open(strConfigFile, "w");
    // Sérialisation du document JSON pour sauvegarder la valeur modifiée.
    serializeJson(jsonDocument, configFile);
    // Fermeture du fichier de config
    configFile.close();
  }
}

void setupSPIFFS(bool bFormat = false)
{
  MYDEBUG_PRINTLN("-SPIFFS : Montage du système de fichier");

  if (SPIFFS.begin(true))
  {
    MYDEBUG_PRINTLN("-SPIFFS : MONTE");

    if (bFormat)
    {
      SPIFFS.format();
      bFormat = false;
    }
    isSPIFFSSetup = true;
    PrintSPIFFSFiles();
    getConfig(true);
    getHistory();
  }
  else
  {
    MYDEBUG_PRINT("-SPIFFS : Impossible de monter le système de fichier");
    isSPIFFSSetup = false;
  }
}
