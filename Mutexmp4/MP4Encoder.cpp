#include"MP4Encoder.h"
#include<string.h>
#include<stdio.h>
#include <stdlib.h>

int BUFFER_SIZE = 10*1024*1024;

MP4Encoder::MP4Encoder(string &filepath, string &liveId):
m_videoId(NULL),
m_audioId(NULL),
m_nWidth(0),
m_nHeight(0),
m_nTimeScale(0),
m_nFrameRate(0)
{

     m_filepath = filepath;
     m_liveId = liveId;

     spsflag = false;
     ppsflag = false;

     tagSize = 0;
   
     firstH264Tag  = false;
	
     m_firstStamp = 0;
	 
     m_pbPCMBuffer = NULL;
     m_pOutAACBuffer = NULL;
     m_pTempBuffer = NULL;

     firstAacTag = false;	
     m_VLastFrame.m_nTimeStamp = 0;
     m_VLastFrame.m_bKeyFrame = false;

     m_ALastFrame.m_nTimeStamp = 0;
     m_ALastFrame.m_bKeyFrame = false;\
	 
	 
      LastVedioTimestamp = 0;
     LastAudioTimestamp = 0;
     LasWhiteTimestamp = 0;
					   
     totalVedioTimestamp = 0;
     totalAudioTimestamp = 0;
     totalWhiteTimestamp = 0;

     recordTime = 0;
}

MP4Encoder::~MP4Encoder()
{

    /*if(pFileMp4)
    {
      MP4Close(pFileMp4);
      pFileMp4=NULL;
    }*/
	if(NULL != m_pbPCMBuffer)
	{
		delete []m_pbPCMBuffer;
		m_pbPCMBuffer = NULL;
	}
	
	if(NULL != m_pOutAACBuffer)
	{
		delete []m_pOutAACBuffer;
		m_pOutAACBuffer = NULL;
	}
	if(NULL != m_pTempBuffer)
	{
	   delete []m_pTempBuffer;
	   m_pTempBuffer = NULL;	
	}
}

int MP4Encoder::MergeFilesToMp4()
{
	int resCode = 0;
	string cfgfileStr =  m_filepath + m_liveId + "/"  + m_liveId + CFGSTR;
	config_file = new CConfigFileReader(cfgfileStr.c_str());
	
	if(NULL != config_file)
        {
		 recordTimes = std::stoi(config_file->GetConfigName(recordTimestr.c_str()));
		 
		 string mp4name = m_filepath + m_liveId + "/"  + m_liveId + MP4STR;
                 pFileMp4 = CreateMP4File(mp4name.c_str(),352,288);
	  
	          bool retcode = true;
		 for(int i =0 ; i< recordTimes; i++)
		 {
                         recordTime  =  i;
			 if(i != 0)
			 {
				 string numberStr = "(";
				 numberStr.append("+").append(std::to_string(i)).append(")");
				 string fileName = m_filepath + m_liveId + "/"  + m_liveId + numberStr;
				 
				 if(i != 1)
				 {
					 string aackey = aacTimestamp+std::to_string(i-1);
			                 LastVedioTimestamp = std::stoi(config_file->GetConfigName(aackey.c_str()));
			 
			                 string h264key = h264Timestamp+std::to_string(i-1);
                                         LastAudioTimestamp = std::stoi(config_file->GetConfigName(h264key.c_str()));
			 
			                 string whitekey = whiteTimestamp+std::to_string(i-1);
                                         LasWhiteTimestamp = std::stoi(config_file->GetConfigName(whitekey.c_str()));					 
				 }else
				 {
					  LastVedioTimestamp = std::stoi(config_file->GetConfigName(aacTimestamp.c_str()));
                                          LastAudioTimestamp = std::stoi(config_file->GetConfigName(h264Timestamp.c_str()));
                                          LasWhiteTimestamp = std::stoi(config_file->GetConfigName(whiteTimestamp.c_str()));			   
				 }
				 
			         totalVedioTimestamp = LastVedioTimestamp;
                                 totalAudioTimestamp = LastAudioTimestamp;
                                 totalWhiteTimestamp = LasWhiteTimestamp;	
				
                                  //firstH264Tag  = false;
                                  m_firstStamp = 0;

                                  //firstAacTag = false;
                                  m_VLastFrame.m_nTimeStamp = 0;
                               
                                  m_ALastFrame.m_nTimeStamp = 0;
                               
				 retcode = WriteAacH264file(pFileMp4, fileName.c_str());
				 
		   }else
		   {
				 string fileName = m_filepath + m_liveId + "/"  + m_liveId;
				 retcode = WriteAacH264file(pFileMp4, fileName.c_str());
		   }
	}
	}else
	{
		resCode = 1;
		return resCode;
	}
	
	CloseMP4File(pFileMp4);
	
	return resCode;
}


