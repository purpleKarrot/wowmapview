#ifndef TREEITEM_HPP
#define TREEITEM_HPP

#include <QList>
#include <string>
#include <algorithm>

class TreeItem
{
public:
	TreeItem(const std::string& name = std::string(), TreeItem* parent = 0) :
		name_(name), color_(0), parent_(parent)
	{
	}

	~TreeItem()
	{
		qDeleteAll(children_);
	}

	TreeItem* parent() const
	{
		return parent_;
	}

	TreeItem* child(int row) const
	{
		return children_[row];
	}

	TreeItem* child(const std::string& name);

	int children() const
	{
		return children_.size();
	}

	int row() const
	{
		if (!parent_)
			return 0;

		return parent_->children_.indexOf(const_cast<TreeItem*> (this));
	}

	std::string name() const
	{
		return name_;
	}

	std::string path() const
	{
		return path_;
	}

	void path(const std::string& p)
	{
		path_ = p;
	}

	int color() const
	{
		return color_;
	}

	void color(int c)
	{
		if (color_ < c)
			color_ = c;
	}

private:
	std::string name_;
	std::string path_;
	int color_;

	TreeItem* parent_;
	QList<TreeItem*> children_;
};

#endif /* TREEITEM_HPP */
