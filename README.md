# SF_MergeServer
Linux 下合成服务
1.用Mp4v2库把aac,h264裸码流合成MP4文件；
2.Http服务用队列Web接收Web参数，生产者消费者模式，线程池进行异步处理合成每个ID对应的文件。
