/**
 * @file i2c.c
 * @author James Brennan
 * @date March 10th, 2020
 * @brief Contains all the I2C driver functions
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries

//** Silicon Lab include files

//** User/developer include files
#include "i2c.h"

//***********************************************************************************
// defined files
//***********************************************************************************
enum state{Call, sendCommand, Read, MS, LS, MStop, WriteDone, writeLSB, writeMSB};
//enum I2C{I2C_0, I2C_1};


//***********************************************************************************
// Private variables
//***********************************************************************************

static I2C_STATE_MACHINE i2c0_State;
static I2C_STATE_MACHINE i2c1_State;

//***********************************************************************************
// Private functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Function to reset the I2C bus
 *
 * @details
 * 	 This routine is a low level driver.  The application code calls this function
 * 	 to reset the bus before enabling interupts
 *
 *
 * @param[in] i2c
 *   Pointer to the base peripheral address of the I2C peripheral being opened
 *
 *
 ******************************************************************************/

void static i2c_bus_reset(I2C_TypeDef *i2c){
	uint32_t savestate;
	//resting the i2c state machine
	i2c->CMD = I2C_CMD_ABORT;

	//saving the state of IEN register
	savestate = i2c->IEN;

	//disabling interrupts
	i2c->IEN = ~i2c->IEN;

	//clearing all interrupts
	i2c->IFC = savestate;


	//clear the transmit buffer

	i2c->CMD = I2C_CMD_CLEARTX;

	////Perform reset by writing to the start and stop bits simultaneously
	i2c->CMD = I2C_CMD_START | I2C_CMD_STOP;



	//stalling while reset is finished
	while(!(i2c->IF & I2C_IF_MSTOP));

	//clearing all interrupts
	i2c->IFC = i2c->IF;


	//restoring the IEN

	i2c->IEN = savestate;

	//resting the i2c state machine
	i2c->CMD = I2C_CMD_ABORT;
}

/***************************************************************************//**
 * @brief
 *   Function handle the ACK interrupt
 *
 * @details
 * 	 This routine is part of the I2C state machine, it handles the ACK interrupt and
 * 	 all relevant state transitions.
 *
 *
 * @param[in] i2cState
 *   Pointer to the STRUCT which handles the state machine
 *
 *
 ******************************************************************************/


void static i2c_ACK_fun(I2C_STATE_MACHINE *i2cState){
	switch(i2cState->STATE){
		case Call:{
			i2cState->i2c->TXDATA  = i2cState->Command; //sends the command to the peripheral
			i2cState->STATE = sendCommand;
			if(i2cState->Command == Veml_WRITE){     //in the case of turning on the veml, the state machine re-directs
				i2cState->STATE = writeLSB;
				break;
			}
			else{
			break;
			}
		}
		case sendCommand:{
			if((i2cState->Command == READ_TEMP) || (i2cState->Command == READ_HUM) || (i2cState->Command == READ_REG) || (i2cState->Command == Veml_READ)){
				i2cState->i2c->CMD        = I2C_CMD_START;       //Start CMD
				i2cState->i2c->TXDATA     = (i2cState->slaveAddress<<1)|(true); //Address + read command
				i2cState->STATE = Read;
			}
			else if(i2cState->Command ==  WRITE_REG){
				i2cState->i2c->TXDATA   = i2cState->writeData;
				i2cState->STATE = WriteDone;
			}
			break;
		}
		case Read:{
			if(i2cState->Command == Veml_READ){ //handles the correct order of reads for the  veml vs si7021
				i2cState->STATE = LS;
			}
			else{
			i2cState->STATE = MS;
			}
			break;
		}
		case WriteDone:{
			i2cState->i2c->CMD  = I2C_CMD_STOP;
			i2cState->STATE = MStop;
			break;
		}
		case writeLSB:{
			i2cState->i2c->TXDATA   = i2cState->writeData;         //this and the following case handles the two byte writes
			i2cState->STATE = writeMSB;							   //required for the veml.
			break;
		}
		case writeMSB:{
				i2cState->i2c->TXDATA   = (i2cState->writeData);
				i2cState->STATE = WriteDone;
				break;
		}

		default:
			EFM_ASSERT(false);
			break;
	}

}

/***************************************************************************//**
 * @brief
 *   Function handle the NACK interrupt
 *
 * @details
 * 	 This routine is part of the I2C state machine, it handles the NACK interrupt and
 * 	 all relevant state transitions.
 *
 *
 * @param[in] i2cState
 *   Pointer to the STRUCT which handles the state machine
 *
 *
 ******************************************************************************/

