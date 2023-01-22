#ifndef NES_STATUS_H
#define NES_STATUS_H


#include <cstdint>
#include "register.h"

// Status register for the CPU.
class StatusRegister : public Register<uint8_t> {
public:
    //@formatter:off
    // Flags for the status register
    enum Flag {
        Carry            = 1 << 0,
        Zero             = 1 << 1,
        InterruptDisable = 1 << 2,
        Decimal          = 1 << 3, // has no effect on the NES
        Break            = 1 << 4,
        Unused           = 1 << 5,
        Overflow         = 1 << 6,
        Negative         = 1 << 7,
    };
    //@formatter:on

    // Set default flags based on the provided data.
    void set_defaults(uint8_t flags, uint16_t data);

    // Override for setting the value (unused should always be set to 1
    // for the status register so we'll ensure that in the override).
    void set_value(uint8_t v) override;
private:
};



#endif //NES_STATUS_H
