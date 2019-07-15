#include"MP4Encoder.h"
#include<string.h>
#include<stdio.h>
#include <stdlib.h>

int BUFFER_SIZE = 10*1024*1024;
MP4Encoder::MP4Encoder(string &filepath, string &liveId)
{
     m_filepath = filepath;
     m_liveId = liveId;

     h264Tagbuffer = NULL;
     aacTagbuffer = NULL;
     config_file = NULL;

     MP4EncoderInit();

     testTag = 0;
}

void MP4Encoder::MP4EncoderInit()
{
    m_videoId = NULL;
    m_audioId = NULL;
    m_nWidth = 0;
    m_nHeight = 0;
    m_nTimeScale = 0;
    m_nFrameRate = 0;

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

    recordTime = 0;
}

int MP4Encoder::MergeFilesToMp4(string fileName)
{
     MP4EncoderInit();
     int retcode = 0;
     string mp4name = fileName + MP4STR;
     pFileMp4 = CreateMP4File(mp4name.c_str(),352,288);
     if(pFileMp4){
        LOG(ERROR) << "开始合成 liveId:"<<m_liveId<<"  fileName:"<<fileName;
        retcode = WriteAacH264file(pFileMp4, fileName.c_str());
     }else{
        printf(" CreateMP4File filed liveId:%s  fileName:%s\n", m_liveId.c_str() ,fileName.c_str());
        LOG(ERROR) << "CreateMP4File filed liveId"<<m_liveId<<"  fileName:"<<fileName;
     }

     CloseMP4File(pFileMp4);
     return retcode;
}

int MP4Encoder::MergeFilesToMp4(int recordtimes)
{
    int retcode = 1;
    int recordTimes = recordtimes;
		 
    string mp4name = m_filepath + m_liveId + "/"  + m_liveId + MP4STR;
    pFileMp4 = CreateMP4File(mp4name.c_str(),352,288);
    if(pFileMp4) {
        int retcode = 0;
        string fileName;
        for (int i = 0; i < recordTimes; i++) {
            if (i != 0) {
                string numberStr = "(";
                numberStr.append("+").append(std::to_string(i)).append(")");
                fileName = m_filepath + m_liveId + "/" + m_liveId + numberStr;

                /*tagSize = 0;
                firstH264Tag  = false;
                m_firstStamp = 0;

                m_pbPCMBuffer = NULL;
                m_pOutAACBuffer = NULL;
                m_pTempBuffer = NULL;
                firstAacTag = false;*/
                m_firstStamp = 0;
                m_VLastFrame.m_nTimeStamp = 0;
                m_ALastFrame.m_nTimeStamp = 0;
            } else {
                fileName = m_filepath + m_liveId + "/" + m_liveId;
            }

            printf("开始合成 fileName:%s\n", fileName.c_str());  
            LOG(ERROR) << "开始合成 liveId:"<<m_liveId<<"  fileName:"<<fileName;
            retcode = WriteAacH264file(pFileMp4, fileName.c_str());
            if (1 != retcode){
                //printf("合成失败 fileName:%s\n", fileName.c_str());
                LOG(ERROR) << "合成失败 liveId:"<<m_liveId<<"  fileName:"<<fileName;
            }else{
                LOG(ERROR) << "合成成功 liveId:"<<m_liveId<<"  fileName:"<<fileName;
                printf("merge succeed fileName:%s\n", fileName.c_str());
            }
        }
    }else
    {
        printf(" CreateMP4File filed liveId:%s\n", m_liveId.c_str());
        LOG(ERROR) << "CreateMP4File filed liveId:"<<m_liveId;
    }
    CloseMP4File(pFileMp4);
    return retcode;
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
      printf("ERROR:Open file fialed.\n");
      LOG(ERROR) << "CreateMP4File Openfile fialed liveId:"<<m_liveId;
      return false;
   }
   m_nWidth=width;
   m_nHeight=height;
   m_nTimeScale=90000;
   m_nFrameRate=30;
   MP4SetTimeScale(hMp4file,m_nTimeScale);
   return hMp4file;
}

