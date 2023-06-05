// CP1521 22T3 Assignment 2: mipsc -- a MIPS simulator
// starting point code v1.0 - 24/10/22
// Pyae Sone Oo <z5271704>

// PUT YOUR HEADER COMMENT HERE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// ADD ANY ADDITIONAL #include HERE
#include <math.h>

#define MAX_LINE_LENGTH 256
#define INSTRUCTIONS_GROW 64

// ADD ANY ADDITIONAL #defines HERE
#define NUM_OF_REGISTERS 32
#define INSTRUCTION_LENGTH 32
#define OP_SIZE 6
#define FUN_SIZE 6
#define R_SIZE 5
#define IM_SIZE 16

void execute_instructions(uint32_t n_instructions,
                          uint32_t instructions[],
                          int trace_mode);
char *process_arguments(int argc, char *argv[], int *trace_mode);
uint32_t *read_instructions(char *filename, uint32_t *n_instructions_p);
uint32_t *instructions_realloc(uint32_t *instructions, uint32_t n_instructions);

// ADD ANY ADDITIONAL FUNCTION PROTOTYPES HERE
uint32_t get_nth_bit(uint32_t value, int n);
void convert_to_bits(uint32_t value, int how_many_bits,int *bits_string);
uint32_t get_Code(int *code_segment, int size);

// YOU DO NOT NEED TO CHANGE MAIN
// but you can if you really want to
int main(int argc, char *argv[]) {
    int trace_mode;
    char *filename = process_arguments(argc, argv, &trace_mode);

    uint32_t n_instructions;
    uint32_t *instructions = read_instructions(filename, &n_instructions);

    execute_instructions(n_instructions, instructions, trace_mode);

    free(instructions);
    return 0;
}


