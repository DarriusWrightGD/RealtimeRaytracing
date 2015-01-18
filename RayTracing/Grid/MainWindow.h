#pragma once

#include <QtWidgets\qmainwindow.h>
#include <QtWidgets\qlayout.h>
#include <QtWidgets\qaction.h>
#include <QtCore\qstring.h>
#include <QtWidgets\qmenu.h>
#include <QtWidgets\qmenubar.h>
#include <QtWidgets\qdialog.h>
#include <QtWidgets\qfiledialog.h>
#include <QtCore\qdebug.h>
#include <QtWidgets\qlayout.h>
#include <QtWidgets\qcheckbox.h>
#include "RenderWindow.h"
#include <string>
#include <QtWidgets\qlabel.h>
#include <QtWidgets\qtabwidget.h>
#include "UI\FloatSlider.h"
#include "UI\IntSlider.h"
#include "UI\Float3Slider.h"
#include <QtWidgets\qcombobox.h>
#include <QtQml\qqmlengine.h>
#include <QtQml\qqmlcomponent.h>
#include <QtQuick\qquickitem.h>
#include <QtQml\qqmlapplicationengine.h>
#include <QtQuick\qquickview.h>
#include <QtQml\qqmlproperty.h>

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(RenderWindow * render);
	~MainWindow(void);
	/*
	number of samples
	number of bounces 
	*/


private:
	void initializeViews();
	void connectSignals();
	void connectQMLSignals();


	QQmlApplicationEngine engine;
	//QQmlEngine engine;
	QQmlComponent leftComponent;
	QQmlComponent rightComponent;
	QObject * object;
	QQuickItem * rightQml;
	QQuickItem * leftQml;
	QWidget * rightContainer;
	QWidget * leftContainer;
	QObject * rightObject;

	QWidget * mainWidget;
	RenderWindow * renderer;
	QMenu * fileMenu;
	QMenu * viewMenu;
	QAction * addAction(const char * title, int shortCut= 0);
	void addMenu(const char * title,QAction * action);
	void addMenus();
	//QLabel * featuresLabel;
	//QCheckBox * enableShadows;
	//QCheckBox * enableReflections;
	//QCheckBox * enableRefractions;
	//QComboBox * lightSelection;
	QComboBox * optimizationSelection;
	//QComboBox * sampleType;
	//FloatSlider * pointLightRadius;
	//FloatSlider * specularPower;
	//Float3Slider * direction;
	//Float3Slider * position;
	//Float3Slider * ambient;
	//Float3Slider * diffuse;
	//Float3Slider * specular;
	//Float3Slider * cameraLookAt;
	//FloatSlider * cameraDistance;
//	FloatSlider * cameraFocalDistance;
	//FloatSlider * cameraRadius;
	//QComboBox * cameraChoice;

	IntSlider * sampling;
	//QCheckBox * softShadows;
	//IntSlider * numberOfRefractions;
	//IntSlider * numberOfReflections;
	IntSlider * numberOfBounces;
	bool rayInterfaceEnabled;


	float radius;
	QTimer updateTimer;
	QQuickView * rightView;
	QQuickView * leftView;
	int gameObjectIndex;
	void keyPressEvent(QKeyEvent * e)override;
	void keyReleaseEvent(QKeyEvent * e)override;

private slots:
	//void changeLightType(int);
	//void changeCameraType(int);
	void changeBounceValue(int);
	void changeName();
	void gameObjectSelectionChanged(int, int);
	void changeMeshColor(qreal x, qreal y, qreal z);
	void changeRefractionIndex(qreal value);
	void changeMaterialType(int type);
	void changeLightPosition(qreal x, qreal y , qreal z);
	void changeCameraPosition(qreal x , qreal y, qreal z);
	void changeCameraLookAt(qreal x , qreal y, qreal z);
	void changeCameraDistance(qreal x);
	void changeSamples(int value);
	void changeBounces(int value);
	void updateGameObjectList();

private:
	void addObject();
	void addSphere();
	void updateWindow();
	void setRayInterfaceEnabled(bool enabled);
	//void renderWindow();
};

