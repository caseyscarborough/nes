#ifndef NES_REGISTER_H
#define NES_REGISTER_H


#include <cstdint>

class Register {
public:
    // Get the full status register value.
    [[nodiscard]] virtual uint8_t get_value() const;

    // Set the full status register value.
    virtual void set_value(uint8_t v);

    // Set a specific flag.
    virtual void set(uint8_t flag);

    // Clear (disable) a flag on the status register.
    virtual void clear(uint8_t flag);

    // Set the flag if the condition is true, otherwise, clear the flag.
    virtual void set_if(uint8_t flag, bool condition);

    // Check if the flag is set.
    [[nodiscard]] virtual bool is_set(uint8_t flag) const;

    // Check if the flag is clear.
    [[nodiscard]] virtual bool is_clear(uint8_t flag) const;

protected:
    uint8_t value = 0x0;
};


#endif //NES_REGISTER_H
