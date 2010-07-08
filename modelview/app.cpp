#include <GL/glew.h>
#include <QApplication>
#include "widgets/MainWindow.hpp"
#include "app.h"
#include "globalvars.h"
#include <wx/dir.h>
#include <wx/tokenzr.h>

IMPLEMENT_APP_NO_MAIN(WowModelViewApp)

static bool qt_running = true;

void QtDummy::shutdown()
{
	qt_running = false;
}

int main(int argc, char **argv)
{
	QApplication qapp(argc, argv);
	QtDummy qtdummy;
	QObject::connect(&qapp, SIGNAL(lastWindowClosed()), //
		&qtdummy, SLOT(shutdown()));

	MainWindow main_window;
	main_window.show();

	return wxEntry(argc, argv);
}

int WowModelViewApp::MainLoop()
{
	while (wxAppBase::Pending() || qt_running)
	{
		if (wxAppBase::Pending())
			wxAppBase::Dispatch();

		if (qt_running)
			QApplication::processEvents();
	}

	return 0;
}

bool WowModelViewApp::OnInit()
{
	wxLog::SetActiveTarget(new wxLogStream(&std::clog));

	wxFileName fname(argv[0]);
	wxString userPath = fname.GetPath(wxPATH_GET_VOLUME) + SLASH
		+ wxT("userSettings");
	wxFileName::Mkdir(userPath, 0777, wxPATH_MKDIR_FULL);

	// Application Info
	SetVendorName(_T("WoWModelViewer"));
	SetAppName(_T("WoWModelViewer"));

	// Now create our main frame.
	frame = new ModelViewer();
	SetTopWindow(frame);

	// Point our global vars at the correct memory location
	g_modelViewer = frame;
	g_canvas = frame->canvas;
	g_animControl = frame->animControl;

	frame->interfaceManager.Update();

	if (frame->canvas)
	{
		frame->canvas->Show(true);

		if (!frame->canvas->init)
			frame->canvas->InitGL();
	}

	return true;
}
