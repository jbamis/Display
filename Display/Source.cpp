/* Display- RasberryPi  C++  wiringPi
* 
* Install -
* sudo apt - get install wiringPi
* sudo apt - get install - y i2c - tools
* sudo raspi - config
* i2cdetect - y 1
*
*/

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "Display.h"
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
	short UT;
	long UP;
	int i;
	const char stringToDisplayLine1[] = { "Temp (F) = " };
	const char stringToDisplayLine2[] = { "Pres(in) = " };

	Display Display0;
	Pressure pres;

	int fd;						//File Descriptor
	int devId1 = 0X27;			//get from (i2cdetect -y 1//	fd = wiringPiI2CSetup(devId1);
	fd = wiringPiI2CSetup(devId1);
	cout << "Init result 1: " << fd << endl;
	Display0.initialize(fd);

	int devId2 = 0X77;
	fd = wiringPiI2CSetup(devId2);
	cout << "Init result 2: " << fd << endl;
	pres.initialize(fd);						//bmp180_get_cal_param

	for (;;)
	{
		UT = pres.uncompensated_temp();			//bmp180_get_Uncompensated_Temp
		Temp = pres.calculate_true_temp(UT);	//bmp180_get_True_Temp
		UP = pres.uncompensated_pres();			//bmp180_get_Uncompensated_Pressure
		Pres = pres.calculate_true_pres(UP);	//bmp180_get_True_Pressure

		Display0.positionCursor(0, 0);			//position cursor
		for (i = 0; stringToDisplayLine1[i] != '\0'; i++)		// Temp
		{
			Display0.writeStringToLCD(stringToDisplayLine1[i]);
		}
		itoa(Temp, buffer);
		for (i = 0; i < 2; i++)
		{
			Display0.writeStringToLCD(buffer[i]);
		}
		Display0.writeStringToLCD('.');
		for (i = 2; i < 4; i++)
		{
			Display0.writeStringToLCD(buffer[i]);
		}

		Display0.positionCursor(1, 0);
		for (i = 0; stringToDisplayLine2[i] != '\0'; i++)		// Pressure
		{
			Display0.writeStringToLCD(stringToDisplayLine2[i]);
		}
		itoa(Pres, buffer);
		for (i = 0; i < 2; i++)
		{
			Display0.writeStringToLCD(buffer[i]);
		}
		Display0.writeStringToLCD('.');
		for (i = 2; i < 4; i++)
		{
			Display0.writeStringToLCD(buffer[i]);
		}

		delay(1000);
	}
	return(returnid);
}