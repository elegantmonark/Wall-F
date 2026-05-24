#include <Pixy2.h>

Pixy2 pixy;

// Replace xxxx if signatures are retrained differently.
const uint8_t SIG_RED = xxxx;
const uint8_t SIG_GREEN = xxxx;
const uint8_t SIG_BLUE = xxxx;
const uint8_t SIG_BASE_1 = xxxx;
const uint8_t SIG_BASE_2 = xxxx;
const uint8_t SIG_BASE_3 = xxxx;
const uint8_t SIG_BASE_4 = xxxx;

void setup()
{
  Serial.begin(115200);
  pixy.init();
  pixy.setLamp(0, 0);
}

void loop()
{
  uint16_t blocks = pixy.ccc.getBlocks();

  if (blocks)
  {
    for (uint16_t i = 0; i < blocks; i++)
    {
      Serial.print("sig=");
      Serial.print(pixy.ccc.blocks[i].m_signature);
      Serial.print(" x=");
      Serial.print(pixy.ccc.blocks[i].m_x);
      Serial.print(" y=");
      Serial.print(pixy.ccc.blocks[i].m_y);
      Serial.print(" w=");
      Serial.print(pixy.ccc.blocks[i].m_width);
      Serial.print(" h=");
      Serial.println(pixy.ccc.blocks[i].m_height);
    }
  }
  else
  {
    Serial.println("no blocks");
  }

  delay(250);
}

