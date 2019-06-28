/********************************************************************
filename:MP4Encoder.h
created:2019-06-20
author:lijian
purpose:¿mp4v2¿¿¿¿¿¿¿¿AAC¿h264¿¿mp4¿¿¿¿
*********************************************************************/

#include "mp4v2/mp4v2.h"
#include "EasyAACEncoder/faac.h"
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string>
#include <string.h>
#include "base.h"

#define HTON16(x)  ((x>>8&0xff)|(x<<8&0xff00))
#define HTON24(x)  ((x>>16&0xff)|(x<<16&0xff0000)|x&0xff00)
#define HTON32(x)  ((x>>24&0xff)|(x>>8&0xff00)| (x << 8 & 0xff0000) | (x << 24 & 0xff000000))

using namespace std;

typedef struct _MP4ENC_NaluUnit
{
   int type;
   int size;
   unsigned char*data;
}MP4ENC_NaluUnit;

//¿¿¿¿¿¿¿¿?
typedef struct _RTMPFrame {
	bool m_bKeyFrame;
	int	 m_nTimeStamp;
}RTMPFrame;

extern string AACSTR,H264STR,JSONSTR,MP4STR,CFGSTR;
extern string aacTimestamp,h264Timestamp,whiteTimestamp,recordTimestr;

class MP4Encoder
{
public:
     MP4Encoder(string &filepath, string &liveId);
   
     ~MP4Encoder();
public:

    //ºÏ³ÉÂãÂëÁ÷µ½MP4
    int MergeFilesToMp4();
    
    //¿¿¿¿¿¿¿mp4¿¿
    MP4FileHandle CreateMP4File(const char* fileName,int width,int height,int timeScale=90000,int frameRate=25);

    //¿¿¿¿¿¿aac¿¿ ,h264¿¿¿¿mp4¿¿
    bool WriteAacH264file(MP4FileHandle pFileMp4 ,const char* filename);
   
    //¿¿¿¿H264
    int WriteH264Tag(MP4FileHandle hMp4File,const unsigned char* pData,int size, int timestramp);
   	
    //¿¿¿¿Aac
    int WriteAAcTag(MP4FileHandle hMp4File,const unsigned char *pData,int size,int timestramp);
	
    //¿¿mp4¿¿
    void CloseMP4File(MP4FileHandle hMp4File);
  
private:
	
    //¿¿AAC ADTS¿¿?    
	int GetADTSInit(int nSampleRate, int nChannal, int bitsPerSample);
  
    //¿¿ADTS¿¿?¿¿MP4AudioTrackId¿¿
    void GetADTSInfo(unsigned char *aacheadbuff, int buffsize, int &nAudioSampleRate, int &channelConfiguration ,int &cnt);
   
private:

   /*ÅäÖÃÎÄ¼þ*/
   CConfigFileReader *config_file;
   
   //¿¿¿¿pps¿sps¿¿
   bool spsflag;
   bool ppsflag;

   //MP4¿¿¿¿
   MP4FileHandle  pFileMp4;

   int m_firstStamp;

   //¿¿ TrackId¿¿¿¿?   
   int m_nWidth;   
   int m_nHeight;
   int m_nFrameRate;
   int m_nTimeScale;
   MP4TrackId m_videoId;

   bool firstH264Tag;
   RTMPFrame  m_VLastFrame;
 
   
   //¿¿ TrackId¿¿¿¿?   
   bool aacfirst;
   int nAudioSampleRate;
   MP4TrackId m_audioId;
   faacEncHandle m_hEncoder;
   int m_nSampleRate;
   int m_nAudioChannal;
   int m_nBitsPerSample;
   unsigned long m_nInputSamples;
   unsigned long m_nMaxOutputBytes; 
   unsigned long m_nMaxInputBytes;
   
   char *m_pbPCMBuffer;
   char *m_pOutAACBuffer;
   char *m_pTempBuffer;

   int tagSize;

   int SampleRate;
   int channel;

   bool firstAacTag;
   RTMPFrame  m_ALastFrame;

   string m_filepath;
   string m_liveId;
   int recordTimes;
   
   int LastVedioTimestamp;
   int LastAudioTimestamp;
   int LasWhiteTimestamp;
					   
   int totalVedioTimestamp;
   int totalAudioTimestamp;
   int totalWhiteTimestamp;

   int recordTime;
};
