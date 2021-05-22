#include "mainWindow.h"
#include <string>
#include <wx/choicdlg.h>

wxDEFINE_EVENT(NEEDS_REPAINT, wxCommandEvent);

wxBEGIN_EVENT_TABLE(mainWindow, wxFrame)
	EVT_SIZE(mainWindow::onSize)
	EVT_COMMAND(WINDOW_ID, NEEDS_REPAINT, mainWindow::onNeedsRepaint)
wxEND_EVENT_TABLE()

mainWindow::mainWindow(wxSize size, threadSafeScreen* tss) : wxFrame(nullptr, WINDOW_ID, "VirtualScreen", wxPoint(200, 200), size), tss(tss) {
	renderSurface = new wxWindow(this, wxID_ANY, wxDefaultPosition, size);
	renderSurface->SetBackgroundStyle(wxBG_STYLE_PAINT);
	renderSurface->Bind(wxEVT_PAINT, &mainWindow::onPaint, this);
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

void mainWindow::onNeedsRepaint(wxCommandEvent& event) {
	doRepaint();
}

void mainWindow::sendNeedsRepaint() {
	wxCommandEvent* event = new wxCommandEvent(NEEDS_REPAINT, WINDOW_ID);
	event->SetEventObject(this);
	GetEventHandler()->QueueEvent(event);
}