//***********************************************************************************
// Include files
//***********************************************************************************

#ifndef SRC_HEADER_FILES_SI7021_H_
#define SRC_HEADER_FILES_SI7021_H_

/* System include statements */


/* Silicon Labs include statements */
#include "em_i2c.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_assert.h"
#include "brd_config.h"


/* The developer's include statements */
#include "HW_delay.h"
#include "i2c.h"


//***********************************************************************************
// defined files
//***********************************************************************************

//this callback is used for the SI7021 test, it does not have an associated scheduler function
#define SI7021_TEST_CB              0x00000200

//this callback is used once on startup when the si7021 is configured
#define Si7021_Write_Reg_CB			0x00000100

#define      SI7021_I2C_ADDRESS           0x40

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

 void si7021_i2c_open(I2C_TypeDef *i2c, uint32_t SDA_route, uint32_t SCL_route);

 void si7021_read(uint32_t SI7021_READ_CB, I2C_TypeDef *i2c, uint32_t command);


 void si7021_write(uint32_t SI7021_READ_CB, I2C_TypeDef *i2c, uint32_t command, uint8_t writeData);

 bool si7021_test(I2C_TypeDef *i2c);

 float si7021_return_humidity(void);

 float si7021_return_temperature(void);

 int si7021_return_uReg(void);

#endif /* SRC_HEADER_FILES_SI7021_H_ */
