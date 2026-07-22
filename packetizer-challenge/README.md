# Dynamox C/C++ Developer Challenge

This repository contains a custom C++ network protocol implementation developed for the **Dynamox** technical evaluation challenge. 

---

## 🚀 Architecture Highlights

* **Single Dual-Mode Executable:** A single compiled binary acts as either a **Server** or a **Client** based on command-line (`argv`) arguments.
* **Hardware/Transport Abstraction (`NetworkDevice`):** The transport layer is cleanly decoupled from the core protocol logic, allowing seamless swapping of the underlying network mechanism (e.g., Unix domain sockets, TCP/IP sockets, or mock emulators).
* **Compile-Time Configuration via Traits:** Uses C++ Traits to define system-wide parameters, buffer sizes, timeouts, and failure policies at compile-time without runtime performance overhead (*zero-cost abstractions*).
* **Fault Injection & Resiliency Testing:** Includes an `UnsafeUnixNetworkDevice` implementation capable of injecting configurable probabilities of **packet loss** and **data corruption** to test protocol reliability and packet retransmission mechanisms.
* **Unix Pipe Input Stream:** The client accepts payload streams directly through standard input (`stdin`) using **Linux Pipes (`|`)**, allowing seamless integration with system utilities (`cat`, `dd`, `echo`).

---

## 🛠️ Prerequisites & Building

### Prerequisites
* C++17 (or higher) compliant compiler (`g++` or `clang++`)
* `make`
* Linux / POSIX-compliant environment
  
### Building the Project

To compile the source code, run:

```bash
make clean && make
```

## 💻 Step-by-Step Execution Tutorial

### Step 1: Start the Server

In your **first terminal**, launch the server instance. The server must be running first to listen for incoming packets on the `NetworkDevice` abstraction layer:

```bash
./build/Dynamox SERVER
```
### Step 2: Send Data from the Client

Open a **second terminal** and start the client. The client reads its input from **standard input (`stdin`)**, making it easy to integrate with common Unix utilities.

For example, to send the contents of a text file:

```bash
cat data/LoremIpsum.txt | ./build/Dynamox CLIENT
```

## 🧪 Running the Automated Tests

The repository includes an automated test script that starts the server in a separate terminal and executes predefined client-side transmission tests.

Simply run:

```bash
make tests
```

> **Note:** `make tests` requires either **`gnome-terminal`** or **`xterm`** to be installed so it can automatically launch the server process in a separate terminal window.
