#ifndef BRD_CONFIG_HG
#define BRD_CONFIG_HG

//***********************************************************************************
// Include files
//***********************************************************************************
/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_i2c.h"

/* The developer's include statements */


//***********************************************************************************
// defined files
//***********************************************************************************

//PWM period and route

#define		PWM_PER				1.8		// PWM period in seconds
#define		PWM_ACT_PER			0.25	// PWM active period in seconds
#define     PWM_ROUTE_0         28      //Output for PWM, LED 0
#define     PWM_ROUTE_1         28      //Output for PWM, LED 1

//I2C routing

#define     I2C1_SCL_ROUTE      19      //Output for I2C0 to SI7021, SCL
#define     I2C1_SDA_ROUTE		19		//Output for I2C0 to SI7021, SDA
#define     I2C0_SCL_ROUTE		6      //Output for I2C1 to veml6030, SCL
#define     I2C0_SDA_ROUTE		8		//Output for I2C0 to veml6030, SDA

//si7021 commands
#define READ_REG				   0xE7
#define READ_TEMP				   0xE0
#define READ_HUM				   0xF5
#define WRITE_REG                  0xE6

//veml commands
#define Veml_WRITE				   0x00
#define Veml_READ                  0x04


//SI7021 pins are
#define SI7021_SCL_PORT gpioPortC  //I2C0_SDA #16 I2C0_SCL #15 I2C1_SDA #20 I2C1_SCL #19
#define SI7021_SCL_PIN 11u
#define SI7021_SDA_PORT gpioPortC
#define SI7021_SDA_PIN 10u
#define SI7021_SENSOR_EN_PORT gpioPortB
#define SI7021_SENSOR_EN_PIN 10u


//VEML6030 pins are
#define VEML6030_SCL_PORT gpioPortB  //I2C0_SDA #16 I2C0_SCL #15 I2C1_SDA #20 I2C1_SCL #19
#define VEML6030_SCL_PIN 12u
#define VEML6030_SDA_PORT gpioPortB
#define VEML6030_SDA_PIN 13u
#define VEML6030_SENSOR_EN_PORT gpioPortB
#define VEML6030_SENSOR_EN_PIN 10u

//compiler directive for selecting I2C peripheral
#ifdef Select_I2C_One
	#define I2C I2C1
#else
	#define I2C I2C0
#endif

// GPIO pin setup
#define STRONG_DRIVE

// LED 0 pin is
#define	LED0_PORT				gpioPortF
#define LED0_PIN				04u
#define LED0_DEFAULT			false 	// Default false (0) = off, true (1) = on
#define LED0_GPIOMODE			gpioModePushPull

// LED 1 pin is
#define LED1_PORT				gpioPortF
#define LED1_PIN				05u
#define LED1_DEFAULT			false	// Default false (0) = off, true (1) = on
#define LED1_GPIOMODE			gpioModePushPull

#ifdef STRONG_DRIVE
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
#else
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
#endif

//LEUART RX AND TX
#define UART_TX_PORT            gpioPortD
#define UART_TX_PIN             10u

#define UART_RX_PORT            gpioPortD
#define UART_RX_PIN             11u


// System Clock setup
#define MCU_HFXO_FREQ			cmuHFRCOFreq_32M0Hz //setting cpu freq to 32MHz


// LETIMER PWM Configuration
//#define		PWM_ROUTE_0			LETIMER_ROUTELOC0_OUT0LOC_LOC01
//#define		PWM_ROUTE_1			LETIMER_ROUTELOC0_OUT1LOC_LOC01



//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

#endif
