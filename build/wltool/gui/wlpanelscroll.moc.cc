/****************************************************************************
** Meta object code from reading C++ file 'wlpanelscroll.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../wltool/gui/wlpanelscroll.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wlpanelscroll.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WLPanelScroll[] = {

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
      24,   15,   14,   14, 0x0a,
      65,   61,   14,   14, 0x0a,
     104,  102,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WLPanelScroll[] = {
    "WLPanelScroll\0\0rec_list\0"
    "slotNewRecordList(const RecordList*)\0"
    "rec\0slotNewRecordSelected(const Record*)\0"
    "f\0slotSetShowKey(bool)\0"
};

void WLPanelScroll::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WLPanelScroll *_t = static_cast<WLPanelScroll *>(_o);
        switch (_id) {
        case 0: _t->slotNewRecordList((*reinterpret_cast< const RecordList*(*)>(_a[1]))); break;
        case 1: _t->slotNewRecordSelected((*reinterpret_cast< const Record*(*)>(_a[1]))); break;
        case 2: _t->slotSetShowKey((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData WLPanelScroll::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject WLPanelScroll::staticMetaObject = {
    { &PanelScroll::staticMetaObject, qt_meta_stringdata_WLPanelScroll,
      qt_meta_data_WLPanelScroll, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WLPanelScroll::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WLPanelScroll::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WLPanelScroll::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WLPanelScroll))
        return static_cast<void*>(const_cast< WLPanelScroll*>(this));
    return PanelScroll::qt_metacast(_clname);
}

int WLPanelScroll::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PanelScroll::qt_metacall(_c, _id, _a);
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
