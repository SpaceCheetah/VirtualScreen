#include "ArduinoReader.h"
#include <thread>
#include <String>
#include <iostream>

#define DEBUGSTREAM \
if (debugDisabled) {} \
else std::cout
#define RFERROR \
else { \
		 onFatalError("ReadFile", GetLastError()); \
		 return false; \
}

void ArduinoReader::onFatalError(std::string function, unsigned long error) {
	if (error == 995) { //This is expected, as we call CancelSynchronousIo
		std::cout << "Closing, aborted active I/O\n";
	} else {
		std::cout << function << " failed with error: ";
		char buffer[255];
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ARGUMENT_ARRAY, NULL, error, 0, (LPSTR)&buffer, 255, NULL);
		std::cout << buffer;
	}
	onFatalError();
}

void ArduinoReader::onFatalError() {
	if (!shouldStop) { //if the program is already stopping, window will likely be invalid (fatal error on stop is pretty likely since CancelSynchronousIo triggers one)
		std::cout << "Closing due to fatal error" << std::endl;
		wxCloseEvent* event = new wxCloseEvent(wxEVT_CLOSE_WINDOW, WINDOW_ID);
		event->SetEventObject(window);
		window->GetEventHandler()->QueueEvent(event);
	}
}

int ArduinoReader::parseInt(unsigned char* buffer) {
	return ((unsigned int)buffer[0] << 24) | ((unsigned int)buffer[1] << 16) | ((unsigned int)buffer[2] << 8) | buffer[3];
}

ArduinoReader::ArduinoReader(mainWindow* window, threadSafeScreen* tss, bool resetFlag, bool debug, std::string comPort) : window{ window }, tss{ tss }, resetFlag{ resetFlag }, debugDisabled{ !debug }, comPort{ comPort } {
	readerThread = std::thread(&ArduinoReader::startReal, this);
}

ArduinoReader::~ArduinoReader() {
	//join owned thread
	shouldStop = true; //setting a variable across threads would usually be risky, but that's the point of std::atomic
	CancelSynchronousIo(readerThread.native_handle()); //without this, the code could get stuck indefinitely waiting for serial data
	readerThread.join();
	//close handle
	CloseHandle(serialHandle);
}

bool ArduinoReader::waitForArduinoStart() {
	constexpr char startMessage[] = "VS START";
	char buffer = '\0';
	int index = 0;
	unsigned long bytesRead = 0;
	if (!WriteFile(serialHandle, startMessage, 8, &bytesRead, NULL)) { //send VS START to arduino; arduino could use this to know when to send the image when not using --reset
		onFatalError("WriteFile", GetLastError());
		return false;
	}
	while (!shouldStop) {
		if (ReadFile(serialHandle, &buffer, 1, &bytesRead, NULL)) {
			if (buffer == startMessage[index]) {
				index++;
				if (index == 8) {
					std::cout << "VS START Received\n";
					return true;
				}
			} else {
				index = 0;
			}
		} RFERROR
	}
	return false;
}

bool ArduinoReader::readHeader() {
	unsigned char buffer[8];
	unsigned long bytesRead = 0;
	if (ReadFile(serialHandle, &buffer, 8, &bytesRead, NULL)) {
		xRes = parseInt(buffer);
		yRes = parseInt(buffer + 4);
		std::cout << "X resolution: " << xRes << "\nY resolution: " << yRes << std::endl;
		tss->setImage(wxImage{ xRes,yRes,true });
		window->sendNeedsRepaint();
	} RFERROR
	return true;
}

