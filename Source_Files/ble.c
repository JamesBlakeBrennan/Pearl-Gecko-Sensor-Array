/**
 * @file ble.c
 * @author James Brennan
 * @date 4/6/2021
 * @brief Contains all the functions to interface the application with the HM-18
 *   BLE module and the LEUART driver
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************
#include "ble.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************

static CIRC_TEST_STRUCT test_struct;

static BLE_CIRCULAR_BUF ble_cbuf;

/***************************************************************************//**
 * @brief BLE module
 * @details
 *  This module contains all the functions to interface the application layer
 *  with the HM-18 Bluetooth module.  The application does not have the
 *  responsibility of knowing the physical resources required, how to
 *  configure, or interface to the Bluetooth resource including the LEUART
 *  driver that communicates with the HM-18 BLE module.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************
static void update_circ_wrtindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by);
static void update_circ_readindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by);
static uint8_t ble_circ_space(void);






/***************************************************************************//**
 * @brief
 *  This function initializes the circular buffer
 *
 * @details
 * 	The function initializes the static circular buffer struct
 *
 *
 ******************************************************************************/

static void ble_circ_init(void){


	ble_cbuf.size = CSIZE;
	ble_cbuf.size_mask = CSIZE-1;
	ble_cbuf.read_ptr = 0;
	ble_cbuf.write_ptr = 0;



 }


/***************************************************************************//**
 * @brief
 *  This function pushes a string onto the buffer
 *
 * @details
 * 	The function first checks whether the buffer is full before attempting to push
 * 	a packet onto it
 *
 * @param[in] string
 *   The string to be pushed onto the buffer
 *
 ******************************************************************************/


 static void ble_circ_push(char *string){

	 //size of input string
		 	 uint8_t header = strlen(string);
	 	 //checking space in the buffer
	 	 uint8_t space = ble_circ_space();
	 	 if(header >= space){
	 		 EFM_ASSERT(false);
	 	 }


	 	 else{
	 		ble_cbuf.cbuf[ble_cbuf.write_ptr] = header;
	 		update_circ_wrtindex(&ble_cbuf, 1);

	 	 for(int i = 0; i<header; i++){
	 		 ble_cbuf.cbuf[ble_cbuf.write_ptr] = string[i];
	 		 update_circ_wrtindex(&ble_cbuf, 1);
	 	 }

	 	 }

 }

 /***************************************************************************//**
   * @brief
   *  ble_circ_space will return the space available on the buffer
   *
   * @details
   * 	 This function is used by the various circular buffer functions to check
   * 	 on the space available in the buffer
   *
   * @param[in] update_by
   *   the length of the string being pushed
   *
   ******************************************************************************/

 static uint8_t ble_circ_space(void){

	 uint8_t space;
	 space = CSIZE - ((ble_cbuf.write_ptr - ble_cbuf.read_ptr) &(ble_cbuf.size - 1));
	 return space;

 }

 /***************************************************************************//**
   * @brief
   *  update_circ_wrtindex will update the write index
   *
   * @details
   * 	 This function is called by the ble_circ_push function in order to update the write
   * 	 pointer.
   *
   * @param[in] index_struct
   *   The struct being used to handle the circular buffer
   *
   * @param[in] update_by
   *   the length of the string being pushed
   *
   ******************************************************************************/

 static void update_circ_wrtindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by){
	 index_struct->write_ptr = (index_struct->write_ptr + update_by)%index_struct->size;
 }

 /***************************************************************************//**
   * @brief
   *  update_circ_readindex will update the read index
   *
   * @details
   * 	 This function is called by the ble_circ_pop function in order to update the read
   * 	 pointer.
   *
   * @param[in] index_struct
   *   The struct being used to handle the circular buffer
   *
   * @param[in] update_by
   *   the length of the string being popped
   *
   ******************************************************************************/

 static void update_circ_readindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by){
	 index_struct->read_ptr = (index_struct->read_ptr + update_by)%index_struct->size;
 }


 /***************************************************************************//**
  * @brief
  *  ble circ pop will pop a string off the circular buffer and send it to the ble
  *
  * @details
  * 	 This function first checks whether the LEUART TX is busy, if so it returns.
  * 	 If not, it then checks to see if there are any  strings to be popped off
  * 	 the circular buffer. If the circular buffer is empty, it returns. If there
  * 	 is a string to be popped, it does so and then sends the string to the
  * 	 leuart state machine to be written.
  *
  * @param[in] test
  *   test is set to true if this function is being used for the circular buffer test,
  *   otherwise it should be set to false.
  *
  ******************************************************************************/

 bool ble_circ_pop(bool test){

	 if(leuart_tx_busy(LEUART0)){
		 return true;
	 }

	 uint8_t space = ble_circ_space();
		 	 if(CSIZE == space){
		 		 return true;
		 	 }


	 //checks if above if statement failed, not very
	 EFM_ASSERT((LEUART0->STATUS) || _LEUART_STATUS_TXIDLE_MASK  );

	 //read header, check size
		uint8_t header  = ble_cbuf.cbuf[ble_cbuf.read_ptr];
	 //update read index
	update_circ_readindex(&ble_cbuf, 1);

	 //memset temp buffer
	char temp_buff[CSIZE-1];
	memset(temp_buff, 0, ble_cbuf.size_mask);
	memset(test_struct.result_str, 0, ble_cbuf.size_mask);
	 //for loop
	 for(int i = 0; i< header; i++){
		 temp_buff[i] = ble_cbuf.cbuf[ble_cbuf.read_ptr] ;
	 		 update_circ_readindex(&ble_cbuf, 1);
	 	 }
	 temp_buff[header] = '\0';
	 if(test == CIRC_TEST){
		 for(int i = 0; i<header; i++){
			 test_struct.result_str[i] = temp_buff[i];
		 }
	 }
	 else{

		 leuart_start(LEUART0, temp_buff, header);
	 }
	 return false;
 }


