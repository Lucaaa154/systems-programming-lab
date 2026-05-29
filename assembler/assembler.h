/*
*  first and second pass header file
*/

#ifndef assembler_H
#define assembler_H
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* defining macros */
#define ERROR -1
#define MAX_GUIDANCE_NUM 16383 /* the max num that can be represented with 15 bits */
#define MAX_INSTRUCTION_NUM 2047 /* the max num that can be represented with 15 bits */
#define MACR_DEF "macr"
#define MACR_END "endmacr"
#define MAX_LINE_LEN 83 /* 80 chars + "new line" + null char + one more char so we can check length validity */
#define MAX_LABEL_LEN 31
#define MEM_IMG_SIZE 4096 /* max memory*/
#define MEM_FIRST_INDEX 100 /* the first memory word */
#define TARGET_MOVE 3 /* for bitwise, to move 2 bits to "target operands" */
#define OP_MOVE 11 /* to move 11 bits to "opcode" */
#define SOURCE_MOVE 7 /* to move 9 bits to "source operand" */
#define SOURCE_REGISTER_MOVE 6 /* moving 6 bit to source register */
#define TARGET_REGISTER_MOVE 3 /* moving 3 bit to target register */
#define HIGHEST_NUM 16383 /* the highest signed number that we can represent in 15 bit */
#define BIT_ONE 1 /* E bit on in ARE or imidiate addressing method */
#define BIT_TWO 2 /* R bit on in ARE or direct addressing method */
#define BIT_THREE 4 /* A bit on in ARE or indirect addressing  method */
#define BIT_FOUR 8 /* register addressing method */

/* global instructions list */
extern char instructions_list[16][5];
/* global data list */
extern char guidance_list[4][8];


/* macro attributes */
struct macro_att
{ 
    char macro_name [MAX_LINE_LEN]; /* macro name */
    struct macro_att *next; /* points to the next macro */
    int lines_number; /* number of the macro lines */
    char **lines; /* macro lines */
};

/* 
   struct of a word of instruction that is divided to appropriate fields. 
*/
struct instruction
{
  unsigned int ARE:3;/* A.R.E code */
  unsigned int target:4;/* target operand */
  unsigned int source:4;/* source operand */
  unsigned int op:4;/* opcode */
};

/* 
   symbol_table struct
   dec_num: decimal number of the line of their declaration.
   note: entry labels will be in the range (5100-9196)and extern label will be in the range > 10000
   after that it will be reduced to the appropriate address.
*/
struct label_node 
{
    char label[MAX_LABEL_LEN]; /* label name */
    int dec_num; /* label address */
    char label_type; /* ':' for a label with no .entry and with no .extern, 'n' for entry, 'x' for extern */
    struct label_node* next; /* next label node */
};

/*** functions declaration ***/

/* 
   function to handle every file in agrv[i]
*/
void files_handle(char *file);

/*
   this function expands all the macros and checks its definition validity.
*/
int macro_expansion(FILE *fp, FILE *wr_fp, char *file_name, struct macro_att **table_head);

/* convert the name of the file to name of the file + the appropriate ending */
void convert_ending(char *file_name, int step);

/* to free the macros table */
void free_macros(struct macro_att *macros_table);

/*
   checks if the received word is a macro name that exists in the macros table.
*/
struct macro_att* defined_macro(char *word, struct macro_att *table);

/*
   This function exapands the reveived macro to the received file.
*/
void expand_macro(FILE *fp, struct macro_att *mcro_pointer);

/*
   The function checks if the received string is a name of instruction.
*/
int instruction_validity(char *word);

/*
   The function checks if the received string is one of the guidance types.
*/
int guidance_validity(char *word);

/* The function checks if the reveived string is a label, if its operand so the def_bit should be 0
   if its a label definition so def_bit should be 1
*/
int label_validity (char *word,int def_bit,int lineNum);

/* checking if the received string is a number. with signs and without signs (+-) */
int num_validity(char *word, int lineNum, char t);

/* function returns a number using 2's complement, if its a positive number so it will return the same number */
int twos_complement_value(int value);

/* function to check if the received label has declared before and return pointer to it, if didnt declared
   so the function will return NULL.
*/
struct label_node* defined_label(struct label_node **head, char *label);

/* adding label to the received label list taking into account the external and entry labels */
int add_label(struct label_node **head, char* label, int dec_num, int line_num,char c);

/* The function to add a macro to the macros table */
int add_macro(FILE *pf, char *file_name, struct macro_att **table_head, struct macro_att **table_tail, char *line, int *lineNum);

/* first_pass function that runs after the preassembler */
int first_pass (FILE *file_ptr, char *filename, struct macro_att *macros_table);

/* checking if the type of the line and returns 'i' for instruction 'g' for guidance 'l' for label */
char line_type(char *word, int lineNum);

/* checks if the received string is an instruction */
int instruction_word(char *inst_name,struct label_node **head, unsigned int buffer_inst[MEM_IMG_SIZE], int *IC, int *DC, int lineNum);

/* function to get and handle the instruction operands */
int get_operands(struct label_node **head,  unsigned int buffer_inst[MEM_IMG_SIZE], int *IC, int *DC, int lineNum, struct instruction *inst, char *word);

/* function to get the second operand and also encode a part of it */
int get_sec_operand(char **operand2 , unsigned int buffer_inst[MEM_IMG_SIZE], int *IC, int *DC, int lineNum, struct instruction *inst);

/* encoding the opernads with binary base, instead of labels that only encode a part of it (A.R.E) */
int binary_encode(unsigned int buffer_inst[MEM_IMG_SIZE], struct label_node **head, struct instruction *inst, char *operand1, char *operand2, int *IC, int *DC, int lineNum);

/* function to check if the received string is one of the registers r0-r7 */
int register_validity(char *word);

/* function to insert the appropriate addresing method  */
void address_method(struct instruction *inst, int opcode, int address);

/* checks if the received string is an instruction */
int guidance(unsigned int buffer_data[MEM_IMG_SIZE], unsigned int buffer_inst[MEM_IMG_SIZE], int *IC, int *DC, int lineNum, char *word, struct label_node** head, struct macro_att *macros_table) ;

/* function to check(using functions) the .data parameters. also it gets and encode these parameters */
int data_params_validity(char param[MAX_LINE_LEN], int lineNum);

/* function to check(using functions) the .string parameters. also it gets and encode these parameters */
int string_params_validity(char param[MAX_LINE_LEN], int lineNum);

/* adding the ic number at the end of the first pass */
void update_label_list(struct label_node *head, int IC);

/* function that runs after completing the first pass succecfully, this function also calling the output printing function
   only when its completed succecfully 
*/
int second_pass (FILE *file_ptr, char *filename, unsigned int buffer_data[MEM_IMG_SIZE] , unsigned int buffer_inst[MEM_IMG_SIZE], struct label_node **head, int IC, int DC);

/* printing the instruction and guidance with octal base (base 8) */
void octal_print(char *filename, unsigned int buffer_data[MEM_IMG_SIZE], unsigned int buffer_inst[MEM_IMG_SIZE], int IC, int DC);

/* function to print the external labels and entry labels to files with appropriate ending */
void label_print (struct label_node **head, char *filename);

#endif
