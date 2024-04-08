#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Constants for error examination.*/
#define VALID 0
#define ERROR 1

/*Each command mila takes a spacec of 4.*/
#define COMMAND_MILA_SIZE 4

/*For valid conditional branching function.*/
#define BNE_OPCODE 15
#define BGT_OPCODE 18

/*2^25-1 the given memory limit.*/
#define MEMORY_LIMIT 33554431

/*Constants of the different input line types.*/
#define IS_EMPTY_LINE 10
#define IS_COMMENT 20
#define IS_ENTRY 30
#define IS_EXTERN 40
#define IS_COMMAND 50
#define IS_INSTRUCTION 60
#define UNDEFINED 70

/*Constants marking the columns of the type, funct and opcode of the command table matrix respectively.*/
#define TYPE 1
#define FUNCT 2
#define OPCODE 3

/*Constants for the different data types.*/
#define DH 0
#define DW 1
#define DB 2
#define ASCIZ 3

/*Constants for the symbol table, data table, binary machine code and check later list, respectively.*/
#define SYMBOL 100
#define DATA 200
#define BMC 300
#define STANDBY 400

/*Constants for the given max input length and label length not including the null character. */
#define MAX_INPUT_LENGTH 80
#define MAX_LABEL_LENGTH 31

/*Constants marking:
Total columns in command matrix
beginning of the J type commands
final command
total instructions
position of .entry within the instruction array.
position of the .extern within the instruction array.*/
#define TOTAL_COMMAND_COLS 4
#define JCOMMANDSTART 23
#define STOPCOMMAND 26
#define INSTRUCTIONS 6
#define ENTRY_INSTRCTION 4 
#define EXTERN_INSTRUCTION 5
#define ICOMMANDSTART 8

/*Constants for total registers and base 10 (when converting to decimal)*/
#define REGISTER31 31
#define BASE10 10

/*In data instruction:
DW type takes 32 bits
DH type takes 16 bits
DB type takes 8 bits
ASCIZ type takes 8 bits
BMC jumps in 4 bytes
DW data type jumps in 4 bytes
DH data type jumps 2 bytes*/
#define BITS32 32
#define BITS16 16
#define BITS8 8
#define BMC_PRINT_FACTOR 4
#define DW_ADDRESS_FACTOR 4 
#define DH_ADDRESS_FACTOR 2

/*Constants when it comes to the final translation:
postfix4 '.ob\0'
postfix5 '.ent\0' and '.ext\0'
each output line contains at most 8 hexadecimals
a counter to generate a new line character for when 4 couples of hexadecimals have been printed.*/
#define POSTFIX4 4
#define POSTFIX5 5
#define NEW_LINE_COUNTER 4

/*Command related constants.*/
#define OPCODE_FIELD 6
#define FUNCT_FIELD 5
#define RS_FIELD 5
#define RT_FIELD 5
#define RT_FIELD 5
#define NOT_IN_USE 6
#define IMMED_FIELD 16
#define ADDRESS_FIELD 25


/*Pointers for the 4 different linked lists in this project, their definitions and helper structs/union.*/
typedef struct SymbolList * symbol_ptr;
typedef struct CodeList * bmc_ptr;
typedef struct DataList * data_ptr;
typedef struct CheckLaterList * checklater_ptr;

/*Struct representing command of type R.*/
typedef struct 
{

   unsigned int not_in_use : 6;
   unsigned int funct : 5;
   unsigned int rd : 5;
   unsigned int rt : 5;
   unsigned int rs : 5;
   unsigned int opcode : 6;

}commandR;

/*Struct representing command of type I.*/
typedef struct 
{

   int immed : 16;
   unsigned int rt : 5;
   unsigned int rs : 5;
   unsigned int opcode : 6;

}commandI;

/*Struct representing command of type J.*/
typedef struct 
{

   int address : 25;
   unsigned int reg : 1;
   unsigned int opcode : 6;

}commandJ;

/*Helper command union.*/
typedef union 
{
   char m[4];
   commandR R;
   commandI I;
   commandJ J;

}Mila;

