/******************************************************************************
 *  Filename:       sensor_tmp007.c
 *  Revised:
 *  Revision:
 *
 *  Description:    Driver for the TI TMP06 infrared thermophile sensor.
 *
 *  Copyright (C) 2014 - 2015 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/* -----------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------
 */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "BoardSupport/inc/tmp007.h"
#include "BoardSupport/inc/I2CDriver.h"
#include "inc/hw_memmap.h"


/* -----------------------------------------------------------------------------
 *                                           Constants
 * ------------------------------------------------------------------------------
 */

/* Slave address */
#define TMP007_I2C_ADDRESS              0x40

/* TMP006 register addresses */
#define TMP007_REG_ADDR_VOLTAGE         0x00
#define TMP007_REG_ADDR_LOCAL_TEMP      0x01
#define TMP007_REG_ADDR_CONFIG          0x02
#define TMP007_REG_ADDR_OBJ_TEMP        0x03
#define TMP007_REG_ADDR_STATUS          0x04
#define TMP007_REG_ADDR_STATUS_ENABLE   0x05
#define TMP007_REG_ADDR_TC0_COEFFICIENT 0x11
#define TMP007_REG_ADDR_TC1_COEFFICIENT 0x12
#define TMP007_REG_PROD_ID              0x1F

/* TMP006 register values */
#define TMP007_VAL_CONFIG_ON            0x1040  // Sensor on state (1 sec) with Output Filtering On
#define TMP007_VAL_CONFIG_OFF           0x0000  // Sensor off state
#define TMP007_VAL_CONFIG_RESET         0x8000
#define TMP007_VAL_PROD_ID              0x0078  // Product ID
#define TMP007_VAL_TC0					0x0000  // TC0 Coefficient Register
#define TMP007_VAL_TC1					0x0000  // TC1 Coefficient Register

#define TMP007_VAL_STATUS_MASK_ALERT    0x8000  // Enable Alert Pin
#define TMP007_VAL_STATUS_MASK_TEMP     0x4000  // Enable Temperature Conversion Ready Enable

/* Bit values */
#define CONV_RDY_BIT                    0x4000  // Conversion ready 

/* Register length */
#define REGISTER_LENGTH                 2

/* Sensor data size */
#define DATA_SIZE                       4

/* Byte swap of 16-bit register value */
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)
#define SWAP(v) ( (LO_UINT16(v) << 8) | HI_UINT16(v) )

/* -----------------------------------------------------------------------------
 *                                           Local Functions
 * ------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------
 */
static uint16_t val;
/* -----------------------------------------------------------------------------
 *                                           Public functions
 * ------------------------------------------------------------------------------
 */


/*******************************************************************************
 * @fn          sensorTmp007Init
 *
 * @brief       Initialize the temperature sensor driver
 *
 * @return      none
 ******************************************************************************/
void sensorTmp007Init(void)
{
	// Configure sensor 
	//return (sensorTmp007Enable(false));
}


/*******************************************************************************
 * @fn          sensorTmp007Enable
 *
 * @brief       Turn the sensor on/off
 *
 * @return      none
 ******************************************************************************/
void sensorTmp007Enable(bool enable)
{
	if (enable)
	{
		val = TMP007_VAL_CONFIG_ON;
	}
	else
	{
		val = TMP007_VAL_CONFIG_OFF;
	}

	val = SWAP(val);

    SetSlaveAddress(I2C1_BASE, TMP007_I2C_ADDRESS);
    StartTransmission(I2C1_BASE, TMP007_REG_ADDR_CONFIG);
    ContinueTransmission(I2C1_BASE, val);
    ContinueTransmission(I2C1_BASE, val >> 8);
    EndTransmission(I2C1_BASE);

    SetSlaveAddress(I2C1_BASE, TMP007_I2C_ADDRESS);
    StartTransmission(I2C1_BASE, TMP007_REG_ADDR_TC0_COEFFICIENT);
    ContinueTransmission(I2C1_BASE, TMP007_VAL_TC0);
    ContinueTransmission(I2C1_BASE, TMP007_VAL_TC0 >> 8);
    EndTransmission(I2C1_BASE);

    SetSlaveAddress(I2C1_BASE, TMP007_I2C_ADDRESS);
    StartTransmission(I2C1_BASE, TMP007_REG_ADDR_TC1_COEFFICIENT);
    ContinueTransmission(I2C1_BASE, TMP007_VAL_TC1);
    ContinueTransmission(I2C1_BASE, TMP007_VAL_TC1 >> 8);
    EndTransmission(I2C1_BASE);

}

/*******************************************************************************
 * @fn          sensorTmp007EnableInterruptConversion
 *
 * @brief       Enable the alert pin to signal the interrupt conversion
 *
 * @return      none
 ******************************************************************************/
//bool sensorTmp007EnableInterruptConversion(bool enable)
//{
//	bool success;
//
//	if (enable)
//	{
//		val = TMP007_VAL_STATUS_MASK_ALERT | TMP007_VAL_STATUS_MASK_TEMP;
//	}
//	else
//	{
//		val = TMP007_VAL_CONFIG_OFF;
//	}
//
//	val = SWAP(val);
//	success = writeI2C(TMP007_I2C_ADDRESS, TMP007_REG_ADDR_STATUS_ENABLE, (uint8_t*)&val, REGISTER_LENGTH);
//
//	return (success);
//}

