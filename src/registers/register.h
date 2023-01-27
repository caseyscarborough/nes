#ifndef NES_REGISTER_H
#define NES_REGISTER_H


#include <string>
#include <cstdint>
#include "../utils.h"

template<typename T>
class Register {
public:

    Register(): Register(0) {};

    explicit Register(T value) {
        this->value = value;
    }

    virtual ~Register() = default;

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

    // Get a specific flag.
    virtual uint8_t get(T flag) {
        return ((value & flag) >> utils::count_trailing_zeroes(flag));
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

    virtual void set(T flag, T data) {
        // Shift the data by the number of trailing zeroes for this flag.
        data <<= utils::count_trailing_zeroes(flag);

        // Mask the data with the flag to ensure we don't modify any
        // bits outside the flag.
        data &= flag;

        // Disable the current flag in the register.
        value &= ~flag;

        // Enable the specified bits for the flag.
        value |= data;
    }

    // TODO: Figure out how to overload = to set value. No matter what I try, I keep getting "no viable overloaded '='"
    // Use .set_value() for now

    virtual Register<T> &operator=(const T &data) {
        value = data;
        return *this;
    }

    virtual Register<T> &operator+=(const T &data) {
        value += data;
        return *this;
    }

    virtual Register<T> &operator-=(const T &data) {
        value -= data;
        return *this;
    }

    virtual bool operator>=(const T &data) {
        return value >= data;
    }

    virtual bool operator<=(const T &data) {
        return value <= data;
    }

    virtual bool operator==(const T &data) {
        return value == data;
    }

    virtual bool operator!=(const T &data) {
        return value != data;
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
