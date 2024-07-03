/** \file MyAdafruitIO.h
 *
 * \page adafruitio Adafruit IO
 *
 * \section adafruit_intro Introduction
 *
 * Ce fichier contient les déclarations et les définitions des fonctions pour la gestion de la communication MQTT avec Adafruit IO.
 *
 * Il est important de noter que dans le cadre de la démonstration, qui est une PoC (proof of concept), l'abonnement à l'autre appareil n'est pas dynamique en fonction de l'appareil rencontré.
 * Le problème majeur responsable de cela est qu'une tentative d'instanciation dynamique d'un abonnement à un autre feed fait soit crasher l'appareil, soit perturbe la connexion à Adafruit dans le meilleur des cas.
 *
 * \section adafruit_variables Variables
 *
 * Ce fichier contient la déclaration des variables globales suivantes :
 * - client : Client WiFi pour la connexion au broker Adafruit IO
 * - mqtt_client : Client MQTT Adafruit avec les informations de connexion
 *
 * Pour fonctionner, il a besoin des variables suivantes, déclarées dans le fichier MyCreds.h :
 * - IO_USERNAME : Identifiant de l'utilisateur lié à l'appareil sur Adafruit.io
 * - IO_KEY : Clé liée à l'utilisateur sur Adafruit.io
 * - IO_OTHER_USERNAME : Identifiant de l'utilisateur lié à l'autre appareil sur Adafruit.io
 * - user_health_status : Etat de santé de l'utilisateur lié à l'appareil
 *
 * \section adafruit_feeds Feeds
 *
 * Ce fichier définit les feeds auxquels l'appareil est abonné et publie des données :
 * - health_feed : Feed pour le statut de santé de l'utilisateur
 * - other_health_feed : Feed pour le statut de santé de l'autre appareil
 * - sub_user_health : Abonnement au feed de santé de l'utilisateur
 * - pub_user_health : Publication du statut de santé de l'utilisateur
 * - sub_other_health : Abonnement au feed de santé de l'autre appareil
 * - is_sub_to_other : Indique si l'appareil est abonné au feed de l'autre appareil
 *
 * \section adafruit_functions Fonctions
 *
 * Ce fichier définit les fonctions suivantes :
 * - update_own_health : Met à jour le statut de santé de l'utilisateur et publie sur le feed correspondant
 * - own_health_callback : Fonction de rappel appelée lorsqu'un message est reçu sur le feed de santé de l'utilisateur
 * - other_health_callback : Fonction de rappel appelée lorsqu'un message est reçu sur le feed de santé de l'autre appareil ; si l'autre utilisateur est malade, met à jour l'état de santé de l'utilisateur comme étant cas CONTACT
 * - register_contact : Enregistre et traite un contact avec un autre utilisateur ; si besoin, invoque le subscribe sur son feed de santé
 * - setupAdafruitIO : Initialise la connexion au WiFi et configure les fonctions de rappel pour les feeds
 * - connectAdafruitIO : Connecte le client MQTT au broker Adafruit IO
 * - loopAdafruitIO : Boucle de traitement des paquets MQTT et gestion de la connexion au broker Adafruit IO
 */

#include <Ticker.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* Configuration *************************************/
// Connexion Adafruit
#define IO_SERVER "io.adafruit.com"
#define IO_SERVERPORT 1883

/************************** Variables ****************************************/
// Instanciation du client WiFi qui servira à se connecter au broker Adafruit
WiFiClient client;
// Instanciation du client Adafruit avec les informations de connexion
Adafruit_MQTT_Client mqtt_client(&client, IO_SERVER, IO_SERVERPORT, IO_USERNAME, IO_USERNAME, IO_KEY);

/****************************** Feeds ****************************************/
// Création des Feed auxquels nous allons souscrire :
String health_feed = String(IO_USERNAME) + "/feeds/" + CTDevice_ID + ".health";
String other_health_feed = String(IO_OTHER_USERNAME) + "/feeds/" + Other_CDevice_ID + ".health";
Adafruit_MQTT_Subscribe sub_user_health = Adafruit_MQTT_Subscribe(&mqtt_client, health_feed.c_str());
Adafruit_MQTT_Publish pub_user_health = Adafruit_MQTT_Publish(&mqtt_client, health_feed.c_str());
Adafruit_MQTT_Subscribe sub_other_health = Adafruit_MQTT_Subscribe(&mqtt_client, other_health_feed.c_str());
bool is_sub_to_other = false;

