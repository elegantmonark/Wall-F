const uint8_t ULTRASONIC_TRIG_PIN = 8;
const uint8_t ULTRASONIC_ECHO_PIN = A2;

// Replace xxxx with the obstacle threshold selected after testing.
const int OBSTACLE_CM = xxxx;
const int ULTRA_TIMEOUT_US = 25000;

float readUltrasonicCm()
{
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, ULTRA_TIMEOUT_US);
  if (duration == 0)
  {
    return 400.0f;
  }

  return duration * 0.0343f / 2.0f;
}

void setup()
{
  Serial.begin(9600);
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
}

void loop()
{
  float distance = readUltrasonicCm();
  Serial.print("Distance cm: ");
  Serial.print(distance);
  Serial.print(" | Threshold cm: ");
  Serial.println(OBSTACLE_CM);
  delay(250);
}

