/**
 * @file sleep_routines.c
 * @author James Brennan
 * @date February 21st, 2020
 * @brief Handles sleep routines.
 *
 */

/**************************************************************************
* @file sleep.c
***************************************************************************
* @section License
* <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
***************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
**************************************************************************/



//***********************************************************************************
// Include files
//***********************************************************************************
#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static int lowest_energy_mode [MAX_ENERGY_MODES];

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Configures the variable used to determine the lowest sleep modes
 *
 * @details
 * 	 This routine initializes the private variable used for the sleep functions
 *
 *
 * @note
 *   This function should be called once on start up to configure the sleep functions
 *
 *
 *
 ******************************************************************************/

void sleep_open(void){
	int i;
	for(i = 0; i< MAX_ENERGY_MODES; i++){
		lowest_energy_mode[i] = false;
	}


}

/***************************************************************************//**
 * @brief
 *   Blocks a sleep mode
 *
 * @details
 * 	 This routine sends a block order for a particular energy mode
 *
 *
 * @param[in] EM
 *   The energy mode to be blocked
 *
 *
 *
 ******************************************************************************/

void sleep_block_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	//code here
	lowest_energy_mode[EM] = lowest_energy_mode[EM]+1;

	EFM_ASSERT (lowest_energy_mode[EM] < 5);

	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *   Unblocks a sleep mode
 *
 * @details
 * 	 This routine sends an unblock order for a particular energy mode
 *
 *
 * @param[in] EM
 *   The energy mode to be unblocked
 *
 *
 *
 ******************************************************************************/

void sleep_unblock_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	//code here
	lowest_energy_mode[EM] = lowest_energy_mode[EM]-1;

	EFM_ASSERT (lowest_energy_mode[EM] >= 0);

	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *   Enters a sleep mode
 *
 * @details
 * 	 This routine enters the lowest sleep mode
 *
 *
 *
 *
 *
 ******************************************************************************/

void enter_sleep(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();


	if(lowest_energy_mode[EM0]>0){
		CORE_EXIT_CRITICAL();
		return;
	}
	else if(lowest_energy_mode[EM1]>0){
		CORE_EXIT_CRITICAL();
		return;
	}
	else if(lowest_energy_mode[EM2]>0){
		EMU_EnterEM1();
		CORE_EXIT_CRITICAL();
		return;
		}
	else if(lowest_energy_mode[EM3]>0){
		EMU_EnterEM2(true);
		CORE_EXIT_CRITICAL();
		return;
		}
	else{
		EMU_EnterEM3(true);
		CORE_EXIT_CRITICAL();
		return;
	}



}

/***************************************************************************//**
 * @brief
 *   Returns the current lowest energy mode
 *
 * @details
 * 	 This routine returns the lowest current energy mode
 *
 *
 * @return
 *	 The current lowest energy mode
 *
 *
 ******************************************************************************/

uint32_t current_block_energy_mode(void){
int i =  0;

while(1){
	if(  i == 0){
		if(i<MAX_ENERGY_MODES){
			if(lowest_energy_mode[i] != 0){
				return i;
			}
			else{
				i = i+1;
			}
		}
		else{
		return MAX_ENERGY_MODES - 1;
		}
	}
	else{
		if(i<MAX_ENERGY_MODES){
			if(lowest_energy_mode[i] != 0){
				return i;
				}
			else{
				i = i+1;
			}
		}
		else{
		return MAX_ENERGY_MODES - 1;
		}
	}

}


}



