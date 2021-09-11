/*
 * bmmcp_slave.h
 *
 *  Created on: Jul 25, 2021
 *      Author: tamas
 */

#ifndef APPLICATION_USER_BMMCP_SLAVE_H_
#define APPLICATION_USER_BMMCP_SLAVE_H_

#include "bmmcp/bmmcp_common.h"
#include "stdint.h"
void BMMCP_slave_process(int16_t mech_speed, int16_t current);
#endif /* APPLICATION_USER_BMMCP_SLAVE_H_ */
