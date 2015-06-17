/****************************************************************************
** Meta object code from reading C++ file 'specdata.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../planarrad/gui/specdata.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'specdata.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpecData[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      36,    9,    9,    9, 0x05,
      74,    9,    9,    9, 0x05,
     103,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     126,    9,    9,    9, 0x0a,
     151,    9,    9,    9, 0x0a,
     169,    9,    9,    9, 0x0a,
     188,    9,    9,    9, 0x0a,
     208,    9,    9,    9, 0x0a,
     233,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SpecData[] = {
    "SpecData\0\0signalSetValueAxesRange()\0"
    "signalNewCurrentWavelength(SpecData*)\0"
    "signalAxisChanged(SpecData*)\0"
    "signalClose(SpecData*)\0slotDataSetListChanged()\0"
    "slotColourCombo()\0slotDataSetCombo()\0"
    "slotDataTypeCombo()\0slotAxisClicked(Button*)\0"
    "slotCloseClicked()\0"
};

void SpecData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpecData *_t = static_cast<SpecData *>(_o);
        switch (_id) {
        case 0: _t->signalSetValueAxesRange(); break;
        case 1: _t->signalNewCurrentWavelength((*reinterpret_cast< SpecData*(*)>(_a[1]))); break;
        case 2: _t->signalAxisChanged((*reinterpret_cast< SpecData*(*)>(_a[1]))); break;
        case 3: _t->signalClose((*reinterpret_cast< SpecData*(*)>(_a[1]))); break;
        case 4: _t->slotDataSetListChanged(); break;
        case 5: _t->slotColourCombo(); break;
        case 6: _t->slotDataSetCombo(); break;
        case 7: _t->slotDataTypeCombo(); break;
        case 8: _t->slotAxisClicked((*reinterpret_cast< Button*(*)>(_a[1]))); break;
        case 9: _t->slotCloseClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SpecData::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpecData::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SpecData,
      qt_meta_data_SpecData, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpecData::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpecData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpecData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpecData))
        return static_cast<void*>(const_cast< SpecData*>(this));
    return QWidget::qt_metacast(_clname);
}

int SpecData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void SpecData::signalSetValueAxesRange()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SpecData::signalNewCurrentWavelength(SpecData * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SpecData::signalAxisChanged(SpecData * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SpecData::signalClose(SpecData * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
