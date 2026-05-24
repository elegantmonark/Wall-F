#include <Servo.h>

const uint8_t SERVO_RIGHT_PIN = 6;
const uint8_t SERVO_LEFT_PIN = 7;

// Replace xxxx values after testing on the physical clamp.
const int SERVO_RIGHT_OPEN = xxxx;
const int SERVO_RIGHT_CLOSED = xxxx;
const int SERVO_LEFT_OPEN = xxxx;
const int SERVO_LEFT_CLOSED = xxxx;

Servo servoRight;
Servo servoLeft;

void setup()
{
  servoRight.attach(SERVO_RIGHT_PIN);
  servoLeft.attach(SERVO_LEFT_PIN);
}

void loop()
{
  servoRight.write(SERVO_RIGHT_OPEN);
  servoLeft.write(SERVO_LEFT_OPEN);
  delay(1500);

  servoRight.write(SERVO_RIGHT_CLOSED);
  servoLeft.write(SERVO_LEFT_CLOSED);
  delay(1500);
}

