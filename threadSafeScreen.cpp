#include "threadSafeScreen.h"
#include "font.h"

wxImage threadSafeScreen::getImage() {
	wxCriticalSectionLocker locker(lock);
	return image;
}

void threadSafeScreen::setImage(wxImage image) {
	wxCriticalSectionLocker locker(lock);
	initialized = true;
	threadSafeScreen::image = image;
}

void threadSafeScreen::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	wxCriticalSectionLocker locker(lock);
	image.SetRGB(x, y, r, g, b);
}

void threadSafeScreen::drawLine(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b) {
	draw([=](wxDC& dc) {
		dc.DrawLine(x1, y1, x2, y2);
	}, r, g, b);
}

void threadSafeScreen::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r, unsigned char g, unsigned char b) {
	draw([=](wxDC& dc) {
		const wxPoint points[] = {wxPoint(x1,y1),wxPoint(x2,y2),wxPoint(x3,y3)};
		dc.DrawPolygon(3, points);
	}, r, g, b);
}

void threadSafeScreen::drawRectangle(int x, int y, int width, int height, unsigned char r, unsigned char g, unsigned char b) {
	draw([=](wxDC& dc) {
		dc.DrawRectangle(x, y, width, height);
	}, r, g, b);
}

void threadSafeScreen::drawPolygon(std::vector<wxPoint> vertices, unsigned char r, unsigned char g, unsigned char b) {
	draw([=](wxDC& dc) {
		dc.DrawPolygon(vertices.size(), &vertices[0]);
	}, r, g, b);
}

void threadSafeScreen::drawEllipse(int x, int y, int width, int height, unsigned char r, unsigned char g, unsigned char b) {
	draw([=](wxDC& dc) {
		dc.DrawEllipse (x, y, width, height);
	}, r, g, b);
}

void threadSafeScreen::drawText(int x, int y, std::string text, unsigned char r, unsigned char g, unsigned char b) {
	draw([=](wxDC& dc) {
		int xCurrent = x;
		int yCurrent = y;
		for (char c : text) {
			if (c == '\n' || (xCurrent + 5) > image.GetSize().GetX()) {
				yCurrent += 8;
				xCurrent = x;
			}
			if (drawChar(dc, c, xCurrent, yCurrent)) {
				xCurrent += 6;
			}
		}
	}, r, g, b);
}

bool threadSafeScreen::drawChar(wxDC& dc, char c, int x, int y) {
	std::bitset<36> charBitmap = font::loadChar(c);
	if (charBitmap.test(0)) { //for some reason 0 is rightmost bit
		int i = 35;
		for (int yl = 0; yl < 7; yl++) {
			for (int xl = 0; xl < 5; xl++) {
				if (charBitmap.test(i)) {
					dc.DrawPoint(xl + x, yl + y);
				}
				i--;
			}
		}
		return true;
	} else return false;
}

void threadSafeScreen::draw(std::function<void(wxDC&)> callback, unsigned char r, unsigned char g, unsigned char b) {
	wxCriticalSectionLocker locker(lock);
	wxBitmap bitmap(image, -1);
	wxMemoryDC dc(bitmap);
	wxPen* pen = wxThePenList->FindOrCreatePen(wxColor(r, g, b));
	wxBrush* brush = wxTheBrushList->FindOrCreateBrush(wxColor(r, g, b));
	dc.SetPen(*pen);
	dc.SetBrush(*brush);
	callback(dc);
	image = bitmap.ConvertToImage();
}