#include <xc.h>
#include "i2c.h"
#include <stdio.h>

// *** Defines ***
#define _XTAL_FREQ 8000000      // Oscillator frequency PBA6: 8MHz (w/o PLL)

#define READ 1
#define WRITE 0

// *** GENERAL ***

/**
 * @brief               Initialize I2C as Master
 * @param clockspeed    clockspeed
 */
void I2C_Master_Init(const unsigned long clockspeed)
{
  SSPCON = 0b00101000;            //SSP Module as Master
  SSPCON2 = 0;
  SSPADD = (_XTAL_FREQ/(4*clockspeed))-1; //Setting Clock Speed
  SSPSTAT = 0;
  TRISC3 = 1;                   //Setting input as given in datasheet
  TRISC4 = 1;                   //Setting input as given in datasheet
}

// *** COORDINATION ***

/**
 * @brief           calls the slave, must be executed before write or read functions
 * @param address   address of slave
 * @param rw        1 = read, 0 = write
 */
void I2C_Master_CallSlave(unsigned char address, char rw)
{
    I2C_Master_Write(address + rw); // address + read
}

/**
 * @brief           starts the conversion & calls the slave, must be executed before write or read functions
 * @param address   address of slave
 * @param rw        1 = read, 0 = write
 */
void I2C_Master_CallSlavePlus(unsigned char address, char rw)
{
    I2C_Master_Start();
    I2C_Master_Write(address + rw); // address + read
}

/**
 * @brief           wait for next operation
 */
void I2C_Master_Wait()
{
  while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F)); //Transmit is in progress
}

/**
 * @brief           start I2C communication
 */
void I2C_Master_Start()
{
  I2C_Master_Wait();    
  SEN = 1;             //Initiate start condition
}

/**
 * @brief           restart -> if start() w/o having stopped before
 */
void I2C_Master_Restart()
{
  I2C_Master_Wait();
  RSEN = 1;           //Initiate repeated start condition
}

/**
 * @brief           Stop
 */
void I2C_Master_Stop()
{
  I2C_Master_Wait();
  PEN = 1;           //Initiate stop condition
}

/**
 * @brief           sends the acknowledge bit (ACK)
 */
void I2C_Master_sendACK(void)
{
    SSP1CON2bits.ACKDT = 0;
    SSP1CON2bits.ACKEN = 1; // start the ACK/NACK
}

/**
 * @brief           sends the not acknowledge bit (NACK/NAK)
 */
void I2C_Master_sendNAK(void)
{
    SSP1CON2bits.ACKDT = 1;
    SSP1CON2bits.ACKEN = 1; // start the ACK/NACK
}

// *** TRANSFER ***

/**
 * @brief           writes content to I2C
 * @param content   what to write (1st Time: address, 2nd Time: )
 */
void I2C_Master_Write(unsigned content)
{
  I2C_Master_Wait();
  SSPBUF = content;         //Write data to SSPBUF
}

/**
 * @brief           writes content to I2C & stops the conversion
 * @param content   what to write (1st Time: address, 2nd Time: )
 */
void I2C_Master_WritePlus(unsigned content)
{
    I2C_Master_Write(content);
    I2C_Master_Stop();
}

/**
 * @brief                   starts the conversion, calls a slave, performs multiple write actions and stops the conversion
 * @param address           address of slave
 * @param content           char or char array, consisting of all arguments that have to be written
 * @param contentlength     length of the array "content": 1 per index, if content is a single char, write "1"
 */
void I2C_Master_WriteComplete(unsigned char address, unsigned char* content, char contentlength)
{
    I2C_Master_CallSlavePlus(address, WRITE); // + write (0)

    for (int i = 0; i < contentlength; i++)
    {
        I2C_Master_Write(*content);
        content++;
    }
    
    I2C_Master_Stop();
}

/**
 * @brief               reads from I2C
 * @param acknowledge   1 means not acknowledge and 0 means acknowledge. This value will be transmitted when we set the ACKEN bit (above). This bit has application only in master receive mode.
 * @return              returns the result that was read
 */
signed short I2C_Master_Read(unsigned short acknowledge)
{
  unsigned short temp;
  I2C_Master_Wait();
  RCEN = 1;
  I2C_Master_Wait();
  temp = SSPBUF;                //Read data from SSPBUF
  I2C_Master_Wait();
  ACKDT = (acknowledge)?0:1;    //Acknowledge bit
  ACKEN = 1;                    //Acknowledge sequence
  return temp;
}

/**
 * @brief               reads from I2C & stops the conversion
 * @param acknowledge   1 means not acknowledge and 0 means acknowledge. This value will be transmitted when we set the ACKEN bit (above). This bit has application only in master receive mode.
 * @return              returns the result that was read
 */
signed short I2C_Master_ReadPlus(unsigned short acknowledge)
{
    signed short returnVal = I2C_Master_Read(acknowledge);
    I2C_Master_Stop();
    return returnVal;
}

/**
 * @brief               reads from I2C & stops the conversion
 * @param acknowledge   1 means not acknowledge and 0 means acknowledge. This value will be transmitted when we set the ACKEN bit (above). This bit has application only in master receive mode.
 * @return              returns the result that was read
 */

/**
 * @brief                   starts the conversion, calls a slave, performs multiple write actions and stops the conversion
 * @param address           address of slave
 * @param byte              pointer to ACK / NAK Bits: READ RESULTS WILL BE STORED IN THIS ARRAY!!!!
 * @param contentlength     length of the array "content": 1 per index, if content is a single char, write "1"
 */
void I2C_Master_ReadComplete(unsigned char address, unsigned char *byte, unsigned char contentlength)
{
    I2C_Master_WriteComplete(address, 0, 1); // set to address in write mode: register switch to temperature
    
    I2C_Master_CallSlavePlus(address, READ); // start + set address @ read (1)
    
    for (int i = 0; i < contentlength; i++)
    {
        *byte = I2C_Master_Read(*byte);
        byte++; // pointer to next index
    }
    
    I2C_Master_Stop();
}
