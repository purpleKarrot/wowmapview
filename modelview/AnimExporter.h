#ifndef GIFEXPORTER_H
#define GIFEXPORTER_H

#include "util.h"

// wxwidgets
#include "wx/wx.h"
//#include "wx/frame.h"
//#include "wx/stattext.h"
//#include "wx/button.h"

#include "modelcanvas.h"
#ifdef _WINDOWS
#include "AVIGenerator.h"
#endif

#include "CxImage/ximage.h" // RGBQUAD

// File Source Change log:
// Version | Date     | Comments
// -----------------------------------------------------------------------------------------------------
// 0.5.07  | 3/02/07  | Minor changes to the code to make it more standardised using hungarian notation.
//					  | Merged the "Gif" and the "Avi" exporter into one class object.
// 

// Credit goes out to the Warcraft3 Viewer
class CAnimationExporter : public wxFrame
{
	DECLARE_CLASS(CAnimationExporter)
    DECLARE_EVENT_TABLE()

private:
	unsigned int m_iTotalAnimFrames;		// Total frames of the model animation
	unsigned int m_iTotalFrames;			// Total frames of the animated gif
	unsigned int m_iWidth, m_iHeight;		// Width and Height of the source image
	unsigned long m_iNewWidth, m_iNewHeight;// New width and height of the output image
	unsigned long m_iDelay;					// Delay between frames

	bool m_bTransparent, m_bDiffuse, m_bShrink, m_bGreyscale, m_bPng;	// Various options and toggles settings

	unsigned long m_iSize;		// Size of our data buffer to hold the pixel data

	float m_fAnimSpeed;			// Animation Speed
	int m_iTimeStep;			// frame difference between each frame
	RGBQUAD *m_pPal;

	wxString m_strFilename;		// Filename to save our animated gif into.

	//GUI objects
	wxStaticText *lblCurFrame, *lblFile;
	wxButton *btnStart, *btnCancel;
	wxTextCtrl *txtFrames, *txtSizeX, *txtSizeY, *txtDelay;
	wxCheckBox *cbTrans, *cbDither, *cbShrink, *cbGrey, *cbPng;

public:
	CAnimationExporter(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE|wxCAPTION|wxFRAME_NO_TASKBAR);
	~CAnimationExporter();


	// gui functions
	// ------------------------------------------
	void OnButton(wxCommandEvent &event);
	void OnCheck(wxCommandEvent &event);


	// gif export functions
	// ------------------------------------------
	void Init(const wxString fn = _T("temp.gif"));
	void CreateGif();


	// avi export functions
	// ------------------------------------------
	void CreateAvi(wxString fn);

	
};



#endif

