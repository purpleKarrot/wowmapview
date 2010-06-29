#include "EquipmentControl.hpp"
#include <QGridLayout>
#include <QLabel>

EquipmentControl::EquipmentControl(CharControl& old) :
	old(old)
{
	QGridLayout* layout = new QGridLayout(this);

#define ADD_CONTROLS(type, caption)                        \
	buttons[type] = new QPushButton("-- None --", this);   \
	layout->addWidget(new QLabel(caption, this), type, 0); \
	layout->addWidget(buttons[type], type, 1);

	ADD_CONTROLS(CS_HEAD, "Head")
	ADD_CONTROLS(CS_SHOULDER, "Shoulder")

	ADD_CONTROLS(CS_SHIRT, "Shirt")
	ADD_CONTROLS(CS_CHEST, "Chest")
	ADD_CONTROLS(CS_BELT, "Belt")
	ADD_CONTROLS(CS_PANTS, "Legs")
	ADD_CONTROLS(CS_BOOTS, "Boots")

	ADD_CONTROLS(CS_BRACERS, "Bracers")
	ADD_CONTROLS(CS_GLOVES, "Gloves")
	ADD_CONTROLS(CS_CAPE, "Cape")

	ADD_CONTROLS(CS_HAND_RIGHT, "Right hand")
	ADD_CONTROLS(CS_HAND_LEFT, "Left hand")

	ADD_CONTROLS(CS_QUIVER, "Quiver")
	ADD_CONTROLS(CS_TABARD, "Tabard")

	ADD_CONTROLS(CS_MOUNT, "Mount")
	connect(buttons[CS_MOUNT],SIGNAL(clicked()),this,SLOT(choose_mount()));

#undef ADD_CONTROLS

	setLayout(layout);
}

EquipmentControl::~EquipmentControl()
{
}

void EquipmentControl::choose_mount()
{
	old.selectMount();
}
