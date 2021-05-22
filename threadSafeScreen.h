#pragma once
#include "wx/wx.h"
class threadSafeScreen {
	wxImage image;
	wxCriticalSection lock;
	void draw(std::function<void(wxDC&)>, unsigned char r, unsigned char g, unsigned char b);
	threadSafeScreen(threadSafeScreen const&) = delete; //atomic_bool can't be copied or moved, nor can a wxCriticalSection since it uses a mutex
	threadSafeScreen& operator =(threadSafeScreen const&) = delete;
	bool drawChar(wxDC& dc, char c, int x, int y);
public:
	threadSafeScreen() = default;
	std::atomic_bool initialized;
	wxImage getImage();
	void setImage(wxImage image);
	void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	void drawLine(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b);
	void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r, unsigned char g, unsigned char b);
	void drawRectangle(int x, int y, int width, int height, unsigned char r, unsigned char g, unsigned char b);
	void drawPolygon(std::vector<wxPoint> vertices, unsigned char r, unsigned char g, unsigned char b);
	void drawEllipse(int x, int y, int width, int height, unsigned char r, unsigned char g, unsigned char b);
};

