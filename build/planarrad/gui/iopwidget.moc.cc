/****************************************************************************
** Meta object code from reading C++ file 'iopwidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../planarrad/gui/iopwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'iopwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_IOPWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   11,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      49,   11,   10,   10, 0x0a,
      78,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_IOPWidget[] = {
    "IOPWidget\0\0ret_ok\0signalProcessingFinished(bool)\0"
    "slotProcessingFinished(bool)\0"
    "slotUpdateEnabled()\0"
};

void IOPWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        IOPWidget *_t = static_cast<IOPWidget *>(_o);
        switch (_id) {
        case 0: _t->signalProcessingFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->slotProcessingFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->slotUpdateEnabled(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData IOPWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject IOPWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_IOPWidget,
      qt_meta_data_IOPWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &IOPWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *IOPWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *IOPWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_IOPWidget))
        return static_cast<void*>(const_cast< IOPWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int IOPWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void IOPWidget::signalProcessingFinished(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
