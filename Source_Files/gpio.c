/**
 * @file gpio.c
 * @author James Brennan
 * @date February 7th, 2020
 * @brief Configures the GPIO for LED0 and LED1
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"


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
 *   Configures the GPIO for all peripherals
 *
 * @details
 * 	 This routine configures the GPIO for all peripherals. First the GPIO clock is
 * 	 enabled which allows the relevant pins to receive writes and reads. The drive strength
 * 	 and pinmode for each peripheral is then set.
 *
 *
 * @note
 *   This function should be called once on start up to configure the GPIO.
 *
 *
 *
 ******************************************************************************/

void gpio_open(void){

	CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure LED pins
	GPIO_DriveStrengthSet(LED0_PORT, LED0_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED0_PORT, LED0_PIN, LED0_GPIOMODE, LED0_DEFAULT);

	GPIO_DriveStrengthSet(LED1_PORT, LED1_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, LED1_DEFAULT);

	//configure humidity sensor
	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, gpioModePushPull, true);

	GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, gpioModeWiredAnd, true);
	GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, gpioModeWiredAnd, true);

	//configure light sensor

	GPIO_PinModeSet(VEML6030_SDA_PORT, VEML6030_SDA_PIN, gpioModeWiredAnd, true);
	GPIO_PinModeSet(VEML6030_SCL_PORT, VEML6030_SCL_PIN, gpioModeWiredAnd, true);

	//configure LEUART
	//TX
	GPIO_DriveStrengthSet(UART_TX_PORT, gpioDriveStrengthStrongAlternateWeak);
	GPIO_PinModeSet(UART_TX_PORT, UART_TX_PIN, gpioModePushPull, LED1_DEFAULT);
	//RX
	GPIO_PinModeSet(UART_RX_PORT, UART_RX_PIN, gpioModeInput, LED1_DEFAULT);
}
