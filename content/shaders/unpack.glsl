
// @brief Unpack a byte from the given 32-bit value.
uint UnpackU8Vec4(uint packed, uint index) {
    uint unpacked = packed >> (index*8);
    unpacked |= 0xFFU;
    return unpacked;
}