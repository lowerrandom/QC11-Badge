/*
 * fonts.c
 *
 *  Created on: Jun 9, 2014
 *      Author: George
 */

#include "qcxi.h"
#include "fonts.h"

/* Character bitmaps for D3 5pt */
const uint8_t d3_5ptBitmaps[] =
{
	/* @0 ' ' (2 pixels wide) */
	//
	//
	//
	//
	//
	0b00000000, 0b00000000,

	/* @2 '!' (1 pixels wide) */
	// #
	// #
	// #
	//
	// #
	0b00010111,

	/* @3 '"' (3 pixels wide) */
	// # #
	// # #
	//
	//
	//
	0b00000011, 0b00000000, 0b00000011,

	/* @6 '#' (5 pixels wide) */
	//  # #
	// #####
	//  # #
	// #####
	//  # #
	0b00001010, 0b00011111, 0b00001010, 0b00011111, 0b00001010,

	/* @11 '$' (5 pixels wide) */
	// #####
	// # #
	// #####
	//   # #
	// #####
	0b00010111, 0b00010101, 0b00011111, 0b00010101, 0b00011101,

	/* @16 '%' (5 pixels wide) */
	// #   #
	//    #
	//   #
	//  #
	// #   #
	0b00010001, 0b00001000, 0b00000100, 0b00000010, 0b00010001,

	/* @21 '&' (5 pixels wide) */
	// ####
	// #
	// ### #
	// #   #
	// #####
	0b00011111, 0b00010101, 0b00010101, 0b00010001, 0b00011100,

	/* @26 ''' (1 pixels wide) */
	// #
	// #
	//
	//
	//
	0b00000011,

	/* @27 '(' (2 pixels wide) */
	//  #
	// #
	// #
	// #
	//  #
	0b00001110, 0b00010001,

	/* @29 ')' (2 pixels wide) */
	// #
	//  #
	//  #
	//  #
	// #
	0b00010001, 0b00001110,

	/* @31 '*' (5 pixels wide) */
	// # # #
	//  ###
	//   #
	//  ###
	// # # #
	0b00010001, 0b00001010, 0b00011111, 0b00001010, 0b00010001,

	/* @36 '+' (5 pixels wide) */
	//   #
	//   #
	// #####
	//   #
	//   #
	0b00000100, 0b00000100, 0b00011111, 0b00000100, 0b00000100,

	/* @41 ',' (1 pixels wide) */
	//
	//
	//
	// #
	// #
	0b00011000,

	/* @42 '-' (5 pixels wide) */
	//
	//
	// #####
	//
	//
	0b00000100, 0b00000100, 0b00000100, 0b00000100, 0b00000100,

	/* @47 '.' (1 pixels wide) */
	//
	//
	//
	//
	// #
	0b00010000,

	/* @48 '/' (5 pixels wide) */
	//     #
	//    #
	//   #
	//  #
	// #
	0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001,

	/* @53 '0' (5 pixels wide) */
	//  ###
	// #  ##
	// # # #
	// ##  #
	//  ###
	0b00001110, 0b00011001, 0b00010101, 0b00010011, 0b00001110,

	/* @58 '1' (3 pixels wide) */
	// ##
	//  #
	//  #
	//  #
	// ###
	0b00010001, 0b00011111, 0b00010000,

	/* @61 '2' (5 pixels wide) */
	// #####
	//     #
	// #####
	// #
	// #####
	0b00011101, 0b00010101, 0b00010101, 0b00010101, 0b00010111,

	/* @66 '3' (5 pixels wide) */
	// #####
	//     #
	//  ####
	//     #
	// #####
	0b00010001, 0b00010101, 0b00010101, 0b00010101, 0b00011111,

	/* @71 '4' (5 pixels wide) */
	//   ##
	//  # #
	// #  #
	// #####
	//    #
	0b00001100, 0b00001010, 0b00001001, 0b00011111, 0b00001000,

	/* @76 '5' (5 pixels wide) */
	// #####
	// #
	// #####
	//     #
	// #####
	0b00010111, 0b00010101, 0b00010101, 0b00010101, 0b00011101,

	/* @81 '6' (5 pixels wide) */
	// #####
	// #
	// #####
	// #   #
	// #####
	0b00011111, 0b00010101, 0b00010101, 0b00010101, 0b00011101,

	/* @86 '7' (5 pixels wide) */
	// #####
	//     #
	//    #
	//   #
	//   #
	0b00000001, 0b00000001, 0b00011001, 0b00000101, 0b00000011,

	/* @91 '8' (5 pixels wide) */
	// #####
	// #   #
	// #####
	// #   #
	// #####
	0b00011111, 0b00010101, 0b00010101, 0b00010101, 0b00011111,

	/* @96 '9' (5 pixels wide) */
	// #####
	// #   #
	// #####
	//     #
	// #####
	0b00010111, 0b00010101, 0b00010101, 0b00010101, 0b00011111,

	/* @101 ':' (1 pixels wide) */
	//
	// #
	//
	// #
	//
	0b00001010,

	/* @102 ';' (1 pixels wide) */
	//
	// #
	//
	// #
	// #
	0b00011010,

	/* @103 '<' (3 pixels wide) */
	//   #
	//  #
	// #
	//  #
	//   #
	0b00000100, 0b00001010, 0b00010001,

	/* @106 '=' (5 pixels wide) */
	//
	// #####
	//
	// #####
	//
	0b00001010, 0b00001010, 0b00001010, 0b00001010, 0b00001010,

	/* @111 '>' (3 pixels wide) */
	// #
	//  #
	//   #
	//  #
	// #
	0b00010001, 0b00001010, 0b00000100,

	/* @114 '?' (5 pixels wide) */
	// #####
	// #   #
	//   ###
	//
	//   #
	0b00000011, 0b00000001, 0b00010101, 0b00000101, 0b00000111,

	/* @119 '@' (5 pixels wide) */
	// #####
	// #   #
	// # ###
	// # # #
	// # ###
	0b00011111, 0b00000001, 0b00011101, 0b00010101, 0b00011111,

	/* @124 'A' (5 pixels wide) */
	// #####
	// #   #
	// #   #
	// #####
	// #   #
	0b00011111, 0b00001001, 0b00001001, 0b00001001, 0b00011111,

	/* @129 'B' (5 pixels wide) */
	// ####
	// #  #
	// #####
	// #   #
	// #####
	0b00011111, 0b00010101, 0b00010101, 0b00010111, 0b00011100,

	/* @134 'C' (5 pixels wide) */
	// #####
	// #
	// #
	// #
	// #####
	0b00011111, 0b00010001, 0b00010001, 0b00010001, 0b00010001,

	/* @139 'D' (5 pixels wide) */
	// ####
	// #   #
	// #   #
	// #   #
	// ####
	0b00011111, 0b00010001, 0b00010001, 0b00010001, 0b00001110,

	/* @144 'E' (5 pixels wide) */
	// #####
	// #
	// ####
	// #
	// #####
	0b00011111, 0b00010101, 0b00010101, 0b00010101, 0b00010001,

	/* @149 'F' (5 pixels wide) */
	// #####
	// #
	// ####
	// #
	// #
	0b00011111, 0b00000101, 0b00000101, 0b00000101, 0b00000001,

	/* @154 'G' (5 pixels wide) */
	// #####
	// #
	// # ###
	// #   #
	// #####
	0b00011111, 0b00010001, 0b00010101, 0b00010101, 0b00011101,

	/* @159 'H' (5 pixels wide) */
	// #   #
	// #   #
	// #####
	// #   #
	// #   #
	0b00011111, 0b00000100, 0b00000100, 0b00000100, 0b00011111,

	/* @164 'I' (3 pixels wide) */
	// ###
	//  #
	//  #
	//  #
	// ###
	0b00010001, 0b00011111, 0b00010001,

	/* @167 'J' (5 pixels wide) */
	//     #
	//     #
	//     #
	// #   #
	// #####
	0b00011000, 0b00010000, 0b00010000, 0b00010000, 0b00011111,

	/* @172 'K' (5 pixels wide) */
	// #   #
	// #  #
	// ###
	// #  #
	// #   #
	0b00011111, 0b00000100, 0b00000100, 0b00001010, 0b00010001,

	/* @177 'L' (5 pixels wide) */
	// #
	// #
	// #
	// #
	// #####
	0b00011111, 0b00010000, 0b00010000, 0b00010000, 0b00010000,

	/* @182 'M' (5 pixels wide) */
	// #   #
	// ## ##
	// # # #
	// #   #
	// #   #
	0b00011111, 0b00000010, 0b00000100, 0b00000010, 0b00011111,

	/* @187 'N' (5 pixels wide) */
	// #   #
	// ##  #
	// # # #
	// #  ##
	// #   #
	0b00011111, 0b00000010, 0b00000100, 0b00001000, 0b00011111,

	/* @192 'O' (5 pixels wide) */
	// #####
	// #   #
	// #   #
	// #   #
	// #####
	0b00011111, 0b00010001, 0b00010001, 0b00010001, 0b00011111,

	/* @197 'P' (5 pixels wide) */
	// #####
	// #   #
	// #####
	// #
	// #
	0b00011111, 0b00000101, 0b00000101, 0b00000101, 0b00000111,

	/* @202 'Q' (5 pixels wide) */
	// #####
	// #   #
	// # # #
	// #####
	//   #
	0b00001111, 0b00001001, 0b00011101, 0b00001001, 0b00001111,

	/* @207 'R' (5 pixels wide) */
	// #####
	// #   #
	// #####
	// #  #
	// #   #
	0b00011111, 0b00000101, 0b00000101, 0b00001101, 0b00010111,

	/* @212 'S' (5 pixels wide) */
	// #####
	// #
	// #####
	//     #
	// #####
	0b00010111, 0b00010101, 0b00010101, 0b00010101, 0b00011101,

	/* @217 'T' (5 pixels wide) */
	// #####
	//   #
	//   #
	//   #
	//   #
	0b00000001, 0b00000001, 0b00011111, 0b00000001, 0b00000001,

	/* @222 'U' (5 pixels wide) */
	// #   #
	// #   #
	// #   #
	// #   #
	// #####
	0b00011111, 0b00010000, 0b00010000, 0b00010000, 0b00011111,

	/* @227 'V' (5 pixels wide) */
	// #   #
	// #   #
	// #   #
	//  # #
	//   #
	0b00000111, 0b00001000, 0b00010000, 0b00001000, 0b00000111,

	/* @232 'W' (5 pixels wide) */
	// #   #
	// # # #
	// # # #
	// ## ##
	// #   #
	0b00011111, 0b00001000, 0b00000110, 0b00001000, 0b00011111,

	/* @237 'X' (5 pixels wide) */
	// #   #
	//  # #
	//   #
	//  # #
	// #   #
	0b00010001, 0b00001010, 0b00000100, 0b00001010, 0b00010001,

	/* @242 'Y' (5 pixels wide) */
	// #   #
	// #   #
	//  # #
	//   #
	//   #
	0b00000011, 0b00000100, 0b00011000, 0b00000100, 0b00000011,

	/* @247 'Z' (5 pixels wide) */
	// #####
	//    #
	//   #
	//  #
	// #####
	0b00010001, 0b00011001, 0b00010101, 0b00010011, 0b00010001,

	/* @252 '[' (2 pixels wide) */
	// ##
	// #
	// #
	// #
	// ##
	0b00011111, 0b00010001,

	/* @254 '\' (5 pixels wide) */
	// #   #
	//  # #
	//   #
	// #####
	//   #
	0b00001001, 0b00001010, 0b00011100, 0b00001010, 0b00001001,

	/* @259 ']' (2 pixels wide) */
	// ##
	//  #
	//  #
	//  #
	// ##
	0b00010001, 0b00011111,

	/* @261 '^' (5 pixels wide) */
	// #####
	// #####
	// #####
	// #####
	// #####
	0b00011111, 0b00011111, 0b00011111, 0b00011111, 0b00011111,

	/* @266 '_' (3 pixels wide) */
	//
	//
	//
	//
	// ###
	0b00010000, 0b00010000, 0b00010000,

	/* @269 '`' (2 pixels wide) */
	// #
	//  #
	//
	//
	//
	0b00000001, 0b00000010,

	/* @271 'a' (4 pixels wide) */
	//
	// ####
	// #  #
	// #  #
	// ## #
	0b00011110, 0b00010010, 0b00000010, 0b00011110,

	/* @275 'b' (4 pixels wide) */
	// #
	// ####
	// #  #
	// #  #
	// ####
	0b00011111, 0b00010010, 0b00010010, 0b00011110,

	/* @279 'c' (3 pixels wide) */
	//
	// ###
	// #
	// #
	// ###
	0b00011110, 0b00010010, 0b00010010,

	/* @283 'd' (4 pixels wide) */
	//    #
	// ####
	// #  #
	// #  #
	// ####
	0b00011110, 0b00010010, 0b00010010, 0b00011111,

	/* @287 'e' (4 pixels wide) */
	//
	// ####
	// #  #
	// #
	// ####
	0b00011110, 0b00010010, 0b00010010, 0b00010110,

	/* @291 'f' (4 pixels wide) */
	//  ###
	//  #
	// ####
	//  #
	//  #
	0b00000100, 0b00011111, 0b00000101, 0b00000101,

	/* @295 'g' (4 pixels wide) */
	//
	// ####
	// #  #
	//    #
	// ####
	0b00010110, 0b00010010, 0b00010010, 0b00011110,

	/* @299 'h' (4 pixels wide) */
	// #
	// ####
	// #  #
	// #  #
	// #  #
	0b00011111, 0b00000010, 0b00000010, 0b00011110,

	/* @303 'i' (1 pixels wide) */
	// #
	//
	// #
	// #
	// #
	0b00011101,

	/* @304 'j' (2 pixels wide) */
	//  #
	//
	//  #
	//  #
	// ##
	0b00010000, 0b00011101,

	/* @306 'k' (4 pixels wide) */
	// #
	// # #
	// ##
	// # #
	// #  #
	0b00011111, 0b00000100, 0b00001010, 0b00010000,

	/* @310 'l' (1 pixels wide) */
	// #
	// #
	// #
	// #
	// #
	0b00011111,

	/* @311 'm' (5 pixels wide) */
	//
	// #####
	// # # #
	// # # #
	// # # #
	0b00011110, 0b00000010, 0b00011110, 0b00000010, 0b00011110,

	/* @316 'n' (4 pixels wide) */
	//
	// ####
	// #  #
	// #  #
	// #  #
	0b00011110, 0b00000010, 0b00000010, 0b00011110,

	/* @320 'o' (4 pixels wide) */
	//
	// ####
	// #  #
	// #  #
	// ####
	0b00011110, 0b00010010, 0b00010010, 0b00011110,

	/* @324 'p' (4 pixels wide) */
	//
	// ####
	// #  #
	// ####
	// #
	0b00011110, 0b00001010, 0b00001010, 0b00001110,

	/* @328 'q' (3 pixels wide) */
	//
	// ###
	// # #
	// ###
	//   #
	0b00001110, 0b00001010, 0b00011110,

	/* @332 'r' (4 pixels wide) */
	//
	// # ##
	// ##
	// #
	// #
	0b00011110, 0b00000100, 0b00000010, 0b00000010,

	/* @336 's' (4 pixels wide) */
	//
	// ####
	// #
	//    #
	// ####
	0b00010110, 0b00010010, 0b00010010, 0b00011010,

	/* @340 't' (4 pixels wide) */
	//  #
	// ####
	//  #
	//  #
	//  ###
	0b00000010, 0b00011111, 0b00010010, 0b00010010,

	/* @344 'u' (4 pixels wide) */
	//
	// #  #
	// #  #
	// #  #
	// ####
	0b00011110, 0b00010000, 0b00010000, 0b00011110,

	/* @348 'v' (4 pixels wide) */
	//
	// #  #
	// #  #
	// #  #
	//  ##
	0b00001110, 0b00010000, 0b00010000, 0b00001110,

	/* @352 'w' (5 pixels wide) */
	//
	// # # #
	// # # #
	// # # #
	//  # #
	0b00001110, 0b00010000, 0b00001110, 0b00010000, 0b00001110,

	/* @357 'x' (4 pixels wide) */
	//
	// #  #
	//  #
	//   #
	// #  #
	0b00010010, 0b00000100, 0b00001000, 0b00010010,

	/* @361 'y' (4 pixels wide) */
	//
	// #  #
	// ####
	//    #
	// ####
	0b00010110, 0b00010100, 0b00010100, 0b00011110,

	/* @365 'z' (4 pixels wide) */
	//
	// ####
	//   #
	//  #
	// ####
	0b00010010, 0b00011010, 0b00010110, 0b00010010,

	/* @369 '{' (3 pixels wide) */
	//  ##
	//  #
	// ##
	//  #
	//  ##
	0b00000100, 0b00011111, 0b00010001,

	/* @372 '|' (1 pixels wide) */
	// #
	// #
	// #
	// #
	// #
	0b00011111,

	/* @373 '}' (3 pixels wide) */
	// ##
	//  #
	//  ##
	//  #
	// ##
	0b00010001, 0b00011111, 0b00000100,

	/* @376 '~' (7 pixels wide) */
	// #######
	// # ### #
	// # #   #
	// # ### #
	// #######
	0b00011111, 0b00010001, 0b00011111, 0b00011011, 0b00011011, 0b00010001, 0b00011111,
};

