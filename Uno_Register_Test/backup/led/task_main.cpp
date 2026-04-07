// - **우노(AVR) 환경에서의 장점:**
// - 비선점형(Cooperative) 방식이므로 메모리(SRAM)가 2KB에 불과한 우노에서도 오버헤드 없이 매우 가볍게 동작함.
// - FreeRTOS 같은 무거운 운영체제를 올리기 부담스러운 8비트 환경에서 `delay()` 없는 다중 타이밍 제어(예: 여러 개의 LED 독립 제어, 주기적인 센서 읽기)를 구현할 때 최적의 선택임.
// RAM or Flash Consumption <= 50% 일때 사용

#include <Arduino.h>
#include <TaskScheduler.h>

Scheduler runner;

// 태스크 콜백 함수 선언
void blinkLED1();
void blinkLED2();
void blinkLED3();

// 태스크 객체 생성 (주기ms, 반복횟수, 콜백함수, 스케줄러포인터, 자동시작)
Task task1(500, TASK_FOREVER, &blinkLED1, &runner, true); // 1초 주기
Task task2(300, TASK_FOREVER, &blinkLED2, &runner, true);  // 0.3초 주기
Task task3(1500, TASK_FOREVER, &blinkLED3, &runner, true);  // 0.3초 주기

void blinkLED1() {
    digitalWrite(13, !digitalRead(13)); // 내장 LED 토글
}

void blinkLED2() {
    digitalWrite(12, !digitalRead(12)); // 외부 LED 토글
}

void blinkLED3() {
    digitalWrite(11, !digitalRead(11)); // 외부 LED 토글
}

void setup() {
    pinMode(13, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(11, OUTPUT);
    runner.startNow(); // 스케줄러 시작
}

void loop() {
    runner.execute(); // 스케줄러 실행 루프 (delay 사용 금지)
}                                                                                                                     



//  "