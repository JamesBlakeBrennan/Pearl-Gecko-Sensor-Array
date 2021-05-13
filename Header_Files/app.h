//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	APP_HG
#define	APP_HG

/* System include statements */
#include "string.h"
#include <stdio.h>
#include "stdlib.h"

/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_assert.h"

/* The developer's include statements */
#include "cmu.h"
#include "gpio.h"
#include "letimer.h"
#include "brd_config.h"
#include "scheduler.h"
#include "sleep_routines.h"
#include "Si7021.h"
#include "ble.h"
#include "HW_Delay.h"
#include "veml6030.h"


//***********************************************************************************
// defined files
//***********************************************************************************

// Application scheduled event

#define LETIMER_COMP0_CB			0x00000001   //0b0001
#define LETIMER_COMP1_CB            0x00000002   //0b0010
#define LETIMER_UF_CB               0x00000004   //0b0100
#define Si7021_Read_Humidity_CB     0x00000008   //0b1000
#define BOOT_UP_CB                  0x00000010  //0b10000

//this callback is used to manage the circular buffer, it is also defined in ble.h
#define BLE_TX_DONE_CB              0x00000020 //0b100000

#define Si7021_Read_Temperature_CB	0x00000040
#define Si7021_Read_Reg_CB			0x00000080

//this callback is used only once on startup, it is also defined in si7021.h
#define Si7021_Write_Reg_CB			0x00000100

#define VEML6030_Write_CB           0x00000400
#define VEML6030_Read_CB            0x00000800

#define SYSTEM_BLOCK_EM				EM3    //MUST BLOCK FOR BLE TEST

//ble test
//#define BLE_TEST_ENABLED
//#define CBUF_TEST_ENABLED






//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void scheduled_letimer0_uf_cb (void);
void scheduled_letimer0_comp0_cb (void);
void scheduled_letimer0_comp1_cb (void);
void scheduled_si7021_humidity_cb (void);
void scheduled_si7021_temperature_cb (void);
void scheduled_si7021_writeReg_cb (void);
void scheduled_si7021_readReg_cb (void);
void scheduled_boot_up_cb (void);
void scheduled_ble_tx_done_cb (void);
void scheduled_veml6030_lux_cb (void);
void scheduled_veml6030_write_cb (void);

#endif
