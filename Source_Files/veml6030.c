  /**
 * @file Si7021.c
 * @author James Brennan
 * @date March 13th, 2020
 * @brief Contains all the Si7021 driver functions
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries

//** Silicon Lab include files

//** User/developer include files
#include "veml6030.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static uint32_t ldata;


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Driver to open I2C to the veml6030
 *
 * @details
 * 	 This routine is a low level driver.  The application code calls this function
 * 	 once to open I2C communication with the veml6030
 *
 * @note
 *   This function is normally called once to initialize I2C to the peripheral
 *
 * @param[in] i2c
 *   Pointer to the base peripheral address of the I2C peripheral being opened
 *
 * @param[in] SDA_route
 *   SDA out-route for the i2c peripheral being used
 *
 * @param[in] SCL_route
 *   SCL out-route for the i2c peripheral being used
 *
 ******************************************************************************/

void veml6030_i2c_open(I2C_TypeDef *i2c, uint32_t SDA_route, uint32_t SCL_route ){

	 I2C_OPEN_STRUCT i2c_open_struct;
	 //not ready to enable i2c yet
	 i2c_open_struct.enable = true;
	 //setting to master
	 i2c_open_struct.master = true;
	 //using the currently configured reference clock
	 i2c_open_struct.refFreq = false;
	 //using maximum possible freq, limited by the Si7021 whose max is 400kHz
	 i2c_open_struct.freq = I2C_FREQ_FAST_MAX;   //about 392 kHz
	 //Using the appropriate ratio for the max freq we selected
	 i2c_open_struct.clhr = i2cClockHLRAsymetric;

	 //setting up and enabling the output pins
	 i2c_open_struct.out_pin_SDA = SDA_route;
	 i2c_open_struct.out_pin_SCL = SCL_route;
	 i2c_open_struct.out_pin_SDA_en = true;
	 i2c_open_struct.out_pin_SCL_en = true;


	 i2c_open(i2c, &i2c_open_struct);


	 uint16_t writeData = 0b0000000000000000;
	 veml6030_write(VEML6030_Write_CB , i2c, Veml_WRITE, writeData);
	 while(!(get_scheduled_events() & (VEML6030_Write_CB)));




 }

/***************************************************************************//**
 * @brief
 *   Function to read from the veml6030
 *
 * @details
 * 	 This function should be called whenever one wishes to read humidity
 *
 *
 *
 * @param[in] i2c
 *   Pointer to the base peripheral address of the I2C peripheral being opened
 *
 * @param[in] SI7021_READ_CB
 *   Callback variable used as part of the scheduler
 *
 *
 ******************************************************************************/

void veml6030_read(uint32_t VEML6030_CB, I2C_TypeDef *i2c, uint32_t command){
	uint32_t address = 0x48;

	I2C_Start(i2c, &ldata, address, VEML6030_CB, command, false);




}


/***************************************************************************//**
 * @brief
 *   This function to returns the humidity value from the si7021
 *
 * @details
 * 	 This function should be called as part of the scheduler after the I2C state machine has completed
 *
 *
 *
 * @param[out] humidity
 * Returns the humidity read on the si7021
 *
 *
 ******************************************************************************/


void veml6030_write(uint32_t VEML6030_CB, I2C_TypeDef *i2c, uint32_t command, uint8_t writeData){
	uint32_t address =0x48;
	I2C_Start(i2c, false, address, VEML6030_CB , command, writeData);


}

/***************************************************************************//**
 * @brief
 *   This function to returns the lux value from the veml6030
 *
 * @details
 * 	 This function should be called as part of the scheduler after the relevant I2C state machine has completed
 *
 *
 *
 * @param[out] lux
 * Returns the lux read on the veml6030
 *
 *
 ******************************************************************************/

float veml6030_return_lux(void){
	//float humidity;
	float lux = 0.0576*((float)(ldata)); //equation for lux conversion based on current settings, gain =  1, integration time = 100ms

    ldata = 0; //resetting ldata before the next read
	return lux;
}






