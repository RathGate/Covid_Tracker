/** \file MyCore0.h
 *
 * \page Core0
 * \tableofcontents
 *
 * \section core0_intro Introduction
 *
 * Ce fichier contient les déclarations et les définitions des fonctions pour l'utilisation du Core 0 de l'ESP32.
 *
 * Ici, le Core 0 est utilisé pour faire tourner le serveur web en arrière plan, sans qu'il soit ralenti par les autres tâches répétitives.
 *
 * \section core0_functions Fonctions
 *
 * Ce fichier définit les fonctions suivantes :
 * - core0_loop2 : Fonction exécutée en boucle sur le Core 0 pour gérer certaines tâches
 * - core0_setup : Configuration de la tâche exécutée sur le Core 0
 */

TaskHandle_t Task1;

void loop2(void *parameter)
{
  for (;;)
  {
    loopWebServer(); // Boucle du serveur web
    delay(20);       // Délai pour laisser au coeur le temps de faire d'autres tâches
  }
}

void setupMyCore0()
{
  // Initialisation d'une tâche que nous allons mettre sur le Core 0
  xTaskCreatePinnedToCore(
      loop2,        // Nom de la fonction associée à la tâche
      "secondLoop", // Nom de la tâche
      10000,        // Taille mémoire assignée à la tâche
      NULL,         // Mettre NULL dans tous les cas
      0,            // Priorité de la tâche : 0 étant la haute priorité. *IMPORTANT* Si priorité 1 et pas d'activité sur la tâche alors l'ESP est redémarré
      &Task1,       // Reference d'une variable taskHandle
      0);           // Choisir le core 0 ou 1
}