// simulate execution of  instruction codes in  instructions array
// output from syscall instruction & any error messages are printed
//
// if trace_mode != 0:
//     information is printed about each instruction as it executed
//
// execution stops if it reaches the end of the array
void execute_instructions(uint32_t n_instructions,
                          uint32_t instructions[],
                          int trace_mode) {

    // REPLACE THIS FUNCTION WITH YOUR OWN IMPLEMENTATION

    int bits_string[INSTRUCTION_LENGTH] = {0};
    int32_t mips_registers[NUM_OF_REGISTERS] = {0};

    int32_t hi = 0;     //$hi
    int32_t lo = 0;     //$lo
    int64_t hilo = 0;   //$hilo

    char *instruction = "\0";

    for (uint32_t pc = 0; pc < n_instructions; pc++) {
        // start implementation
        convert_to_bits(instructions[pc], INSTRUCTION_LENGTH, bits_string);

        uint32_t opCode = get_Code(&bits_string[0], OP_SIZE);
        uint32_t funCode = get_Code(&bits_string[26], FUN_SIZE);
        
        if(opCode == 0){
            if(funCode == 12){
                instruction = "syscall";
            }
            else if(funCode == 16){
               instruction = "mfhi";
            }
            else if(funCode == 18){
                instruction = "mflo";
            }
            else if(funCode == 24){
                instruction = "mult";
            }
            else if(funCode == 26){
                instruction = "div";
            }
            else if(funCode == 32){
                instruction = "add";
            }
            else if(funCode == 34){
                instruction = "sub";
            }
            else if(funCode == 42){
               instruction = "slt";
            }
        }
        else if(opCode == 4){
            instruction = "beq";
        }
        else if(opCode == 5){
            instruction = "bne";
        }
        else if(opCode == 8){
            instruction = "addi";
        }
        else if(opCode == 13){
            instruction = "ori";
        }
        else if(opCode == 15){
            instruction = "lui";
        }
        else if(opCode == 28){
           instruction = "mul";
        }
        
       if (trace_mode) {
            printf("%u: 0x%08X %s", pc, instructions[pc], instruction);
       }

       
        if(strcmp(instruction, "syscall") == 0){
            if (trace_mode) {
                if (mips_registers[2] == 1) {
                    printf("\n>>> syscall %u\n", mips_registers[2]);
                    printf("<<< %u\n", mips_registers[4]);
                } else if (mips_registers[2] == 10) {
                    printf("\n>>> syscall %u\n", mips_registers[2]);
                    exit(0);
                } else if (mips_registers[2] == 11) {
                    printf("\n>>> syscall %u\n", mips_registers[2]);
                    printf("<<< %c\n", mips_registers[4]);
                } else {
                    printf("\n>>> syscall %u\n", mips_registers[2]);
                    fprintf(stderr, "Unknown system call: %u\n", mips_registers[2]);
                    exit(1);
                }
            } else if (trace_mode == 0) {
                if (mips_registers[2] == 1) {
                    printf("%u", mips_registers[4]);
                } else if (mips_registers[2] == 10) {
                    exit(0);
                } else if (mips_registers[2] == 11) {
                    printf("%c", mips_registers[4]);
                } else {
                    fprintf(stderr, "Unknown system call: %u\n", mips_registers[2]);
                    exit(1);
                }
            }
        }
        else if (strcmp(instruction, "mfhi") == 0){ //mfhi
            int32_t rd = get_Code(&bits_string[16],R_SIZE);
            mips_registers[rd] = hi;
            if(trace_mode){
                printf(" $%u\n", rd); 
                printf(">>> $%u = %d\n", rd, mips_registers[rd]);
            }
        }
        else if (strcmp(instruction, "mflo") == 0){ //mflo
            int32_t rd = get_Code(&bits_string[16],R_SIZE);
            mips_registers[rd] = lo;
            if(trace_mode){
                printf(" $%u\n", rd);
                printf(">>> $%u = %d\n", rd, mips_registers[rd]);
            }
            if(rd == 0){
                mips_registers[rd] = 0;
            }
        }
        else if (strcmp(instruction, "mult") == 0){ //mult
            uint32_t rs = get_Code(&bits_string[6], R_SIZE);
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);
            hilo = (uint64_t)mips_registers[rs] * (uint64_t)mips_registers[rt];
            hi = hilo >> 32;
            lo = hilo;
            if(trace_mode){ 
                printf(" $%u, $%u\n", rs, rt); 
                printf(">>> HI = %d\n", hi);
                printf(">>> LO = %d\n", lo);
            }
        }
        else if (strcmp(instruction, "div") == 0){ //div
            uint32_t rs = get_Code(&bits_string[6], R_SIZE);
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);

            lo = mips_registers[rs] / mips_registers[rt];
            hi = mips_registers[rs] % mips_registers[rt];
            hilo |= (uint64_t)lo;
            hilo |= ((uint64_t)hi) << 32;
            if(trace_mode){
                printf("  $%u, $%u\n", rs, rt);
                printf(">>> HI = %d\n", hi);
                printf(">>> LO = %d\n", lo);
            }
        }
        else if (strcmp(instruction, "add") == 0){ //add
            uint32_t rs = get_Code(&bits_string[6], R_SIZE);
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);
            uint32_t rd = get_Code(&bits_string[16],R_SIZE);
            mips_registers[rd] = mips_registers[rs] + mips_registers[rt];
            if(trace_mode){
                printf("  $%u, $%u, $%u\n", rd, rs, rt);
                printf(">>> $%u = %d\n", rd, mips_registers[rd]);
            }  
            if(rd == 0){
                mips_registers[rd] = 0;
            }            
        }
        else if (strcmp(instruction, "sub") == 0){ //sub
            uint32_t rs = get_Code(&bits_string[6], R_SIZE);
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);
            uint32_t rd = get_Code(&bits_string[16],R_SIZE);
            mips_registers[rd] = mips_registers[rs] - mips_registers[rt];
            if(trace_mode){
                printf("  $%u, $%u, $%u\n", rd, rs, rt);
                printf(">>> $%u = %d\n", rd, mips_registers[rd]); 
            }
            if(rd == 0){
                mips_registers[rd] = 0;
            } 
        }
        else if(strcmp(instruction, "slt") == 0){  //slt
            uint32_t rs = get_Code(&bits_string[6], R_SIZE);
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);
            uint32_t rd = get_Code(&bits_string[16],R_SIZE);
                    
            if(mips_registers[rs] < mips_registers[rt]){
                mips_registers[rd] = 1;
            }
            else{
                mips_registers[rd] = 0;
            }

            if(trace_mode){
                printf("  $%u, $%u, $%u\n", rd, rs, rt);
                printf(">>> $%u = %d\n", rd, mips_registers[rd]);
            }
            if(rd == 0){
                mips_registers[rd] = 0;
            }
        } 
        else if (strcmp(instruction, "beq") == 0){ // beq
            uint32_t rs = get_Code(&bits_string[6], R_SIZE);
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);
            int16_t imValue = get_Code(&bits_string[16], IM_SIZE);
            
            if (mips_registers[rs] == mips_registers[rt]) {
                pc = pc + (imValue - 1);
            }
            if (trace_mode) {
                printf("  $%u, $%u, %d\n", rs, rt, imValue);
                if (mips_registers[rs] == mips_registers[rt]) {
                    printf(">>> branch taken to PC = %u\n", pc + 1);
                } else{
                    printf(">>> branch not taken\n");
                }
            }
        }
        else if (strcmp(instruction, "bne") == 0){ // bne
            uint32_t rs = get_Code(&bits_string[6], R_SIZE);
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);
            int16_t imValue = get_Code(&bits_string[16], IM_SIZE);
           
            if (mips_registers[rs] != mips_registers[rt]) {
                pc = pc + (imValue - 1);
            }
            if (trace_mode) {
                printf("  $%u, $%u, %d\n", rs, rt, imValue);
                if (mips_registers[rs] != mips_registers[rt]) {
                    printf(">>> branch taken to PC = %u\n", pc + 1);
                } else {
                    printf(">>> branch not taken\n");
                }
            }
        }
        else if (strcmp(instruction, "addi") == 0){   //addi
            uint32_t rs = get_Code(&bits_string[6], R_SIZE);
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);
            int32_t imValue = get_Code(&bits_string[16], IM_SIZE);
            if(bits_string[16] == 1){
                imValue = 0xFFFF0000 | imValue; 
            }
            
            mips_registers[rt] = mips_registers[rs] + (imValue);
            if(trace_mode){
                printf(" $%u, $%u, %d\n", rt, rs, imValue);
                printf(">>> $%u = %d\n", rt, mips_registers[rt]);
            }
            if(rt == 0){
                    mips_registers[rt] = 0;
            }
        }
        else if (strcmp(instruction, "ori") == 0){ //ori
            uint32_t rs = get_Code(&bits_string[6], R_SIZE);
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);
            int32_t imValue = get_Code(&bits_string[16], IM_SIZE);
            mips_registers[rt] = mips_registers[rs] | (uint32_t)imValue;
            if(bits_string[16] == 1){
                imValue = 0xFFFF0000 | imValue; 
            }
            if(trace_mode){
                printf("  $%u, $%u, %d\n", rt, rs, imValue);
                printf(">>> $%u = %d\n", rt, mips_registers[rt]);
            }
            if(rt == 0){
                    mips_registers[rt] = 0;
            }
        }
        else if (strcmp(instruction, "lui") == 0){ //lui
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);
            int32_t imValue = get_Code(&bits_string[16], IM_SIZE);
            if(bits_string[16] == 1){
                 imValue = 0xFFFF0000 | imValue; 
            }
            mips_registers[rt] = imValue << 16;

            if(trace_mode){
                printf("  $%u, %d\n", rt, imValue);
                printf(">>> $%u = %d\n", rt, mips_registers[rt]);   
            }
            if(rt == 0){
                    mips_registers[rt] = 0;
            }
        }
        else if (strcmp(instruction, "mul") == 0){ //mul
            uint32_t rs = get_Code(&bits_string[6], R_SIZE);
            uint32_t rt = get_Code(&bits_string[11], R_SIZE);
            uint32_t rd = get_Code(&bits_string[16], R_SIZE);
            mips_registers[rd] = mips_registers[rs] * mips_registers[rt];
            if(trace_mode){
                printf("  $%u, $%u, $%u\n", rd, rs, rt);
                printf(">>> $%u = %d\n", rd, mips_registers[rd]);
            }
            if(rd == 0){
                    mips_registers[rd] = 0;
            }
        }
        else{
            if(trace_mode){
                fprintf(stderr, "\nInvalid instruction 0x%08X at PC = %u\n", instructions[pc], pc);
                exit(1);
            }
        }
    }
}

