/****************************************************************************
** Meta object code from reading C++ file 'wlchart.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../wltool/gui/wlchart.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wlchart.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WLChart[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,    9,    8,    8, 0x0a,
      59,   55,    8,    8, 0x0a,
      98,   96,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WLChart[] = {
    "WLChart\0\0rec_list\0"
    "slotNewRecordList(const RecordList*)\0"
    "rec\0slotNewRecordSelected(const Record*)\0"
    "f\0setShowKey(bool)\0"
};

void WLChart::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WLChart *_t = static_cast<WLChart *>(_o);
        switch (_id) {
        case 0: _t->slotNewRecordList((*reinterpret_cast< const RecordList*(*)>(_a[1]))); break;
        case 1: _t->slotNewRecordSelected((*reinterpret_cast< const Record*(*)>(_a[1]))); break;
        case 2: _t->setShowKey((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData WLChart::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject WLChart::staticMetaObject = {
    { &BufferWidget::staticMetaObject, qt_meta_stringdata_WLChart,
      qt_meta_data_WLChart, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WLChart::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WLChart::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WLChart::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WLChart))
        return static_cast<void*>(const_cast< WLChart*>(this));
    return BufferWidget::qt_metacast(_clname);
}

int WLChart::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BufferWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
