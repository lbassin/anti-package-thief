#include <Arduino.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>

#define RX_BLUETOOTH 2
#define TX_BLUETOOTH 3
#define STATE_BLUETOOTH 4

#define ALARM_PIN 5
#define ALARM_DURATION 5000

MPU6050 mpu;
SoftwareSerial hc05(RX_BLUETOOTH, TX_BLUETOOTH);

void initMpu() {
    while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G)) {
        exit(1);
    }

    mpu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);

    mpu.setIntFreeFallEnabled(false);
    mpu.setIntZeroMotionEnabled(false);
    mpu.setIntMotionEnabled(false);

    mpu.setDHPFMode(MPU6050_DHPF_5HZ);

    mpu.setMotionDetectionThreshold(8);
    mpu.setMotionDetectionDuration(2);

    mpu.setZeroMotionDetectionThreshold(4);
    mpu.setZeroMotionDetectionDuration(2);
}

void initBluetooth() {
    pinMode(RX_BLUETOOTH, INPUT);
    pinMode(TX_BLUETOOTH, OUTPUT);
    pinMode(STATE_BLUETOOTH, INPUT);

    hc05.begin(38400);
}

void initAlarm() {
    pinMode(ALARM_PIN, OUTPUT);
}

void setup() {
    initMpu();
    initBluetooth();
    initAlarm();
}

void loop() {
    Activites act = mpu.readActivites();

    if (act.isActivity) {
        hc05.write(0x01);
        digitalWrite(ALARM_PIN, HIGH);
        delay(ALARM_DURATION);
        digitalWrite(ALARM_PIN, LOW);
    }
}