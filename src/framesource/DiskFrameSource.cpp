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
    if(m_bIsIFrameStarted == false)
    {
        IFrameSource_Stop();
    }

    if(this->m_pCurrentFrame != nullptr)
    {
        delete this->m_pCurrentFrame;
        this->m_pCurrentFrame = nullptr;
    }
    m_FilePaths.clear();
    m_nCurrentIndex = 0;
} 


DiskFrameSource::DiskFrameSource(DiskFrameSource&& other) noexcept
    : m_FilePaths(std::move(other.m_FilePaths)) 
{
    m_nCurrentIndex = other.m_nCurrentIndex;
    m_bLoopBack = other.m_bLoopBack;
    m_nFPS = other.m_nFPS;

    m_pAVFormatCtx = other.m_pAVFormatCtx;
    m_pAVCodecCtx = other.m_pAVCodecCtx;
    m_pSwsCtx = other.m_pSwsCtx;
    m_pAVPacket = other.m_pAVPacket;
    m_pAVConvertedFrame = other.m_pAVConvertedFrame;
    m_pAVFrameBuffer = other.m_pAVFrameBuffer;
    m_nVideoStreamIndex = other.m_nVideoStreamIndex;
}

DiskFrameSource& DiskFrameSource::operator=(DiskFrameSource&& other) noexcept
{
    if (this != &other)
    {
        m_FilePaths = std::move(other.m_FilePaths); 
        m_nCurrentIndex = other.m_nCurrentIndex;
        m_bLoopBack = other.m_bLoopBack;
        m_nFPS = other.m_nFPS;

        m_pAVFormatCtx = other.m_pAVFormatCtx;
        m_pAVCodecCtx = other.m_pAVCodecCtx;
        m_pSwsCtx = other.m_pSwsCtx;
        m_pAVPacket = other.m_pAVPacket;
        m_pAVConvertedFrame = other.m_pAVConvertedFrame;
        m_pAVFrameBuffer = other.m_pAVFrameBuffer;
        m_nVideoStreamIndex = other.m_nVideoStreamIndex;
    }
    return *this;
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
        if(width < 0 || height < 0 || fmt == IFrameFormat::E_UNKNOWN)
        {
            CAMSIMLOG_INFO("Frame Format is unknown. Frame format shall be obtained directly from video meta data.");
        }
        this->m_pCurrentFrame = new IFrame();
        this->m_pCurrentFrame->width = width;
        this->m_pCurrentFrame->height = height;
        this->m_pCurrentFrame->format = fmt;
        this->m_pCurrentFrame->frameSize = CalculateFrameSize(width, height, fmt); 
        this->m_pCurrentFrame->dataOwner = std::unique_ptr<unsigned char[]>(new unsigned char[this->m_pCurrentFrame->frameSize]);
        this->m_pCurrentFrame->data = this->m_pCurrentFrame->dataOwner.get();
    }

    if(this->PopulateVideoFiles(location, m_FilePaths) <= 0)
    {
        eError = CamSimErrorType::E_ERRORCODE_INITIALIZATION_FAILED;
    }
    else
    {
        m_nCurrentIndex = 0;
        m_bLoopBack = loop;
        m_nFPS = fps;
    }

    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        m_eError = eError;
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
        eStatus = CamSimStatusType::E_STATUS_FAILURE;
    }
    return eStatus;
}

