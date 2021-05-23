#include "mainWindow.h"
#include <string>
#include <wx/choicdlg.h>

wxBEGIN_EVENT_TABLE(mainWindow, wxFrame)
	EVT_SIZE(mainWindow::onSize)
	EVT_TIMER(TIMER_ID, mainWindow::onFrame)
wxEND_EVENT_TABLE()

mainWindow::mainWindow(wxSize size, threadSafeScreen* tss, int delay) : wxFrame(nullptr, WINDOW_ID, "VirtualScreen", wxPoint(200, 200), size), tss(tss) {
	renderSurface = new wxWindow(this, wxID_ANY, wxDefaultPosition, size);
	renderSurface->SetBackgroundStyle(wxBG_STYLE_PAINT);
	renderSurface->Bind(wxEVT_PAINT, &mainWindow::onPaint, this);
	frameTimer = new wxTimer(this, TIMER_ID);
	frameTimer->Start(delay);
}

void mainWindow::onPaint(wxPaintEvent& event) {
	if (buffer.IsOk()) {
		wxPaintDC dc(renderSurface);
		dc.DrawBitmap(buffer, 0, 0);
	}
}

void mainWindow::onSize(wxSizeEvent& event) {
	doRepaint();
}

void mainWindow::doRepaint() {
	if (tss->initialized) {
		wxSize clientSize = wxFrame::GetClientSize();
		int width = clientSize.GetWidth();
		int height = clientSize.GetHeight();
		if (width > 0 && height > 0) {
			renderSurface->SetSize(clientSize);
			wxImage scaled = tss->getImage().Scale(width, height);
			buffer = wxBitmap(scaled, -1);
			renderSurface->Refresh();
			renderSurface->Update();
		}
	}
}

void mainWindow::onFrame(wxTimerEvent& event) {
	if (updateNextFrame) {
		updateNextFrame = false;
		doRepaint();
	}
}