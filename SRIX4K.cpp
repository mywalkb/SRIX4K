#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

//#define DEBUG

#include "SRIX4K.h"

/**************************************************************************/
/*!
    Send InListPassiveTarget command
*/
/**************************************************************************/
bool SRIX4K::init(void) 
{
    if (!writeRegister(REG_CIU_Control, 0x10)) // set bit 4 for configure as Initiator or Reader/Writer
        return false;
    if (!writeRegister(REG_CIU_TxAuto, 0x0F)) // setting of the antenna driver
        return false;
    if (!writeRegister(REG_CIU_Mode, 0x3B)) // general modes for transmitting and receiving
        return false;

    // configure for ISO1443B and kb106
    if (!writeRegister(REG_CIU_TxMode, (ISO1443B & 0x03) | ((kb106 & 0x07) << 4) | 0x80))
        return false;
    if (!writeRegister(REG_CIU_RxMode, (ISO1443B & 0x03) | ((kb106 & 0x07) << 4) | 0x80))
        return false;

    if(!setRFField(0, 1))
        return false;

    if(!setTimeoutComm(0x6)) // timeout 3.2 ms (default 51.2ms) good for write block which don't replay
        return false;

    /*byte config = 0x80;
    if (!writeRegister(REG_CIU_TxControl, 0x83)) // antenna on
        return false;*/

    //Send PN532 InlistPassiveTarget command
    // bug in pn532 must call InListPassiveTarget before raw transmission also in libnfc there is this issue
    // 1 max target
    // 0x03 => ISO14443B
    // 0x00 => AFI all PICCs
    byte buf[] = {0x00}; // AFI
    if(!startPassiveTargetIDDetection(ISO1443B, sizeof(buf), buf))
        return false;
    return true;
}
/**************************************************************************/
/*!
    Send INITATE and SELECT command
*/
/**************************************************************************/
bool SRIX4K::initiate_select(void) 
{
    byte abSelInit[] = {0x06, 0x00};
    byte abRx[8];
    byte len = sizeof(abRx);

    if(!inCommunicateThru(abSelInit, sizeof(abSelInit), abRx, &len))
        return false;

    abSelInit[0] = 0x0e;
    abSelInit[1] = abRx[0];

    len = sizeof(abRx);
    if(!inCommunicateThru(abSelInit, sizeof(abSelInit), abRx, &len))
        return false;

    return true;
}
/**************************************************************************/
/*!
    Send READ_BLOCK command
*/
/**************************************************************************/
bool SRIX4K::readblock(uint8_t address, uint8_t *block) 
{
    byte abRdBlk[] = {0x08, address};
    byte abRx[8];
    byte len = sizeof(abRx);

    if(!inCommunicateThru(abRdBlk, sizeof(abRdBlk), abRx, &len))
        return false;

    block[0] = abRx[0];
    block[1] = abRx[1];
    block[2] = abRx[2];
    block[3] = abRx[3];

    return true;
}

/**************************************************************************/
/*!
    Send WRITE_BLOCK command
*/
/**************************************************************************/
bool SRIX4K::writeblock(uint8_t address, uint8_t block[4]) {
    writeblock(address, block, false);
}

bool SRIX4K::writeblock(uint8_t address, uint8_t block[4], bool bVerify) {
    writeblock(address, block, bVerify, 1);
}

bool SRIX4K::writeblock(uint8_t address, uint8_t block[4], bool bVerify, byte iRetry) 
{
    byte abWrBlk[] = {0x09, address, block[0], block[1], block[2], block[3]};
    byte abRx[2];
    byte len = 0;

    while(iRetry-->0) 
    {
#ifdef DEBUG
        Serial.print("inCommunicateThru ");
        Serial.println(address, HEX);  
#endif
        if(!inCommunicateThru(abWrBlk, sizeof(abWrBlk), abRx, &len)) // about 20 ms for write block
            continue;

        // wait almost 5ms after write (datasheet), there is a timeout 3.2ms in pn532 for wait a response + other check and we got 7/8/9 ms 
  
        if (bVerify)
        {
            uint8_t blockread[4];

            if (readblock(address, blockread)) // about 10 ms for read block
            {
                if(blockread[0] == block[0] && blockread[1] == block[1] && blockread[2] == block[2] && blockread[3] == block[3])
                    return true;
                else
                    continue;
            }
            else
            {
#ifdef DEBUG
                Serial.println("Read after write failed");  
#endif
                continue;
            }
        }
        else
            return true;
    }
    return false;
}

/**************************************************************************/
/*!
    Send GET_UID command
*/
/**************************************************************************/
bool SRIX4K::uid(uint8_t *uid) 
{
    byte abUid[] = {0x0b};
    byte abRx[12];
    byte len = sizeof(abRx);

    if(!inCommunicateThru(abUid, sizeof(abUid), abRx, &len))
        return false;

    uid[0] = abRx[0];
    uid[1] = abRx[1];
    uid[2] = abRx[2];
    uid[3] = abRx[3];
    uid[4] = abRx[4];
    uid[5] = abRx[5];
    uid[6] = abRx[6];
    uid[7] = abRx[7];

    return true;
}
