/*!
*   Component name: LIS2DH12
*
*   File name: LIS2DH12.c
*
* Description:
* Device driver for LIS2DH12 accelerometer.
* Provides a public interface for control of the LIS2DH12.
*
*  Copyright 2018 (c) DecaWave Ltd, Dublin, Ireland.
*
*  All rights reserved.
*
*  Author: Decawave, 2018
*/

#include <stdio.h>

#include "port_platform.h"
#include "lis2dh12.h"
#include "lis2dh12registers.h"

// Local variables and constants

// FIFO element
struct sFifoXYZelement
{
    uint16_t u16_X;
    uint16_t u16_Y;
    uint16_t u16_Z;
};

typedef struct sFifoXYZelement tsFifoElement;

// Storage for LIS2 FIFO samples
static tsFifoElement atsFifo[MAX_FIFO_SIZE];

// Seamphores for events
static volatile bool boInterruptEvent = false;

// Accelerometer mode - convert from left justified, signed
// by shifting readings
static uint8_t u8SignExtend;

// 8, 10, 12 bit resolution
enum AccelRES
{
    mode_8bit,
    mode_10bit,
    mode_12bit
};

// Currently selected resolution
enum AccelRES eRsolution;

// Local function declarations
static void vInterruptInit(void);
static void vThresholdConfigure(uint8_t u8Level, uint8_t u8Duration, uint8_t u8Mode);
static void vDumpFifo(uint8_t u8Num);

// Public interface functions

/*!
* @brief Initialise the LIS2DH12 Accelerometer
*
* Initialises the LIS2DH12 accelerometer into known state.
* After an MCU restart or Power-On-Reset, LIS2 may still be running,
* so first force a reboot to initialise registers and place device
* into power-down mode.
* Allow ample time for the boot process to complete - datasheet says
* reboot takes 'approximately' 5ms.
* Next, place device in low-power mode, high resolution, 1Hz sampling
* on a single axis.
*/
void vLIS2_Init (void)
{
    // If this function is called after power on, then the LIS2DH12
    // will still be in boot mode, allow time for boot to complete.
    Sleep(10);

    // If called by a power on reset, then force the device to boot,
    // allowing time for the boot to complete.
    vTWI_Write(CTRL_REG5, BOOT);
    Sleep(10);

    // Put device into low-power mode.
    vTWI_Write(CTRL_REG4, HIRES_MODE);
    vTWI_Write(CTRL_REG1, (X_EN | LPEN | ODR_1Hz));

    // Configure MCU to accept LIS2DH12 INT1 pin interrupts
    vInterruptInit();
}

/*!
* @brief Test TWI connection to LIS2DH12 accelerometer
*
* Read and return the LIS2DH12's WHO_AM_I register.
*/
uint8_t u8LIS2_TestRead(void)
{
    uint8_t u8whoAmI;
    vTWI_Read (WHO_AM_I, &u8whoAmI);

    return u8whoAmI;
}

/*!
* @brief Places LIS2DH12 Accelerometer into low-power standby
*/
void vLIS2_PowerDown (void)
{
    // Set the ODR value to put device into power-down mode.
    vTWI_Write(CTRL_REG1, ODR_PDOWN);
}

/*!
* @brief Enable wake-up detection.
*
* Configured to generate an interupt if X,Y,Z sensors
* detect instantaneous dynamic acceleration greater than
* +/- 32mg over a 2g range sampled at 10Hz.
* High-Pass filter is enable to remove static (gravitational)
* acceleration.
*/
void vLIS2_EnableWakeUpDetect (void)
{
    // Configure threshold detector for activity wake-up event
    vThresholdConfigure (THS_FS2G_32mg, 0, (XHIE | YHIE | ZHIE));
}

/*!
* @brief Enable prolonged inactivity detection.
*
* Configured to generate an interupt if X,Y,Z sensors
* detect enduring dynamic acceleration less than
* +/- 32mg over a 2g range for a period of 3 seconds sampling
* at 10Hz.
* High-Pass filter is enable to remove static (gravitational)
* acceleration.
*/
void vLIS2_EnableInactivityDetect (void)
{
    // Configure threshold detector for inactivity event
    vThresholdConfigure (THS_FS2G_32mg, 30, (XLIE | YLIE | ZLIE));
}

void vLIS2_EnableFifoSampling (void)
{
    // Set semaphore, no FIFO event yet
    boInterruptEvent = false;

  // Enable X,Y,Z sensors and set a default sample rate
    vTWI_Write(CTRL_REG1, (ODR_10Hz | X_EN | Y_EN | Z_EN));

    // Disable high-pass filtering
    vTWI_Write(CTRL_REG2, 0);

    // Enabling FIFO high watermark interrupt
    vTWI_Write(CTRL_REG3, I1_WTM);

    // Selecting 12-bit resolution, range +/- 2g
    vTWI_Write(CTRL_REG4, (HIRES_MODE | FS_2G | BDU_ENABLE));
    eRsolution = mode_12bit;
    u8SignExtend = 4;

    // Enable FIFO interrupt, latched on INT1 pin
    vTWI_Write(CTRL_REG5, (FIFO_EN | LIR_INT1));

    // Set BYPASS mode to reset the FIFO
    vTWI_Write(FIFO_CTRL_REG, BYPASS);
    uint8_t u8Dummy;
    vTWI_Read(FIFO_SRC_REG, &u8Dummy);

    // Set FIFO to streaming mode, set level of high watermark and
    // enable interrupt on reaching high watermark
    vTWI_Write(FIFO_CTRL_REG, (STREAM_MODE | TR_INT1 | FIFO_WATERMARK_30));
}


