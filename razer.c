#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <byteswap.h>
#include "libusb.h"

enum rMode {
RAZER_MODE_DPI = 0,
RAZER_MODE_LOGO_COLOR,
RAZER_MODE_WHEEL_COLOR,
RAZER_MODE_LOGO_SOLID,
RAZER_MODE_WHEEL_SOLID,
RAZER_MODE_LOGO_DISABLE,
RAZER_MODE_WHEEL_DISABLE
};

uint8_t* buildPacket(enum rMode mode, void* value){
	uint8_t* packet = malloc(90);
	memset(packet, 0x00, 90);
	memcpy(packet, "\x00\x3f\x00\x00\x00", 5); // Header
	switch(mode){
		case RAZER_MODE_DPI:
			packet[88] = 0x06;
			memcpy(packet+5, "\x07\x04\x05", 3);
			//uint8_t vbuffer[2];
			//vbuffer[0] = (uint16_t*)value[1]; // endianess
			//vbuffer[1] = (uint16_t*)value[0];
			uint16_t vbuffer = __bswap_16(*(uint16_t*)value);
			memcpy(packet+9, &vbuffer, 2);
			memcpy(packet+11, &vbuffer, 2);
			break;
		case RAZER_MODE_LOGO_COLOR:
			packet[88] = 0xfd;
			memcpy(packet+5, "\x05\x03\x01\x01\x04", 5);
			memcpy(packet+10, value, 3);
			break;
		case RAZER_MODE_WHEEL_COLOR:
			packet[88] = 0xf8;
			break;
		case RAZER_MODE_LOGO_SOLID:
			packet[88] = 0x04;
			break;
		case RAZER_MODE_WHEEL_SOLID:
			packet[88] = 0x01;
			break;
		case RAZER_MODE_LOGO_DISABLE:
			packet[88] = 0x05;
			break;
		case RAZER_MODE_WHEEL_DISABLE:
			packet[88] = 0x00;
			break;
		default:
			free(packet);
			return NULL;
	}

	return packet;
}

static void sendPacket(uint8_t* packet){
	libusb_init(NULL);
	libusb_device_handle* razerHandle = libusb_open_device_with_vid_pid(NULL, 0x1532, 0x0043);
	libusb_set_auto_detach_kernel_driver(razerHandle, 1);
	libusb_claim_interface(razerHandle, 0);
	libusb_set_configuration(razerHandle, 1);
	libusb_control_transfer(razerHandle, 
				0x21,
				9,
				0x0300,
				0,
				packet,
				90,
				0);

}

static void printPacket(uint8_t* packet){
	for(int i = 0; i<90; i++){
		printf("%.2X", packet[i]);
	};
	printf("\n");
}

int main(){
	uint8_t* packet;

	/*
	uint16_t value = 100;
	packet = buildPacket(RAZER_MODE_DPI, &value);
	printPacket(packet);

	uint8_t valueRGB[3] = {0xFF, 0xAB, 0xCD};
	packet = buildPacket(RAZER_MODE_LOGO_COLOR, valueRGB);
	printPacket(packet);
	*/

	uint8_t valueRGB[3] = {0xFF, 0x00, 0x00};
	packet = buildPacket(RAZER_MODE_LOGO_COLOR, valueRGB);
	sendPacket(packet);

	return 0;
}

