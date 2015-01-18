#pragma once

#include <QtCore\qvariant.h>
#include "GameObject.h"
#include <QtQml\qqmllist.h>


class GameObjectContainer : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QQmlListProperty<GameObject> m_list READ getList)
public:
	GameObjectContainer(const GameObjectContainer &);
	GameObjectContainer(void);
	~GameObjectContainer(void);
	QQmlListProperty<GameObject> getList();
	void addObject(GameObject * object);
	Q_INVOKABLE GameObject * getData(int i );

private :
	QString name;
	QList<GameObject*> m_list;
	void clear();

};

Q_DECLARE_METATYPE(GameObjectContainer);
