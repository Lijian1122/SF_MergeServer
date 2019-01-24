#include "MergeRunable.h"

MergeRunable::MergeRunable(std::string liveID)
{
	 m_liveId = liveID;
         m_liveIdSize = 0;
}
int MergeRunable::run()
{
    int rescode = searchFile();
    if(0 != rescode)
    {
       LOG(ERROR)<<"执行任务 liveID:"<<m_liveId<<" 失败"<< "  rescode:"<<rescode;

       return rescode;
    }
    LOG(INFO)<<"执行任务 liveID:"<<m_liveId<<"  合成成功"<< "  rescode:"<<rescode;

    rescode = UpdataMergeflag(rescode);
    return rescode;
}

int MergeRunable::UpdataMergeflag(int flag)
{
    LibcurClient  m_httpclient;
    std::string resCodeInfo;
    std::string urlparm = "live_update?liveId=";
   
    urlparm.append( m_liveId);
    urlparm.append("&mixFlag=");
          
    char flagStr[10] ={};
    snprintf(flagStr, sizeof(flagStr), "%d",flag);
    urlparm.append(flagStr);
    urlparm.append("&operateId=8888");
  
    std::string updataUrl = IPPORT + urlparm;
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
	return main_ret;
}

int MergeRunable::searchFile()
{
	DIR *dir;
    struct dirent *ptr;
    int rescode = 0;

    //获取文件路径
     m_liveIdSize = m_liveId.size();
    std::string m_path = RELATIVEPATH;
    m_path.append(m_liveId);
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
		   
           if(string::npos == fileName.find(MERGESTR))
		   {
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
    } 

    closedir(dir);
    if(0 != m_AacMap.size())
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
    }
  
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
	
}
