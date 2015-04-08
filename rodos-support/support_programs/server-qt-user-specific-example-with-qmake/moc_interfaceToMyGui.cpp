/****************************************************************************
** Meta object code from reading C++ file 'interfaceToMyGui.h'
**
** Created: Sat Nov 3 13:14:38 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../server-qt-user-specific-example/interfaceToMyGui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'interfaceToMyGui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_InterfaceToMyGUI[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,
      37,   17,   17,   17, 0x08,
      56,   17,   17,   17, 0x08,
      78,   72,   17,   17, 0x08,
      93,   72,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_InterfaceToMyGUI[] = {
    "InterfaceToMyGUI\0\0pedastrian1Event()\0"
    "pedastrian2Event()\0lineEditEvent()\0"
    "value\0dialEvent(int)\0horizontalSliderEvent(int)\0"
};

const QMetaObject InterfaceToMyGUI::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_InterfaceToMyGUI,
      qt_meta_data_InterfaceToMyGUI, 0 }
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
        switch (_id) {
        case 0: pedastrian1Event(); break;
        case 1: pedastrian2Event(); break;
        case 2: lineEditEvent(); break;
        case 3: dialEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: horizontalSliderEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