CamSimStatusType DiskFrameSource::IFrameSource_GetNextFrame(IFrame& frame)
{
    CamSimStatusType eStatus = CamSimStatusType::E_STATUS_SUCCESS;
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;

    if (!m_pAVFormatCtx || !m_pAVPacket || !m_pAVCodecCtx || !m_pCurrentFrame || !m_pSwsCtx) 
    {
        CAMSIMLOG_ERROR("One or more required FFmpeg contexts are uninitialized");
        return CamSimStatusType::E_STATUS_FAILURE;
    }

    int ret = -1;
    uint8_t* dstData[1] = { this->m_pCurrentFrame->data };
    int dstLinesize[1] = { (int)(this->m_pCurrentFrame->frameSize/this->m_pCurrentFrame->height) };

    try
    {
        // Read frames from the stream
        while ((ret = av_read_frame(m_pAVFormatCtx, m_pAVPacket)) >= 0) {
            // Check if this packet belongs to the video stream
            if (m_pAVPacket->stream_index == m_nVideoStreamIndex) {

                // Send packet to decoder
                ret = avcodec_send_packet(m_pAVCodecCtx, m_pAVPacket);
                if (ret < 0) {
                    CAMSIMLOG_ERROR("Failed to send packet to decoder");
                    break;
                }

                // Receive frame from decoder
                ret = avcodec_receive_frame(m_pAVCodecCtx, m_pAVConvertedFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    continue; // Try again
                } else if (ret < 0) {
                    CAMSIMLOG_ERROR("Failed to receive frame from decoder");
                    break;
                }

                // Convert frame to target pixel format (e.g., RGB24)
                sws_scale(m_pSwsCtx,
                        m_pAVConvertedFrame->data, m_pAVConvertedFrame->linesize, 0,
                        m_pAVCodecCtx->height,
                        dstData, dstLinesize);

                // Set IFrame metadata
                frame.ShallowCopy(*(this->m_pCurrentFrame));

                break; // Successfully read one frame
            }
        }
    }
    catch(const std::exception& e)
    {
        eError = CamSimErrorType::E_ERRORCODE_FRAME_GRABBING_FAILURE;
        CAMSIMLOG_ERROR(e.what());
    }
    



    if (ret < 0 && ret != AVERROR_EOF) {
        eError = CamSimErrorType::E_ERRORCODE_FRAME_GRABBING_FAILURE;
    }

    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        m_eError = eError;
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
        eStatus = CamSimStatusType::E_STATUS_FAILURE;
    }
    return eStatus;
}
CamSimStatusType DiskFrameSource::IFrameSource_Start()
{
    CamSimStatusType eStatus = CamSimStatusType::E_STATUS_SUCCESS;
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;
    int numBytes = 0;
    AVCodecParameters* codecPar = nullptr;
    const AVCodec* codec;

    if(m_nCurrentIndex <= -1)
    {
        CAMSIMLOG_ERROR("Invalid File Index");
        eError = CamSimErrorType::E_ERRORCODE_INITIALIZATION_FAILED;
        goto cleanup;
    }
    // Allocate format context
    m_pAVFormatCtx = avformat_alloc_context();

    // Open input file
    if (avformat_open_input(&m_pAVFormatCtx, m_FilePaths[m_nCurrentIndex].c_str(), nullptr, nullptr) < 0) {
        CAMSIMLOG_ERROR("Failed to open file: %s", m_FilePaths[m_nCurrentIndex].c_str());
        eError = CamSimErrorType::E_ERRORCODE_INITIALIZATION_FAILED;
        goto cleanup;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(m_pAVFormatCtx, nullptr) < 0) {
        CAMSIMLOG_ERROR("Failed to retrieve stream information");
        eError = CamSimErrorType::E_ERRORCODE_INITIALIZATION_FAILED;
        goto cleanup;
    }

    // Find the first video stream
    m_nVideoStreamIndex = -1;
    for (unsigned int i = 0; i < m_pAVFormatCtx->nb_streams; ++i) {
        if (m_pAVFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_nVideoStreamIndex = i;
            break;
        }
    }

    if (m_nVideoStreamIndex == -1) {
        CAMSIMLOG_ERROR("No video stream found");
        eError = CamSimErrorType::E_ERRORCODE_INITIALIZATION_FAILED;
        goto cleanup;
    }

    // Find decoder
    codecPar = m_pAVFormatCtx->streams[m_nVideoStreamIndex]->codecpar;
    codec = avcodec_find_decoder(codecPar->codec_id);
    if (!codec) {
        eError = CamSimErrorType::E_ERRORCODE_INITIALIZATION_FAILED;
        goto cleanup;
    }
    else
    {
        CAMSIMLOG_INFO("Codec Information:: S.Name:%s, L.Name: %s, Codec ID: %d", codec->name, codec->long_name, codecPar->codec_id);
        CAMSIMLOG_INFO("Video Information:: Width: %d, Height: %d, Format: %d", codecPar->width, codecPar->height, codecPar->format);
    }

    // Allocate codec context
    m_pAVCodecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(m_pAVCodecCtx, codecPar);

    if (avcodec_open2(m_pAVCodecCtx, codec, nullptr) < 0) {
        eError = CamSimErrorType::E_ERRORCODE_INITIALIZATION_FAILED;
        goto cleanup;
    }

    // Set SWS context for color conversion
    m_pSwsCtx = sws_getContext(m_pAVCodecCtx->width, m_pAVCodecCtx->height, m_pAVCodecCtx->pix_fmt,
                            m_pCurrentFrame->width, m_pCurrentFrame->height, GetAVPixelFormatFromIFrameFormat(m_pCurrentFrame->format),
                            SWS_BILINEAR, nullptr, nullptr, nullptr);
                            
    // Allocate converted frame buffer
    m_pAVPacket = av_packet_alloc();
    m_pAVConvertedFrame = av_frame_alloc();
    if (!m_pAVConvertedFrame || !m_pAVPacket) {
        eError = CamSimErrorType::E_ERRORCODE_MEMORY_ALLOC_FAILED;
        goto cleanup;
    }

    numBytes = av_image_get_buffer_size(m_pAVCodecCtx->pix_fmt, m_pCurrentFrame->width, m_pCurrentFrame->height, 1);
    m_pAVFrameBuffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(m_pAVConvertedFrame->data, m_pAVConvertedFrame->linesize, m_pAVFrameBuffer,
                         GetAVPixelFormatFromIFrameFormat(m_pCurrentFrame->format), m_pCurrentFrame->width, m_pCurrentFrame->height, 1);

cleanup:
    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        IFrameSource_Stop();  // Clean all resources
        m_eError = eError;
        CAMSIMLOG_ERROR("DiskFrameSource Start Failed with error: %s", CAMSIMERRORSTRING(eError));
        eStatus = CamSimStatusType::E_STATUS_FAILURE;
    }
    else
    {
        CAMSIMLOG_INFO("DiskFrameSource Start Success. FileIndex: %d", m_nCurrentIndex);
        
        if(m_bLoopBack)
        {
            m_nCurrentIndex = (m_nCurrentIndex+1) % m_FilePaths.size();    
        }
        else
        {
            m_nCurrentIndex = m_nCurrentIndex + 1 ;
            if(m_nCurrentIndex >= (int)m_FilePaths.size())
            {
                m_nCurrentIndex = -1;
            }
        }
        
    }

    return eStatus;
}

