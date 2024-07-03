/**
 * \file DeepSleep.h
 *
 * \page DeepSleep
 * \tableofcontents
 *
 * \section deepsleep_intro Introduction
 *
 * Ce fichier contient les déclarations et les définitions des fonctions pour la gestion du mode Deep Sleep.
 *
 * \section deepsleep_functions Fonctions
 *
 * Ce fichier définit les fonctions suivantes :
 * - myDeepSleep : Met le système en mode Deep Sleep pendant une durée spécifiée
 * - setupDeepSleepTicker : Initialise le Ticker associé à la fonction myDeepSleep
 *
 * \section deepsleep_variables Variables Globales
 *
 * Ce fichier définit les variables globales suivantes :
 * - uS_TO_S_FACTOR : Facteur de conversion de microsecondes en secondes
 * - SLEEP_TIME : Durée d'endormissement en secondes
 * - SLEEP_INTERVAL : Durée entre chaque endormissement en secondes
 * - myDeepSleepTicker : Ticker pour la gestion des interruptions pour le mode Deep Sleep
 * - bootCount : Compteur d'endormissement/démarrage conservé en mémoire RTC
 */

#define uS_TO_S_FACTOR 1000000
#define SLEEP_TIME 5       // temps d'endormissement (s)
#define SLEEP_INTERVAL 120 // intervalle entre chaque endormissement (s)

Ticker myDeepSleepTicker;        // ticker lié à l'endormissement
RTC_DATA_ATTR int bootCount = 0; // variable permanante gardant trace du nombre d'endormissements

// Provoque l'endormissement de l'appareil
void myDeepSleep()
{
  MYDEBUG_PRINT("-DEEPSLEEP : ");
  MYDEBUG_PRINTLN(bootCount++);
  MYDEBUG_PRINTLN("-DEEPSLEEP : DeepSleep pendant 3 secondes");
  esp_sleep_enable_timer_wakeup(SLEEP_TIME * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

// Associe la fonction d'endormissement au ticker
void setupDeepSleepTicker()
{
  MYDEBUG_PRINTLN("-TICKER : Initialisation d'un ticker DeepSleep toutes les 120 secondes");
  myDeepSleepTicker.attach(SLEEP_INTERVAL, myDeepSleep);
}
