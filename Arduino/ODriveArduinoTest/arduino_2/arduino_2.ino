// Arduino 2 code with functions:
// * accelerometer 
// * Serial output to computer

// acc/gyro
#include "BMI088.h"
float ax = 0, ay = 0, az = 0;
float gx = 0, gy = 0, gz = 0;
int16_t temp = 0;

void setup(void)
{
    Wire.begin();
    Serial.begin(115200);
    while(!Serial);
    Serial.println("BMI088 Raw Data");
    while(1)
    {
        if(bmi088.isConnection())
        {
            bmi088.initialize();
            Serial.println("BMI088 is connected");
            break;
        }
        else Serial.println("BMI088 is not connected");
        delay(2000);
    }
}
 
float sensor_read(){
    //read all sensors, temp, encoder, accelerometer/gyro etc. (Odrive for encoder)
    //Serial.println("sensor_read call workes");
    bmi088.getAcceleration(&ax, &ay, &az);
    bmi088.getGyroscope(&gx, &gy, &gz);
    temp = bmi088.getTemperature();

    ax = ax/(9.82*100);
    ay = ay/(9.82*100);
    az = az/(9.82*100);

    Serial.print(ax); //N
    Serial.print(",");
    Serial.print(ay); //N
    Serial.print(",");
    Serial.print(az); //N
    Serial.print(",");
    return ax, ay, az;
}
