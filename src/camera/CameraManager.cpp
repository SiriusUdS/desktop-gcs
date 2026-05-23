#include "CameraManager.h"

#include "CamConfig.h"

#include <glad/glad.h>

CameraManager::CameraManager() {
}

CameraManager::~CameraManager() {
    for (auto& pair: textureMap) {
        glDeleteTextures(1, &pair.second); 
    }
}

CameraManager& CameraManager::get() {
    static CameraManager instance;
    return instance;
}

void CameraManager::init() {
    for (auto IP :CamConfig::ips) {
        std::string ip = (std::string)IP;
        addFeed(ip, CamConfig::port);
    }
    
}

void CameraManager::processFrameAndUploadToGPU() {
    camerRenderData.clear();
    
    for (auto& feed: videoFeeds) {
        cv::Mat frame;
        uint32_t texId = 0;
        
        if (feed->GetLatestFrame(frame)) {
            texId = getGPUTexture(feed->GetID(), frame);
        }else {
            texId = textureMap[feed->GetID()];
        }
        
        camerRenderData.push_back({
            feed->GetID(),
            static_cast<uint32_t>(texId),
            frame.empty() ? 400: frame.cols,
            frame.empty() ? 225: frame.rows,
        }
        );
    }
}

const std::vector<CameraRenderData>& CameraManager::getRenderData() const {
    return camerRenderData;
}

void CameraManager::addFeed(std::string& ip, int port) {
    auto feed = std::make_unique<VideoFeed>(ip, port, nextFeedId++);
    feed->Start();
    videoFeeds.push_back(std::move(feed));
}

uint32_t CameraManager::getGPUTexture(int camId, const cv::Mat& newFrame) {
    if (newFrame.empty()) {
        return 0;
    }
    
    uint32_t texture = textureMap[camId];
    
    if (texture == 0) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        textureMap[camId] = texture;
    }else {
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newFrame.cols, newFrame.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, newFrame.ptr(0));
    return texture;
}