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
#include "Si7021.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static uint32_t hdata;
static uint32_t tdata;
static uint32_t uReg;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Driver to open I2C to the si7021
 *
 * @details
 * 	 This routine is a low level driver.  The application code calls this function
 * 	 once to open I2C communication with the si7021
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

void si7021_i2c_open(I2C_TypeDef *i2c, uint32_t SDA_route, uint32_t SCL_route ){

	//timer delay as per the si7021 power on specifications
	 //timer_delay(80);

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
	 //while(i2c->STATE & I2C_STATE_BUSY);

	 uint8_t writeData = 0b00000001;

	 //running the test
	 EFM_ASSERT(si7021_test(i2c));
	 si7021_write(Si7021_Write_Reg_CB, i2c,  WRITE_REG, writeData);
	 //delaying after writing to the si7021 user register as per specifications
	// timer_delay(15);


 }

/***************************************************************************//**
 * @brief
 *   Function to read from the si7021
 *
 * @details
 * 	 This function should be called whenever one wishes to read humidity or
 * 	 temperature
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

void si7021_read(uint32_t SI7021_READ_CB, I2C_TypeDef *i2c, uint32_t command){
	uint32_t address = SI7021_I2C_ADDRESS;
	if(command == READ_HUM){
		I2C_Start(i2c, &hdata, address, SI7021_READ_CB, command, false);
	}

	else if(command == READ_TEMP){
		I2C_Start(i2c, &tdata, address, SI7021_READ_CB, command, false);
	}
	else if(command == READ_REG){
		I2C_Start(i2c, &uReg, address, SI7021_READ_CB, command, false);
	}

}

/***************************************************************************//**
 * @brief
 *   This function writes to the si7021
 *
 * @details
 * 	 This function can be used to change the resolution of the si7021 by writing to
 * 	 the user register.
 *
 *
 *
 * @param[in] SI7021_READ_CB
 *   Callback for whichever write operation is occuring
 *
 * @param[in] i2c
 *   i2c peripheral being used
 *
 * @param[in] command
 *  command to be sent to the si7021
 *
 * @param[in] writeData
 *   Data to be written to the si7021
 *
 *
 ******************************************************************************/


