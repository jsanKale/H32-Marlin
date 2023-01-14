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

/**
 * HAL for stm32duino.com based on Libmaple and compatible (STM32F1)
 */

#ifdef __STM32F1__
#include "../../inc/MarlinConfig.h"
#include "HAL.h"

#if (defined(SERIAL_USB) && !defined(USE_USB_COMPOSITE))
  //USBSerial SerialUSB;
#endif

uint16_t MarlinHAL::adc_result = 0;

// ------------------------
// Private Variables
// ------------------------
//STM32ADC adc(ADC1);

enum TEMP_PINS {
    TEMP_BED=14,
    TEMP_0=15,
};

// ------------------------
// Public functions
// ------------------------

//
// Leave PA11/PA12 intact if USBSerial is not used
//
#if SERIAL_USB
  namespace wirish { namespace priv {
        void board_setup_usb() {}
  }
}
#endif

void MarlinHAL::init() {
  NVIC_SetPriorityGrouping(0x3);
  stc_wdt_init_t new_WDT_config;
  new_WDT_config.enCountCycle = WdtCountCycle65536;///< Count cycle
  new_WDT_config.enClkDiv = WdtPclk3Div8192;               ///< Count clock division
  new_WDT_config.enRefreshRange = WdtRefresh100Pct;         ///< Allow refresh percent range
  new_WDT_config.enSleepModeCountEn = Disable;     ///< Enable/disable count in the sleep mode
  new_WDT_config.enRequsetType = WdtTriggerResetRequest;    ///< Refresh error or count underflow trigger event type
  WDT_Init(&new_WDT_config);
}

/* VGPV Done with defines
// disable interrupts
void cli() { noInterrupts(); }

// enable interrupts
void sei() { interrupts(); }
*/

/**
 * TODO: Check this and change or remove.
 * currently returns 1 that's equal to poweron reset.
 */

#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21)
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                  ((uint8_t)0x39)
#define RCC_FLAG_LSERDY                  ((uint8_t)0x41)
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x61)
#define RCC_FLAG_PINRST                  ((uint8_t)0x7A)
#define RCC_FLAG_PORRST                  ((uint8_t)0x7B)
#define RCC_FLAG_SFTRST                  ((uint8_t)0x7C)
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x7D)
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x7E)
#define RCC_FLAG_LPWRRST                 ((uint8_t)0x7F)

typedef enum { RESET = 0, SET   = !RESET } FlagStatus, ITStatus;

FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG){
  FlagStatus bitstatus = RESET;
  return bitstatus;
}

extern "C" {
  extern unsigned int _ebss; // end of bss section
}

// ------------------------
// Watchdog Timer
// ------------------------

#if ENABLED(USE_WATCHDOG)

  #include "iwdg.h"

  void watchdogSetup() {
    // do whatever. don't remove this function.
  }

  void MarlinHAL::watchdog_init() {
    iwdg_init();
  }

  void MarlinHAL::watchdog_refresh() {
    iwdg_feed();
  }

#endif

void HAL_watchdog_refresh() {
  MarlinHAL::watchdog_refresh();
}

// ------------------------
// ADC
// ------------------------
// Init the AD in continuous capture mode
void MarlinHAL::adc_init() {}

void MarlinHAL::adc_start(const pin_t adc_pin) {
	TEMP_PINS pin_index;
	switch (adc_pin) {
		case TEMP_0_PIN: pin_index = TEMP_0; break;
		case TEMP_BED_PIN: pin_index = TEMP_BED; break;
    default: pin_index = TEMP_0; break;
	}
	adc_result = adc_read(ADC1,(uint8_t)pin_index);
}

uint16_t analogRead(pin_t pin) {
  const bool is_analog = _GET_MODE(pin) == INPUT_ANALOG;
  return is_analog ? analogRead(uint8_t(pin)) : 0;
}

// Wrapper to maple unprotected analogWrite
void analogWrite(pin_t pin, int pwm_val8) {
  if (PWM_PIN(pin))
    analogWrite(uint8_t(pin), pwm_val8);
}

void flashFirmware(int16_t value) { NVIC_SystemReset(); }

#endif // __STM32F1__
