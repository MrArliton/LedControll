#define _CRT_SECURE_NO_WARNINGS
#include "led.cpp"
#include <tchar.h>
#include <fstream>
Led led; // Рабочий класс

//
int bok1;
int bok2;
int port;
int length;
//

LPCWSTR getComPort() {
std::wifstream in("C:\\ConfigLed\\config.txt"); // окрываем файл для чтения
wchar_t *str=new wchar_t[10];
if (in.is_open())
{
	in >> str;
	in >> bok1;
	in >> bok2;
	in >> length;
	in >> port;
}
in.close();     // закрываем файл
return str;
}
char* buffer = new char[13];
char* format(int i,int red,int green,int blue) {
	buffer = new char[13];
	buffer[0] = 'o';
	char* buff = led.toChar(i);
	buffer[1] = buff[1];
	buffer[2] = buff[2];
	delete[]buff;
	buff = led.toChar(red);
	for (int i = 0; i < 3;i++) {
		buffer[3 + i] = buff[i];
	}
	delete[]buff;
	buff = led.toChar(green);
	for (int i = 0; i < 3; i++) {
		buffer[6 + i] = buff[i];
	}
	delete[]buff;
	buff = led.toChar(blue);
	for (int i = 0; i < 3; i++) {
		buffer[9 + i] = buff[i];
	}
	buffer[12] ='0';
	delete[]buff;
	return buffer;
}
int _tmain(int argc, TCHAR* argv[])
{
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	led.openComPort(getComPort());
	//	char* buf = format(i,255,255,255);
	while (1) {
		led.update();
		int width = led.getWidth() / (length - (bok1 + bok2));
		int height = led.getHeight() / bok1;

		if (led.isConnected()) {
			for (int i = 0; i < bok1; i++) {
				char* buf = format(i,led.getRedSquare(width, led.getHeight() - height * (i + 1), height, width * 2),led.getGreenSquare(width, led.getHeight() - height * (i + 1), height, width * 2),led.getBlueSquare(width, led.getHeight() - height * (i + 1), height, width * 2));
				led.write(buf);
			}
			for (int i = bok1; i < length - bok2; i++) {
				led.write(format(
					i,
					(led.getRedSquare(width * (i - bok1), 0, height * 4, width)),
					(led.getGreenSquare(width * (i - bok1), 0, height * 4, width)),
					(led.getBlueSquare(width * (i - bok1), 0, height * 4, width))
				));
			}
			for (int i = length - bok2; i < length; i++) {
				led.write(format((i),
					(led.getRedSquare(led.getWidth() - width * 2, height * (i - (length - bok2)), height, width * 2)),
					(led.getGreenSquare(led.getWidth() - width * 2, height * (i - (length - bok2)), height, width * 2)),
					(led.getBlueSquare(led.getWidth() - width * 2, height * (i - (length - bok2)), height, width * 2))
				));
			}

		}
		else {
			led.openComPort(getComPort());
		}
	}
	}