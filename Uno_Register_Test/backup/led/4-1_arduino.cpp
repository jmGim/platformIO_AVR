#include <Arduino.h>



unsigned int count = 0;

void setup() {
    Serial.begin(9600);
    Serial.available();
    pinMode(LED_BUILTIN, OUTPUT);  // LED_BUILTIN = pin 13
}



void loop() {
    digitalWrite(13, HIGH);
    _delay_ms(1000);
    digitalWrite(13, LOW);
    _delay_ms(1000);

    Serial.println(count++);
    // _delay_ms(1000);

}

