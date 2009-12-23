#ifndef MANAGER_H
#define MANAGER_H

#include <string>
#include <map>

// base class for manager objects

class ManagedItem {
	int refcount;
public:
	std::string name;
	ManagedItem(std::string n): name(n), refcount(0) {}
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
		if (items[id]->delref()) {
			ManagedItem *i = items[id];
			doDelete(id);
			names.erase(names.find(i->name));
			items.erase(items.find(id));
			delete i;
		}
	}

	void delbyname(std::string name)
	{
		if (has(name)) del(get(name));
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