int MP4Encoder::MergeFilesToMp4(string fileName)
{
     spsflag = false;
     ppsflag = false;

     tagSize = 0;
   
     firstH264Tag  = false;
	
     m_firstStamp = 0;
	 
     m_pbPCMBuffer = NULL;
     m_pOutAACBuffer = NULL;
     m_pTempBuffer = NULL;

     firstAacTag = false;	
     m_VLastFrame.m_nTimeStamp = 0;
     m_VLastFrame.m_bKeyFrame = false;

     m_ALastFrame.m_nTimeStamp = 0;
     m_ALastFrame.m_bKeyFrame = false;
	 
     m_videoId = NULL;
     m_audioId = NULL;
     m_nWidth = 0;
     m_nHeight = 0;
     m_nTimeScale = 0;
     m_nFrameRate = 0;
	
	
     recordTime = 0;
     bool retcode = true;
	 
     string mp4name = fileName + MP4STR;
     pFileMp4 = CreateMP4File(mp4name.c_str(),352,288);
	 
     retcode = WriteAacH264file(pFileMp4, fileName.c_str());

     CloseMP4File(pFileMp4);
	 
     if(retcode)
     {
	 return 0;
     }else
     {
         return 2;
      }
}

int MP4Encoder::MergeFilesToMp4(int recordtimes)
{
    int resCode = 0;
    int recordTimes = recordtimes;
		 
    string mp4name = m_filepath + m_liveId + "/"  + m_liveId + MP4STR;
    pFileMp4 = CreateMP4File(mp4name.c_str(),352,288);
	  
    bool retcode = true;
	
    for(int i =0 ; i< recordTimes; i++)
    {
             recordTime  =  i;
	     if(i != 0)
	     {
		    string numberStr = "(";
		    numberStr.append("+").append(std::to_string(i)).append(")");
		    string fileName = m_filepath + m_liveId + "/"  + m_liveId + numberStr;
			
                     m_firstStamp = 0;

                     tagSize = 0;
   
                     firstH264Tag  = false;
                     m_firstStamp = 0;
	 
                     m_pbPCMBuffer = NULL;
                     m_pOutAACBuffer = NULL;
                     m_pTempBuffer = NULL;

                     firstAacTag = false;

                     m_VLastFrame.m_nTimeStamp = 0;
                     m_ALastFrame.m_nTimeStamp = 0;

                     retcode = WriteAacH264file(pFileMp4, fileName.c_str());
				 
	    }else
	    {
		      string fileName = m_filepath + m_liveId + "/"  + m_liveId;
		      retcode = WriteAacH264file(pFileMp4, fileName.c_str());
            } 
     }
	
     CloseMP4File(pFileMp4);
	
     return resCode;
}

MP4FileHandle MP4Encoder::CreateMP4File(const char *pFileName,int width,int height,int timeScale/*=90000*/,int frameRate/*=25*/)
{
   if(pFileName==NULL)
   {
     return false;
   }
   
   //createmp4file
   MP4FileHandle hMp4file=MP4Create(pFileName);
   if(hMp4file==MP4_INVALID_FILE_HANDLE)
   {
      printf("ERROR:Openfilefialed.\n");
      return false;
   }
   m_nWidth=width;
   m_nHeight=height;
   m_nTimeScale=90000;
   m_nFrameRate=30;
   MP4SetTimeScale(hMp4file,m_nTimeScale);
   return hMp4file;
}