// ADD YOUR FUNCTIONS HERE
uint32_t get_nth_bit(uint32_t value, int n) { 
    uint64_t shifted_value = value >> n; 
    int bit = shifted_value & 1; 
    return bit;
}

void convert_to_bits(uint32_t value, int how_many_bits, int *bits_string){   
    for (int i = how_many_bits - 1; i >= 0; i--) { 
        uint32_t bit = get_nth_bit(value, i); 
        *bits_string = bit;
        bits_string++;
    }
}

uint32_t get_Code(int* code_segment, int size){
    uint32_t code = 0;
    int power = 0;
    for(int i = size-1; i >= 0; i--){
        code += (uint32_t)(code_segment[i] * pow(2,power)); 
        power++;
    }
   
    return code;
}

// DO NOT CHANGE ANY CODE BELOW HERE


// check_arguments is given command-line arguments
// it sets *trace_mode to 0 if -r is specified
//         *trace_mode is set to 1 otherwise
// the filename specified in command-line arguments is returned
char *process_arguments(int argc, char *argv[], int *trace_mode) {
    if (
        argc < 2 ||
        argc > 3 ||
        (argc == 2 && strcmp(argv[1], "-r") == 0) ||
        (argc == 3 && strcmp(argv[1], "-r") != 0)
    ) {
        fprintf(stderr, "Usage: %s [-r] <file>\n", argv[0]);
        exit(1);
    }
    *trace_mode = (argc == 2);
    return argv[argc - 1];
}