void static i2c_NACK_fun(I2C_STATE_MACHINE *i2cState){
	switch(i2cState->STATE){
		case Read:{
			i2cState->i2c->CMD        = I2C_CMD_START;       //Start CMD
			i2cState->i2c->TXDATA     = (i2cState->slaveAddress<<1)|(true); //Address + read command
			break;
		}
		case Call:{
			i2cState->i2c->CMD        = I2C_CMD_START;       //Start CMD
			i2cState->i2c->TXDATA     = (i2cState->slaveAddress<<1)|(false); //Loading address write
			break;
		}
		default:
			EFM_ASSERT(false);
			break;
	}

}

/***************************************************************************//**
 * @brief
 *   Function handle the RXDATAV interrupt
 *
 * @details
 * 	 This routine is part of the I2C state machine, it handles the RXDATAV interrupt and
 * 	 all relevant state transitions.
 *
 *
 * @param[in] i2cState
 *   Pointer to the STRUCT which handles the state machine
 *
 *
 ******************************************************************************/
void static i2c_RXDATAV_fun(I2C_STATE_MACHINE *i2cState){
	switch(i2cState->STATE){
		case MS:{
		if((i2cState->Command == READ_TEMP) || (i2cState->Command == READ_HUM)){
		*(i2cState->Data) = i2cState->i2c->RXDATA<<8;
		i2cState->STATE = LS;
		i2cState->i2c->CMD  = I2C_CMD_ACK;
		}
		else if(i2cState->Command == READ_REG){
			*(i2cState->Data) = i2cState->i2c->RXDATA;
			i2cState->STATE = MStop;
			i2cState->i2c->CMD  = I2C_CMD_NACK;
			i2cState->i2c->CMD  = I2C_CMD_STOP;
		}
		else if(i2cState->Command == Veml_READ){
			*(i2cState->Data) |= i2cState->i2c->RXDATA<<8;
			i2cState->STATE = MStop;
			i2cState->i2c->CMD  = I2C_CMD_NACK;
			i2cState->i2c->CMD  = I2C_CMD_STOP;
		}
		break;
		}
		case LS:{
		if(i2cState->Command == Veml_READ){
			*(i2cState->Data) |= i2cState->i2c->RXDATA;
			i2cState->STATE = MS;
			i2cState->i2c->CMD  = I2C_CMD_ACK;
		}
		else{
			*(i2cState->Data) |= i2cState->i2c->RXDATA;
			i2cState->STATE = MStop;
			i2cState->i2c->CMD  = I2C_CMD_NACK;
			i2cState->i2c->CMD  = I2C_CMD_STOP;
		}
		break;
		}
		default:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *   Function handle the MSTOP interrupt
 *
 * @details
 * 	 This routine is part of the I2C state machine, it handles the MSTOP interrupt and
 * 	 all relevant state transitions.
 *
 *
 * @param[in] i2cState
 *   Pointer to the STRUCT which handles the state machine
 *
 *
 ******************************************************************************/

void static i2c_MSTOP_fun(I2C_STATE_MACHINE *i2cState){
	switch(i2cState->STATE){
		case MStop:{
			sleep_unblock_mode(EM2);
			add_scheduled_event(i2cState->SI7021_Read_CB);
			i2cState->STATE = Call;
		break;
		}
		default:
			EFM_ASSERT(false);
			break;
	}
}


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Driver to open an set an I2C peripheral
 *
 * @details
 * 	 This routine is a low level driver.  The application code calls this function
 * 	 to open one of the I2C peripherals for communication as either a master or a slave.
 *
 * @note
 *   This function is normally called once to initialize the peripheral and the
 *   function I2C start
 *
 * @param[in] i2c
 *   Pointer to the base peripheral address of the I2C peripheral being opened
 *
 * @param[in] app_i2c_struct
 *   Is the STRUCT that the calling routine will use to set the parameters for I2C operation
 *
 ******************************************************************************/

void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *app_i2c_struct){


	//enabling the clock to whichever I2C peripheral has been selected
	if(i2c == I2C0){
			CMU_ClockEnable(cmuClock_I2C0, true);

		}
	else if(i2c == I2C1){
		CMU_ClockEnable(cmuClock_I2C1, true);

	}

	//verifying that the clock has been enabled correctly

	if ((i2c->IF & 0x01) == 0) {
		i2c->IFS = 0x01;
	EFM_ASSERT(i2c->IF & 0x01);
	i2c->IFC = 0x01;
	}
	else {
		i2c->IFC = 0x01;
	EFM_ASSERT(!(i2c->IF & 0x01));
	}


	//Initializing the i2cs init struct using the input struct

	I2C_Init_TypeDef i2c_init_values;

	i2c_init_values.enable = app_i2c_struct->enable;
	i2c_init_values.master = app_i2c_struct->master;
	i2c_init_values.refFreq = app_i2c_struct->refFreq;
	i2c_init_values.freq = app_i2c_struct->freq;
	i2c_init_values.clhr = app_i2c_struct->clhr;

	I2C_Init(i2c, &i2c_init_values);

	//defining and enable output routes
	i2c->ROUTELOC0 = (app_i2c_struct->out_pin_SCL << 8) | app_i2c_struct->out_pin_SDA;
	i2c->ROUTEPEN = (app_i2c_struct->out_pin_SCL_en << 1) | app_i2c_struct->out_pin_SDA_en;

	i2c_bus_reset(i2c);
	//interrupts
	I2C_IntClear(i2c, I2C_IF_ACK |  I2C_IF_NACK | I2C_IF_MSTOP | I2C_IF_SSTOP |I2C_IF_RXDATAV);
	I2C_IntEnable(i2c, I2C_IF_ACK |  I2C_IF_NACK | I2C_IF_MSTOP | I2C_IF_SSTOP | I2C_IF_RXDATAV);

	if(i2c == I2C0){
		NVIC_EnableIRQ(I2C0_IRQn);
	}
	else if(i2c == I2C1){
		NVIC_EnableIRQ(I2C1_IRQn);
	}


}

/***************************************************************************//**
 * @brief
 *   Driver to Start an I2C peripheral
 *
 * @details
 * 	 This routine is a low level driver.  The application code calls this function
 * 	 to start one of the I2C peripherals
 *
 *
 * @param[in] i2c
 *   Pointer to the base peripheral address of the i2c peripheral being opened
 *
 * @param[in] data
 *   pointer the variable where the data being taken from i2c rx buffer will be stored, if
 *   I2C_Start is being used for a write, this parameter can be set to false.
 *
 *@param[in] address
 *	address of the slave module
 *
 *
 * @param[in] I2C_CB
 *   Callback variable, used for the scheduler
 *
 *@param[in] Command
 *   Specific command to be sent to the slave
 *
 * @param[in] writeData
 *   If being used for a write, data to be written will be sent in via this parameter.
 *   If being used for a read, this can be set to false.
 ******************************************************************************/



void I2C_Start(I2C_TypeDef *i2c, uint32_t *data, uint32_t address, uint32_t I2C_CB, uint32_t Command, uint16_t writeData ){


	EFM_ASSERT((i2c->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE); // X = the I2C peripheral #

	sleep_block_mode(EM2);


	if(i2c == I2C0){
		i2c0_State.STATE = Call;
		i2c0_State.slaveAddress = address;
		i2c0_State.i2c = i2c;
		i2c0_State.Command = Command;
		i2c0_State.Data = data;
		i2c0_State.SI7021_Read_CB = I2C_CB;
		i2c0_State.writeData = writeData;
	}

	else if(i2c == I2C1){
		i2c1_State.STATE = Call;
		i2c1_State.slaveAddress = address;
		i2c1_State.i2c = i2c;
		i2c1_State.Command = Command;
		i2c1_State.Data = data;
		i2c1_State.SI7021_Read_CB = I2C_CB;
		i2c1_State.writeData = writeData;
	}


	i2c->CMD        = I2C_CMD_START;       //Start CMD
	i2c->TXDATA     = (address<<1)|(false); //Loading address write
}

	/***************************************************************************//**
	 * @brief
	 *   ISR for I2C0
	 *
	 *
	 ******************************************************************************/

void I2C0_IRQHandler(void){
	uint32_t int_flag;
	int_flag = I2C0->IF & I2C0->IEN;
	I2C0->IFC = int_flag;

	if (int_flag & I2C_IF_ACK){
		i2c_ACK_fun(&i2c0_State);
	}
	if (int_flag & I2C_IF_NACK ){
		i2c_NACK_fun(&i2c0_State);
	}
	if (int_flag & I2C_IF_RXDATAV){
		i2c_RXDATAV_fun(&i2c0_State);
	}
	if (int_flag & I2C_IF_MSTOP){
		i2c_MSTOP_fun(&i2c0_State);
	}

}

/***************************************************************************//**
 * @brief
 *   ISR for I2C1
 *
 *
 ******************************************************************************/


void I2C1_IRQHandler(void){
uint32_t int_flag;
int_flag = I2C1->IF & I2C1->IEN;
I2C1->IFC = int_flag;

	if (int_flag & I2C_IF_ACK){
		i2c_ACK_fun(&i2c1_State);
	}
	if (int_flag & I2C_IF_NACK ){
		i2c_NACK_fun(&i2c1_State);
	}
	if (int_flag & I2C_IF_RXDATAV){
		i2c_RXDATAV_fun(&i2c1_State);
	}
	if (int_flag & I2C_IF_MSTOP){
		i2c_MSTOP_fun(&i2c1_State);
	}

}


