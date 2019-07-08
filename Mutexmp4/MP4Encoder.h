/********************************************************************
版权所有:北京三海教育科技有限公司
作者：lijian
版本：V0.0.1
时间：2019-06-20
功能：用mp4v2库把H264和AAC裸码流合成MP4文件

V0.0.2
2019.07.04 解决合成的mp4文件总时长比实际总时长较短的问题，解决音频比视频播放快2s的问题(音频的时间精度设置为80000)
2019.07.05 优化代码,增加返回值异常处理，增加日志
2019.07.08 重新整理代码，加注释
*********************************************************************/

#include "mp4v2/mp4v2.h"
#include "EasyAACEncoder/faac.h"
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string>
#include <string.h>
#include "glog/logging.h"
#include "base.h"

#define HTON16(x)  ((x>>8&0xff)|(x<<8&0xff00))
#define HTON24(x)  ((x>>16&0xff)|(x<<16&0xff0000)|x&0xff00)
#define HTON32(x)  ((x>>24&0xff)|(x>>8&0xff00)| (x << 8 & 0xff0000) | (x << 24 & 0xff000000))

using namespace std;

extern string AACSTR,H264STR,JSONSTR,MP4STR,CFGSTR;
extern string aacTimestamp,h264Timestamp,whiteTimestamp,recordTimestr;

//Nalu结构体
typedef struct _MP4ENC_NaluUnit
{
   int type;
   int size;
   unsigned char*data;
}MP4ENC_NaluUnit;


typedef struct _RTMPFrame {
	bool m_bKeyFrame;
	int	 m_nTimeStamp;
}RTMPFrame;


class MP4Encoder
{
	
public:
     MP4Encoder(string &filepath, string &liveId);
     ~MP4Encoder();
public:

    //合成裸码流到MP4
    int MergeFilesToMp4();
	
    int MergeFilesToMp4(string fileName);

    int MergeFilesToMp4(int recordtimes);

	//创建mp4文件句柄
    MP4FileHandle CreateMP4File(const char* fileName,int width,int height,int timeScale=90000,int frameRate=25);

	//把H264和AAC按照时间戳写入mp4文件
    int WriteAacH264file(MP4FileHandle pFileMp4 ,const char* filename);

	//把一帧H264写入mp4文件
    int WriteH264Tag(MP4FileHandle hMp4File,const unsigned char* pData,int size, int timestramp);

	//把一帧AAC写入mp4文件
    int WriteAAcTag(MP4FileHandle hMp4File,const unsigned char *pData,int size,int timestramp);

	//关闭mp4文件句柄
    void CloseMP4File(MP4FileHandle hMp4File);
  
private:

    //MP4Encoder初始化
    void MP4EncoderInit();

	//根据获取音频属性,初始化AudioTrack
	int GetADTSInit(int nSampleRate, int nChannal, int bitsPerSample);

	//解析ADTS,获取音频属性
    void GetADTSInfo(unsigned char *aacheadbuff, int buffsize, int &nAudioSampleRate, int &channelConfiguration ,int &cnt);
   
private:

   /*配置文件*/
   CConfigFileReader *config_file;

   /*是否是首帧sps pps*/
   bool spsflag;
   bool ppsflag;

   /*MP4文件句柄*/
   MP4FileHandle  pFileMp4;

   int m_firstStamp;
   
   /*video属性字段*/
   MP4TrackId m_videoId;
   bool firstH264Tag;
   RTMPFrame  m_VLastFrame;
   
   int m_nWidth;   
   int m_nHeight;
   int m_nFrameRate;
   int m_nTimeScale;
   
   /*audio属性字段*/
   MP4TrackId m_audioId;
   int nAudioSampleRate;   
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

   int SampleRate;
   int channel;

   bool firstAacTag;
   RTMPFrame  m_ALastFrame;

   string m_filepath;
   string m_liveId;
   int recordTimes;
   
   int tagSize;
   int recordTime;

   /*h264tag和aacTag的缓冲区*/
   unsigned char *h264Tagbuffer;
   unsigned char *aacTagbuffer;
};
