# WALL-F Wiring

This document describes the final wiring used by `finalcode/FINALTESTING/FINALTESTING.ino`.

## Schematic

The final circuit schematic is available as a PNG preview:

```text
hardware/schematic/wall-f-schematic.png
```

The PDF reference extracted from the submitted final report is also included:

```text
hardware/schematic/wall-f-schematic-from-report-page.pdf
```

![WALL-F schematic](hardware/schematic/wall-f-schematic.png)

The schematic matches the final report circuit diagram and should be read together with the pin mapping below. Editable KiCad source files will be added under `hardware/kicad/` once they are exported and checked.

## Arduino Pin Map

| Net | Arduino Pin | Module | Notes |
|---|---|---|---|
| MOTOR_A_DIR | D2 | MKRVRS motor driver | Direction input |
| MOTOR_A_PWM | D3 | MKRVRS motor driver | PWM / enable input |
| MOTOR_B_DIR | D4 | MKRVRS motor driver | Direction input |
| MOTOR_B_PWM | D5 | MKRVRS motor driver | PWM / enable input |
| SERVO_RIGHT_SIG | D6 | Right clamp servo | Servo signal |
| SERVO_LEFT_SIG | D7 | Left clamp servo | Servo signal |
| US_TRIG | D8 | HC-SR04 ultrasonic | Trigger output |
| IR_OUT | D9 | TCRT5000 IR sensor | Active LOW in final code |
| PIXY_SPI_SS | D10 | PixyCam 2.1 | SPI chip select |
| PIXY_SPI_MOSI | D11 | PixyCam 2.1 | SPI MOSI |
| PIXY_SPI_MISO | D12 | PixyCam 2.1 | SPI MISO |
| PIXY_SPI_SCK | D13 | PixyCam 2.1 | SPI clock |
| US_ECHO | A2 | HC-SR04 ultrasonic | Echo input, used as digital input |
| I2C_SDA | A4 | 16x2 I2C LCD | I2C data |
| I2C_SCL | A5 | 16x2 I2C LCD | I2C clock |

## Motor Driver Notes

The final code names the two motor output groups as A and B:

```cpp
const uint8_t DIR_A_PIN = 2;
const uint8_t PWM_A_PIN = 3;
const uint8_t DIR_B_PIN = 4;
const uint8_t PWM_B_PIN = 5;
```

Before publishing a final schematic, verify the physical left/right motor labels against the code's forward, backward, and spin helpers. If the robot drives correctly with the final code, preserve the physical wiring and document the channel mapping from the tested build.

## Power Architecture

Final build power sources:

| Rail | Source | Used For |
|---|---|---|
| VBAT_UNO_9V | 9 V alkaline battery | Arduino Uno barrel input |
| VBAT_MOTOR_9V | 9 V alkaline battery | MKRVRS motor supply |
| V_SERVO_AAA | 4xAAA alkaline pack | FS90MG servo power |
| V_LOGIC_5V | Arduino / regulated 5 V rail | PixyCam, LCD, logic modules |
| GND_COMMON | Shared ground | All modules |

All grounds must be common between the Arduino, motor driver, PixyCam, LCD, ultrasonic sensor, IR sensor, and servo battery negative.

## Capacitors

The final design used two 1000 uF capacitors to reduce voltage dips:

- One across the Arduino 9 V supply terminals.
- One across a main power rail for transient smoothing.

Show these in the schematic as labelled components, for example `C1 1000uF` and `C2 1000uF`.

## PixyCam SPI Warning

The PixyCam uses Arduino SPI pins. Avoid assigning other devices to D10-D13 unless they are intentionally sharing the SPI bus.

The final design moved:

- Ultrasonic TRIG to D8
- Ultrasonic ECHO to A2
- IR sensor output to D9

This avoids conflicts with the PixyCam SPI connection.

## Servo Supply Warning

The two clamp servos should not be powered directly from the Arduino 5 V pin. The final build used a separate 4xAAA pack for the servo rail, with the servo ground tied to common ground.
