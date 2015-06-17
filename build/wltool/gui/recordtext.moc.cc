/****************************************************************************
** Meta object code from reading C++ file 'recordtext.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../wltool/gui/recordtext.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'recordtext.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RecordText[] = {

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
      16,   12,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RecordText[] = {
    "RecordText\0\0rec\0slotNewRecordSelected(const Record*)\0"
};

void RecordText::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RecordText *_t = static_cast<RecordText *>(_o);
        switch (_id) {
        case 0: _t->slotNewRecordSelected((*reinterpret_cast< const Record*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RecordText::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RecordText::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_RecordText,
      qt_meta_data_RecordText, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RecordText::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RecordText::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RecordText::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RecordText))
        return static_cast<void*>(const_cast< RecordText*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int RecordText::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
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
