
#ifndef _WLTOOL_GUI_RECORDLISTMODEL_H
#define _WLTOOL_GUI_RECORDLISTMODEL_H

#include <QAbstractListModel>
#include "wl/archivefile.h"
#include "wl/record.h"

using namespace jude::wl;



class RecordListModel : public QAbstractListModel {

  ArchiveFile* af;

public:

  RecordListModel() : af(0) { }

  void setArchiveFile(ArchiveFile* a) { af = a; reset(); }

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

  int rowCount(const QModelIndex& = QModelIndex()) const { return (af) ? af->recordCount() : 0;  }
  int columnCount(const QModelIndex& = QModelIndex()) const { return 4; }

};



#endif // _WLTOOL_GUI_RECORDLISTMODEL_H

