#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <time.h>
#include "VData.h"

#define PORT 8080

#define CMD_PULL 1
#define CMD_ADD 2
#define CMD_PULL_ID 3
#define CMD_REPLACE 4
#define CMD_INVALID -1
#define CMD_EXIT 0

typedef struct {
    int command;  // Command type
    int id1;      // First ID (if applicable)
    int id2;      // Second ID (if applicable)
} Command;


struct VData vehicleData[64];

int vehicle_ids[64];

int nextVic = 0;

//Method to add vehicle to data array
void addVic(int id, char response[]){
    if (id<=0 || id > 65535) {
        snprintf(response, 64, "Incorrect ID value. Must be nonzero 16-bit int less than 65536");
    }
    else if(nextVic < 64){
    struct VData newV = {0};
    vehicle_ids[nextVic] = id;
    getVehicleData(&newV);
    vehicleData[nextVic] = newV;
    snprintf(response, 64, "Vic %d added to slot %d\n", id, nextVic);
    printf("Vehicle %d data: OilTemp=%d, MAFSensor=%d, BatteryVoltage=%d, TirePressure=%d, FuelLevel=%d\n",
        id, newV.OilTemp, newV.MAFSensor, newV.BatteryVoltage, newV.TirePressure, newV.FuelLevel);
    nextVic++;
    }
    else{
        snprintf(response, 64, "Error: Vehicle slot is full.");
    }
}
//Method to replace vehicle to data array
void replaceVic(int id, int new_id, char response[]){
    if (new_id<=0 || new_id > 65535) {
        snprintf(response, 64, "Incorrect ID value. Must be nonzero 16-bit int less than 65536");
    }
    else {
        struct VData newV = {0};
        getVehicleData(&newV);
        for(int i = 0; i < 64; i++){
            int check_id = vehicle_ids[i];
            if(check_id==id){
                vehicleData[i] = newV;
                vehicle_ids[i] = new_id;
                snprintf(response, 64, "Vic %d added to slot %d\n", new_id, i);
                return;
            }
        }
        snprintf(response, 64, "No vehicle matching given id found.");
    }
}
//method to pull all Vic data
void pullAll(struct VDataID table[]){
    for(int i=0;i<64;i++){
        struct VData data = vehicleData[i];
        int id = vehicle_ids[i];
        if (id == 0) memset(&table[i], 0, sizeof(struct VDataID)); //0 is empty id
        else{
            printf("\nVIC ID:%d Record Accessed\n",id);
            printf("Vehicle %d data: OilTemp=%d, MAFSensor=%d, BatteryVoltage=%d, TirePressure=%d, FuelLevel=%d\n",
                id, data.OilTemp, data.MAFSensor, data.BatteryVoltage, data.TirePressure, data.FuelLevel);
            table[i].Id = id;
            table[i].OilTemp = data.OilTemp;
            table[i].MAFSensor = data.MAFSensor;
            table[i].BatteryVoltage = data.BatteryVoltage;
            table[i].TirePressure = data.TirePressure;
            table[i].FuelLevel = data.FuelLevel;
            table[i].FuelConsumptionRate = data.FuelConsumptionRate;
            table[i].ErrorCodes = data.ErrorCodes;
        }
    }
}
//method to pull one vic based on id
void pullID(struct VDataID *result, int t_id){
    for(int i=0;i<64;i++){
        struct VData data = vehicleData[i];
        int id = vehicle_ids[i];
        if (id == t_id) {
            printf("\nVIC ID:%d Record Accessed\n",id);
            result->Id = id;
            result->OilTemp = data.OilTemp;
            result->MAFSensor = data.MAFSensor;
            result->BatteryVoltage = data.BatteryVoltage;
            result->TirePressure = data.TirePressure;
            result->FuelLevel = data.FuelLevel;
            result->FuelConsumptionRate = data.FuelConsumptionRate;
            result->ErrorCodes = data.ErrorCodes;
        }
    }
}

void handleClient(int client_socket) {
    Command cmd;
    //commands to handle polling for pull requests at regular intervals
    struct pollfd pfd;
    pfd.fd = client_socket;
    pfd.events = POLLIN;
    time_t last_check = time(NULL);
    const int timeout_ms = 1000; //should be 1 second

    while (1) {
        //check available data otherwise check elapsed time to push data every minute for now
        int poll_res = poll(&pfd, 1, timeout_ms);
        if (poll_res > 0){
        // Receive command from client
            ssize_t bytes_received = recv(client_socket, &cmd, sizeof(cmd), 0);
            if (bytes_received <= 0) {
                printf("Client disconnected or error occurred.\n");
                break;
        }
        // Command handling logic
        printf("\nReceived command: ");
        switch (cmd.command) {
                case CMD_PULL:
                    printf("PULL ALL\n");
                    struct VDataID vehicles[64];
                    pullAll(vehicles);
                    send(client_socket, vehicles, sizeof(vehicles), 0);
                    break;
                case CMD_ADD:
                    char response[64];
                    printf("ADD Vehicle ID: %d\n", cmd.id1);
                    addVic(cmd.id1, response);
                    printf("Sending: %s\n", response);
                    send(client_socket, response, sizeof(response), 0);  
                    break;
                case CMD_PULL_ID:
                    printf("PULL Vehicle ID: %d\n", cmd.id1);
                    struct VDataID getV = {0};
                    pullID(&getV,cmd.id1);
                    printf("Vehicle %d data: OilTemp=%d, MAFSensor=%d, BatteryVoltage=%d, TirePressure=%d, FuelLevel=%d\n",
                    getV.Id, getV.OilTemp, getV.MAFSensor, getV.BatteryVoltage, getV.TirePressure, getV.FuelLevel);
                    send(client_socket, &getV, sizeof(getV), 0);         
                    break;
                case CMD_REPLACE:
                    printf("REPLACE Vehicle ID %d with %d\n", cmd.id1, cmd.id2);
                    response[64];
                    replaceVic(cmd.id1, cmd.id2, response);
                    printf("Sending: %s\n", response);
                    send(client_socket, response, sizeof(response), 0); 
                    break;
                case CMD_EXIT:
                    printf("EXIT command received. Closing connection.\n");
                    close(client_socket);
                    return;
                default:
                    printf("INVALID command received.\n");
                    break;
            }
        }else if (poll_res == 0) {
            time_t now = time(NULL);
            if (difftime(now, last_check) >= 60) { 
                printf("\nPushing current Vic data to gateway.\n");
                struct VDataID vehicles[64];
                pullAll(vehicles);
                send(client_socket, vehicles, sizeof(vehicles), 0);
                last_check = now;
            }
        } else {
            perror("Poll error");
            break;
        }
    }

    // Close client socket
    close(client_socket);
}

int main() {
    char empt[64];
    memset(vehicle_ids, 0, sizeof(vehicle_ids));
    memset(vehicleData, 0, sizeof(vehicleData)); //reset vehicle memory on restart
    addVic(1, empt); //for testing, ensure something is present
    int server_fd, client_socket;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {

        // Accept client connection
        client_socket = accept(server_fd, (struct sockaddr*)&address, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        printf("New client connected.\n");

        // Handle client communication
        handleClient(client_socket);
    }

    // Close server socket
    close(server_fd);
    return 0;
}
