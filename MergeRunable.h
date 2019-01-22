#ifndef MERGERUNABLE_H
#define MERGERUNABLE_H

#include "glog/logging.h"
#include "json.hpp"
#include "LibcurClient.h"
#include "webserver.h"

using json = nlohmann::json;

using namespace std;

//extern string AACSTR ,H264STR ,JSONSTR ,MERGESTR ,RELATIVEPATH ,IPPORT;

class MergeRunable
{
public:
    MergeRunable(std::string path);
    ~MergeRunable();

    int run();
	
    std::string getLiveID(){return basePath;}

private:

   int readFileList();

   int mergeFile(std::map<int, string> &Map ,std::string filetype);

   //更新合成状态
   int UpdataRecordflag(int flag);

   //解析http返回json
   int ParseJsonInfo(std::string &jsonStr ,std::string &resCodeInfo);

private:

   std::map<int, string> m_AacMap;
   std::map<int, string> m_H264Map;
   std::map<int, string> m_JsonMap;
	
   std::string basePath;

   std::string liveIDStr;
};

#endif // MERGERUNABLE_H