#include <Pixy2.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ============================================================
// ---------------------- USER SETTINGS ------------------------
// ============================================================

// ---------- LCD ----------
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// ---------- Pins ----------
const uint8_t DIR_A_PIN   = 2;
const uint8_t PWM_A_PIN   = 3;
const uint8_t DIR_B_PIN   = 4;
const uint8_t PWM_B_PIN   = 5;

const uint8_t SERVO_RIGHT_PIN = 6;   // user requested: right clamp on pin 6
const uint8_t SERVO_LEFT_PIN  = 7;   // user requested: left clamp on pin 7

const uint8_t ULTRASONIC_TRIG_PIN = 8;
const uint8_t IR_SENSOR_PIN       = 9;
const uint8_t ULTRASONIC_ECHO_PIN = A2;

const uint8_t LCD_SDA_PIN = A4;   // hardware I2C on Uno
const uint8_t LCD_SCL_PIN = A5;   // hardware I2C on Uno

// ---------- Servo angles ----------
const int SERVO_RIGHT_OPEN   = 0;    // pin 6
const int SERVO_RIGHT_CLOSED = 35;
const int SERVO_LEFT_OPEN    = 180;  // pin 7 (mirrored)
const int SERVO_LEFT_CLOSED  = 145;

// ---------- Pixy signatures ----------
const uint8_t SIG_RED    = 1;
const uint8_t SIG_GREEN  = 2;
const uint8_t SIG_BLUE   = 3;
const uint8_t SIG_BASE_1 = 4;  // orange
const uint8_t SIG_BASE_2 = 5;  // yellow
const uint8_t SIG_BASE_3 = 6;  // pink
const uint8_t SIG_BASE_4 = 7;  // purple

// ---------- Motion tuning ----------
const int TRAVEL_SPEED  = 100;
const int BASE_APPROACH = 90;
const int INCH_SPEED    = 70;
const int DEPOSIT_INCH_SPEED = 60;
const int DEPOSIT_PULLBACK_SPEED = INCH_SPEED;
const int TURN_SPEED    = 100;
const int CENTRE_SPEED  = 55;

// ---------- Turn timing ----------               v
const unsigned long TURN_180_MS = 1800;                 // user-specified
const unsigned long TURN_360_MS = TURN_180_MS * 1;      // derived
const unsigned long TURN_90_MS  = TURN_180_MS / 2;
const unsigned long POST_DEPOSIT_TURN_180_MS = TURN_180_MS;

// ---------- Ultrasonic / obstacle ----------
const int OBSTACLE_CM = 10;
const int ULTRA_TIMEOUT_US = 25000; // ~4m max
const uint8_t ULTRA_SAMPLES = 9;
const uint8_t ULTRA_SMOOTH_WINDOW = 3;
const unsigned long OBSTACLE_WAIT_CLEAR_MS = 3000;
const unsigned long OBSTACLE_REVERSE_MS = 320;
const unsigned long OBSTACLE_FORWARD_AFTER_TURN_MS = 1000;
const unsigned long DEPOSIT_INCH_FORWARD_MS = 3000;
const float BASE_DEPOSIT_CM = 4.0f;
const unsigned long BASE_DEPOSIT_TIMEOUT_MS = 7000;

// ---------- Pixy tuning ----------
const int PIXY_FRAME_CENTER_X = 158;  // approx center of Pixy2 316px width
const int CENTER_DEADBAND     = 12;   // centered if abs(error) <= this
const int CENTER_LOCK_FRAMES  = 5;    // how many centered frames before "locked"
const float STEER_KP          = 0.45; // proportional steering gain

// ---------- Search timing ----------
const unsigned long SEARCH_FORWARD_TIMEOUT_MS = 13000; // 10-15 seconds target
const unsigned long SEARCH_SWEEP_INTERVAL_MS  = 2600;
const unsigned long SEARCH_FORWARD_STEP_MS    = 900;
const unsigned long SEARCH_FORWARD_SLICE_MS   = 60;
const unsigned long POST_TURN_PUSH_MS         = 900;
const unsigned long CHASE_TIMEOUT_MS          = 25000;
const unsigned long RETURN_TIMEOUT_MS         = 25000;
const unsigned long BASE_LOG_TIMEOUT_MS       = 3000;
const unsigned long CENTER_TIMEOUT_MS         = 4000;
const unsigned long LOST_REACQUIRE_TIMEOUT_MS = 3500;
const unsigned long REVERSE_AFTER_DEPOSIT_MS = 2000;
const unsigned long STARTUP_REVERSE_MS = 2000;
const unsigned long SWEEP_CONFIRM_MS = 180;

