// SERIAL CONSTS
const int BAUD = 9600;

// LEDS CONSTS
const int LED_RED    = 13;
const int LED_YELLOW = 12;
const int LED_GREEN  = 11;
const int LED_BLUE   = 10;

// INSTRUCTIONS CONSTS
const int INSTRUCTION_X_INDEX  = 0;
const int INSTRUCTION_Y_INDEX  = 1;
const int INSTRUCTION_OP_INDEX = 2;
typedef enum INSTRUCTION {
  umL     = 0x0,
  zeroL   = 0x1,
  AonB    = 0x2,
  nAonB   = 0x3,
  AeBn    = 0x4,
  nB      = 0x5,
  nA      = 0x6,
  nAxnB   = 0x7,
  AxB     = 0x8,
  copiaA  = 0x9,
  copiaB  = 0xA,
  AeB     = 0xB,
  AenB    = 0xC,
  nAeB    = 0xD,
  AoB     = 0xE,
  nAeBn   = 0xF
} INSTRUCTION;

// CPU MEMORY CONSTS
const int MEMORY_SIZE = 100;
const String EMPTY_CELL = "";
const int PROGRAM_STARTING_INDEX = 4;
const int CP_INDEX = 0;
const int W_INDEX = 1;
const int X_INDEX = 2;
const int Y_INDEX = 3;

// CPU MEMORY
String cpu_memory[MEMORY_SIZE];

// FUNCTIONS
void initialize_memory();
void print_memory();
String int_to_binary_string(int value, int bits = 4);
int hexstring_to_int(String hex_string);

void setup() {
  // SERIAL SETUP
  Serial.begin(BAUD);
  
  // LEDS SETUP
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  
  // CPU MEMORY SETUP
  initialize_memory();
  cpu_memory[CP_INDEX] = int_to_hexstring(PROGRAM_STARTING_INDEX);
}

bool program_running = false;
void loop() {
  // MEMORY SETUP
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil(' ');
    input.trim();

    if (input.equalsIgnoreCase("start")) {
      if (cpu_memory[PROGRAM_STARTING_INDEX] == EMPTY_CELL) {
        Serial.println("No program in memory. Load instructions first.");
        return;
      }
      
      program_running = true;
      Serial.println("Starting program execution...");
      cpu_memory[CP_INDEX] = int_to_hexstring(PROGRAM_STARTING_INDEX);
      print_memory(); 
      delay(1000);
      return;
    }

    int index = hexstring_to_int(cpu_memory[CP_INDEX]);
    if (index < MEMORY_SIZE) {
        cpu_memory[index] = input;
        cpu_memory[CP_INDEX] = int_to_hexstring(++index);
        Serial.println("Instruction '" + input + "' loaded at address " + (index - 1));
    } else {
        Serial.println("Memory is full.");
    }
  }

  // PROGRAM EXECUTION
  if (!program_running) {
    return;
  }

  int current_address = hexstring_to_int(cpu_memory[CP_INDEX]);
  String current_instruction = cpu_memory[current_address];

  if (current_instruction == EMPTY_CELL || current_address >= MEMORY_SIZE) {
    program_running = false;
    Serial.println("Program finished.");
    cpu_memory[CP_INDEX] = int_to_hexstring(PROGRAM_STARTING_INDEX); 
    return;
  }

  print_memory();
  
  int x, y, op;
  x  = hexstring_to_int(current_instruction.charAt(INSTRUCTION_X_INDEX));
  y  = hexstring_to_int(current_instruction.charAt(INSTRUCTION_Y_INDEX));
  op = hexstring_to_int(current_instruction.charAt(INSTRUCTION_OP_INDEX));

  int result;
  switch (op) {
    case umL:   result = 1;              break;
    case zeroL: result = 0;              break;
    case AonB:  result = (x | (~y));     break;
    case nAonB: result = ((~x) | (~y));  break;
    case AeBn:  result = (~(x & y));     break; 
    case nB:    result = (~y);           break;
    case nA:    result = (~x);           break;
    case nAxnB: result = ((~x) ^ (~y));  break;
    case AxB:   result = (x ^ y);        break;
    case copiaA:result = x;              break;
    case copiaB:result = y;              break;
    case AeB:   result = (x & y);        break;
    case AenB:  result = (x & (~y));     break;
    case nAeB:  result = ((~x) & y);     break;
    case AoB:   result = (x | y);        break;
    case nAeBn: result = ((~x) & (~y));  break; 
  }

  result &= 0xF; 
  String binary_string_result = int_to_binary_string(result, 4);

  cpu_memory[X_INDEX] = current_instruction.charAt(INSTRUCTION_X_INDEX);
  cpu_memory[Y_INDEX] = current_instruction.charAt(INSTRUCTION_Y_INDEX);
  cpu_memory[W_INDEX] = binary_string_result;

  digitalWrite(LED_RED,    binary_string_result[0] - '0');
  digitalWrite(LED_YELLOW, binary_string_result[1] - '0');
  digitalWrite(LED_GREEN,  binary_string_result[2] - '0');
  digitalWrite(LED_BLUE,   binary_string_result[3] - '0');

  cpu_memory[CP_INDEX] = int_to_hexstring(current_address + 1);

  delay(1000);
}

String int_to_binary_string(int value, int bits = 4) {
  String result = "";
  for (int i = bits - 1; i >= 0; i--) {
    result += ((value >> i) & 1) ? '1' : '0';
  }
  return result;
}

int hexstring_to_int(String hex_string) {
  return (int) strtol(hex_string.c_str(), NULL, 16);
}

int hexstring_to_int(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
  if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
  return -1;
}

String int_to_hexstring(int value) {
  char buffer[4];
  sprintf(buffer, "%03X", value);
  return String(buffer);
}

void initialize_memory() {
  for (int i = 0; i < MEMORY_SIZE; i++) {
	cpu_memory[i] = EMPTY_CELL;
  }
}

void print_memory() {
  Serial.print("[REGISTERS] CP: " + cpu_memory[CP_INDEX]);
  Serial.print(" | W: " + cpu_memory[W_INDEX]);
  Serial.print(" | X: " + cpu_memory[X_INDEX]);
  Serial.println(" | Y: " + cpu_memory[Y_INDEX]);

  Serial.println("[MEMORY DUMP]");

  int active_address = -1;
  if (program_running) {
    active_address = hexstring_to_int(cpu_memory[CP_INDEX]);
  }

  const int cells_per_line = 8;
  for (int i = 0; i < MEMORY_SIZE; i++) {
    if (i % cells_per_line == 0) {
      if (i > 0) {
        Serial.println();
      }
      
      Serial.print(int_to_hexstring(i) + ": ");
    }

    String content = cpu_memory[i];
    String output;

    if (i == active_address) {
      output = ">" + content + "<";
    } else {
      output = " " + content + " ";
    }
    
    Serial.print(output);
    for (int p = 0; p < (6 - output.length()); p++) {
        Serial.print(" ");
    }

    if (i > PROGRAM_STARTING_INDEX + cells_per_line && content == EMPTY_CELL) {
        if (cpu_memory[i-1] == EMPTY_CELL && cpu_memory[i-2] == EMPTY_CELL) {
            Serial.println(" ...");
            break;
        }
    }
  }
  Serial.println("\n-------------------------------------------");
}







