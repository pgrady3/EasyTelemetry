#define CURRENT_CAL 1.0
#define CURRENT_OFFSET 0.0;

#define VOLTAGE_CAL 2.056

uint16_t INAreadReg(uint8_t reg);
void INAinit();
double INAvoltage();
double INAcurrent();

double INAcurrent()
{
  int16_t raw = INAreadReg(0x01); //deliberate bad cast! the register is stored as two's complement
  return (float)raw / 800.0 * CURRENT_CAL + CURRENT_OFFSET; //2.5uV lsb and 2mOhm resistor
}

double INAvoltage()
{
  uint16_t raw = INAreadReg(0x02);
  return (float)raw / 800.0 * VOLTAGE_CAL; //multiply by 1.25mV LSB
}

void INAinit()
{
  Wire.beginTransmission(0x40);
  Wire.write(0x00);//reg select = 0x00
  Wire.write(0b0111);//64 averages, 1ms voltage sampling
  Wire.write(0b100111);//1ms current sampling, free running
  Wire.endTransmission();
}

uint16_t INAreadReg(uint8_t reg)
{
  Wire.beginTransmission(0x40);
  Wire.write(reg);//read from the bus voltage
  Wire.endTransmission();

  Wire.requestFrom(0x40, 2);

  delayMicroseconds(100);
  if (Wire.available() < 2)
    return 0;

  uint16_t resp = (uint16_t)Wire.read() << 8;
  resp |= Wire.read();

  return resp;
}
