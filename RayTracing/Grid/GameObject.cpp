#include "GameObject.h"

GameObject::GameObject()
{

}

GameObject::GameObject(QString name, Type type, int index): m_name(name), m_type((int)type), m_index(index)
{

}

GameObject::~GameObject(void)
{
}

GameObject::GameObject(const GameObject & gameObject)
{

}


QString GameObject::name()const 
{
	return m_name;

}

void GameObject::setName(const QString & n)
{
	if(n != m_name)
	{
		m_name  = n ;
		emit nameChanged();
	}
}

qint32 GameObject::index()const
{
	return m_index;
}

int GameObject::type()const
{
	return (int)m_type;

}

void GameObject::setIndex(const int & index)
{
	m_index = index;
}
void GameObject::setType(const Type & t)
{
	m_type = t;
}