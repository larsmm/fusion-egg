#pragma once
#include "app.h"
#include <Arduino.h>

class FadeLedApp : public App {
public:
    FadeLedApp(int pin, int channel, unsigned long period, int pwm_min, int pwm_max):
        _pin(pin), _channel(channel), _period(period), _pwm_min(pwm_min), _pwm_max(pwm_max) {}
    void setPwmMin(int min) { _pwm_min = min; }
    void setPwmMax(int max) { _pwm_max = max; }
    void setPwmFrequency(uint32_t freq) { _pwm_freq = freq; }
    void setPeriod(unsigned long period) { _period = period; }

    void init() override {
        pinMode(_pin, OUTPUT);
        _start = millis();
        ledcSetup(_channel, _pwm_freq, 8);
        ledcAttachPin(_pin, _channel);
    }
    void deinit() override {
        ledcWrite(_channel, 0);
    }
    void loop() override {
        unsigned long t = millis() - _start;
        float phase = (t % _period) / (float)_period;
        float value = 0.5f * (1.0f - cos(phase * 2 * PI));
        int pwm = _pwm_min + (int)((_pwm_max - _pwm_min) * value);
        ledcWrite(_channel, pwm);
        delay(10);
    }

private:
    int _pin;
    int _channel;
    unsigned long _start = 0;
    unsigned long _period = 5000;
    int _pwm_min = 30;
    int _pwm_max = 255;
    uint32_t _pwm_freq = 8000;
};
