#include "webserver.h"

//Http处理请求
void ev_handler(struct mg_connection *nc, int ev, void *ev_data) 
{
      switch (ev) 
      {
         case MG_EV_ACCEPT: 
         {
            char addr[32];
            mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                          MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
            break;
         }
         case MG_EV_HTTP_REQUEST: 
         {
             struct http_message *hm = (struct http_message *) ev_data;
             char addr[32];
             mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                          MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
             
             char url[(int)hm->uri.len];
             sprintf(url, "%.*s",(int)hm->uri.len,hm->uri.p);
             LOG(INFO) << "Get Url:"<<url;
   
             RESCODE ret = RESCODE::NO_ERROR;
             if(strcmp(url, APIStr.c_str()) == 0)
             {				 
		        int parmlen = (int)hm->query_string.len;
                        char parmStr[parmlen];
                        sprintf(parmStr, "%.*s",parmlen,hm->query_string.p);		 
		        printf("url参数长度：%d  %s\n",parmlen ,parmStr);
              
                		
	        char *liveId_buf = (char*)malloc(sizeof(char)*parmlen);
		if(NULL == liveId_buf)
                {       
                   LOG(ERROR) << "参数liveId malloc失败:"<<liveId_buf;
                   ret = RESCODE::MALLOC_ERROR;
                   goto end;            
                }
                memset(liveId_buf, 0 ,sizeof(char)*parmlen);
                mg_get_http_var(&hm->query_string, "liveId", liveId_buf, parmlen);//获取liveID
				
                if(0 == strlen(liveId_buf))
                {         
                   LOG(ERROR)<<"LIVEID EMPTY";
                   ret = RESCODE::LIVEID_ERROR;
                   goto end;
                }
                		
	        LOG(INFO)<<"解析参数 livID:"<<liveId_buf;		
                MergeParmList->pushLockList((void*)liveId_buf);				
           }else
	   {
	        LOG(ERROR)<<"Methond ERROR";
                ret = RESCODE::METHOD_ERROR;
           }
  end:
           char numStr[10] ={};
           snprintf(numStr, sizeof(numStr), "%d",ret);
     
           //json返回值
           json obj;
           obj["code"] = numStr;
           std::string resStr = obj.dump();

           mg_send_response_line(nc, 200, "Content-Type: application/json;charset=utf-8\r\n");
           mg_printf(nc,"\r\n%s\r\n",resStr.c_str());
    
           nc->flags |= MG_F_SEND_AND_CLOSE;
           break;
      }    
      case MG_EV_CLOSE:
      {      
         break;
      }
  }
}

//处理合成参数 线程
void *mergeManage_fun(void *data)
{
    void *parmdata = NULL;
    while(merge_flag)
    {             
       //录制参数出 参数队列
       parmdata = MergeParmList->popLockList(); 
       if(NULL != parmdata)
       {
	       char *mergeparm = (char*)parmdata;
		   
               LOG(INFO) << "管理线程获取参数 直播ID:"<<mergeparm;
	       //新建任务实例，并加入任务队列
               MergeRunable *taskObj = new MergeRunable(mergeparm);
               threadpool->AddTask(taskObj);
               if(NULL != mergeparm)
               { 
                  free(mergeparm);
                  mergeparm= NULL;
               }       
      }
   }  
   return data;
}

//解析Http返回json数据
int parseResdata(string &resdata,  int ret ,PARSE_TYPE m_Type)
{     
    if(!resdata.empty())
    {   
        json m_object = json::parse(resdata);
        if(m_object.is_object())
        {
            string resCode = m_object.value("code", "oops");
            ret = atoi(resCode.c_str() );
            if(0 == ret)
            {	  
		        switch(m_Type) 
                {
		           case PARSE_TYPE::GETAPI:  //解析获取API接口
	               {
			            json data_object = m_object.at("data");
						
		                string IP = data_object.value("ip", "oops");
                        string port = data_object.value("port", "oops");
                        IpPort = IpPort.append(IP).append(":").append(port);

                        ServerCreate = data_object.value("server_create", "oops");
                        ServerDelete = data_object.value("server_delete", "oops");
                        ServerSelect = data_object.value("server_select", "oops");
                        ServerUpdate = data_object.value("server_update", "oops");

                        liveUpdate = data_object.value("live_update", "oops");
                        liveSelect = data_object.value("live_select", "oops");
                        liveUpload = data_object.value("live_upload", "oops");

                        cout<<IpPort  <<ServerCreate  <<ServerDelete  <<ServerSelect  <<liveUpdate <<endl;	 
			            break;
	               }
			       case PARSE_TYPE::REGISTONLINE:  //解析注册合成服务
	               {                        
				       merge_serverId = m_object.value("serverId", "oops");
                       LOG(INFO)<<"录制服务 merge_serverId:"<<merge_serverId;	
                       printf("serverID: %s\n", merge_serverId.c_str());	
			           break;
	               }
			       case PARSE_TYPE::UPDATA:  //解析定时上传合成在线
	               {
					   break;
	               }	    
              }			  
         }else
         {
			 LOG(ERROR)<<"接口返回数据异常 ret:"<< ret<<"  错误信息:"<<m_object.value("msg", "oops");
         }  
	  }else
      {
		  ret = -1;
          LOG(ERROR)<<" 接口返回数据不完整";
	  }		   
   }else
   {
	   ret = -2;
	   LOG(ERROR) << "Http接口返回 数据为空";   
   }
   
   return ret;
}

