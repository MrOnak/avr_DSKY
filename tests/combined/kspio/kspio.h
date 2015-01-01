/**
 * Port of the KSPIO code (c) by 'zitronen' to AVR C
 * http://forum.kerbalspaceprogram.com/threads/66393
 *
 * Zitronen's code is released under Creative Commons Attribution (BY).
 * The same is true for my port.
 *
 * Zitronen's code was based around the Arduino library and
 * mostly intended as a demonstration, to get people started.
 *
 * I'm porting this to AVR's variant of C to shrink things down
 * and speed it up a little bit. Other than that, the code is
 * unaltered.
 *
 * This port is based off of the "KSPIODemo8" and expects to
 * be communicating with v0.15.3 of the KSPIO plugin.
 *
 * @author Dominique Stender <dst@st-webdevelopment.com>
 */

#ifndef KSPIO_H
#define KSPIO_H

// structs

typedef struct {
    uint8_t id; //1
    float AP; //2
    float PE; //3
    float SemiMajorAxis; //4
    float SemiMinorAxis; //5
    float VVI; //6
    float e; //7
    float inc; //8
    float G; //9
    int32_t TAp; //10
    int32_t TPe; //11
    float TrueAnomaly; //12
    float Density; //13
    int32_t period; //14
    float RAlt; //15
    float Alt; //16
    float Vsurf; //17
    float Lat; //18
    float Lon; //19
    float LiquidFuelTot; //20
    float LiquidFuel; //21
    float OxidizerTot; //22
    float Oxidizer; //23
    float EChargeTot; //24
    float ECharge; //25
    float MonoPropTot; //26
    float MonoProp; //27
    float IntakeAirTot; //28
    float IntakeAir; //29
    float SolidFuelTot; //30
    float SolidFuel; //31
    float XenonGasTot; //32
    float XenonGas; //33
    float LiquidFuelTotS; //34
    float LiquidFuelS; //35
    float OxidizerTotS; //36
    float OxidizerS; //37
    uint32_t MissionTime; //38
    float deltaTime; //39
    float VOrbit; //40
    uint32_t MNTime; //41
    float MNDeltaV; //42
    float Pitch; //43
    float Roll; //44
    float Heading; //45
} vesselData_t;

typedef struct {
    uint8_t id;
    uint8_t M1;
    uint8_t M2;
    uint8_t M3;
} handShakePacket_t;

typedef struct {
    uint8_t id;
    uint8_t MainControls; //SAS RCS Lights Gear Brakes Precision Abort Stage
    uint8_t Mode; //0 = stage, 1 = docking, 2 = map
    uint16_t ControlGroup; //control groups 1-10 in 2 bytes
    uint8_t AdditionalControlByte1; //other stuff
    uint8_t AdditionalControlByte2;
    int16_t Pitch; //-1000 -> 1000
    int16_t Roll; //-1000 -> 1000
    int16_t Yaw; //-1000 -> 1000
    int16_t TX; //-1000 -> 1000
    int16_t TY; //-1000 -> 1000
    int16_t TZ; //-1000 -> 1000
    int16_t Throttle; //    0 -> 1000
} controlPacket_t;

extern vesselData_t kspio_vData;

extern void kspio_init();
extern int8_t kspio_input();

void kspio_initTXPackets();
void kspio_handshake();
uint8_t kspio_boardReceiveData();
void kspio_boardSendData(uint8_t * address, uint8_t len);

#endif // KSPIO_H
