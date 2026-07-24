# Build

`cd packetizer-application`

`mkdir -p build && cd build`

- Use UDP as transport layer

    `cmake -DTRANSPORT_BACKEND=UDP ..`

- Use TCP as transport layer

    `cmake -DTRANSPORT_BACKEND=TCP ..`

`cmake --build .`

# Running

`./peer <local port> <remote ip> <remote port>`

Example: runnning two separate process that communicate with each other.

Process 1:

`./peer 5000 127.0.0.1 6000`

Process 2:

`./peer 6000 127.0.0.1 5000`

**NOTE:** since TCP uses handshaking to estabilish connection between ports, a timeout may happen if the user doesn't start the second peer in time. I've used a timeout of 30 seconds so the port isn't taken for long if the application goes idle without a second peer.