/****************************************************************************
** Meta object code from reading C++ file 'interfaceToMyGui.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interfaceToMyGui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'interfaceToMyGui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_InterfaceToMyGUI[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,
      37,   17,   17,   17, 0x08,
      56,   17,   17,   17, 0x08,
      72,   17,   17,   17, 0x08,
      94,   88,   17,   17, 0x08,
     109,   88,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_InterfaceToMyGUI[] = {
    "InterfaceToMyGUI\0\0pedastrian1Event()\0"
    "pedastrian2Event()\0sendTextEvent()\0"
    "lineEditEvent()\0value\0dialEvent(int)\0"
    "horizontalSliderEvent(int)\0"
};

void InterfaceToMyGUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        InterfaceToMyGUI *_t = static_cast<InterfaceToMyGUI *>(_o);
        switch (_id) {
        case 0: _t->pedastrian1Event(); break;
        case 1: _t->pedastrian2Event(); break;
        case 2: _t->sendTextEvent(); break;
        case 3: _t->lineEditEvent(); break;
        case 4: _t->dialEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->horizontalSliderEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData InterfaceToMyGUI::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject InterfaceToMyGUI::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_InterfaceToMyGUI,
      qt_meta_data_InterfaceToMyGUI, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &InterfaceToMyGUI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *InterfaceToMyGUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *InterfaceToMyGUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_InterfaceToMyGUI))
        return static_cast<void*>(const_cast< InterfaceToMyGUI*>(this));
    if (!strcmp(_clname, "Ui::MainWindow"))
        return static_cast< Ui::MainWindow*>(const_cast< InterfaceToMyGUI*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int InterfaceToMyGUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
