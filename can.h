/*
 * can_driver.h
 *
 * Created: 06.10.2017 09:07:01
 *  Author: rendellc
 */ 


#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_
#include "global_declarations.h"

void can_init();

void can_send(can_msg_t msg, uint8_t tx_buffer_select);

can_msg_t can_read_buffer(uint8_t rx_buffer_select);


#endif /* CAN_DRIVER_H_ */