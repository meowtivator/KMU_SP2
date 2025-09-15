#include <Arduino.h>
#define PIN_LED 7
void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  while(!Serial){
    ;
  }
}

void loop() {
  digitalWrite(PIN_LED, LOW);
  delay(1000);
  int cnt{};
  while(cnt < 5){
    digitalWrite(PIN_LED, HIGH);
    delay(100);
    digitalWrite(PIN_LED, LOW);
    delay(100);
    cnt++;
  }
  digitalWrite(PIN_LED, HIGH);
  while(1){
    ;
  }
}
