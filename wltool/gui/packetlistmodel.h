
#ifndef _WLTOOL_GUI_PACKETLISTMODEL_H
#define _WLTOOL_GUI_PACKETLISTMODEL_H

#include <QAbstractListModel>
#include "wl/archivefile.h"
#include "wl/record.h"

using namespace jude::wl;



class PacketListModel : public QAbstractListModel {

  ArchiveFile* af;

public:

  PacketListModel() : af(0) { }

  void setArchiveFile(ArchiveFile* a) { af = a; reset(); }

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

  int rowCount(const QModelIndex& = QModelIndex()) const { return (af) ? af->packetCount() : 0;  }
  //int rowCount(const QModelIndex& = QModelIndex()) const { return (af) ? 10 : 0;  }
  //int rowCount(const QModelIndex& = QModelIndex()) const { return 10;  }
  int columnCount(const QModelIndex& = QModelIndex()) const { return 17; }

};



#endif // _WLTOOL_GUI_PACKETLISTMODEL_H

