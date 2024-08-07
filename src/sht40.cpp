#include <SensirionI2cSht4x.h>
#include <Wire.h>

SensirionI2cSht4x sensor;

static char errorMessage[64];
static int16_t error;

void setup_sensor() {

    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    Wire.begin();
    sensor.begin(Wire, SHT40_I2C_ADDR_44);

    sensor.softReset();
    delay(10);
    uint32_t serialNumber = 0;
    error = sensor.serialNumber(serialNumber);
    if (error != 0) {
        Serial.print("Error trying to execute serialNumber(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    Serial.print("serialNumber: ");
    Serial.print(serialNumber);
    Serial.println();
}

void loop_sensor() {
    float temperature = 0;
    float humidity = 0;
    error = sensor.measureHighPrecision(temperature, humidity);
    if (error != 0) {
        Serial.print("Error trying to execute measure(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    delay(1000);
}