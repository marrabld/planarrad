/****************************************************************************
** Meta object code from reading C++ file 'datawidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../planarrad/gui/datawidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'datawidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DataWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      36,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      56,   11,   11,   11, 0x0a,
      67,   11,   11,   11, 0x0a,
      80,   11,   11,   11, 0x0a,
     115,   95,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DataWidget[] = {
    "DataWidget\0\0signalDataSetsChanged()\0"
    "signalReportError()\0slotLoad()\0"
    "slotRemove()\0slotUnselect()\0"
    "selected,deselected\0"
    "slotSelectionChanged(QItemSelection,QItemSelection)\0"
};

void DataWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DataWidget *_t = static_cast<DataWidget *>(_o);
        switch (_id) {
        case 0: _t->signalDataSetsChanged(); break;
        case 1: _t->signalReportError(); break;
        case 2: _t->slotLoad(); break;
        case 3: _t->slotRemove(); break;
        case 4: _t->slotUnselect(); break;
        case 5: _t->slotSelectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DataWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DataWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DataWidget,
      qt_meta_data_DataWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataWidget))
        return static_cast<void*>(const_cast< DataWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int DataWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void DataWidget::signalDataSetsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void DataWidget::signalReportError()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
