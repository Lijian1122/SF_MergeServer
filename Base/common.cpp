#include "common.h"
#include <string>

using namespace std;

string  ServerPort;
string  APIStr;
string  updateOnlineUrl;
string  RELATIVEPATH;
string  IPPORT;
string  serverName;
string  IpPort;
string  LOGDIR;

//Http API方法名
string ServerCreate;
string ServerDelete;
string ServerSelect;
string ServerUpdate;

//合成服务ID
string merge_serverId;

string liveUpdate;
string liveSelect;
string liveUpload;

string  AACSTR = ".aac";
string  H264STR = ".h264";
string  JSONSTR = ".json";
string  MERGESTR = "merge.";
string  MP4STR = ".mp4";
string  CFGSTR = ".cfg";

//裸码流文件路径
string  MergeFilePath;

//合成配置文件字段
string aacTimestamp =  "AACTimestamp";
string h264Timestamp =  "H264Timestamp";
string whiteTimestamp =  "WhiteTimestamp";
string recordTimestr = "RecordTimes";

