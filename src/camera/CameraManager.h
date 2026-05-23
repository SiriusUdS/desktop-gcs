#pragma once
#include "CameraRenderData.h"
#include "VideoFeed.h"

#include <memory>
#include <string>
#include <vector>

class CameraManager {
public:
    static CameraManager& get();
    
    CameraManager(const CameraManager&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;
    
    void init();
    void processFrameAndUploadToGPU();
    const std::vector<CameraRenderData>& getRenderData() const;
    void addFeed(std::string& ip, int port);
private:
    CameraManager();
    ~CameraManager();
    std::vector<std::unique_ptr<VideoFeed>> videoFeeds;
    std::vector<CameraRenderData> camerRenderData;
    
    std::unordered_map<int, uint32_t> textureMap;
    uint32_t getGPUTexture(int camId, const cv::Mat& newFrame);
    
    int nextFeedId = 1;
};
