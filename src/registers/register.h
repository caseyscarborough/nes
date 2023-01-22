#ifndef NES_REGISTER_H
#define NES_REGISTER_H


#include <cstdint>

template<typename T>
class Register {
public:
    // Get the full status register value.
    [[nodiscard]] virtual T get_value() const {
        return value;
    }

    // Set the full status register value.
    virtual void set_value(T v) {
        value = v;
    }

    // Set a specific flag.
    virtual void set(T flag) {
        value |= flag;
    }

    // Clear (disable) a flag on the status register.
    virtual void clear(T flag) {
        value &= ~flag;
    }

    // Set the flag if the condition is true, otherwise, clear the flag.
    virtual void set_if(T flag, bool condition) {
        if (condition) {
            set_value(flag);
        } else {
            clear(flag);
        }
    }

    // Check if the flag is set.
    [[nodiscard]] virtual bool is_set(T flag) const {
        return (value & flag) == flag;
    }

    // Check if the flag is clear.
    [[nodiscard]] virtual bool is_clear(T flag) const {
        return !is_set(flag);
    }

protected:
    T value = 0x0;
};

#endif //NES_REGISTER_H
