/****************************************************************************
** Meta object code from reading C++ file 'specwidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../planarrad/gui/specwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'specwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpecWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      37,   11,   11,   11, 0x0a,
      65,   11,   11,   11, 0x0a,
      87,   11,   11,   11, 0x0a,
     111,   11,   11,   11, 0x0a,
     132,  129,   11,   11, 0x0a,
     162,  129,   11,   11, 0x0a,
     191,  189,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SpecWidget[] = {
    "SpecWidget\0\0slotLogCheckBoxChanged()\0"
    "slotUpdateWavelengthRange()\0"
    "slotDataSetsChanged()\0slotSetValueAxesRange()\0"
    "slotAddSpecData()\0sd\0slotRemoveSpecData(SpecData*)\0"
    "slotAxisChanged(SpecData*)\0d\0"
    "slotCurrentDepthChanged(double)\0"
};

void SpecWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpecWidget *_t = static_cast<SpecWidget *>(_o);
        switch (_id) {
        case 0: _t->slotLogCheckBoxChanged(); break;
        case 1: _t->slotUpdateWavelengthRange(); break;
        case 2: _t->slotDataSetsChanged(); break;
        case 3: _t->slotSetValueAxesRange(); break;
        case 4: _t->slotAddSpecData(); break;
        case 5: _t->slotRemoveSpecData((*reinterpret_cast< SpecData*(*)>(_a[1]))); break;
        case 6: _t->slotAxisChanged((*reinterpret_cast< SpecData*(*)>(_a[1]))); break;
        case 7: _t->slotCurrentDepthChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SpecWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpecWidget::staticMetaObject = {
    { &BasePanel::staticMetaObject, qt_meta_stringdata_SpecWidget,
      qt_meta_data_SpecWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpecWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpecWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpecWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpecWidget))
        return static_cast<void*>(const_cast< SpecWidget*>(this));
    return BasePanel::qt_metacast(_clname);
}

int SpecWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BasePanel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
