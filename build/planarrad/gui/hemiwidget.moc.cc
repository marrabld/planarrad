/****************************************************************************
** Meta object code from reading C++ file 'hemiwidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../planarrad/gui/hemiwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hemiwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_HemiWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      34,   11,   11,   11, 0x0a,
      53,   11,   11,   11, 0x0a,
      75,   73,   11,   11, 0x0a,
     109,  107,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_HemiWidget[] = {
    "HemiWidget\0\0slotDataSetsChanged()\0"
    "slotDataSetCombo()\0slotDataTypeCombo()\0"
    "d\0slotCurrentDepthChanged(double)\0v\0"
    "slotSensSliderMoved(int)\0"
};

void HemiWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HemiWidget *_t = static_cast<HemiWidget *>(_o);
        switch (_id) {
        case 0: _t->slotDataSetsChanged(); break;
        case 1: _t->slotDataSetCombo(); break;
        case 2: _t->slotDataTypeCombo(); break;
        case 3: _t->slotCurrentDepthChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->slotSensSliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData HemiWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject HemiWidget::staticMetaObject = {
    { &BasePanel::staticMetaObject, qt_meta_stringdata_HemiWidget,
      qt_meta_data_HemiWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &HemiWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *HemiWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *HemiWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HemiWidget))
        return static_cast<void*>(const_cast< HemiWidget*>(this));
    return BasePanel::qt_metacast(_clname);
}

int HemiWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BasePanel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