// ---------- Base-size stop tuning ----------
// Robot stops when returned base appears close enough to startup view.
// Ratios below are intentionally a bit forgiving due to camera jitter.
const float BASE_WIDTH_RATIO_STOP  = 0.95;
const float BASE_HEIGHT_RATIO_STOP = 0.95;
const float BASE_AREA_RATIO_STOP   = 0.90;

// ---------- Misc ----------
const bool LEFT_FORWARD_DIR  = HIGH;
const bool LEFT_BACKWARD_DIR = LOW;
const bool RIGHT_FORWARD_DIR  = HIGH;
const bool RIGHT_BACKWARD_DIR = LOW;

// ============================================================
// ------------------- GLOBAL OBJECTS/STATE --------------------
// ============================================================

Pixy2 pixy;
Servo servoRight;
Servo servoLeft;
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

struct BlockInfo
{
  bool valid;
  uint8_t sig;
  int x;
  int y;
  int w;
  int h;
  long area;
};

uint8_t loggedBaseSignature = 0;
int loggedBaseWidth = 0;
int loggedBaseHeight = 0;
long loggedBaseArea = 0;
bool pixyPausedForUltrasonic = false;
bool isFacingArena = false;

bool fullSweep360UntilFound(uint8_t targetSig, const char* label, bool suppressObstacleBecauseTargetIsBase);
bool confirmSignatureVisible(uint8_t targetSig, unsigned long confirmMs);

// ============================================================
// ----------------------- LCD HELPERS -------------------------
// ============================================================

void lcdPrint2(const char *line1, const char *line2)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void lcdPrintSigLogged(uint8_t sig)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Base logged:");
  lcd.setCursor(0, 1);
  lcd.print(signatureName(sig));
}

const char* signatureName(uint8_t sig)
{
  switch (sig)
  {
    case SIG_RED:    return "RED (1)";
    case SIG_GREEN:  return "GREEN (2)";
    case SIG_BLUE:   return "BLUE (3)";
    case SIG_BASE_1: return "ORANGE (4)";
    case SIG_BASE_2: return "YELLOW (5)";
    case SIG_BASE_3: return "PINK (6)";
    case SIG_BASE_4: return "PURPLE (7)";
    default:         return "UNKNOWN";
  }
}

void setPixyPaused(bool paused)
{
  pixyPausedForUltrasonic = paused;

  // Force the Pixy lamp permanently OFF.
  // If setLamp is unsupported by firmware, behaviour degrades safely.
  pixy.setLamp(0, 0);
}

// ============================================================
// ---------------------- CLAMP CONTROL ------------------------
// ============================================================

void openClamp()
{
  servoRight.write(SERVO_RIGHT_OPEN);
  servoLeft.write(SERVO_LEFT_OPEN);
  delay(350);
}

void closeClamp()
{
  servoRight.write(SERVO_RIGHT_CLOSED);
  servoLeft.write(SERVO_LEFT_CLOSED);
  delay(350);
}

bool ballInClamp()
{
  // user specified: IR detects object in clamp
  // most FC-51 style outputs LOW on detect
  return (digitalRead(IR_SENSOR_PIN) == LOW);
}

// ============================================================
// ---------------------- MOTOR CONTROL ------------------------
// ============================================================

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

void driveForward(int pwm)
{
  setMotorOutputs(LEFT_FORWARD_DIR, pwm, RIGHT_FORWARD_DIR, pwm);
}

void driveBackward(int pwm)
{
  setMotorOutputs(LEFT_BACKWARD_DIR, pwm, RIGHT_BACKWARD_DIR, pwm);
}

void spinRight(int pwm)
{
  // in-place rotate right
  setMotorOutputs(LEFT_FORWARD_DIR, pwm, RIGHT_BACKWARD_DIR, pwm);
}

