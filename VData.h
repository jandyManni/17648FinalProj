#ifndef VDATA
#define VDATA
#include <stdio.h>
#include <stdint.h>

#pragma pack(1)  //geeks for geeks other struct format for GCC and clang
struct VData {
    int16_t OilTemp;  // Degrees F
    uint16_t MAFSensor; // 0-11 bit A/D signal
    uint8_t BatteryVoltage; // 0-12 volts
    uint16_t TirePressure; //0-11 bit
    uint16_t FuelLevel; //Liters
    uint8_t FuelConsumptionRate;//Liters/hour
    uint32_t ErrorCodes; //4 4-8 bit universal codes 
};
struct VDataID {
    int16_t Id; //Vehicle ID
    int16_t OilTemp;  // Degrees F
    uint16_t MAFSensor; // 0-11 bit A/D signal
    uint8_t BatteryVoltage; // 0-12 volts
    uint16_t TirePressure; //0-11 bit
    uint16_t FuelLevel; //Liters
    uint8_t FuelConsumptionRate;//Liters/hour
    uint32_t ErrorCodes; //4 4-8 bit universal codes 
};

void getVehicleData(struct VData *data);

#endif