void si7021_write(uint32_t SI7021_READ_CB, I2C_TypeDef *i2c, uint32_t command, uint8_t writeData){
	uint32_t address = SI7021_I2C_ADDRESS;
	I2C_Start(i2c, false, address, SI7021_READ_CB, command, writeData);


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

float si7021_return_humidity(void){
	float humidity;
	humidity = (125*((float)(hdata))/65536) - 6; //equation for humidity conversion
	return humidity;
}

/***************************************************************************//**
 * @brief
 *   This function to returns the temperature value from the si7021
 *
 * @details
 * 	 This function should be called as part of the scheduler after the I2C state machine has completed
 * 	 a temperature read
 *
 *
 *
 * @param[out] temperature
 *     The temperature reading from a previous humidity measurement
 *
 *
 ******************************************************************************/


float si7021_return_temperature(void){
	float temp;
	temp = (175.72*((float)(tdata))/65536) - 46.85; //equation for temperature conversion
	return temp;
}

/***************************************************************************//**
 * @brief
 *   This function returns the value of the si7021 user register
 *
 * @details
 * 	 This function should be called as part of the scheduler after the I2C state machine has completed
 * 	 its si7021 user register read
 *
* @param[out] uReg
 *   The value of the si7021 user register
 *
 ******************************************************************************/

int si7021_return_uReg(void){

	return uReg;

}


/***************************************************************************//**
 * @brief
 *   si7021 test tests whether i2c communication to the si7021 has been properly
 *   configured.
 *
 * @details
 * 	 Si7021 performs four primary functions:
 * 	 It first writes to the user register to reset it to its default
 * 	 state.
 *
 * 	 It then writes to the register to change the resolution of temperature
 * 	 and humidity output.
 *
 * 	 After this the function attempts a humidity and temperature read
 *
 * 	 It then resets the si7021 to default settings
 *
 * 	 Each of these is following by a check of some kind to ensure it was successful.
 * 	 In the case of a write, the function will check that it was successful by then
 * 	 performing a read and comparing it to the expected value.
 *
 * @note
 *   The rest resets the state of the  si7021 before completion, so configuring
 *   of the si7021 must also be perform in application code
 *
 * @param[in] *i2c
 *   This specifies the i2c bus we are using to test the si7021
 *
 * @return
 *   Returns bool true if successfully passed through the tests in this
 *   function.
 ******************************************************************************/


bool si7021_test(I2C_TypeDef *i2c){

	//initializing data required for the test
	uint32_t address = SI7021_I2C_ADDRESS;
	uint8_t writeData = 0b00000000;
	uReg = 0;

	//changes the si7021 to default settings, in case the test is run multiple times in a row or
	//immediately after application code
	I2C_Start(i2c, false, address, SI7021_TEST_CB, WRITE_REG, writeData);
	//delaying after writing to the si7021 user register as per specifications
	timer_delay(15);

	//these while loops are used throughout the test, they make sure it does not continue until the i2c is free
	while(!(get_scheduled_events() & (SI7021_TEST_CB)));
	remove_scheduled_event(SI7021_TEST_CB);

	//reads from the user register to check if the si7021 is indeed in default settings
	I2C_Start(i2c, &uReg, address, SI7021_TEST_CB, READ_REG, false);
	while(!(get_scheduled_events() & (SI7021_TEST_CB)));
	remove_scheduled_event(SI7021_TEST_CB);
	EFM_ASSERT(uReg == 58); //58 is the default value of the user register in decimal

	writeData = 0b00000001;
	//writes to the user register to change the resolution on temp/humidity
	I2C_Start(i2c, false, address, SI7021_TEST_CB, WRITE_REG, writeData);
	while(!(get_scheduled_events() & (SI7021_TEST_CB)));
	remove_scheduled_event(SI7021_TEST_CB);
	//delaying after writing to the si7021 user register as per specifications
	timer_delay(15);

	//reads from the user register to ensure the write was successful
	I2C_Start(i2c, &uReg, address, SI7021_TEST_CB, READ_REG, false);
	while(!(get_scheduled_events() & (SI7021_TEST_CB)));
	remove_scheduled_event(SI7021_TEST_CB);
	EFM_ASSERT(uReg == 59);//59 is the value of the user register, in decimal, after the resolution has been changed to 8/12

	//takes a humidity reading
	hdata = 0;											//sets hdata to 0
	I2C_Start(i2c, &hdata, address, SI7021_TEST_CB, READ_HUM, false);
	while(!(get_scheduled_events() & (SI7021_TEST_CB)));
	remove_scheduled_event(SI7021_TEST_CB);
	float hum = si7021_return_humidity();
	EFM_ASSERT((hum > 20) && (hum < 50));								//if hdata is non-zero than a read has occurred

	//takes a temperature reading											//sets tdata to 0
	I2C_Start(i2c, &tdata, address, SI7021_TEST_CB, READ_TEMP, false);
	while(!(get_scheduled_events() & (SI7021_TEST_CB)));
	remove_scheduled_event(SI7021_TEST_CB);
	float temp = si7021_return_temperature();
	EFM_ASSERT((temp > 20) && (temp < 30));								//if tdata is non-zero than a read has occurred

	//resets the si7021 to default settings
	writeData = 0b00000000;
	I2C_Start(i2c, false, address, SI7021_TEST_CB, WRITE_REG, writeData);
	while(!(get_scheduled_events() & (SI7021_TEST_CB)));
	remove_scheduled_event(SI7021_TEST_CB);
	//delaying after writing to the si7021 user register as per specifications
	timer_delay(15);


	return true;


}



