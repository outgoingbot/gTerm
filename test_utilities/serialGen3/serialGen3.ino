#include <Arduino.h>
#include <math.h>

const float stepSize = 0.01;
const float twoPi = 2 * PI;
const float amplitude = 10.0;

char bufSin[12];
char bufSq[12];
char bufSaw[12];
char outBuf[40];   // enough for one full line

void setup() {
    Serial.begin(115200);
    while (!Serial);
}

void loop() {
    for (float x = 0; x <= twoPi; x += stepSize) {

        float sineValue = amplitude * sin(x);
        float squareValue = (sin(x+90) >= 0.0f) ? amplitude : -amplitude;
        float sawValue = ((x / twoPi) * 2.0f * amplitude) - amplitude;

        dtostrf(sineValue, 0, 3, bufSin);
        dtostrf(squareValue, 0, 3, bufSq);
        dtostrf(sawValue, 0, 3, bufSaw);

        // Build one line manually: sin,square,saw\r\n
        snprintf(outBuf, sizeof(outBuf), "%s,%s,%s\r\n", bufSin, bufSq, bufSaw);

        // One single serial write
        Serial.print(outBuf);
        //delay(5);
    }
}