int MP4Encoder::WriteH264Tag(MP4FileHandle hMp4File,const unsigned char* pData,int size, int timeStramp)
{
   if(hMp4File == NULL)
   {
     return -1;
   }
   if(pData == NULL)
   {
     return -1;
   }
   MP4ENC_NaluUnit nalu;
   nalu.type = pData[0]&0x1f;
   nalu.data =(unsigned char*)pData;
   nalu.size = size;
   
   //printf("shijiancuo xieru 111111: %d\n", timeStramp);

   if(nalu.type==0x07)//sps
   {
      if(!spsflag)
      {
           m_videoId=MP4AddH264VideoTrack
                    (hMp4File,
                     m_nTimeScale,
                     MP4_INVALID_DURATION, ///m_nTimeScale/m_nFrameRate           
                     m_nWidth,//width
                     m_nHeight,//height
                     nalu.data[1],//sps[1]AVCProfileIndication
                     nalu.data[2],//sps[2]profile_compat
                     nalu.data[3],//sps[3]AVCLevelIndication
                     3);//4byteslengthbeforeeachNALunit
          if(m_videoId==MP4_INVALID_TRACK_ID)
          {
             printf("add video track failed.\n");
             return 0;
          }
          MP4SetVideoProfileLevel(hMp4File,0x7F);//SimpleProfile@Level3
          MP4AddH264SequenceParameterSet(hMp4File,m_videoId,nalu.data,nalu.size);
          spsflag = true;
    }
  }else if(nalu.type==0x08)//pps
  {
       if(!ppsflag)
       {
         MP4AddH264PictureParameterSet(hMp4File,m_videoId,nalu.data,nalu.size);
         ppsflag = true;
       }
 }else
 {
     MP4Duration uDuration = 0;
	 
     if(!firstH264Tag)
     {
	      m_firstStamp = timeStramp;
	      firstH264Tag = true;
     }else
     {       
            int uFrameMS = timeStramp - m_VLastFrame.m_nTimeStamp;
            uDuration  = (m_nTimeScale/1000)*uFrameMS;
         
		      
         /*if(uDuration < 0|| uDuration == 0 )
         {
            uDuration  = m_nTimeScale/m_nFrameRate;
         }*/
     }

    m_VLastFrame.m_bKeyFrame = firstH264Tag;
	
    if(uDuration > 10000)
    {
           printf("h264 uDuration:%d  lastTimeStramp:%d  timeStramp :%d :\n", uDuration, m_VLastFrame.m_nTimeStamp,timeStramp);
    }
    
    m_VLastFrame.m_nTimeStamp = timeStramp;
    
	 
     int datalen=nalu.size+4;
     unsigned char *data=new unsigned char[datalen];
     data[0]=nalu.size>>24;
     data[1]=nalu.size>>16;
     data[2]=nalu.size>>8;
     data[3]=nalu.size&0xff;
     memcpy(data+4,nalu.data,nalu.size);

     /*if(uDuration > 10000)
     {
           printf("h264 uDuration:%d  %d : \n", uDuration);
     }*/
     //printf("h264 uDuration:%d: \n", uDuration);
	 
     if(!firstH264Tag)
     {
           MP4WriteSample(hMp4File,m_videoId,data,datalen,m_nTimeScale/m_nFrameRate,0,1);
     }else
     {
	  MP4WriteSample(hMp4File,m_videoId,data,datalen,uDuration,0,0);
     }
     tagSize++;
     delete[]data;
 }
}


