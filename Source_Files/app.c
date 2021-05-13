/**
 * @file app.c
 * @author James Brennan
 * @date February 7th, 2020
 * @brief Contains the application specific functions. Details about PWM can be
 * configured here using the letimer_pwm_struct located inapp_letimer_pwm_open.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************

static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Sets up the peripherals and starts the LETIMER
 *
 * @details
 *	This function configures the clock tree to the peripherals, GPIO, and the
 *	LETIMER's registers for PWM. It should be called after the chip is initialized
 *	to start the application.
 *
 *
 * @note
 *	Please see individual functions for more information.
 *
 ******************************************************************************/

void app_peripheral_setup(void){

	cmu_open();
	gpio_open();
	scheduler_open();
	sleep_open();
	sleep_block_mode(SYSTEM_BLOCK_EM);

	veml6030_i2c_open(I2C0, I2C0_SDA_ROUTE, I2C0_SCL_ROUTE );
	si7021_i2c_open(I2C1, I2C1_SDA_ROUTE, I2C1_SCL_ROUTE );




	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1);

	ble_open();
	//letimer_start(LETIMER0, true);   // letimer_start will inform the LETIMER0 peripheral to begin counting. (moved to scheduler)
	//add_scheduled_event(BOOT_UP_CB);

}

/***************************************************************************//**
 * @brief
 * Initializes the letimer_pwm_struct and then calls letimer_pwm_open
 *
 *
 * @details
 * The letimer_pwm_struct is used to configure details about the pwm operation.
 * This includes the period, active period, gpio, and debug options. The timer
 * itself should be disabled here as it is enabled by a different function once
 * configuration is complete.
 *
 * @note
 *
 *
 * @param[in] period
 *	float which defines the total PWM period
 *
 * @param[in] act_period
 *	float which defines the PWM active period
 *
 * @param[in] out0_route
 *	integer which defines the location of the LETIMER OUT0 pin
 *
 * @param[in] out1_route
 *	integer which defines the location of the LETIMER OUT1 pin
 *
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route){
	// Initializing LETIMER0 for PWM operation by creating the
	// letimer_pwm_struct and initializing all of its elements

	APP_LETIMER_PWM_TypeDef letimer_pwm_struct;
	letimer_pwm_struct.debugRun = false;
	letimer_pwm_struct.enable = false;
	letimer_pwm_struct.out_pin_route0 = out0_route;
	letimer_pwm_struct.out_pin_route1 = out1_route;
	letimer_pwm_struct.out_pin_0_en = false;   //disabling LEDs
	letimer_pwm_struct.out_pin_1_en = false;   //disabling LEDs
	letimer_pwm_struct.period = period;
	letimer_pwm_struct.active_period = act_period;

	letimer_pwm_struct.comp0_irq_enable = false;
	letimer_pwm_struct.comp0_cb = LETIMER_COMP0_CB;
	letimer_pwm_struct.comp1_irq_enable = false;
	letimer_pwm_struct.comp1_cb = LETIMER_COMP1_CB;
	letimer_pwm_struct.uf_irq_enable = true;
	letimer_pwm_struct.uf_cb = LETIMER_UF_CB;

	letimer_pwm_open(LETIMER0, &letimer_pwm_struct);

}

/***************************************************************************//**
 * @brief
 * Handles the UF event
 *
 *
 * @details
 * At this point the UF event handles sleep modes
 *
 * @note
 *

 *
 ******************************************************************************/

void scheduled_letimer0_uf_cb (void){


	EFM_ASSERT(get_scheduled_events() & LETIMER_UF_CB);
	remove_scheduled_event(LETIMER_UF_CB);
	/* uint32_t currentmode = current_block_energy_mode();
	sleep_unblock_mode(currentmode);

	if(currentmode < 4){
		sleep_block_mode(currentmode+1);
	}
	else{
		sleep_block_mode(EM0);
	}*/
	//

	si7021_read(Si7021_Read_Humidity_CB, I2C1, READ_HUM);
	veml6030_read(VEML6030_Read_CB, I2C0,  0x04);


}

/***************************************************************************//**
 * @brief
 * Handles the COMP0 event
 *
 *
 * @details
 * details here
 *
 * @note
 *

 *
 ******************************************************************************/

void scheduled_letimer0_comp0_cb (void){

	remove_scheduled_event(LETIMER_COMP0_CB);
	EFM_ASSERT(false);
}

/***************************************************************************//**
 * @brief
 * Handles the COMP1 event
 *
 *
 * @details
 * details here
 *
 * @note
 *

 *
 ******************************************************************************/
void scheduled_letimer0_comp1_cb (void){



	remove_scheduled_event(LETIMER_COMP1_CB);
	EFM_ASSERT(false);

}


/***************************************************************************//**
 * @brief
 * Handles the Si7021 humidity event
 *
 *
 * @details
 * This function handles the si7021 humidity callback, which is set by the si7021
 * read function. The si7021 read function is called every 1.8s from the letimer0
 * callback function.
 *
 * @note
 *

 *
 ******************************************************************************/
void scheduled_si7021_humidity_cb (void){


	EFM_ASSERT(get_scheduled_events() & (Si7021_Read_Humidity_CB));


	remove_scheduled_event(Si7021_Read_Humidity_CB);

	char humidity_str[25];

	float hdata;
	hdata = si7021_return_humidity();



	sprintf(humidity_str,"\nHumidity = %.1f %%\n", hdata);

	ble_write(humidity_str);


	if(hdata >= 45.0f){
			GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, ~LED1_DEFAULT);
		}
	else{
			GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, LED1_DEFAULT);
		}
	si7021_read(Si7021_Read_Temperature_CB, I2C1, READ_TEMP);

}

