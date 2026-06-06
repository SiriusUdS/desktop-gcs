#include "VideoFeed.h"

#include "CamConfig.h"
#include "Logging.h"

VideoFeed::VideoFeed(const std::string& ip, int port, int feedID) : id(feedID) {
    url = "rtsp://" + std::string(CamConfig::user) + ":" + std::string(CamConfig::password) + "@" + ip + ":" + std::to_string(port) + std::string(CamConfig::path);
}

VideoFeed::~VideoFeed() {
    Stop();
}

void VideoFeed::Start() {
    isRunning = true;
    workerThread = std::thread(&VideoFeed::ThreadFunction, this);
}

void VideoFeed::Stop() {
    isRunning = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

bool VideoFeed::GetLatestFrame(cv::Mat& frame) {
    if (!newFrameAvailable) return false;
    
    std::lock_guard<std::mutex> lock(frameMutex);
    latestFrame.copyTo(frame);
    newFrameAvailable = false;
    return true;
}

int VideoFeed::GetID() const {
    return id;
}

void VideoFeed::ThreadFunction() {
    cv::VideoCapture cap(url, cv::CAP_FFMPEG);
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1); //Favors latency
    if (!cap.isOpened()) {
        GCS_APP_LOG_ERROR("Video Feed {} could not be opened", id);
        return;
    }
    
    cv::Mat temp;
    while (isRunning) {
        if (cap.read(temp)) {
            cv::cvtColor(temp, temp, cv::COLOR_BGR2RGBA);
            
            std::lock_guard<std::mutex> lock(frameMutex);
            temp.copyTo(latestFrame);
            newFrameAvailable = true;
        }
    }
    cap.release();
}