#include "FileList.hpp"
#include "FileListModel.hpp"
#include <QVBoxLayout>
#include <QHeaderView>
#include <iostream>
#include <boost/algorithm/string/predicate.hpp>
#include "mpq.hpp"
#include "../globalvars.h"

FileList::FileList()
{
	combo_box = new QComboBox(this);
	combo_box->addItems( //
		QStringList() << "Models" << "WMOs" << "Sounds" << "Images" << "ADTs");
	connect(combo_box, SIGNAL(activated(const QString&)), //
		this, SLOT(filter(const QString&)));

	tree_view = new QTreeView(this);
	tree_view->header()->hide();

	//	file_list_model = new FileListModel(this);
	//	tree_view->setModel(file_list_model);

	connect(tree_view, SIGNAL(doubleClicked(const QModelIndex&)), //
		this, SLOT(select(const QModelIndex&)));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(combo_box);
	layout->addWidget(tree_view);
	setLayout(layout);
}

FileList::~FileList()
{
}

static inline bool filterModelsSearch(std::string const& s)
{
	return boost::algorithm::iends_with(s, ".m2");
}

static inline bool filterWMOsSearch(std::string const& s)
{
	return boost::algorithm::iends_with(s, ".wmo");
}

static inline bool filterSoundsSearch(std::string const& s)
{
	return boost::algorithm::iends_with(s, ".wav")
		|| boost::algorithm::iends_with(s, ".mp3");
}

static inline bool filterGraphicsSearch(std::string const& s)
{
	return boost::algorithm::iends_with(s, ".blp");
}

static inline bool filterADTsSearch(std::string const& s)
{
	return boost::algorithm::iends_with(s, ".adt");
}

void FileList::filter(const QString& text)
{
	std::set<FileTreeItem> filelist;

	if (text == "Models")
		FS().getFileLists(filelist, filterModelsSearch);
	else if (text == "WMOs")
		FS().getFileLists(filelist, filterWMOsSearch);
	else if (text == "Sounds")
		FS().getFileLists(filelist, filterSoundsSearch);
	else if (text == "Images")
		FS().getFileLists(filelist, filterGraphicsSearch);
	else if (text == "ADTs")
		FS().getFileLists(filelist, filterADTsSearch);

	file_list_model = new FileListModel(this);

	for (std::set<FileTreeItem>::iterator it = filelist.begin(); it
		!= filelist.end(); ++it)
	{
		file_list_model->add_path(it->file_name, it->color);
	}

	tree_view->setModel(file_list_model);
}

void FileList::select(const QModelIndex& index)
{
	if (!file_list_model->hasChildren(index))
	{
		std::string path =
			static_cast<TreeItem*> (index.internalPointer())->path();

		if (filterModelsSearch(path))
			g_modelViewer->LoadModel(path);
	}
}
