#include <WiFi.h>
#include <Wire.h>
#include "SH1106.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const uint8_t I2C_SDA_PIN  = 21;
const uint8_t I2C_SCL_PIN  = 22;

const uint8_t OLED_ADDRESS = 0x3c;

const uint8_t redPin       = 26;
const uint8_t greenPin     = 18;
const uint8_t bluePin      = 19;

const uint8_t numberOfBits = 16;
const uint64_t frequency   = 1300;

enum channel { redChannel, greenChannel, blueChannel, maxChannel };

TaskHandle_t xTempTaskHandle = NULL;
TaskHandle_t xOledTempTaskHandle = NULL;

SH1106 oled( OLED_ADDRESS, I2C_SDA_PIN, I2C_SCL_PIN );

AsyncWebServer server(80);

String statusText;
float temperature = -1000;

float coldTemp = 15; //only blue light at this temp
float niceTemp = 20; //only green light at this temp
float warmTemp = 25; //only red light at this temp

uint32_t PWMdepth;

void setup()
{
  btStop();
  Serial.begin( 115200 );
  Serial.println();

  //setup pwm
  ledcSetup( redChannel, frequency, numberOfBits );
  ledcSetup( greenChannel, frequency, numberOfBits );
  ledcSetup( blueChannel, frequency, numberOfBits );

  ledcAttachPin( redPin, redChannel);
  ledcAttachPin( greenPin, greenChannel);
  ledcAttachPin( bluePin, blueChannel);

  PWMdepth = ( 0x00000001 << numberOfBits ) - 1;

  xTaskCreatePinnedToCore(
    oledTask,                        /* Function to implement the task */
    "oledTask",                      /* Name of the task */
    3000,                            /* Stack size in words */
    NULL,                            /* Task input parameter */
    2,                               /* Priority of the task */
    &xOledTempTaskHandle,            /* Task handle. */
    1);

  xTaskCreatePinnedToCore(
    wifiTask,                        /* Function to implement the task */
    "wifiTask",                      /* Name of the task */
    3000,                            /* Stack size in words */
    NULL,                            /* Task input parameter */
    1,                               /* Priority of the task */
    NULL,                            /* Task handle. */
    1);
}

void loop()
{
  vTaskDelete( NULL );
}
