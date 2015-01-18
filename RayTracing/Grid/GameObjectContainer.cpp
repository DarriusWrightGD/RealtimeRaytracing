#include "GameObjectContainer.h"

GameObjectContainer::GameObjectContainer(void)
{
}


GameObjectContainer::~GameObjectContainer(void)
{
}

GameObjectContainer::GameObjectContainer(const GameObjectContainer & g)
{

}


QQmlListProperty<GameObject> GameObjectContainer::getList()
{
	return QQmlListProperty<GameObject>(this,m_list);
}

void GameObjectContainer::addObject(GameObject * gameObject)
{
	m_list.append(gameObject);
}

GameObject * GameObjectContainer::getData(int i )
{
	return m_list.at(i);
}