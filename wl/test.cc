
#include <stdlib.h>
#include <string.h>
#include "archivefile.h"
#include "config.h"
#include "byteio.h"



// main

int main(int argc, char **argv) {

  if (argc!=2) {
    fprintf(stderr,"Missing input file.\n");
    return 1;
  }

  char* input_name = argv[1];

  //AutoConfig auto_conf;

  Config::init(Sensor::ACS, Sensor::CTD, Sensor::ECOBB3);

  //config.channel(3).setColumn(1, "Date", WLSensorPacket::DATA_TEXT); 
  //config.channel(3).setColumn(2, "Time", WLSensorPacket::DATA_TEXT); 
  //config.channel(3).setColumn(3, "dark550", WLSensorPacket::DATA_DOUBLE); 
  //config.channel(3).setColumn(4, "sig550", WLSensorPacket::DATA_DOUBLE); 
 
  //auto_conf.add(config);

  ArchiveFile af;
  af.read(input_name);
  af.display();

  Config::clear();

  return 0;
  /*
  //char output_name[strlen(input_name)+4];
 
  strcpy(output_name, input_name);
  strcpy(output_name+strlen(input_name), ".raw");
  output_name[strlen(input_name)+4] = 0;
  printf("%s -> %s\n",input_name,output_name);
  
  FILE* fin = fopen(input_name,"r");
  if (!fin) { printf("failed to open input file %s\n", input_name); exit(1); }

  //FILE* fout = fopen(output_name,"w");
  //if (!fout) { printf("failed to open ouput file %s\n", output_name); exit(1); }

  

  WLArchivePacket p;
  int i = 0;
  while (!feof(fin)) {
    if (!p.read(fin)) { printf("EOF\n"); break; }
    //p.display();
    printf("Packet %d %.3f [%d] ",i++,(double)p.originTimeMsec()/1000,p.dataLength());
    for (int j=0;j<9;j++) printf("%d ",p.channelBytes(j));
    printf("\n");
    for (int j=0;j<9;j++) {
      if (p.channelBytes(j) > 0) {
	printf(" Chan: %d ",j); 
	p.displayChannelData(j);
      }
    }
    if (p.destinationUnit()!=1) { printf("ERROR\n"); }
  }
  */
  /*

  int c1 = fgetc(fin);
  int c2 = fgetc(fin);
  int c3 = fgetc(fin);
  int c4 = fgetc(fin);
 
  int packet_count = 0;

  do {
    if (c1==255 && c2==0 && c3==255 && c4==0) { 
      ACSPacket* a = new ACSPacket(fin);
      packet_count++;
      printf("\nACS Packet %d\n",packet_count);
      //a->display();
      a->write(fout);
      c1 = fgetc(fin);
      c2 = fgetc(fin);
      c3 = fgetc(fin);
      c4 = fgetc(fin);
      if (c1>31 && c1<='z') printf("%c",c1);
      else printf("[%d]",c1);
      if (c2>31 && c2<='z') printf("%c",c2);
      else printf("[%d]",c2);
      if (c3>31 && c3<='z') printf("%c",c3);
      else printf("[%d]",c3);
      if (c4>31 && c4<='z') printf("%c",c4);
      else printf("[%d]",c4);
    } else {
      if (c4>31 && c4<='z') printf("%c",c4);
      else printf("[%d]",c4);
      c1 = c2;
      c2 = c3;
      c3 = c4;
      c4 = fgetc(fin);
    }
  } while (c4!=EOF);
  */

  //  fclose(fin);
  //fclose(fout);

  //printf("\n");
  //printf("Total ACS Packets: %d\n",packet_count);

  return 0;
}

