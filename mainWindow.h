#pragma once

#include "wx/wx.h"
#include "threadSafeScreen.h"

constexpr auto WINDOW_ID = 2001;
constexpr auto TIMER_ID = 2002;

class mainWindow : public wxFrame {
	wxTimer* frameTimer = nullptr;
public:
	mainWindow(mainWindow const&) = delete;
	mainWindow& operator= (mainWindow const&) = delete;
	std::atomic_bool updateNextFrame = false;
	mainWindow(wxSize size, threadSafeScreen* tss, int delay);
	void doRepaint();
	threadSafeScreen* tss;
	wxBitmap buffer;
	wxWindow* renderSurface;
	void onPaint(wxPaintEvent& event);
	void onSize(wxSizeEvent& event);
	void onFrame(wxTimerEvent& event);
	wxDECLARE_EVENT_TABLE();
};