int MP4Encoder::WriteAAcTag(MP4FileHandle hMp4File,const unsigned char* pBufferG711,int nG711Len,int timeStramp)
{
     if(hMp4File == NULL)
     {
        return -1;
     }
     if(pBufferG711 == NULL)
     { 
        return -2;
     }
	 
     MP4Duration uDuration = 0;
     if (!firstAacTag) 
     {	
          firstAacTag = true;
     } else 
     {
                //if(m_ALastFrame.m_nTimeStamp != 0)
                //{
                     int uFrameMS = timeStramp - m_ALastFrame.m_nTimeStamp;
                     uDuration  = (80000/1000)*uFrameMS;

                //}
                //int uFrameMS = timeStramp - m_ALastFrame.m_nTimeStamp;
                //uDuration  = (m_nSampleRate/1000)*uFrameMS;
		 
		if(uDuration < 0 || uDuration == 0)
		{
		    uDuration = 80000/1000;
		}
     }

     m_ALastFrame.m_bKeyFrame = firstAacTag;
	
     /*if(recordTime > 0 && timeStramp > 0 && m_ALastFrame.m_nTimeStamp == 0) //从第二个合成开始
     {
        int uFrameMS =  timeStramp - totalAudioTimestamp;
        uDuration  = (m_nTimeScale/1000)*uFrameMS;     

     }*/

    if(uDuration > 10000)
    {
           printf("aac uDuration:%d  lastTimeStramp:%d  timeStramp :%d :\n", uDuration, m_ALastFrame.m_nTimeStamp,timeStramp);
    }
	 
     m_ALastFrame.m_nTimeStamp = timeStramp;
    
     if(!firstAacTag)
     {
          MP4WriteSample(hMp4File, m_audioId, pBufferG711, nG711Len,80000/1000 , 0, 1);
     }else
     {
          MP4WriteSample(hMp4File, m_audioId, pBufferG711, nG711Len, uDuration, 0, 1);      
     }
     return 0;
}


void MP4Encoder::CloseMP4File(MP4FileHandle hMp4File)
{
   if(hMp4File)
   {
      MP4Close(hMp4File);
      hMp4File=NULL;
   }
}

