// Header files for the DJI OSDK Communication App - Video Controller
#ifndef VIDEO_CONTROLLER_HPP
#define VIDEO_CONTROLLER_HPP

#include <iostream>
#include <dji_vehicle.hpp>

// Forward declaration to avoid including full dji_vehicle.hpp if only types are needed
// However, for CameraRGBImage, we do need it.
// So including dji_vehicle.hpp is fine here as it's central to the class.

namespace dji_osdk_communication_app
{
    class VideoController
    {
    public:
        VideoController(DJI::OSDK::Vehicle *vehicle_ptr);
        ~VideoController();

        bool startMainCameraStream();
        void stopMainCameraStream();

    private:
        DJI::OSDK::Vehicle *vehicle; // Pointer to the main vehicle object
        bool stream_active;          // Flag to check if the stream is active
        static int frame_count;      // Static variable to count frames
        // Callback function to handle camera stream data
        static void cameraStreamCallback(DJI::OSDK::CameraRGBImage img, void *userData);

        // Member function to handle the data received from the camera stream
        void handleStreamData(DJI::OSDK::CameraRGBImage &img);
    }
}
#endif // VIDEO_CONTROLLER_HPP