CamSimStatusType DiskFrameSource::IFrameSource_Stop()
{
    CamSimStatusType eStatus = CamSimStatusType::E_STATUS_SUCCESS;
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;

    if (m_pAVPacket) {
        av_packet_free(&m_pAVPacket);
        m_pAVPacket = nullptr;
    }

    if (m_pAVConvertedFrame) {
        av_frame_free(&m_pAVConvertedFrame);
        m_pAVConvertedFrame = nullptr;
    }

    if (m_pAVCodecCtx) {
        avcodec_free_context(&m_pAVCodecCtx);
        m_pAVCodecCtx = nullptr;
    }

    if (m_pAVFormatCtx) {
        avformat_close_input(&m_pAVFormatCtx);
        m_pAVFormatCtx = nullptr;
    }

    if (m_pAVFrameBuffer) {
        av_free(m_pAVFrameBuffer);
        m_pAVFrameBuffer = nullptr;
    }

    m_nVideoStreamIndex = -1;

    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        m_eError = eError;
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
        eStatus = CamSimStatusType::E_STATUS_FAILURE;
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
            CAMSIMLOG_INFO("GRAY8 Format: Setting Frame Size: %d", nFrameSize);
            break;
        }

        case IFrameFormat::E_GRAY16:
        {
            nFrameSize = width * height * 2;
            CAMSIMLOG_INFO("GRAY16 Format: Setting Frame Size: %d", nFrameSize);
            break;
        } 

        case IFrameFormat::E_RGB:
        case IFrameFormat::E_BGR:
        {
            nFrameSize = width * height * 3;
            CAMSIMLOG_INFO("RGB/BGR Format: Setting Frame Size: %d", nFrameSize);
            break;
        }

        case IFrameFormat::E_UYVY:
        {
            nFrameSize = width * height * 2;
            CAMSIMLOG_INFO("UYVY Format: Setting Frame Size: %d", nFrameSize);
            break;
        } 

        default:
        {
            nFrameSize = width * height * 0;
            CAMSIMLOG_WARN("Unknown Format: Setting FRAME size %d", nFrameSize);
            eError = CamSimErrorType::E_ERRORCODE_UNKNOWN_FORMAT;
            break;
        }
    }

    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        m_eError = eError;
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
        m_eError = eError;
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

