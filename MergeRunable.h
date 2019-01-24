#ifndef MERGERUNABLE_H
#define MERGERUNABLE_H

#include <dirent.h>
#include <unistd.h>
#include "glog/logging.h"
#include "json.hpp"
#include "LibcurClient.h"
#include "common.h"

using json = nlohmann::json;

using namespace std;

extern string AACSTR ,H264STR ,JSONSTR ,MERGESTR ,RELATIVEPATH ,IPPORT;

class MergeRunable
{
public:
    MergeRunable(std::string liveID);
    ~MergeRunable();

	//开始启动合成
    int run();
	
	//获取LiveID
    std::string getLiveID(){return m_liveId;}

private:

   //将文件进行分类
   int searchFile();

   //找到的文件插入不同类型的map中
   int setFileNameToMap(std::map<int, string> &Map , std::string &fileName , std::string &fileType);
   
   //合成文件
   int mergeFile(std::map<int, string> &Map ,std::string filetype);

   //更新合成状态
   int UpdataMergeflag(int flag);

   //解析http返回json
   int ParseJsonInfo(std::string &jsonStr ,std::string &resCodeInfo);

private:
   std::map<int, string> m_AacMap;
   std::map<int, string> m_H264Map;
   std::map<int, string> m_JsonMap;
	
   std::string m_liveId;
   int m_liveIdSize;

};

#endif // MERGERUNABLE_H
