
#include "packetlistmodel.h"
#include "base/jlog.h"



QVariant PacketListModel::data(const QModelIndex &index, int role) const {

  if (role == Qt::TextAlignmentRole) {
    if (index.column() <= 2) return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
    if (index.column() >= 3) return QVariant(Qt::AlignRight | Qt::AlignVCenter);
  }


  if (!af || role!=Qt::DisplayRole) return QVariant();

  ArchivePacket* ap = af->archivePacket(index.row());

  if (!ap) return QVariant();

  if (index.column() == 0) return QString(" ")+QString::number(ap->destinationUnit())+" ";
  if (index.column() == 1) return QString(" ")+QString::number(ap->packetType())+" ";
  if (index.column() == 2) return QString(" ")+QString::number(ap->senderUnit())+" ";
  if (index.column() == 3) return QString(" ")+QString::number(ap->modulus())+" ";
  if (index.column() == 4) return QString(" ")+QString::number(ap->dataLength())+" ";
  if (index.column() == 5) return QString(" ")+QString::number(ap->originTime())+" ";
  if (index.column() == 6) return QString(" ")+QString::number(ap->originTimeMsec())+" ";
  if (index.column() >= 7 && index.column() <= 15) return QString(" ")+QString::number(ap->channelBytes(index.column()-7))+" ";
  if (index.column() == 16) return ap->isBad() ? " N " : " Y ";
 
  /*
  printf("Packet Type: %d\n",packetType());
  printf("Logger ID: %d\n",senderUnit());
  printf("Modulus: %d\n",modulus());
  printf("Length: %d\n",dataLength());
  printf("Origin Time: %d (%.3f s)\n",(int)originTime(),(double)originTimeMsec()/1000);
  printf("Chan Bytes: ");
  for (int i=0;i<9;i++) printf(" %d",channelBytes(i)); 
  printf("\n");
  */

  return QVariant();
}



QVariant PacketListModel::headerData(int section, Qt::Orientation orientation, int role) const {

  if (role!=Qt::DisplayRole) return QVariant();

  if (orientation==Qt::Vertical) return section+1;

  if (section==0) return "Dest Unit";
  if (section==1) return "Pkt Type";
  if (section==2) return "Logger ID";
  if (section==3) return "Modulus";
  if (section==4) return "Length";
  if (section==5) return "Time";
  if (section==6) return "Msec";
  if (section >= 7 && section <= 15) return QString("c") + QString::number(section-7);
  if (section==16) return "OK";

  //if (section==1) return "Description";
  //if (section==2) return "Sensor";
  //if (section==3) return "Depth";
  //if (section==4) return "Clarity";
  //if (section==5) return "Library";
  //if (section==6) return "Benthos";
  //if (section==7) return "Result";

  return QVariant();
}


