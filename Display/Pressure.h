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
	int uncompensated_temp();
	long calculate_true_temp(int UT);
	long uncompensated_pres();
	long calculate_true_pres(int UP);

private:
	int fd;
	int reg;
	int data;

	int AC1;
	int AC2;
	int AC3;
	unsigned int AC4;
	unsigned int AC5;
	unsigned int AC6;
	int B1;
	int B2;
	int MB;
	int MC;
	int MD;

	long UT;
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

	reg = 0xaa;
	data = wiringPiI2CReadReg16(fd, reg);
	AC1 = data;

	reg = 0xac;
	data = wiringPiI2CReadReg16(fd, reg);
	AC2 = data;

	reg = 0xaf;
	data = wiringPiI2CReadReg16(fd, reg);
	AC3 = data;

	reg = 0xb0;
	data = wiringPiI2CReadReg16(fd, reg);
	AC4 = data;

	reg = 0xb2;
	data = wiringPiI2CReadReg16(fd, reg);
	AC5 = data;

	reg = 0xb4;
	data = wiringPiI2CReadReg16(fd, reg);
	AC6 = data;

	reg = 0xb6;
	data = wiringPiI2CReadReg16(fd, reg);
	B1 = data;

	reg = 0xb8;
	data = wiringPiI2CReadReg16(fd, reg);
	B2 = data;

	reg = 0xba;
	data = wiringPiI2CReadReg16(fd, reg);
	MB = data;

	reg = 0xbc;
	data = wiringPiI2CReadReg16(fd, reg);
	MC = data;

	reg = 0xbe;
	data = wiringPiI2CReadReg16(fd, reg);
	MD = data;
}

int Pressure::uncompensated_temp()
{
	data = 0x2e;
	reg = 0xf4;
	wiringPiI2CWriteReg8(fd, reg, data);
	delayMicroseconds(4500);

	data = 0xf6;
	data = wiringPiI2CReadReg16(fd, reg);
	return(data);
}

long Pressure::calculate_true_temp(int UT)
{
	long Temp;
	X1 = ((UT - AC6) * AC5) >> 15;
	X2 = MC << 11;
	X2 = X2 / (X1 + MD);
	B5 = X1 + X2;
	Temp = (B5 + 8) >> 4;			// Temp in 0.1 degrees C
	Temp = (Temp + 32) * (50/9);	// Temp in 0.01 degree F
	return(Temp);

}

long Pressure::uncompensated_pres()
{
	data = 0x34;
	reg = 0xf4;
	wiringPiI2CWriteReg8(fd, reg, data);
	delayMicroseconds(4500);

	long UP = 0;
	reg = 0xf6;
	data = wiringPiI2CReadReg8(fd, reg);
	UP = data;
	UP <<= 16;
	reg = 0xf7;
	data = wiringPiI2CReadReg16(fd, reg);
	UP = UP + data;
	return(UP);

}

long Pressure::calculate_true_pres(int UP)
{
	int p;
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
	p = ((float)p / 0.2953) / 10;			// Pres in inHg

	return(p);
}