//Http服务监听 线程
void *httpServer_fun(void *pdata)
{
   struct mg_mgr mgr;
   struct mg_connection *nc;

   mg_mgr_init(&mgr, NULL);
   nc = mg_bind(&mgr, ServerPort.c_str(), ev_handler);
   if (nc == NULL) 
   {
      printf("Failed to create listener!\n");
      LOG(ERROR)<<"Failed to create listener";
      httpSev_flag = 0;
      pthread_detach(pthread_self());
      return 0;
   }

   printf("合成服务已启动 on port: %s\n" ,ServerPort.c_str()); 
   LOG(INFO) <<"合成服务已启动 on port:"<<ServerPort;

   mg_set_protocol_http_websocket(nc);
   while(httpSev_flag)
   {    
      mg_mgr_poll(&mgr, 1000);
   }
   return pdata;
}

//创建日志文件夹
int CreateLogFileDir(const char *sPathName)  
{  
      char DirName[256];  
      strcpy(DirName, sPathName);  
      int i,len = strlen(DirName);
      for(i=1; i<len; i++)  
      {  
          if(DirName[i]=='/')  
          {  
              DirName[i] = 0; 
              if(access(DirName, F_OK)!=0)  
              {  
                   
                  if(mkdir(DirName, 0755)==-1)  
                  {   
                      printf("mkdir log error\n");  
                      LOG(ERROR)<<"mkdir log error"; 
                      return -1;   
                  }             
              }  
              DirName[i] = '/';  
          }  
      }  
      return 0;  
} 

//定时上传状态 线程
void *httpTime_fun(void *pdata)
{
   s_httpclient = new LibcurClient;

   /*updateOnlineUrl = IpPort;
   updateOnlineUrl.append("server_update?serverId=");
   updateOnlineUrl.append(merge_serverId);
   updateOnlineUrl.append("&netFlag=20");
   cout<<"updata url:"<<updateOnlineUrl<<endl;*/

   updateOnlineUrl = IpPort;
   updateOnlineUrl.append(ServerUpdate);
   updateOnlineUrl.append("?serverId=");

   updateOnlineUrl.append(merge_serverId);
   updateOnlineUrl.append("&netFlag=1");
   cout<<"time url:"<<updateOnlineUrl<<endl;

   //录制服务在线状态定时上传
   while(merge_flag)
   {
      sleep(10);
	  
      int main_ret = s_httpclient->HttpGetData(updateOnlineUrl.c_str());
      if(0 == main_ret)
      {
		std::string resData = s_httpclient->GetResdata();
        main_ret = parseResdata(resData, main_ret ,PARSE_TYPE::UPDATA);
		if(0 != main_ret)
		{
		  LOG(ERROR) << "解析定时返回数据失败  main_ret:"<<main_ret; 
		}
     }else
     {
        //LOG(ERROR) << "调用定时上在线状态接口失败  main_ret:"<<main_ret;  
     }	
   }

   if(NULL != s_httpclient)
   {
      delete s_httpclient;
      s_httpclient = NULL;
   }
   return pdata;
}

//停止服务
int stopServer()
{
	int main_ret = pthread_join(httpServer_t, NULL);
    if(0 != main_ret)
    {
      printf("http服务线程退出错误  ret:%d" ,main_ret); 
      LOG(ERROR)<<"http服务线程退出错误  ret:"<<main_ret; 
    } 
	
	if(NULL != MergeParmList)
	{
		delete MergeParmList;
		MergeParmList = NULL;
	}
    return main_ret;
}

