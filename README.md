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
- [Documenation](#documentation)
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
- The client runs locally and connects to the containerized server. The client will open with possible commands listed to the user. The client allows the user to add new vehicle's by id, pull all vehicle data, pull by id, and replace by id.

## Troubleshooting

### "Docker Not Found/ Permission Denied": 
- Ensure Docker Desktop is installed and running.

- Enable WSL 2 integration in Docker Desktop settings.

- If you see permission errors, add your WSL user to the docker group:

```bash
sudo usermod -aG docker $USER
newgrp docker
```

### "Bind failed: Address already in use":
- top any existing container using the same name with

```bash
docker rm -f server-instance
```

### "GCC Not Found:"
```bash
gcc --version
```

## File Descriptions

### DockerFile
Defines the docker image using ubuntu as the base image. Expects user to provide gcc at runtime.

### api_test.sh
Script to automate the following:
- Compile server code from local GCC
- Build Docker image
- Run the Docker container
- Compile and run client code locally
- Cleaning up container after client exit

### VData.c and VData.h
Simulates vehicle data retrieval with a ramdomly filled packed struct. Future implementations may allow for adjust_vehicle_data() method to simulate fluctuations in vehicle data that is already pulled.

### server.c
Vehicle API that manages up to 64 vehicles, does the following:
- Maintains array of 64 vehicle data structs and array of 64 vehicle ids (16bit int)
- Listens on port 8080 for client connection
- Once connected, polls port for client requests before pushing all maintained vehicle data at regular intervals
- Handles client pull, pull id, replace vehicle, add id requests

### client.c
Simulates computer connected to vehicle API, does the following:
- Polls port for push data and user input every second
- If push data, retrieves and prints formatted
- If client input, sends and awaits api response

## Documentation
Initial code setup and inspiration for tcp developed after reading the following articles:
- https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
- https://www.geeksforgeeks.org/socket-programming-cc/

ChatGPT was used to assist debugging, and to assist construction of dockerfile and .sh to utilize local gcc in the setup of container and test client.