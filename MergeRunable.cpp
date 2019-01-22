#include "MergeRunable.h"

MergeRunable::MergeRunable(std::string path)
{
	 basePath =  path;
}

int MergeRunable::run()
{
  
  int rescode = readFileList();

  if(0 != rescode)
  {
      LOG(ERROR)<<"执行任务 liveID:"<<basePath<<" 失败"<< "  rescode:"<<rescode;

      return rescode;
  }

  LOG(INFO)<<"执行任务 liveID:"<<basePath<<"  合成成功"<< "  rescode:"<<rescode;

  rescode = UpdataRecordflag(rescode);
  return rescode;
}


 //更新录制状态
int MergeRunable::UpdataRecordflag(int flag)
{

    LibcurClient  m_httpclient;

    std::string resCodeInfo;
    std::string urlparm = "live_update?liveId=";
   
    urlparm.append(liveIDStr);
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

         if(0 != main_ret)
         {
             std::cout<<main_ret<<endl;

             LOG(ERROR)<<" 返回http接口失败!";
             return main_ret;
         }else
         {      
            resCodeInfo = m_object.value("msg", "oops");
          
            LOG(INFO)<<"执行任务 liveID:"<<basePath<<"  返回http接口信息  msg:"<<resCodeInfo;
            return main_ret;
         }
    }else
    {
        LOG(ERROR)<<"执行任务 liveID:"<<basePath<<" 返回http 接口数据不全!";
        main_ret = 1;
        return main_ret;
    }
}


