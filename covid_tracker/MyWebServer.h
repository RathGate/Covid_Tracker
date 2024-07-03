/**
 * \file MyWebServer.h
 *
 * \page WebServer
 * \tableofcontents
 *
 * \section webserver_intro Introduction
 *
 * Ce fichier contient les déclarations et les définitions des fonctions pour la gestion du serveur web.
 *
 * \section webserver_functions Fonctions
 *
 * Ce fichier définit les fonctions suivantes :
 * - setupWebServer : Configuration et démarrage du serveur web
 * - loopWebServer : Gestion des requêtes clients dans la boucle principale
 *
 * \section webserver_handlers Handlers de Requêtes
 *
 * Ce fichier définit les handlers de requêtes suivants :
 * - handleRoot : Gestion de la requête pour la racine du serveur web
 * - handleScan : Gestion de la requête pour la page de scan des réseaux WiFi
 * - handleConnect : Gestion de la requête pour la connexion à un réseau WiFi
 * - handleConfig : Gestion de la requête pour la configuration de l'appareil
 * - handleHealth : Gestion de la requête pour l'état de santé de l'appareil
 * - handleHistory : Gestion de la requête pour l'historique de rencontre de l'appareil
 * - handleFormat : Gestion de la requête pour le formatage du SPIFFS
 * - handleNotFound : Gestion de la requête pour les routes non trouvées
 *
 * \section webserver_variables Variables Globales
 *
 * Ce fichier définit les variables globales suivantes :
 * - monWebServeur : Objet WebServer pour la gestion du serveur web
 * - isConnected : Statut de connexion au serveur
 * - SSID : Nom du réseau WiFi
 * - password : Mot de passe du réseau WiFi
 */
#include <WebServer.h>

// Variables
WebServer monWebServeur(80); // Serveur web sur le port 80

bool isConnected;
String SSID;
String password;

void handleRoot()
{
  MYDEBUG_PRINTLN("-WEBSERVER : requete root");

  // Préparation du code HTML à retourner
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='30'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; } .title { display: flex; justify-content: space-between; }</style>";
  out += "</head><body>";
  out += "<div class=\"title\"><h1>Bienvenue</h1>";
  if (WiFi.status() == WL_CONNECTED)
  {
    out += "<p>&#128732;</p>";
  }
  out += "</div><br>";
  out += "Depuis cette page, vous pouvez<br><ul>";
  out += "<li><a href=\"scan\"> Se connecter au WiFi</a></li>";
  out += "<li><a href=\"health\"> Mon etat de sante</a></li>";
  out += "<li><a href=\"history\"> Mon historique de rencontre</a></li>";
  // out += "<li><a href=\"format\"> Formater le SPIFFS</a></li>";
  out += "<li><a href=\"config\"> Configurer l'appareil</a></li></ul>";
  out += "</body></html>";

  // Envoi de la réponse en HTML
  monWebServeur.send(200, "text/html", out);
}

void handleScan()
{

  MYDEBUG_PRINTLN("-WEBSERVER : requete scan");

  // La carte scanne les réseaux WiFi à proximité
  int n = WiFi.scanNetworks();

  // Construction de la réponse HTML
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='30'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Page de scan</h1><br>";

  // Intégration des réseaux WiFi trouvés dans la page HTML
  // On créé un form par réseau trouvé, avec un input caché contenant le SSID, et un input pour le mot de passe.
  // Form en POST pour récupérer les informations depuis la requête
  if (n == 0)
  {
    MYDEBUG_PRINTLN("- AUCUN réseau WiFi trouvé");
  }
  else
  {
    out += "<ul>";
    for (int i = 0; i < n; ++i)
    {
      out += "<form action='/connect' method='POST'>";
      out += "<p>" + WiFi.SSID(i) + "</p>";
      out += "<input hidden name='SSID' value='" + WiFi.SSID(i) + "'>";
      out += "<input type='text' name='password' placeholder='password' required>";
      out += "<input type='submit' value='Connect'>";
      out += "</form>";
    }
    out += "</ul>";
  }

  // Fin de la réponse HTML
  out += "</body></html>";

  // Envoi de la page HTML
  monWebServeur.send(200, "text/html", out);
}

