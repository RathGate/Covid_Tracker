/** \file MyBLE.h
 *
 * \page BLE
 * \tableofcontents
 *
 * \section BLE_intro Introduction
 *
 * Ce fichier contient les déclarations et les définitions des fonctions pour la gestion de la communication Bluetooth Low Energy (BLE).
 *
 * \section BLE_configuration Configuration
 *
 * Ce fichier contient la configuration suivante :
 * - SERVICE_UUID : Identifiant du service BLE
 * - scanTime : Temps de scan en secondes
 * - scanInterval
 * - scanWindow
 *
 * En cas de fonctionnement dans un milieu trop dense en termes d'appareil bluetooth, il est possible de modifier les variables scanTime, scanInterval et scanWindow pour réduire la quantité de crashes.
 * Cependant, leur fonctionnement pratique semble relever des sciences obscures.
 *
 * \section BLE_variables Variables
 *
 * Ce fichier contient les variables globales suivantes :
 * - pBLEScan : Pointeur vers l'objet BLEScan pour la détection des périphériques BLE
 * - BLE_ticker_time : Temps total de fonctionnement du bluetooth, sert à calculer l'intervalle depuis le dernier scan
 * - last_interval : Dernier intervalle de temps entre deux scans BLE
 * - has_found_device : Indique si un dispositif BLE a été trouvé lors du scan. Cette variable sert à éviter les crashes lors de la démonstration en milieu dense : l'appareil cherchera à n'en détecter qu'un autre avec le même SERVICE UUID.
 *
 * \section BLE_functions Fonctions
 *
 * Ce fichier définit les fonctions suivantes :
 * - setupBLEServer : Configuration du serveur BLE pour l'envoi des informations
 * - MyAdvertisedDeviceCallbacks : Classe de rappel pour la détection des périphériques BLE
 * - loopBLEClient : Boucle de détection des périphériques BLE à proximité
 * - setupBLEClient : Configuration du client BLE pour la détection des périphériques
 *
 */

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID "436f6e74-6163-7420-5472-61636b65727d" // Oui un des caractères est différent pas de panique :)

// Variables liées aux délais de scan du Bluetooth
// En cas de crash lié à un environnement trop denses en appareils bluetooth,
// ces variables peuvent être modifiées.
int scanTime = 2; // En secondes
int scanInterval = 512;
int scanWindow = 49; // Doit être inférieur ou égal à setInterval

LEScan *pBLEScan;

unsigned long BLE_ticker_time; // temps total de fonctionnement du bluetooth
int last_interval;             // intervalle en secondes entre le dernier scan et le précédent
bool has_found_device = false; // sert à ne détecter qu'un seul appareil

void setupBLEServer()
{
  // Génère les crédentials du Bluetooth basé sur le nom de l'appareil
  bt_hostname = generate_service_ID("_BT");
  char *char_bt_hostname = (char *)bt_hostname.c_str();

  MYDEBUG_PRINTLN("-BLE : Démarrage du serveur sous le nom : " + bt_hostname);

  // Initialise le serveur Bluetooth
  BLEDevice::init(char_bt_hostname);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  BLEDevice::startAdvertising();
  MYDEBUG_PRINTLN("-BLE : Serveur démarré");
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    // S'il a déjà trouvé l'autre appareil, la fonction s'annule
    if (has_found_device)
    {
      return;
    }
    // Si le service UUID de l'autre appareil est le même
    if (SERVICE_UUID == advertisedDevice.getServiceUUID().toString())
    {
      // Calcule la distance
      float ratio = (-69 - advertisedDevice.getRSSI()) / (10 * 2);
      float Distance = pow(10, ratio);
      if (Distance <= tracker_distance)
      {
        MYDEBUG_PRINT("Detected device : ");
        MYDEBUG_PRINTLN(advertisedDevice.getName().c_str());
        // L'autre appareil est à distance suffisante et donc détecté comme un contact
        register_contact(last_interval);
        has_found_device = true; // évite de rechercher d'autres appareils une fois le contact détecté
      }
    }
  }
};

// Boucle du client Bluetooth
void loopBLEClient()
{
  has_found_device = false;                                      // initialise la détection à false
  last_interval = round((micros() - BLE_ticker_time) / 1000000); // récupère le temps depuis la dernière détection
  BLE_ticker_time = micros();

  MYDEBUG_PRINT("-BLE Client : Scan @ ");
  timeClient.update();
  MYDEBUG_PRINTLN(timeClient.getFormattedTime());

  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  MYDEBUG_PRINT("-BLE Client : Device trouvé");
  MYDEBUG_PRINTLN(foundDevices.getCount());

  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  MYDEBUG_PRINTLN("-BLE Client : Scan terminé");
  delay(1000);
}

// Met en place le client Bluetooth
void setupBLEClient()
{
  MYDEBUG_PRINTLN("-BLE Client : Démarrage");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); // associe la détection d'appareil à une fonction callback
  pBLEScan->setActiveScan(true);                                             // plus efficace mais plus gourmand en énergie
  pBLEScan->setInterval(scanInterval);
  pBLEScan->setWindow(scanWindow);
  MYDEBUG_PRINTLN("-BLE Client : Démarré");
}
