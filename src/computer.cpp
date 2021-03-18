#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include "computer.hpp"

enum State {
    RUNNING,
    STEP,
    PAUSED,
};

Computer::Computer() {
    std::srand(std::time(NULL));
    std::memset(memory, 0, sizeof(memory));
    u8 font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    std::memcpy(&memory[0x50], font, 5*16);
    display = new Display();
    std::memset(v, 0, sizeof(v));
    std::memset(stack, 0, sizeof(stack));
    state = PAUSED;
    reset();
}

Computer::~Computer() {
    delete [] display;
}

void Computer::loop() {
    if (state == PAUSED)
        return;
    else if (state == STEP) {
        u16 opcode = fetch();
        execute(opcode);
        if (dt > 0)
            dt--;
        if (st > 0)
            st--;
        state = PAUSED;
    } else {
        u16 opcode = fetch();
        execute(opcode);
        if (dt > 0)
            dt--;
        if (st > 0)
            st--;
    }

}

void Computer::load(const char *path) {
    FILE *file = std::fopen(path, "r");
    if (file == NULL) {
        std::cerr << "Error al abrir el archivo " << path << "\n";
        return;
    }
    long int size;
    std::fseek(file, 0, SEEK_END);
    size = std::ftell(file);
    std::rewind(file);
    std::fread(&memory[0x200], 1, size, file);
    std::fclose(file);
    //state = STEP;
    // std::cout << "ROM:\n";
    // for (int k = 0; k < size; k++) {
    //     std::printf("%X ", memory[0x200+k]);
    // }
    // std::printf("\n");
}

u16 Computer::fetch() {
    u16 hbit = ((u16) memory[pc]) << 8;
    u16 lbit = memory[pc+1];
    u16 opcode =  hbit | lbit;
    pc += 2;
    return opcode;
}

void Computer::execute(u16 opcode) {
    step_count++;
    u8 op = (opcode & 0xF000) >> 12;
    u16 nnn = opcode & 0x0FFF;
    u8 nn = opcode & 0x00FF;
    u8 n = opcode & 0x000F;
    u8 x = (opcode & 0x0F00) >> 8;
    u8 y = (opcode & 0x00F0) >> 4;

    switch (op) {
    case 0:
        if (nn == 0xE0)
            display->clear();
        else if (nn == 0xEE) {
            if (sp > 0)
                sp--;
            pc = stack[sp];
        }
        break;
    case 1:
        pc = nnn;
        break;
    case 2:
        stack[sp++] = pc;
        pc = nnn;
        break;
    case 3:
        if (v[x] == nn)
            pc += 2;
        break;
    case 4:
        if (v[x] != nn)
            pc += 2;
        break;
    case 5:
        if (v[x] == v[y])
            pc += 2;
        break;
    case 6:
        v[x] = nn;
        break;
    case 7:
        v[x] += nn;
        break;
    case 8:
        switch (n) {
        case 0:
            v[x] = v[y];
            break;
        case 1:
            v[x] |= v[y];
            break;
        case 2:
            v[x] &= v[y];
            break;
        case 3:
            v[x] ^= v[y];
            break;
        case 4:
            {
            u16 sum = v[x] + v[y];
            if (sum > 0xFF)
                v[0xF] = 1;
            else
                v[0xF] = 0;
            v[x] = (u8)sum;
            }
            break;
        case 5:
            if (v[x] > v[y])
                v[0xF] = 1;
            else
                v[0xF] = 0;
            v[x] -= v[y];
            break;
        case 6:
            v[0xF] = v[x] & 0b1;
            v[x] >>= 1;
            break;
        case 7:
            if (v[y] > v[x])
                v[0xF] = 1;
            else
                v[0xF] = 0;
            v[x] = v[y] - v[x];
            break;
        case 0xE:
            v[0xF] = v[x] & 0b10000000;
            v[x] <<= 1;
            break;
        default:
            break;
        }
        break;
    case 9:
        if (v[x] != v[y])
            pc += 2;
        break;
    case 0xA:
        i = nnn;
        break;
    case 0xB:
        pc = nnn + v[0];
        break;
    case 0xC:
        v[x] = (u8)rand() & nn;
        break;
    case 0xD:
        display->draw(v[x], v[y], n, *this);
        break;
    case 0xE:
        {
        //u16 key = 0b1 << v[x];
        if (nn == 0x9E) {
            if (keyboard[v[x]])
                pc += 2;
        } else if (nn == 0xA1) {
            if (!(keyboard[v[x]]))
                pc += 2;
        }
        }
        break;
    case 0xF:
        switch (nn) {
        case 0x07:
            v[x] = dt;
            break;
        case 0x0A:
            {
            bool pressed = false;
            u8 key;
            for (int i = 0; i < 16; i++) {
                if (keyboard[i]) {
                    pressed = true;
                    key = i;
                    break;
                }
            }
            if (pressed)
                v[x] = key;
            else
                pc -= 2;
            }
            break;
        case 0x15:
            dt = v[x];
            break;
        case 0x18:
            st = v[x];
            break;
        case 0x1E:
            i += v[x];
            break;
        case 0x29:
            i = 0x50 + v[x];
            break;
        case 0x33:
            {
            u8 tmp = v[x];
            for (int j = 2; j >= 0; j--) {
                memory[i+j] = tmp%10;
                tmp /= 10;
            }
            }
            break;
        case 0x55:
            for (int j = 0; j <= x; j++)
                memory[i + j] = v[j];
            break;
        case 0x65:
            for (int j = 0; j <= x; j++)
                v[j] = memory[i + j];
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void Computer::reset() {
    pc = 0x200;
    i = 0;
    sp = 0;
    for (int j = 0; j < 16; j++)
        v[j] = 0;
    for (int j = 0; j < 16; j++)
        stack[j] = 0;
    for (int j = 0; j < 16; j++)
        keyboard[j] = 0;
    dt = 0;
    st = 0;
    display->clear();
    step_count = 0;
}

void Display::clear() {
    std::memset(pixels, 0, sizeof(pixels));
}

void Display::draw(u8 x, u8 y, u8 n, Computer &chip_8) {
    u8 x_coord = x%64, y_coord = y % 32;
    u8 offset = x % 8;
    chip_8.v[0xF] = 0;
    u16 pixel;
    // for (int row = 0; row < n; row++) {
    //     u16 coords = 8*(y_coord+row) + x_coord;
    //     u8 current_row = chip_8.memory[chip_8.i + row];
    //     if (y_coord + row > 32) break;
    //     for (int k = 0; k < 8; k++) {
    //         u8 current_pixel = current_row & (0b10000000 >> k);
    //         if (current_pixel && (pixels[coords] & (current_pixel >> offset)))
    //             chip_8.v[0xF] = 1;
    //         pixels[coords] ^= current_pixel >> offset;
    //         if (offset != 0 && coords + 1 < sizeof(pixels)) {
    //             if (current_pixel && (pixels[coords + 1] & (current_pixel << (k - ((k+offset)%8)))))
    //                 chip_8.v[0xF] = 1;
    //             pixels[coords + 1] ^= current_pixel << (k - ((k+offset)%8));
    //         }
    //     }
    // }
    for (int row = 0; row < n; row++) {
        pixel = chip_8.memory[chip_8.i + row];
        for (int x = 0; x < 8; x++) {
            if (pixel & (0x80u >> x)) {
                u16 coords = 64*(y_coord + row) + x_coord + x;
                if (chip_8.display->pixels[coords]) {
                    chip_8.v[0xF] = 1;
                }
                chip_8.display->pixels[coords] ^= 1;
            }
        }
    }
}