// read hexadecimal numbers from filename one per line
// numbers are return in a malloc'ed array
// *n_instructions is set to size of the array
uint32_t *read_instructions(char *filename, uint32_t *n_instructions_p) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror(filename);
        exit(1);
    }

    uint32_t *instructions = NULL;
    uint32_t n_instructions = 0;
    char line[MAX_LINE_LENGTH + 1];
    while (fgets(line, sizeof line, f) != NULL) {

        // grow instructions array in steps of INSTRUCTIONS_GROW elements
        if (n_instructions % INSTRUCTIONS_GROW == 0) {
            instructions = instructions_realloc(instructions, n_instructions + INSTRUCTIONS_GROW);
        }

        char *endptr;
        instructions[n_instructions] = (uint32_t)strtoul(line, &endptr, 16);
        if (*endptr != '\n' && *endptr != '\r' && *endptr != '\0') {
            fprintf(stderr, "line %d: invalid hexadecimal number: %s",
                    n_instructions + 1, line);
            exit(1);
        }
        if (instructions[n_instructions] != strtoul(line, &endptr, 16)) {
            fprintf(stderr, "line %d: number too large: %s",
                    n_instructions + 1, line);
            exit(1);
        }
        n_instructions++;
    }
    fclose(f);
    *n_instructions_p = n_instructions;
    // shrink instructions array to correct size
    instructions = instructions_realloc(instructions, n_instructions);
    return instructions;
}


// instructions_realloc is wrapper for realloc
// it calls realloc to grow/shrink the instructions array
// to the specified size
// it exits if realloc fails
// otherwise it returns the new instructions array
uint32_t *instructions_realloc(uint32_t *instructions, uint32_t n_instructions) {
    instructions = (uint32_t *)realloc(instructions, n_instructions * sizeof *instructions);
    if (instructions == NULL) {
        fprintf(stderr, "out of memory");
        exit(1);
    }
    return instructions;
}