void handleConnect()
{
  // On vérifie si une requête POST est detectée
  // Si c'est le cas on vérifie le nombre d'arguments dans le lien et si les noms sont bien SSID/password
  if (monWebServeur.method() == HTTP_POST)
  {
    MYDEBUG_PRINT("WEBSERVER : SCAN ARGUMENTS : ");
    if (monWebServeur.args() > 0)
    {
      MYDEBUG_PRINTLN("WEBSERVER - ARGUMENT SSID/PASSWORD");
      for (int j = 0; j < monWebServeur.args(); ++j)
      {
        if (monWebServeur.argName(j) == "SSID")
        {
          SSID = monWebServeur.arg(monWebServeur.argName(j));
        }
        else if (monWebServeur.argName(j) == "password")
        {
          password = monWebServeur.arg(monWebServeur.argName(j));
        }
      }

      if (SSID && password)
      {
        if (isConnected = TryWifiConnexion(SSID, password))
        {
          updateNetwork("wan", SSID, password);
        }
      }
    }
  }

  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='30'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Connexion en cours</h1><br>";
  if (isConnected)
  {
    out += "Connexion reussie, <a href='/'>retour au menu</a>";
  }
  else
  {
    out += "Echec de connexion, <a href='/scan'>retour a la liste des reseaux</a>";
  }
  // Fin de la réponse HTML
  out += "</body></html>";

  // Envoi de la page HTML
  monWebServeur.send(200, "text/html", out);
}

void handleConfig()
{
  SSID = "";
  password = "";
  MYDEBUG_PRINTLN("-WEBSERVER : requete config");

  // Construction de la réponse HTML
  String out = "";
  out += "<html><head><meta http-equiv='' content='30'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; } .pad, label, input, button { margin: 5px 0 }</style>";
  out += "</head><body>";
  out += "<h1>Page de config</h1>";
  out += "<form id=\"form-config\" style=\"display: inline-grid\" method=\"POST\" action=\"config\">";
  out += "<div class=\"pad\"><b>Nom de l'appareil</b> : " + lan_ssid + "</div><br>";
  out += "<label for=\"lan_pswd\">Mot de passe du point d'acces</label>";
  out += "<input name=\"lan_pswd\" placeholder=" + lan_pswd + " value=" + lan_pswd + " minlength=\"8\" required>";
  out += "<label for=\"tracker_distance\">Distance de detection (metres)</label>";
  out += "<input name=\"tracker_distance\" placeholder=" + (String)tracker_distance + " value=" + tracker_distance + " type=\"number\" min=\"1\" max=\"5\" required>";
  out += "<label for=\"tracker_time\">Temps de contact (secondes)</label>";
  out += "<input name=\"tracker_time\" placeholder=" + (String)tracker_time + " value=" + tracker_time + " type=\"number\" min=\"60\" max=\"600\" required>";
  out += "<button type=\"submit\">Mettre a jour</button>";
  out += "</form>";
  out += "<script> document.getElementById(\"form-config\").addEventListener(\"submit\", (event) => { event.preventDefault(); let form = event.target; form.submit();})</script>";
  out += "</body></html>";

  if (monWebServeur.method() == HTTP_POST)
  {
    if (monWebServeur.args() > 0)
    {
      MYDEBUG_PRINTLN("POST REQUEST CONFIG");
      for (int i = 0; i < monWebServeur.args(); ++i)
      {
        if (monWebServeur.argName(i) == "device_name")
        {
          // SSID = monWebServeur.arg(monWebServeur.argName(i));
        }
        else if (monWebServeur.argName(i) == "lan_pswd")
        {
          password = monWebServeur.arg(monWebServeur.argName(i));
        }
        else
        {
          updateDeviceConfig(monWebServeur.argName(i), monWebServeur.arg(monWebServeur.argName(i)));
        }
      }

      updateNetwork("lan", SSID, password);
    }
  }

  // Envoi de la réponse HTML
  monWebServeur.send(200, "text/html", out);
}

/**
 * Fonction de gestion de la route /health
 */