void spinLeft(int pwm)
{
  // in-place rotate left
  setMotorOutputs(LEFT_BACKWARD_DIR, pwm, RIGHT_FORWARD_DIR, pwm);
}

void steerForward(int leftPwm, int rightPwm)
{
  setMotorOutputs(LEFT_FORWARD_DIR, leftPwm, RIGHT_FORWARD_DIR, rightPwm);
}

void reverseBriefly(int pwm, unsigned long ms)
{
  driveBackward(pwm);
  delay(ms);
  stopMotors();
}

void timedTurn(bool turnRightDirection, unsigned long durationMs, int pwm, bool showLCD, const char* lcdLine2)
{
  if (showLCD)
  {
    lcdPrint2("Turning", lcdLine2);
  }

  unsigned long start = millis();
  while (millis() - start < durationMs)
  {
    if (turnRightDirection)
      spinRight(pwm);
    else
      spinLeft(pwm);

    delay(10);
  }
  stopMotors();
}

// ============================================================
// ------------------- ULTRASONIC / FILTER ---------------------
// ============================================================

float readUltrasonicCmRawPrecise()
{
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, ULTRA_TIMEOUT_US);
  if (duration == 0)
  {
    return 400.0f; // no echo -> treat as far away
  }

  float distanceCm = duration * 0.0343f / 2.0f;
  return distanceCm;
}

int readUltrasonicCmRaw()
{
  float distanceCm = readUltrasonicCmRawPrecise();
  return (int)distanceCm;
}

float medianOfArray(float *values, uint8_t count)
{
  for (uint8_t i = 0; i < count - 1; i++)
  {
    for (uint8_t j = 0; j < count - 1 - i; j++)
    {
      if (values[j] > values[j + 1])
      {
        float temp = values[j];
        values[j] = values[j + 1];
        values[j + 1] = temp;
      }
    }
  }

  return values[count / 2];
}

float medianDistanceCmPrecise()
{
  // Stage 1: burst median
  float readings[ULTRA_SAMPLES];
  for (uint8_t i = 0; i < ULTRA_SAMPLES; i++)
  {
    readings[i] = readUltrasonicCmRawPrecise();
    delay(random(8, 18)); // slight jitter reduces sync/crosstalk effects
  }
  float burstMedian = medianOfArray(readings, ULTRA_SAMPLES);

  // Stage 2: rolling median over recent burst medians
  static float history[ULTRA_SMOOTH_WINDOW];
  static uint8_t historyCount = 0;
  static uint8_t historyIndex = 0;

  history[historyIndex] = burstMedian;
  historyIndex = (historyIndex + 1) % ULTRA_SMOOTH_WINDOW;
  if (historyCount < ULTRA_SMOOTH_WINDOW)
    historyCount++;

  float temp[ULTRA_SMOOTH_WINDOW];
  for (uint8_t i = 0; i < historyCount; i++)
    temp[i] = history[i];

  return medianOfArray(temp, historyCount);
}

int medianDistanceCm()
{
  return (int)medianDistanceCmPrecise();
}