/*Symbol table.*/
struct SymbolList 
{
   char *symbol;
   int address;
   char *type;
   char *optional_type;
   symbol_ptr next;
}; 

/*Binary machine code.*/
struct CodeList
{
   char *var;
   char type;
   int addr;
   Mila mila;
   bmc_ptr next;
};

/*Data table.*/
struct DataList 
{
   char *type;
   int value;
   int address;
   data_ptr next;
};

/*Stand by list.*/
struct CheckLaterList
{
   char *toCheck;
   int line;
   checklater_ptr next;
};

/*Function declarations, their file location and order.*/

/*General_Functions.c*/
int valid_File_Name(char *file);
void remove_space_tab(char *input);
int inputType(char *input, int line, int *flag, char *cTable[][TOTAL_COMMAND_COLS], char **iTable);
void extract_Label(char *label, char *input, int *errs, int line);
void print_Bits(int number,int bits);
void print_List(void *list, int t);
void free_List(void **list, int t);
void reset_Parameters(FILE **fp, bmc_ptr *bmc, symbol_ptr *symbols, data_ptr *data, checklater_ptr *standby, char **output_file, int *errs, int *IC, int *DC);

/*The_Passages.c*/
void the_Passages(int argc, char **argv);

/*First_Passage.c*/
void firstPassage(FILE *fp, bmc_ptr *bmc, symbol_ptr *symbols, data_ptr *data, checklater_ptr *standby, int *errs, int *IC, int *DC, char **iTable, char *cTable[][TOTAL_COMMAND_COLS]);

/*Second_Passage.c*/
void secondPassage(FILE *fp, bmc_ptr *bmc, symbol_ptr *symbols, data_ptr *data, int *errs, int IC, int DC, char **iTable, char *cTable[][TOTAL_COMMAND_COLS]);

/*SecondPassage_Setup.c*/
void secondpassage_Setup(FILE *fp, bmc_ptr *bmc, symbol_ptr *symbols, data_ptr *data, checklater_ptr *standby, int *errs, int IC, int DC, char **iTable, char *cTable[][TOTAL_COMMAND_COLS]);

/*Final_Translation.c*/
void final_Translation(bmc_ptr b, data_ptr d, symbol_ptr s, char *output_file, int ic, int dc);

/*Data_Retriever.c*/
void retrieve(char *content, int table_idx, int *addr, data_ptr *d);
void add2SymbolList(char *sym, int line_type, int addr, symbol_ptr *s);
void update_Data_Address(data_ptr *data, int addr);

/*Command_Converter.c*/
void convert(char *ty, char *ft, char *oc, char *content, int addr, bmc_ptr *bmc);
void put_On_Stand_By(checklater_ptr *standby, char *temp, int line);
void update_Symbol_Address(symbol_ptr *symbols, int addr);
void update_Symbols_Entry(char *sym, symbol_ptr *s, int *errs);
void update_BMC_Adresses(symbol_ptr s, bmc_ptr *b);

/*FirstPassage_Error_Examiner.c*/
int legal_Label(char *cTable[][TOTAL_COMMAND_COLS], char **iTable, char *input, int length, int line);
int potential_Errors_For_R(char *input, char *type, int length, int line);
int potential_Errors_For_I(checklater_ptr *standby, char *input, char *type, int length, int line);
int potential_Errors_For_J(checklater_ptr *standby, char **iTable, char *cTable[][TOTAL_COMMAND_COLS], char *input, char *opcode, int length, int line);
int potential_Errors_For_Data(char *input, int table_idx, int line);

/*SecondPassage_Error_Examiner.c*/
void all_Variables_Exist(symbol_ptr s, checklater_ptr sb, int *errs);
void all_Variables_Defined_Once(symbol_ptr s, int *errs);
void valid_Variables_In_Checklater_List(checklater_ptr standby, int *errs, char *cTable[][TOTAL_COMMAND_COLS], char **iTable);
void valid_Conditional_Branching(symbol_ptr s, bmc_ptr b, int *errs);