/*************************** Sketch Code ************************************/

// Met la santé de l'utilisateur a jour et publie sur le broker
void update_own_health(String data)
{
  if (data == "MALADE" || data == "CONTACT")
  {
    pub_user_health.publish(data.c_str());
  }
  else
  {
    pub_user_health.publish("SAIN");
  }
}

// Callback sur le subscribe au feed santé de l'utilisateur
void own_health_callback(char *data, uint16_t len)
{
  MYDEBUG_PRINT("-AdafruitIO -- Call sur moi : ");
  MYDEBUG_PRINTLN(data);
  // Enregistre l'état de santé de l'utilisateur dans le programme
  set_user_health_status((String)data);
  // Sauvegarde l'état de santé dans les fichiers de l'appareil
  save_health_status();
}

// Callback sur le subscribe au feed santé de l'autre utilisateur
void other_health_callback(char *data, uint16_t len)
{
  // Ignore le callback si l'utilisateur n'est pas supposé être subscribed à l'autre
  if (!is_sub_to_other)
  {
    return;
  }

  MYDEBUG_PRINT("-AdafruitIO -- Call sur other : ");
  MYDEBUG_PRINTLN(data);
  // Met le contact à jour dans les fichiers
  update_contact_health(Other_CDevice_ID, data);

  // Si l'autre utilisateur est malade, l'utilisateur est déclaré cas CONTACT
  if (String(data) == "MALADE" && user_health_status == "SAIN")
  {
    update_own_health("CONTACT");
  }
}

// Mise en place de la connexion à Adafruit IO et définition des sub/pub
void setupAdafruitIO()
{
  // Vérification de la connexion WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    setupWiFi();
  }

  // Subscribe aux feeds de l'utilisateur
  sub_user_health.setCallback(own_health_callback);
  mqtt_client.subscribe(&sub_user_health);

  // Subscribe aux feeds de l'autre utilisateur
  sub_other_health.setCallback(other_health_callback);
  mqtt_client.subscribe(&sub_other_health);

  // NOTE : Le subscribe à l'utilisateur est "simulé" par la variable is_sub_to_other.
  // Tenter de se subscribe à un autre moment, par exemple pendant la loop, rend la liaison
  // à Adafruit instable.
}

// Connexion au broker Adafruit
void connectAdafruitIO()
{
  if (mqtt_client.connected())
  {
    return;
  }

  MYDEBUG_PRINT("-AdafruitIO : Connexion au broker ... ");
  int8_t ret;
  // Boucle provoquant la connexion toutes les 10s tant qu'elle n'est pas étable
  while ((ret = mqtt_client.connect()) != 0)
  {
    MYDEBUG_PRINT("[ERREUR : ");
    MYDEBUG_PRINT(mqtt_client.connectErrorString(ret));
    MYDEBUG_PRINT("] nouvelle tentative dans 10 secondes ...");
    mqtt_client.disconnect();
    delay(10000);
  }
  // Connexion réussie
  MYDEBUG_PRINTLN("[OK]");
}

// Boucle Adafruit recevant les paquets
void loopAdafruitIO()
{
  connectAdafruitIO();
  mqtt_client.processPackets(10000);
  // Si le serveur ne répond pas, coupe la connexion
  if (!mqtt_client.ping())
  {
    mqtt_client.disconnect();
  }
}

// Enregistre le contact avec un autre appareil
void register_contact(int time = 10, String name = "")
{
  if (name == "")
  {
    name = Other_CDevice_ID;
  }

  // Ajoute les informations de contact à l'historique
  int cum_time = add_to_history(name, time);
  // Si l'utilisateur n'est pas sub à l'autre et que le temps de contact total
  // est suffisant, s'y sub.
  if (cum_time >= tracker_time && !is_sub_to_other)
  {
    MYDEBUG_PRINTLN("SUBBING TO OTHER !");
    is_sub_to_other = true;
  }
}
