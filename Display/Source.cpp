/* Display  1602A-1   C++  wiringPi
* Combining code from David Turnoff and Zhengnan Lee.
* sudo apt-get install wiringPi
* sudo apt-get install -y i2c-tools
* sudo raspi-config
* i2cdetect -y 1
* 
*/



#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <unistd.h>
#include <iostream>
#include <string>


using namespace std;

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
	int positionCursor(char line, char column);
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
	delay(15);
	rawTimedWrite(0X33, LCD_CMND);
	delayMicroseconds(4200);
	rawTimedWrite(0X32, LCD_CMND);
	delayMicroseconds(4200);
	rawTimedWrite(0X28, LCD_CMND);
	delayMicroseconds(4200);
	rawTimedWrite(0X0C, LCD_CMND);
	delayMicroseconds(4200);
	rawTimedWrite(0X01, LCD_CMND);
	delayMicroseconds(4200);
	rawTimedWrite(0X00, LCD_CMND);
	delayMicroseconds(4200);

	return(returnid);
}

int I2C::positionCursor(char line, char column)
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
	wiringPiI2CWrite(fd, buf | LCD_BACKLIGHT | LCD_ENABLED | cleanRS);
	delay(2);
	wiringPiI2CWrite(fd, buf | LCD_BACKLIGHT | cleanRS);
	delay(2);


	buf = (data & 0X0f) << 4;
	wiringPiI2CWrite(fd, buf | LCD_BACKLIGHT | LCD_ENABLED | cleanRS);
	delay(2);
	wiringPiI2CWrite(fd, buf | LCD_BACKLIGHT | cleanRS);
	delay(2);
}

int I2C::writeStringToLCD(int data)
{
	rawTimedWrite(data, LCD_CHAR);
}



int main(void)
{
	int fd_init;
	int returnid = 0;
	int devId = 0X27;
	char stringToDisplayLine1[] = { "Hello," };
	char stringToDisplayLine2[] = { "World!" };

	I2C I2C0;

	fd_init = wiringPiI2CSetup(devId);
	cout << "Init result: " << fd_init << endl;

	I2C0.initialize(fd_init);
	I2C0.positionCursor(0, 5);

	int i;
	for (i = 0; stringToDisplayLine1[i] != '\0'; i++)
	{
		I2C0.writeStringToLCD(stringToDisplayLine1[i]);
	}

	I2C0.positionCursor(1, 5);

	for (i = 0; stringToDisplayLine2[i] != '\0'; i++)
	{
		I2C0.writeStringToLCD(stringToDisplayLine2[i]);
	}
}