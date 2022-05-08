# SRIX4K
Arduino library for SRIX4K communication using PN532

Require arduino library mywalkb/PN532 (forked by SeeedStudio)

Example:

```c++
#include <PN532_SPI.h>
#include "SRIX4K.h"

PN532_SPI pn532spi(SPI, 10);
SRIX4K nfc(pn532spi);

void setup(void) 
{
    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (! versiondata) 
        while (1); // halt

    nfc.SAMConfig();

    nfc.init(); // SRIX4K init set registers in PN532
}
void loop(void)
{
    if(nfc.initiate_select())
    {
        uint8_t uid[8];
        nfc.uid(uid);
        // print uid
    }
    delay(300);
}
```
