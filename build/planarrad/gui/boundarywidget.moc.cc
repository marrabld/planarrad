/****************************************************************************
** Meta object code from reading C++ file 'boundarywidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../planarrad/gui/boundarywidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'boundarywidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BoundaryWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_BoundaryWidget[] = {
    "BoundaryWidget\0\0slotUpdateEnabled()\0"
};

void BoundaryWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BoundaryWidget *_t = static_cast<BoundaryWidget *>(_o);
        switch (_id) {
        case 0: _t->slotUpdateEnabled(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData BoundaryWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BoundaryWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BoundaryWidget,
      qt_meta_data_BoundaryWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BoundaryWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BoundaryWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BoundaryWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BoundaryWidget))
        return static_cast<void*>(const_cast< BoundaryWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int BoundaryWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
