#include "Arduino.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file
#include "Wire.h"

MPU6050 mpu;

#define LED_PIN 13
#define MOVEMENT_SENSIBILITY 50

void triggerMpuInterruption();
void exitOnError(const char *message);

uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorInt16 aaWorldPrevious;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

volatile bool mpuDidInterrupt = false;

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing I2C devices...");

    Wire.begin();
    Wire.setClock(400000);

    mpu.initialize();
    if (!mpu.testConnection()) {
        exitOnError("MPU6050 Connection failed");
    }

    if (mpu.dmpInitialize() > 0) {
        exitOnError("MPU6050 Initialization failed");
    }

    packetSize = mpu.dmpGetFIFOPacketSize();

    mpu.setXGyroOffset(1);
    mpu.setYGyroOffset(-70);
    mpu.setZGyroOffset(39);
    mpu.setZAccelOffset(964);
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);

    mpu.setDMPEnabled(true);

    pinMode(2, INPUT);
    attachInterrupt(digitalPinToInterrupt(2), triggerMpuInterruption, RISING);

    Serial.println("DMP ready! Waiting for first interrupt...");

    pinMode(LED_PIN, OUTPUT);
}

uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
void loop() {
    Serial.println("loop");

    while (!mpuDidInterrupt) {
        Serial.println("Waiting");
    }

    mpuDidInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    fifoCount = mpu.getFIFOCount();
    if (fifoCount < packetSize) {
        Serial.println("if1");
        return; // Buffer does not contain valid data, waiting next event
    }

    if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
        mpu.resetFIFO(); // FIFO Overflow
        Serial.println("if2");
        return;
    }

    if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
        Serial.println("if3");
        while (fifoCount >= packetSize) { // Get all the data available in the buffer (Manage 2 events at same time)
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            fifoCount -= packetSize;
        }

        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetAccel(&aa, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
        mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);

        bool shouldDisplayData = (
                abs(aaWorld.x - aaWorldPrevious.x) > MOVEMENT_SENSIBILITY ||
                abs(aaWorld.y - aaWorldPrevious.y) > MOVEMENT_SENSIBILITY ||
                abs(aaWorld.z - aaWorldPrevious.z) > MOVEMENT_SENSIBILITY
        );

        aaWorldPrevious = aaWorld;

        if (shouldDisplayData) {
            Serial.print("aworld\t");
            Serial.print(aaWorld.x);
            Serial.print("\t");
            Serial.print(aaWorld.y);
            Serial.print("\t");
            Serial.println(aaWorld.z);
        }
    }
}

void triggerMpuInterruption() {
    mpuDidInterrupt = true;
}

void exitOnError(const char *message) {
    Serial.println(message);
    delay(1000);
    exit(1);
}