#pragma once


// Camera/Video Control Pins
#define CAM1_ON_OFF 3
#define CAM2_ON_OFF 2
#define VTX_ON_OFF 4

#define VIDEO_SELECT 1

#define CAM1_TX 9
#define CAM1_RX 8
#define CAM2_TX 11
#define CAM2_RX 10

// I2C Busses
#define BATTSENSE_SCL 5
#define BATTSENSE_SDA 6
#define BATTSENSE_ALERT 7

#define I2C_SCL 36
#define I2C_SDA 37

// CAN
#define CAN_SPI_SCK 14
#define CAN_SPI_MOSI 15
#define CAN_SPI_MISO 16
#define CAN_NCS 17
#define CAN_NINT 12
#define CAN_NINT1 13
#define CAN_SLNT 21
#define CAN_FAULT 26

// Buzzer - defined in buzzer.cpp
// #define BUZZER 35

//LEDs
#define LED_BLUE 39
#define LED_GREEN 40
#define LED_ORANGE 41
#define LED_RED 42