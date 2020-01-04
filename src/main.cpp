#include <Arduino.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>

#define RX_BLUETOOTH 2
#define TX_BLUETOOTH 3

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

    mpu.setMotionDetectionThreshold(2);
    mpu.setMotionDetectionDuration(5);

    mpu.setZeroMotionDetectionThreshold(4);
    mpu.setZeroMotionDetectionDuration(2);
}

void initBluetooth() {
    pinMode(RX_BLUETOOTH, INPUT);
    pinMode(TX_BLUETOOTH, OUTPUT);

    hc05.begin(9600);
}

void setup() {
    initMpu();
    initBluetooth();
}

void loop() {
    Activites act = mpu.readActivites();

    if (act.isActivity) {
        hc05.write(0x01);
    }
}