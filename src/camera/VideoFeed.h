#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class VideoFeed {
public:
    VideoFeed(const std::string& ip, int port, int feedID);
    ~VideoFeed();
    void Start();
    void Stop();
    bool GetLatestFrame(cv::Mat& outFrame);
    int GetID() const;
private:
    std::string url;
    int id;
    
    std::thread workerThread;
    std::mutex frameMutex;
    std::atomic<bool> isRunning{false};
    std::atomic<bool> newFrameAvailable{false};
    cv::Mat latestFrame;
    
    void ThreadFunction();
};
