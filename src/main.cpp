#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <csignal>    // For signal handling (Ctrl+C)
#include <filesystem> // For std::filesystem::current_path() C++17

// DJI OSDK Includes
#include <dji_vehicle.hpp>
#include <dji_linux_helpers.hpp>

// Project specific include
#include "dji-osdk-communication-app/video_controller.hpp"

using namespace DJI::OSDK;

// Global pointers for OSDK
// TODO: encapsulate further on a later stage
Vehicle *vehicle_ptr_global = nullptr;
LinuxSetup *linuxEnvironment_global = nullptr;
bool app_running = true;

// --- Configuration Structure ---
struct AppConfig
{
    std::string appName;
    int appID;
    std::string appKey;
    std::string appLicense;
    std::string serialDevice;
    int baudRate;
};

// --- Function to read UserConfig.txt ---
bool readUserConfig(AppConfig &config)
{
    std::string config_file_path = "../UserConfig.txt";
    std::ifstream configFile(config_file_path);

    if (!configFile.is_open())
    {
        // Try path next to executable as a fallback (if CMAKE_RUNTIME_OUTPUT_DIRECTORY changes)
        config_file_path = "UserConfig.txt";
        configFile.open(config_file_path);
    }

    if (!configFile.is_open())
    {
        std::cerr << "ERROR: Cannot open UserConfig.txt. Expected at ../UserConfig.txt or UserConfig.txt (relative to executable)." << std::endl;
        std::cerr << "Current working directory: " << std::filesystem::current_path() << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(configFile, line))
    {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            key.erase(0, key.find_first_not_of(" \t\n\r"));
            key.erase(key.find_last_not_of(" \t\n\r") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);

            if (key == "app_name")
                config.appName = value;
            else if (key == "app_id")
                config.appID = std::stoi(value);
            else if (key == "app_key")
                config.appKey = value;
            else if (key == "app_license")
                config.appLicense = value;
            else if (key == "serial_device")
                config.serialDevice = value;
            else if (key == "baud_rate")
                config.baudRate = std::stoi(value);
        }
    }
    configFile.close();

    if (config.appID == 0 || config.appKey.empty() || config.serialDevice.empty() || config.baudRate == 0)
    {
        std::cerr << "ERROR: One or more required fields missing in UserConfig.txt" << std::endl;
        return false;
    }
    std::cout << "UserConfig.txt loaded successfully." << std::endl;
    std::cout << "  App Name: " << config.appName << std::endl;
    std::cout << "  App ID: " << config.appID << std::endl;
    std::cout << "  Serial Device: " << config.serialDevice << std::endl;
    std::cout << "  Baud Rate: " << config.baudRate << std::endl;
    return true;
}

// --- Signal Handler for Ctrl+C ---
void signalHandler(int signum)
{
    std::cout << "\nInterrupt signal (" << signum << ") received." << std::endl;
    app_running = false;
}

// --- Main Application ---
int main(int argc, char **argv)
{
    signal(SIGINT, signalHandler);

    AppConfig config = {};
    if (!readUserConfig(config))
    {
        return -1;
    }

    linuxEnvironment_global = new LinuxSetup(argc, argv, true);
    vehicle_ptr_global = linuxEnvironment_global->getVehicle();
    if (vehicle_ptr_global == nullptr)
    {
        std::cerr << "ERROR: Vehicle not initialized, exiting." << std::endl;
        delete linuxEnvironment_global;
        return -1;
    }

    DJI::OSDK::AppInfo app_info;
    app_info.appID = config.appID;
    strncpy(app_info.appKey, config.appKey.c_str(), sizeof(app_info.appKey) - 1);
    app_info.appKey[sizeof(app_info.appKey) - 1] = '\0';
    app_info.baudRate = config.baudRate;
    strncpy(app_info.deviceName, config.serialDevice.c_str(), sizeof(app_info.deviceName) - 1);
    app_info.deviceName[sizeof(app_info.deviceName) - 1] = '\0';
    app_info.useAdvancedSensing = false;

    std::cout << "Attempting to activate OSDK..." << std::endl;
    ACK::ErrorCode ack = vehicle_ptr_global->activate(&app_info, DJI::OSDK::Vehicle::activateBlocking);

    if (ACK::getError(ack))
    {
        std::cerr << "ERROR: Failed to activate OSDK. Error code: " << ACK::getError(ack) << std::endl;
        ACK::getErrorCodeMessage(ack, __func__);
        delete vehicle_ptr_global; // vehicle is managed by linuxEnvironment
        delete linuxEnvironment_global;
        return -1;
    }
    std::cout << "OSDK Activated successfully!" << std::endl;
    std::cout << "Drone firmware version: " << vehicle_ptr_global->getFwVersion() << std::endl;
    std::cout << "OSDK version: " << vehicle_ptr_global->getOSDKVersion() << std::endl;

    // --- Initialize and use VideoController ---
    dji_osdk_app::VideoController videoController(vehicle_ptr_global);
    if (videoController.startMainCameraStream())
    {
        std::cout << "SUCCESS: Main camera stream initiated via VideoController." << std::endl;
    }
    else
    {
        std::cerr << "ERROR: Failed to start main camera stream via VideoController." << std::endl;
    }

    std::cout << "Application running. Press Ctrl+C to exit." << std::endl;
    while (app_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Cleaning up and exiting..." << std::endl;
    // VideoController destructor will attempt to stop stream if active
    // Explicit stop can also be called if needed before videoController goes out of scope.
    // videoController.stopMainCameraStream();

    if (linuxEnvironment_global)
    {
        delete linuxEnvironment_global;
        linuxEnvironment_global = nullptr;
        vehicle_ptr_global = nullptr;
    }

    std::cout << "Application terminated." << std::endl;
    return 0;
}