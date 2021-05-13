# Pearl-Gecko-Sensor-Array
This repository includes various sensor and communication drivers for the Silicon Labs Pearl Gecko. They have been integrated into a complete project which includes a scheduler to allow them to function together. 

The project includes:

Driver for the Si7021 humidity sensor  
Driver for the VEML603 ambient light sensor  
I2C driver for the sensors written as an interupt driven state machine  
Driver for the HM-19 bluetooth module    
UART driver for the bluetooth module  
A circular buffer to hold strings waiting to be sent to the bluetooth module  
Scheduler  

The application code grabs readings off these sensors and sends them to the bluetooth module. Output can be read using a bluetooth terminal app. The project has been designed with low energy design principles in mind.