bool MP4Encoder::WriteAacH264file(MP4FileHandle pFileMp4, const char* file)
{
      /*string mp4name = file;
      mp4name = mp4name.append(".mp4");
      pFileMp4 = CreateMP4File(mp4name.c_str(),352,288);*/

      printf("wenjianming: %s\n",file);

      string h264name = file;
      h264name = h264name.append(H264STR);
      FILE*fp264=fopen(h264name.c_str(),"rb");
      if(!fp264)
      {
         printf("ERROR: open h264 file failed!");
         return false;
      }
	  
	  fseek(fp264, 0L, SEEK_END); 
	  int h264filelen = ftell(fp264); 
      fseek(fp264, 0L, SEEK_SET);
      unsigned char *buffer = new unsigned char[BUFFER_SIZE];
    
	  char h264headbuff[4] = {0};
	  int H264TAGSIZE = 4;
	 
	  int h264DataSize = 0;
	  int h264WriteTotal = 0;
	  int h264timestramp = 0;
	  
      string aacname = file;
      aacname = aacname.append(AACSTR);
      FILE*fpaac=fopen(aacname.c_str(),"rb");
      if(!fpaac)
      {
         printf("ERROR: open aac file failed!");
         return false;
      }
      fseek(fpaac, 0L, SEEK_END); 
      int aacfilelen = ftell(fpaac); 
      fseek(fpaac, 0L, SEEK_SET);
      int aacWriteTotal = 0; 
	  int ADTSHEADSIZE = 7;
	  int aacdataSize = 0;
	  int cnt = 0;
	  int aactimestramp = 0;
	  int nAudioSampleRate = 0;
	  int channelConfiguration = 0; 
	  unsigned char aacheadbuff[7] = {0};
	  unsigned char *aacframe=(unsigned char *)malloc(BUFFER_SIZE);
	  
	  bool h264flag = true;
	  bool aacflag = true;
	 
	  bool h264over = false;
	  bool accover = false;
	  
	  
	  while((h264over && accover) == false)
      {	  
		  if(h264flag)
		  {
			   if(h264WriteTotal < h264filelen)
			   {
                  memset(h264headbuff,0,H264TAGSIZE);
                  h264DataSize = 0; 
		          int data_size = fread(h264headbuff,1,H264TAGSIZE,fp264);            
		          if(data_size < H264TAGSIZE)
		          {                 
			          h264over = true;
			          if(accover)
			          {
				          break;                                    
				      }
			          continue;
		          }
		          h264WriteTotal += H264TAGSIZE;   

		          memcpy(&h264DataSize, h264headbuff, H264TAGSIZE);  		  
		          h264DataSize = HTON32(h264DataSize);
		   
		          memset(buffer,BUFFER_SIZE,0);
		          data_size = fread(buffer,1,h264DataSize,fp264);
		          if(data_size < h264DataSize)
		          {
		              h264over = true;
			          if(accover)
			          {
				         break;
			          }
			          continue;
		          }			
		          h264WriteTotal += h264DataSize;
                          //printf("%#X %#X %#X %#X\n",buffer[0], buffer[1], buffer[2], buffer[3]);
	 	
                          memcpy(&h264timestramp, buffer, 3);
                          h264timestramp = HTON24(h264timestramp);

                          //h264timestramp  = h264timestramp + totalVedioTimestamp;
                          /*if(recordTime >  0 && h264timestramp == 0)
                          {
                             h264timestramp =  m_VLastFrame.m_nTimeStamp;
                          }*/
		
				  
		  if(h264WriteTotal > h264filelen || h264WriteTotal == h264filelen)
                  {
                       h264over = true;
                       unsigned char *h264databuff = buffer+4;
                       
		       WriteH264Tag(pFileMp4,h264databuff,h264DataSize - 4,h264timestramp);
                       //printf("zui hou yige h246 xieru: %d\n",h264DataSize - 4); 
                  }  
		          //printf("h264 huoqudaode shijiancuo:%d %d %d \n",h264timestramp,h264WriteTotal, h264filelen);
			 }else
             {
				 h264over = true;
			 }				 
		}	
		if(aacflag)
		{
			if(aacWriteTotal < aacfilelen)
			{
			    memset(aacheadbuff,0,ADTSHEADSIZE);
                              
                int data_size = fread(aacheadbuff, 1,ADTSHEADSIZE , fpaac);
                if(data_size < ADTSHEADSIZE)
                {
                    accover = true;
		            if(h264over)
		            {
		             	break;
	                }
		            continue;
                }
               
		        aacdataSize = 0;  
                aacdataSize |= (aacheadbuff[3] & 0x03) <<11;     //high 2 bit
                aacdataSize |= (aacheadbuff[4])<<3;                //middle 8 bit
                aacdataSize |= (aacheadbuff[5] & 0xe0)>>5;     //low 3bit
				
	            aacWriteTotal += ADTSHEADSIZE;				
		        //GetADTSInfo(aacheadbuff,aacdataSize,nAudioSampleRate,channelConfiguration,cnt);
			
	            if(cnt == 0 && recordTime == 0)
	            {
                       if(!firstAacTag)
                       {
                            GetADTSInfo(aacheadbuff,aacdataSize,nAudioSampleRate,channelConfiguration,cnt);	   
		            GetADTSInit(SampleRate,channel,32);
					    
		             //m_nTimeScale=SampleRate;
                             //MP4SetTimeScale(pFileMp4,m_nTimeScale);
                      }
	            }  
	            cnt++;
				
	            memset(aacframe ,BUFFER_SIZE ,0);               
                data_size = fread(aacframe,1,aacdataSize-7,fpaac);
                if(data_size < aacdataSize - 7)
                {
                     accover = true;
		             if(h264over)
	                 {
			             break;
		             }
		             continue;
                }
                aacWriteTotal += data_size;
				
                //printf("%#X %#X %#X %#X\n",aacframe[0], aacframe[1], aacframe[2], aacframe[3]);
		        memcpy(&aactimestramp, aacframe, 3);
                aactimestramp = HTON24(aactimestramp);
				
                //aactimestramp =aactimestramp + totalAudioTimestamp;
				
	        //printf("aac huoqudaode shijiancuo:%d %d %d\n",aactimestramp, aacWriteTotal, aacfilelen);
                if(aacWriteTotal > aacfilelen || aacWriteTotal == aacfilelen)
                {
                     accover = true;
                     unsigned char *aacdatabuff = aacframe+4;
		             WriteAAcTag(pFileMp4,aacdatabuff, aacdataSize - 11,aactimestramp);
                }         
			}else
               {
				accover = true;
				if(h264over)
				{
					break;
				}
			}				
		}
		
		if(!h264over && !accover)
		{
		    if(h264timestramp < aactimestramp || h264timestramp == aactimestramp )
		    {
			       unsigned char *h264databuff = buffer+4;
		           WriteH264Tag(pFileMp4,h264databuff,h264DataSize - 4,h264timestramp);
                   //printf("h264 xieru: %d\n",h264DataSize - 4);      
			       aacflag = false;
                               h264flag = true;
		    }else
		    {
         
                   unsigned char *aacdatabuff = aacframe+4;
			       WriteAAcTag(pFileMp4,aacdatabuff,aacdataSize - 11,aactimestramp);
                   //printf("aac xieru: %d\n",aacdataSize - 4); 
			       h264flag = false;
                   aacflag = true;
		    }
	   }else
	   { 
           if(accover && h264over)
           {
               break;
                 
           }else if(accover && !h264over)
	       {
		           unsigned char *h264databuff = buffer+4;             
		           WriteH264Tag(pFileMp4,h264databuff,h264DataSize - 4, h264timestramp);
                   //printf("aac wanle h246 xieru: %d\n",h264DataSize - 4);     
		           aacflag = false;
                   h264flag = true;     
	       }else if(h264over && !accover)
	       {                    
                   unsigned char *aacdatabuff = aacframe+4;
		           WriteAAcTag(pFileMp4,aacdatabuff, aacdataSize - 11,aactimestramp);
                   //printf("h264 wanle aac xieru: %d\n",aacdataSize - 11);    
		           h264flag = false;
                   aacflag = true;	
	        }
      }      
    }
	
    fclose(fp264);
    delete[]buffer;
   
    fclose(fpaac);
    free(aacframe);

    //CloseMP4File(pFileMp4);
	
    return true;
}

