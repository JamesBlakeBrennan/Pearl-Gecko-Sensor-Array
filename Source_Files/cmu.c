/**
 * @file cmu.c
 * @author James Brennan
 * @date February 7th, 2020
 * @brief Enables oscillators and sets up the clock tree for LETIMER0.
 *
 */



//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Configures the clock tree for LETIMER0
 *
 * @details
 * 	 This routine configures the clock tree for LETIMER0. For this project, we
 * 	 are using the ULFRCO. This oscillator is enabled by default. the LFRCO and
 * 	 LFXO are also enabled by default, so they are disabled. The ULFRCO is then
 * 	 driven to the clock tree where LETIMER0 resides. The LE clock tree must also
 * 	 be enabled.
 *
 *
 * @note
 *   This function should be called once on start up to configure the clock tree.
 *
 *
 *
 ******************************************************************************/

void cmu_open(void){

		CMU_ClockEnable(cmuClock_HFPER, true);

		// By default, Low Frequency Resistor Capacitor Oscillator, LFRCO, is enabled,
		// Disable the LFRCO oscillator
		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	 // What is the enumeration required for LFRCO?

		// Disable the Low Frequency Crystal Oscillator, LFXO
		CMU_OscillatorEnable(cmuOsc_LFXO, true, false);	// What is the enumeration required for LFXO?

		// No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H

		// Route LF clock to LETIMER0 clock tree
		CMU_ClockSelectSet(cmuClock_LFA , cmuSelect_ULFRCO);	// What clock tree does the LETIMER0 reside on?

		//route
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

		// Now, you must ensure that the global Low Frequency is enabled
		CMU_ClockEnable(cmuClock_CORELE, true);	//This enumeration is found in the Lab 2 assignment


}