// IMPORTANT:
// During initial base logging and final base homing, the robot is intentionally
// facing / approaching the base. In those moments the home base itself would
// appear as an "obstacle" to the front ultrasonic sensor. So obstacle avoidance
// is bypassed only during those phases.
bool avoidObstacleIfNeeded(bool suppressBecauseTargetIsBase, uint8_t rescanSig = 0, const char* rescanLabel = "", bool allowRescan = true)
{
  if (suppressBecauseTargetIsBase)
  {
    return false;
  }

  int d = medianDistanceCm();
  if (d > OBSTACLE_CM)
  {
    return false;
  }

  stopMotors();
  lcdPrint2("Obstacle <=10cm", "Wait 3 sec");

  unsigned long waitStart = millis();
  while (millis() - waitStart < OBSTACLE_WAIT_CLEAR_MS)
  {
    delay(110);
    d = medianDistanceCm();
    if (d > OBSTACLE_CM)
    {
      stopMotors();
      delay(80);
      return true;
    }
  }

  // If still blocked after 3s, back up slightly.
  lcdPrint2("Obstacle stuck", "Reverse a bit");
  reverseBriefly(INCH_SPEED, OBSTACLE_REVERSE_MS);
  delay(80);

  // Then run a 360 sweep for the desired signature.
  bool foundDuringSweep = false;
  if (allowRescan && rescanSig != 0)
  {
    lcdPrint2("Obstacle scan", "360 target");
    unsigned long scanStart = millis();
    while (millis() - scanStart < TURN_360_MS + 300)
    {
      spinRight(TURN_SPEED);

      BlockInfo b;
      if (getLargestBlockBySignature(rescanSig, b))
      {
        stopMotors();
        if (confirmSignatureVisible(rescanSig, SWEEP_CONFIRM_MS))
        {
          foundDuringSweep = true;
          break;
        }
      }

      delay(25);
    }
    stopMotors();
  }

  if (foundDuringSweep)
  {
    return true;
  }

  // Not found: from original heading, turn 180 and move forward a bit.
  lcdPrint2("No target", "Turn 180");
  timedTurn(true, TURN_180_MS, TURN_SPEED, false, "");
  delay(120);

  lcdPrint2("Resume search", "Forward bit");
  driveForward(TRAVEL_SPEED);
  delay(OBSTACLE_FORWARD_AFTER_TURN_MS);
  stopMotors();
  delay(120);

  return true;
}

// ============================================================
// ----------------------- PIXY HELPERS ------------------------
// ============================================================

bool isBaseSignature(uint8_t sig)
{
  return (sig == SIG_BASE_1 || sig == SIG_BASE_2 || sig == SIG_BASE_3 || sig == SIG_BASE_4);
}

bool getLargestBlockBySignature(uint8_t signature, BlockInfo &out)
{
  out.valid = false;
  if (pixyPausedForUltrasonic)
    return false;

  uint16_t numBlocks = pixy.ccc.getBlocks();
  if (numBlocks == 0)
    return false;

  long bestArea = -1;

  for (uint16_t i = 0; i < numBlocks; i++)
  {
    auto &b = pixy.ccc.blocks[i];
    if (b.m_signature != signature)
      continue;

    long area = (long)b.m_width * (long)b.m_height;
    if (area > bestArea)
    {
      bestArea = area;
      out.valid = true;
      out.sig = b.m_signature;
      out.x = b.m_x;
      out.y = b.m_y;
      out.w = b.m_width;
      out.h = b.m_height;
      out.area = area;
    }
  }

  return out.valid;
}

bool getLargestBaseBlock(BlockInfo &out)
{
  out.valid = false;
  if (pixyPausedForUltrasonic)
    return false;

  uint16_t numBlocks = pixy.ccc.getBlocks();
  if (numBlocks == 0)
    return false;

  long bestArea = -1;

  for (uint16_t i = 0; i < numBlocks; i++)
  {
    auto &b = pixy.ccc.blocks[i];
    if (!isBaseSignature(b.m_signature))
      continue;

    long area = (long)b.m_width * (long)b.m_height;
    if (area > bestArea)
    {
      bestArea = area;
      out.valid = true;
      out.sig = b.m_signature;
      out.x = b.m_x;
      out.y = b.m_y;
      out.w = b.m_width;
      out.h = b.m_height;
      out.area = area;
    }
  }

  return out.valid;
}

// ============================================================
// -------------------- BASE LOG / RETURN ----------------------
// ============================================================

bool logBaseSignatureAtStart()
{
  lcdPrint2("Starting WALL-F", "Scan base...");
  unsigned long start = millis();

  BlockInfo best;
  best.valid = false;
  best.area = -1;

  while (millis() - start < BASE_LOG_TIMEOUT_MS)
  {
    BlockInfo current;
    if (getLargestBaseBlock(current))
    {
      if (!best.valid || current.area > best.area)
      {
        best = current;
      }
    }
    delay(35);
  }

  if (!best.valid)
  {
    lcdPrint2("Base scan fail", "No signature");
    return false;
  }

  loggedBaseSignature = best.sig;
  loggedBaseWidth = best.w;
  loggedBaseHeight = best.h;
  loggedBaseArea = best.area;

  lcdPrintSigLogged(loggedBaseSignature);
  delay(1200);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Base size:");
  lcd.setCursor(0, 1);
  lcd.print(best.w);
  lcd.print("x");
  lcd.print(best.h);
  delay(1200);

  return true;
}