/***************************************************************************//**
 * @brief
 * driver to open ble functionality
 * @details
 * 	 This routine is initializes the leuart struct and calls leuart open
 *
 *@note
 *   need only be called once on startup
 *
 ******************************************************************************/

void ble_open(void){//uint32_t tx_event, uint32_t rx_event
		LEUART_OPEN_STRUCT leuart_open_struct;

		leuart_open_struct.baudrate = HM10_BAUDRATE;

		leuart_open_struct.databits = HM10_DATABITS;

		leuart_open_struct.enable = HM10_ENABLE;

		leuart_open_struct.parity = HM10_PARITY;

		leuart_open_struct.stopbits = HM10_STOPBITS;

		leuart_open_struct.refFreq = HM10_REFFREQ;

		 //setting up and enabling the output pins
		//leuart_open_struct.sfubrx = SDA_route;
		//leuart_open_struct.startframe = SCL_route;
		//leuart_open_struct.startframe_en = true;
		//leuart_open_struct.sigframe = true;
		//leuart_open_struct.sigframe_en = true;
		leuart_open_struct.rx_loc = LEUART0_RX_ROUTE;
		leuart_open_struct.rx_pin_en = true;
		leuart_open_struct.tx_loc = LEUART0_TX_ROUTE;
		leuart_open_struct.tx_pin_en = true;
		leuart_open_struct.rx_en = true;
		leuart_open_struct.tx_en = true;
		//leuart_open_struct.rx_done_evt = true;
		//leuart_open_struct.tx_done_evt; = true;

		ble_circ_init();


		leuart_open(LEUART0, &leuart_open_struct);


}


/***************************************************************************//**
 * @brief
 *
 * This routine initiates a write to the bluetooth module
 *
 * @details
 * this function must be called anytime a write is desired, it starts the leuart state machine
 *
 ******************************************************************************/


void ble_write(char* string){

	//uint32_t string_len = strlen(string);
	//leuart_start(LEUART0, string, string_len);
	ble_circ_push(string);
	ble_circ_pop(CIRC_OPER);
	//add_scheduled_event(BLE_TX_DONE_CB);

}

/***************************************************************************//**
 * @brief
 *   BLE Test performs two functions.  First, it is a Test Driven Development
 *   routine to verify that the LEUART is correctly configured to communicate
 *   with the BLE HM-18 module.  Second, the input argument passed to this
 *   function will be written into the BLE module and become the new name
 *   advertised by the module while it is looking to pair.
 *
 * @details
 * 	 This global function will use polling functions provided by the LEUART
 * 	 driver for both transmit and receive to validate communications with
 * 	 the HM-18 BLE module.  For the assignment, the communication with the
 * 	 BLE module must use low energy design principles of being an interrupt
 * 	 driven state machine.
 *
 * @note
 *   For this test to run to completion, the phone most not be paired with
 *   the BLE module.  In addition for the name to be stored into the module
 *   a breakpoint must be placed at the end of the test routine and stopped
 *   at this breakpoint while in the debugger for a minimum of 5 seconds.
 *
 * @param[in] *mod_name
 *   The name that will be written to the HM-18 BLE module to identify it
 *   while it is advertising over Bluetooth Low Energy.
 *
 * @return
 *   Returns bool true if successfully passed through the tests in this
 *   function.
 ******************************************************************************/

