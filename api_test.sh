#!/bin/bash

# Check for local gcc
if ! command -v gcc &> /dev/null; then
    echo "Error: gcc is not installed on your local machine."
    exit 1
fi

SERVER_BINARY="server"
CLIENT_BINARY="client"

echo "Compiling server using local gcc..."
gcc VData.c server.c -o $SERVER_BINARY
if [ $? -ne 0 ]; then
    echo "Server compilation failed!"
    exit 1
fi

echo "Building Docker container..."
docker buildx build --load -t server-container -f DockerFile .

# Remove existing container for multiple runs
if docker ps -a --format '{{.Names}}' | grep -q "^server-instance\$"; then
    echo "Removing existing container 'server-instance'..."
    docker rm -f server-instance
fi

echo "Running Docker container with mounted server binary..."
docker run -d --name server-instance -v "$(pwd)/$SERVER_BINARY:/app/server" -p 8080:8080 server-container /app/server

# wait for the server to start
sleep 2

echo "Checking running container..."
docker ps | grep server-instance

echo "Compiling client using local gcc..."
gcc VData.c client.c -o $CLIENT_BINARY
if [ $? -ne 0 ]; then
    echo "Client compilation failed!"
    exit 1
fi

echo "Running the client..."
./client

#shut down server
docker stop server-instance
docker rm server-instance
