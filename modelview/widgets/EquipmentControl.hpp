#ifndef EQUIPMENTCONTROL_HPP
#define EQUIPMENTCONTROL_HPP

#include <QWidget>
#include <QPushButton>
#include "../charcontrol.h"

class EquipmentControl: public QWidget
{
Q_OBJECT

public:
	EquipmentControl(CharControl& old);
	~EquipmentControl();

private:
	CharControl& old;
	QPushButton* buttons[NUM_CHAR_SLOTS];
};

#endif /* EQUIPMENTCONTROL_HPP */
