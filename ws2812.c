#if !BADGE_TARGET
/*
 * Software functions for MSP430 to drive WS2812/B RGB LEDs via one-wire bus
 *
 * The SPI peripheral will be used in cooperation with one of two transport stream encodings schemes.
 * One 3-bit and one 4-bit encoding was implemented.
 *
 */

#include "qcxi.h"
#include "ws2812.h"


uint8_t ws_frameBuffer[(ENCODING * sizeof(ledcolor_t) * NUMBEROFLEDS)] = { 0, };

void ws2812_init() {
	USCI_B_SPI_masterInit(
		USCI_B0_BASE,
		USCI_B_SPI_CLOCKSOURCE_SMCLK,
		8000000, // should always be so.
		2666666,
		USCI_B_SPI_MSB_FIRST,
		USCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
		USCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW
	);

	USCI_B_SPI_enable(USCI_B0_BASE);

	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3, GPIO_PIN0);
}

void shiftLed(ledcolor_t* leds, ledcount_t ledCount) {
	ledcolor_t tmpLed;
	ledcount_t ledIdx;

	tmpLed = leds[ledCount-1];
	for(ledIdx=(ledCount-1); ledIdx > 0; ledIdx--) {
		leds[ledIdx] = leds[ledIdx-1];
	}
	leds[0] = tmpLed;
}

// copy bytes from the buffer to SPI transmit register
// should be reworked to use DMA
void sendBuffer(uint8_t* buffer, ledcount_t ledCount) {
	uint16_t bufferIdx;
	__bic_SR_register(GIE); // TODO: need to make this interrupt based:
	for (bufferIdx=0; bufferIdx < (ENCODING * sizeof(ledcolor_t) * ledCount); bufferIdx++) {
		while (!(UCB0IFG & UCTXIFG));		// wait for TX buffer to be ready
		UCB0TXBUF = buffer[bufferIdx];
	}
	__bis_SR_register(GIE);
	__delay_cycles(300);
}


void fillFrameBuffer(ledcolor_t* leds, ledcount_t ledCount, uint8_t* buffer, uint8_t encoding) {
	// encoding is 3, like it or not.
	ledcount_t ledIdx;
	uint16_t fbIdx;

	fbIdx = 0;
	for (ledIdx = 0; ledIdx < ledCount; ledIdx++) {
		encodeData3bit(&leds[ledIdx], &buffer[fbIdx]);
		fbIdx += (3 * sizeof(ledcolor_t));
	}
}

void fillFrameBufferSingleColor(ledcolor_t* led, ledcount_t ledCount, uint8_t* buffer, uint8_t encoding) {
	//encoding is 3, like it or not.
	ledcount_t ledIdx;
	uint16_t fbIdx;

	fbIdx = 0;
	for (ledIdx = 0; ledIdx < ledCount; ledIdx++) {
		encodeData3bit(led, &buffer[fbIdx]);
		fbIdx += (3 * sizeof(ledcolor_t));
	}
}

/*
 ******************
 * 3-bit encoding *
 ******************
 *
 * 8 bits from LED color stream encoded in 3 byte for transport stream (SPI TX)
 * or: 1 bit from LED color stream encoded in 3 bit for transport stream
 *
 *				_
 * ZERO: 100	 |__
 *	 	 	 	__
 * ONE : 110	  |_
 *
 * the bit   in the middle defines the value
 *
 * data stream: 0x23		 		 0  0  1  0  0  0  1  1
 * encoding:						1x01x01x01x01x01x01x01x0
 * transport stream:				100100110100100100110110
 *
 * initial mask: 0x92 0x49 0x24		100100100100100100100100
 *
 * sourcebit :						 7  6  5  4  3  2  1  0
 * encoding  :						1x01x01x01x01x01x01x01x0
 * targetbit :						 6  3  0  5  2  7  4  1
 * targetbyte:						|   0   |   1   |   2   |
 *
 * sourcebit -> (targetbit,targetbyte)
 * 7->(6,0)
 * 6->(3,0)
 * 5->(0,0)
 * 4->(5,1)
 * 3->(2,1)
 * 2->(7,2)
 * 1->(4,2)
 * 0->(1,2)
 */
void encodeData3bit(ledcolor_t* led, uint8_t* output) {
	uint8_t colorIdx, outputIdx;
	uint8_t grbLED[sizeof(*led)];	// reordered color order
	uint8_t shiftRegister;

	// WS2812 is expecting GRB instead of RGB
	grbLED[0] = led->green;
	grbLED[1] = led->red;
	grbLED[2] = led->blue;

	outputIdx = 0;
	// loop over the color bytes and convert each bit to three bits for transport stream
	for (colorIdx=0; colorIdx < sizeof(grbLED); colorIdx++) {
		// prepare frameBuffer with initial transport bitmask
		output[outputIdx+0] = 0x92;
		output[outputIdx+1] = 0x49;
		output[outputIdx+2] = 0x24;

		/*
		 * bit remapping starts here
		 */

		// right shift bits
		shiftRegister = grbLED[colorIdx];
		shiftRegister >>= 1;	// 1 shift from original
		output[outputIdx+0] |= (shiftRegister & BIT6);	// source bit 7
		output[outputIdx+1] |= (shiftRegister & BIT2);	// source bit 3
		shiftRegister >>= 2;	// 3 shifts from original
		output[outputIdx+0] |= (shiftRegister & BIT3);	// source bit 6
		shiftRegister >>= 2;	// 5 shifts from original
		output[outputIdx+0] |= (shiftRegister & BIT0);	// source bit 5

		// left shift bits
		shiftRegister = grbLED[colorIdx];
		shiftRegister <<= 1;	// 1 shift from original
		output[outputIdx+1] |= (shiftRegister & BIT5);	// source bit 4
		output[outputIdx+2] |= (shiftRegister & BIT1);	// source bit 0
		shiftRegister <<= 2;	// 3 shifts from original
		output[outputIdx+2] |= (shiftRegister & BIT4);	// source bit 1
		shiftRegister <<= 2;	// 5 shifts from original
		output[outputIdx+2] |= (shiftRegister & BIT7);	// source bit 2

		outputIdx += 3;	// next three bytes (color)
	}
}
#endif