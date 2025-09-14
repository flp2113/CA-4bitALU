#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <unordered_map>

#define BUFFER_SIZE 10
#define NUM_BUFFER 3
#define CODE_FILE "input.ula"
#define INSTRUCTIONS_FILE "output.hex"
#define INSTRUCTION_SIZE 3
#define byte unsigned char
#define ERROR -1
#define SUCCESS 0

std::unordered_map<std::string, byte> instructions_dictionary = {
    {"umL", 0x0},
    {"zeroL", 0x1},
    {"AonB", 0x2},
    {"nAonB", 0x3},
    {"AeBn", 0x4},
    {"nB", 0x5},
    {"nA", 0x6},
    {"nAxnB", 0x7},
    {"AxB", 0x8},
    {"copiaA", 0x9},
    {"copiaB", 0xA},
    {"AeB", 0xB},
    {"AenB", 0xC},
    {"nAeB", 0xD},
    {"AoB", 0xE},
    {"nAeBn", 0xF}
};

void print_binary();

int main() {
    FILE* code = nullptr;
    FILE* instructions = nullptr;

    code = fopen(CODE_FILE, "r");
    if (!code) {
        printf("Code: The code file is not opened.");
        return ERROR;
    }

    remove(INSTRUCTIONS_FILE);
    instructions = fopen(INSTRUCTIONS_FILE, "ab");
    if (!instructions) {
        printf("Instructions: The code file is not opened.");
        return ERROR;
    }

    char buffer[BUFFER_SIZE];
    fgets(buffer, BUFFER_SIZE, code);
    if (strcmp(buffer, "inicio:") == 0) {
        printf("ERROR: Inicio\n");
        return ERROR;
    }

    byte x, y, w = 0;
    while (fgets(buffer, BUFFER_SIZE, code) != NULL && strcmp(buffer, "fim.") != 0) {
        char num_buffer[NUM_BUFFER] = "";
        num_buffer[0] = buffer[2];
        if (buffer[3] == ';') {
            num_buffer[1] = '\0';
        } else {
            num_buffer[1] = buffer[3];
            num_buffer[2] = '\0';
        }

        if (strstr(buffer, "X=") != NULL) {
            x = atoi(num_buffer);
        } else if (strstr(buffer, "Y=") != NULL) {
            y = atoi(num_buffer);
        } else if (strstr(buffer, "W=") != NULL) {
            std::string instruction = "";
            for (int i = 2; i < strlen(buffer) && buffer[i] != ';'; i++) {
                instruction += buffer[i];
            }
            w = instructions_dictionary[instruction];
            byte binary[INSTRUCTION_SIZE] = {x, y, w};
            fwrite(binary, sizeof(byte), INSTRUCTION_SIZE, instructions);
        } else {
            printf("Code: ERROR in the code.");
            return ERROR;
        }
    }

    fclose(code);
    fclose(instructions);

    print_binary();

    return SUCCESS;
}

void print_binary() {
    FILE* bin = fopen(INSTRUCTIONS_FILE, "rb");
    if (!bin) {
        std::cerr << "ERROR: Could not open instructions file for reading.\n";
        return;
    }

    fseek(bin, 0, SEEK_END);
    long filesize = ftell(bin);
    rewind(bin);

    byte numbers[filesize];

    fread(numbers, sizeof(byte), filesize, bin);
    fclose(bin);

    std::cout << "BINARY CONTENT:\n";
    for (int i = 0; i < filesize; i++) {
        std::cout << std::uppercase << std::setw(1) << std::hex << static_cast<unsigned int>(numbers[i]);
        if ((i + 1) % 3 == 0) std::cout << "\n";
    }
}