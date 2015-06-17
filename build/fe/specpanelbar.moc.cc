/****************************************************************************
** Meta object code from reading C++ file 'specpanelbar.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../fe/specpanelbar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'specpanelbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_jude__fe__SpecPanelBar[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
      43,   41,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_jude__fe__SpecPanelBar[] = {
    "jude::fe::SpecPanelBar\0\0buttonsChanged()\0"
    "b\0buttonClicked(Button*)\0"
};

void jude::fe::SpecPanelBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpecPanelBar *_t = static_cast<SpecPanelBar *>(_o);
        switch (_id) {
        case 0: _t->buttonsChanged(); break;
        case 1: _t->buttonClicked((*reinterpret_cast< Button*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData jude::fe::SpecPanelBar::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject jude::fe::SpecPanelBar::staticMetaObject = {
    { &Q3VBox::staticMetaObject, qt_meta_stringdata_jude__fe__SpecPanelBar,
      qt_meta_data_jude__fe__SpecPanelBar, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &jude::fe::SpecPanelBar::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *jude::fe::SpecPanelBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *jude::fe::SpecPanelBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_jude__fe__SpecPanelBar))
        return static_cast<void*>(const_cast< SpecPanelBar*>(this));
    return Q3VBox::qt_metacast(_clname);
}

int jude::fe::SpecPanelBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3VBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void jude::fe::SpecPanelBar::buttonsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
