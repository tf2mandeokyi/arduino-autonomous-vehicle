# 아두이노 자율주행 자동차

2023년 2학기 창의적공학설계 팀 프로젝트

## 파일 목록

| 파일 | 설명 |
|-|-|
| `arduino-autonomous-vehicle.ino` | 메인 소스 파일 |
| `header.h` | 클래스 헤더 파일 |
| `ir-receiver.ino` | IR 리모컨 수신기 관련 코드 |
| `line-tracker.ino` | 라인트랙킹 센서 관련 코드 |
| `motor.ino` | 모터 관련 코드 (L293D) |
| `ultra-sonic.ino` | 초음파 센서 관련 코드 |

## 필수 라이브러리

- [Adafruit Motor Shield library](https://github.com/adafruit/Adafruit-Motor-Shield-library) (1.0.1) by Adafruit
- [IRremote](https://github.com/Arduino-IRremote/Arduino-IRremote) (4.2.0) by shirriff, z3t0, ArminJo
- [NewPing](https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home) (1.9.7) by Tim Eckel
