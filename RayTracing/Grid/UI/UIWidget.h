#pragma once
#include <QtWidgets\qwidget.h>

class UIWidget : QWidget
{
public:
	UIWidget(void);
	virtual ~UIWidget(void);
	virtual void update() = 0;
};

