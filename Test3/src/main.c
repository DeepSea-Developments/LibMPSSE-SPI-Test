/*!
 * \file main.c
 *
 * \author DeepSea Developments
 * \date 20200404
 *
 * 
 */

/******************************************************************************/
/* 							 Include files										   */
/******************************************************************************/
/* Standard C libraries */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/* OS specific libraries */
#ifdef _WIN32
#include<windows.h>
#endif

/* Include libMPSSE header */
#include "libMPSSE_spi.h"

/* Include D2XX header*/
#include "ftd2xx.h"

/******************************************************************************/
/*								Macro and type defines							   */
/******************************************************************************/
/* Helper macros */

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

/* Application specific macro definations */
#define SPI_DEVICE_BUFFER_SIZE		256
#define SPI_WRITE_COMPLETION_RETRY		10
#define START_ADDRESS_EEPROM 	0x00 /*read/write start address inside the EEPROM*/
#define END_ADDRESS_EEPROM		0x10
#define RETRY_COUNT_EEPROM		10	/* number of retries if read/write fails */
#define CHANNEL_TO_OPEN			0	/*0 for first available channel, 1 for next... */
#define SPI_SLAVE_0				0
#define SPI_SLAVE_1				1
#define SPI_SLAVE_2				2
#define DATA_OFFSET				4
#define USE_WRITEREAD			0

/******************************************************************************/
/*								Global variables							  	    */
/******************************************************************************/
static FT_HANDLE ftHandle;
static uint8 buffer[SPI_DEVICE_BUFFER_SIZE] = {0};

/******************************************************************************/
/*						Public function definitions						  		   */
/******************************************************************************/


/*!
 * \brief Main function / Entry point to the sample application
 *
 * This function is the entry point to the sample application. It opens the channel, writes to the
 * EEPROM and reads back.
 *
 * \param[in] none
 * \return Returns 0 for success
 * \sa
 * \note
 * \warning
 */
int main()
{
	printf("Test 1!\n");
	
	FT_STATUS status = FT_OK;
	FT_DEVICE_LIST_INFO_NODE devList = {0};
	ChannelConfig channelConf = {0};
	uint8 address = 0;
	uint32 channels = 0;
	uint16 data = 0;
	uint8 i = 0;
	uint8 latency = 255;	
	
	/* SPI Channel configuration */

	//channelConf.ClockRate = 5000;
	channelConf.ClockRate = 30000000;
	channelConf.LatencyTimer = latency;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3;// | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0x00000000;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/
	//channelConf.Pin = 0xFFFFFFFF;

	/* init library */
#ifdef _MSC_VER
	Init_libMPSSE();
#endif
	
	//STEP1: Get number of channels
	status = SPI_GetNumChannels(&channels);
	APP_CHECK_STATUS(status);
	
	printf("Number of available SPI channels = %d\n",(int)channels);

	if(channels == 0)
	{
		printf("No SPI devices found");
		return 0;
	}

	for(i=0;i<channels;i++)
	{
		status = SPI_GetChannelInfo(i,&devList);
		APP_CHECK_STATUS(status);
		printf("Information on channel number %d:\n",i);
		/* print the dev info */
		printf("		Flags=0x%x\n",devList.Flags);
		printf("		Type=0x%x\n",devList.Type);
		printf("		ID=0x%x\n",devList.ID);
		printf("		LocId=0x%x\n",devList.LocId);
		printf("		SerialNumber=%s\n",devList.SerialNumber);
		printf("		Description=%s\n",devList.Description);
		printf("		ftHandle=0x%x\n",(unsigned int)devList.ftHandle);/*is 0 unless open*/
	}

	//STEP 2: Open channel
	status = SPI_OpenChannel(CHANNEL_TO_OPEN,&ftHandle);
	APP_CHECK_STATUS(status);
	
	printf("\nhandle=0x%x status=0x%x\n",(unsigned int)ftHandle,status);
	
	//Step 3: Init Channel
	status = SPI_InitChannel(ftHandle,&channelConf);
	APP_CHECK_STATUS(status);

	
	uint8 buffer_in[15];
	uint8 buffer_out[15];
	uint32 sizeToTransfer=1;
	uint32 sizeTransfered=0;
	
	uint8 gpio_val = 0;
	//Example loop for reading GPIO Values
	for(;;)
	{
			/*Read 1 bytes*/
		gpio_val = 0;
		status = FT_ReadLowGPIO(ftHandle, &gpio_val);
		APP_CHECK_STATUS(status);

		printf("GPIO val: %d %d %d %d %d %d %d %d\n",
				(gpio_val & (1<<0)) && (1<< 0),
				(gpio_val & (1<<1)) && (1<< 1),
				(gpio_val & (1<<2)) && (1<< 2),
				(gpio_val & (1<<3)) && (1<< 3),
				(gpio_val & (1<<4)) && (1<< 4),
				(gpio_val & (1<<5)) && (1<< 5), //INT pin in VA800A-SPI
				(gpio_val & (1<<6)) && (1<< 6),
				(gpio_val & (1<<7)) && (1<< 7)); //PD pin in VA800A-SPI

		sleep(1);
	}

	//Example loop for writing GPIO Values
	for(;;)
	{
		status = FT_WriteGPIO(ftHandle, 0x00, 0xFF);
		APP_CHECK_STATUS(status);
		printf("ON\n");
		sleep(1);


		status = FT_WriteGPIO(ftHandle, 0x00, 0x00);
		APP_CHECK_STATUS(status);
		printf("OFF\n");
		sleep(1);
		
	}

	

	status = SPI_CloseChannel(ftHandle);

}

