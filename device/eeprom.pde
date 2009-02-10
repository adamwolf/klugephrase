#include <Wire.h>
//Code from fi

//TODO:
//I really should abstract out a little bit and get write_int, read_int, write_block, read_block, write_byte, read_byte
//so I don't have to mention things like EEPROM ADDRESS anywhere else.

void i2c_eeprom_read_16_block(int deviceaddress, unsigned int eeaddress, byte* buffer)
{
  i2c_eeprom_read_buffer(deviceaddress, eeaddress, buffer, 16);
}

void i2c_eeprom_write_16_block(int deviceaddress, unsigned int eeaddress, byte* buffer)
{
  int i;
  byte b;

  for (i = 0; i < 16; i++)
  {
    b = buffer[i];
    i2c_eeprom_write_byte(deviceaddress, eeaddress + i, b);

  }
}

void i2c_eeprom_write_int( int deviceaddress, unsigned int eeaddress, int data )
{
  i2c_eeprom_write_byte(deviceaddress, eeaddress, data>>8);
  i2c_eeprom_write_byte(deviceaddress, eeaddress+1, data & 0xFF);
}

int i2c_eeprom_read_int( int deviceaddress, unsigned int eeaddress )
{
  int i;
  i = i2c_eeprom_read_byte(deviceaddress, eeaddress);
  i = i<<8;
  i = i + i2c_eeprom_read_byte(deviceaddress, eeaddress+1);
  return i;
}


void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) {
  int rdata = data;
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8)); // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.send(rdata);
  Wire.endTransmission();
  delay(10); //voodooish...
}

// WARNING: address is a page address, 6-bit end will wrap around
// also, data can be maximum of about 30 bytes, because the Wire library has a buffer of 32 bytes
void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte* data, byte length ) {
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddresspage >> 8)); // MSB
  Wire.send((int)(eeaddresspage & 0xFF)); // LSB
  byte c;
  for ( c = 0; c < length; c++)
    Wire.send(data[c]);
  Wire.endTransmission();
}

byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8)); // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) rdata = Wire.receive();
  return rdata;
}

// maybe let's not read more than 30 or 32 bytes at a time!
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte *buffer, int length ) {
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8)); // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,length);
  int c = 0;
  for ( c = 0; c < length; c++ )
    if (Wire.available()) buffer[c] = Wire.receive();
}

void eeprom_setup()
{
  Wire.begin();
}
