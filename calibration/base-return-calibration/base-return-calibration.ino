#include <Pixy2.h>

Pixy2 pixy;

// Replace xxxx with the base signature being measured.
const uint8_t TEST_BASE_SIGNATURE = xxxx;

// Replace xxxx after measuring the base from the startup distance.
const int LOGGED_BASE_WIDTH = xxxx;
const int LOGGED_BASE_HEIGHT = xxxx;
const long LOGGED_BASE_AREA = xxxx;

// Replace xxxx after testing return stopping behaviour.
const float BASE_WIDTH_RATIO_STOP = xxxx;
const float BASE_HEIGHT_RATIO_STOP = xxxx;
const float BASE_AREA_RATIO_STOP = xxxx;

void setup()
{
  Serial.begin(115200);
  pixy.init();
  pixy.setLamp(0, 0);
}

void loop()
{
  uint16_t blocks = pixy.ccc.getBlocks();
  bool found = false;

  for (uint16_t i = 0; i < blocks; i++)
  {
    if (pixy.ccc.blocks[i].m_signature != TEST_BASE_SIGNATURE)
    {
      continue;
    }

    int w = pixy.ccc.blocks[i].m_width;
    int h = pixy.ccc.blocks[i].m_height;
    long area = (long)w * (long)h;

    bool widthReached = w >= (int)(LOGGED_BASE_WIDTH * BASE_WIDTH_RATIO_STOP);
    bool heightReached = h >= (int)(LOGGED_BASE_HEIGHT * BASE_HEIGHT_RATIO_STOP);
    bool areaReached = area >= (long)(LOGGED_BASE_AREA * BASE_AREA_RATIO_STOP);

    Serial.print("base w=");
    Serial.print(w);
    Serial.print(" h=");
    Serial.print(h);
    Serial.print(" area=");
    Serial.print(area);
    Serial.print(" reached=");
    Serial.println((widthReached && heightReached) || areaReached ? "YES" : "NO");

    found = true;
  }

  if (!found)
  {
    Serial.println("base not visible");
  }

  delay(250);
}

