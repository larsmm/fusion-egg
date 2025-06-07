/* 
 * Copyright 2023 Alexander Alber and Thomas Schmid
 * 
 * SPDX-License-Identifier: GPL-2.0
 */

#pragma once

#include "Arduino.h"
#include <FastLED.h>
#include <Wire.h>
#include <ADXL345_WE.h>

#define ADXL345_I2CADDR 0x53  // 0x1D if SDO = HIGH
#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_ACC_INT GPIO_NUM_4

// 4-Pin SK9822 LEds:
// #define LED_PIN_CLOCK 16
// #define LED_PIN_DATA 17

// 3-Pin WS28x or SK6812 LEDs (all LED-Strips will get the same signal!):
#define LED_PIN1 32
#define LED_PIN2 25
#define LED_PIN3 27

#define NUM_LEDS 48

// Additional single LEDs driven by mosfet for inside illumination:
#define LED1_PIN 25
#define LED2_PIN 27

// Pin for analog microphone input:
#define MIC_PIN 15

extern CRGB leds[NUM_LEDS];
extern ADXL345_WE myAcc;

int zauberstab_init();
float get_sample();
void switch_app();
bool acc_has_event();