#pragma once
#include "IFrameSource.h"
#include <string>

class DiskFrameSource : public IFrameSource 
{
private:
    std::vector<std::string> m_FilePaths;
    size_t m_nCurrentIndex = 0;
    bool m_bLoopBack = false;
    int m_nFPS;

public:
    DiskFrameSource();
    ~DiskFrameSource();
    CamSimStatusType IFrameSource_Init(std::string location, int width, int height, IFrameFormat fmt, int fps, bool loop) override;
    CamSimStatusType IFrameSource_GetNextFrame(IFrame& frame) override;
    CamSimStatusType IFrameSource_Start() override;
    CamSimStatusType IFrameSource_Stop() override;

private:
    int CalculateFrameSize(int width, int height, IFrameFormat fmt);
    int PopulateVideoFiles(const std::string& directoryPath, std::vector<std::string>& FilePaths);

public: 
    std::vector<std::string>& GetFilePaths();
    IFrame* GetCurrentFrame();

};