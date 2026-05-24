const uint8_t DIR_A_PIN = 2;
const uint8_t PWM_A_PIN = 3;
const uint8_t DIR_B_PIN = 4;
const uint8_t PWM_B_PIN = 5;

// Replace xxxx after timing turns on the final surface with fresh batteries.
const int TURN_SPEED = xxxx;
const unsigned long TURN_90_MS = xxxx;
const unsigned long TURN_180_MS = xxxx;
const unsigned long TURN_360_MS = xxxx;

const bool LEFT_FORWARD_DIR = HIGH;
const bool LEFT_BACKWARD_DIR = LOW;
const bool RIGHT_FORWARD_DIR = HIGH;
const bool RIGHT_BACKWARD_DIR = LOW;

void setMotorOutputs(bool dirA, int pwmA, bool dirB, int pwmB)
{
  digitalWrite(DIR_A_PIN, dirA);
  digitalWrite(DIR_B_PIN, dirB);
  analogWrite(PWM_A_PIN, constrain(pwmA, 0, 255));
  analogWrite(PWM_B_PIN, constrain(pwmB, 0, 255));
}

void stopMotors()
{
  analogWrite(PWM_A_PIN, 0);
  analogWrite(PWM_B_PIN, 0);
}

void spinRight(int pwm)
{
  setMotorOutputs(LEFT_FORWARD_DIR, pwm, RIGHT_BACKWARD_DIR, pwm);
}

void spinLeft(int pwm)
{
  setMotorOutputs(LEFT_BACKWARD_DIR, pwm, RIGHT_FORWARD_DIR, pwm);
}

void timedTurnRight(unsigned long durationMs)
{
  unsigned long start = millis();
  while (millis() - start < durationMs)
  {
    spinRight(TURN_SPEED);
  }
  stopMotors();
}

void setup()
{
  pinMode(DIR_A_PIN, OUTPUT);
  pinMode(PWM_A_PIN, OUTPUT);
  pinMode(DIR_B_PIN, OUTPUT);
  pinMode(PWM_B_PIN, OUTPUT);
  stopMotors();
  delay(3000);
}

void loop()
{
  timedTurnRight(TURN_90_MS);
  delay(3000);

  timedTurnRight(TURN_180_MS);
  delay(3000);

  timedTurnRight(TURN_360_MS);
  delay(5000);
}

