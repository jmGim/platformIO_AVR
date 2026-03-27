

![LED_problem_with_resistor_value](./LED_problem_with_resistor_value.png)

- Standard LEDs only need 10–20mA, and most low-power devices cannot handle 200mA.

- The LED toggle system fails with a 10-ohm resistor because it allows too much current to flow, likely causing the power supply to trigger protection mechanisms (like thermal shutdown or voltage drop),


-------------------------------------------------------------------------
## SoftwareSerial 을 통한 상대방 LED 제어시 유의 
- 상대방 혹은 자신의 Serial에만 write 해야함. 
- 둘 다 동시에 Write 하면 연속해서 값을 읽으려고 함.


