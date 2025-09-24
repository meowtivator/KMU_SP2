#include <Arduino.h>
#define LED_PIN 7

int duty = 0;
int direction = 1;
int period_us = 1000;  // 1ms PWM 주기 (조절 가능)

unsigned long lastDutyUpdateTime = 0;
const unsigned long dutyStepDelay = 5;  // 5ms 간격으로 duty 1씩 변화 → 1초 삼각파

unsigned long lastPlotTime = 0;
const unsigned long plotInterval = 20;  // 플로터에 20ms 간격으로 출력

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial);
}

void pwm_cycle() {
  int onTime = period_us * duty / 100;
  int offTime = period_us - onTime;

  digitalWrite(LED_PIN, HIGH);
  delayMicroseconds(onTime);
  digitalWrite(LED_PIN, LOW);
  delayMicroseconds(offTime);
}

void set_duty(int d) {
  duty = constrain(d, 0, 100);
}

void set_period(int p) {
  period_us = constrain(p, 100, 10000);
}

void loop() {
  unsigned long now = millis();

  // duty 갱신 (5ms 간격)
  if (now - lastDutyUpdateTime >= dutyStepDelay) {
    duty += direction;
    if (duty >= 100) {
      duty = 100;
      direction = -1;
    } else if (duty <= 0) {
      duty = 0;
      direction = 1;
    }
    set_duty(duty);
    lastDutyUpdateTime = now;
  }

  // 시리얼 플로터 출력 (20ms 간격)
  if (now - lastPlotTime >= plotInterval) {
    Serial.print("duty: ");
    Serial.println(duty);
    lastPlotTime = now;
  }

  pwm_cycle();
}
