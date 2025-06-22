#include "DiskFrameSource.h"
#include "../utils/CamSimLogger.h"
#include <filesystem>
/**
 * @brief Construct a new Disk Frame Source:: Disk Frame Source object
 * 
 */
DiskFrameSource::DiskFrameSource()
    : IFrameSource() 
{
}

/**
 * @brief Destroy the Disk Frame Source:: Disk Frame Source object
 * 
 */
DiskFrameSource::~DiskFrameSource()
{
    if(this->m_pCurrentFrame != nullptr)
    {
        if(this->m_pCurrentFrame->data != nullptr)
        {
            delete[] this->m_pCurrentFrame->data;
            this->m_pCurrentFrame->data = nullptr;
        }
        delete this->m_pCurrentFrame;
        this->m_pCurrentFrame = nullptr;
    }
} 

/**
 * @brief DiskFrameSource::IFrameSource_Init
 * Input Frame Source initialization of type file based present on the disk.
 * Checks if the Framesource is already initialized.
 * @param location: location of the video files
 * @param width: width of Frame Source
 * @param height: height of Frame Source
 * @param fmt: Format of the Frame Source
 * @param fps: FPS of the Frame Source
 * @param loop: Loopback enabled or not.
 * @return CamSimStatusType 
 */
CamSimStatusType DiskFrameSource::IFrameSource_Init(std::string location, int width, int height, IFrameFormat fmt, int fps, bool loop)
{
    CamSimStatusType eStatus = CamSimStatusType::E_STATUS_SUCCESS;
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;

    if(this->m_pCurrentFrame != nullptr)
    {
        eError = CamSimErrorType::E_ERRORCODE_MEMORY_ALLOC_FAILED;
    }
    else
    {
        this->m_pCurrentFrame = new IFrame();
        this->m_pCurrentFrame->width = width;
        this->m_pCurrentFrame->height = height;
        this->m_pCurrentFrame->format = fmt;
        this->m_pCurrentFrame->frameSize = CalculateFrameSize(width, height, fmt); 
        this->m_pCurrentFrame->data = new unsigned char[this->m_pCurrentFrame->frameSize > 0 ? this->m_pCurrentFrame->frameSize : 1];
    }

    if(this->PopulateVideoFiles(location, m_FilePaths) <= 0)
    {
        eError = CamSimErrorType::E_ERRORCODE_INITIALIZATION_FAILED;
    }
    else
    {
        m_bLoopBack = loop;
        m_nFPS = fps;
    }

    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
    }
    return eStatus;
}
CamSimStatusType DiskFrameSource::IFrameSource_GetNextFrame(IFrame& frame)
{
    CamSimStatusType eStatus = CamSimStatusType::E_STATUS_SUCCESS;
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;


    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
    }
    return eStatus;
}
CamSimStatusType DiskFrameSource::IFrameSource_Start()
{
    CamSimStatusType eStatus = CamSimStatusType::E_STATUS_SUCCESS;
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;


    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
    }
    return eStatus;
}

CamSimStatusType DiskFrameSource::IFrameSource_Stop()
{
    CamSimStatusType eStatus = CamSimStatusType::E_STATUS_SUCCESS;
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;


    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
    }
    return eStatus;
}

int DiskFrameSource::CalculateFrameSize(int width, int height, IFrameFormat fmt)
{
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;
    int nFrameSize = 0;

    switch(fmt)
    {
        case IFrameFormat::E_GRAY8:
        {
            nFrameSize = width * height * 1;
            CAMSIMLOG_INFO("Setting Frame Size: %d", nFrameSize);
            break;
        }

        case IFrameFormat::E_GRAY16:
        {
            nFrameSize = width * height * 2;
            CAMSIMLOG_INFO("Setting Frame Size: %d", nFrameSize);
            break;
        } 

        case IFrameFormat::E_RGB:
        case IFrameFormat::E_BGR:
        {
            nFrameSize = width * height * 3;
            CAMSIMLOG_INFO("Setting Frame Size: %d", nFrameSize);
            break;
        }

        case IFrameFormat::E_UYVY:
        {
            nFrameSize = width * height * 2;
            CAMSIMLOG_INFO("Setting Frame Size: %d", nFrameSize);
            break;
        } 

        default:
        {
            nFrameSize = MAX_FRAME_WIDTH * MAX_FRAME_HEIGHT * MAX_FRAME_CHANNELS;
            CAMSIMLOG_WARN("Setting maximum FRAME size %d", nFrameSize);
            break;
        }
    }

    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
    }
    return nFrameSize;
}

int DiskFrameSource::PopulateVideoFiles(const std::string& directoryPath,
                        std::vector<std::string>& FilePaths)
{
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;
    FilePaths.clear();
    int totalFileCount = 0;

    try 
    {
        if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) {
            eError = CamSimErrorType::E_ERRORCODE_INVALID_FILE_OR_DIRECTORY;
            CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
        }

        if(eError == CamSimErrorType::E_ERRORCODE_NOERROR)
        {
            for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
                if (entry.is_regular_file()) {
                    ++totalFileCount;

                    std::string extension = entry.path().extension().string();
                    if (extension == ".mp4" || extension == ".avi" ||
                        extension == ".mkv" || extension == ".mov" ||
                        extension == ".flv") {
                        FilePaths.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        CAMSIMLOG_ERROR("Exception: %s", ex.what());
        eError = CamSimErrorType::E_ERRORCODE_INVALID_FILE_OR_DIRECTORY;
    }

    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
    }
    return totalFileCount;
}

std::vector<std::string>& DiskFrameSource::GetFilePaths()
{
    return m_FilePaths;
}

IFrame* DiskFrameSource::GetCurrentFrame()
{
    return m_pCurrentFrame;
}