void MP4Encoder::GetADTSInfo(unsigned char *aacheadbuff, int size, int &nAudioSampleRate, int &channelConfiguration,int &cnt)
{
	                    char profile_str[9]={0};
                            char frequence_str[10]={0};

                            unsigned char profile=aacheadbuff[2]&0xC0;
                            profile=profile>>6;
                            switch(profile){
                            case 0: sprintf(profile_str,"Main");break;
                            case 1: sprintf(profile_str,"LC");break;
                            case 2: sprintf(profile_str,"SSR");break;
                            default:sprintf(profile_str,"unknown");break;
                           }

                           unsigned char sampling_frequency_index=aacheadbuff[2]&0x3C;
                           sampling_frequency_index=sampling_frequency_index>>2;
                           switch(sampling_frequency_index){
                           case 0: sprintf(frequence_str,"96000Hz"); nAudioSampleRate = 96000; break;
                           case 1: sprintf(frequence_str,"88200Hz"); nAudioSampleRate = 88200;break;
                           case 2: sprintf(frequence_str,"88200Hz"); nAudioSampleRate = 88200;break;
                           case 3: sprintf(frequence_str,"88200Hz"); nAudioSampleRate = 88200;break;
                           case 4: sprintf(frequence_str,"44100Hz"); nAudioSampleRate = 44100;break;
                           case 5: sprintf(frequence_str,"32000Hz"); nAudioSampleRate = 32000;break;
                           case 6: sprintf(frequence_str,"24000Hz"); nAudioSampleRate = 24000; break;
                           case 7: sprintf(frequence_str,"22050Hz"); nAudioSampleRate = 22050; break;
                           case 8: sprintf(frequence_str,"16000Hz"); nAudioSampleRate = 16000; break;
                           case 9: sprintf(frequence_str,"12000Hz"); nAudioSampleRate = 12000; break;
                           case 10: sprintf(frequence_str,"11025Hz");nAudioSampleRate = 11025; break;
                           case 11: sprintf(frequence_str,"8000Hz");nAudioSampleRate = 8000; break;
                           default:sprintf(frequence_str,"unknown");nAudioSampleRate = 0; break;
                         }

                         // channelConfiguration  = 0;
                         channelConfiguration |= ((aacheadbuff[2] & 0x01) << 2);     //high 2 bit  ?2??,?16???
                         channelConfiguration |= (aacheadbuff[3] & 0xC0) >> 6;      //?6???0

                         int profile12 = 0;
                         profile12 |= ((aacheadbuff[2] & 0xC0) >> 6);

                         int sampling_frequency_index12 =0;
                         sampling_frequency_index12 |= ((aacheadbuff[2] & 0x3C) >> 2);
                         unsigned char audioConfig[2] = { 0 };
                         unsigned char audioObjectType = profile12 + 1;  ///?profile=1;
                         audioConfig[0] = (audioObjectType << 3) | (sampling_frequency_index12 >> 1);
                     
                         audioConfig[1] = (sampling_frequency_index12 << 7) | (channelConfiguration << 3);

                         SampleRate = nAudioSampleRate;
                         channel = channelConfiguration;

                         printf("%5d| %8s|  %8s| 0x%x | %5d|\n", cnt, profile_str, frequence_str, channelConfiguration, size);
}