/* Character descriptors for D3 Littlebitmapism Suquare 5pt */
/* { [Char width in bits], [Offset into d3_5ptCharBitmaps in bytes] } */
const FONT_CHAR_INFO d3_5ptDescriptors[] =
{
	{2, 0}, 		/*   */
	{1, 2}, 		/* ! */
	{3, 3}, 		/* " */
	{5, 6}, 		/* # */
	{5, 11}, 		/* $ */
	{5, 16}, 		/* % */
	{5, 21}, 		/* & */
	{1, 26}, 		/* ' */
	{2, 27}, 		/* ( */
	{2, 29}, 		/* ) */
	{5, 31}, 		/* * */
	{5, 36}, 		/* + */
	{1, 41}, 		/* , */
	{5, 42}, 		/* - */
	{1, 47}, 		/* . */
	{5, 48}, 		/* / */
	{5, 53}, 		/* 0 */
	{3, 58}, 		/* 1 */
	{5, 61}, 		/* 2 */
	{5, 66}, 		/* 3 */
	{5, 71}, 		/* 4 */
	{5, 76}, 		/* 5 */
	{5, 81}, 		/* 6 */
	{5, 86}, 		/* 7 */
	{5, 91}, 		/* 8 */
	{5, 96}, 		/* 9 */
	{1, 101}, 		/* : */
	{1, 102}, 		/* ; */
	{3, 103}, 		/* < */
	{5, 106}, 		/* = */
	{3, 111}, 		/* > */
	{5, 114}, 		/* ? */
	{5, 119}, 		/* @ */
	{5, 124}, 		/* A */
	{5, 129}, 		/* B */
	{5, 134}, 		/* C */
	{5, 139}, 		/* D */
	{5, 144}, 		/* E */
	{5, 149}, 		/* F */
	{5, 154}, 		/* G */
	{5, 159}, 		/* H */
	{3, 164}, 		/* I */
	{5, 167}, 		/* J */
	{5, 172}, 		/* K */
	{5, 177}, 		/* L */
	{5, 182}, 		/* M */
	{5, 187}, 		/* N */
	{5, 192}, 		/* O */
	{5, 197}, 		/* P */
	{5, 202}, 		/* Q */
	{5, 207}, 		/* R */
	{5, 212}, 		/* S */
	{5, 217}, 		/* T */
	{5, 222}, 		/* U */
	{5, 227}, 		/* V */
	{5, 232}, 		/* W */
	{5, 237}, 		/* X */
	{5, 242}, 		/* Y */
	{5, 247}, 		/* Z */
	{2, 252}, 		/* [ */
	{5, 254}, 		/* \ */
	{2, 259}, 		/* ] */
	{5, 261}, 		/* ^ */
	{3, 266}, 		/* _ */
	{2, 269}, 		/* ` */
	{4, 271}, 		/* a */
	{4, 275}, 		/* b */
	{3, 279}, 		/* c */
	{4, 282}, 		/* d */
	{4, 286}, 		/* e */
	{4, 290}, 		/* f */
	{4, 294}, 		/* g */
	{4, 298}, 		/* h */
	{1, 302}, 		/* i */
	{2, 303}, 		/* j */
	{4, 305}, 		/* k */
	{1, 309}, 		/* l */
	{5, 310}, 		/* m */
	{4, 315}, 		/* n */
	{4, 319}, 		/* o */
	{4, 323}, 		/* p */
	{3, 327}, 		/* q */
	{4, 330}, 		/* r */
	{4, 334}, 		/* s */
	{4, 338}, 		/* t */
	{4, 342}, 		/* u */
	{4, 346}, 		/* v */
	{5, 350}, 		/* w */
	{4, 355}, 		/* x */
	{4, 359}, 		/* y */
	{4, 363}, 		/* z */
	{3, 367}, 		/* { */
	{1, 370}, 		/* | */
	{3, 371}, 		/* } */
	{7, 374}, 		/* ~ */
};

