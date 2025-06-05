/* 
 * Copyright 2023 Alexander Alber and Thomas Schmid
 * 
 * SPDX-License-Identifier: GPL-2.0
 */

#include "app.h"
#include "driver/adc.h"
#include "zauberstab.h"
#include <vector>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <FastLED.h>

struct BeatDetectApp beat_detect_app
{
};

struct QuarterApp quarter_app
{
};

struct FackelApp fackel_app
{
};



std::vector<std::reference_wrapper<App>> apps = {
    std::ref<App>(beat_detect_app),
    std::ref<App>(fackel_app),
    std::ref<App>(quarter_app),
    };

static unsigned int current_app = 0;
static unsigned int next_app;
static bool init_successfull = false;
static bool sleep_active=false;

void blinkAllLeds(CRGB color, int times, int on_ms = 150, int off_ms = 150, uint8_t brightness = 255, int num_leds_from_end = 0) {
    uint8_t old_brightness = FastLED.getBrightness();
    fadeToBlackBy(leds, NUM_LEDS, 0xFF);
    FastLED.setBrightness(brightness);
    int start_led = 0;
    int end_led = NUM_LEDS;
    if (num_leds_from_end > 0 && num_leds_from_end < NUM_LEDS) {
        start_led = NUM_LEDS - num_leds_from_end;
    }
    for (int t = 0; t < times; ++t) {
        for (int i = start_led; i < end_led; ++i) {
            leds[i] = color;
        }
        FastLED.show();
        delay(on_ms);
        for (int i = start_led; i < end_led; ++i) {
            leds[i] = CRGB::Black;
        }
        FastLED.show();
        if (t < times - 1) delay(off_ms);
    }
    FastLED.setBrightness(old_brightness);
}

void ota() {
    // Prüfe, ob das Ei auf dem Kopf steht
    xyzFloat g = myAcc.getGValues();
    // Serial.print("Y-Achse (OTA-Check): ");
    // Serial.println(g.y);
    if (g.y >= -0.7) return;

    blinkAllLeds(CRGB::White, 3, 250, 250, 30);

    // WLAN Access Point starten
    WiFi.mode(WIFI_AP);
    WiFi.softAP("FusionEgg-OTA", "12345678");
    Serial.println("Ei steht auf dem Kopf, starte OTA-Modus");
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());

    // OTA initialisieren
    ArduinoOTA.setHostname("fusionegg");
    ArduinoOTA.onStart([]() {
        Serial.println("OTA Update gestartet");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("OTA Update abgeschlossen");
        blinkAllLeds(CRGB::Green, 1, 2000, 0, 30);
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("OTA Fehler [%u]\n", error);
        blinkAllLeds(CRGB::Red, 1, 2000, 0, 30);
    });
    ArduinoOTA.begin();
    Serial.println("OTA-Server bereit. Warte auf Update...");

    // OTA-Loop: Warte auf Update, blockiere normale Ausführung
    while (true) {
        ArduinoOTA.handle();
        delay(250);
    }
}

void switch_app() {
    if (!sleep_active) {
        next_app = current_app + 1;
    } else {
        sleep_active = false;
        next_app = 0;
    }

    if (next_app >= apps.size()) {
        next_app = 0;
        // Turn off leds before going to sleep
        fadeToBlackBy(leds, NUM_LEDS, 0xFF);
        FastLED.show();

        //configure wakeup source
        esp_sleep_enable_ext0_wakeup(PIN_ACC_INT, 1);
        
        //bedtime
        sleep_active = true;
        esp_deep_sleep_start();
    }

    next_app = next_app % apps.size();
}

void setup()
{
    Serial.begin(115200);
    // sleep(10);  // useful for debugging
    Serial.println("setup start");

    next_app = current_app;
    if (zauberstab_init() != 0) {
        return;
    }

    ota();

    init_successfull = true;
    apps[current_app].get().init();
    Serial.println("setup completed");
}

void loop()
{
    if (!init_successfull) {
        return;
    }

    if (acc_has_event()) {
        // Serial.println("acc has event");
        String axes = myAcc.getActTapStatusAsString();
        byte intSource = myAcc.readAndClearInterrupts();

        if (myAcc.checkInterrupt(intSource, ADXL345_DOUBLE_TAP)) {
            switch_app();
            Serial.println("double tap detected");
        }
    }

    if (next_app != current_app)
    {
        Serial.print("app: ");
        Serial.println(next_app);
        apps[current_app].get().deinit();
        apps[next_app].get().init();
        current_app = next_app;
        fadeToBlackBy(leds, NUM_LEDS, 0xFF);
    }

    apps[current_app].get().loop();
}