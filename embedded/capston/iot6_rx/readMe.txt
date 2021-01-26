Timer, Character LCD, UART 개발 완료.
irRemote는 Receiver 부분 작성 중.
USART의 UDRIE0가 계속 발생하여 Timer3의 출력 비교 ISR 이 호출되지 못하는 문제 해결.
Timer3를 켜고 irReceiver를 작동시키면 인식 안되는 문제 발생.