bool ble_test(char *mod_name){
	uint32_t	str_len;

	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	// This test will limit the test to the proper setup of the LEUART
	// peripheral, routing of the signals to the proper pins, pin
	// configuration, and transmit/reception verification.  The test
	// will communicate with the BLE module using polling routines
	// instead of interrupts.
	// How is polling different than using interrupts?
	// ANSWER: Polling is constantly searching for an input, consuming cpu resources,
	//         while an interrupt comes from outside the cpu and forces the cpu to pause
	//         and handle it.
	// How does interrupts benefit the system for low energy operation?
	// ANSWER: The cpu does not have to spend resources to wait for the input, this itself
	//         saves power but also means the cpu can enter lower energy modes
	// How does interrupts benefit the system that has multiple tasks?
	// ANSWER: The cpu can focus on other tasks instead of spending time polling.

	// First, you will need to review the DSD HM10 datasheet to determine
	// what the default strings to write data to the BLE module and the
	// expected return statement from the BLE module to test / verify the
	// correct response

	// The test_str is used to tell the BLE module to end a Bluetooth connection
	// such as with your phone.  The ok_str is the result sent from the BLE module
	// to the micro-controller if there was not active BLE connection at the time
	// the break command was sent to the BLE module.
	// Replace the test_str "" with the command to break or end a BLE connection
	// Replace the ok_str "" with the result that will be returned from the BLE
	//   module if there was no BLE connection
	char		test_str[80] = "AT";
	char		ok_str[80] = "OK";


	// output_str will be the string that will program a name to the BLE module.
	// From the DSD HM10 datasheet, what is the command to program a name into
	// the BLE module?
	// The  output_str will be a string concatenation of the DSD HM10 command
	// and the input argument sent to the ble_test() function
	// Replace the output_str "" with the command to change the program name
	// Replace the result_str "" with the first part of the expected result
	//  the backend of the expected response will be concatenated with the
	//  input argument
	char		output_str[80] = "AT+NAME";
	char		result_str[80] = "OK+Set:";


	// To program the name into your module, you must reset the module after you
	// have sent the command to update the modules name.  What is the DSD HM10
	// name to reset the module?
	// Replace the reset_str "" with the command to reset the module
	// Replace the reset_result_str "" with the expected BLE module response to
	//  to the reset command
	char		reset_str[80] = "AT+RESET";
	char		reset_result_str[80] = "OK+RESET";
	char		return_str[80];

	bool		success;
	bool		rx_disabled, rx_en, tx_en;
	uint32_t	status;

	// These are the routines that will build up the entire command and response
	// of programming the name into the BLE module.  Concatenating the command or
	// response with the input argument name
	strcat(output_str, mod_name);
	strcat(result_str, mod_name);

	// The test routine must not alter the function of the configuration of the
	// LEUART driver, but requires certain functionality to insure the logical test
	// of writing and reading to the DSD HM10 module.  The following c-lines of code
	// save the current state of the LEUART driver that will be used later to
	// re-instate the LEUART configuration

	status = leuart_status(HM10_LEUART0);
	if (status & LEUART_STATUS_RXBLOCK) {
		rx_disabled = true;
		// Enabling, unblocking, the receiving of data from the LEUART RX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKDIS);
	}
	else rx_disabled = false;
	if (status & LEUART_STATUS_RXENS) {
		rx_en = true;
	} else {
		rx_en = false;
		// Enabling the receiving of data from the RX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_RXENS));
	}

	if (status & LEUART_STATUS_TXENS){
		tx_en = true;
	} else {
		// Enabling the transmission of data to the TX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_TXENS));
		tx_en = false;
	}
	leuart_cmd_write(HM10_LEUART0, (LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX));

	// This sequence of instructions is sending the break ble connection
	// to the DSD HM10 module.
	// Why is this command required if you want to change the name of the
	// DSD HM10 module?
	// ANSWER:
	str_len = strlen(test_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, test_str[i]);
	}

	// What will the ble module response back to this command if there is
	// a current ble connection?
	// ANSWER:
	str_len = strlen(ok_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (ok_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// This sequence of code will be writing or programming the name of
	// the module to the DSD HM10
	str_len = strlen(output_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, output_str[i]);
	}

	// Here will be the check on the response back from the DSD HM10 on the
	// programming of its name
	str_len = strlen(result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// It is now time to send the command to RESET the DSD HM10 module
	str_len = strlen(reset_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, reset_str[i]);
	}

	// After sending the command to RESET, the DSD HM10 will send a response
	// back to the micro-controller
	str_len = strlen(reset_result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (reset_result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// After the test and programming have been completed, the original
	// state of the LEUART must be restored
	if (!rx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXDIS);
	if (rx_disabled) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKEN);
	if (!tx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXDIS);
	leuart_if_reset(HM10_LEUART0);

	success = true;


	CORE_EXIT_CRITICAL();
	return success;
}

/***************************************************************************//**
 * @brief
 *   Circular Buff Test is a Test Driven Development function to validate
 *   that the circular buffer implementation
 *
 * @details
 * 	 This Test Driven Development test has tests integrated into the function
 * 	 to validate that the routines can successfully identify whether there
 * 	 is space available in the circular buffer, the write and index pointers
 * 	 wrap around, and that one or more packets can be pushed and popped from
 * 	 the circular buffer.
 *
 * @note
 *   If anyone of these test will fail, an EFM_ASSERT will occur.  If the
 *   DEBUG_EFM=1 symbol is defined for this project, exiting this function
 *   confirms that the push, pop, and the associated utility functions are
 *   working.
 *
 * @par
 *   There is a test escape that is not possible to test through this
 *   function that will need to be verified by writing several ble_write()s
 *   back to back and verified by checking that these ble_write()s were
 *   successfully transmitted to the phone app.
 *
 ******************************************************************************/

 void circular_buff_test(void){
	 bool buff_empty;
	 int test1_len = 50;
	 int test2_len = 25;
	 int test3_len = 5;

	 // Why this 0 initialize of read and write pointer?
	 // Student Response: When the buffer is empty the read and write pointer should both
	 // 				  be set to 0. This normally happens when the buffer is initialized.
	 //
	 ble_cbuf.read_ptr = 0;
	 ble_cbuf.write_ptr = 0;

	 // Why do none of these test strings contain a 0?
	 // Student Response: That could indicate the end of a string
	 //
	 for (int i = 0;i < test1_len; i++){
		 test_struct.test_str[0][i] = i+1;
	 }
	 test_struct.test_str[0][test1_len] = 0;

	 for (int i = 0;i < test2_len; i++){
		 test_struct.test_str[1][i] = i + 20;
	 }
	 test_struct.test_str[1][test2_len] = 0;

	 for (int i = 0;i < test3_len; i++){
		 test_struct.test_str[2][i] = i +  35;
	 }
	 test_struct.test_str[2][test3_len] = 0;

	 // What is this test validating?
	 // Student response: The buffer is empty so ble_circ_space should
	 //                   return the maximum size of the buffer.

	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // Why is there only one push to the circular buffer at this stage of the test
	 // Student Response: to make sure that the buffer is properly acting as a FIFO
	 //  				  and that one string can be pushed then popped
	 //
	 ble_circ_push(&test_struct.test_str[0][0]);

	 // What is this test validating?
	 // Student response: That the space has updated properly, based on the last string
	 EFM_ASSERT(ble_circ_space() == (CSIZE - test1_len - 1));

	 // Why is the expected buff_empty test = false?
	 // Student Response: The buffer should not be empty, so ble_circ_pop should return false
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test1_len; i++){
		 EFM_ASSERT(test_struct.test_str[0][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response: That the length of popped string is equal to the length of the pushed string
	 EFM_ASSERT(strlen(test_struct.result_str) == test1_len);

	 // What is this test validating?
	 // Student response: That the buffer is empty
	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // What does this next push on the circular buffer test?
	 // Student Response: That the push function is working correctly and that csize updates properly

	 ble_circ_push(&test_struct.test_str[1][0]);


	 EFM_ASSERT(ble_circ_space() == (CSIZE - test2_len - 1));

	 // What does this next push on the circular buffer test?
	 // Student Response: That two strings can be pushed onto the buffer and that csize
	 //	 	 	 	 	  returns the correct size
	 ble_circ_push(&test_struct.test_str[2][0]);


	 EFM_ASSERT(ble_circ_space() == (CSIZE - test2_len - 1 - test3_len - 1));

	 // What does this next push on the circular buffer test?
	 // Student Response: It tests the write pointer and read pointer wrap around properly and that
	 //	 	 	 	 	  the buffer is not overflown.
	 EFM_ASSERT(abs(ble_cbuf.write_ptr - ble_cbuf.read_ptr) < CSIZE);

	 // Why is the expected buff_empty test = false?
	 // Student Response: The buffer is not empty so pop should return false
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test2_len; i++){
		 EFM_ASSERT(test_struct.test_str[1][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response: that a string can be popped properly and that the csize updates
	 EFM_ASSERT(strlen(test_struct.result_str) == test2_len);

	 EFM_ASSERT(ble_circ_space() == (CSIZE - test3_len - 1));

	 // Why is the expected buff_empty test = false?
	 // Student Response: the buffer is not empty
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test3_len; i++){
		 EFM_ASSERT(test_struct.test_str[2][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response: That the pop worked correctly, and that the buffer is now empty
	 EFM_ASSERT(strlen(test_struct.result_str) == test3_len);

	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // Using these three writes and pops to the circular buffer, what other test
	 // could we develop to better test out the circular buffer?
	 // Student Response: We could attempt to overflow the buffer and check if that has
	 //                   been accounted for. We could also check that the strings actually
	 	 	 	 	 //   match instead of just the length.


	 // Why is the expected buff_empty test = true?
	 // Student Response: the buffer is empty
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == true);
	 ble_write("\nPassed Circular Buffer Test\n");

 }






