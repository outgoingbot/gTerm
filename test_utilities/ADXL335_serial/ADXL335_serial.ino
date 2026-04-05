// ADXL335 Accelerometer - Raw to g (float) output at 115200 baud
// Format: X.XXX,Y.YYY,Z.ZZZ\r\n

#include <Arduino.h>

const int xPin = A0;
const int yPin = A1;
const int zPin = A2;

// ADXL335 calibration constants (adjust these if needed after testing)
const float zeroG = 512.0;     // 0g ≈ 1.65V → 512 on 10-bit ADC (5V)
const float scale  = 0.0039;   // ≈ 3.9mV/g → 0.0039 V per g (at 5V)

char bufX[12];
char bufY[12];
char bufZ[12];
char outBuf[40];

void setup() {
    Serial.begin(115200);
    while (!Serial);
    //Serial.println("ADXL335 Ready");
}

void loop() {
    int xRaw = analogRead(xPin);
    int yRaw = analogRead(yPin);
    int zRaw = analogRead(zPin);

    float xG = (xRaw - zeroG) * scale;
    float yG = (yRaw - zeroG) * scale;
    float zG = (zRaw - zeroG) * scale;

    dtostrf(xG, 0, 3, bufX);
    dtostrf(yG, 0, 3, bufY);
    dtostrf(zG, 0, 3, bufZ);

    snprintf(outBuf, sizeof(outBuf), "%s,%s,%s\r\n", bufX, bufY, bufZ);
    Serial.print(outBuf);

    delay(10);   // ~20 Hz update rate - change if you need faster/slower
}
