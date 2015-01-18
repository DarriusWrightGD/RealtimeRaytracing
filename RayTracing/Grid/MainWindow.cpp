#include "MainWindow.h"
#include <QtQuick\qquickview.h>
#include <QtQml\qqml.h>
#include <QtQml\qqmlengine.h>
#include <QtQml\qqmlfile.h>
#include <QtQml\qjsengine.h>
#include <QtQml\qqmlscriptstring.h>
#include <QtQml\qqmlcontext.h>
#include "GameObjectContainer.h"
#include <QtQuick\qquickitem.h>
#include <QtQml\qqml.h>


MainWindow::MainWindow(RenderWindow * render) : renderer(render),rightObject(NULL),
	leftComponent(&engine, QUrl::fromLocalFile("left.qml")),
	rightComponent(&engine, QUrl::fromLocalFile("right.qml")), gameObjectIndex(0) , rayInterfaceEnabled(true)
{

	connect(renderer,SIGNAL(updateList()), this, SLOT(updateGameObjectList()));

	qmlRegisterUncreatableType<GameObject>("GameObject", 1,0, "GameObject","Instantiated C++ side !");
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	setEnabled(true);
	//engine.load(QUrl(QStringLiteral("left.qml")));


//	QVariantList listElements;
//	listElements.append(QVariant::fromValue(new GameObject("Cube",Type::Mesh_Type,0)));
//	engine.rootContext()->setContextProperty("gameObject",QVariant::fromValue(listElements));



	initializeViews();
	connectSignals();

	mainWidget = new QWidget();
	QHBoxLayout * mainLayout = new QHBoxLayout();
	
	mainLayout->addWidget(leftContainer);

	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);
	mainLayout->addWidget(renderer);
	mainLayout->addWidget(rightContainer);
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);
	
	addMenus();
	connect(&updateTimer,&QTimer::timeout,this, &MainWindow::updateWindow);
	
	updateTimer.start();

	
}


void MainWindow::updateGameObjectList()
{
	QVariantList listElements;

	for(GameObject * g  : renderer->gameObjectList)
	{
		listElements.append(QVariant::fromValue(g));
	}

	//listElements.append(QVariant::fromValue(new GameObject("Cubelkfdsjaslkfdjsakf",Type::Mesh_Type,0)));
	//engine.rootContext()->setContextProperty("gContainer",QVariant::fromValue(renderer->gameObjectContainer));
	leftView->rootContext()->setContextProperty("gameObject",QVariant::fromValue(listElements));
}

MainWindow::~MainWindow(void)
{
}


void MainWindow::initializeViews()
{
	rightView = new QQuickView();
	rightContainer = QWidget::createWindowContainer(rightView, this);
	rightView->setSource(QUrl("right.qml"));
	rightContainer->setMinimumSize(500, 200);
	rightContainer->setMaximumSize(500, 700);
	rightContainer->setFocusPolicy(Qt::TabFocus);
	rightQml =rightView->rootObject();
	QMetaObject::invokeMethod(rightQml, "setLightPosition",Q_ARG(QVariant,renderer->lights[gameObjectIndex].position.s[0]),Q_ARG(QVariant,renderer->lights[gameObjectIndex].position.s[1]),Q_ARG(QVariant,renderer->lights[gameObjectIndex].position.s[2]));


	leftView = new QQuickView();
	leftContainer = QWidget::createWindowContainer(leftView, this);
	leftView->setSource(QUrl("left.qml"));

	leftContainer->setMinimumSize(300, 200);
	leftContainer->setMaximumSize(300, 700);
	leftContainer->setFocusPolicy(Qt::TabFocus);

	rightQml = rightView->rootObject();
	leftQml = leftView->rootObject();

	qmlRegisterUncreatableType<GameObject>("GameObject", 1,0, "GameObject","Instanciated C++ side !");
	//for()

	QVariantList listElements;

	for(GameObject * g  : renderer->gameObjectList)
	{
		listElements.append(QVariant::fromValue(g));
	}

	//listElements.append(QVariant::fromValue(new GameObject("Cubelkfdsjaslkfdjsakf",Type::Mesh_Type,0)));
	//engine.rootContext()->setContextProperty("gContainer",QVariant::fromValue(renderer->gameObjectContainer));
	leftView->rootContext()->setContextProperty("gameObject",QVariant::fromValue(listElements));



}


