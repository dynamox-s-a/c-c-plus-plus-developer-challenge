# Build

`cd packetizer-application`

`mkdir -p build && cd build`

`cmake ..`

`cmake --build .`

# Running

`./peer <local port> <remote ip> <remote port>`

Example: runnning two separate process that communicate with each other.

Process 1:

`./peer 5000 127.0.0.1 6000`

Process 2:

`./peer 6000 127.0.0.1 5000`