/*******************************************************************************
 * @fn          sensorTmp007Read
 *
 * @brief       Read the sensor voltage and sensor temperature registers
 *
 * @param       rawtTemp - temperature in 16 bit format
 *
 * @param       rawtObjTemp - object temperature in 16 bit format
 *
 * @return      TRUE if valid data
 ******************************************************************************/
uint16_t sensorTmp007Read(uint8_t registerAddress, bool reading)
{
    uint16_t success;

    SetSlaveAddress(I2C1_BASE, TMP007_I2C_ADDRESS);
    SignalMasterReceive(I2C1_BASE, registerAddress, TMP007_I2C_ADDRESS);

    uint8_t msb = StartReceive(I2C1_BASE);
    uint8_t lsb = EndReceive(I2C1_BASE);
    success = (msb << 8) | lsb;

	if (success)
	{
		val = SWAP(val);
		success = val & CONV_RDY_BIT;
	}

	uint16_t localTemperature;
	uint16_t objectTemperature;
	uint8_t msbRaw, lsbRaw;
	uint8_t msbObject, lsbObject;


	if (success)
	{
		// Read the sensor registers
	    SetSlaveAddress(I2C1_BASE, TMP007_I2C_ADDRESS);
	    SignalMasterReceive(I2C1_BASE, registerAddress, TMP007_I2C_ADDRESS);

	    msbRaw = StartReceive(I2C1_BASE);
	    lsbRaw = EndReceive(I2C1_BASE);

        SetSlaveAddress(I2C1_BASE, TMP007_I2C_ADDRESS);
        SignalMasterReceive(I2C1_BASE, registerAddress, TMP007_I2C_ADDRESS);

        msbObject = StartReceive(I2C1_BASE);
        lsbObject = EndReceive(I2C1_BASE);
	}

	if (!success)
	{
		//	  sensorSetErrorData
	}

	// Swap bytes
	localTemperature = (msbRaw << 6) | (lsbRaw >> 2);
	objectTemperature = (msbObject << 6) | (lsbObject >> 2);

	if (msbRaw & 0x80)
	{
		localTemperature |= 0xF000;
	}
	if (msbObject & 0x80)
	{
		objectTemperature |= 0xF000;
	}

	if (reading == RAW)
	{
	    return localTemperature;
	}
	if (reading == OBJECT)
	{
	    return objectTemperature;
	}

	return 0;
}

/*******************************************************************************
 * @fn          sensorTmp007Test
 *
 * @brief       Run a sensor self-test
 *
 * @return      TRUE if passed, FALSE if failed
 ******************************************************************************/
bool sensorTmp007Test(void)
{
	// Check product ID
    SetSlaveAddress(I2C1_BASE, TMP007_I2C_ADDRESS);
    SignalMasterReceive(I2C1_BASE, TMP007_REG_PROD_ID, TMP007_I2C_ADDRESS);

    uint8_t msb = StartReceive(I2C1_BASE);
    uint8_t lsb = EndReceive(I2C1_BASE);
    uint16_t data = (msb << 8) | lsb;
	
	if (data != TMP007_VAL_PROD_ID)
	{
		return (false);
	}
	// Turn sensor on
	sensorTmp007Enable(true);

	// Check config register (on)
    SetSlaveAddress(I2C1_BASE, TMP007_I2C_ADDRESS);
    SignalMasterReceive(I2C1_BASE, TMP007_REG_ADDR_CONFIG, TMP007_I2C_ADDRESS);

    msb = StartReceive(I2C1_BASE);
    lsb = EndReceive(I2C1_BASE);
    data = (msb << 8) | lsb;

    if (data != TMP007_VAL_CONFIG_ON)
    {
        return (false);
    }

	// Turn sensor off
	sensorTmp007Enable(false);

	// Check config register (off)
    SetSlaveAddress(I2C1_BASE, TMP007_I2C_ADDRESS);
    SignalMasterReceive(I2C1_BASE, TMP007_REG_ADDR_CONFIG, TMP007_I2C_ADDRESS);

    msb = StartReceive(I2C1_BASE);
    lsb = EndReceive(I2C1_BASE);
    data = (msb << 8) | lsb;

    if (data != TMP007_VAL_CONFIG_OFF)
    {
        return (false);
    }

	return (true);
}

#ifdef USE_FPU

/*******************************************************************************
 * @fn          sensorTmp007Convert
 *
 * @brief       Convert raw data to object and ambience temperature
 *
 * @param       rawTemp - raw temperature from sensor
 *
 * @param       rawObjTemp - raw temperature from sensor
 *
 * @param       tObj - converted object temperature
 *
 * @param       tAmb - converted ambience temperature
 *
 * @return      none
 ******************************************************************************/
void sensorTmp007Convert(uint16_t rawTemp, uint16_t rawObjTemp, float *tObj, 
		float *tAmb)
{
	const float SCALE_LSB = 0.03125;
//	float t;
//	int it;

//	it = (int)((rawObjTemp) >> 2);
//	t = ((float)(it)) * SCALE_LSB;
	*tObj = ((float)(rawObjTemp)*SCALE_LSB);

	*tAmb = ((float)(rawTemp)*SCALE_LSB);
}

#endif /* USE_FPU */
