**Development Environment on OBC:**
    *   **Operating System:** Linux (Ubuntu).
    *   **C++ Compiler:** `g++`.
    *   **CMake:** For building the OSDK and application we need `cmake`. `sudo apt-get install cmake`
    *   **DJI OSDK Library:**
        *   Clone the official repository: `git clone https://github.com/dji-sdk/Onboard-SDK.git`
        *   Navigate into the cloned directory.
        *   Follow the instructions in the OSDK documentation to build the core library (usually involves `mkdir build && cd build && cmake .. && make && sudo make install`).
        *   Pay attention to platform-specific build instructions if any (e.g., for ARM on Jetson).