/****************************************************************************
** Meta object code from reading C++ file 'profilewidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../planarrad/gui/profilewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'profilewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProfileWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      53,   51,   14,   14, 0x0a,
      79,   14,   14,   14, 0x0a,
     104,   14,   14,   14, 0x0a,
     129,   14,   14,   14, 0x0a,
     151,   14,   14,   14, 0x0a,
     172,   14,   14,   14, 0x0a,
     196,   14,   14,   14, 0x0a,
     220,  217,   14,   14, 0x0a,
     256,  217,   14,   14, 0x0a,
     293,  286,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ProfileWidget[] = {
    "ProfileWidget\0\0d\0signalCurrentDepthChanged(double)\0"
    "v\0slotDepthSliderMoved(int)\0"
    "slotDepthReturnPressed()\0"
    "slotLogCheckBoxChanged()\0slotDataSetsChanged()\0"
    "slotUpdateMaxDepth()\0slotSetValueAxesRange()\0"
    "slotAddProfileData()\0pd\0"
    "slotRemoveProfileData(ProfileData*)\0"
    "slotAxisChanged(ProfileData*)\0src_pd\0"
    "slotNewCurrentWavelength(ProfileData*)\0"
};

void ProfileWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ProfileWidget *_t = static_cast<ProfileWidget *>(_o);
        switch (_id) {
        case 0: _t->signalCurrentDepthChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->slotDepthSliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->slotDepthReturnPressed(); break;
        case 3: _t->slotLogCheckBoxChanged(); break;
        case 4: _t->slotDataSetsChanged(); break;
        case 5: _t->slotUpdateMaxDepth(); break;
        case 6: _t->slotSetValueAxesRange(); break;
        case 7: _t->slotAddProfileData(); break;
        case 8: _t->slotRemoveProfileData((*reinterpret_cast< ProfileData*(*)>(_a[1]))); break;
        case 9: _t->slotAxisChanged((*reinterpret_cast< ProfileData*(*)>(_a[1]))); break;
        case 10: _t->slotNewCurrentWavelength((*reinterpret_cast< ProfileData*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ProfileWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ProfileWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ProfileWidget,
      qt_meta_data_ProfileWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ProfileWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ProfileWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ProfileWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProfileWidget))
        return static_cast<void*>(const_cast< ProfileWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ProfileWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void ProfileWidget::signalCurrentDepthChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