/***************************************************************************//**
 * @brief
 * Handles the Si7021 temperature event
 *
 *
 * @details
 * This function handles the si7021 temperature callback, which is set by the si7021
 * read function. The si7021 read function which uses this callback is called from the
 * humidity callback function after a humidity read is complete.
 * @note
 *

 *
 ******************************************************************************/

void scheduled_si7021_temperature_cb (void){
	EFM_ASSERT(get_scheduled_events() & (Si7021_Read_Temperature_CB));
	remove_scheduled_event(Si7021_Read_Temperature_CB);


	char temp_str[25];

	//float hdata;
	//hdata = si7021_return_humidity();

	//sprintf(humidity_str,"Humidity = %.1f %%",hdata);




	//char temperature_str[25];

	float tdata;
	tdata = si7021_return_temperature();

	sprintf(temp_str,"Temperature = %.1f C\n", tdata);





	ble_write(temp_str);

}

/***************************************************************************//**
 * @brief
 * Handles the veml6030 write event
 *
 *
 * @details
 * This function handles the veml6030 write callback, which is set by the veml3060
 * write function. This callback function should only run once, on startup, in order
 * to start the VEML
 * @note
 *

 *
 ******************************************************************************/

void scheduled_veml6030_write_cb (void){
	EFM_ASSERT(get_scheduled_events() & (VEML6030_Write_CB));
	remove_scheduled_event(VEML6030_Write_CB);
	//veml6030_read(VEML6030_Read_CB, I2C0,  0x04);



}

/***************************************************************************//**
 * @brief
 * Handles the veml6030 lux event
 *
 *
 * @details
 * This function handles the veml3060 lux callback, which is set by the veml6030
 * read function. The veml6030 read function is called every 1.8s from the letimer0
 * uf callback. This function takes the lux reading and sends it to the bluetooth
 * via the ble_write function.
 * @note
 *

 *
 ******************************************************************************/

void scheduled_veml6030_lux_cb (void){
	EFM_ASSERT(get_scheduled_events() & (VEML6030_Read_CB));
	remove_scheduled_event(VEML6030_Read_CB);


	char lux_str[25];

	//float hdata;
	//hdata = si7021_return_humidity();

	//sprintf(humidity_str,"Humidity = %.1f %%",hdata);




	//char temperature_str[25];

	float ldata;
	ldata = veml6030_return_lux();

	sprintf(lux_str,"Lux = %.1f \n", ldata);





	ble_write(lux_str);

}

/***************************************************************************//**
 * @brief
 * Handles the si7021 write register event
 *
 *
 * @details
 * This function handles the si7021 write-register callback. It is generally only
 * called to reconfigure the resolution on the si7021.
 * @note
 * Is used several times during the si7021 TDD function

 *
 ******************************************************************************/

void scheduled_si7021_writeReg_cb (void){
	EFM_ASSERT(get_scheduled_events() & (Si7021_Write_Reg_CB));
	remove_scheduled_event(Si7021_Write_Reg_CB);

	si7021_read(Si7021_Read_Reg_CB, I2C1, 0xE7);

}

/***************************************************************************//**
 * @brief
 * Handles the si7021 read register event
 *
 *
 * @details
 * This function handles the si7021 read-register callback. It can be used to check
 * the state of the si7021's command register and confirm that a write was successful
 *
 * @note
 * Is used several times during the TDD function

 *
 ******************************************************************************/
void scheduled_si7021_readReg_cb (void){
	EFM_ASSERT(get_scheduled_events() & (Si7021_Read_Reg_CB));
	remove_scheduled_event(Si7021_Read_Reg_CB);
	int uReg;
	uReg = si7021_return_uReg();
	EFM_ASSERT(uReg == 59);
	add_scheduled_event(BOOT_UP_CB);

}


/***************************************************************************//**
 * @brief
 * Handles the boot_up_cb event
 *
 *
 * @details
 * This event runs the ble_test if BLE_TEST_ENABLED is defined. It then runs the
 * circular buffer test. After the tests are complete the function then starts the
 * letimer. It runs only once on startup.
 *
 *
 * @note
 * letimer_start has been moved to this function to ensure that all the peripherals
 * have been configured correctly before interrupts begin occurring.

 *
 ******************************************************************************/
void scheduled_boot_up_cb (void){


	EFM_ASSERT(get_scheduled_events() & BOOT_UP_CB);
	remove_scheduled_event(BOOT_UP_CB);

	//letimer_start(LETIMER0, true);   // letimer_start will inform the LETIMER0 peripheral to begin counting.

	#ifdef BLE_TEST_ENABLED
	EFM_ASSERT(ble_test("JBBtooth"));
	#endif

	#ifdef CBUF_TEST_ENABLED
	circular_buff_test();
	timer_delay(2000);
	#endif

	ble_write("\nHello World\n");
	ble_write("ADC Lab\n");
	ble_write("James Brennan\n");


	letimer_start(LETIMER0, true);   // letimer_start will inform the LETIMER0 peripheral to begin counting.

}

/***************************************************************************//**
 * @brief
 * Handles the ble_tx_done event
 *
 *
 * @details
 *	This function is called each time a transfer has completed to the ble module.
 *	It checks the circular buffer for any strings waiting to be popped.
 *
 *
 *
 * @note
 * This function is key to the proper operation of the circular buffer

 *
 ******************************************************************************/

void scheduled_ble_tx_done_cb (void){
	EFM_ASSERT(get_scheduled_events() & BLE_TX_DONE_CB);
	remove_scheduled_event(BLE_TX_DONE_CB);

	ble_circ_pop(CIRC_OPER);




}


