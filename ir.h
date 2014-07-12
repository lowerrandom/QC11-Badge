/*
 * ir.h
 *
 *  Created on: Jun 28, 2014
 *      Author: George
 */

#ifndef IR_H_
#define IR_H_

#include <stdint.h>

void init_ir();
void ir_write_single_byte(uint8_t);
void ir_write(uint8_t*, uint8_t, uint8_t);
void ir_process_rx_ready();
void ir_process_timestep();
uint8_t ir_check_crc();
inline uint8_t ir_paired();

#define IR_PROTO_LISTEN 	0x0
#define IR_PROTO_HELLO_C 	0x11
#define IR_PROTO_ITP_C 		0x12
#define IR_PROTO_PAIRING_C 	0x13
#define IR_PROTO_PAIRED_C 	0x14
#define IR_PROTO_HELLO_S 	0x21
#define IR_PROTO_ITP_S 		0x22
#define IR_PROTO_PAIRING_S 	0x23
#define IR_PROTO_PAIRED_S 	0x24

#endif /* IR_H_ */
