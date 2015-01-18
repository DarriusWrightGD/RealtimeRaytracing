/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[20];
    char stringdata[272];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 17),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 10),
QT_MOC_LITERAL(4, 41, 26),
QT_MOC_LITERAL(5, 68, 15),
QT_MOC_LITERAL(6, 84, 1),
QT_MOC_LITERAL(7, 86, 1),
QT_MOC_LITERAL(8, 88, 1),
QT_MOC_LITERAL(9, 90, 21),
QT_MOC_LITERAL(10, 112, 5),
QT_MOC_LITERAL(11, 118, 18),
QT_MOC_LITERAL(12, 137, 4),
QT_MOC_LITERAL(13, 142, 19),
QT_MOC_LITERAL(14, 162, 20),
QT_MOC_LITERAL(15, 183, 18),
QT_MOC_LITERAL(16, 202, 20),
QT_MOC_LITERAL(17, 223, 13),
QT_MOC_LITERAL(18, 237, 13),
QT_MOC_LITERAL(19, 251, 20)
    },
    "MainWindow\0changeBounceValue\0\0changeName\0"
    "gameObjectSelectionChanged\0changeMeshColor\0"
    "x\0y\0z\0changeRefractionIndex\0value\0"
    "changeMaterialType\0type\0changeLightPosition\0"
    "changeCameraPosition\0changeCameraLookAt\0"
    "changeCameraDistance\0changeSamples\0"
    "changeBounces\0updateGameObjectList"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x08 /* Private */,
       3,    0,   82,    2, 0x08 /* Private */,
       4,    2,   83,    2, 0x08 /* Private */,
       5,    3,   88,    2, 0x08 /* Private */,
       9,    1,   95,    2, 0x08 /* Private */,
      11,    1,   98,    2, 0x08 /* Private */,
      13,    3,  101,    2, 0x08 /* Private */,
      14,    3,  108,    2, 0x08 /* Private */,
      15,    3,  115,    2, 0x08 /* Private */,
      16,    1,  122,    2, 0x08 /* Private */,
      17,    1,  125,    2, 0x08 /* Private */,
      18,    1,  128,    2, 0x08 /* Private */,
      19,    0,  131,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal, QMetaType::QReal,    6,    7,    8,
    QMetaType::Void, QMetaType::QReal,   10,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal, QMetaType::QReal,    6,    7,    8,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal, QMetaType::QReal,    6,    7,    8,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal, QMetaType::QReal,    6,    7,    8,
    QMetaType::Void, QMetaType::QReal,    6,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->changeBounceValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->changeName(); break;
        case 2: _t->gameObjectSelectionChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->changeMeshColor((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 4: _t->changeRefractionIndex((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 5: _t->changeMaterialType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->changeLightPosition((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 7: _t->changeCameraPosition((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 8: _t->changeCameraLookAt((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        case 9: _t->changeCameraDistance((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 10: _t->changeSamples((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->changeBounces((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->updateGameObjectList(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
