#pragma once

#include <QtCore\qobject.h>

	enum Type
	{
		Mesh_Type = 0,
		Light_Type,
		Camera_Type
	};

class GameObject : public QObject
{
	Q_OBJECT
	Q_ENUMS(Type)
	Q_PROPERTY(int type READ type)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(qint32 index READ index)

public:
	GameObject();
	GameObject(const GameObject & );
	GameObject(QString name, Type type, int index);
	~GameObject(void);



	Q_INVOKABLE int type()const;
	Q_INVOKABLE QString name()const;
	Q_INVOKABLE void setName(const QString & n);
	Q_INVOKABLE void setIndex(const int & index);
	Q_INVOKABLE void setType(const Type & t);
	Q_INVOKABLE int index()const;

signals:
	void nameChanged();
private :
	QString m_name;
	qint32 m_index;
	int m_type;

};


Q_DECLARE_METATYPE(GameObject);