/*!
* @brief Polled function call returning interrupt event semaphore - check only.
*/
bool boLIS2_InterruptOccurred (void)
{
    return boInterruptEvent;
}

/*!
* @brief For polled function call - clear pending interrupt
*/
void boLIS2_InterruptClear (void)
{
    boInterruptEvent = false;
}
/*!
* @brief Returns interrupt status register,
* clearing any pending interrupts.
*/
uint8_t u8LIS2_EventStatus (void)
{
    uint8_t u8status;
    vTWI_Read(INT1_SRC, &u8status);

    return u8status;
}

/*!
* @brief Polled function call that checks for FIFO full events
* and then copies the LIS2 internal FIFO to a local structure.
* Calls a function to dump the FIFO samples to the STDIO.
*/
void vLIS2_Task (void)
{
    if (boInterruptEvent)
    {
        uint8_t u8FifoStat;
        uint8_t u8NumReadings;

        // clear the interrupt source, ignore status reg contents
        vTWI_Read(INT1_SRC, &u8FifoStat);

        // Find number of FIFO samples
        vTWI_Read(FIFO_SRC_REG, &u8FifoStat);
        u8NumReadings = u8FifoStat & FSS_MASK;

        // Read all LIS2 FIFO samples to local structure
        uint8_t u8Cnt, u8Reading, u8LoData;
        for (u8Cnt=0; u8Cnt < u8NumReadings; u8Cnt++)
        {
            // X-axis
            vTWI_Read(OUT_X_LO, &u8Reading);
            u8LoData = u8Reading;
            vTWI_Read(OUT_X_HI, &u8Reading);
            atsFifo[u8Cnt].u16_X = ( (u8Reading << 8) | u8LoData);

            // Y-axis
            vTWI_Read(OUT_Y_LO, &u8Reading);
            u8LoData = u8Reading;
            vTWI_Read(OUT_Y_HI, &u8Reading);
            atsFifo[u8Cnt].u16_Y = ( (u8Reading << 8) | u8LoData);

            // Z-axis
            vTWI_Read(OUT_Z_LO, &u8Reading);
            u8LoData = u8Reading;
            vTWI_Read(OUT_Z_HI, &u8Reading);
            atsFifo[u8Cnt].u16_Z = ( (u8Reading << 8) | u8LoData);
        }

        // Send samples to STDOUT
        vDumpFifo(u8NumReadings);

        // wait for Fifo to refill
        boInterruptEvent = false;

    }
}

/* --- Local scope functions */

/*!
* @brief Handler for LIS2DH12 INT1 interrupt pin, sets a semaphore.
* Interrupt is active on positive edge and latched until cleared by
* reading the INT1 status register.
*/
void vInterruptHandler(void)
{
    // Register interrupt event
    boInterruptEvent = true;
}

/*!
* @brief Configure an IO pin as a positive edge triggered interrupt source.
*/
static void vInterruptInit (void)
{
    boInterruptEvent = false;
    
    portSetInterruptHook(vInterruptHandler);
}

/*!
* @brief Configures the LIS2DH12 threshold detectors.
*
* Inputs: u8Level - requested acceleration detection threshold
*                   u8Duration - acceleration duration must be sustained
*                   for this duration.
*                   Duration in multiples of sampling rate.
*                   u8Mode - detection mode, passing above or below threshold.
*/
static void vThresholdConfigure (uint8_t u8Level, uint8_t u8Duration, uint8_t u8Mode)
{
  // Enable X,Y,Z sensors and set a default sample rate
    vTWI_Write(CTRL_REG1, (ODR_10Hz | X_EN | Y_EN | Z_EN));

    // Enable high-pass filtering with highest cut-off frequency
    // and unfiltered samples to the data registers
    vTWI_Write(CTRL_REG2, (HPF_CUTOFF_F0 | HP_IA1));

    // Enable INT1 interrupts
    vTWI_Write(CTRL_REG3, I1_IA1);

    // Select measurement range to +/- 2g, 12-bit resolution
    vTWI_Write(CTRL_REG4, HIRES_MODE | FS_2G);
    //vTWI_Write(CTRL_REG4, FS_2G);
    eRsolution = mode_12bit;
    u8SignExtend = 4;

    // Set wake-up threshold level
    vTWI_Write(INT1_THS, u8Level);

    // Set duration that threshold needs to be held
    vTWI_Write(INT1_DURATION, u8Duration);

    // Enable interrupt on INT1 pin
    boInterruptEvent = false;
    vTWI_Write(CTRL_REG5, LIR_INT1);

    // Read reference register to force HP filters to current
    // acceleration/tilt value
    uint8_t u8dummy;
    vTWI_Read(REFERENCE_REG, &u8dummy);

    // Enable threshold event
    vTWI_Write(INT1_CFG, u8Mode);
}

/*!
* @brief Send the local copy of the LIS2DH12 FIFO samples to STDOUT.
*/
static void vDumpFifo(uint8_t u8Num)
{
    uint8_t u8Cnt;

    for (u8Cnt=0; u8Cnt < u8Num; u8Cnt++)
    {
        // Convert twos-compliment into signed 16-bit integer and remove left justification
        printf("%d %d %d\n",((int16_t)atsFifo[u8Cnt].u16_X) / (1 << u8SignExtend),
                            ((int16_t)atsFifo[u8Cnt].u16_Y) / (1 << u8SignExtend),
                            ((int16_t)atsFifo[u8Cnt].u16_Z) / (1 << u8SignExtend));
    }
}
