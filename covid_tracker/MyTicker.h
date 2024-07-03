/**
 * \file MyTicker.h
 *
 * \page Ticker
 * \tableofcontents
 *
 * \section ticker_intro Introduction
 *
 * Ce fichier contient l'initialisation du ticker lié à l'envoi régulier de données sur le feed de l'utilisateur.
 * Il ne s'agit pas du seul ticker du programme, mais celui-ci a été séparé pour être visible dans le cadre de la notation.
 *
 * Il peut sembler contre-intuitif de renvoyer régulièrement un état de santé qui a déjà été envoyé au broker lors de sa mise-à-jour, mais sans cette procédure, un autre utilisateur s'abonnant à un feed de santé n'aurait aucune connaissance de l'état de santé de l'autre personne tant que celui-ci n'est pas mis à jour, ce qui n'est pas très pratique si l'autre utilisateur est déjà malade !
 *
 * \section ticker_functions Fonctions
 *
 * Ce fichier définit les fonctions suivantes :
 * - setupTicker : Configuration de l'objet Ticker pour déclencher une fonction à intervalle régulier
 * - ticker_health_update : Fonction appelée périodiquement pour envoye l'état de santé de l'utilisateur.
 *
 * \section ticker_variables Variables Globales
 *
 * Ce fichier définit les variables globales suivantes :
 * - send_ticker : Objet Ticker pour les envois périodiques
 * - SEND_INTERVAL : Intervalle en secondes entre chaque envoi des données. Attention au throtle du compte Adafruit !
 *
 */

#include <Ticker.h>

Ticker send_ticker;
#define SEND_INTERVAL = 300;

// Envoie l'état de santé actuel au feed Adafruit
void ticker_health_update()
{
  update_own_health(user_health_status);
}

// Lie le ticker à la fonction d'envoi des données
void setupTicker()
{
  send_ticker.attach(SEND_INTERVAL, ticker_health_update);
}
