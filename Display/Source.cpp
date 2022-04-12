/* Display- RasberryPi  1602A-1   C++  wiringPi
* 
* Combining code from David Turnoff (Youtube) and Zhengnan Lee (GitHub).
* 
* Install -
* sudo apt-get install wiringPi
* sudo apt-get install -y i2c-tools
* sudo raspi-config
* i2cdetect -y 1
*
* 
* Display - (4 Bytes)
* P0a - RS                      P0b - RS
* P1a - W/R                     P1b - W/R
* P2a - Enable(1 then 0)        P2b - Enable(1 then 0)
* P3a - Backlight               P3b - Backlight  
* P4a - CMND4 or D4             P4b - CMND0 or D0
* P5a - CMND5 or D5             P5b - CMND1 or D1
* P6a - CMND6 or D6             P6b - CMND2 or D2
* P7a - CMND7 or D7             P7b - CMND3 or D3
* 
*/

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "I2C1.h"
#include "Pressure.h"


using namespace std;

class I2C;
class Pressure;

/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
	int start = 0;
	int end = length - 1;
	while (start < end)
	{
		swap(*(str + start), *(str + end));
		start++;
		end--;
	}
}


// Implementation of itoa()
char* itoa(long num, char* str)
{
	int i = 0;
	bool isNegative = false;

	/* Handle 0 explicitly, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled only with
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0)
	{
		isNegative = true;
		num = -num;
	}

	// Process individual digits
	while (num != 0)
	{
		int rem = num % 10;
		str[i++] = rem + '0';
		num = num / 10;
	}

	// If number is negative, append '-'
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator

	// Reverse the string
	reverse(str, i);

	return str;
}

int main(void)
{
	char buffer[33];

	int returnid = 0;
	long Temp;
	long Pres;
	int UT;
	long UP;
	int i;
	const char  stringToDisplayLine1[] = { "Temp = " };
	const char stringToDisplayLine2[] = { "Pres = " };


	I2C I2C0;
	Pressure pres;

	int fd;						//File Descriptor
	int devId1 = 0X27;			//get from (i2cdetect -y 1//	fd = wiringPiI2CSetup(devId1);
	fd = wiringPiI2CSetup(devId1);
	cout << "Init result 1: " << fd << endl;
	I2C0.initialize(fd);

	int devId2 = 0X77;
	fd = wiringPiI2CSetup(devId2);
	cout << "Init result 2: " << fd << endl;
	pres.initialize(fd);

	for (;;)
	{
		UT = pres.uncompensated_temp();
		Temp = pres.calculate_true_temp(UT);
		UP = pres.uncompensated_pres();
		Pres = pres.calculate_true_pres(UP);

		I2C0.positionCursor(0, 0);

		for (i = 0; stringToDisplayLine1[i] != '\0'; i++)		// Temp
		{
			I2C0.writeStringToLCD(stringToDisplayLine1[i]);
		}

		itoa(Temp, buffer);
		for (i = 0; i < 2; i++)
		{
			I2C0.writeStringToLCD(buffer[i]);
		}
		I2C0.writeStringToLCD('.');
		for (i = 2; i < 4; i++)
		{
			I2C0.writeStringToLCD(buffer[i]);
		}

		I2C0.positionCursor(1, 0);

		for (i = 0; stringToDisplayLine2[i] != '\0'; i++)		// Temp
		{
			I2C0.writeStringToLCD(stringToDisplayLine2[i]);
		}

		itoa(Pres, buffer);
		for (i = 0; i < 2; i++)
		{
			I2C0.writeStringToLCD(buffer[i]);
		}
		I2C0.writeStringToLCD('.');
		for (i = 2; i < 4; i++)
		{
			I2C0.writeStringToLCD(buffer[i]);
		}

		delay(1000);
	}
	return(returnid);
}