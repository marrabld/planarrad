/****************************************************************************
** Meta object code from reading C++ file 'wltool_gui.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../wltool/gui/wltool_gui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wltool_gui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WLTool_Gui[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   12,   11,   11, 0x05,
      64,   60,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     111,  103,   11,   11, 0x0a,
     143,  136,   11,   11, 0x0a,
     178,  136,   11,   11, 0x0a,
     208,  136,   11,   11, 0x0a,
     250,  230,   11,   11, 0x0a,
     313,  230,   11,   11, 0x0a,
     375,   11,   11,   11, 0x0a,
     397,   11,   11,   11, 0x0a,
     434,  420,   11,   11, 0x0a,
     457,  451,   11,   11, 0x0a,
     486,  484,   11,   11, 0x0a,
     513,   11,   11,   11, 0x0a,
     544,   11,   11,   11, 0x0a,
     570,   11,   11,   11, 0x0a,
     592,   11,   11,   11, 0x0a,
     613,   11,   11,   11, 0x0a,
     624,   11,   11,   11, 0x0a,
     647,   11,   11,   11, 0x0a,
     668,   11,   11,   11, 0x0a,
     693,   11,   11,   11, 0x0a,
     725,   11,   11,   11, 0x0a,
     746,  743,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WLTool_Gui[] = {
    "WLTool_Gui\0\0rec_list\0"
    "signalNewRecordList(const RecordList*)\0"
    "rec\0signalNewRecordSelected(const Record*)\0"
    "ret_val\0processingFinished(bool)\0"
    "ret_ok\0finishedLoadProcessedEdAndLu(bool)\0"
    "finishedLoadProcessedEs(bool)\0"
    "finishedDoMerge(bool)\0selected,deselected\0"
    "slotRecordTableSelectionChanged(QItemSelection,QItemSelection)\0"
    "slotMergeTableSelectionChanged(QItemSelection,QItemSelection)\0"
    "slotLoadArchiveFile()\0slotCloseArchiveFile()\0"
    "channel_index\0slotDoMerge(int)\0index\0"
    "slotCurrentTabChanged(int)\0f\0"
    "slotChartKeysToggled(bool)\0"
    "slotLoadProcessedEdAndLuFile()\0"
    "slotLoadProcessedEsFile()\0"
    "slotSaveChannelFile()\0slotSaveMergedFile()\0"
    "slotQuit()\0slotCancelProcessing()\0"
    "slotUpdateProgress()\0slotSensorConfigDialog()\0"
    "slotVisualisationConfigDialog()\0"
    "slotAboutDialog()\0fp\0slotFilePathOpened(JString)\0"
};

void WLTool_Gui::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WLTool_Gui *_t = static_cast<WLTool_Gui *>(_o);
        switch (_id) {
        case 0: _t->signalNewRecordList((*reinterpret_cast< const RecordList*(*)>(_a[1]))); break;
        case 1: _t->signalNewRecordSelected((*reinterpret_cast< const Record*(*)>(_a[1]))); break;
        case 2: _t->processingFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->finishedLoadProcessedEdAndLu((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->finishedLoadProcessedEs((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->finishedDoMerge((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->slotRecordTableSelectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 7: _t->slotMergeTableSelectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 8: _t->slotLoadArchiveFile(); break;
        case 9: _t->slotCloseArchiveFile(); break;
        case 10: _t->slotDoMerge((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->slotCurrentTabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->slotChartKeysToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->slotLoadProcessedEdAndLuFile(); break;
        case 14: _t->slotLoadProcessedEsFile(); break;
        case 15: _t->slotSaveChannelFile(); break;
        case 16: _t->slotSaveMergedFile(); break;
        case 17: _t->slotQuit(); break;
        case 18: _t->slotCancelProcessing(); break;
        case 19: _t->slotUpdateProgress(); break;
        case 20: _t->slotSensorConfigDialog(); break;
        case 21: _t->slotVisualisationConfigDialog(); break;
        case 22: _t->slotAboutDialog(); break;
        case 23: _t->slotFilePathOpened((*reinterpret_cast< const JString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData WLTool_Gui::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject WLTool_Gui::staticMetaObject = {
    { &Tool_Gui::staticMetaObject, qt_meta_stringdata_WLTool_Gui,
      qt_meta_data_WLTool_Gui, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WLTool_Gui::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WLTool_Gui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WLTool_Gui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WLTool_Gui))
        return static_cast<void*>(const_cast< WLTool_Gui*>(this));
    return Tool_Gui::qt_metacast(_clname);
}

int WLTool_Gui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Tool_Gui::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    return _id;
}

// SIGNAL 0
void WLTool_Gui::signalNewRecordList(const RecordList * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WLTool_Gui::signalNewRecordSelected(const Record * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
