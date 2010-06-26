#include "TreeItem.hpp"

class Match
{
public:
	Match(const std::string& left) :
		left(left)
	{
	}

	bool operator()(TreeItem* right) const
	{
		return left == right->name();
	}

private:
	std::string left;
};

TreeItem* TreeItem::child(const std::string& name)
{
	QList<TreeItem*>::iterator iterator = std::find_if(children_.begin(),
		children_.end(), Match(name));

	if (iterator != children_.end())
		return *iterator;

	TreeItem* item = new TreeItem(name, this);
	children_.append(item);
	return item;
}
