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
        printf("Erro: Não foi possível abrir o arquivo de código '%s'.\n", CODE_FILE);
        return ERROR;
    }

    remove(INSTRUCTIONS_FILE);
    instructions = fopen(INSTRUCTIONS_FILE, "ab");
    if (!instructions) {
        printf("Erro: Não foi possível criar o arquivo de saída '%s'.\n", INSTRUCTIONS_FILE);
        fclose(code);
        return ERROR;
    }

    char buffer[BUFFER_SIZE];

    // Verifica a primeira linha "inicio:"
    if (fgets(buffer, BUFFER_SIZE, code) == NULL || strncmp(buffer, "inicio:", 7) != 0) {
        printf("ERRO: O código deve começar com 'inicio:'\n");
        fclose(code);
        fclose(instructions);
        return ERROR;
    }

    byte x = 0, y = 0, w = 0;
    
    // Loop principal para ler o arquivo
    while (fgets(buffer, BUFFER_SIZE, code) != NULL && strncmp(buffer, "fim.", 4) != 0) {
        // Ignora linhas vazias
        if(buffer[0] == '\n' || buffer[0] == '\r') continue;

        if (strstr(buffer, "X=") != NULL) {
            x = atoi(&buffer[2]);
        } else if (strstr(buffer, "Y=") != NULL) {
            y = atoi(&buffer[2]);
        } else if (strstr(buffer, "W=") != NULL) {
            std::string instruction = "";
            for (int i = 2; i < strlen(buffer) && buffer[i] != ';'; i++) {
                instruction += buffer[i];
            }

            // Verificação segura da instrução
            if (instructions_dictionary.count(instruction)) {
                w = instructions_dictionary[instruction];
            } else {
                printf("ERRO: Instrução desconhecida -> %s\n", instruction.c_str());
                fclose(code);
                fclose(instructions);
                return ERROR;
            }
            
            byte binary[INSTRUCTION_SIZE] = {x, y, w};
            fwrite(binary, sizeof(byte), INSTRUCTION_SIZE, instructions);
        } else {
            printf("Code: ERRO na sintaxe do código na linha: %s", buffer);
            fclose(code);
            fclose(instructions);
            return ERROR;
        }
    }

    fclose(code);
    fclose(instructions);

    printf("Arquivo '%s' gerado com sucesso!\n\n", INSTRUCTIONS_FILE);
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
