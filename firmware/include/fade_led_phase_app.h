#pragma once
#include "app.h"
#include <Arduino.h>

class FadeLedPhaseApp : public App {
public:
    FadeLedPhaseApp(int pin1, int channel1, int pin2, int channel2, unsigned long period, int pwm_min1, int pwm_max1, int pwm_min2, int pwm_max2)
        : _pin1(pin1), _channel1(channel1), _pin2(pin2), _channel2(channel2), _period(period), _pwm_min1(pwm_min1), _pwm_max1(pwm_max1), _pwm_min2(pwm_min2), _pwm_max2(pwm_max2) {}

    void setPwmMin1(int min1) { _pwm_min1 = min1; }
    void setPwmMax1(int max1) { _pwm_max1 = max1; }
    void setPwmMin2(int min2) { _pwm_min2 = min2; }
    void setPwmMax2(int max2) { _pwm_max2 = max2; }
    void setPwmFrequency(uint32_t freq) { _pwm_freq = freq; }
    void setPeriod(unsigned long period) { _period = period; }

    void init() override {
        pinMode(_pin1, OUTPUT);
        pinMode(_pin2, OUTPUT);
        _start = millis();
        ledcSetup(_channel1, _pwm_freq, 8);
        ledcAttachPin(_pin1, _channel1);
        ledcSetup(_channel2, _pwm_freq, 8);
        ledcAttachPin(_pin2, _channel2);
    }
    void deinit() override {
        ledcWrite(_channel1, 0);
        ledcWrite(_channel2, 0);
    }
    void loop() override {
        unsigned long t = millis() - _start;
        float phase = (t % _period) / (float)_period;
        float value1 = 0.5f * (1.0f - cos(phase * 2 * PI));
        float value2 = 0.5f * (1.0f - cos((phase + 0.5f) * 2 * PI)); // 180° Phasenversatz
        int pwm1 = _pwm_min1 + (int)((_pwm_max1 - _pwm_min1) * value1);
        int pwm2 = _pwm_min2 + (int)((_pwm_max2 - _pwm_min2) * value2);
        ledcWrite(_channel1, pwm1);
        ledcWrite(_channel2, pwm2);
        delay(10);
    }

private:
    int _pin1;
    int _channel1;
    int _pin2;
    int _channel2;
    unsigned long _start = 0;
    unsigned long _period = 5000;
    int _pwm_min1 = 0;
    int _pwm_max1 = 255;
    int _pwm_min2 = 0;
    int _pwm_max2 = 255;
    uint32_t _pwm_freq = 8000;
};