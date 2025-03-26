#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "VData.h"


void getVehicleData(struct VData *data) {
    srand(time(NULL));
    data->OilTemp = 60 + rand() % 41;
    data->MAFSensor = rand() % 2048;
    data->BatteryVoltage = rand() % 13; 
    data->TirePressure = 20 + rand() % 31;
    data->FuelLevel = rand() % 11; 
    data->FuelConsumptionRate = 1 + rand() % 5; 
    data->ErrorCodes = (0xA1 << 24) | (0xC1 << 16) | (0x55 << 8) | 0x23;
}


