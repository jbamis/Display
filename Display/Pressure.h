/* 
*  BMP180
*/

#pragma once

//                Register
#define out_xlsb	0XF8
#define out_lsb		0XF7
#define out_msb		0XF6
#define ctrl_meas	0XF4
#define soft		0XE0
#define id			0XD0

class Pressure
{
public:
	Pressure();
	~Pressure();
	void initialize(int fd_init);
	short uncompensated_temp();
	long calculate_true_temp(int UT);
	long uncompensated_pres();
	long calculate_true_pres(int UP);

private:
	int fd;
	short reg;
	short data;

	short AC1;				//= 408	;
	short AC2;				//= -72	;
	short AC3;				//= 14383 ;
	unsigned short AC4;		//= 32741 ;
	unsigned short AC5;		//= 32757 ;
	unsigned short AC6;		//= 23153 ;
	short B1;				//= 6190  ;
	short B2;				//= 4     ;
	short MB;				//= 32768 ;
	short MC;				//= -8711 ;
	short MD;				//= 2868  ;

	short UT;
	long UP;
	long X1;
	long X2;
	long X3;
	long B3;
	unsigned long B4;
	long B5;
	long B6;
	unsigned long B7;
};

Pressure::Pressure()
{

}

Pressure::~Pressure()
{

}

void Pressure::initialize(int fd_init)
{
	fd = fd_init;
	short data1;

	reg = 0xaa;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xab;
	data = wiringPiI2CReadReg8(fd, reg);
	AC1 = data + data1;

	reg = 0xac;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xad;
	data = wiringPiI2CReadReg8(fd, reg);
	AC2 = data + data1;

	reg = 0xae;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xaf;
	data = wiringPiI2CReadReg8(fd, reg);
	AC3 = data + data1;


	reg = 0xb0;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xb1;
	data = wiringPiI2CReadReg8(fd, reg);
	AC4 = data + data1;

	reg = 0xb2;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xb3;
	data = wiringPiI2CReadReg8(fd, reg);
	AC5 = data + data1;

	reg = 0xb4;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xb5;
	data = wiringPiI2CReadReg8(fd, reg);
	AC6 = data + data1;

	reg = 0xb6;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xb7;
	data = wiringPiI2CReadReg8(fd, reg);
	B1 = data + data1;

	reg = 0xb8;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xb9;
	data = wiringPiI2CReadReg8(fd, reg);
	B2 = data + data1;

	reg = 0xba;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xbb;
	data = wiringPiI2CReadReg8(fd, reg);
	MB = data + data1;

	reg = 0xbc;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xbd;
	data = wiringPiI2CReadReg8(fd, reg);
	MC = data + data1;

	reg = 0xbe;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = 0xbf;
	data = wiringPiI2CReadReg8(fd, reg);
	MD = data + data1;
}

short Pressure::uncompensated_temp()
{
	short data1;
	data = 0x2e;
	reg = ctrl_meas;
	wiringPiI2CWriteReg8(fd, reg, data);
	delayMicroseconds(4500);

	reg = out_msb;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 8;
	reg = out_lsb;
	data = wiringPiI2CReadReg8(fd, reg);
	UT = data + data1;
	return(UT);
}

long Pressure::calculate_true_temp(int UT)
{
	long Temp;
	X1 = ((UT - AC6) * AC5) >> 15;
	X2 = MC << 11;
	X2 = X2 / (X1 + MD);
	B5 = X1 + X2;
	Temp = (B5 + 8) >> 4;			// Temp in 0.1 degrees C
	Temp = (Temp  * (90/5)) + 3200;	// Temp in 0.01 degree F
	return(Temp);
}

long Pressure::uncompensated_pres()
{
	long data1;
	unsigned short data2;
	
	data = 0x34;
	reg = ctrl_meas;
	wiringPiI2CWriteReg8(fd, reg, data);
	delay(77);

	reg = out_msb;
	data1 = wiringPiI2CReadReg8(fd, reg);
	data1 <<= 16;
	reg = out_lsb;
	data2 = wiringPiI2CReadReg8(fd, reg);
	data2 <<= 8;
	UP = data1 + data2;
	reg = out_xlsb;
	data1 = wiringPiI2CReadReg8(fd, reg);
	UP = (UP + data1) >> 8 ;
	return(UP);

}

long Pressure::calculate_true_pres(int UP)
{
	long p;
	B6 = B5 - 4000;
	X1 = (B2 * (B6 * B6 >> 12)) >> 11;
	X2 = (AC2 * B6) >> 11;
	X3 = X1 + X2;
	B3 = (((long)AC1 * 4 + X3) + 2) >> 2;
	X1 = (AC3 * B6) >> 13;
	X2 = (B1 * (B6 * B6 >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = AC4 * (unsigned long)(X3 + 32768) >> 15;
	B7 = ((unsigned long)UP - B3) * 50000;
	if (B7 > 0x8000000)
	{
		p = (B7 * 2) / B4;
	}
	else
	{
		p = (B7 / B4) * 2;
	}
	X1 = (p >> 8) * (p >> 8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * p) >> 16;
	p = p + ((X1 + X2 + 3791) >> 4);		// Pres in Pa
	p = (float)p * 0.02953;					// Pres in inHg

	return(p);
}

