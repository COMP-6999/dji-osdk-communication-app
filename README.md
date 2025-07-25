## Development Environment on OBC:

### Operating System:
Linux (Ubuntu 20.04 or 22.04).
JetPack latest or 5.1.4

### C++ Compiler: 
`g++`

### CMake:
For building the OSDK and application, we need `cmake`. 
`sudo apt-get install cmake`

### DJI OSDK Library:
_Step 1:_ Clone the official repository: 
`git clone https://github.com/dji-sdk/Onboard-SDK.git`

_Step 2:_ Navigate into the cloned directory.

_Step 3:_ Follow the instructions in the OSDK documentation to build the core library
usually involves doing:
`mkdir build && cd build && cmake .. && make && sudo make install`

_Step 4:_ Pay attention to platform-specific build instructions in this case for ARM on Jetson.