bool reachedHomeSize(const BlockInfo &b)
{
  bool widthReached  = (b.w >= (int)(loggedBaseWidth  * BASE_WIDTH_RATIO_STOP));
  bool heightReached = (b.h >= (int)(loggedBaseHeight * BASE_HEIGHT_RATIO_STOP));
  bool areaReached   = (b.area >= (long)(loggedBaseArea * BASE_AREA_RATIO_STOP));

  // Require either both dimensions, or area as backup.
  return ((widthReached && heightReached) || areaReached);
}

bool centerOnSignature(uint8_t targetSig, const char* label, unsigned long timeoutMs, bool suppressObstacleBecauseTargetIsBase)
{
  lcdPrint2("Centering", label);

  unsigned long start = millis();
  uint8_t lockedFrames = 0;
  int lastError = 0;

  while (millis() - start < timeoutMs)
  {
    if (avoidObstacleIfNeeded(suppressObstacleBecauseTargetIsBase, targetSig, label, true))
    {
      lockedFrames = 0;
      continue;
    }

    BlockInfo b;
    if (getLargestBlockBySignature(targetSig, b))
    {
      int error = b.x - PIXY_FRAME_CENTER_X;
      lastError = error;

      if (abs(error) <= CENTER_DEADBAND)
      {
        stopMotors();
        lockedFrames++;
        if (lockedFrames >= CENTER_LOCK_FRAMES)
        {
          stopMotors();
          return true;
        }
      }
      else
      {
        lockedFrames = 0;
        int turnPwm = constrain(CENTRE_SPEED + abs(error) / 4, 40, 110);

        if (error > 0)
          spinRight(turnPwm);
        else
          spinLeft(turnPwm);
      }
    }
    else
    {
      lockedFrames = 0;
      if (lastError >= 0)
        spinRight(CENTRE_SPEED);
      else
        spinLeft(CENTRE_SPEED);
    }

    delay(35);
  }

  stopMotors();
  return false;
}

bool confirmSignatureVisible(uint8_t targetSig, unsigned long confirmMs)
{
  unsigned long start = millis();
  while (millis() - start < confirmMs)
  {
    BlockInfo b;
    if (!getLargestBlockBySignature(targetSig, b))
      return false;
    delay(20);
  }
  return true;
}

bool timedSpinUntilFound(uint8_t targetSig, bool turnRightDirection, unsigned long durationMs, const char* lcdLine2)
{
  lcdPrint2("Scanning", lcdLine2);

  unsigned long start = millis();
  while (millis() - start < durationMs)
  {
    if (avoidObstacleIfNeeded(false, targetSig, lcdLine2, true))
    {
      start = millis();
      continue;
    }

    if (turnRightDirection)
      spinRight(TURN_SPEED);
    else
      spinLeft(TURN_SPEED);

    BlockInfo b;
    if (getLargestBlockBySignature(targetSig, b))
    {
      stopMotors();
      if (confirmSignatureVisible(targetSig, SWEEP_CONFIRM_MS))
      {
        return true;
      }
    }

    delay(25);
  }

  stopMotors();
  return false;
}

void timedSpinNoSearch(bool turnRightDirection, unsigned long durationMs, const char* lcdLine2)
{
  lcdPrint2("Turning", lcdLine2);

  unsigned long start = millis();
  while (millis() - start < durationMs)
  {
    if (turnRightDirection)
      spinRight(TURN_SPEED);
    else
      spinLeft(TURN_SPEED);

    delay(25);
  }

  stopMotors();
}

bool sweep180Search(uint8_t targetSig, const char* label)
{
  // Right 90 deg
  if (timedSpinUntilFound(targetSig, true, TURN_90_MS, label))
    return true;

  // Left 180 deg
  if (timedSpinUntilFound(targetSig, false, TURN_180_MS, label))
    return true;

  // Return to original heading: Right 90 deg
  timedSpinNoSearch(true, TURN_90_MS, label);
  return false;
}