int MP4Encoder::WriteH264Tag(MP4FileHandle hMp4File,const unsigned char* pData,int size, long timeStramp)
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
             LOG(ERROR) << "WriteH264Tag add video track failed liveId:"<<m_liveId;
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
	
     if(uDuration > 20000 || uDuration < 0)
     {
           //printf("h264 uDuration:%ld  lastTimeStramp:%ld  timeStramp :%ld :\n", uDuration, m_VLastFrame.m_nTimeStamp,timeStramp);
           LOG(WARNING) << " h264 uDuration:"<<uDuration<<"   lastTimeStramp: "<< m_VLastFrame.m_nTimeStamp<<"    timeStramp:"<<timeStramp<< 
                        " liveId:"<<m_liveId;
     }
    
     m_VLastFrame.m_nTimeStamp = timeStramp;

     int datalen=nalu.size+4;
     unsigned char *data=new unsigned char[datalen];
     data[0]=nalu.size>>24;
     data[1]=nalu.size>>16;
     data[2]=nalu.size>>8;
     data[3]=nalu.size&0xff;
     memcpy(data+4,nalu.data,nalu.size);

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


int MP4Encoder::WriteAAcTag(MP4FileHandle hMp4File,const unsigned char* pBufferG711,int nG711Len,long timeStramp)
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
         int uFrameMS = timeStramp - m_ALastFrame.m_nTimeStamp;
         uDuration  = (m_nSampleRate/1000)*uFrameMS;

         //}
         //int uFrameMS = timeStramp - m_ALastFrame.m_nTimeStamp;
         //uDuration  = (m_nSampleRate/1000)*uFrameMS;
		 
          if(uDuration < 0) //uDuration  == 0
	  {
		    uDuration = m_nSampleRate/1000;
	  }
     }

     m_ALastFrame.m_bKeyFrame = firstAacTag;

    if(uDuration > 20000 || uDuration < 0)
    {
          //printf("aac uDuration:%ld  lastTimeStramp:%ld  timeStramp :%ld :\n", uDuration, m_ALastFrame.m_nTimeStamp,timeStramp);
          LOG(WARNING) << " aac uDuration:"<<uDuration<<"   lastTimeStramp"<< m_VLastFrame.m_nTimeStamp<<"    timeStramp:"<<timeStramp<< 
                          " liveId:"<<m_liveId;
    }
    m_ALastFrame.m_nTimeStamp = timeStramp;
    
    if(!firstAacTag)
    {
         MP4WriteSample(hMp4File, m_audioId, pBufferG711, nG711Len,m_nSampleRate/1000 , 0, 1);
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
     printf("开始关闭mp4句柄\n");
     LOG(INFO) << "MP4Encoder::CloseMP4File 开始关闭mp4句柄 liveId:"<<m_liveId;

     MP4Close(pFileMp4, MP4_CLOSE_DO_NOT_COMPUTE_BITRATE);
     pFileMp4 = NULL;

     printf("已经关闭mp4句柄\n");
     LOG(INFO) << "MP4Encoder::CloseMP4File 已经关闭mp4句柄 liveId:"<<m_liveId;
   }
}

