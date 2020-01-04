#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

class Led{

private:
	HDC hdc;
	int ScreenWidth;	
	int ScreenHeight;
	char* bits;
	int linesize;
	// Serial
	DCB dcbSerialParams;
	LPCWSTR portName = new wchar_t[10];
	HANDLE hSerial;

public:
	Led() {
		hdc = GetDC(0);
		ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		bits = new char[2];
	}
	void update() {
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBm = CreateCompatibleBitmap(hdc, ScreenWidth, ScreenHeight);
		HBITMAP oldBm = (HBITMAP)SelectObject(memDC, memBm);
		BitBlt(memDC, 0, 0, ScreenWidth, ScreenHeight, hdc, 0, 0, SRCCOPY);

		BITMAPINFOHEADER info;
		memset(&info, 0, sizeof(info));
		info.biSize = sizeof(BITMAPINFOHEADER);
		info.biCompression = BI_RGB;
		info.biPlanes = 1;
		info.biBitCount = 32;
		info.biWidth = ScreenWidth;
		info.biHeight = ScreenHeight;
		info.biSizeImage = ScreenHeight * ((ScreenWidth * info.biBitCount / 8 + 3) & ~3);

		linesize = (ScreenWidth * info.biBitCount / 8 + 3) & ~3;
		delete []bits;
		bits = new char[ScreenHeight * linesize];	
		GetDIBits(memDC, memBm, 0,ScreenHeight, bits, (BITMAPINFO*)& info, 0);
		SelectObject(memDC, oldBm);
		DeleteObject(memBm);
		DeleteDC(memDC);
	}


	unsigned GetPixel(int x, int y)
	{
		if (!bits || (x < 0) || (x >= ScreenWidth) || (y < 0) || (y >= ScreenHeight))
			return 0;
		return *(unsigned*)(bits + (ScreenHeight - y - 1) * linesize + x * 4);
	}
	int getRed(int x,int y) {
	return (GetPixel(x, y)>>16) & 255;
	}
	int getGreen(int x,int y) {
		return (GetPixel(x, y) >> 8) & 255;
	}
	int getBlue(int x,int y) {
		return (GetPixel(x, y) >> 0) & 255;
	}
	int getRedSquare(int x,int y,int width,int height) {
	//	std::cout << " " << x << " " << y << " " << height << " " << width << " \n";
		int red = 0;
		for (int i = x; i < x+width;i++) {
			for (int o = y; o < y+height;o++) {
				red += getRed(i, o);
			}
		}
		red = red / (1+((width)*( height)));
		//std::cout << " - " << red << " - ";
		return red;
	}
	int getGreenSquare(int x, int y, int width, int height) {
	//	std::cout << " " << x << " " << y << " " << height << " " << width << " \n";
		int green = 0;
		for (int i = x; i < x+width; i++) {
			for (int o = y; o < y+height; o++) {
				green += getGreen(i, o);
			}
		}
		green = green / (1+(width )*( height));
		return green;
	}
	int getBlueSquare(int x, int y, int width, int height) {
	//	std::cout << " " << x << " " << y << " " << height << " " << width << " \n";
		int blue = 0;
		for (int i = x; i < x+width; i++) {
			for (int o = y; o < y+height; o++) {
				blue += getBlue(i, o);
			}
		}
		blue = blue / (1+(width)*(height));
		//std::cout << " - " << blue << " - ";
		return blue;
	}
	char* toChar(int x) { // Собственное преобразование
		char* ch = new char[4];
		ch[0] = (x / 100)+'0';
		ch[1] = ((x - ((x / 100) * 100)) / 10)+'0';
		ch[2] = ((x - ((x / 100) * 100)) - ((x - ((x / 100) * 100)) / 10) * 10) + '0';
		ch[3] = 0;
	
		return ch;
	}
	~Led() {
		delete[]bits;
		ReleaseDC(0, hdc);
	}
	bool isConnected() {
		return ((hSerial) && (hSerial != INVALID_HANDLE_VALUE));
	}
	bool openComPort(LPCWSTR sPortName) {
		portName = sPortName;
		hSerial = ::CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hSerial == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
			{
				std::cout << "serial port does not exist.\n";
			}
			std::cout << "some other error occurred.\n";
		}
		 dcbSerialParams = { 0 };
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		if (!GetCommState(hSerial, &dcbSerialParams))
		{
			std::cout << "serial port does not exist.\n";
			return false;
		}
		dcbSerialParams.BaudRate = CBR_115200;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;
		if (!SetCommState(hSerial, &dcbSerialParams))
		{
			std::cout << "serial port does not exist.\n";
			return false;
		}
		return true;
	}
	bool write(char* str) {
		Sleep(15);
		if (isConnected()) {
			WriteFile(hSerial, str, sizeof(str), NULL, NULL);
			delete[] str;
			return true;
		}
		delete[] str;
		return false;
	}

	bool write(const char* str) {
		Sleep(15);
		if (isConnected()) {
			WriteFile(hSerial, str, sizeof(str), NULL, NULL);
			return true;
		}
		return false;
		}
	char* read(int bytes) {
		char* chars = new char[bytes];
		if (isConnected()) {
			ReadFile(hSerial,chars,bytes,NULL,NULL);
		}
		return chars;
	}
	bool closeComPort() {
		if (isConnected()) {
			CloseHandle(hSerial);
			return true;
		}
		else {
			return false;
		}
	}
	int getWidth() {
		return ScreenWidth;
	}
	int getHeight() {
		return ScreenHeight;
	}
};
