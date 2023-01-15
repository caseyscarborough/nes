#ifndef NES_REGISTER_H
#define NES_REGISTER_H


#include <cstdint>

class StatusRegister {
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

    // Get the full status register value.
    [[nodiscard]] uint8_t get() const;

    // Set the full status register value.
    void set(uint8_t v);

    // Set (enable) a flag on the status register.
    void set(StatusRegister::Flag flag);

    // Clear (disable) a flag on the status register.
    void clear(StatusRegister::Flag flag);

    // Set the flag if the condition is true, otherwise, clear the flag.
    void update(StatusRegister::Flag flag, bool condition);

    // Update specified flags with their default operations on the provided data.
    void update(uint8_t flags, uint16_t data);

    // Check if the flag is set.
    [[nodiscard]] bool is_set(Flag flag) const;

    // Check if the flag is clear.
    [[nodiscard]] bool is_clear(Flag flag) const;
private:
    uint8_t value;
};


#endif //NES_REGISTER_H