void handleHealth()
{
  MYDEBUG_PRINTLN("-WEBSERVER : requete etat de sante");

  // Construction de la réponse HTML
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='5'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; } button { border: none; outline: inherit; padding: 10px; } .buttons_container { display: flex; gap: 15px; } .sain { color: #58D68D; } .contact { color: #F39C12; } .malade { color: #E74C3C; }</style>";
  out += "</head><body>";
  out += "<h1>Mon etat de sante</h1>";
  out += "<p><b>Mon nom</b> : <span style=\"text-transform: capitalize;\">" + CTDevice_ID + "</span><br><b>Mon etat</b> : <span style=\"text-transform: lowercase;\">" + user_health_status + "</span></p>";
  out += "<div class=\"buttons_container\">";
  out += "<form action=\"health\" method=\"post\" class=\"form-example\">";
  out += "<button name=\"health_status\" style=\"background-color: #58D68D;\" type=\"submit\" value=\"SAIN\">SAIN</button>";
  out += "</form>";
  out += "<form action=\"health\" method=\"post\" class=\"form-example\">";
  out += "<button name=\"health_status\" style=\"background-color: #F39C12;\" type=\"submit\" value=\"CONTACT\">CAS CONTACT</button>";
  out += "</form>";
  out += "<form action=\"health\" method=\"post\" class=\"form-example\">";
  out += "<button name=\"health_status\" style=\"background-color: #E74C3C;\" type=\"submit\" value=\"MALADE\">MALADE</button>";
  out += "</form>";
  out += "</div>";
  out += "</body></html>";

  if (monWebServeur.method() == HTTP_POST)
  {
    if (monWebServeur.args() > 0)
    {
      MYDEBUG_PRINTLN("POST REQUEST CONFIG");
      for (int i = 0; i < monWebServeur.args(); ++i)
      {
        if (monWebServeur.argName(i) == "health_status")
        {
          update_own_health(monWebServeur.arg(monWebServeur.argName(i)));
        }
      }
    }
  }

  // Envoi de la réponse HTML
  monWebServeur.send(200, "text/html", out);
}

void handleHistory()
{
  // Construction de la réponse HTML
  String out = "";
  out += "<html><head>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; } button { border: none; outline: inherit; padding: 10px; } th, td { padding: 5px; } .buttons_container { display: flex; gap: 15px; } .sain { color: #58D68D; } .contact { color: #F39C12; } .malade { color: #E74C3C; }</style>";
  out += "</head><body>";
  out += "<h1>Mon historique de rencontre</h1>";
  out += "<p><b>Mon nom</b> : <span style=\"text-transform: capitalize;\">" + CTDevice_ID + "</span><br><b>Mon etat</b> : <span style=\"text-transform: lowercase;\">" + user_health_status + "</span></p>";
  out += "<table>";
  out += "<thead>";
  out += "<tr>";
  out += "<th scope=\"col\" style=\"text-transform: capitalize;\">Personne</th>";
  out += "<th scope=\"col\">Duree du contact</th>";
  out += "<th scope=\"col\">Derniere rencontre</th>";
  out += "<th scope=\"col\">Etat de sante</th>";
  out += "</tr>";
  out += "</thead>";
  out += "<tbody>";

  DynamicJsonDocument history = getHistory();

  for (JsonObject elem : history.as<JsonArray>())
  {

    const char *name = elem["name"];
    const int time_spent = elem["time_spent"];
    const char *last_encountered = elem["last_encountered"];
    const char *health_status = elem["health_status"];

    out += "<tr>";
    out += "<th scope=\"row\">";
    out += name;
    out += "</th>";
    out += "<td>";
    out += time_spent;
    out += "</td>";
    out += "<td>";
    out += last_encountered;
    out += "</td>";
    out += "<td>";
    out += health_status;
    out += "</td>";
    out += "</tr>";
  }

  out += "</tbody>";
  out += "</table>";
  out += "</body></html>";
  // Envoi de la réponse HTML
  monWebServeur.send(200, "text/html", out);
}

void handleNotFound()
{
  MYDEBUG_PRINTLN("-WEBSERVER : erreur de route");

  // Construction de la réponse HTML
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += monWebServeur.uri();
  message += "\nMethod: ";
  message += (monWebServeur.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += monWebServeur.args();
  message += "\n";
  for (uint8_t i = 0; i < monWebServeur.args(); i++)
  {
    message += " " + monWebServeur.argName(i) + ": " + monWebServeur.arg(i) + "\n";
  }

  // Envoi de la réponse HTML
  monWebServeur.send(404, "text/plain", message);
}

void setupWebServer()
{
  // On a besoin d'une connexion WiFi !
  // Test de la connexion WiFi. Si elle n'est pas active alors on démarre le WiFi
  // if (WiFi.status() != WL_CONNECTED){setupWiFi();}  // Connexion WiFi
  MYDEBUG_PRINTLN("-WEBSERVER : Démarrage");

  // Configuration de mon serveur web en définissant plusieurs routes
  // A chaque route est associée une fonction
  monWebServeur.on("/", handleRoot);
  monWebServeur.on("/scan", handleScan);
  monWebServeur.on("/config", handleConfig);
  monWebServeur.on("/health", handleHealth);
  monWebServeur.on("/history", handleHistory);
  monWebServeur.onNotFound(handleNotFound);
  monWebServeur.on("/connect", handleConnect);

  monWebServeur.begin(); // Démarrage du serveur
  MYDEBUG_PRINTLN("-WEBSERVER : Serveur Web démarré");
}

void loopWebServer(void)
{
  monWebServeur.handleClient();
}
