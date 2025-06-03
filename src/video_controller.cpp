// Get live video feed from drone camera
#include "dji-osdk-communication-app/video_controller.hpp"

// initialize static member
int dji_osdk_communication_app::VideoController::frame_count = 0;

namespace dji_osdk_communication_app
{
    VideoController::VideoController(DJI::OSDK::Vehicle *vehicle_ptr)
        : vehicle(vehicle_ptr), stream_active(false)
    {
        if (vehicle == nullptr)
        {
            std::cerr << "ERROR [VideoController]: Vehicle or camera module is null!" << std::endl;
        }
    }

    VideoController::~VideoController()
    {
        if (stream_active)
        {
            stopMainCameraStream();
        }
    }

    bool VideoController::startMainCameraStream()
    {
        if (!vehicle || !vehicle->camera)
        {
            std::cerr << "ERROR [VideoController]: Cannot start stream, vehicle or camera not initialized." << std::endl;
            return false;
        }
        if (stream_active)
        {
            std::cout << "INFO [VideoController]: Stream already active." << std::endl;
            return true;
        }

        std::cout << "INFO [VideoController]: Setting camera stream view to Main Camera." << std::endl;
        vehicle->camera->setStreamView(DJI::OSDK::CAMERA_STREAM_VIEW_MAIN_CAMERA);
        // For M300 with multiple main cameras, we might need:
        // When testing we can uncomment this
        // vehicle->camera[DJI::OSDK::PAYLOAD_INDEX_0]->setStreamView(DJI::OSDK::CAMERA_STREAM_VIEW_MAIN_CAMERA);

        std::cout << "INFO [VideoController]: Attempting to start H.264/H.265 stream..." << std::endl;
        // Pass 'this' as userData so the static callback can call the member function
        bool result = vehicle->camera->startH264Stream(VideoController::cameraStreamCallback, this);

        if (result)
        {
            std::cout << "SUCCESS [VideoController]: H.264/H.265 stream started." << std::endl;
            stream_active = true;
        }
        else
        {
            std::cerr << "ERROR [VideoController]: Failed to start H.264/H.265 stream." << std::endl;
            stream_active = false;
        }
        return stream_active;
    }

    void VideoController::stopMainCameraStream()
    {
        if (!vehicle || !vehicle->camera)
        {
            std::cerr << "ERROR [VideoController]: Cannot stop stream, vehicle or camera not initialized." << std::endl;
            return;
        }
        if (!stream_active)
        {
            std::cout << "INFO [VideoController]: Stream not active, no need to stop." << std::endl;
            return;
        }

        std::cout << "INFO [VideoController]: Stopping H.264/H.265 stream..." << std::endl;
        vehicle->camera->stopH264Stream();
        stream_active = false;
        std::cout << "INFO [VideoController]: Stream stopped." << std::endl;
    }

    // Static callback function
    void VideoController::cameraStreamCallback(DJI::OSDK::CameraRGBImage img, void *userData)
    {
        VideoController *self = static_cast<VideoController *>(userData);
        if (self)
        {
            self->handleStreamData(img);
        }
    }

    // Member function to process data
    void VideoController::handleStreamData(DJI::OSDK::CameraRGBImage &img)
    {
        // img.rawData points to the H.264/H.265 NAL unit data
        // The size of this data chunk is implicitly handled by OSDK for the callback.
        std::cout << "Received camera stream data chunk " << ++frame_count
                  << ". Data pointer: " << (void *)img.rawData << std::endl;

        // --- IMPORTANT ---
        // TODO: Implement on a later stage, what we want to do with the data:
        // 1. Copy data from img.rawData.
        // 2. Push to a thread-safe queue for a decoder thread.
    }
}