bool fullSweep360UntilFound(uint8_t targetSig, const char* label, bool suppressObstacleBecauseTargetIsBase)
{
  lcdPrint2("360 sweep", label);
  unsigned long start = millis();

  while (millis() - start < TURN_360_MS + 300)
  {
    avoidObstacleIfNeeded(suppressObstacleBecauseTargetIsBase, targetSig, label, true);
    spinRight(TURN_SPEED);

    BlockInfo b;
    if (getLargestBlockBySignature(targetSig, b))
    {
      stopMotors();
      if (confirmSignatureVisible(targetSig, SWEEP_CONFIRM_MS))
      {
        return true;
      }
    }

    delay(25);
  }

  stopMotors();
  return false;
}

// ============================================================
// -------------------- BALL SEARCH / CHASE --------------------
// ============================================================

bool searchForBall(uint8_t targetSig, const char* label)
{
  // First check if already in view
  BlockInfo b;
  if (getLargestBlockBySignature(targetSig, b))
  {
    stopMotors();
    return true;
  }

  lcdPrint2("Search mode", "Forward bias");
  unsigned long start = millis();
  unsigned long lastSweep = millis();
  uint8_t missedSweeps = 0;

  while (millis() - start < SEARCH_FORWARD_TIMEOUT_MS)
  {
    if (avoidObstacleIfNeeded(false, targetSig, label, true))
    {
      lastSweep = millis();
      continue;
    }

    // If ball appears, stop immediately
    if (getLargestBlockBySignature(targetSig, b))
    {
      stopMotors();
      return true;
    }

    lcdPrint2("Searching", "Drive forward");
    unsigned long stepStart = millis();
    while ((millis() - stepStart < SEARCH_FORWARD_STEP_MS) && (millis() - start < SEARCH_FORWARD_TIMEOUT_MS))
    {
      if (avoidObstacleIfNeeded(false, targetSig, label, true))
      {
        lastSweep = millis();
        break;
      }

      if (getLargestBlockBySignature(targetSig, b))
      {
        stopMotors();
        return true;
      }

      driveForward(TRAVEL_SPEED);
      delay(SEARCH_FORWARD_SLICE_MS);
    }

    stopMotors();
    delay(60);

    if (getLargestBlockBySignature(targetSig, b))
    {
      stopMotors();
      return true;
    }

    if (millis() - lastSweep >= SEARCH_SWEEP_INTERVAL_MS)
    {
      lcdPrint2("Search sweep", "180 deg");
      if (sweep180Search(targetSig, label))
      {
        stopMotors();
        return true;
      }

      lastSweep = millis();
      missedSweeps++;
      if (missedSweeps >= 2)
      {
        lcdPrint2("Search sweep", "360 recover");
        if (fullSweep360UntilFound(targetSig, label, false))
        {
          stopMotors();
          return true;
        }
        missedSweeps = 0;
      }
    }
  }

  stopMotors();
  return false;
}
bool chaseBallAndClamp(uint8_t targetSig, const char* label)
{
  lcdPrint2("Chasing ball", label);

  unsigned long start = millis();
  unsigned long lastSeen = millis();
  int lastError = 0;

  while (millis() - start < CHASE_TIMEOUT_MS)
  {
    if (avoidObstacleIfNeeded(false, targetSig, label, true))
    {
      lastSeen = millis();
      continue;
    }

    if (ballInClamp())
    {
      stopMotors();
      lcdPrint2("Ball detected", "Closing clamp");
      closeClamp();
      delay(250);
      return true;
    }

    BlockInfo b;
    if (getLargestBlockBySignature(targetSig, b))
    {
      lastSeen = millis();
      int error = b.x - PIXY_FRAME_CENTER_X;
      lastError = error;

      // adaptive forward speed:
      // large block => slow inch
      int baseSpeed = (b.area > 2800) ? INCH_SPEED : TRAVEL_SPEED;

      int correction = (int)(STEER_KP * error);
      int leftPwm  = constrain(baseSpeed + correction, 20, 110);
      int rightPwm = constrain(baseSpeed - correction, 20, 110);

      // if nearly centered, go straighter
      if (abs(error) <= CENTER_DEADBAND)
      {
        leftPwm = baseSpeed;
        rightPwm = baseSpeed;
      }

      steerForward(leftPwm, rightPwm);
    }
    else
    {
      stopMotors();

      // brief reacquire based on last known error
      if (millis() - lastSeen < LOST_REACQUIRE_TIMEOUT_MS)
      {
        if (lastError >= 0)
          spinRight(CENTRE_SPEED);
        else
          spinLeft(CENTRE_SPEED);

        delay(80);
        stopMotors();
      }
      else
      {
        // lost target more seriously -> do a local sweep first
        lcdPrint2("Ball lost", "Reacquire...");
        if (sweep180Search(targetSig, label))
        {
          lastSeen = millis();
        }
        else
        {
          // as a stronger recovery, 360 sweep
          if (fullSweep360UntilFound(targetSig, label, false))
          {
            lastSeen = millis();
          }
          else
          {
            stopMotors();
            return false;
          }
        }
      }
    }

    delay(35);
  }

  stopMotors();
  return false;
}

