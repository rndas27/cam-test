#include <Arduino.h>
#include <Wire.h>
#include <pins.h>
#include <SPI.h>
#include <ACAN2517FD.h>
#include <ACAN2517FDSettings.h>

bool selected_camera = false; // false --> camera 1, true --> camera 2

// #define VIDEO_MUX
//#define CAN_TEST
//#define REG_TEST
// #define REG_OFF
// #define MOSFET_TEST
#define CHRISTMAS_TEST

#ifdef CAN_TEST

ACAN2517FD can (CAN_CS, SPI, CAN_INT) ; // You can use SPI2, SPI3, if provided by your microcontroller

#endif

#ifdef CHRISTMAS_TEST
#include <buzzer.h>
bool cur_light_state = false;
#endif


void setup() {
    Serial.begin(9600);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_ORANGE, OUTPUT);


    digitalWrite(LED_ORANGE, HIGH);
    delay(3000);
    Serial.println("Begin setup.");

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    Wire.begin();

    #ifdef CHRISTMAS_TEST
      pinMode(BUZZER_PIN, OUTPUT);
      digitalWrite(BUZZER_PIN, LOW);
      ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    #endif

    #ifdef REG_OFF
      pinMode(REG_12V, OUTPUT);
      digitalWrite(REG_12V, LOW);
    #endif

    #ifdef REG_TEST
      pinMode(REG_12V, OUTPUT);
      digitalWrite(REG_12V, HIGH);
    #endif

    #ifdef CAN_TEST
      Serial.println("Initializing CAN Controller");

      pinMode(CAN_CS, OUTPUT);
      pinMode(CAN_INT, INPUT);
      pinMode(CAN_SILENT, OUTPUT);

      pinMode(CAN_FAULT, INPUT);

      digitalWrite(CAN_SILENT, LOW);

      ACAN2517FDSettings can_settings (ACAN2517FDSettings::OSC_40MHz, 125*1000, ACAN2517FDSettings::DATA_BITRATE_x1  );
      can_settings.mRequestedMode = ACAN2517FDSettings::Normal20B;

      const uint32_t errorCode = can.begin (can_settings, [] { can.isr () ; }) ;
      if (0 == errorCode) {
        Serial.println ("Can ok") ;
      }else{
        Serial.print ("Error Can: 0x") ;
        Serial.println (errorCode, HEX) ;
      }
    #endif

    #ifdef MOSFET_TEST
      pinMode(ON_OFF_2, OUTPUT);
      pinMode(ON_OFF_VTX, OUTPUT);
      digitalWrite(ON_OFF_2, LOW);
      digitalWrite(ON_OFF_VTX, LOW);
    #endif

    delay(500);
    
    Serial.println("Setup complete.");
    digitalWrite(LED_ORANGE, LOW);
}

int read_reg(int reg, int bytes) {
    Wire.beginTransmission(0x40);
    Wire.write(reg);
    if(Wire.endTransmission()){
        Serial.println("I2C Error");
    }
    Wire.requestFrom(0x40, bytes);
    int val = 0;
    for(int i = 0; i < bytes; i++){
        int v = Wire.read();
        if(v == -1) Serial.println("I2C Read Error");
        val = (val << 8) | v;
    }
    return val;
}

bool toggle_camera() {
    selected_camera = !selected_camera;
    digitalWrite(VIDEO_SELECT, selected_camera);
    digitalWrite(LED_BLUE, selected_camera);
    return selected_camera;
}

static unsigned gSendDate = 0 ;
static unsigned gSentCount = 0 ;

void loop() {
    // int power = read_reg(0x8, 3);
    // int current = read_reg(0x7, 2);
    // int temp = read_reg(0x6, 2);
    // int voltage = read_reg(0x5, 2);
    // Serial.print("Voltage ");
    // Serial.println(voltage * 3.125 / 1000.0);
    // Serial.print("Temp ");
    // Serial.println(temp * 125 / 1000.0);
    // Serial.print("Current ");
    // Serial.println(current * 1.2 / 1000.0);
    // Serial.print("Power ");
    // Serial.println(power * 240 / 1000000.0);

    #ifdef VIDEO_MUX
      delay(5000);
      bool cur_stream = toggle_camera();
      Serial.print(cur_stream ? "Stream 2\n" : "Stream 1\n");
    #endif

    #ifdef CHRISTMAS_TEST
      //play christmas
      for(int i = 0; i < MERRY_CHRISTMAS_LENGTH; i++){
        cur_light_state = !cur_light_state;

        digitalWrite(LED_GREEN, cur_light_state);
        digitalWrite(LED_RED, !cur_light_state);

        Sound s = merry_christmas[i];
        ledcWriteTone(BUZZER_CHANNEL, s.frequency);
        delay(s.duration_ms);
      }

      delay(2000);
    #endif

    #ifdef CAN_TEST
      delay(500);

      Serial.println("Controller phase:");
      

      CANFDMessage message ;
      if (gSendDate < millis ()) {
        message.id = 0x542 ;

        // error: assigning to an array from an initializer list
        // message.data = {0, 1, 2, 3, 4, 5, 6, 7} ;
        message.data[0] = 7;
        message.data[1] = 7;
        message.data[2] = 5;
        message.data[3] = 2;
        message.data[4] = 8;
        message.data[5] = 3;
        message.data[6] = 3;
        message.data[7] = 4;

        message.len = 8;

        const bool ok = can.tryToSend (message) ;
        if (ok) {
          gSendDate += 2000 ;
          gSentCount += 1 ;
          Serial.print ("Sent msg# ") ;
          Serial.println (gSentCount) ;
          digitalWrite(LED_GREEN, HIGH);
          delay(50);
          digitalWrite(LED_GREEN, LOW);

        } else {
          Serial.println("FAILED SEND");
          digitalWrite(LED_RED, HIGH);
          delay(50);
          digitalWrite(LED_RED, LOW);
        }


        // Diagnostic info
        Serial.println("Diag info:");
        // Read diag register 0

        uint32_t diag0 = can.diagInfos(0);
        Serial.print("Diag 0: ");
        Serial.println(diag0, BIN);

        // Read diag register 1
        uint32_t diag1 = can.diagInfos(1);
        Serial.print("Diag 1: ");
        Serial.println(diag1, BIN);

      }

      if (can.receive (message)) {
        Serial.print ("CAN Received: ") ;
        Serial.printf("%d%d%d%d%d%d%d%d\n", message.data[0], message.data[1], message.data[2], message.data[3], message.data[4], message.data[5], message.data[6], message.data[7]);
      }

      Serial.println("Transciever phase:");

      bool can_fault = digitalRead(CAN_FAULT);
      if(can_fault) {
        Serial.println("CAN Fault");
        digitalWrite(LED_RED, HIGH);

      } else {
        digitalWrite(LED_RED, LOW);
      }


    #endif

    #ifdef MOSFET_TEST
    digitalWrite(ON_OFF_2, HIGH);
    digitalWrite(LED_BLUE, HIGH);

    delay(3000);
    digitalWrite(ON_OFF_2, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(ON_OFF_VTX, HIGH);
    digitalWrite(LED_RED, HIGH);

    delay(3000);
    digitalWrite(ON_OFF_VTX, LOW);
    digitalWrite(LED_RED, LOW);

    #endif

    delay(1);
}