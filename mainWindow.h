#pragma once

#include "wx/wx.h"
#include "threadSafeScreen.h"

//constexpr auto NAME_ID = 2001;

constexpr auto WINDOW_ID = 2001;

wxDECLARE_EVENT(NEEDS_REPAINT, wxCommandEvent);

class mainWindow : public wxFrame {
	//wxButton* name = nullptr;
public:
	mainWindow(wxSize size, threadSafeScreen* tss);
	void sendNeedsRepaint();
	void doRepaint();
	threadSafeScreen* tss;
	wxBitmap buffer;
	wxWindow* renderSurface;
	//<return type> name(wxEventName& evt);
	void onPaint(wxPaintEvent& event);
	void onSize(wxSizeEvent& event);
	void onNeedsRepaint(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};