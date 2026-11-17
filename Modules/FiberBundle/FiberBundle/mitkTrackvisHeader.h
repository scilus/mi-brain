#ifndef _TRACKVIS_HEADER
#define _TRACKVIS_HEADER

// Structure to hold metadata of a TrackVis file
// ---------------------------------------------
struct TrackVis_header
{
  char idString[6];
  short int dim[3];
  float voxelSize[3];
  float origin[3];
  short int nbScalars;
  char scalarName[10][20];
  short int nbProperties;
  char propertyName[10][20];
  float voxToWorld[16];
  char reserved[444];
  char voxelOrder[4];
  char pad2[4];
  float imageOrientationPatient[6];
  char pad1[2];
  unsigned char invertX;
  unsigned char invertY;
  unsigned char invertZ;
  unsigned char swapXY;
  unsigned char swapYZ;
  unsigned char swapZX;
  int nbCount;
  int version;
  int hdrSize;
};

#endif
