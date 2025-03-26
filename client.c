#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
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
    int id1;      // First ID
    int id2;      // Second ID
} Command;

Command parseInput(const char *input) {
    Command cmd = {CMD_INVALID, -1, -1}; //Default to bad command
    char action[10];
    int id1, id2;
//parse input 
    if (sscanf(input, "%9s", action) == 1) {
        if (strcmp(action, "pull") == 0) {
            if (sscanf(input, "pull %d", &id1) == 1) {
                cmd.command = CMD_PULL_ID;
                cmd.id1 = id1;
            } else {
                cmd.command = CMD_PULL;
            }
        } 
        else if (strcmp(action, "add") == 0) {
            if (sscanf(input, "add %d", &id1) == 1) {
                cmd.command = CMD_ADD;
                cmd.id1 = id1;
            }
        } 
        else if (strcmp(action, "replace") == 0) {
            if (sscanf(input, "replace %d, %d", &id1, &id2) == 2) {
                cmd.command = CMD_REPLACE;
                cmd.id1 = id1;
                cmd.id2 = id2;
            }
        }
        else if (strcmp(action, "exit") == 0) {
            cmd.command = CMD_EXIT;
        }
    }
    return cmd;
}

void printVehicle(struct VDataID vic) {
    printf("\nID: %d", vic.Id);
    printf("\nOil Temp: %d°F", vic.OilTemp);
    printf("\nMAF Sensor: %d", vic.MAFSensor);
    printf("\nBattery Voltage: %dV", vic.BatteryVoltage);
    printf("\nTire Pressure: %d PSI", vic.TirePressure);
    printf("\nFuel Level: %d Liters", vic.FuelLevel);
    printf("\nFuel Consumption Rate: %d Liters/hour", vic.FuelConsumptionRate);
    printf("\nError Codes: 0x%X\n", vic.ErrorCodes);
}

void parseVehicles(struct VDataID table[]) {
    for (int i = 0; i < 64; i++) {
        if (table[i].Id == 0) {
            if (i == 0) printf("No Records Found");
            break; 
        }
        printVehicle(table[i]);
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 address
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }
    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    // logic to govern user interaction, print commands first
    printf("\nEnter 'pull' to see all data\nEnter 'pull id#' to see a specific vehicle\nEnter 'add id#' to synchronize a new vehicle\nEnter 'replace old_id#, new_id#' to replace a vehicle\nEnter 'exit' to end\nUser: ");
    while (1){
        //poll data structures
        struct pollfd pfd[2]; //one for socket, one for user input, assistance from gpt for understanding how this works
        pfd[0].fd = sock;
        pfd[0].events = POLLIN;
        pfd[1].fd = STDIN_FILENO;
        pfd[1].events = POLLIN;
        const int timeout_ms = 1000; //spend a second awaiting both polls adjust
        int poll_res = poll(pfd, 2, 1000);
            if (poll_res < 0) {
                perror("poll");
                break;
            }
        if (pfd[0].revents & POLLIN) {
            printf("\nPush Data Recieved\n");
            struct VDataID vehicles[64];
            ssize_t bytes_received = recv(sock, vehicles, sizeof(vehicles), 0);
            if (bytes_received > 0) {
                parseVehicles(vehicles);
                printf("\nUser:");
            } else {
                printf("Server Connection Closed or Error Occurred.\n");
                break;
            }
        }        
        //poll for and retrieve user input
        if (pfd[1].revents & POLLIN) {
            char input[50];
            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("Error reading input.\n");
                continue;
            }
        // process user input
            input[strcspn(input, "\n")] = 0;
            Command cmd = parseInput(input);
            if (cmd.command == CMD_INVALID) {
                printf("\nInvalid command\n");
                printf("\nEnter 'pull' to see all data\nEnter 'pull id#' to see a specific vehicle\nEnter 'add id#' to synchronize a new vehicle\nEnter 'replace old_id#, new_id#' to replace a vehicle\nEnter 'exit' to end\nUser: ");
                continue; //handle faulty command
            }
        //send command on good input
            send(sock, &cmd, sizeof(cmd), 0);
        // react based on expected response
        switch (cmd.command) {
        case CMD_PULL:
            printf("\nCommand: PULL ALL\n");
            struct VDataID vehicles[64];
            ssize_t bytes_received = recv(sock, vehicles, sizeof(vehicles), 0);
            if (bytes_received > 0) {
                parseVehicles(vehicles);
            } else {
                printf("Failed to receive vehicle data.\n");
            }
            break;
        case CMD_ADD:
            char response[64];
            printf("\nCommand: ADD, ID: %d\n", cmd.id1);
            bytes_received = recv(sock, response, sizeof(response), 0);
            if (bytes_received > 0) {
                printf("%s\n", response);
            } else {
                printf("Failed to add vehicle data.\n");
            }
            break;
        case CMD_PULL_ID:
            printf("\nCommand: PULL, ID: %d\n", cmd.id1);
            struct VDataID result = {0};
            bytes_received = recv(sock, &result, sizeof(result), 0);
            if (bytes_received > 0) {
                if(result.Id == 0)
                    printf("No Record Found\n");
                else 
                    printVehicle(result);
            } else {
                printf("Failed to receive vehicle data.\n");
            }
            break;
        case CMD_REPLACE:
            printf("\nCommand: REPLACE, Old ID: %d, New ID: %d\n", cmd.id1, cmd.id2);
            break;
        case CMD_EXIT:
            printf("Command: EXIT");
        default:
            printf("\nInvalid command\n"); //reiterate commands on invalid input
             }
        if (cmd.command == CMD_EXIT) {
            printf("Client Disconnecting.\n");
            break;
        }
        }
    }
    // Close socket
    close(sock);
    return 0;
}