int MP4Encoder::GetADTSInit(int nSampleRate,int nChannal,int bitsPerSample)
{
       m_nSampleRate = nSampleRate;
       m_nAudioChannal = nChannal;
       m_nBitsPerSample = bitsPerSample;
       m_nInputSamples = 0;
       m_nMaxOutputBytes = 0;


       // init faac
       m_hEncoder = faacEncOpen( nSampleRate, nChannal, &m_nInputSamples, &m_nMaxOutputBytes);
       m_nMaxInputBytes=m_nInputSamples*bitsPerSample/8;   

       m_pbPCMBuffer = new char[m_nMaxInputBytes];
       m_pOutAACBuffer = new char[m_nMaxOutputBytes];
       m_pTempBuffer = new char[BUFFER_SIZE];
       memset(m_pTempBuffer, 0 , BUFFER_SIZE);

      // Get current encoding configuration
      faacEncConfigurationPtr pConfiguration = faacEncGetCurrentConfiguration(m_hEncoder);
      if(!pConfiguration )
      {
         printf("GetCurrentConfiguration error!\n");
         return false;
      }

      //设置版本,录制MP4文件时要用MPEG4
      pConfiguration->version = MPEG4 ;
      pConfiguration->aacObjectType = LOW; //LC编码

      //输入数据类型
      pConfiguration->inputFormat = FAAC_INPUT_16BIT;

      // outputFormat (0 = Raw; 1 = ADTS)
      // 录制MP4文件时，要用raw流。检验编码是否正确时可设置为 adts传输流，
      pConfiguration->outputFormat= 0;


     //瞬时噪声定形(temporal noise shaping，TNS)滤波器
     pConfiguration->shortctl = SHORTCTL_NORMAL;
     pConfiguration->useTns=true;
     //pConfiguration->useLfe=false;
     pConfiguration->quantqual=100;
     pConfiguration->bandWidth=0;
     pConfiguration->bitRate=0;

    //Set encoding configuration
    faacEncSetConfiguration( m_hEncoder, pConfiguration);

    // 2.2 add audio track m_nSampleRate
    m_audioId = MP4AddAudioTrack(pFileMp4,80000,1024,MP4_MPEG4_AUDIO_TYPE);
    MP4SetAudioProfileLevel(pFileMp4, 0x02);

    //2.4 get decoder info
    unsigned char* faacDecoderInfo = NULL;    
    unsigned long  faacDecoderInfoSize = 0; 

    if(faacEncGetDecoderSpecificInfo(m_hEncoder, &faacDecoderInfo, &faacDecoderInfoSize)) 
    {        
       free(faacDecoderInfo);
       return false ;   
    }

   /*nsigned char faacDecoderInfo[2] = {0x12,0x10};
     faacDecoderInfoSize = 2 */
   //2.5 set encoder info [16bit-8000hz-1channal->{ 0x15, 0x88 } ]
    bool bOk = MP4SetTrackESConfiguration(pFileMp4,m_audioId,faacDecoderInfo,faacDecoderInfoSize);

    if(!bOk)
    {
      free(faacDecoderInfo);
      return false;   
    }
    free(faacDecoderInfo);
    return true;	
}
