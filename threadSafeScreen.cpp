#include "threadSafeScreen.h"

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

bool threadSafeScreen::drawChar(wxDC& dc, char c, int x, int y) { //using Minecraftia as reference, but monospace
	switch (c) {
	//0-31 are all control chars
	case 32:
		return true; //space, draw blank
	case 33:
		dc.DrawLine(x + 2, y, x + 2, y + 4);
		dc.DrawPoint(x + 2, y + 6);
		return true; // !, drawn
	case 34:
		dc.DrawLine(x + 1, y, x + 1, y + 1);
		dc.DrawLine(x + 3, y, x + 3, y + 1);
		dc.DrawPoint(x, y + 2);
		dc.DrawPoint(x + 2, y + 2);
		return true; // ", drawn
	case 35:
		dc.DrawLine(x + 1, y, x + 1, y + 6);
		dc.DrawLine(x + 3, y, x + 3, y + 6);
		dc.DrawLine(x, y + 2, x + 4, y + 2);
		dc.DrawLine(x, y + 4, x + 4, y + 4);
		return true; // #, drawn
	case 36:
		dc.DrawPoint(x + 2, y);
		dc.DrawPoint(x + 2, y + 6);
		dc.DrawPoint(x, y + 2);
		dc.DrawPoint(x + 4, y + 4);
		dc.DrawLine(x + 1, y + 1, x + 4, y + 1);
		dc.DrawLine(x + 1, y + 3, x + 3, y + 3);
		dc.DrawLine(x, y + 5, x + 3, y + 5);
		return true; // $, drawn
	case 37:
		dc.DrawPoint(x + 4, y);
		dc.DrawPoint(x, y + 6);
		dc.DrawPoint(x + 2, y + 3);
		dc.DrawLine(x, y, x, y + 1);
		dc.DrawLine(x + 4, y + 5, x + 4, y + 6);
		dc.DrawLine(x + 3, y + 1, x + 3, y + 2);
		dc.DrawLine(x + 1, y + 4, x + 1, y + 5);
		return true; // %, drawn
	case 38:
		dc.DrawPoint(x + 2, y);
		dc.DrawPoint(x + 1, y + 1);
		dc.DrawPoint(x + 3, y + 1);
		dc.DrawLine(x + 2, y + 2, x + 2, y + 4);
		dc.DrawPoint(x + 1, y + 3);
		dc.DrawPoint(x + 4, y + 3);
		dc.DrawLine(x, y + 4, x, y + 5);
		dc.DrawLine(x + 3, y + 4, x + 3, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 2, y + 6);
		dc.DrawPoint(x + 4, y + 6);
		return true; // &, drawn
	case 39:
		dc.DrawLine(x + 2, y, x + 2, y + 1);
		dc.DrawPoint(x + 1, y + 2);
		return true; // ', drawn
	case 40:
		dc.DrawLine(x + 2, y, x + 3, y);
		dc.DrawPoint(x + 1, y + 1);
		dc.DrawLine(x, y + 2, x, y + 4);
		dc.DrawPoint(x + 1, y + 5);
		dc.DrawLine(x + 2, y + 6, x + 3, y + 6);
		return true; // (, drawn
	case 41:
		dc.DrawLine(x + 1, y, x + 2, y);
		dc.DrawPoint(x + 3, y + 1);
		dc.DrawLine(x + 4, y + 2, x + 4, y + 4);
		dc.DrawPoint(x + 3, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 2, y + 6);
		return true; // ), drawn
	case 42:
		dc.DrawPoint(x, y);
		dc.DrawPoint(x + 3, y);
		dc.DrawLine(x + 1, y + 1, x + 2, y + 1);
		dc.DrawPoint(x, y + 2);
		dc.DrawPoint(x + 3, y + 2);
		return true; // *, drawn
	case 43:
		dc.DrawLine(x, y + 3, x + 4, y + 3);
		dc.DrawLine(x + 2, y + 1, x + 2, y + 5);
		return true; // +, drawn
	case 44:
		dc.DrawLine(x, y + 5, x, y + 6);
		return true; // , drawn
	case 45:
		dc.DrawLine(x, y + 3, x + 4, y + 3);
		return true; // -, drawn
	case 46:
		dc.DrawLine(x, y + 4, x, y + 5);
		return true; // ., drawn
	case 47:
		dc.DrawPoint(x, y + 6);
		dc.DrawPoint(x + 4, y);
		dc.DrawPoint(x + 2, y + 3);
		dc.DrawLine(x + 3, y + 1, x + 3, y + 2);
		dc.DrawLine(x + 1, y + 4, x + 1, y + 5);
		return true; // /, drawn
	case 48:
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x, y + 1, x, y + 5);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		dc.DrawPoint(x + 3, y + 2);
		dc.DrawPoint(x + 2, y + 3);
		dc.DrawPoint(x + 1, y + 4);
		return true; // 0, drawn
	case 49:
		dc.DrawPoint(x + 1, y + 1);
		dc.DrawLine(x + 2, y, x + 2, y + 6);
		dc.DrawLine(x, y + 6, x + 4, y + 6);
		return true; // 1, drawn
	case 50:
		dc.DrawPoint(x, y + 1);
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 2);
		dc.DrawLine(x + 2, y + 3, x + 3, y + 3);
		dc.DrawPoint(x + 1, y + 4);
		dc.DrawPoint(x, y + 5);
		dc.DrawPoint(x + 4, y + 5);
		dc.DrawLine(x, y + 6, x + 4, y + 6);
		return true; // 2, drawn
	case 51:
		dc.DrawPoint(x, y + 1);
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 2);
		dc.DrawLine(x + 2, y + 3, x + 3, y + 3);
		dc.DrawLine(x + 4, y + 4, x + 4, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		dc.DrawPoint(x, y + 5);
		return true; // 3, drawn
	case 52:
		dc.DrawPoint(x + 3, y);
		dc.DrawPoint(x + 2, y + 1);
		dc.DrawPoint(x + 1, y + 2);
		dc.DrawPoint(x, y + 3);
		dc.DrawLine(x + 4, y, x + 4, y + 6);
		dc.DrawLine(x, y + 4, x + 3, y + 4);
		return true; // 4, drawn
	case 53:
		dc.DrawLine(x, y, x + 4, y);
		dc.DrawPoint(x, y + 1);
		dc.DrawLine(x, y + 2, x + 3, y + 2);
		dc.DrawLine(x + 4, y + 3, x + 4, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		dc.DrawPoint(x, y + 5);
		return true; // 5, drawn
	case 54:
		dc.DrawLine(x + 2, y, x + 3, y);
		dc.DrawPoint(x + 1, y + 1);
		dc.DrawLine(x, y + 2, x, y + 5);
		dc.DrawLine(x + 1, y + 3, x + 3, y + 3);
		dc.DrawLine(x + 4, y + 4, x + 4, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		return true; // 6, drawn
	case 55:
		dc.DrawPoint(x, y + 1);
		dc.DrawLine(x, y, x + 4, y);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 2);
		dc.DrawPoint(x + 3, y + 3);
		dc.DrawLine(x + 2, y + 4, x + 2, y + 6);
		return true; // 7, drawn
	case 56:
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x, y + 1, x, y + 2);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 2);
		dc.DrawLine(x + 1, y + 3, x + 3, y + 3);
		dc.DrawLine(x, y + 4, x, y + 5);
		dc.DrawLine(x + 4, y + 4, x + 4, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		return true; // 8, drawn
	case 57:
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x, y + 1, x, y + 2);
		dc.DrawLine(x + 1, y + 3, x + 3, y + 3);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 4);
		dc.DrawPoint(x + 3, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 2, y + 6);
		return true; // 9, drawn
	case 58:
		dc.DrawLine(x, y + 1, x, y + 2);
		dc.DrawLine(x, y + 5, x, y + 6);
		return true; // :, drawn
	case 59:
		dc.DrawLine(x, y + 1, x, y + 2);
		dc.DrawLine(x, y + 4, x, y + 6);
		return true; // ;, drawn
	case 60:
		dc.DrawPoint(x + 3, y);
		dc.DrawPoint(x + 2, y + 1);
		dc.DrawPoint(x + 1, y + 2);
		dc.DrawPoint(x, y + 3);
		dc.DrawPoint(x + 1, y + 4);
		dc.DrawPoint(x + 2, y + 5);
		dc.DrawPoint(x + 3, y + 6);
		return true; // <, drawn
	case 61:
		dc.DrawLine(x, y + 1, x + 4, y + 1);
		dc.DrawLine(x, y + 4, x + 4, y + 4);
		return true; // =, drawn
	case 62:
		dc.DrawPoint(x + 1, y);
		dc.DrawPoint(x + 2, y + 1);
		dc.DrawPoint(x + 3, y + 2);
		dc.DrawPoint(x + 4, y + 3);
		dc.DrawPoint(x + 3, y + 4);
		dc.DrawPoint(x + 2, y + 5);
		dc.DrawPoint(x + 1, y + 6);
		return true; // >, drawn
	case 63:
		dc.DrawPoint(x, y + 1);
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 2);
		dc.DrawPoint(x + 3, y + 3);
		dc.DrawPoint(x + 2, y + 4);
		dc.DrawPoint(x + 2, y + 6);
		return true; // ?, drawn
	case 64:
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x, y + 1, x, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 4);
		dc.DrawPoint(x + 3, y + 4);
		dc.DrawLine(x + 2, y + 2, x + 2, y + 4);
		return true; // @, kind of drawn; Minecraftia cheats by using 6x7 pixels instead of 5x7 like for all the other chars
	case 65:
		dc.DrawLine(x, y + 1, x, y + 6);
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 6);
		dc.DrawLine(x + 1, y + 3, x + 3, y + 3);
		return true; // A, drawn
	case 66:
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x, y, x, y + 6);
		dc.DrawPoint(x + 4, y + 1);
		dc.DrawLine(x + 1, y + 2, x + 3, y + 2);
		dc.DrawLine(x + 4, y + 3, x + 4, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		return true; // B, drawn
	case 67:
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawPoint(x + 4, y + 1);
		dc.DrawLine(x, y + 1, x, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		dc.DrawPoint(x + 4, y + 5);
		return true; // C, drawn
	case 68:
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x, y, x, y + 6);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 5);
		return true; // D, drawn
	case 69:
		dc.DrawLine(x + 1, y, x + 4, y);
		dc.DrawLine(x, y, x, y + 6);
		dc.DrawLine(x + 1, y + 2, x + 3, y + 2);
		dc.DrawLine(x + 1, y + 6, x + 4, y + 6);
		return true; // E, drawn
	case 70:
		dc.DrawLine(x, y, x + 4, y);
		dc.DrawLine(x, y + 1, x, y + 6);
		dc.DrawLine(x + 1, y + 2, x + 3, y + 2);
		return true; // F, drawn
	case 71:
		dc.DrawLine(x + 1, y, x + 4, y);
		dc.DrawLine(x, y + 1, x, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		dc.DrawLine(x + 4, y + 2, x + 4, y + 5);
		dc.DrawLine(x + 2, y + 2, x + 4, y + 2);
		return true; // G, drawn
	case 72:
		dc.DrawLine(x, y, x, y + 6);
		dc.DrawLine(x + 4, y, x + 4, y + 6);
		dc.DrawLine(x + 1, y + 2, x + 3, y + 2);
		return true; // H, drawn
	case 73:
		dc.DrawLine(x, y, x + 4, y);
		dc.DrawLine(x + 2, y + 1, x + 2, y + 5);
		dc.DrawLine(x, y + 6, x + 4, y + 6);
		return true; // I, drawn
	case 74:
		dc.DrawPoint(x, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		dc.DrawLine(x + 4, y, x + 4, y + 5);
		return true; // J, drawn
	case 75:
		dc.DrawLine(x, y, x, y + 6);
		dc.DrawLine(x + 1, y + 2, x + 2, y + 2);
		dc.DrawPoint(x + 3, y + 1);
		dc.DrawPoint(x + 4, y);
		dc.DrawPoint(x + 3, y + 3);
		dc.DrawLine(x + 4, y + 4, x + 4, y + 6);
		return true; // K, drawn
	case 76:
		dc.DrawLine(x, y, x, y + 5);
		dc.DrawLine(x, y + 6, x + 4, y + 6);
		return true; // L, drawn
	case 77:
		dc.DrawLine(x, y, x, y + 6);
		dc.DrawLine(x + 4, y, x + 4, y + 6);
		dc.DrawPoint(x + 1, y + 1);
		dc.DrawPoint(x + 3, y + 1);
		dc.DrawPoint(x + 2, y + 2);
		return true; // M, drawn
	case 78:
		dc.DrawLine(x, y, x, y + 6);
		dc.DrawLine(x + 4, y, x + 4, y + 6);
		dc.DrawPoint(x + 1, y + 1);
		dc.DrawPoint(x + 2, y + 2);
		dc.DrawPoint(x + 3, y + 3);
		return true; // N, drawn
	case 79:
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x, y + 1, x, y + 5);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		return true; // O, drawn
	case 80:
		dc.DrawLine(x, y, x, y + 6);
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawPoint(x + 4, y + 1);
		dc.DrawLine(x + 1, y + 2, x + 3, y + 2);
		return true; // P, drawn
	case 81:
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawLine(x, y + 1, x, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 2, y + 6);
		dc.DrawLine(x + 4, y + 1, x + 4, y + 4);
		dc.DrawPoint(x + 3, y + 5);
		dc.DrawPoint(x + 4, y + 6);
		return true; // Q, drawn
	case 82:
		dc.DrawLine(x, y, x, y + 6);
		dc.DrawLine(x + 1, y, x + 3, y);
		dc.DrawPoint(x + 4, y + 1);
		dc.DrawLine(x + 1, y + 2, x + 3, y + 2);
		dc.DrawLine(x + 4, y + 3, x + 4, y + 6);
		return true; // R, drawn
	case 83:
		dc.DrawLine(x + 1, y, x + 4, y);
		dc.DrawPoint(x, y + 1);
		dc.DrawLine(x + 1, y + 2, x + 3, y + 2);
		dc.DrawLine(x + 4, y + 3, x + 4, y + 5);
		dc.DrawPoint(x, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		return true; // S, drawn
	case 84:
		dc.DrawLine(x, y, x + 4, y);
		dc.DrawLine(x + 2, y + 1, x + 2, y + 6);
		return true; // T, drawn
	case 85:
		dc.DrawLine(x, y, x, y + 5);
		dc.DrawLine(x + 4, y, x + 4, y + 5);
		dc.DrawLine(x + 1, y + 6, x + 3, y + 6);
		return true; // U, drawn
	case 86:
		dc.DrawLine(x, y, x, y + 3);
		dc.DrawLine(x + 4, y, x + 4, y + 3);
		dc.DrawLine(x + 1, y + 4, x + 1, y + 5);
		dc.DrawLine(x + 3, y + 4, x + 3, y + 5);
		dc.DrawPoint(x + 2, y + 6);
		return true; // V, drawn

	}
}