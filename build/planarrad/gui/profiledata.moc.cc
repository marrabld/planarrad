/****************************************************************************
** Meta object code from reading C++ file 'profiledata.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../planarrad/gui/profiledata.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'profiledata.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProfileData[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      39,   12,   12,   12, 0x05,
      80,   12,   12,   12, 0x05,
     112,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     138,   12,   12,   12, 0x0a,
     163,   12,   12,   12, 0x0a,
     181,   12,   12,   12, 0x0a,
     200,   12,   12,   12, 0x0a,
     222,  220,   12,   12, 0x0a,
     249,   12,   12,   12, 0x0a,
     275,   12,   12,   12, 0x0a,
     300,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ProfileData[] = {
    "ProfileData\0\0signalSetValueAxesRange()\0"
    "signalNewCurrentWavelength(ProfileData*)\0"
    "signalAxisChanged(ProfileData*)\0"
    "signalClose(ProfileData*)\0"
    "slotDataSetListChanged()\0slotColourCombo()\0"
    "slotDataSetCombo()\0slotDataTypeCombo()\0"
    "v\0slotWavelnSliderMoved(int)\0"
    "slotWavelnReturnPressed()\0"
    "slotAxisClicked(Button*)\0slotCloseClicked()\0"
};

void ProfileData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ProfileData *_t = static_cast<ProfileData *>(_o);
        switch (_id) {
        case 0: _t->signalSetValueAxesRange(); break;
        case 1: _t->signalNewCurrentWavelength((*reinterpret_cast< ProfileData*(*)>(_a[1]))); break;
        case 2: _t->signalAxisChanged((*reinterpret_cast< ProfileData*(*)>(_a[1]))); break;
        case 3: _t->signalClose((*reinterpret_cast< ProfileData*(*)>(_a[1]))); break;
        case 4: _t->slotDataSetListChanged(); break;
        case 5: _t->slotColourCombo(); break;
        case 6: _t->slotDataSetCombo(); break;
        case 7: _t->slotDataTypeCombo(); break;
        case 8: _t->slotWavelnSliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->slotWavelnReturnPressed(); break;
        case 10: _t->slotAxisClicked((*reinterpret_cast< Button*(*)>(_a[1]))); break;
        case 11: _t->slotCloseClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ProfileData::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ProfileData::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ProfileData,
      qt_meta_data_ProfileData, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ProfileData::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ProfileData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ProfileData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProfileData))
        return static_cast<void*>(const_cast< ProfileData*>(this));
    return QWidget::qt_metacast(_clname);
}

int ProfileData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void ProfileData::signalSetValueAxesRange()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ProfileData::signalNewCurrentWavelength(ProfileData * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProfileData::signalAxisChanged(ProfileData * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ProfileData::signalClose(ProfileData * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
