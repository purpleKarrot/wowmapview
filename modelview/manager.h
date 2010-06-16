#ifndef MANAGER_H
#define MANAGER_H

#pragma warning( disable : 4100 )

// wxWidgets
#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// STL
#include <string>
#include <map>

// base class for manager objects

class ManagedItem {
	int refcount;
public:
	std::string name;
	ManagedItem(std::string n): name(n), refcount(0) { }
	virtual ~ManagedItem() {}

	void addref()
	{
		++refcount;
	}

	bool delref()
	{
		return --refcount==0;
	}
	
};



template <class IDTYPE>
class Manager {
public:
	std::map<std::string, IDTYPE> names;
	std::map<IDTYPE, ManagedItem*> items;

	Manager()
	{
	}

	virtual IDTYPE add(std::string name) = 0;

	virtual void del(IDTYPE id)
	{
		if (items.find(id) == items.end())  {
			doDelete(id);
			return; // if we can't find the item id, delete the texture
		}

		if (items[id]->delref()) {
			ManagedItem *i = items[id];

			if (!i)
				return;

			#ifdef _DEBUG
				wxLogMessage(_T("Unloading Texture: %s"), i->name.c_str());
			#endif

			doDelete(id);
			names.erase(names.find(i->name));
			items.erase(items.find(id));

			wxDELETE(i);
		}
	}

	void delbyname(std::string name)
	{
		if (has(name)) 
			del(get(name));
	}

	virtual void doDelete(IDTYPE id) {}

	bool has(std::string name)
	{
		return (names.find(name) != names.end());
	}

	IDTYPE get(std::string name)
	{
		return names[name];
	}

	std::string get(IDTYPE id)
	{
		return names[id];
	}

	void clear()
	{
		/*
		for (std::map<IDTYPE, ManagedItem*>::iterator it=items.begin(); it!=items.end(); ++it) {
			ManagedItem *i = (*it);
			
			wxDELETE(i);
		}
		*/

		for (int i=0; i<50; i++) {
			if(items.find(i) != items.end()) {
				del(i);
			}
		}
		
		names.clear();
		items.clear();
	}

protected:
	void do_add(std::string name, IDTYPE id, ManagedItem* item)
	{
		names[name] = id;
		item->addref();
		items[id] = item;
	}
};

class SimpleManager : public Manager<int> {
	int baseid;
public:
	SimpleManager() : baseid(0)
	{
	}

protected:
	int nextID()
	{
		return baseid++;
	}
};

#endif

