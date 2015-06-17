/****************************************************************************
** Meta object code from reading C++ file 'modelwidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../planarrad/gui/modelwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modelwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ModelWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      35,   33,   12,   12, 0x05,
      60,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      89,   12,   12,   12, 0x0a,
     124,   12,   12,   12, 0x0a,
     155,   12,   12,   12, 0x0a,
     190,   12,   12,   12, 0x0a,
     214,   12,   12,   12, 0x0a,
     236,   12,   12,   12, 0x0a,
     266,  259,   12,   12, 0x0a,
     295,   12,   12,   12, 0x0a,
     318,  316,   12,   12, 0x0a,
     350,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ModelWidget[] = {
    "ModelWidget\0\0signalReportError()\0f\0"
    "signalIsProcessing(bool)\0"
    "signalProcessingFinishedOk()\0"
    "slotBuildInputRadianceFileThread()\0"
    "slotBuildInterfaceFileThread()\0"
    "slotBuildPhaseFunctionFileThread()\0"
    "slotRunSlabToolThread()\0slotStartProcessing()\0"
    "slotCancelProcessing()\0ret_ok\0"
    "slotProcessingFinished(bool)\0"
    "slotUpdateProgress()\0s\0"
    "slotProcessingActivity(JString)\0"
    "slotCurrentDirChanged()\0"
};

void ModelWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ModelWidget *_t = static_cast<ModelWidget *>(_o);
        switch (_id) {
        case 0: _t->signalReportError(); break;
        case 1: _t->signalIsProcessing((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->signalProcessingFinishedOk(); break;
        case 3: _t->slotBuildInputRadianceFileThread(); break;
        case 4: _t->slotBuildInterfaceFileThread(); break;
        case 5: _t->slotBuildPhaseFunctionFileThread(); break;
        case 6: _t->slotRunSlabToolThread(); break;
        case 7: _t->slotStartProcessing(); break;
        case 8: _t->slotCancelProcessing(); break;
        case 9: _t->slotProcessingFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->slotUpdateProgress(); break;
        case 11: _t->slotProcessingActivity((*reinterpret_cast< const JString(*)>(_a[1]))); break;
        case 12: _t->slotCurrentDirChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ModelWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ModelWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ModelWidget,
      qt_meta_data_ModelWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ModelWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ModelWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ModelWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ModelWidget))
        return static_cast<void*>(const_cast< ModelWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ModelWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void ModelWidget::signalReportError()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ModelWidget::signalIsProcessing(bool _t1)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(const_cast< ModelWidget *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ModelWidget::signalProcessingFinishedOk()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
