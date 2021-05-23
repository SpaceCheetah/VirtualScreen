#pragma once
#include <thread>
#include "mainWindow.h"
#include "threadSafeScreen.h"

class ArduinoReader {
public:
	ArduinoReader(mainWindow* window, threadSafeScreen* tss, bool resetFlag, bool debug, std::string comPort, long baud);
	~ArduinoReader();
	ArduinoReader(ArduinoReader const&) = delete; //can't move or copy, as neither atomic types nor threads can be copied or moved
	ArduinoReader& operator =(ArduinoReader const&) = delete;
private:
	void onFatalError(std::string function, unsigned long error);
	void onFatalError();
	unsigned int parseInt(unsigned char* buffer);
	bool waitForArduinoStart();
	bool readHeader();
	bool mainLoop();
	void startReal();
	bool initializeSerial();
	//pointers (owned)
	HANDLE serialHandle;
	DCB dcb;
	//pointers (not owned)
	mainWindow* window;
	threadSafeScreen* tss;
	//threads (owned)
	std::thread readerThread;
	//instance vars
	std::atomic_bool shouldStop;
	std::string comPort;
	bool resetFlag, debugDisabled;
	int xRes, yRes;
	long baud;
};

