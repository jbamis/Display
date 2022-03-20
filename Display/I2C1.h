#pragma once

#include <wiringPiI2C.h>
#include <wiringPi.h>

#define LCD_BACKLIGHT 0X08
#define LCD_ENABLED   0X04
#define LCD_CMND 0
#define LCD_CHAR 1


class I2C
{
public:
	I2C();
	~I2C();
	int initialize(int fd);
	int rawTimedWrite(int data, int cmdOrChar);
	void positionCursor(char line, char column);
	int writeStringToLCD(int stringToDisplay);

private:
	int returnid;
	int fd;
};

I2C::I2C()
{

}

I2C::~I2C()
{

}

int I2C::initialize(int fd_init)
{
	fd = fd_init;
	returnid = rawTimedWrite(0X33, LCD_CMND);	// Must initialize to 8-line mode at first
	if (returnid == -1) return(returnid);
	delayMicroseconds(4200);

	returnid = rawTimedWrite(0X32, LCD_CMND);	// Then initialize to 4-line mode
	if (returnid == -1) return(returnid);
	delayMicroseconds(4200);

	returnid = rawTimedWrite(0X28, LCD_CMND);	 // 2 Lines & 5*7 dots
	if (returnid == -1) return(returnid);
	delayMicroseconds(4200);

	returnid = rawTimedWrite(0X0C, LCD_CMND);	 // Enable display without cursor
	if (returnid == -1) return(returnid);
	delayMicroseconds(4200);

	returnid = rawTimedWrite(0X01, LCD_CMND);	 // Clear Screen
	if (returnid == -1) return(returnid);
	delayMicroseconds(4200);

	returnid = rawTimedWrite(0X00, LCD_CMND);
	if (returnid != -1) return(returnid);
	delayMicroseconds(4200);
	return(returnid);
}

void I2C::positionCursor(char line, char column)
{
	char clearLine = line & 1;
	char cleanColumn = column & 0X0f;
	int data = (0X80 | (clearLine << 6) | cleanColumn);
	rawTimedWrite((0X80 | (clearLine << 6) | cleanColumn), LCD_CMND);
}

int I2C::rawTimedWrite(int data, int cmndOrChar)
{
	int cleanRS = cmndOrChar & 0X01;
	int buf = data & 0Xf0;
	returnid = wiringPiI2CWrite(fd, buf | LCD_BACKLIGHT | LCD_ENABLED | cleanRS);
	if (returnid == -1) return(returnid);
	delay(2);
	returnid = wiringPiI2CWrite(fd, buf | LCD_BACKLIGHT | cleanRS);
	if (returnid == -1) return(returnid);
	delay(2);


	buf = (data & 0X0f) << 4;
	returnid = wiringPiI2CWrite(fd, buf | LCD_BACKLIGHT | LCD_ENABLED | cleanRS);
	if (returnid == -1) return(returnid);
	delay(2);
	returnid = wiringPiI2CWrite(fd, buf | LCD_BACKLIGHT | cleanRS);
	if (returnid == -1) return(returnid);
	delay(2);
}

int I2C::writeStringToLCD(int data)
{
	returnid = rawTimedWrite(data, LCD_CHAR);
	return(returnid);
}
