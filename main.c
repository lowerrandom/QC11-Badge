/*
 * Queercon XI Badge main code
 *
 * main.c
 *
 * (c) 2014 George Louthan
 * 3-clause BSD license; see license.md.
 */

#include "qcxi.h"
#include <string.h>
#include <stdlib.h>

#include "radio.h"
#include "fonts.h"
#include "clocks.h"
#include "leds.h"
#include "ir.h"
#include "ws2812.h"
#include "anim.h"
#include "main.h"

#pragma DATA_SECTION (my_conf, ".infoA");
#pragma DATA_SECTION (backup_conf, ".infoB");

const qcxiconf my_conf;

#if BADGE_TARGET
const qcxiconf backup_conf
//= {
//		{0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
//		{0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
//		{0xffff, 0xffff, 0xffff, 0xffff},
//		0xff, 0xff,
//		0x55,
//		"George",
//		"0xDECAFBAD!",
//		0xffff
//}
;
#else
const qcxiconf backup_conf = {
		{0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
		{0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
		{0xffff, 0xffff, 0xffff, 0xffff},
		0xff, 0xff,
		0xff,
		"",
		"",
		0xffff
};
#endif

qcxiconf disk_conf;

// Interrupt flags to signal the main thread:
volatile uint8_t f_rfm_rx_done = 0;
volatile uint8_t f_rfm_tx_done = 0;
volatile uint8_t f_ir_tx_done = 0;
volatile uint8_t f_ir_rx_ready = 0;
volatile uint8_t f_new_second = 0;
volatile uint8_t f_alarm = 0;

uint8_t f_paired = 0;
uint8_t f_unpaired = 0;
uint8_t f_paired_new_person = 0;
uint8_t f_paired_new_trick = 0;
uint8_t f_animation_done = 0;
uint8_t f_ir_itp_step = 0;
uint8_t f_ir_pair_abort = 0;
uint8_t f_paired_trick = 0;

// Global state:
uint8_t clock_is_set = 0;
uint8_t my_clock_authority = 0xff;
uint16_t loops_to_rf_beacon = 10 * TIME_LOOP_HZ;
char pairing_message[20] = "";
char event_message[40] = "";
uint8_t my_trick = 0;
uint16_t known_tricks = 0;
uint8_t known_trick_count = 0;
uint8_t known_props = 0;
qcxipayload in_payload;

qcxipayload out_payload = {
		RFM_BROADCAST, 0xff, 0xff, 0xff, 0xff, 0};

uint16_t rainbow_lights = 0;
uint8_t light_blink = 0;

// My general app-level status:
uint8_t badge_status = 0;
uint8_t am_idle = 1;

#define PAIR_INIT 0
#define PAIR_ONSCREEN 1
#define PAIR_WAVE 2
#define PAIR_GREETING 3
#define PAIR_MESSAGE 4
#define PAIR_IDLE 5

uint8_t pair_state = 0;

// Gaydar:
uint8_t window_position = 0; // Currently only used for restarting radio & skipping windows.
uint8_t neighbor_count = 0;
uint8_t window_seconds = RECEIVE_WINDOW_LENGTH_SECONDS;
uint8_t trick_seconds = TRICK_INTERVAL_SECONDS;
uint8_t target_gaydar_index = 0;
uint8_t gaydar_index = 0;
uint8_t neighbor_badges[BADGES_IN_SYSTEM] = {0};

#if !BADGE_TARGET
volatile uint8_t f_ser_rx = 0;
#endif

char time[6] = "00:00";

// Main thread signals:

uint16_t s_prop_cycles = 0,
		 s_prop_animation_length = 0;

uint8_t s_prop_id = 0,
		s_prop_authority = 0,
		s_propped = 0,
		s_need_rf_beacon = 0,
		s_rf_retransmit = 0,
		s_new_pair = 0,
		s_new_trick = 0,
		s_new_score = 0,
		s_new_prop = 0,
		s_trick = 0,
		s_prop = 0,
		s_update_rainbow = 0,
		s_disk_is_inserted = 0;

volatile uint8_t f_disk_is_inserted = 0;

uint8_t itps_pattern = 0;

// Scores:
uint8_t my_score = 0;
uint8_t shown_score = 0;
uint8_t s_count_score = 0;
#define COUNT_SCORE_CYCLES 4
uint8_t s_count_score_cycles = 0;

void set_known_tricks() {
	known_tricks = 1 << my_trick;
	known_trick_count = 1;

	for (uint8_t trick_id = 0; trick_id < TRICK_COUNT; trick_id++) {
		if (trick_id == my_trick) continue;
		for (uint8_t badge_id = trick_id; badge_id < BADGES_IN_SYSTEM; badge_id+=TRICK_COUNT) {
			if (paired_badge(badge_id) && !(known_tricks & 1<<(trick_id))) {
				known_tricks |= 1 << trick_id;
				known_trick_count++;
				switch(known_trick_count) {
				case 4:
					set_score(37, 1); //4th trick
					break;
				case 10:
					set_score(38, 2); //10th trick
					break;
				case TRICK_COUNT:
					set_score(40,3); // all tricks
				}
				break;
			}
		}
	}
}

void set_my_score_from_config() {
	my_score = 0;
	// Count the bits set in score:
	uint16_t v = 0;
	for (uint8_t i=0; i<4; i++) {
		v = ~my_conf.scores[i];
		for (;v;my_score++) {
			v &= v - 1;
		}
	}

	s_count_score_cycles = COUNT_SCORE_CYCLES;
	shown_score = 0;
	s_count_score = 1;

	known_props = 0;
	known_props += (my_score>=3);
	known_props += (my_score>=6);
	known_props += (my_score>=11);
	known_props += (my_score>=17);
	known_props += (my_score>=24);
	known_props += (my_score>=31);
}

void set_score(uint16_t id, uint16_t value) {
	uint16_t score_frame = id / 16;
	uint16_t score_bits = 0;
	while (value--) {
		score_bits |= 1 << (id % 16) + value;
	}
	if (!(~(my_conf.scores[score_frame]) & score_bits)) {
		// haven't seen it, so we need to set its 1 to a 0.
		uint16_t new_config_word = my_conf.scores[score_frame] & ~(score_bits);
		FLASH_unlockInfoA();
		FLASH_write16(&new_config_word, &(my_conf.scores[score_frame]), 1);
		FLASH_lockInfoA();
		s_new_score = 1;
		set_my_score_from_config();
	}
}

void init_power() {
#if BADGE_TARGET
	// Set Vcore to 1.8 V - NB: allows MCLK up to 8 MHz only
//	PMM_setVCore(PMM_CORE_LEVEL_0);
//	PMMCTL0 &= 0b11111100;
//	PMMCTL0 |= 0b11;
#else
	PMM_setVCore(PMM_CORE_LEVEL_3);
//	PMMCTL0 |= 0b11;
#endif
}

void init_gpio() {
	// Start out by turning off all the pins.
	P1DIR = 0xFF;
	P1OUT = LED_BLANK;
	P2DIR = 0xFF;
	P2OUT = 0x00;
	P3DIR = 0xFF;
	P3OUT = 0x00;
	P4DIR = 0xFF;
	P4OUT = 0x00;
	P5DIR = 0xFF;
	P5OUT = 0x00;
	P6DIR = 0xFF;
	P6OUT = 0x00;
}

void set_badge_seen(uint8_t id) {
	uint8_t badge_frame = id / 16;
	uint8_t badge_bit = 1 << (id % 16);
	if (!(~(my_conf.met_ids[badge_frame]) & badge_bit)) {
		// haven't seen it, so we need to set its 1 to a 0.
		uint16_t new_config_word = my_conf.met_ids[badge_frame] & ~(badge_bit);
		FLASH_unlockInfoA();
		FLASH_write16(&new_config_word, &(my_conf.met_ids[badge_frame]), 1);
		FLASH_lockInfoA();
	} // otherwise, nothing to do.
	if (id < 12 && currentTime.Hours == 5) {
		set_score(34, 1); // sleep your way to the top
	}
}

uint8_t event_attended(uint8_t id) {
	return ~my_conf.events_attended & (1 << id);
}

void set_event_attended(uint8_t id) {
	// No if event_attended() is needed here because this is only called once,
	//  and that call is wrapped in an if.
	if (light_blink == 128 + id) {
		light_blink = 0;
		s_update_rainbow = 1;
	}
	uint8_t new_event_attended = my_conf.events_attended & ~(1 << id);
	FLASH_unlockInfoA();
	FLASH_write8(&new_event_attended, &my_conf.events_attended, 1);
	FLASH_lockInfoA();
}

void set_event_occurred(uint8_t id) {
	if (~my_conf.events_occurred & (1 << id))
		return;
	uint8_t new_event_occurred = my_conf.events_occurred & ~(1 << id);
	FLASH_unlockInfoA();
	FLASH_write8(&new_event_occurred, &my_conf.events_occurred, 1);
	FLASH_lockInfoA();
}

uint8_t paired_badge(uint8_t id) {
	uint8_t badge_frame = id / 16;
	uint8_t badge_bit = 1 << (id % 16);
	return (~(my_conf.paired_ids[badge_frame]) & badge_bit)? 1: 0;
}

void set_badge_paired(uint8_t id) {
	if (id != 0xff) {
		f_paired = 1;
	}
	ir_proto_seqnum = 0;
	ir_pair_setstate(IR_PROTO_PAIRED);
}

void set_gaydar_target() {
	if (neighbor_count > 3)
		target_gaydar_index = 4;
	else
		target_gaydar_index = neighbor_count;
}

/*
 * So here's the flow of this thing.
 *
 * * STARTUP (POST, message, etc)
 * ------ block until finished ----
 *
 * Here are the things that can be flagged:
 *
 * Time based:
 * * Event alert raised (interrupt flag)
 * * DONE It's been long enough that we can do a trick (set flag from time loop)
 * ** DONE  (maybe the trick is a prop)
 * * DONE Time to beacon the radio (set flag from time loop)
 * * DONE Time to beacon the IR (set flag from time loop)
 *
 * From the radio:
 * * DONE Receive a beacon (at some point, we need to decide if it means we:)
 * ** adjust neighbor count
 * ** DONE Are near a base station (arrive event)
 * ** DONE should schedule a prop
 * ** DONE set our clock
 *
 * From the IR
 * * DONE Docking with base station - don't REALLY dock...
 * * DONE Pairing
 * ** DONE possibly new person
 * *** DONE possibly new person with a new trick
 *
 * Looping - here are the priorities:
 *
 * * Set clock (pre-empts)
 * * Arrived at event (animation, don't wait for previous to finish)
 * * Event alert (animation, wait for idle)
 * * Pair begins (animation and behavior pre-empts, don't wait to finish)
 * * New pairing person (wait for idle)
 * * New trick learned (wait for idle)
 * * Score earned (wait for idle)
 * * Prop earned (wait for idle)
 * * Pair expires (wait for idle)
 * * Get propped (from radio beacon)
 * * Do a trick or prop (idle only)
 *
 * * Adjust neighbor count (from radio beacon)
 * *
 *
 */

uint8_t skip_window = 1;

int main( void )
{
	init_watchdog();
	init_power();
	init_gpio();
	init_clocks();
	check_config();
	led_init();
	init_rtc();
	init_ir();
	init_alarms();
#if !BADGE_TARGET
	ws2812_init();
	ser_init();
#endif
	__bis_SR_register(GIE);
	init_radio(); // requires interrupts enabled.

	srand(my_conf.badge_id);

	// Power-on self test:
	uint8_t post_result = post();
	if (post_result != 0) {
		// Display error code:
		char hex[4] = {0, 0, 0, 0};
		hex[0] = (post_result/16 < 10)? '0' + post_result/16 : 'A' - 10 + post_result/16;
		hex[1] = (post_result%16 < 10)? '0' + post_result%16 : 'A' - 10 + post_result%16;
		ser_print("Error code... ");
		ser_print(hex);
		ser_print("\n\n");
		fillFrameBufferSingleColor(&leds[6], NUMBEROFLEDS, ws_frameBuffer, ENCODING);
		ws_set_colors_async(NUMBEROFLEDS);
		delay(1000);
	} else if (my_conf.handle[0]) {
		led_print_scroll(my_conf.handle, 1);
	} else {
		led_print_scroll("qcxi", 0);
	}
	uint8_t color = 0;
	led_anim_init();

	ser_cls();
	ser_print("Starting up...");
	delay(1000);

	// Main sequence:
	f_rfm_rx_done = 0;
	while (1) {

		// New serial message?
		if (f_ser_rx) {
			f_ser_rx = 0;
			ser_print((uint8_t *) ser_buffer_rx);
			if (ser_buffer_rx[1] >= '0' && ser_buffer_rx[1] <= '7') {
				// BEGIN event
				ser_print("Setting this base station up for event.");
//				ser_print(ser_buffer_rx);
				out_payload.base_id = ser_buffer_rx[1] - '0';
			} else if (ser_buffer_rx[1] == 'Q') {
				out_payload.base_id = 0xff;
				ser_print("Ending event.");
				// END event
			} else if (ser_buffer_rx[1] == 'B') {
				out_payload.base_id = BUS_BASE_ID;
				ser_print("I'm the bus.");
			}
			memset(ser_buffer_rx, 0, 255);
		}

		if (f_rfm_tx_done) {
			f_rfm_tx_done = 0;
			// Back to normal RX automode:
			write_single_register(0x3b, RFM_AUTOMODE_RX);
		}

		/*
		 * Process link-layer IR messages if needed.
		 */
		if (f_ir_rx_ready) {
			f_ir_rx_ready = 0;
			ir_process_rx_ready();
		}

		if (f_ir_pair_abort) {
			f_ir_pair_abort = 0;
		}


		if (f_rfm_rx_done) {
			f_rfm_rx_done = 0;

			if ((!clock_is_set && in_payload.clock_authority != 0xff) || (in_payload.clock_authority < my_clock_authority))
			{
				memcpy(&currentTime, &in_payload.time, sizeof (Calendar));
				clock_is_set = 1;
				init_rtc();
				my_clock_authority = in_payload.clock_authority;
				out_payload.clock_authority = 1;
				init_alarms();
				set_score(48, 1); // clock setting
			}

			if (in_payload.prop_from != 0xFF && in_payload.prop_from != my_conf.badge_id) {
				// It's a prop notification.
				// If we don't currently have a prop scheduled, or if this prop is
				// more authoritative than our currently scheduled prop, it's time
				// to do a prop.
				// If we're paired, and this is from the person
				// we're paired with, it's the most authoritative prop possible.
				uint8_t prop_authority = in_payload.prop_from;
				if (( (!s_propped && !s_prop)
				     || prop_authority < s_prop_authority
					)
					&& in_payload.prop_time_loops_before_start)
				{
					// TODO: only set s_propped for the ball.
					s_propped = 1;


					s_prop = 0;
					s_prop_authority = in_payload.prop_from;
					s_prop_cycles = in_payload.prop_time_loops_before_start;
					s_prop_id = in_payload.prop_id;
					out_payload.prop_from = s_prop_authority;
					out_payload.prop_time_loops_before_start = s_prop_cycles;
					out_payload.prop_id = s_prop_id;
				} else if (s_propped) {
					// If we're already propped, and our current prop has higher authority,
					// retransmit it.
					s_rf_retransmit = 1;
				}
			}
		}
#if BADGE_TARGET
		static uint8_t event_id = 0;
		/*
		 * Calendar interrupts:
		 *
		 * * Event alert raised (interrupt flag)
		 *
		 */
		if (f_alarm) { // needs to be before f_new_second?
			event_id = f_alarm & 0b0111;
			if (clock_is_set  && !event_attended(event_id)) {
				if (f_alarm & ALARM_START_LIGHT) {
					light_blink = 128 + event_id;
				}
				if (f_alarm & ALARM_STOP_LIGHT) {
					light_blink = 0;
					s_update_rainbow = 1;
				}
				if (f_alarm & ALARM_DISP_MSG) {
					strcpy(event_message, alarm_msg);
					if (!led_display_text) {
						led_print_scroll(event_message, 1);
						am_idle = 0;
					}
				}
			}
			if (!(f_alarm & ALARM_NO_REINIT)) {
				init_alarms();
			}
			f_alarm = 0;
		}
#endif

		if (f_new_second) {
			f_new_second = 0;

			currentTime = RTC_A_getCalendarTime(RTC_A_BASE);
			out_payload.time.Hours = currentTime.Hours;
			out_payload.time.Minutes = currentTime.Minutes;
			out_payload.time.DayOfMonth = currentTime.DayOfMonth;
			out_payload.time.DayOfWeek = currentTime.DayOfWeek;
			out_payload.time.Month = currentTime.Month;
			out_payload.time.Year = currentTime.Year;
			out_payload.time.Seconds = currentTime.Seconds;

			if (currentTime.Hours == 7 && currentTime.DayOfMonth >= 8 &&
					currentTime.DayOfMonth <= 10) {
				// 7am Friday, Saturday, and Sunday.
				set_score(49+currentTime.DayOfMonth-8, 1); // OBEY: on at 7
			}

			if (!trick_seconds && !itps_pattern) {
				trick_seconds = TRICK_INTERVAL_SECONDS-1 + (rand()%3);
				if (rand() % 3) {
					// wave
					s_trick = TRICK_COUNT+1;
				} else {
					// TODO: puppy only
					// trick
					static uint8_t known_trick_to_do;
					known_trick_to_do = rand() % known_trick_count;
					// start with the first known trick:

					while (!(known_tricks & 1<<s_trick)) {
						s_trick++;
					}

					while (known_trick_to_do) {
						s_trick++;
						if (known_tricks & 1<<s_trick) {
							// if and only if we know the candidate trick, do
							// we decrement known_trick_to_do.
							known_trick_to_do--;
						}
					}
					// doing s_trick...
					s_trick++; // because the s_trick flag is trick_id+1
				}
			} else { // if (!sprite_animate && !led_text_scrolling && !s_propped) {
				trick_seconds--;
			}

			window_seconds--;
			if (!window_seconds) {
				window_seconds = RECEIVE_WINDOW_LENGTH_SECONDS;
				if (skip_window != window_position) {
					s_need_rf_beacon = 1;
				}
				neighbor_count = 0;
				for (uint8_t i=0; i<BADGES_IN_SYSTEM; i++) {
					if (neighbor_badges[i]) {
						neighbor_count++;
						neighbor_badges[i]--;
					}
				}
				if (neighbor_count > 5 && (next_event_flag & ALARM_START_LIGHT)) {
					// if next alarm is turning a light on (low-rent stand-in
					// for being between events), and you're around >5 people,
					// that's 2 points.
					set_score(17, 2); // Socialite.
				}

				window_position = (window_position + 1) % RECEIVE_WINDOW;
				if (!window_position) {
					skip_window = rand() % RECEIVE_WINDOW;
				}
				// If we're rolling over the window and have no neighbors,
				// try a radio reboot, in case that can gin up some neighbors
				// for some reason.
				if (!window_position && neighbor_count == 0) {
					init_radio();
				}
				set_gaydar_target();
			}
		}

		if (f_time_loop) {
			f_time_loop = 0;
			if (!s_disk_is_inserted) {
//				led_timestep();
			}

			fillFrameBufferSingleColor(&leds[color], NUMBEROFLEDS, ws_frameBuffer, ENCODING);
			ws_set_colors_async(NUMBEROFLEDS);
			color= (color + 1) % 21;

			if (loops_to_ir_timestep) {
				loops_to_ir_timestep--;
			} else {
				loops_to_ir_timestep = IR_LOOPS;
				ir_process_timestep();
			}

			// number of ITPs to display data for: (ITPS_TO_PAIR-ITPS_TO_SHOW_PAIRING)
			// (ITPS_TO_PAIR-ITPS_TO_SHOW_PAIRING) / 5: ITPS per light

			if (s_prop_cycles) {
				s_prop_cycles--;
				out_payload.prop_time_loops_before_start = s_prop_cycles;
			}
		}

		// This is background:
		if (s_need_rf_beacon && rfm_reg_state == RFM_REG_IDLE  && !(read_single_register_sync(0x27) & (BIT1+BIT0))) {
			out_payload.beacon = 0;

			if (!clock_is_set)
				out_payload.clock_authority = 0xff;
			// TODO: only send if clock is set.
			if (clock_is_set)
				radio_send_sync();
			s_need_rf_beacon = 0;
		} else if (s_rf_retransmit && rfm_reg_state == RFM_REG_IDLE && !(read_single_register_sync(0x27) & (BIT1+BIT0))) {
			out_payload.beacon = 0;

			if (!clock_is_set)
				out_payload.clock_authority = 0xff;
			if (clock_is_set)
				radio_send_sync();
			s_rf_retransmit = 0;
		}

		// Is an animation finished?
		if (f_animation_done) {
			f_animation_done = 0;
			led_display_left |= BIT0;
			am_idle = 1;
		}

		// Pre-emptive:
//
//		if (am_idle) { // Can do another action now.
//			switch(badge_status) {
//			case BSTAT_GAYDAR:
//				if (s_propped && !s_prop_cycles) {
//					// Do a prop effect:
//					am_idle = 0;
//					s_propped = 0;
//					out_payload.prop_from = 0xff;
//					out_payload.prop_time_loops_before_start = 0;
//
//					if (s_prop_id <= 1) { // 0, 1 are full:
//						if (s_prop_id != 1)
//							led_display_left &= ~BIT0;
////						full_animate(prop_effects[s_prop_id], PROP_FRAMESKIP);
//					} else { // the rest are full:
////						left_sprite_animate(prop_effects_sprites[s_prop_id-2], PROP_FRAMESKIP);
//					}
//					s_trick = 0;
//
//				} else if (target_gaydar_index > gaydar_index) {
//					am_idle = 0;
//					right_sprite_animate(gaydar[gaydar_index], 4, 0, 1, 1);
//					left_sprite_animate(anim_sprite_wave, 4);
//					gaydar_index++;
//				} else if (target_gaydar_index < gaydar_index) {
//					am_idle = 0;
//					gaydar_index--;
//					right_sprite_animate(gaydar[gaydar_index], 4, 0, -1, gaydar_index!=0);
//					left_sprite_animate(anim_sprite_wave, 4);
//				}
//				break;
//			case BSTAT_PAIR:
//				if (f_unpaired) {
//					f_unpaired = 0;
//					s_disk_is_inserted = 0;
//					itps_pattern = 0;
//					s_update_rainbow = 1;
//					badge_status = BSTAT_GAYDAR;
//					am_idle = 0;
//					right_sprite_animate(anim_sprite_walkin, 2, 1, -1, 0);
//					left_sprite_animate(anim_sprite_wave, 2);
//					break;
//				}
//				switch(pair_state) {
//				case PAIR_INIT: // Pat just walked on
//					am_idle = 0;
//					pair_state = PAIR_WAVE;
//					right_sprite_animate(anim_sprite_wave, 5, 1, 1, 1);
//					left_sprite_animate(anim_sprite_wave, 5);
//					break;
//				case PAIR_WAVE:
//					am_idle = 0;
//					memset(pairing_message, 0, 20);
//					strcat(pairing_message, "Hi ");
//					strcat(pairing_message, ir_rx_handle);
//					led_print_scroll(pairing_message, 1);
//					pair_state = PAIR_GREETING;
//					break;
//				case PAIR_GREETING:
//					if (!ir_rx_message[0]) {
//						pair_state = PAIR_IDLE;
//					} else {
//						am_idle = 0;
//						led_print_scroll(ir_rx_message, 1);
//						pair_state = PAIR_MESSAGE;
//						break;
//					}
//					break;
//				case PAIR_MESSAGE:
//					pair_state = PAIR_IDLE;
//					break;
//				}
//			} // end switch(badge_status)
//		}

		if (f_disk_is_inserted) {
			f_disk_is_inserted = 0;
			s_disk_is_inserted = 1;

			ser_cls();
			// TODO: get all their scoring and pairing information.

			// TODO: WS: solid color.
			// TODO: delay briefly for WS solid

			ser_print("Hello");
			if (ir_rx_handle[0]) {
				ser_print(", ");
				ser_print(ir_rx_handle);
			} else {
				ser_print("!");
			}
			ser_print("\r\n\r\n");
			delay(100);
			ser_print("This is your badge\r\n");
			delay(100);
			ser_print("There are many like it, but this one is yours.\r\n");
			delay(1200);
			ser_print("Here's a graph of your pairing history...\r\n");
			delay(100);

			uint8_t badge_frame = 0;
			uint16_t badge_bit = 0;
			uint8_t pair_count = 0;
			uint8_t uber_count = 0;
			char pair_count_str[4] = {'0', '0', '0', 0};
			ser_print("________________________________");

			uint8_t disk_known_trick_count = 1;
			uint16_t disk_known_tricks = 1 << (disk_conf.badge_id % TRICK_COUNT);
			uint8_t trick_id = 0;

			for (uint8_t i=0; i<150; i++) {
				if (i % 30 == 0) {
					ser_print("\r\n|");
				}
				badge_frame = i / 16;
				badge_bit = 1 << (i % 16);

				if ((~(disk_conf.paired_ids[badge_frame]) & badge_bit)
						|| (i < 12 && (~(disk_conf.scores[0]) & badge_bit))) {
					pair_count++;
					trick_id = i % TRICK_COUNT;
					if (!(disk_known_tricks & 1<<(trick_id))) {
						disk_known_tricks |= 1 << trick_id;
						disk_known_trick_count++;
					}

					if (i < 12) {
						uber_count++;
						ser_print("U");
					} else {
						ser_print("#");
					}
				} else {
					ser_print(" ");
				}
				if (i % 30 == 29) {
					ser_print("|");
					delay(100);
				}
			}
			ser_print("\r\n--------------------------------\r\n");

			delay(100);

			pair_count_str[0] = '0' + pair_count/100;
			pair_count_str[1] = '0' + (pair_count/10) % 10;
			pair_count_str[2] = '0' + pair_count % 10;
			ser_print("\r\n  ");
			ser_print(pair_count_str);
			ser_print("/150 with ");
			pair_count_str[0] = '0'+uber_count/10;
			pair_count_str[1] = '0'+uber_count % 10;
			pair_count_str[2] = 0;
			ser_print(pair_count_str);
			ser_print(" ubers.\r\n\r\n");

			delay(500);

			ser_print("You've learned to do ");
			pair_count_str[0] = disk_known_trick_count/10 + '0';
			pair_count_str[1] = (disk_known_trick_count % 10) + '0';
			pair_count_str[2] = 0;
			ser_print(pair_count_str);
			ser_print("/12 tricks.\r\n\r\n");

			delay(1000);

			uint8_t disk_score = 0;

			// Count the bits set in score:
			uint16_t v = 0;
			for (uint8_t i=0; i<4; i++) {
				v = ~disk_conf.scores[i];
				for (;v;disk_score++) {
					v &= v - 1;
				}
			}

			delay(1000);
//
			if (disk_score >= 3) {
				ser_print(" * Your badge knows the 'Ball' prop\r\n");
				delay(500);
			}
			if (disk_score >= 6) {
				ser_print(" * Your badge knows the 'Rainstick' prop\r\n");
				delay(500);
			}
			if (disk_score >= 11) {
				ser_print(" * Your badge knows the 'Earthquake' prop\r\n");
				delay(500);
			}
			if (disk_score >= 17) {
				ser_print(" * Your badge knows the 'Disco wand' prop\r\n");
				delay(500);
			}
			if (disk_score >= 24) {
				ser_print(" * Your badge knows the 'Trapdoor' prop\r\n");
				delay(500);
			}
			if (disk_score >= 31) {
				ser_print(" * Your badge knows the 'Dynamite' prop!\r\n");
				ser_print("You've collected every prop!\r\n");
				delay(500);
			}

			delay(2000);


			// Now list scores?
			ser_print("\r\n\r\n\r\n\r\nHere are your scoring achievements:\r\n\r\n");
			delay(500);

			ser_print("[");
			pair_count_str[0] = '0'+uber_count/10;
			pair_count_str[1] = '0'+uber_count % 10;
			pair_count_str[2] = 0;
			ser_print(pair_count_str);
			ser_print(   "] UBER PAIRER.\r\n");
			ser_print("     One point for pairing with each QC uber badge holder\r\n\r\n");
			delay(1500);

			if (~disk_conf.scores[0] & (1 << 12)) {
				ser_print(" [3] UBER WHORE\r\n");
				ser_print("     Pair with lots of uber badges.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[0] & (1 << 15)) {
				ser_print(" [1] IS ANYBODY THERE?\r\n");
				ser_print("     Receive a radio beacon for the first time.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[1] & (1)) {
				ser_print(" [1] MY BUDDY AND ME\r\n");
				ser_print("     Pair with another badge.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[1] & (1 << 1)) {
				ser_print(" [2] SOCIALITE\r\n");
				ser_print("     Hang out with a lot of QC badges outside of events.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[1] & (1 << 3)) {
				ser_print(" [3] THURSDAY PRE-PARTY\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[1] & (1 << 6)) {
				ser_print(" [2] FRIDAY MIXER\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[1] & (1 << 8)) {
				ser_print(" [2] POOL PARTY\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[1] & (1 << 10)) {
				ser_print(" [1] SATURDAY MIXER\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[1] & (1 << 11)) {
				ser_print(" [5] SUPER SECRET SQUIRREL\r\n");
				ser_print("     Sunday night after-party\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[2] & 1) {
				ser_print(" [1] SUNDAY MIXER\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[2] & (1 << 1)) {
				ser_print(" [1] I'M ON A BUS\r\n");
				ser_print("     Ride the bus.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[2] & (1 << 2)) {
				ser_print(" [1] SLEEP YOUR WAY TO THE TOP\r\n");
				ser_print("     Be near a QC uber badge at 5 AM.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[2] & (1 << 1)) {
				ser_print(" [2] FASHIONABLY EARLY\r\n");
				ser_print("     Arrive at a QC event before it starts.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[3] & (1 << 6)) {
				ser_print(" [3] CLEANING CREW.\r\n");
				ser_print("     Stay to the end of a QC event.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[2] & (1 << 5)) {
				ser_print(" [1] TRICK COLLECTOR\r\n");
				ser_print("     Learn at least 4 stick figure tricks.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[2] & (1 << 6)) {
				ser_print(" [2] TRICK EXPERT\r\n");
				ser_print("     Learn at least 10 stick figure tricks.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[2] & (1 << 8)) {
				ser_print(" [3] TRICK MASTER\r\n");
				ser_print("     Learn all the available stick figure tricks.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[3] & (1)) {
				ser_print(" [1] WHAT TIME IS IT?\r\n");
				ser_print("     Get your badge's clock set for the first time.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[3] & (1 << 1)) {
				ser_print(" [1] OBEY. (1/3)\r\n");
				ser_print("     Leave your batteries in overnight on Thursday.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[3] & (1 << 2)) {
				ser_print(" [1] OBEY. (2/3)\r\n");
				ser_print("     Leave your batteries in overnight on Friday.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[3] & (1 << 3)) {
				ser_print(" [1] OBEY. (3/3)\r\n");
				ser_print("     Leave your batteries in overnight on Saturday.\r\n\r\n");
				delay(1500);
			}

			if (~disk_conf.scores[3] & (1 << 4)) {
				ser_print(" [2] FLOPPY DISK.\r\n");
				ser_print("     Put your badge in the box.\r\n\r\n");
				delay(1500);
			}

			ser_print("That makes your score ");

			pair_count_str[0] = disk_score/10 + '0';
			pair_count_str[1] = (disk_score % 10) + '0';
			pair_count_str[2] = 0;

			ser_print(pair_count_str);
			ser_print(".\r\n\r\n");

			delay(500);
			ser_print("See you next year!\r\n\r\n");
			delay(500);
			ser_print("                                     qcxi.");
			delay(3000);

		}

		if (f_unpaired) {
			f_unpaired = 0;
			ser_cls();
			s_disk_is_inserted = 0;
		}

		// Any status, if we have no more idle-status-change processing to do:
		if (am_idle) {
			if (s_trick) {
				uint16_t trick_len = 0;
//				while (!(tricks[s_trick-1][trick_len++].lastframe & BIT7));
				trick_len *= 4; // TODO.
				if (!(s_propped || s_prop) ||
						(s_propped && trick_len+TIME_LOOP_HZ < s_prop_cycles) ||
						(trick_len+TIME_LOOP_HZ < s_prop_cycles - s_prop_animation_length))
				{
					am_idle = 0;

					if (pair_state == PAIR_IDLE) {
						ir_proto_seqnum = s_trick;
					}
//					left_sprite_animate((spriteframe *)tricks[s_trick-1], 4);
					s_trick = 0; // this needs to be after the above statement. Duh.
				}
			}
		}

		if (f_paired_trick > TRICK_COUNT+1) {
			f_paired_trick = 0;
		}
		if (pair_state == PAIR_IDLE && f_paired_trick) {
//			right_sprite_animate((spriteframe *)tricks[f_paired_trick-1], 4, 1, 1, 0xff);
			f_paired_trick = 0;
		}

		// Background:
		if (s_update_rainbow) {
			s_update_rainbow = 0;
			rainbow_lights &= 0b1111111111100000;
			if (clock_is_set)
				rainbow_lights &= ~BIT9;
			if (itps_pattern) {
				rainbow_lights |= itps_pattern;
			} else if (s_count_score) {
				rainbow_lights |= (shown_score & 0b11111);
			} else {
				if (my_score >= 31) {
					rainbow_lights |= 0b11111;
				} else {
					rainbow_lights |= (my_score & 0b11111);
				}
			}

			if (!light_blink) {
				rainbow_lights &= 0b1111111000011111;
				// set according to events attended...
				uint8_t reverse_events = ((my_conf.events_attended * 0x0802LU & 0x22110LU) | (my_conf.events_attended * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
				rainbow_lights |= ((uint16_t) ~reverse_events & 0b11111000) << 2;
			}

//			led_set_rainbow(rainbow_lights);
		}

		// Going to sleep... mode...
//		__bis_SR_register(LPM3_bits + GIE);
	}
} // end main()

uint8_t post() {
	__bic_SR_register(GIE);
	uint8_t post_result = 0;

	// Clocks
	if (xt1_status == STATUS_FAIL) {
		post_result |= POST_XT1F;
	}
	if (xt2_status == STATUS_FAIL) {
		post_result |= POST_XT2F;
	}
#if BADGE_TARGET

	led_set_rainbow(0b1111111111);

	// LED test pattern
	memset(disp_buffer, 0xff, sizeof disp_buffer);
	led_update_display();
	for (uint8_t i=LED_PERIOD; i>0; i--) {
		led_enable(i);
		delay(20);
	}
	led_disable();
	delay(500);
#endif
	__bis_SR_register(GIE);

	ir_reject_loopback = 0;
	// IR loopback
	ir_write("test", 0xff, 0);
#if BADGE_TARGET
	uint16_t spin = 65535;
#else
	uint32_t spin = 1572840;
#endif
	while (spin-- && !f_ir_rx_ready);
	if (f_ir_rx_ready) {
		f_ir_rx_ready = 0;
		if (!ir_check_crc()) {
			// IR integrity fault
			post_result |= POST_IRIF;
		} else {
			if (strcmp("test", (char *) ir_rx_frame) != 0)
				post_result |= POST_IRVF; // IR value fault
		}
	} else {
		post_result |= POST_IRGF; // IR general fault
	}
	ir_reject_loopback = 1;

	return post_result;
}

uint16_t config_crc(qcxiconf conf) {
	CRC_setSeed(CRC_BASE, 0xBEEF);
	CRC_set8BitData(CRC_BASE, conf.badge_id);
	for (uint8_t i=0; i<sizeof(conf.handle); i++) {
		CRC_set8BitData(CRC_BASE, conf.handle[i]);
	}

	for (uint8_t i=0; i<sizeof(conf.message); i++) {
		CRC_set8BitData(CRC_BASE, conf.message[i]);
	}
	return CRC_getResult(CRC_BASE);
}

void check_config() {
	WDT_A_hold(WDT_A_BASE);

	uint16_t crc = config_crc(my_conf);

	if (crc != my_conf.crc) {
		// this means we need to load the backup conf:
		// we ignore the CRC of the backup conf.
		uint8_t* new_config_bytes = (uint8_t *) &backup_conf;

		FLASH_unlockInfoA();
		uint8_t flash_status = 0;
		do {
			FLASH_segmentErase((uint8_t *)INFOA_START);
			flash_status = FLASH_eraseCheck((uint8_t *)INFOA_START, 128);
		} while (flash_status == STATUS_FAIL);

		FLASH_write8(new_config_bytes, (uint8_t *)INFOA_START, sizeof(qcxiconf) - sizeof my_conf.crc);

		crc = config_crc(backup_conf);
		FLASH_write16(&crc, &my_conf.crc, 1);

		FLASH_lockInfoA();
	}

	// Decide which tricks we know:
	my_trick = my_conf.badge_id % TRICK_COUNT;
	set_known_tricks();

	set_my_score_from_config();
	s_new_score = 0;

	// Setup our IR pairing payload:
//	memcpy(&(ir_pair_payload[0]), my_conf.handle, 11); // causes problems?
//	memcpy(&(ir_pair_payload[11]), my_conf.message, 17);
	strcpy(&(ir_pair_payload[0]), my_conf.handle);
	strcpy(&(ir_pair_payload[11]), my_conf.message);
	out_payload.from_addr = my_conf.badge_id;
}

void delay(uint16_t ms)
{
	while (ms--)
    {
        __delay_cycles(MCLK_DESIRED_FREQUENCY_IN_KHZ);
    }
}
