#ifndef FILECONTROL_H
#define FILECONTROL_H

class FileControl: public wxWindow
{
	DECLARE_CLASS(FileControl)
	DECLARE_EVENT_TABLE()

	std::set<FileTreeItem> filelist;
public:
	// Constructor + Deconstructor
	FileControl(wxWindow* parent, wxWindowID id);
	~FileControl();

	void Init(ModelViewer* mv=NULL);
	void OnTreeSelect(wxTreeEvent &event);
	void OnTreeCollapsedOrExpanded(wxTreeEvent &event);
	void OnButton(wxCommandEvent &event);
	void OnChoice(wxCommandEvent &event);
	void OnTreeMenu(wxTreeEvent &event);
	void OnPopupClick(wxCommandEvent &evt);
	void Export(wxString val, int select);
	void ExportPNG(wxString val, wxString suffix);

	wxTreeCtrl *fileTree;
	wxButton *btnSearch;
	wxTextCtrl *txtContent;
	wxChoice *choFilter;
	wxTreeItemId CurrentItem;
#ifdef	PLAY_MUSIC
	wxMediaCtrl *mcPlayer;
#endif
	int filterMode;

	ModelViewer* modelviewer; // point to parent
	
private:
	void ClearCanvas();
	void UpdateInterface();
};

class FileTreeData:public wxTreeItemData
{
public:
	std::string fn;
	FileTreeData(std::string fn):fn(fn) {}
};

#endif