#include "mainApp.h"
#include <wx/cmdline.h>
#include "ArduinoReader.h"
#include <wx/msgout.h>

//This stuff would usually be created automatically with wxIMPLEMENT_APP; however, I wanted to create my own main function, and the easiest way seemed to be skipping this definition
mainApp& wxGetApp() {
	return *static_cast<mainApp*>(wxApp::GetInstance());
}
wxAppConsole* wxCreateApp() {
	return new mainApp;
}
wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction)wxCreateApp);
//default code end

int main(int argc, char** argv) {
	return wxEntry(argc, argv);
}

bool mainApp::OnInit() {
	wxMessageOutputStderr cmdLineOutput(stdout);
	wxMessageOutput* previous = wxMessageOutput::Set(&cmdLineOutput);
	static const wxCmdLineEntryDesc cmdLineDesc[] = {
		{wxCMD_LINE_PARAM, NULL, NULL, "port", wxCMD_LINE_VAL_STRING},
		{wxCMD_LINE_USAGE_TEXT, NULL, NULL, "  port must be a COM port, e.g. COM3"},
		{wxCMD_LINE_OPTION, "b", "baud", "Baud rate (9600 default)", wxCMD_LINE_VAL_NUMBER},
		{wxCMD_LINE_OPTION, "f", "framerate", "Frame rate (60 default)", wxCMD_LINE_VAL_DOUBLE},
		{wxCMD_LINE_SWITCH, "r", "reset", "Reset arduino before connecting"},
		{wxCMD_LINE_SWITCH, "d", "debug", "Enable extra debug messages"},
		{wxCMD_LINE_SWITCH, "h", "help", "Display this message", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
		{wxCMD_LINE_NONE}
	};
	wxCmdLineParser parser(cmdLineDesc, argc, argv);
	if (!parser.Parse()) {//parse returns 0 if all required params are present and no syntax errors
		long baud = 9600;
		parser.Found("b", &baud);
		double framerate = 60;
		parser.Found("f", &framerate);
		wxMessageOutput::Set(previous);
		wxInitAllImageHandlers();
		tss = new threadSafeScreen();
		window = new mainWindow(wxSize(500, 500), tss, 1000 / framerate);
		reader = new ArduinoReader(window, tss, parser.Found("r"), parser.Found("d"), parser.GetParam(0).ToStdString(), baud);
		window->SetIcon(wxICON(LOGO));
		window->Show();
		wxMessageOutput::Set(previous);
		return true;
	}
	wxMessageOutput::Set(previous);
	return false;
}


int mainApp::OnExit() {
	delete reader;
	delete tss;
	return 0;
}