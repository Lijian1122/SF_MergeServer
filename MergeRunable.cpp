#include "MergeRunable.h"

MergeRunable::MergeRunable(std::string liveID)
{
     m_liveId = liveID;
     m_liveIdSize = 0;

     config_file = NULL;
     recordTimes = 0;

     m_AacMap.clear();
     m_H264Map.clear();
     m_JsonMap.clear();
}

/*int MergeRunable::run()
{
    int rescode = 0;
    m_mp4encoder = new MP4Encoder(MergeFilePath,m_liveId);
    if(m_mp4encoder) {
        rescode = searchFile();
        recordTimes = m_H264Map.size();

        if (recordTimes > 0) {
            rescode = m_mp4encoder->MergeFilesToMp4(recordTimes);
            if (1 != rescode) {
                LOG(ERROR) << "执行任务 liveID:" << m_liveId << " 失败" << "  rescode:" << rescode;
            } else {
                LOG(INFO) << "执行任务 liveID:" << m_liveId << "  合成成功" << "  rescode:" << rescode;
                printf("执行任务 liveID:%s  合成成功 rescode:%d\n", m_liveId.c_str(), rescode);
            }
        } else {
             rescode = 8;  //无文件 设置状态为8
             LOG(ERROR) << "执行任务 liveID:" << m_liveId << "  录制文件为空!!!";
        }
    }else
    {
        rescode = 9;
        LOG(ERROR)<<"执行任务 liveID:"<<m_liveId<<" 失败"<< "  rescode:"<<rescode<<"实例化合成对象失败！";
    }

    //更新合成状态 ,1为合成成功，其他均为异常
    rescode = UpdataMergeflag(rescode);
    return rescode;
}*/

int MergeRunable::run()
{
    int rescode = 0;
    m_mp4encoder = new MP4Encoder(MergeFilePath,m_liveId);
    if(m_mp4encoder)
    {
          rescode =  searchFile();
          string fileName;
          recordTimes = m_H264Map.size();

          if(recordTimes > 0)
          {
              for (int i = 0; i < recordTimes; i++) {
                  if (i != 0) 
                  {
                      string numberStr = "(";
                      numberStr.append("+").append(std::to_string(i)).append(")");
                      fileName = MergeFilePath + m_liveId + "/" + m_liveId + numberStr;
                  } else {
                      fileName = MergeFilePath + m_liveId + "/" + m_liveId;
                  }
                  rescode = m_mp4encoder->MergeFilesToMp4(fileName);

                  if (1 != rescode) {
                      LOG(ERROR) << "执行任务 liveID:" << m_liveId << " 失败" << "  rescode:" << rescode << "   fileName:"
                                 << fileName;
                  } else {

                      LOG(INFO) << "执行任务 liveID:" << m_liveId << "  合成成功" << "  rescode:" << rescode << "   fileName:"
                                 << fileName;
                      printf("执行任务 liveID:%s  合成成功  rescode:%d  fileName:%s\n", m_liveId.c_str(), rescode, fileName.c_str());
                      rescode = 1;
                  }
              }
          } else 
          {
                rescode = 8;  //无文件设置状态为 8
                LOG(ERROR)<<"执行任务 liveID:"<<m_liveId<<"  录制文件为空!!!";
          }

          //rescode = UpdataMergeflag(rescode);
    }else
    {
         rescode = 9;
         LOG(ERROR)<<"执行任务 liveID:"<<m_liveId<<" 失败"<< "  rescode:"<<rescode<<"实例化合成对象失败！";
    }

    //更新合成状态 ,1为合成成功，其他均为异常
    LOG(ERROR)<<"rescode :"<<rescode;
    rescode = UpdataMergeflag(rescode);
    return rescode;
}

//更新合成状态
int MergeRunable::UpdataMergeflag(int flag)
{
    LibcurClient  m_httpclient;
    std::string resCodeInfo;

   
    std::string urlparm = IpPort + liveUpdate + "?liveId=";
   
    urlparm.append( m_liveId);
    urlparm.append("&mixFlag=");
          
    char flagStr[10] ={};
    snprintf(flagStr, sizeof(flagStr), "%d",flag);
    urlparm.append(flagStr);
    urlparm.append("&operateId=test");
  
    std::string updataUrl = urlparm;
    LOG(INFO)<<"UpdataMergeUrl:"<<updataUrl;

    int m_ret = m_httpclient.HttpGetData(updataUrl.c_str());
    std::string resData = m_httpclient.GetResdata();
    m_ret = ParseJsonInfo(resData, resCodeInfo);

    return m_ret;
}

//解析http返回json
int MergeRunable::ParseJsonInfo(std::string &jsonStr ,std::string &resCodeInfo)
{
    int main_ret = 0;
    std::cout<<"parse json:"<<jsonStr<<endl;
    if(!jsonStr.empty())
    {
        json m_object = json::parse(jsonStr);
        if(m_object.is_object())
        {
            string resCode = m_object.value("code", "oops");
            main_ret = atoi(resCode.c_str() );

            if(0 == main_ret)
            {
                LOG(INFO)<<"执行任务 liveID:"<<m_liveId <<"  上传合成状态成功";          
            }else
            {      
                resCodeInfo = m_object.value("msg", "oops");          
                LOG(ERROR)<<"执行任务 liveID:"<<m_liveId<<"  上传合成状态失败  msg:"<<resCodeInfo;
            }
       }else
       {
          LOG(ERROR)<<"执行任务 liveID:"<<m_liveId<<" 返回http 接口数据不全!";
          main_ret = 1;   
        }
     }else
     {
         LOG(ERROR)<<"执行任务 liveID:"<<m_liveId<<" 返回http 接口数据为空!";
         main_ret = 2;
     }
     return main_ret;
}

