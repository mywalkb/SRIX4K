#ifndef SRIX4K_H
#define SRIX4K_H

#include "Arduino.h"
#include <PN532.h>

class SRIX4K : public PN532 {
public:
  SRIX4K(PN532Interface &interface) : PN532(interface) {} ;

  // SRIX4K functions
  bool init(void);
  bool initiate_select(void);
  bool readblock(uint8_t address, uint8_t *block);
  bool writeblock(uint8_t address, uint8_t block[4]);
  bool writeblock(uint8_t address, uint8_t block[4], bool bVerify);
  bool writeblock(uint8_t address, uint8_t block[4], bool bVerify, byte iRetry);
  bool uid(uint8_t *block);
};

#endif