// ============================================================
// -------------------- RETURN TO BASE -------------------------
// ============================================================

bool inchForwardToDepositWall()
{
  lcdPrint2("At home base", "Inch 5 sec");
  setPixyPaused(true);

  driveForward(DEPOSIT_INCH_SPEED);
  delay(DEPOSIT_INCH_FORWARD_MS);
  stopMotors();
  setPixyPaused(false);

  lcdPrint2("Deposit", "Release ball");
  openClamp();
  delay(400);
  return true;
}
bool returnToBaseAndDeposit()
{
  if (loggedBaseSignature == 0)
    return false;

  lcdPrint2("Find base", signatureName(loggedBaseSignature));

  // 360 sweep until home base is found
  if (!fullSweep360UntilFound(loggedBaseSignature, signatureName(loggedBaseSignature), true))
  {
    lcdPrint2("Base not found", "Retry later");
    stopMotors();
    return false;
  }

  // Centre the base carefully
  if (!centerOnSignature(loggedBaseSignature, "Center base", CENTER_TIMEOUT_MS, true))
  {
    // try again once with a full sweep
    if (!fullSweep360UntilFound(loggedBaseSignature, "Base rescan", true))
      return false;

    if (!centerOnSignature(loggedBaseSignature, "Center base", CENTER_TIMEOUT_MS, true))
      return false;
  }

  lcdPrint2("Returning", "To base");

  unsigned long start = millis();
  unsigned long lastSeen = millis();
  int lastError = 0;

  while (millis() - start < RETURN_TIMEOUT_MS)
  {
    // Suppress obstacle avoidance while intentionally approaching base.
    // Otherwise the home base would trip the ultrasonic threshold.
    BlockInfo b;
    if (getLargestBlockBySignature(loggedBaseSignature, b))
    {
      lastSeen = millis();

      // Stop when startup base size is reached again
      if (reachedHomeSize(b))
      {
        stopMotors();
        if (!inchForwardToDepositWall())
        {
          return false;
        }

        lcdPrint2("Backing up", "Clearing base");
        driveBackward(DEPOSIT_PULLBACK_SPEED);
        delay(REVERSE_AFTER_DEPOSIT_MS);
        stopMotors();
        delay(200);

        lcdPrint2("Turning 180", "Next target");
        timedTurn(true, POST_DEPOSIT_TURN_180_MS, TURN_SPEED, false, "");
        delay(200);
        isFacingArena = true;

        return true;
      }

      int error = b.x - PIXY_FRAME_CENTER_X;
      lastError = error;

      // Straight-line proportional steering toward base
      int correction = (int)(STEER_KP * error);
      int leftPwm  = constrain(BASE_APPROACH + correction, 20, 110);
      int rightPwm = constrain(BASE_APPROACH - correction, 20, 110);

      // If almost centered and close, inch forward
      if (abs(error) <= CENTER_DEADBAND && b.area > (loggedBaseArea * 0.55))
      {
        leftPwm = INCH_SPEED;
        rightPwm = INCH_SPEED;
      }

      steerForward(leftPwm, rightPwm);
    }
    else
    {
      stopMotors();

      // If base was only just lost, rotate gently back toward last seen side
      if (millis() - lastSeen < LOST_REACQUIRE_TIMEOUT_MS)
      {
        if (lastError >= 0)
          spinRight(CENTRE_SPEED);
        else
          spinLeft(CENTRE_SPEED);

        delay(80);
        stopMotors();
      }
      else
      {
        // stronger reacquisition
        lcdPrint2("Base lost", "Reacquire...");
        if (!fullSweep360UntilFound(loggedBaseSignature, "Base scan", true))
        {
          stopMotors();
          return false;
        }
      }
    }

    delay(35);
  }

  stopMotors();
  return false;
}

