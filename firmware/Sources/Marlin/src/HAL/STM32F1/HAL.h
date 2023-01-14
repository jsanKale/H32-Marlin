/**
 * Marlin 3D Printer Firmware
 *
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 * Copyright (c) 2016 Bob Cousins bobcousins42@googlemail.com
 * Copyright (c) 2015-2016 Nico Tonnhofer wurstnase.reprap@gmail.com
 * Copyright (c) 2017 Victor Perez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

/**
 * HAL for stm32duino.com based on Libmaple and compatible (STM32F1)
 */

#define CPU_32_BIT

#include "../../core/macros.h"
#include "../shared/Marduino.h"
#include "../shared/math_32bit.h"
#include "../shared/HAL_SPI.h"

#include "fastio.h"

#include "timers.h"

#include <stdint.h>

#include "../../inc/MarlinConfigPre.h"

#include "../../inc/MarlinConfig.h"
#include "MarlinSerial.h"

// ------------------------
// Defines
// ------------------------

#define STM32_FLASH_SIZE 256

// On AVR this is in math.h?
#define square(x) ((x)*(x))

#ifndef strncpy_P
  #define strncpy_P(dest, src, num) strncpy((dest), (src), (num))
#endif

// Fix bug in pgm_read_ptr
#undef pgm_read_ptr
#define pgm_read_ptr(addr) (*(addr))

// Serial ports

#define _MSERIAL(X) MSerial##X
#define MSERIAL(X) _MSERIAL(X)

#define MYSERIAL1 MSERIAL(2)
#define LCD_SERIAL MSERIAL(4)

#if !WITHIN(SERIAL_PORT, -1, 5)
  #error "SERIAL_PORT must be from -1 to 5"
#endif

#if !WITHIN(SERIAL_PORT_2, -1, 5)
  #error "SERIAL_PORT_2 must be from -1 to 5"
#elif SERIAL_PORT_2 == SERIAL_PORT
  #error "SERIAL_PORT_2 must be different than SERIAL_PORT"
#endif

/**
 * TODO: review this to return 1 for pins that are not analog input
 */
#ifndef analogInputToDigitalPin
  #define analogInputToDigitalPin(p) (p)
#endif

#ifndef digitalPinHasPWM
  #define digitalPinHasPWM(P) (P)//(PIN_MAP[P].timer_device != nullptr)
  #define NO_COMPILE_TIME_PWM
#endif

// Reset Reason
#define RST_POWER_ON   1
#define RST_EXTERNAL   2
#define RST_BROWN_OUT  4
#define RST_WATCHDOG   8
#define RST_JTAG       16
#define RST_SOFTWARE   32
#define RST_BACKUP     64

// ------------------------
// Types
// ------------------------

typedef int8_t pin_t;

// ------------------------
// Interrupts
// ------------------------

#define CRITICAL_SECTION_START()  uint32_t primask = __get_PRIMASK(); (void)__iCliRetVal()
#define CRITICAL_SECTION_END()    if (!primask) (void)__iSeiRetVal()
#define cli() noInterrupts()
#define sei() interrupts()

// ------------------------
// ADC
// ------------------------

#ifdef ADC_RESOLUTION
  #define HAL_ADC_RESOLUTION ADC_RESOLUTION
#else
  #define HAL_ADC_RESOLUTION 10
#endif

#define HAL_ADC_VREF         3.3

uint16_t analogRead(const pin_t pin); // need hal.adc_enable() first
void analogWrite(const pin_t pin, int pwm_val8); // PWM only! mul by 257 in maple!?

//
// Pin Mapping for M42, M43, M226
//
#define GET_PIN_MAP_PIN(index) index
#define GET_PIN_MAP_INDEX(pin) pin
#define PARSED_PIN_INDEX(code, dval) parser.intval(code, dval)

#define JTAG_DISABLE() //afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY)
#define JTAGSWD_DISABLE() //afio_cfg_debug_ports(AFIO_DEBUG_NONE)

#define PLATFORM_M997_SUPPORT
void flashFirmware(int16_t value);

// ------------------------
// Class Utilities
// ------------------------

// Memory related
#define __bss_end __bss_end__

void _delay_ms(const int ms);

extern "C" char* _sbrk(int incr);

#pragma GCC diagnostic push
#if GCC_VERSION <= 50000
  #pragma GCC diagnostic ignored "-Wunused-function"
#endif

static inline int freeMemory() {
  volatile char top;
  return &top - _sbrk(0);
}

#pragma GCC diagnostic pop

class MarlinHAL {
public:

  // Earliest possible init, before setup()
  MarlinHAL() {}

  // Watchdog
  static void watchdog_init()    IF_DISABLED(USE_WATCHDOG, {});
  static void watchdog_refresh() IF_DISABLED(USE_WATCHDOG, {});

  static void init();          // Called early in setup()
  static void init_board() {}  // Called less early in setup()
  static void reboot() {}      // Restart the firmware from 0x0

  // Interrupts
  static bool isr_state() { return !__get_PRIMASK(); }
  static void isr_on()  { return __enable_irq(); }
  static void isr_off() { return __disable_irq(); }

  static void delay_ms(const int delay_ms) { delay(delay_ms); }

  // Tasks, called from idle()
  static void idletask() {}

  // Reset
  static uint8_t get_reset_source() { return 1; }
  static void clear_reset_source() {}

  // Free SRAM
  static int freeMemory() {
    volatile char top;
    return top;
  }

  //
  // ADC Methods
  //

  static uint16_t adc_result;

  // Called by Temperature::init once at startup
  static void adc_init();

  // Called by Temperature::init for each sensor at startup
  static void adc_enable(const pin_t pin) { pinMode(pin, INPUT_ANALOG); }

  // Begin ADC sampling on the given channel
  static void adc_start(const pin_t pin);

  // Is the ADC ready for reading?
  static bool adc_ready() { return true; }

  // The current value of the ADC register
  static uint16_t adc_value() { return adc_result; }

  /**
   * Set the PWM duty cycle for the pin to the given value.
   * Optionally invert the duty cycle [default = false]
   * Optionally change the maximum size of the provided value to enable finer PWM duty control [default = 255]
   * The timer must be pre-configured with set_pwm_frequency() if the default frequency is not desired.
   */
  static void set_pwm_duty(const pin_t pin, const uint16_t v, const uint16_t=255, const bool=false);

  /**
   * Set the frequency of the timer for the given pin.
   * All Timer PWM pins run at the same frequency.
   */
  static void set_pwm_frequency(const pin_t pin, const uint16_t f_desired);

};