AVPixelFormat DiskFrameSource::GetAVPixelFormatFromIFrameFormat(IFrameFormat fmt)
{
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;
    AVPixelFormat avPixFmt = AVPixelFormat::AV_PIX_FMT_NONE;

    switch(fmt)
    {
        case IFrameFormat::E_GRAY8:
        {
            avPixFmt = AVPixelFormat::AV_PIX_FMT_GRAY8;
            break;
        }

        case IFrameFormat::E_GRAY16:
        {
            avPixFmt = AVPixelFormat::AV_PIX_FMT_GRAY16LE;
            break;
        } 

        case IFrameFormat::E_RGB:
        case IFrameFormat::E_BGR:
        {
            avPixFmt = AVPixelFormat::AV_PIX_FMT_RGB24;
            break;
        }

        case IFrameFormat::E_UYVY:
        {
            avPixFmt = AVPixelFormat::AV_PIX_FMT_UYVY422;
            break;
        } 

        default:
        {
            avPixFmt = AVPixelFormat::AV_PIX_FMT_NONE;
            eError = CamSimErrorType::E_ERRORCODE_UNKNOWN_FORMAT;
            break;
        }
    }

    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        m_eError = eError;
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
    }
    return avPixFmt;
}

IFrameFormat DiskFrameSource::GetIFrameFormatFromAVPixelFormat(AVPixelFormat fmt)
{
    CamSimErrorType eError = CamSimErrorType::E_ERRORCODE_NOERROR;
    IFrameFormat iFrameFmt = IFrameFormat::E_UNKNOWN;

    switch(fmt)
    {
        case AVPixelFormat::AV_PIX_FMT_GRAY8:
        {
            iFrameFmt = IFrameFormat::E_GRAY8;
            break;
        }

        case AVPixelFormat::AV_PIX_FMT_GRAY16LE:
        {
            iFrameFmt = IFrameFormat::E_GRAY16;
            break;
        } 

        case AVPixelFormat::AV_PIX_FMT_BGR24:
        {
            iFrameFmt = IFrameFormat::E_BGR; 
            break;
        }
        case AVPixelFormat::AV_PIX_FMT_RGB24:
        {
            iFrameFmt = IFrameFormat::E_RGB;
            break;
        }

        case AVPixelFormat::AV_PIX_FMT_UYVY422:
        {
            iFrameFmt = IFrameFormat::E_UYVY;
            break;
        } 

        default:
        {
            iFrameFmt = IFrameFormat::E_UNKNOWN;
            eError = CamSimErrorType::E_ERRORCODE_UNKNOWN_FORMAT;
            break;
        }
    }

    if(eError != CamSimErrorType::E_ERRORCODE_NOERROR)
    {
        m_eError = eError;
        CAMSIMLOG_ERROR(CAMSIMERRORSTRING(eError));
    }
    return iFrameFmt;
}
