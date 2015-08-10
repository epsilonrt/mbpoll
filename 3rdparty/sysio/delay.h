/**
 * @file sysio/delay.h
 * @brief Temporisation
 *
 * Copyright © 2014 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#ifndef _SYSIO_DELAY_H_
#define _SYSIO_DELAY_H_
#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_delay Temporisation
 *
 *  Ce module fournit les fonctions de temporisation.
 *  @{
 */

/* internal public functions ================================================ */

/**
 * @brief Temporisation en millisecondes
 *
 * Le thread appelant est endormi le nombre de ms correspondants
 *
 * @param lMs nombre de millisecondes, -1 endort le thread jusqu'à ce qu'il
 * soit réveillé par un signal
 * @return 0 ou -1 en cas d'erreur
 */
int delay_ms (long lMs);

/**
 * @brief Temporisation en microsecondes
 *
 * En fonction de la valeur fournie, cette fonction peut réaliser une attente
 * sur boucle ou alarme (pour des valeurs courtes) ou endormir le thread le
 * nombre de us correspondants.
 *
 * @param lUs nombre de microsecondes, -1 endort le thread jusqu'à ce qu'il
 * soit réveillé par un signal
 * @return 0 ou -1 en cas d'erreur
 */
int delay_us (long lUs);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_DELAY_H_ defined */