int MergeRunable::searchFile()
{
    DIR *dir;
    struct dirent *ptr;
    int rescode = 0;

    //获取文件路径
    m_liveIdSize = m_liveId.size();
    std::string m_path =  MergeFilePath + m_liveId;
    
    if((dir=opendir(m_path.c_str())) == NULL)
    {
        LOG(ERROR)<<"执行任务 liveID:"<<m_liveId<<"  打开文件路径失败";
        return -1;
    }   
   
    while((ptr=readdir(dir)) != NULL)
    {       
        if(ptr->d_type == 8) //类型为文件
        {
           string fileName = ptr->d_name;
		   
	       if(string::npos != fileName.find(AACSTR))
           {
                  setFileNameToMap(m_AacMap,fileName,AACSTR);
           }else if(string::npos != fileName.find(H264STR))
           {
                  setFileNameToMap(m_H264Map , fileName,H264STR);
           }else if(string::npos != fileName.find(JSONSTR))
           {
                  setFileNameToMap(m_JsonMap,fileName,JSONSTR);
           }     
       } 
    } 

    closedir(dir);
    /*if(0 != m_AacMap.size())
    {
       rescode = mergeFile(m_AacMap,AACSTR);
    }
    if(0 != m_H264Map.size())
    {
       rescode = mergeFile(m_H264Map,H264STR);
    }
    if(0!= m_JsonMap.size())
    {
       rescode = mergeFile(m_JsonMap,JSONSTR);
    }*/
  
    return rescode;
}

int MergeRunable::setFileNameToMap(std::map<int, string> &Map , std::string &fileName , std::string &fileType)
{

   string format = fileName;
   format = format.erase(0,m_liveIdSize);
   if(fileType == format)
   {
     cout<<0<<"  "<<fileName<<endl;
     Map.insert(std::pair<int,string>(0,fileName));
     return 0;
   }else
   {
     int found = fileName.find_first_of("(");
     string foundstr = fileName.substr(0,found);
     if(foundstr != m_liveId)
     {
         LOG(ERROR)<<"执行任务 liveID:"<<fileName<<"  filename is not matching folder";
         return 1;
     }
   
     //找到文件序号，按照录制的时间先后插入map
     format = format.erase(0,2);
     std::size_t pos = format.find(fileType);
     int size = fileType.size();
     format = format.erase(pos,size);

     format.pop_back();
     int number = atoi(format.c_str() );
     cout<<number<<"  "<<fileName<<endl;
     Map.insert(std::pair<int,string>(number,fileName));
   }
   return 0;
}

int MergeRunable::mergeFile(std::map<int, string> &Map, std::string filetype)
{
    pthread_t tid = pthread_self();
    printf("[tid: %lu]\trun \n: ", tid);
   
    std::map<int ,string>::iterator it;
    FILE  *pFTmp = NULL;
    FILE  *pFSrc = NULL;
	
	//当前文件路径
    std::string m_path = RELATIVEPATH;
	m_path.append(m_liveId);
 
    std::string mergeFilename =m_path + "/"+ MERGESTR + m_liveId + filetype;
	
    cout<<"merge Filename:"<<mergeFilename<<endl;
    LOG(INFO)<<"执行任务 liveID:"<<m_liveId<<"   mergeFilename:"<<mergeFilename;
    if(NULL == (pFTmp = fopen(mergeFilename.c_str(), "ab+")))
    {
        printf("MergeFile:open %s failed!\n", mergeFilename.c_str());
        LOG(ERROR)<<"执行任务 liveID:"<<m_liveId<<"  "<<mergeFilename<<"open failed ";
        return -1;
    }
    it = Map.begin();
    for(;it!= Map.end();it++)
    {
        cout<< it->first<<"  "<<"file name:" <<it->second<<endl;
        LOG(INFO)<<"执行任务  filename:"<<it->second;

        string srcFileStr = m_path + "/"+ (it->second);
        const char *srcFile = srcFileStr.c_str();

        int iLen = 0;
        if(NULL == (pFSrc = fopen(srcFile, "r")))
        {
            printf("MergeFile:open %s failed!\n", srcFile);
            LOG(ERROR)<<"执行任务  MergeFile open "<<srcFile<<" failed ";
            break;
        }

        fseek(pFSrc,0L,SEEK_END); /* 定位到文件末尾 */
        iLen=ftell(pFSrc); /*得到文件大小*/

        int time = iLen/1024;
        int end = iLen%1024;
        char buff[1024] = {0};
        fseek(pFSrc,0L,SEEK_SET); /*定位到文件开头*/
        if(time > 0)
        {
          int i = 0;
          for(;i < time;++i)
          {
              //usleep(3);
              fread(buff,1024,1,pFSrc);
              fwrite(buff,1024,1,pFTmp);
              fseek(pFSrc,1024*(i+1),0);
              memset(buff,0 ,1024);
          }
        }
        fread(buff,end,1,pFSrc);
        fwrite(buff,end,1,pFTmp);
        fclose(pFSrc);
    }
    fclose(pFTmp);
    pFTmp = NULL;
    Map.clear();
    return 0;
}
MergeRunable::~MergeRunable()
{
    if(m_mp4encoder)
    {
        delete  m_mp4encoder;
        m_mp4encoder = NULL;
    }
    if(config_file)
    {
        delete config_file;
        config_file = NULL;
    }

    m_AacMap.clear();
    m_H264Map.clear();
    m_JsonMap.clear();
}