int MP4Encoder::WriteAacH264file(MP4FileHandle pFileMp4, const char* file)
{
      int resCode = 1;
      if(!h264Tagbuffer)
      {
          h264Tagbuffer=(unsigned char *)malloc(BUFFER_SIZE);
          if(!h264Tagbuffer)
          {
              printf("ERROR: malloc h264Tagbuffer failed!\n");
              LOG(ERROR) << "WriteAacH264file malloc h264Tagbuffer failed liveId:"<<m_liveId;
              resCode = 2;
              return resCode;
          }
      }

    if(!aacTagbuffer)
    {
        aacTagbuffer=(unsigned char *)malloc(BUFFER_SIZE);
        if(!aacTagbuffer)
        {
            printf("ERROR: malloc aacTagbuffer failed!\n");
            LOG(ERROR) << "WriteAacH264file malloc aacTagbuffer failed liveId:"<<m_liveId;
            resCode = 3;
            return resCode;
        }
    }

    string h264name = file;
    h264name = h264name.append(H264STR);
    FILE*fp264=fopen(h264name.c_str(),"rb");
    if(!fp264)
    {
        printf("ERROR: open h264 file failed!\n");
        LOG(ERROR) << "WriteAacH264file open h264 file failed liveId:"<<m_liveId;
        resCode = 4;
        fclose(fp264);
        return resCode;
    }
	  
    fseek(fp264, 0L, SEEK_END);
    int h264filelen = ftell(fp264);
    fseek(fp264, 0L, SEEK_SET);
    if(h264filelen == 0)
    {
        printf("ERROR: h264 file is empty\n");
        LOG(ERROR) << "WriteAacH264file h264 file is empty liveId:"<<m_liveId;
        resCode = 5;
        fclose(fp264);
        return resCode;
    }
    
    char h264headbuff[4] = {0};
    int H264TAGSIZE = 4;
    int h264DataSize = 0;
    int h264WriteTotal = 0;
    long h264timestramp = 0;
	  
    string aacname = file;
    aacname = aacname.append(AACSTR);
    FILE*fpaac=fopen(aacname.c_str(),"rb");
    if(!fpaac)
    {
        printf("ERROR: open aac file failed!\n");
        LOG(ERROR) << "WriteAacH264file open aac file liveId:"<<m_liveId;
        resCode = 6; 
        fclose(fp264);
        fclose(fpaac);

        return resCode;
    }
    fseek(fpaac, 0L, SEEK_END); 
    int aacfilelen = ftell(fpaac);
    if(aacfilelen == 0)
    {
           printf("ERROR: aac file is empty\n");
           LOG(ERROR) << "WriteAacH264file aac file is empty liveId:"<<m_liveId;
           resCode = 7;
           fclose(fp264);
           fclose(fpaac);

           return resCode;
    }
    fseek(fpaac, 0L, SEEK_SET);
    int aacWriteTotal = 0; 
    int ADTSHEADSIZE = 7;
    int aacdataSize = 0;
    int cnt = 0;
    long aactimestramp = 0;
    int nAudioSampleRate = 0;
    int channelConfiguration = 0; 
    unsigned char aacheadbuff[7] = {0};
    //unsigned char *aacframe=(unsigned char *)malloc(BUFFER_SIZE);
	  
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
                                  printf("h264文件 已经读不到H264TAGSIZE 数据长度\n");
                                  LOG(ERROR) << "h264文件 已经读不到H264TAGSIZE 数据长度,已读数据长度:"<<h264WriteTotal<<"  文件数据总长度:"<<h264filelen<<
                                           " 文件名:"<<h264name;
      
                                  h264WriteTotal = h264filelen;              
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
		   
		          memset(h264Tagbuffer,BUFFER_SIZE,0);
		          data_size = fread(h264Tagbuffer,1,h264DataSize,fp264);
		          if(data_size < h264DataSize)
		          {
		       
                                  printf("h264文件 已经读不到H264 一帧数据长度\n");
                                  LOG(ERROR)<< "h264文件 已经读不到H264一帧数据长度,已读数据长度:"<<h264WriteTotal<<"  文件数据总长度:"<<h264filelen<<
                                           " 文件名:"<<h264name;
                                  h264WriteTotal = h264filelen;
                                  h264over = true;
			          if(accover)
			          {
				         break;
			          }
			          continue;
		          }			
		          h264WriteTotal += h264DataSize;
                          //printf("%#X %#X %#X %#X\n",h264Tagbuffer[0], h264Tagbuffer[1], h264Tagbuffer[2], h264Tagbuffer[3]);
                          int extendsize = h264Tagbuffer[3];
                          h264timestramp = 0;
                          if(extendsize > 0)
                          {
                              char timestramp[4] = {0};
                              timestramp[0] = h264Tagbuffer[3];
                              memcpy(timestramp+1, h264Tagbuffer,3);

                              memcpy(&h264timestramp, timestramp,4);
                              h264timestramp = HTON32(h264timestramp);                             
                          }else
                          {
                              memcpy(&h264timestramp, h264Tagbuffer,3);
                              h264timestramp = HTON24(h264timestramp);
                          }
		         if(h264WriteTotal > h264filelen || h264WriteTotal == h264filelen)
                         {
                               h264over = true;
                               unsigned char *h264databuff = h264Tagbuffer+4;
                       
		               WriteH264Tag(pFileMp4,h264databuff,h264DataSize - 4,h264timestramp);
                               printf("zui hou yige h246 xieru: %d\n",h264DataSize - 4); 
                            
                               LOG(INFO) << "最后一个h264Tag写入,  时间戳:"<<h264timestramp<<"  fileName:"<<h264name;
                               LOG(INFO) << "已读数据长度："<<h264WriteTotal<<"  数据总长度："<<h264filelen;
                               
                         }  
		         //printf("h264 获取时间戳:%d  已读数据:%d 总数据:%d \n",h264timestramp,h264WriteTotal, h264filelen);  
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
                               printf("aac文件 已经读不到ADTSHEAD 数据长度\n");
                               LOG(ERROR)<< "aac文件 已经读不到ADTSHEAD 数据长度,已读数据长度:"<<aacWriteTotal<<"  文件数据总长度:"<<aacfilelen<<
                                           " 文件名:"<<aacname;
                               accover = true;
                               aacWriteTotal = aacfilelen;
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
	            memset(aacTagbuffer ,BUFFER_SIZE ,0);
                data_size = fread(aacTagbuffer,1,aacdataSize-7,fpaac);
                if(data_size < aacdataSize - 7)
                {
              
                     printf("aac文件 已经读不到aac 一帧长度\n");
                     LOG(ERROR)<< "aac文件 已经读不到aac 一帧长度,已读数据长度:"<<aacWriteTotal<<"  文件数据总长度:"<<aacfilelen<<
                                           " 文件名:"<<aacname;
                     accover = true;
                     aacWriteTotal = aacfilelen;
		     if(h264over)
	             {
			  break;
		     }
		     continue;
	         }
                     aacWriteTotal += data_size;
				
                //printf("%#X %#X %#X %#X\n",aacTagbuffer[0], aacTagbuffer[1], aacTagbuffer[2], aacTagbuffer[3]);
                int extendsize = aacTagbuffer[3];
                aactimestramp = 0;
                if(extendsize > 0)
                {
                       char timestramp[4] = {0};
                       timestramp[0] = aacTagbuffer[3];
                       memcpy(timestramp+1, aacTagbuffer,3);

                       memcpy(&aactimestramp, timestramp,4);
                       aactimestramp = HTON32(aactimestramp);
                             
                }else
                {
		               memcpy(&aactimestramp, aacTagbuffer, 3);
                       aactimestramp = HTON24(aactimestramp);
                }
				
                //aactimestramp =aactimestramp + totalAudioTimestamp;
                // printf("aac huoqudaode shijiancuo:%d %d %d\n",aactimestramp, aacWriteTotal, aacfilelen);
                if(aacWriteTotal > aacfilelen || aacWriteTotal == aacfilelen)
                {
                     accover = true;
                     unsigned char *aacdatabuff = aacTagbuffer+4;
		         
                     printf("zui hou yige aac xieru: %d\n",aacdataSize - 11); 
                     printf("aac huoqudaode shijiancuo:%d %d %d\n",aactimestramp, aacWriteTotal, aacfilelen);

		     WriteAAcTag(pFileMp4,aacdatabuff, aacdataSize - 11,aactimestramp);
                      
                     LOG(INFO) << "最后一个aacTag写入,  时间戳:"<<aactimestramp<<"  fileName:"<<aacname;
                     //LOG(INFO) << "已读数据长度："<<aacWriteTotal<<"  数据总长度："<<aacfilelen;
					 
		     /*if(h264over)
		     {
			break;
	             }*/                
                      
                }         
	      }else{
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
			       unsigned char *h264databuff = h264Tagbuffer+4;
		               WriteH264Tag(pFileMp4,h264databuff,h264DataSize - 4,h264timestramp);
                               //printf("h264 xieru: %d\n",h264DataSize - 4);                          
			       aacflag = false;
			       h264flag = true;
		    }else
		    {                   
                               unsigned char *aacdatabuff = aacTagbuffer+4;
			       WriteAAcTag(pFileMp4,aacdatabuff,aacdataSize - 11,aactimestramp);
                               //printf("aac xieru: %d\n",aacdataSize - 4); 
			       h264flag = false;
                               aacflag = true;
		    }
	   }else
	   { 
           if(accover && h264over)
           {
               printf("333 aac: %d , h246:%d", accover, h264over);
               LOG(INFO) << "h264文件，aac文件都已读完："<<h264name<<"  "<<aacname;
               break;
                 
           }else if(accover && !h264over)
	       {
		           unsigned char *h264databuff = h264Tagbuffer+4;
		           WriteH264Tag(pFileMp4,h264databuff,h264DataSize - 4, h264timestramp);
                           //printf("aac wanle h246 xieru: %d\n",h264DataSize - 4);
                           LOG(INFO) << "aac文件已读完: "<<aacname<<"  h264文件还需写入:"<<h264name<<"  "<<h264DataSize - 4;
		           aacflag = false;
                           h264flag = true;     
	       }else if(h264over && !accover)
	       {                    
                   unsigned char *aacdatabuff = aacTagbuffer+4;
		   WriteAAcTag(pFileMp4,aacdatabuff, aacdataSize - 11,aactimestramp);
                   printf("h264 wanle aac xieru: %d\n",aacdataSize - 11);

                   LOG(INFO) << "h264文件已读完: "<<h264name<<"  aac文件还需写入:"<<aacname<<"  "<<aacdataSize - 11;
		   h264flag = false;
                   aacflag = true;	
	       }
      }
    }

    printf("文件已经读完\n");
    LOG(INFO) << "h264文件已读完: "<<h264name<<"  aac文件也已经读完:"<<aacname;
	
    fclose(fp264);
    fclose(fpaac);
  
    return resCode;
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
       //m_nSampleRate = nSampleRate;
       m_nSampleRate  = 80000;
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
          LOG(ERROR) << "GetCurrentConfiguration error! liveId:"<<m_liveId;
          return false;
      }

      pConfiguration->version = MPEG4 ;
      pConfiguration->aacObjectType = LOW; //LC

      pConfiguration->inputFormat = FAAC_INPUT_16BIT;

      // outputFormat (0 = Raw; 1 = ADTS)
      pConfiguration->outputFormat= 0;

     pConfiguration->shortctl = SHORTCTL_NORMAL;
     pConfiguration->useTns=true;
     //pConfiguration->useLfe=false;
     pConfiguration->quantqual=100;
     pConfiguration->bandWidth=0;
     pConfiguration->bitRate=0;

    //Set encoding configuration
    faacEncSetConfiguration( m_hEncoder, pConfiguration);

    // 2.2 add audio track m_nSampleRate
    m_audioId = MP4AddAudioTrack(pFileMp4,m_nSampleRate,1024,MP4_MPEG4_AUDIO_TYPE);
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

