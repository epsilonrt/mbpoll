/**
 * @file sysio/serial.h
 * @brief Liaison série
 *
 * Copyright (c) 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#ifndef _SYSIO_SERIAL_H_
#define _SYSIO_SERIAL_H_
#include <sysio/defs.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_serial Liaison série
 *
 *  Ce module fournit les fonctions permettant de contrôler une liaison série.
 *  @{
 */

/**
 * @brief Erreur de baudrate
 */
#define EBADBAUD (-2)

/**
 * @enum eSerialDataBits
 * @brief Nombre de bits de données
 */
typedef enum {
  SERIAL_DATABIT_5 = 5,
  SERIAL_DATABIT_6 = 6,
  SERIAL_DATABIT_7 = 7,
  SERIAL_DATABIT_8 = 8,
  SERIAL_DATABIT_UNKNOWN = -1
} eSerialDataBits;

/**
 * @enum eSerialParity
 * @brief Parité
 */
typedef enum {
  SERIAL_PARITY_NONE = 'N',
  SERIAL_PARITY_EVEN = 'E',
  SERIAL_PARITY_ODD  = 'O',
  SERIAL_PARITY_SPACE = 'S',
  SERIAL_PARITY_MARK = 'M',
  SERIAL_PARITY_UNKNOWN = -1
} eSerialParity;

/**
 * @enum eSerialStopBits
 * @brief Nombre de bits de stop
 */
typedef enum {
  SERIAL_STOPBIT_ONE = 1,
  SERIAL_STOPBIT_TWO = 2,
  SERIAL_STOPBIT_ONEHALF = 3,
  SERIAL_STOPBIT_UNKNOWN
} eSerialStopBits;

/**
 * @enum eSerialFlow
 * @brief Type de contrôle de flux
 * 
 * @warning Les modes RS485 ne sont pas gérés par termios (spécifiques à linux)
 */
typedef enum {

  SERIAL_FLOW_NONE = ' ', /**< Pas de contrôle de flux */
  SERIAL_FLOW_RTSCTS = 'H', /**< Contrôle de flux matériel RTS/CTS */
  SERIAL_FLOW_XONXOFF = 'S', /**< Contrôle de flux logiciel XON/XOFF */
  SERIAL_FLOW_RS485_RTS_AFTER_SEND = 'R', /**< RS485 half-duplex, RTS au niveau logique 0 après transmission */
  SERIAL_FLOW_RS485_RTS_ON_SEND = 'r', /**< RS485 half-duplex, RTS au niveau logique 0 pendant transmission */
  SERIAL_FLOW_UNKNOWN = -1
} eSerialFlow;

/* structures =============================================================== */
/**
 * Configuration d'un port série
 */
typedef struct xSerialIos {
  long baud; /**< Vitesse de transmission, négative si erreur */
  eSerialDataBits dbits; /**< Bits de données */
  eSerialParity parity; /**< Parité */
  eSerialStopBits sbits;/**< Bits de stop */
  eSerialFlow flow;/**< Contrôle de flux */
  int flag; /**< Réservé pour un usage futur */
} xSerialIos;

/* internal public functions ================================================ */

/**
 * Ouverture d'un port série
 *
 * Le port est ouvert en lecture/écriture non bloquante, 8 bits sans parité,
 * 1 bit de stop.
 *
 * @param device le nom du port à ouvrir (/dev/tty...)
 * @param xIos configuration du port
 * @return le descripteur de fichier du port ou une valeur négative sur erreur
 */
int iSerialOpen (const char *device, xSerialIos * xIos);


/**
 * Fermeture du port série
 * 
 * Equivaut à un appel de close() en ignorant la valeur retournée, il est donc
 * préférable d'utiliser close() en vérifiant la valeur retournée (c.f. page
 * man 2 close)!
 *
 * @param fd le descripteur de fichier du port
 */
void vSerialClose (int fd);

/**
 *  Vide les buffers de réception et de transmission
 *
 * Le buffer de transmission est transmis, celui de réception est vidé.
 *
 * @param fd le descripteur de fichier du port
 */
void vSerialFlush (int fd);

