#include <Adafruit_MLX90614.h>
#include <Arduino.h>
#include <TFT_eSPI.h>  // Include the graphics library
#include <Wire.h>

#include <dwd.hpp>

#define BAUD 115200
#define BUZZER_PIN 15
#define TRIG_PIN 12  // HC-SR04 trigger pin
#define ECHO_PIN 13  // HC-SR04 echo pin

TFT_eSPI tft = TFT_eSPI();  // Create object "tft"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
double g_Temprature = 0;

TIMER blackOut(30000);

// Global function declerations
void displayTemprature(void);
void beep(void);
void measureTemp(void);
double measureDistance(void);

// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup(void) {
    Serial.begin(BAUD);
    mlx.begin();

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_DARKGREY);
    tft.setTextSize(3);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);

    pinMode(TRIG_PIN, OUTPUT);  // define trigger pin as output
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    delay(100);
    Serial.println("ESP32 CoVid2019 Suka");
}

// -------------------------------------------------------------------------
// Main loop
// -------------------------------------------------------------------------
void loop() {
    static double distance;
    distance = measureDistance();
    if (distance <= 3) {
        delay(500);
        measureTemp();
        displayTemprature();
        beep();
        blackOut.resetTimer();
        Serial.println("Measured!!!");
    }

    if (blackOut.checkInterval()) {
        tft.fillScreen(TFT_BLACK);
    }

    delay(5);
}

// Global finction definitions
void displayTemprature(void) {
    String temprature;
    temprature = String(g_Temprature) + "*C";
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(temprature, tft.width() / 2, tft.height() / 2);
}

void beep(void) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
}

void measureTemp(void) {
    double acc = 0;
    mlx.readAmbientTempC();
    for (int i = 0; i < 20; i++) {
        acc += mlx.readObjectTempC();
        delay(50);
    }
    acc /= 20;
    acc *= 1.05;
    acc += 4;
    g_Temprature = acc;
}

double measureDistance(void) {
    double duration, distance = 0;

    digitalWrite(ECHO_PIN, LOW);  // set the echo pin LOW
    digitalWrite(TRIG_PIN, LOW);  // set the trigger pin LOW
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);  // set the trigger pin HIGH for 10μs
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH);    // measure the echo time (μs)
    distance = (duration / 2.0) * 0.0343;  // convert echo time to distance (cm)

    return distance;
}