bool ArduinoReader::mainLoop() {
	//get packet type
	char buffer = '\0';
	unsigned long bytesRead = 0;
	if (ReadFile(serialHandle, &buffer, 1, &bytesRead, NULL)) {
		switch (buffer) {
		case 0: {//update buffer
			unsigned char* data = (unsigned char*)malloc((size_t)xRes * yRes * 3);//malloc, not new, because according to https://docs.wxwidgets.org/trunk/classwx_image.html#a2c97634b43bdd143f34418fb1f98a690
			if (ReadFile(serialHandle, data, (unsigned long)xRes * yRes * 3, &bytesRead, NULL)) {
				tss->setImage(wxImage{ xRes,yRes,data,false });
				window->sendNeedsRepaint();
				DEBUGSTREAM << "Received updated buffer\n";
			} else {
				free(data);
				onFatalError("ReadFile", GetLastError());
				return false;
			}
			break;//wxImage takes ownership of the data, and frees it, but the data has to be allocated with malloc
		}
		case 1: {//set pixel
			unsigned char data[11];
			if (ReadFile(serialHandle, data, 11, &bytesRead, NULL)) {
				int x = parseInt(data);
				int y = parseInt(data + 4);
				tss->setPixel(x, y, data[8], data[9], data[10]);
				window->sendNeedsRepaint();
				DEBUGSTREAM << "Set pixel " << x << "," << y << " to [" << (int)data[8] << "," << (int)data[9] << "," << (int)data[10] << "]\n";
			} RFERROR
			break;
		}
		case 2: {//draw line
			unsigned char data[19];
			if (ReadFile(serialHandle, data, 19, &bytesRead, NULL)) {
				int x1 = parseInt(data);
				int y1 = parseInt(data + 4);
				int x2 = parseInt(data + 8);
				int y2 = parseInt(data + 12);
				tss->drawLine(x1, y1, x2, y2, data[16], data[17], data[18]);
				window->sendNeedsRepaint();
				DEBUGSTREAM << "Drew line from " << x1 << "," << y1 << " to " << x2 << "," << y2 << " with color [" << (int)data[16] << "," << (int)data[17] << "," << (int)data[18] << "]\n";
			} RFERROR
			break;
		}
		case 3: {//draw triangle
			unsigned char data[27];
			if (ReadFile(serialHandle, data, 27, &bytesRead, NULL)) {
				int x1 = parseInt(data);
				int y1 = parseInt(data + 4);
				int x2 = parseInt(data + 8);
				int y2 = parseInt(data + 12);
				int x3 = parseInt(data + 16);
				int y3 = parseInt(data + 20);
				tss->drawTriangle(x1, y1, x2, y2, x3, y3, data[24], data[25], data[26]);
				window->sendNeedsRepaint();
				DEBUGSTREAM << "Drew triangle with vertices [" << x1 << "," << y1 << "], [" << x2 << "," << y2 << "], [" << x3 << "," << y3 << "] and color [" << (int)data[24] << "," << (int)data[25] << "," << (int)data[26] << "]\n";
			} RFERROR
			break;
		}
		case 4: {//draw rectangle
			unsigned char data[19];
			if (ReadFile(serialHandle, data, 19, &bytesRead, NULL)) {
				int x = parseInt(data);
				int y = parseInt(data + 4);
				int width = parseInt(data + 8);
				int height = parseInt(data + 12);
				tss->drawRectangle(x, y, width, height, data[16], data[17], data[18]);
				window->sendNeedsRepaint();
				DEBUGSTREAM << "Drew rectangle with TL corner [" << x << "," << y << "], width " << width << ", height " << height << ", and color [" << (int)data[16] << "," << (int)data[17] << (int)data[18] << "]\n";
			} RFERROR
			break;
		}
		case 5: {//draw polygon
			char buffer = '\0';
			if (ReadFile(serialHandle, &buffer, 1, &bytesRead, NULL)) {
				int vertices = (int)buffer;
				if (vertices > 2) {
					unsigned char* data = new unsigned char[8 * vertices + 3];
					if (ReadFile(serialHandle, data, 8 * vertices + 3, &bytesRead, NULL)) {
						std::vector<wxPoint> verticesArray{};
						unsigned char* ptr = data;
						for (int i = 0; i < vertices; i++) {
							verticesArray.push_back(wxPoint(parseInt(ptr), parseInt(ptr + 4)));
							ptr += 8;
						}
						tss->drawPolygon(verticesArray, ptr[0], ptr[1], ptr[2]);
						window->sendNeedsRepaint();
						DEBUGSTREAM << "Drew polygon with vertices ";
						for (wxPoint p : verticesArray) {
							DEBUGSTREAM << "[" << p.x << "," << p.y << "], ";
						}
						DEBUGSTREAM << "and color [" << (int)ptr[0] << "," << (int)ptr[1] << "," << (int)ptr[2] << "]\n";
					} RFERROR
					delete data;
				} else {
					std::cout << "Vertices was less than 3: " << vertices << std::endl;
					onFatalError();
					return false;
				}
			} RFERROR
			break;
		}
		case 6: { //draw ellipse
			unsigned char data[19];
			if (ReadFile(serialHandle, data, 19, &bytesRead, NULL)) {
				int x = parseInt(data);
				int y = parseInt(data + 4);
				int width = parseInt(data + 8);
				int height = parseInt(data + 12);
				tss->drawEllipse(x, y, width, height, data[16], data[17], data[18]);
				window->sendNeedsRepaint();
				DEBUGSTREAM << "Drew ellipse with center [" << x << "," << y << "], width " << width << ", height " << height << ", and color [" << (int)data[16] << "," << (int)data[17] << "," << (int)data[18] << "]\n";
			} RFERROR
			break;
		}
		default: {//invalid
			std::cout << "Invalid packet type received: " << (int)buffer << std::endl;
			onFatalError();
			return false;
		}
		}
	} RFERROR
	return true;
}

void ArduinoReader::startReal() {
	if (initializeSerial()) {
		if (waitForArduinoStart()) {
			if (readHeader()) {
				while (!shouldStop && mainLoop()) {}
			}
		}
	}
}

bool ArduinoReader::initializeSerial() {
	comPort = "\\\\.\\" + comPort;
	serialHandle = CreateFileA(comPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	DCB dcb;

	if (serialHandle == INVALID_HANDLE_VALUE) {
		onFatalError("CreateFileA", GetLastError());
		return false;
	}

	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(serialHandle, &dcb)) {
		onFatalError("GetCommState", GetLastError());
		return false;
	}

	dcb.BaudRate = 9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fDtrControl = false;
	dcb.fRtsControl = resetFlag; //RTS resets arduino, but leaving it on would keep it in a reset loop

	if (!SetCommState(serialHandle, &dcb)) {
		onFatalError("SetCommState", GetLastError());
		return false;
	}

	std::cout << "Opened port " << comPort << std::endl;

	if (resetFlag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //sleep for 1 millisecond, to give RTS time to register
		dcb.fRtsControl = false;
		std::cout << "Reset Arduino\n";
		if (!SetCommState(serialHandle, &dcb)) { //turn off rts, so the arduino is able to start
			onFatalError("SetCommState", GetLastError()); //EscapeCommFunction failing isn't really fatal, since manual reset is always possible, but it implies a larger issue with the connection
			return false;
		}
	}

	return true;
}