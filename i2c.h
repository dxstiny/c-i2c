/* 
 * File:   i2c.h
 * Author: David Dias
 *
 * Created on 30. Januar 2020, 11:12
 * 
 * Use this library for any I2C & XC8 usage
 * 
 * ** FUNCTIONS **
 * 
 * § = Function is Necessary
 * 
 * § Master Init will initialise I2C for you, unless you have special requests, 
 * you can simply call the function from the main function while initializing
 * you'll need to give the function the I2C Clock speed, i.e. 100kHz
 * 
 * § Master CallSlave will, as the name suggests, call a slave for you. 
 * it corresponds to the function call Write([Address] + [Read/Write])
 * you give the function address and rw mode
 * 
 * Master CallSlavePlus executes a combination of Start() and CallSlave()
 * this means that you can directly call this function without calling Start() before
 * 
 * § Master Wait waits until all data has been sent/received.
 * Although this function is really important, you do not need to use it yourself.
 * Any call to this function that is actually needed comes from other functions in this file
 * 
 * § Master Start starts the conversion
 * 
 * Master Restart can be easily replaced by Stop() and Start()
 * 
 * § Master Stop stops the conversion
 * 
 * Master sendACK sends only the Acknowledge Bit 
 * (useful for a custom read function, is already included in all read functions!)
 * 
 * Master sendNAK sends only the Not Acknowledge Bit 
 * (useful for a custom read function, is already included in all read functions!)
 * 
 * § Master Write writes data to I2C
 * data could be address and rw-mode (CallSlave), register selection or data to write to a register
 * 
 * Master WritePlus executes a combination of Write() and Stop()
 * 
 * Master WriteComplete executes a combination of CallSlavePlus(), multiple Write() commands and Stop()
 * 
 * § Master Read reads data from I2C
 * data could be the result of a conversion or measurement
 * 
 * Master ReadPlus executes a combination of Read() and Stop()
 * 
 * Master ReadComplete executes a combination of CallSlavePlus, multiple Read() commands and Stop()
 * note that the results will be stored in the array parameter (where ACK/NAK are set)
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

// *** GENERAL ***

/**
 * @brief               Initialize I2C as Master
 * @param clockspeed    clockspeed
 */
void I2C_Master_Init(const unsigned long);

// *** COORDINATION ***

/**
 * @brief           calls the slave, must be executed before write or read functions
 * @param address   address of slave
 * @param rw        1 = read, 0 = write
 */
void I2C_Master_CallSlave(unsigned char, char);

/**
 * @brief           starts the conversion & calls the slave, must be executed before write or read functions
 * @param address   address of slave
 * @param rw        1 = read, 0 = write
 */
void I2C_Master_CallSlavePlus(unsigned char, char);

/**
 * @brief           wait for next operation
 */
void I2C_Master_Wait();

/**
 * @brief           start I2C communication
 */
void I2C_Master_Start();

/**
 * @brief           restart -> if start() without having stopped before
 */
void I2C_Master_Restart();

/**
 * @brief           Stop
 */
void I2C_Master_Stop();

/**
 * @brief           sends the acknowledge bit (ACK)
 */
void I2C_Master_sendACK(void);

/**
 * @brief           sends the not acknowledge bit (NACK/NAK)
 */
void I2C_Master_sendNAK(void);

// *** TRANSFER ***

/**
 * @brief           writes content to I2C
 * @param content   what to write (1st Time: address, 2nd Time: )
 */
void I2C_Master_Write(unsigned);

/**
 * @brief           writes content to I2C & stops the conversion
 * @param content   what to write (1st Time: address, 2nd Time: )
 */
void I2C_Master_WritePlus(unsigned);

/**
 * @brief                   starts the conversion, calls a slave, performs multiple write actions and stops the conversion
 * @param address           address of slave
 * @param content           char or char array, consisting of all arguments that have to be written
 * @param contentlength     length of the array "content": 1 per index, if content is a single char, write "1"
 */
void I2C_Master_WriteComplete(unsigned char, unsigned char*, char);

/**
 * @brief               reads from I2C
 * @param acknowledge   1 means not acknowledge and 0 means acknowledge. This value will be transmitted when we set the ACKEN bit (above). This bit has application only in master receive mode.
 * @return              returns the result that was read
 */
signed short I2C_Master_Read(unsigned short);

/**
 * @brief               reads from I2C & stops the conversion
 * @param acknowledge   1 means not acknowledge and 0 means acknowledge. This value will be transmitted when we set the ACKEN bit (above). This bit has application only in master receive mode.
 * @return              returns the result that was read
 */
signed short I2C_Master_ReadPlus(unsigned short);

/**
 * @brief                   starts the conversion, calls a slave, performs multiple write actions and stops the conversion
 * @param address           address of slave
 * @param byte              pointer to ACK / NAK Bits: READ RESULTS WILL BE STORED IN THIS ARRAY!!!!
 * @param contentlength     length of the array "content": 1 per index, if content is a single char, write "1"
 */
void I2C_Master_ReadComplete(unsigned char, unsigned char *, unsigned char);

#endif	/* I2C_H */

