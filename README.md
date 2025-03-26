# 17648FinalProj

This repository contains the code for a vehicle data server and client project. The server is containerized using Docker and runs on Ubuntu, while the client is compiled and executed locally using your system's GCC. The server and client communicate over TCP on port 8080.

---

## Table of Contents

- [Requirements](#requirements)
- [Setup and Usage](#setup-and-usage)
  - [Using the Shell Script](#using-the-shell-script)
  - [Running the Client and Server](#running-the-client-and-server)
- [Troubleshooting](#troubleshooting)
- [File Descriptions](#file-descriptions)
  - [Dockerfile](#dockerfile)
  - [api_test.sh)](#apitestsh)
  - [VData.c and VData.h](#vdatac-and-vdatah)
  - [server.c](#serverc)
  - [client.c](#clientc)
---

## Requirements

- **Docker Desktop** installed on your machine with **WSL 2 integration enabled** (if using Ubuntu under WSL).
- **GCC** installed on your local machine.
- A Linux/Ubuntu terminal (or WSL 2) for running the shell script.
- Basic familiarity with Git and Docker.

---

## Setup and Usage

### Using the Shell Script

A shell script `api_test.sh` is provided to automate the following steps:
1. Compile the server using your local GCC.
2. Build the Docker container for the server (without containerizing GCC).
3. Mount the compiled server binary into the container and run the server.
4. Compile and run the client locally.

To run the script, open a terminal in the project root directory and execute:

```bash
./run_container.sh          # Run the script
```


### Running the Client and Server
- The server runs within the container and randomly generates vehicle data to track and provide as needed. It also pushes all data to client at 1 minutes intervals.
-The client runs locally and connects to the containerized server. The client will open with possible commands
