//***********************************************************************************
// Include files
//***********************************************************************************

#ifndef SRC_HEADER_FILES_VEML6030_H_
#define SRC_HEADER_FILES_VEML6030_H_

/* System include statements */


/* Silicon Labs include statements */
#include "em_i2c.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_assert.h"
#include "brd_config.h"


/* The developer's include statements */

#include "i2c.h"


//***********************************************************************************
// defined files
//***********************************************************************************

#define      VEML3060_I2C_READ_ADDRESS           0x48
#define      VEML3060_I2C_WRITE_ADDRESS			 0x48

//this callback is used to stall during the veml6030 write on startup
#define VEML6030_Write_CB           0x00000400

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

 void veml6030_i2c_open(I2C_TypeDef *i2c, uint32_t SDA_route, uint32_t SCL_route);

 void veml6030_read(uint32_t VEML6030_CB, I2C_TypeDef *i2c, uint32_t command);


 void veml6030_write(uint32_t VEML6030_CB, I2C_TypeDef *i2c, uint32_t command, uint8_t writeData);

 //bool veml6030_test(I2C_TypeDef *i2c);

 float veml6030_return_lux(void);



#endif /* SRC_HEADER_FILES_VEML6030_H_ */