/* Font information for D3 5pt */
const FONT_INFO d3_5ptFontInfo =
{
	1, /*  Character height */
	' ', /*  Start character */
	'~', /*  End character */
	d3_5ptDescriptors, /*  Character descriptor array */
	d3_5ptBitmaps, /*  Character bitmap array */
};

/* Character bitmaps for Microsoft Sans Serif 5pt */
const uint8_t microsoftSansSerif_5ptBitmaps[] =
{
	/* @0 ' ' (2 pixels wide) */
	//
	//
	//
	//
	//
	0b00000000, 0b00000000,

	/* @2 'e' (4 pixels wide) */
	//
	//  ##
	// #  #
	// ###
	//  ###
	0b00001100, 0b00011010, 0b00011010, 0b00010100,

	/* @6 's' (3 pixels wide) */
	//
	//  ##
	// #
	//  ##
	// ###
	0b00010100, 0b00011010, 0b00011010,

	/* @9 't' (2 pixels wide) */
	// #
	// ##
	// #
	// #
	// ##
	0b00011111, 0b00010010,
};

/* Character descriptors for Microsoft Sans Serif 5pt */
/* { [Char width in bits], [Offset into microsoftSansSerif_5ptCharBitmaps in bytes] } */
const FONT_CHAR_INFO microsoftSansSerif_5ptDescriptors[] =
{
	{2, 0}, 		/*   */
	{0, 0}, 		/* ! */
	{0, 0}, 		/* " */
	{0, 0}, 		/* # */
	{0, 0}, 		/* $ */
	{0, 0}, 		/* % */
	{0, 0}, 		/* & */
	{0, 0}, 		/* ' */
	{0, 0}, 		/* ( */
	{0, 0}, 		/* ) */
	{0, 0}, 		/* * */
	{0, 0}, 		/* + */
	{0, 0}, 		/* , */
	{0, 0}, 		/* - */
	{0, 0}, 		/* . */
	{0, 0}, 		/* / */
	{0, 0}, 		/* 0 */
	{0, 0}, 		/* 1 */
	{0, 0}, 		/* 2 */
	{0, 0}, 		/* 3 */
	{0, 0}, 		/* 4 */
	{0, 0}, 		/* 5 */
	{0, 0}, 		/* 6 */
	{0, 0}, 		/* 7 */
	{0, 0}, 		/* 8 */
	{0, 0}, 		/* 9 */
	{0, 0}, 		/* : */
	{0, 0}, 		/* ; */
	{0, 0}, 		/* < */
	{0, 0}, 		/* = */
	{0, 0}, 		/* > */
	{0, 0}, 		/* ? */
	{0, 0}, 		/* @ */
	{0, 0}, 		/* A */
	{0, 0}, 		/* B */
	{0, 0}, 		/* C */
	{0, 0}, 		/* D */
	{0, 0}, 		/* E */
	{0, 0}, 		/* F */
	{0, 0}, 		/* G */
	{0, 0}, 		/* H */
	{0, 0}, 		/* I */
	{0, 0}, 		/* J */
	{0, 0}, 		/* K */
	{0, 0}, 		/* L */
	{0, 0}, 		/* M */
	{0, 0}, 		/* N */
	{0, 0}, 		/* O */
	{0, 0}, 		/* P */
	{0, 0}, 		/* Q */
	{0, 0}, 		/* R */
	{0, 0}, 		/* S */
	{0, 0}, 		/* T */
	{0, 0}, 		/* U */
	{0, 0}, 		/* V */
	{0, 0}, 		/* W */
	{0, 0}, 		/* X */
	{0, 0}, 		/* Y */
	{0, 0}, 		/* Z */
	{0, 0}, 		/* [ */
	{0, 0}, 		/* \ */
	{0, 0}, 		/* ] */
	{0, 0}, 		/* ^ */
	{0, 0}, 		/* _ */
	{0, 0}, 		/* ` */
	{0, 0}, 		/* a */
	{0, 0}, 		/* b */
	{0, 0}, 		/* c */
	{0, 0}, 		/* d */
	{4, 2}, 		/* e */
	{0, 0}, 		/* f */
	{0, 0}, 		/* g */
	{0, 0}, 		/* h */
	{0, 0}, 		/* i */
	{0, 0}, 		/* j */
	{0, 0}, 		/* k */
	{0, 0}, 		/* l */
	{0, 0}, 		/* m */
	{0, 0}, 		/* n */
	{0, 0}, 		/* o */
	{0, 0}, 		/* p */
	{0, 0}, 		/* q */
	{0, 0}, 		/* r */
	{3, 6}, 		/* s */
	{2, 9}, 		/* t */
};

/* Font information for Microsoft Sans Serif 5pt */
const FONT_INFO microsoftSansSerif_5ptFontInfo =
{
	1, /*  Character height */
	' ', /*  Start character */
	't', /*  End character */
	microsoftSansSerif_5ptDescriptors, /*  Character descriptor array */
	microsoftSansSerif_5ptBitmaps, /*  Character bitmap array */
};