int MergeRunable::readFileList()
{
	  DIR *dir;
    struct dirent *ptr;
    int rescode = 0;

    if((dir=opendir(basePath.c_str())) == NULL)
    {
        perror("Open dir error...");
        LOG(ERROR)<<"执行任务 liveID:"<<basePath<<"  Open dir error...";
        return -1;
    }

    string liveID = basePath.c_str();
    
    int found = liveID.find_last_of("/");
    string ss = liveID.substr(0,found);
    liveIDStr = liveID.substr(found+1);
    int fileNameSize = liveIDStr.size();
   
    while ((ptr=readdir(dir)) != NULL)
    {       
       if(strcmp(ptr->d_name,".")== 0 || strcmp(ptr->d_name,"..")==0)  //current dir OR parrent dir
       {

            continue;
       }else if(ptr->d_type == 8)  //file
       {
           string fileName = ptr->d_name;

           if(string::npos != fileName.find(MERGESTR))
           {
           	   continue;
           }

           if(string::npos != fileName.find(AACSTR))
           { 	   

               string format = fileName;
               format = format.erase(0,fileNameSize);   
               if(AACSTR == format)
               {
               	   cout<<0<<"  "<<fileName<<endl;
               	   m_AacMap.insert(std::pair<int,string>(0,fileName));

               }else
               {
                  int found = fileName.find_first_of("(");
                  string foundstr = fileName.substr(0,found);
                  if(foundstr != liveIDStr)
                  {
                      printf("filename is not matching folder!\n");
                      LOG(ERROR)<<"执行任务 liveID:"<<basePath<<"  filename is not matching folder";
                      continue;
                  }
                  format = format.erase(0,2);
                  std::size_t pos = format.find(AACSTR);
                  int size = AACSTR.size();
                  format = format.erase(pos,size);  

                  format.pop_back();
                  int number = atoi(format.c_str() ); 
                  cout<<number<<"  "<<fileName<<endl;
                  m_AacMap.insert(std::pair<int,string>(number,fileName));

               }

           }else if(string::npos != fileName.find(H264STR))
           {
           	    string format = fileName;
                format = format.erase(0,fileNameSize);   
                if(H264STR == format)
                {
               	   m_H264Map.insert(std::pair<int,string>(0,fileName));

                }else
                {

                   int found = fileName.find_first_of("(");
                   string foundstr = fileName.substr(0,found);
                   if(foundstr != liveIDStr)
                   {
                       printf("filename is not matching folder!\n");
                       LOG(ERROR)<<"执行任务 liveID:"<<basePath<<"  filename is not matching folder";
                       continue;
                   }
                  format = format.erase(0,2);
                  std::size_t pos = format.find(H264STR);
                  int size = H264STR.size();
                  format = format.erase(pos,size);  

                  format.pop_back();          
                  int number = atoi(format.c_str() ); 
                  cout<<number<<"  "<<fileName<<endl;
                  m_H264Map.insert(std::pair<int,string>(number,fileName));
               }
               

           }else if(string::npos != fileName.find(JSONSTR))
           {
           	   string format = fileName;
               format = format.erase(0,fileNameSize);   
               if(JSONSTR == format)
               {
               	   cout<<0<<"  "<<fileName<<endl;
               	   m_JsonMap.insert(std::pair<int,string>(0,fileName));

               }else
               {
                   int found = fileName.find_first_of("(");
                   string foundstr = fileName.substr(0,found);
                   if(foundstr != liveIDStr)
                   {
                     printf("filename is not matching folder!\n");
                     LOG(ERROR)<<"执行任务 liveID:"<<basePath<<"  filename is not matching folder";
                     continue;
                   }
                  format = format.erase(0,2);
                  std::size_t pos = format.find(JSONSTR);
                  int size = JSONSTR.size();
                  format = format.erase(pos,size);  

                  format.pop_back(); 
                  int number = atoi(format.c_str() ); 
                  cout<<number<<"  "<<fileName<<endl;
                  m_JsonMap.insert(std::pair<int,string>(number,fileName));
               }
           }
       }else if(ptr->d_type == 10)//link file
       {
          continue;
       }
       else if(ptr->d_type == 4) //dir
       {
     	   continue;
         
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

int MergeRunable::mergeFile(std::map<int, string> &Map, std::string filetype)
{

    pthread_t tid = pthread_self();
    printf("[tid: %lu]\trun \n: ", tid);
 	  string basePathStr = basePath.c_str();
  
    std::map<int ,string>::iterator it;
    FILE  *pFTmp = NULL;
    FILE  *pFSrc = NULL;

    //获取路径中的liveID
    // string path = basePath.c_str();
    // int found = path.find_last_of("/");
    // string liveIDStr = path.substr(found+1);
    
    std::string mergeFilename =basePathStr + "/"+ MERGESTR + liveIDStr + filetype;
    cout<<"merge Filename:"<<mergeFilename<<endl;
    LOG(INFO)<<"执行任务 liveID:"<<basePath<<"   mergeFilename:"<<mergeFilename;
    if(NULL == (pFTmp = fopen(mergeFilename.c_str(), "ab+")))
    {
        printf("MergeFile:open %s failed!\n", mergeFilename.c_str());
        LOG(ERROR)<<"执行任务 liveID:"<<basePath<<"  "<<mergeFilename<<"open failed ";
        return -1;
    }
    it = Map.begin();
    for(;it!= Map.end();it++)
    {

        cout<< it->first<<"  "<<"file name:" <<it->second<<endl; 
        LOG(INFO)<<"执行任务 liveID:"<<basePath<<"   filename:"<<it->second;

        string srcFileStr = basePathStr + "/"+ (it->second);
        const char *srcFile = srcFileStr.c_str();

        int c = 0;  

        if (NULL == (pFSrc = fopen(srcFile, "r")))
        {
            printf("MergeFile:open %s failed!\n", srcFile);
            LOG(ERROR)<<"执行任务 liveID:"<<basePath<<"  MergeFile open "<<srcFile<<" failed ";
            break;
        }

        while((c = getc(pFSrc)) != EOF)  
        {  
           putc(c,pFTmp);  
        } 

        fclose(pFSrc);

        //hecheng hou shanchu benwenjian
        //remove(srcFileStr.c_str()); 
    
    }
    fclose(pFTmp);
    pFTmp = NULL;
    Map.clear();

    return 0;
}

MergeRunable::~MergeRunable()
{
	
}
