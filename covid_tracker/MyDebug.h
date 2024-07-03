
/**
 * \file MyDebug.h
 *
 * \page MyDebug
 * \tableofcontents
 *
 * \section debug_intro Introduction
 *
 * Ce fichier contient les déclarations et les définitions des fonctions pour le débogage du projet.
 *
 * Il a été réalisé par Maxime Bui dans le cadre du cours lié à l'IoT et est réutilisé tel quel dans ce projet.
 *
 * \section debug_macros Macros de Débogage
 *
 * Ce fichier définit les macros suivantes pour le débogage :
 * - MYDEBUG_PRINT(x) : Imprime x sur le port série si le débogage est activé
 * - MYDEBUG_PRINTDEC(x) : Imprime x en décimal sur le port série si le débogage est activé
 * - MYDEBUG_PRINTHEX(x) : Imprime x en hexadécimal sur le port série si le débogage est activé
 * - MYDEBUG_PRINTLN(x) : Imprime x avec un retour à la ligne sur le port série si le débogage est activé
 * - MYDEBUG_PRINTF(a, b, c, d, e) : Imprime avec formatage sur le port série si le débogage est activé
 *
 */

#ifdef MYDEBUG
#define MYDEBUG_PRINT(x) Serial.print(x)
#define MYDEBUG_PRINTDEC(x) Serial.print(x, DEC)
#define MYDEBUG_PRINTHEX(x) Serial.print(x, HEX)
#define MYDEBUG_PRINTLN(x) Serial.println(x)
#define MYDEBUG_PRINTF(a, b, c, d, e) Serial.printf(a, b, c, d, e)
#else
#define MYDEBUG_PRINT(x)
#define MYDEBUG_PRINTDEC(x)
#define MYDEBUG_PRINTHEX(x)
#define MYDEBUG_PRINTLN(x)
#define MYDEBUG_PRINTF(x)
#endif

void setupDebug()
{
#ifdef MYDEBUG
  Serial.begin(115200);
  MYDEBUG_PRINTLN("Ouverture du port série");
#endif
}
