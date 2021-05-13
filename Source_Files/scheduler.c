/**
 * @file scheduler.c
 * @author James Brennan
 * @date February 21st, 2020
 * @brief Contains the scheduler specific functions.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static unsigned int event_scheduled;

//***********************************************************************************
// Private functions
//***********************************************************************************

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Opens the scheduler
 *
 * @details
 * 	 This function simply clears the event_scheduled variable
 *
 * @note
 *   Should be called to start or reset the scheduler
 *
 *
 ******************************************************************************/

void scheduler_open(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	event_scheduled = event_scheduled & ~event_scheduled; //clears the variable

	CORE_EXIT_CRITICAL();

}

/***************************************************************************//**
 * @brief
 *   Adds an event
 *
 * @details
 * 	 Ors an event with the event scheduled variable
 *
 * @param[in] event
 *   int defining a new event
 *
 *
 ******************************************************************************/

void add_scheduled_event(uint32_t event){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	event_scheduled = event_scheduled | event;

	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *   Removes an event
 *
 * @details
 * 	 Removes an event from the event scheduler variable using &
 *
 * @param[in] event
 *   int defining an event to remove
 *
 *
 ******************************************************************************/


void remove_scheduled_event(uint32_t event){

	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	event_scheduled = event_scheduled & ~event;

	CORE_EXIT_CRITICAL();

}

/***************************************************************************//**
 * @brief
 *   Returns the event_scheduled variable
 *
 * @details
 * 	 Allows one to check how many events are scheduled
 *
 *+
 *
 ******************************************************************************/

uint32_t get_scheduled_events(void){


	return event_scheduled;


}
