/**
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
 /* You put a license on a vitually empty file? Are you serious?*/


#include "platform.h"

#include <fcntl.h> // open()
#include <unistd.h> // close()
#include <time.h> // clock_gettime()

//#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>

#include <sys/ioctl.h>

#define VL53L8CX_ERROR_GPIO_SET_FAIL	-1
#define VL53L8CX_COMMS_ERROR		-2
#define VL53L8CX_ERROR_TIME_OUT		-3

int32_t VL53L8CX_Comms_Init(VL53L8CX_Platform * p_platform, std::string i2c_path)
{
	printf("I2C Path %s at Address = %d\n", i2c_path, p_platform->address);
	p_platform->fd = open(i2c_path.c_str(), O_RDONLY);
	if (p_platform->fd == -1) {
		printf("Failed to open /dev/i2c-1\n");
		return VL53L8CX_COMMS_ERROR;
	}

	if (ioctl(p_platform->fd, I2C_SLAVE, p_platform->address) <0) {
		printf("Could not speak to the device on the i2c bus\n");
		return VL53L8CX_COMMS_ERROR;
	}
	printf("VL53L8CX FD initialization complete.\n");
	return 0;
}

int32_t VL53L8CX_Comms_Close(VL53L8CX_Platform * p_platform)
{
	close(p_platform->fd);
	return 0;
}

#define VL53L8CX_COMMS_CHUNK_SIZE  1024U
static uint8_t i2c_buffer[VL53L8CX_COMMS_CHUNK_SIZE];

int32_t write_multi(
		int fd,
		uint16_t i2c_address,
		uint16_t reg_address,
		uint8_t *pdata,
		uint32_t count)
{
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];

	uint32_t data_size = 0;
	uint32_t position = 0;
	static uint8_t i2c_buffer[VL53L8CX_COMMS_CHUNK_SIZE];

	do {
	data_size = (count - position) > (VL53L8CX_COMMS_CHUNK_SIZE-2) ? (VL53L8CX_COMMS_CHUNK_SIZE-2) : (count - position);

	memcpy(&i2c_buffer[2], &pdata[position], data_size);

	i2c_buffer[0] = (reg_address + position) >> 8;
	i2c_buffer[1] = (reg_address + position) & 0xFF;

	messages[0].addr = i2c_address >> 1;
	messages[0].flags = 0; //I2C_M_WR;
	messages[0].len = data_size + 2;
	messages[0].buf = i2c_buffer;

	packets.msgs = messages;
	packets.nmsgs = 1;

	if (ioctl(fd, I2C_RDWR, &packets) < 0)
		return VL53L8CX_COMMS_ERROR;
	position +=  data_size;

	} while (position < count);
	
	return 0;
}

int32_t read_multi(
		int fd,
		uint16_t i2c_address,
		uint16_t reg_address,
		uint8_t *pdata,
		uint32_t count)
{
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];

	uint32_t data_size = 0;
	uint32_t position = 0;

	do {
		data_size = (count - position) > VL53L8CX_COMMS_CHUNK_SIZE ? VL53L8CX_COMMS_CHUNK_SIZE : (count - position);

		i2c_buffer[0] = (reg_address + position) >> 8;
		i2c_buffer[1] = (reg_address + position) & 0xFF;

		messages[0].addr = i2c_address >> 1;
		messages[0].flags = 0; //I2C_M_WR;
		messages[0].len = 2;
		messages[0].buf = i2c_buffer;

		messages[1].addr = i2c_address >> 1;
		messages[1].flags = I2C_M_RD;
		messages[1].len = data_size;
		messages[1].buf = pdata + position;

		packets.msgs = messages;
		packets.nmsgs = 2;

		if (ioctl(fd, I2C_RDWR, &packets) < 0)
			return VL53L8CX_COMMS_ERROR;

		position += data_size;

	} while (position < count);

	return 0;
}

uint8_t VL53L8CX_RdByte(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_value)
{
	//uint8_t status = 255;
	return(read_multi(p_platform->fd, p_platform->address, RegisterAdress, p_value, 1));
	/* Need to be implemented by customer. This function returns 0 if OK */
	/* I implemented it. Are you happy now? */
}

uint8_t VL53L8CX_WrByte(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t value)
{
	//uint8_t status = 255;
	return(write_multi(p_platform->fd, p_platform->address, RegisterAdress, &value, 1));
	/* Need to be implemented by customer. This function returns 0 if OK */
	/* I implemented this also. Are you my customer or am I yours? */
}

uint8_t VL53L8CX_WrMulti(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	return(write_multi(p_platform->fd, p_platform->address, RegisterAdress, p_values, size));
	/* Need to be implemented by customer. This function returns 0 if OK */
	/* Instead of returning 0, can I return the product? */
}

uint8_t VL53L8CX_RdMulti(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	return(read_multi(p_platform->fd, p_platform->address, RegisterAdress, p_values, size));
	/* Need to be implemented by customer. This function returns 0 if OK */
	/* I should get half the saraly your devs get. */
}
 
uint8_t VL53L8CX_Reset_Sensor(
		VL53L8CX_Platform *p_platform)
{
	uint8_t status = 0;
	
	/* (Optional) Need to be implemented by customer. This function returns 0 if OK */
	/* It's not even used in the API. Why do I have to implement this??? What?? */
	
	/* Set pin LPN to LOW */
	/* Set pin AVDD to LOW */
	/* Set pin VDDIO  to LOW */
	/* Set pin CORE_1V8 to LOW */
	VL53L8CX_WaitMs(p_platform, 100);

	/* Set pin LPN to HIGH */
	/* Set pin AVDD to HIGH */
	/* Set pin VDDIO to HIGH */
	/* Set pin CORE_1V8 to HIGH */
	VL53L8CX_WaitMs(p_platform, 100);

	return status;
}

void VL53L8CX_SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size)
{
	uint32_t i, tmp;
	
	/* Example of possible implementation using <string.h> */
	for(i = 0; i < size; i = i + 4) 
	{
		tmp = (
		  buffer[i]<<24)
		|(buffer[i+1]<<16)
		|(buffer[i+2]<<8)
		|(buffer[i+3]);
		
		memcpy(&(buffer[i]), &tmp, 4);
	}
	/* Well, at least you implemented ONE function for me. */
}	

uint8_t VL53L8CX_WaitMs(
		VL53L8CX_Platform *p_platform,
		uint32_t TimeMs)
{
	uint8_t status = 255;

	/* Need to be implemented by customer. This function returns 0 if OK */
	usleep(TimeMs*1000);
	status = 0;
	/* I can't believe you can't write a simple usleep()... */

	return status;
}