// ============================================================
// ------------------ HIGH-LEVEL MISSION FLOW ------------------
// ============================================================

bool doOneColourCycle(uint8_t colourSig, const char* colourName)
{
  // Starting condition: robot is facing base
  lcdPrint2("Target:", colourName);
  delay(600);

  // Open clamp at beginning of each cycle just to ensure known state
  openClamp();

  // Turn only if this cycle starts while still facing the base.
  if (!isFacingArena)
  {
    lcdPrint2("Turning 180", colourName);
    timedTurn(true, TURN_180_MS, TURN_SPEED, false, "");
    delay(200);
    isFacingArena = true;
  }

  // Immediately after facing arena, do a 180 sweep for target.
  // sweep180Search() restores original heading when not found.
  bool foundAfterTurn = sweep180Search(colourSig, colourName);

  // If not seen in the 180 sweep, reset heading (handled above) then push forward.
  if (!foundAfterTurn)
  {
    lcdPrint2("Initial push", colourName);
    driveForward(TRAVEL_SPEED);
    delay(POST_TURN_PUSH_MS);
    stopMotors();
    delay(120);
  }

  // Search
  while (!searchForBall(colourSig, colourName))
  {
    lcdPrint2("Retry search", colourName);
    delay(300);
  }

  // Centre before the main chase
  centerOnSignature(colourSig, colourName, CENTER_TIMEOUT_MS, false);

  // Chase and clamp
  while (!chaseBallAndClamp(colourSig, colourName))
  {
    lcdPrint2("Retry chase", colourName);
    delay(300);

    // Re-search if chase failed
    while (!searchForBall(colourSig, colourName))
    {
      lcdPrint2("Retry search", colourName);
      delay(300);
    }
    centerOnSignature(colourSig, colourName, CENTER_TIMEOUT_MS, false);
  }

  // Return to base and deposit
  while (!returnToBaseAndDeposit())
  {
    lcdPrint2("Retry return", colourName);
    delay(300);
  }

  lcdPrint2("Deposited:", colourName);
  delay(700);

  return true;
}

// ============================================================
// ---------------------- ARDUINO SETUP ------------------------
// ============================================================

void setup()
{
  pinMode(DIR_A_PIN, OUTPUT);
  pinMode(PWM_A_PIN, OUTPUT);
  pinMode(DIR_B_PIN, OUTPUT);
  pinMode(PWM_B_PIN, OUTPUT);

  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);

  pinMode(IR_SENSOR_PIN, INPUT);

  stopMotors();

  servoRight.attach(SERVO_RIGHT_PIN);
  servoLeft.attach(SERVO_LEFT_PIN);
  openClamp();

  lcd.init();
  lcd.backlight();
  lcdPrint2("Starting", "WALL-F");
  delay(800);

  randomSeed(analogRead(A0));
  pixy.init();
  pixy.setLamp(0, 0);
  // ✅ 1. LOG BASE IMMEDIATELY (no movement yet)
  while (!logBaseSignatureAtStart())
  {
    lcdPrint2("Base log fail", "Retry...");
     delay(1000);
  }
  
  // ✅ 2. NOW reverse away from the base
  lcdPrint2("Backing up", "Init clear");
  driveBackward(CENTRE_SPEED);
  delay(STARTUP_REVERSE_MS);
  stopMotors();
  delay(300);

  lcdPrint2("Ready", "Mission start");
  delay(700);
}

// ============================================================
// ----------------------- ARDUINO LOOP ------------------------
// ============================================================

void loop()
{
  // Infinite strict order: RED -> GREEN -> BLUE -> repeat forever
  doOneColourCycle(SIG_RED,   "RED");
  doOneColourCycle(SIG_GREEN, "GREEN");
  doOneColourCycle(SIG_BLUE,  "BLUE");
}