void MainWindow::changeMeshColor(qreal x, qreal y , qreal z)
{
	renderer->meshes[gameObjectIndex].material.color.s[0] = x;
	renderer->meshes[gameObjectIndex].material.color.s[1] = y;
	renderer->meshes[gameObjectIndex].material.color.s[2] = z;


	qDebug() << "Color Changing";
}


void MainWindow::keyPressEvent(QKeyEvent * e)
{
	//cout << "Pressed Event" << endl;
	renderer->keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent * e)
{
	//cout << "Pressed Event" << endl;
	renderer->keyReleaseEvent(e);
}

void MainWindow::changeRefractionIndex(qreal value)
{
	renderer->meshes[gameObjectIndex].material.refraction = value;
}

void MainWindow::gameObjectSelectionChanged(int index, int type)
{
	Type gameObjectType = (Type) type;
	gameObjectIndex = index;
	rightView->rootContext()->setContextProperty("gameObject",QVariant::fromValue(renderer->gameObjectList[index]));
	//QMetaObject::invokeMethod(object, "myQmlFunction",
	//   Q_RETURN_ARG(QVariant, returnedValue),
	//   Q_ARG(QVariant, msg));

	switch (type)
	{
	case Type::Camera_Type:
		QMetaObject::invokeMethod(rightQml, "setCameraPosition",Q_ARG(QVariant,renderer->camera.position.x),Q_ARG(QVariant,renderer->camera.position.y),Q_ARG(QVariant,renderer->camera.position.z));
		QMetaObject::invokeMethod(rightQml, "setCameraLookAt",Q_ARG(QVariant,renderer->camera.lookAt.x),Q_ARG(QVariant,renderer->camera.lookAt.y),Q_ARG(QVariant,renderer->camera.lookAt.z));
		QMetaObject::invokeMethod(rightQml, "setCameraDistance",Q_ARG(QVariant,renderer->camera.distance));
		break;
	case Type::Light_Type:
		QMetaObject::invokeMethod(rightQml, "setLightPosition",Q_ARG(QVariant,renderer->lights[gameObjectIndex].position.s[0]),Q_ARG(QVariant,renderer->lights[gameObjectIndex].position.s[1]),Q_ARG(QVariant,renderer->lights[gameObjectIndex].position.s[2]));
		break;
	case Type::Mesh_Type:

		QMetaObject::invokeMethod(rightQml, "setMaterialType",Q_ARG(QVariant,(int)renderer->meshes[gameObjectIndex].material.type));
		QMetaObject::invokeMethod(rightQml, "setMeshColor",Q_ARG(QVariant,renderer->meshes[gameObjectIndex].material.color.s[0]),
			Q_ARG(QVariant,renderer->meshes[gameObjectIndex].material.color.s[1]),Q_ARG(QVariant,renderer->meshes[gameObjectIndex].material.color.s[2]));
		QMetaObject::invokeMethod(rightQml, "setRefractionIndex",Q_ARG(QVariant,renderer->meshes[gameObjectIndex].material.refraction));

		break;
	}
}

void MainWindow::changeLightPosition(qreal x, qreal y , qreal z)
{
	renderer->lights[gameObjectIndex].position.s[0] = x;
	renderer->lights[gameObjectIndex].position.s[1] = y;
	renderer->lights[gameObjectIndex].position.s[2] = z;
	//	QMetaObject::invokeMethod(rightQml, "setLightPosition",Q_ARG(QVariant,renderer->lights[gameObjectIndex].position.s[0]),Q_ARG(QVariant,renderer->lights[gameObjectIndex].position.s[1]),Q_ARG(QVariant,renderer->lights[gameObjectIndex].position.s[2]));

}


