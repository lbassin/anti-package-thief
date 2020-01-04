#include <Arduino.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup()
{
    Serial.begin(115200);

    Serial.println("Initialize MPU6050");

    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G))
    {
        Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
        delay(500);
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

void loop()
{
    Activites act = mpu.readActivites();

    if (act.isActivity)
    {
        Serial.println("Moved");
    }
}
