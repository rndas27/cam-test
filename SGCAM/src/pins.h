
#define VIDEO_SELECT 1

#define LED_BLUE 39
#define LED_GREEN 40
#define LED_ORANGE 41
#define LED_RED 42

#define SPI_SCK 14

// Schematic has MOSI @ 16, MISO @ 15. These pins are swapped, so we swap them back in code.
#define SPI_MOSI 15
#define SPI_MISO 16

//I2C
#define SCL 5
#define SDA 6

//buzzer pin
#define BUZZER_PIN 35
#define BUZZER_CHANNEL 1

// Can stuff
#define CAN_CS 17
#define CAN_INT 12

#define CAN_SILENT 21
#define CAN_FAULT 26


//on/off pins
#define ON_OFF_1 3
#define ON_OFF_2 4
#define ON_OFF_VTX 2