void MainWindow::changeMaterialType(int type)
{
	renderer->meshes[gameObjectIndex].material.type = (MaterialType)type;
}

void MainWindow::connectSignals()
{
	//connect(leftQml, SIGNAL(changeModelName()), this,SLOT(changeName()));
	connectQMLSignals();
}

void MainWindow::connectQMLSignals()
{
	connect(leftQml, SIGNAL(modelChanged(int, int)), this , SLOT(gameObjectSelectionChanged(int,int)));
	connect(leftQml, SIGNAL(modelChanged(int,int)), rightQml,SIGNAL(updateQML(int,int)));
	connect(leftQml, SIGNAL(numberOfBouncesChanged(int)), this, SLOT(changeBounces(int)));
	connect(leftQml, SIGNAL(numberOfSamplesChanged(int)), this, SLOT(changeSamples(int)));
	connect(rightQml, SIGNAL(indexOfRefractionChanged(qreal)), this, SLOT(changeRefractionIndex(qreal)));
	connect(rightQml, SIGNAL(colorChanged(qreal,qreal,qreal)), this, SLOT(changeMeshColor(qreal,qreal,qreal)));
	connect(rightQml, SIGNAL(materialChanged(int)), this , SLOT(changeMaterialType(int)));
	connect(rightQml, SIGNAL(lightPositionChanged(qreal,qreal,qreal)), this , SLOT(changeLightPosition(qreal,qreal,qreal)));
	connect(rightQml, SIGNAL(cameraLookAtChanged(qreal,qreal,qreal)), this , SLOT(changeCameraLookAt(qreal,qreal,qreal)));
	connect(rightQml, SIGNAL(cameraPositionChanged(qreal,qreal,qreal)), this , SLOT(changeCameraPosition(qreal,qreal,qreal)));
	connect(rightQml, SIGNAL(cameraDistanceChanged(qreal)), this , SLOT(changeCameraDistance(qreal)));

}


void MainWindow::changeSamples(int value)
{
	renderer->setSamples(value);
}

void MainWindow::changeBounces(int value)
{
	renderer->setMaxBounce(value);
}
void MainWindow::changeCameraPosition(qreal x , qreal y, qreal z)
{
	renderer->camera.position = glm::vec4(x,y,z,renderer->camera.position.w);
}
void MainWindow::changeCameraLookAt(qreal x , qreal y, qreal z)
{
	renderer->camera.lookAt = glm::vec4(x,y,z,renderer->camera.lookAt.w);

}
void MainWindow::changeCameraDistance(qreal value)
{
	renderer->camera.distance  = value;
}


void MainWindow::changeBounceValue(int value)
{
	renderer->setMaxBounce(value);
}

void MainWindow::changeName()
{
	qDebug() << "Changed Name" ;
}
//
//void MainWindow::changeLightType(int index)
//{
//	LightType type = (LightType)index;
//	lightSelection->setCurrentIndex(index);
//
//	position->setVisible(false);
//	direction->setVisible(false);
//	pointLightRadius->setVisible(false);
//
//
//	switch (type)
//	{
//	case LightType::DIRECTIONAL_TYPE:
//		direction->setVisible(true);
//
//		break;
//	case LightType::POINT_TYPE:
//		position->setVisible(true);
//		pointLightRadius->setVisible(true);
//		break;
//	}
//
//
//	renderer->changeLightType(index);
//
//}
//void MainWindow::changeCameraType(int index)
//{
//	CameraType type = (CameraType)index;
//
//	//cameraDistance->setVisible(false);
//	//cameraLookAt->setVisible(false);
//
//	switch (type)
//	{
//	case CameraType::Pinhole:
//		break;
//	case CameraType::FishEye:
//		break;
//	case CameraType::Spherical:
//		break;
//	case CameraType::Stero:
//		break;
//	case CameraType::Thinlens:
//		break;
//	}
//	renderer->changeCameraType(index);
//}

