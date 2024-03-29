#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
 
#include "base.h"
#include "glog/logging.h"
 
#define LOG_FILE "mergemonitor."

#define IPPORT "http://192.168.1.205:8080/live/"

using json = nlohmann::json;

string recordID = "0";

int httpGetfun(int res)
{
    printf("recordID :%s\n", recordID.c_str());
    LibcurClient  m_httpclient;
    std::string url = IPPORT;
  
    url = url.append("server_update?serverId=");
    url.append(recordID);
    url.append("&netFlag=0");
    cout<<"offline url:"<<url<<endl;

    url.append("&netFlag=1&serverIp=192.168.1.206:8000"); 
    int main_ret = m_httpclient.HttpGetData(url.c_str());

   if(main_ret != 0)
   {   
       LOG(ERROR) << "注册录制服务离线失败 错误代号:"<<main_ret;
       return main_ret;

   }else
   {
      json m_object = json::parse(m_httpclient.GetResdata());
      if(m_object.is_object())
      {
         string resCode = m_object.value("code", "oops");
         main_ret = atoi(resCode.c_str() );

         if(0 != main_ret)
         {
             string message = m_object.value("msg", "oops"); //错误信息:%s ,message.c_str()
             
             LOG(ERROR) << "注册录制服务离线失败 main_ret: "<<main_ret;
             //LOG(ERROR) << "message: "<<message;
           
             return main_ret ;
         }
      }
   }

   return 0;
}
 
 
int main(int argc, char **argv) 
{
    int ret, i, status;
    char *child_argv[100] = {0};
    pid_t pid;
    char buff[1024];

    //创建log初始化
    google::InitGoogleLogging("");
    google::SetLogDestination(google::GLOG_INFO,LOG_FILE);
    FLAGS_logbufsecs = 0; //缓冲日志输出，默认为30秒，此处改为立即输出
    FLAGS_max_log_size = 500; //最大日志大小为 100MB
    FLAGS_stop_logging_if_full_disk = true; //当磁盘被写满时，停止日志输出

    if (argc < 2) 
    {   
        sprintf(buff,"Usage:%s <exe_path> <args...>", argv[0]);     
        LOG(ERROR) << buff;
        return -1;
    }
    
    for (i = 1; i < argc; ++i) 
    {
        child_argv[i-1] = (char *)malloc(strlen(argv[i])+1);
        strncpy(child_argv[i-1], argv[i], strlen(argv[i]));
    }

    int execTime = 0;

    while(1){

        pid = fork(); 

        printf("pid: %d\n", pid);

        LOG(INFO) << "pid :"<< (int)pid;
        if(pid == -1) 
        {
            printf("fork() error.errno:%d error:%s", errno, strerror(errno));
            LOG(ERROR)<<"fork() error.errno error:"<< strerror(errno);

            break;

        }else if(pid == 0) 
        {
        
            LOG(ERROR)<<"serverName:"<<child_argv[0];
            ret = execv(child_argv[0], (char **)child_argv); 
                
            if (ret < 0) 
            {
                execTime++;
                printf("Child process execv ret:%d errno:%d error:%s time:%d",ret, errno, strerror(errno),execTime);
                sprintf(buff,"Child process ret:%d errno:%d error:%s time:%d",ret, errno, strerror(errno),execTime);                
                LOG(ERROR)<<buff;

                break;
            }
            
        }else
        {
            //pid = wait(&status);
            waitpid(pid, &status,0);
        
            printf("Child process id: %d \n", pid);
            LOG(INFO)<<"Child process id: "<<(int)pid;

            printf("Child process exit with status: %d\n",status);
            LOG(INFO)<<"Child process exit with status:"<<status;
        }
    }

    printf("Parent process exit!\n");
    LOG(INFO)<<"Parent process exit!";

    return ret;
}
