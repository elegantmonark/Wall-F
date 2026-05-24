const uint8_t IR_SENSOR_PIN = 9;

void setup()
{
  Serial.begin(9600);
  pinMode(IR_SENSOR_PIN, INPUT);
}

void loop()
{
  int value = digitalRead(IR_SENSOR_PIN);

  Serial.print("IR raw: ");
  Serial.print(value);
  Serial.print(" | interpreted: ");
  Serial.println(value == LOW ? "BALL DETECTED" : "NO BALL");

  delay(150);
}

