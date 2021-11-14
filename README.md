# 17keynumpad
board pinout
| pin/pin | B5(9) | (B6(10) | B2(16) | B3(14) 
|-------|:-------:|----:|--------:|--------:|
| B4(8) |backspace|  / |    *    |     -   |
|E6(7)  |   7     |  8 |    9    |    N/A  |
| D7(6) |   4     |  5 |    6    |     +   |
| C6(5) |   1     |  2 |    3    |    N/A  |
| D4(4) |  N/A    |  0 |    .    |   Enter |
 
LCD I2C 16x2
| Arduino | I2C|
|----------|----------|
| GND   | GND|
| VCC   | VCC|
| 2  | SDA|
| 3  | SCL|

  pcb, 17 1n4148 diodes, 3x2U pcb stabilizers, pro micro, 16x2 lcd screen, 2 position switch, 17 switches, 17 keycaps, tp4056, lipo 800/600 mAh battery, 4 m2 screws, 4 m3 screws, 4 orings
  
  enter - first press - shows result
        - second press - repeat the operation...
        - changes value of "left" to result of the operation
  del / num - press - deletes last pressed character
            - press after the operation changes value of "left" to result of the operation and deletes last character
            - hold - changes mode saves left, middle, right
            - hold after the operation changes value of "left" to result of the operation and changes mode
  DISPLAY
  arduino 32 bit float  - rounded to 2 decimal places
  display up max        - 12 + 2 + 2 (numbers + operation + dot)
  display down          - 14 + 1 + 1 (result, numbers + positive/negative + dot)