QAction * MainWindow::addAction(const char * title, int shortCut)
{
	QAction * action = new QAction(tr(title), this);
	if(shortCut != 0 )
		action->setShortcut(shortCut);
	return action;
}
void MainWindow::addMenu(const char * title,QAction * action)
{

}

void MainWindow::setRayInterfaceEnabled(bool enabled)
{
	rayInterfaceEnabled = enabled;
}

void MainWindow::addMenus()
{



	fileMenu = menuBar()->addMenu(tr("&File"));
	viewMenu = menuBar()->addMenu(tr("&View"));
	QAction * addObject = addAction("&Add Object");
	connect(addObject,&QAction::triggered,this,&MainWindow::addObject);
	fileMenu->addAction(addObject);

	//QAction * addSphere = addAction("&Add Sphere");
	//connect(addSphere,&QAction::triggered,this,&MainWindow::addSphere);
	//fileMenu->addAction(addSphere);

	//QAction * enabledGlobalIllumination = addAction("&Global Illumination");
	QAction * enabledShadows = addAction("&Shadows");
	QAction * enabledRayInterface = addAction("&Ray Menu");
	//QAction * enabledReflections = addAction("&Reflections");
	//QAction * enabledRefractions = addAction("&Refractions");

	enabledShadows->setCheckable(true);
	enabledRayInterface->setCheckable(true);
	//enabledReflections->setCheckable(true);
	//enabledRefractions->setCheckable(true);
	//enabledGlobalIllumination->setCheckable(true);
	enabledShadows->setChecked(renderer->isShadowsEnabled());
	enabledRayInterface->setCheckable(true);
	//enabledReflections->setChecked(renderer->isReflectionsEnabled());
	//enabledRefractions->setChecked(renderer->isRefractionsEnabled());
	//enabledGlobalIllumination->setChecked(renderer->globalIlluminationEnabled());

	//connect(enabledGlobalIllumination,&QAction::triggered,renderer,&RenderWindow::setGlobalIllumination);
	//viewMenu->addAction(enabledGlobalIllumination);


	connect(enabledShadows,&QAction::triggered,renderer,&RenderWindow::setShadowsEnabled);
	viewMenu->addAction(enabledShadows);
	connect(enabledRayInterface, &QAction::triggered, this, &MainWindow::setRayInterfaceEnabled);

	//connect(enabledReflections,&QAction::toggled,renderer,&RenderWindow::setReflectionsEnabled);
	//viewMenu->addAction(enabledReflections);

	//connect(enabledRefractions,&QAction::toggled,renderer,&RenderWindow::setRefractionsEnabled);
	//viewMenu->addAction(enabledRefractions);





}

void MainWindow::addSphere()
{
	renderer->addSphere();
}

void MainWindow::addObject()
{
	std::cout << "adding object" << std::endl;

	QString fileName = QFileDialog::getOpenFileName(this,tr("OpenFile"),"","*.obj");
	if(!fileName.isEmpty())
	{
		std::string stringFile (fileName.toLocal8Bit().constData());
		renderer->addObject(stringFile);
	}

}

void MainWindow::updateWindow()
{

	renderer->updateScene();
	//pointLightRadius->update();
	//direction->update();
	//position->update();
	//ambient->update();
	//diffuse->update();
	//specular->update();
	//specularPower->update();
	//cameraLookAt->update();
	//cameraDistance->update();
	///cameraRadius->update();
	//cameraFocalDistance->update();

	//sampling->update();
	//numberOfReflections->update();
	//numberOfRefractions->update();
	setWindowTitle( "Time : " + QString::number(renderer->getInterval()) + "\tFrames : "+ QString::number(renderer->getFPS()));

}