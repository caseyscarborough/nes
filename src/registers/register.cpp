#include "register.h"

void Register::set_if(uint8_t flag, bool condition) {
    if (condition) {
        set_value(flag);
    } else {
        clear(flag);
    }
}

uint8_t Register::get_value() const {
    return value;
}

void Register::set_value(uint8_t v) {
    this->value = v;
}

void Register::set(uint8_t v) {
    value |= v;
}

void Register::clear(uint8_t flag) {
    value &= ~flag;
}


bool Register::is_set(uint8_t flag) const {
    return (value & flag) == flag;
}

bool Register::is_clear(uint8_t flag) const {
    return !is_set(flag);
}
