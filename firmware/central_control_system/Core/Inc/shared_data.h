/*
 * shared_data.h
 *
 *  Created on: May 16, 2021
 *      Author: tamas
 */

#ifndef INC_SHARED_DATA_H_
#define INC_SHARED_DATA_H_

typedef struct {
	float roll;
	float pitch;
	float yaw;
}SD_euler_angles_t;

extern SD_euler_angles_t SD_angles;

#endif /* INC_SHARED_DATA_H_ */
