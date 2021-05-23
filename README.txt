A little project I made to be able to get visual output from my arduino. Targets windows, but since wxWidgets is cross platform all that would have to be changed to support other platforms is the serial stuff.

Depends on wxWidgets; I used 3.1.3, but any version post 3.0 should work.

Protocol:
all integers signed, all bytes unsigned
4-byte integers, big endian
The exe sends VS START to the arduino, and waits to see VS START back. Then, the next 2 integers are treated as the width and height, respectively.
After that, it's just a very basic packet system, with the first byte of each packet being the type.
0: send buffer. width * height * 3 bytes.
1: set pixel. 2 integers for x,y, 3 bytes for rgb.
2: draw line. 4 integers for x1,y1,x2,y2, 3 bytes for rgb.
3: draw triangle. 6 integers for x1,y1,x2,y2,x3,y3, 3 bytes for rgb
4: draw rectangle. 2 integers for top left corner, 2 integers for width,height, and 3 bytes for rgb
5: draw polygon. 1 byte for number of vertices, 2 integers per vertex (x,y), 3 bytes for rgb
6: draw ellipse. 2 integers for top left corner, 2 integers for width,height, 3 bytes for rgb.

Command usage:
VirtualScreen [/b <num>] [/f <double>] [/r] [/d] [/h] port
  port must be a COM port, e.g. COM3
  /b, --baud=<num>              Baud rate (9600 default)
  /f, --framerate=<double>      Frame rate (60 default)
  /r, --reset                   Reset arduino before connecting
  /d, --debug                   Enable extra debug messages
  /h, --help                    Display this message