#pragma once

class Akatsukible {
public:
    static const unsigned char PAYLOAD[];
    virtual void akatsukize() = 0;
};

// Legacy
const unsigned char Akatsukible::PAYLOAD[] = {
    0x41, 0x6B, 0x61, 0x74, 0x73, 0x75,
    0x6B, 0x69, 0x4A, 0x69, 0x61, 0x49,
    0x73, 0x56, 0x65, 0x72, 0x79, 0x43,
    0x75, 0x74, 0x65
};
