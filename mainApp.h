#pragma once

#include "wx/wx.h"
#include "mainWindow.h"
#include "ArduinoReader.h"

class mainApp : public wxApp {
	virtual bool OnInit() override;
	virtual int OnExit() override;
	mainWindow* window = nullptr;
	threadSafeScreen* tss = nullptr;
	ArduinoReader* reader = nullptr;
};