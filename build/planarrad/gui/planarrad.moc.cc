/****************************************************************************
** Meta object code from reading C++ file 'planarrad.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../planarrad/gui/planarrad.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'planarrad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PlanarRad[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,
      37,   10,   10,   10, 0x0a,
      63,   10,   10,   10, 0x0a,
      88,   86,   10,   10, 0x0a,
     111,   10,   10,   10, 0x0a,
     138,   10,   10,   10, 0x0a,
     162,  160,   10,   10, 0x0a,
     194,   10,   10,   10, 0x0a,
     212,   10,   10,   10, 0x0a,
     242,   10,   10,   10, 0x0a,
     275,   10,   10,   10, 0x0a,
     293,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PlanarRad[] = {
    "PlanarRad\0\0slotLoadModelParameters()\0"
    "slotSaveModelParameters()\0"
    "slotChangeCurrentDir()\0f\0"
    "slotIsProcessing(bool)\0"
    "slotProcessingFinishedOk()\0"
    "slotDataSetsChanged()\0d\0"
    "slotCurrentDepthChanged(double)\0"
    "slotReportError()\0slotAdditionalOptionsDialog()\0"
    "slotVisualisationOptionsDialog()\0"
    "slotAboutDialog()\0slotQuit()\0"
};

void PlanarRad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PlanarRad *_t = static_cast<PlanarRad *>(_o);
        switch (_id) {
        case 0: _t->slotLoadModelParameters(); break;
        case 1: _t->slotSaveModelParameters(); break;
        case 2: _t->slotChangeCurrentDir(); break;
        case 3: _t->slotIsProcessing((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->slotProcessingFinishedOk(); break;
        case 5: _t->slotDataSetsChanged(); break;
        case 6: _t->slotCurrentDepthChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->slotReportError(); break;
        case 8: _t->slotAdditionalOptionsDialog(); break;
        case 9: _t->slotVisualisationOptionsDialog(); break;
        case 10: _t->slotAboutDialog(); break;
        case 11: _t->slotQuit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PlanarRad::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PlanarRad::staticMetaObject = {
    { &Tool_Gui::staticMetaObject, qt_meta_stringdata_PlanarRad,
      qt_meta_data_PlanarRad, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PlanarRad::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PlanarRad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PlanarRad::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PlanarRad))
        return static_cast<void*>(const_cast< PlanarRad*>(this));
    return Tool_Gui::qt_metacast(_clname);
}

int PlanarRad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Tool_Gui::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