/**
 * Retourne le nombre d'octets en attente de lecture
 *
 * @param fd le descripteur de fichier du port
 * @return le nombre d'octets en attente de lecture, -1 si erreur
 */
int iSerialDataAvailable (int fd);

/**
 * Scrutation en réception du port série
 *
 * @param fd le descripteur de fichier du port
 * @param timeout_ms temps d'attente maximal, une valeur négative pour l'infini
 * @return le nombre d'octets en attente de lecture, -1 si erreur
 */
int iSerialPoll (int fd, int timeout_ms);

/**
 * Modification de configuration d'un port série
 * 
 * @param fd le descripteur de fichier du port
 * @param xIos configuration du port
 * @return 0, -1 si erreur
 */
int iSerialSetAttr (int fd, const xSerialIos * xIos);

/**
 * Lecture de configuration d'un port série
 * 
 * @param fd le descripteur de fichier du port
 * @param xIos configuration du port lue
 * @return 0, -1 si erreur
 */
int iSerialGetAttr (int fd, xSerialIos * xIos);

/**
 * Chaîne de caractère décrivant la configuration correspondant aux paramètres
 *
 * Le format est BBBBBB-DPSF avec :
 * - BBBBBB Baudrate
 * - D Data bits (5, 6, 7 ,8)
 * - P Parité (N, E, O)
 * - S Stop (1, 2)
 * - F Flow (H, S, R, r)
 * .
 * @return la représentation de la configuration sous forme de string, NULL si
 * erreur.
 */
const char * sSerialAttrToStr (const xSerialIos * xIos);

/**
 * @brief Vérifie si le descripteur de fichier est valide
 * 
 * @param fd le descripteur de fichier du port
 * @return true si valide
 */
bool bSerialFdIsValid (int fd);

/**
 *  Lecture de la vitesse de transmission
 *
 * @param xPort Pointeur sur le port
 * @return la valeur, -1 si erreur.
 */
int iSerialGetBaudrate (int fd);

/**
 *  Lecture du nombre de bits de données
 *
 * @param xPort Pointeur sur le port
 * @return la valeur, -1 si erreur.
 */
eSerialDataBits eSerialGetDataBits (int fd);

/**
 *  Lecture de la parité
 *
 * @param xPort Pointeur sur le port
 * @return la valeur, -1 si erreur.
 */
eSerialParity eSerialGetParity (int fd);

/**
 *  Lecture du nombre de bits de stop
 *
 * @param xPort Pointeur sur le port
 * @return la valeur, -1 si erreur.
 */
eSerialStopBits eSerialGetStopBits (int fd);

/**
 *  Lecture du type de contrôle de flux en cours
 *
 * @param xPort Pointeur sur le port
 * @return la valeur, -1 si erreur.
 */
eSerialFlow eSerialGetFlow (int fd);

/**
 * Modification de la vitesse de transmission
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur.
 */
int iSerialSetBaudrate (int fd, int iBaudrate);

/**
 * Modification du nombre de bits de données
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur.
 */
int iSerialSetDataBits (int fd, eSerialDataBits eDataBits);

/**
 * Modification de la parité
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur.
 */
int iSerialSetParity (int fd, eSerialParity eParity);

/**
 * Modification du nombre de bits de stop
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur.
 */
int iSerialSetStopBits (int fd, eSerialStopBits eStopBits);

/**
 *  Modification du type de contrôle de flux
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur.
 */
int iSerialSetFlow (int fd, eSerialFlow eNewFlow);

/**
 *  Lecture du type de contrôle de flux en cours sous forme "lisible"
 *
 * @param fd le descripteur de fichier du port
 */
const char * sSerialGetFlowStr (int fd);

/**
 * Chaîne de caractère décrivant la configuration du port
 *
 * Le format est BBBBBB-DPSF avec :
 * - BBBBBB Baudrate
 * - D Data bits (5, 6, 7 ,8)
 * - P Parité (N, E, O)
 * - S Stop (1, 2)
 * - F Flow (H, S, R, r)
 * .
 * @param fd le descripteur de fichier du port
 * @return la représentation de la configuration sous forme de string, NULL si
 * erreur.
 */
const char * sSerialAttrStr (int fd);

