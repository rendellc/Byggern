/*!@file
 * Interface for high level CAN bus controller. 
 */

#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_
#include "global_declarations.h"

/*!
 * Initialize mcp2515 and can controller. 
 */
void can_init();

/*!
 * Send message on can bus
 * @param msg Message to send out on the bus
 * @param tx_buffer_select Which tx buffer to use for transmission
 */
void can_send(can_msg_t msg, uint8_t tx_buffer_select);

/*!
 * Get oldest message on can buffer
 * @param rx_buffer_select Which buffer to read from
 * @return return the oldest message in the buffer. If buffer is empty, then returned
 *         message will have SID = MSG_INVALID and length=0. 
 */
can_msg_t can_read_buffer(uint8_t rx_buffer_select);

/// Function for printing message to uart stream in a nicely formatted way. 
void can_print_msg(can_msg_t msg); 

#endif /* CAN_DRIVER_H_ */