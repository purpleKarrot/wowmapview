#ifndef ITEMSELECTION_H
#define ITEMSELECTION_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// wx
#include "wx/regex.h"
#include "wx/choicdlg.h"
#include "wx/listctrl.h"

// stl
#include <vector>

wxColour ItemQualityColour(int quality);

class CharControl;

class ChoiceDialog : public wxSingleChoiceDialog {
	CharControl *cc;
	int type;

    DECLARE_EVENT_TABLE()

public:
	wxListView *m_listctrl;
	ChoiceDialog(CharControl *dest, int type,
	                       wxWindow *parent,
                           const wxString& message,
                           const wxString& caption,
                           const wxArrayString& choices);

	virtual void OnClick(wxCommandEvent &event);
	void OnSelect(wxListEvent &event);
    virtual int GetSelection() const { return m_selection; }
	void EndModal(int retCode) { SetReturnCode(retCode); Hide(); }
};



class FilteredChoiceDialog: public ChoiceDialog {
protected:
    wxTextCtrl* m_pattern;
    const wxArrayString* m_choices;
    std::vector<int> m_indices; // filtered index -> orig inndex

    DECLARE_EVENT_TABLE()

	wxRegEx filter;

public:
    enum{
        ID_FILTER_TEXT = 1000,
        ID_FILTER_BUTTON,
        ID_FILTER_CLEAR
    };

	bool keepFirst;

    FilteredChoiceDialog(CharControl *dest, int type,
	                       wxWindow *parent,
                           const wxString& message,
                           const wxString& caption,
                           const wxArrayString& choices,
						   const std::vector<int> *quality,
						   bool keepfirst = true);

    virtual void OnFilter(wxCommandEvent& event);
    virtual int GetSelection() const { return m_indices[m_selection]; }

	virtual void InitFilter();
	virtual void DoFilter();
	virtual	bool FilterFunc(int index);
};


class CategoryChoiceDialog: public FilteredChoiceDialog
{
protected:
	const std::vector<int>& m_cats;

	wxCheckListBox *m_catlist;
	int numcats;

DECLARE_EVENT_TABLE()

public:
	enum
	{
		ID_CAT_LIST = 1100
	};

	CategoryChoiceDialog(CharControl *dest, int type, wxWindow *parent,
		const wxString& message, const wxString& caption,
		const wxArrayString& choices, const std::vector<int> &cats,
		const wxArrayString& catnames, const std::vector<int> *quality,
		bool keepfirst = true, bool helpmsg = true);

	void Check(int index, bool state = true);
	void OnCheck(wxCommandEvent &e);
	void OnCheckDoubleClick(wxCommandEvent &e);
	bool FilterFunc(int index);
};

#endif