#ifdef SYSIO_OS_POSIX
// -----------------------------------------------------------------------------

#include <termios.h>

/**
 * Modification de configuration d'un port série d'une structure termios
 * 
 * @param ts structure termios
 * @param xIos configuration du port
 * @return 0, -1 si erreur
 */
int iSerialTermiosSetAttr (struct termios * ts, const xSerialIos * xIos);

/**
 * Lecture de configuration d'un port série d'une structure termios
 * 
 * @param ts structure termios
 * @param xIos configuration du port lue
 * @return 0, -1 si erreur
 */
int iSerialTermiosGetAttr (const struct termios * ts, xSerialIos * xIos);

/**
 * Modification du baudrate d'une structure termios
 */
int iSerialTermiosSetBaudrate (struct termios * ts, int iBaudrate);

/**
 * Modification data bits d'une structure termios
 */
int iSerialTermiosSetDataBits (struct termios * ts, eSerialDataBits eDataBits);

/**
 * Modification du parité d'une structure termios
 */
int iSerialTermiosSetParity (struct termios * ts, eSerialParity eParity);

/**
 * Modification du stop bits d'une structure termios
 */
int iSerialTermiosSetStopBits (struct termios * ts, eSerialStopBits eStopBits);

/**
 * Modification du contrôle de flux d'une structure termios
 * La fonction règle ts à SERIAL_FLOW_NONE si un mode RS485 est demandé 
 * (RS485 non géré par termios).
 */
int iSerialTermiosSetFlow (struct termios * ts, eSerialFlow eFlow);

/**
 * Baudrate d'une structure termios
 */
int iSerialTermiosGetBaudrate (const struct termios * ts);

/**
 * Data bits d'une structure termios
 */
int iSerialTermiosGetDataBits (const struct termios * ts);

/**
 * Stop bits d'une structure termios
 */
int iSerialTermiosGetStopBits (const struct termios * ts);

/**
 * Parité d'une structure termios
 */
int iSerialTermiosGetParity (const struct termios * ts);

/**
 * Contrôle de flux d'une structure termios
 * La fonction retourne SERIAL_FLOW_NONE si un mode RS485 est en fonction 
 * (RS485 non géré par termios).
 */
int iSerialTermiosGetFlow (const struct termios * ts);

/**
 *  Chaîne de caractère correspondant à une structure termios
 *
 * La fonction affiche SERIAL_FLOW_NONE si un mode RS485 est en fonction 
 * (RS485 non géré par termios).
 * Le format est BBBBBB-DPSF avec :
 * - BBBBBB Baudrate
 * - D Data bits (5, 6, 7 ,8)
 * - P Parité (N, E, O)
 * - S Stop (1, 2)
 * - F Flow (H, S)
 * .
 */
const char * sSerialTermiosToStr (const struct termios * ts);

/**
 * Durée d'une trame de ulSize octets en secondes
 */
double dSerialTermiosFrameDuration (const struct termios * ts, size_t ulSize);

/**
 *  Baudrate associée à une constante speed_t
 */
int iSerialSpeedToInt (speed_t speed);

/**
 *  Constante speed_t associée à une valeur en baud
 */
speed_t eSerialIntToSpeed (int baud);

// -----------------------------------------------------------------------------
#endif /* SYSIO_OS_POSIX defined */

/**
 *  Chaîne de caractère correspondant à une valeur de contrôle de flux
 */
const char * sSerialFlowToStr (eSerialFlow eFlow);

/**
 *  Chaîne de caractère correspondant à une valeur de parité
 */
const char * sSerialParityToStr (eSerialParity eParity);

/**
 *  Chaîne de caractère correspondant à une valeur de bits de données
 */
const char * sSerialDataBitsToStr (eSerialDataBits eDataBits);

/**
 *  Chaîne de caractère correspondant à une valeur de bits de stop
 */
const char * sSerialStopBitsToStr (eSerialStopBits eStopBits);

/**
 * Durée d'une trame de ulSize octets en secondes
 *
 * @param fd le descripteur de fichier du port
 */
double dSerialFrameDuration (int fd, size_t ulSize);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_SERIAL_H_ */