int MP4Encoder::MergeFilesToMp4()
{
    int resCode = 0;
    string cfgfileStr =  m_filepath + m_liveId + "/"  + m_liveId + CFGSTR;
    config_file = new CConfigFileReader(cfgfileStr.c_str());
    string fileName;
    if(NULL != config_file)
    {
        recordTimes = std::stoi(config_file->GetConfigName(recordTimestr.c_str()));
        string mp4name = m_filepath + m_liveId + "/"  + m_liveId + MP4STR;
        pFileMp4 = CreateMP4File(mp4name.c_str(),352,288);
        int retcode = 0;
        for(int i =0 ; i< recordTimes; i++)
        {
            recordTime  =  i;
            if(i != 0)
            {
                string numberStr = "(";
                numberStr.append("+").append(std::to_string(i)).append(")");
                fileName = m_filepath + m_liveId + "/"  + m_liveId + numberStr;
                m_firstStamp = 0;
                m_VLastFrame.m_nTimeStamp = 0;
                m_ALastFrame.m_nTimeStamp = 0;
            }else
            {
                fileName = m_filepath + m_liveId + "/"  + m_liveId;
            }
            retcode = WriteAacH264file(pFileMp4, fileName.c_str());
        }
    }else
    {
        resCode = 1;
        return resCode;
    }
    CloseMP4File(pFileMp4);
    return resCode;
}

MP4Encoder::~MP4Encoder()
{
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

    if(NULL != config_file)
    {
        delete config_file;
        config_file = NULL;
    }
    if(NULL != aacTagbuffer)
    {
        free(aacTagbuffer);
        aacTagbuffer = NULL;
    }
    if(NULL !=  h264Tagbuffer)
    {
        free(h264Tagbuffer);
        h264Tagbuffer = NULL;
    }
}
