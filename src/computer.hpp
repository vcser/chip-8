#ifndef COMPUTER_H
#define COMPUTER_H
#include "util.hpp"

struct Display;

class Computer {
    public:
        Computer();
        ~Computer();
        void load(const char *path);
        u16 fetch();
        void execute(u16 opcode);
        void loop();
        void reset();
        u16 pc;
        u16 i;
        u8 memory[4096];
        u8 v[16];
        u8 sp;
        u16 stack[16];
        u8 dt;
        u8 st;
        Display *display;
        u8 keyboard[16];
        unsigned int state;
        unsigned long long int step_count;
    private:
};

struct Display {
    public:
        void draw(u8 x, u8 y, u8 n, Computer &chip_8);
        void clear();
        u8 pixels[64*32];
    private:

};

#endif
