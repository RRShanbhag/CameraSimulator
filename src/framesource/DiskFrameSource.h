#pragma once
#include "IFrameSource.h"
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class DiskFrameSource : public IFrameSource 
{
private:
    std::vector<std::string> m_FilePaths;
    int m_nCurrentIndex = -1;
    bool m_bLoopBack = false;
    int m_nFPS;

    AVFormatContext* m_pAVFormatCtx = nullptr;
    AVCodecContext* m_pAVCodecCtx = nullptr;
    SwsContext* m_pSwsCtx = nullptr;
    AVPacket* m_pAVPacket = nullptr;
    AVFrame* m_pAVConvertedFrame = nullptr;
    unsigned char* m_pAVFrameBuffer = nullptr;
    int m_nVideoStreamIndex = -1;

public:
    // Constructor and Destructor
    DiskFrameSource();
    ~DiskFrameSource();

    // Copy Constructor and Assignment
    DiskFrameSource(const DiskFrameSource&) = delete;
    DiskFrameSource& operator=(const DiskFrameSource&) = delete;

    // Move Constructors and Assignment
    DiskFrameSource(DiskFrameSource&& other) noexcept;
    DiskFrameSource& operator=(DiskFrameSource&& other) noexcept;

    CamSimStatusType IFrameSource_Init(std::string location, int width, int height, IFrameFormat fmt, int fps, bool loop) override;
    CamSimStatusType IFrameSource_GetNextFrame(IFrame& frame) override;
    CamSimStatusType IFrameSource_Start() override;
    CamSimStatusType IFrameSource_Stop() override;

private:
    int CalculateFrameSize(int width, int height, IFrameFormat fmt);
    int PopulateVideoFiles(const std::string& directoryPath, std::vector<std::string>& FilePaths);
    AVPixelFormat GetAVPixelFormatFromIFrameFormat(IFrameFormat fmt);
    IFrameFormat GetIFrameFormatFromAVPixelFormat(AVPixelFormat fmt);

public: 
    std::vector<std::string>& GetFilePaths();
    IFrame* GetCurrentFrame();

};