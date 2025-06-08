#include "IFrameSource.h"
#include <string>

class DiskFrameSource : public IFrameSource 
{
    std::vector<std::string> imagePaths;
    size_t currentIndex = 0;
    bool loop = false;
    int width, height;
    IFrameFormat format;
    int fps;

public:
    CamSimStatusType IFrameSource_Init(int width, int height, IFrameFormat fmt, int fps, bool loop) override;
    CamSimStatusType IFrameSource_GetNextFrame(Frame& frame) override;
    CamSimStatusType IFrameSource_Start() override;
    CamSimStatusType IFrameSource_Stop() override;
};