//启动服务
int startServer()
{     
    int main_ret = 0;
    string url = "";

    m_httpclient = new LibcurClient;

    //读取配置文件
    ServerPort = config_file.GetConfigName("ServerPort");
    RELATIVEPATH  = config_file.GetConfigName("Filefolder");
    IpPort = config_file.GetConfigName("IpPort");
    APIStr = config_file.GetConfigName("APIStr");
    HttpAPIStr = config_file.GetConfigName("HttpAPIStr");
    LOGDIR  = config_file.GetConfigName("Logfolder");
    ServerName = config_file.GetConfigName("ServerName");
    ServerNameAPIStr = config_file.GetConfigName("ServerNameAPI");
    ServerCreateStr = config_file.GetConfigName("ServerCreateAPI");  
    string threadCountStr = config_file.GetConfigName("ThreadCount");
    threadCount = std::stoi(threadCountStr);

    MergeFilePath= config_file.GetConfigName("MergeFilePath");

    main_ret = CreateLogFileDir(LOGDIR.c_str());
    if(0 != main_ret)
    {
       LOG(ERROR) << "创建log 文件夹失败"<<" "<<"main_ret:"<<main_ret;
       return main_ret ;
    }

    //创建log初始化
    google::InitGoogleLogging("");
    std::string fileName = LOGDIR.append("mergeServer-");
    google::SetLogDestination(google::GLOG_INFO,fileName.c_str());
    //google::SetLogDestination(google::GLOG_INFO,"./mergelog/mergeServer-");
    FLAGS_logbufsecs = 0; //缓冲日志输出，默认为30秒，此处改为立即输出
    FLAGS_max_log_size = 500; //最大日志大小为 100MB
    FLAGS_stop_logging_if_full_disk = true; //当磁盘被写满时，停止日志输出

    //获取Http API
    main_ret = m_httpclient->HttpGetData(HttpAPIStr.c_str());
    if(0 == main_ret)
    {
	   std::string resData = m_httpclient->GetResdata();
	   main_ret = parseResdata(resData, main_ret ,PARSE_TYPE::GETAPI);
	   if(0 != main_ret)
	   {
		  LOG(ERROR) << "解析Http API接口 数据失败  main_ret:"<<main_ret; 
		  return main_ret;
	   }
    }else
    {
	   LOG(ERROR) << "调用Http API接口失败   main_ret:"<<main_ret;  
           return main_ret;
    }

    //注册服务接口  
    /*url = IpPort;
    url.append("server_create?serverType=2&serverName=");
    url.append(ServerCreate);
    url.append("?serverName=");
    char *format = m_httpclient->UrlEncode(serverName);
    url.append(format);
    url.append("&serverType=LiveRecord&serverApi=192.168.1.206:");
    url.append(s_http_port);    
    url.append(APIStr);*/
    

   url = IpPort;
   url.append(ServerCreate);
   url.append(ServerNameAPIStr);
   char *format = m_httpclient->UrlEncode(ServerName);
   url.append(format);
   url.append(ServerCreateStr);
   url.append(ServerPort);    
   url.append(APIStr);

   printf("注册服务： %s\n", url.c_str());
   main_ret = m_httpclient->HttpGetData(url.c_str());
   if(0 == main_ret)
   {
	   std::string resData = m_httpclient->GetResdata();
	   main_ret = parseResdata(resData, main_ret ,PARSE_TYPE::REGISTONLINE);
	   if(0 != main_ret)
	   {
		  LOG(ERROR) << "解析注册录制 数据失败 main_ret:"<<main_ret; 
		  //return main_ret;
	   }	  
    }else
    {  
       LOG(ERROR) << "调用注册录制服务 失败  main_ret:"<<main_ret;  
       //return main_ret;
    }
  
    //参数队列初始化
    MergeParmList = new CommonList(true);
    if(0 != MergeParmList->getRescode())
    {
       LOG(ERROR) << "初始化参数队列失败  main_ret:"<<MergeParmList->getRescode();
       return main_ret;
    } 
  
 
    //创建http服务线程  
    main_ret = pthread_create(&httpServer_t, NULL, httpServer_fun, NULL);
    if(0 != main_ret)
    { 
      printf("http服务监听线程创建失败 ret:%d" ,main_ret); 
      LOG(ERROR) << "http服务监听线程创建失败"<<" "<<"main_ret:"<<main_ret;
      return main_ret;   
    }
	
	//创建合成任务管理线程
    main_ret = pthread_create(&mergeManage_t, NULL, mergeManage_fun, NULL);
    if(0 != main_ret)
    { 
       LOG(ERROR) << "录制管理线程创建失败"<<" "<<"main_ret:"<<main_ret; 
       return main_ret; 
    }

    //线程池初始化
    threadpool = new CThreadPool(threadCount); //线程池大小为10

    //创建定时上传线程
    main_ret = pthread_create(&httpTime_t,NULL, httpTime_fun, NULL);
    if(0 != main_ret)
    {
       LOG(ERROR) << "http定时上传线程创建失败"<<" "<<"main_ret:"<<main_ret; 
       return main_ret;   
    }
	return main_ret;
}

int main(int argc, char* argv[]) 
{
    printf("monitor_Server :[tid: %ld]\n", syscall(SYS_gettid));

    int main_ret = startServer();

    if(0 != main_ret)
    {
       LOG(INFO) << "server start error:  "<<"main_ret:"<<main_ret;
       return main_ret;
    }

    printf("server starting\n");
    LOG(INFO) << "server start: "<<"main_ret:"<<main_ret;
   
    main_ret = stopServer();

    printf("合成服务已退出 ret:%d\n" ,main_ret);
    LOG(INFO) << "合成服务已退出"<<" "<<"main_ret:"<<main_ret;

    return main_ret;
}
