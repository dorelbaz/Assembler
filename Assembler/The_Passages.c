#include "Data_File.h"

void the_Passages(int argc, char **argv)
{

    /*This is the command table from the project's pdf in string matrix format.
        Note that since funct section (the third column) is not in use when it comes to commands of type I,
        I have repurposed that column to aid me when it comes to distinguishing between the 2 different types of I
        commands. How it helps will be shown in Command_Converter.c.*/

    char *cTable[27][4] = {

    {"add", "R", "1", "0"},

    {"sub", "R", "2", "0"},

    {"and", "R", "3", "0"},

    {"or", "R", "4", "0"},

    {"nor", "R", "5", "0"},

    {"move", "R", "1", "1"},

    {"mvhi", "R", "2", "1"},

    {"mvlo", "R", "3", "1"},

    {"addi", "I", "TAKES_A_CONST", "10"},

    {"subi", "I", "TAKES_A_CONST", "11"},

    {"andi", "I", "TAKES_A_CONST", "12"},

    {"ori", "I", "TAKES_A_CONST", "13"},

    {"nori", "I", "TAKES_A_CONST", "14"},

    {"bne", "I", "NEEDS_VARIABLE_ADDRESS", "15"},

    {"beq", "I", "NEEDS_VARIABLE_ADDRESS", "16"},

    {"blt", "I", "NEEDS_VARIABLE_ADDRESS", "17"},

    {"bgt", "I", "NEEDS_VARIABLE_ADDRESS", "18"},

    {"lb", "I", "TAKES_A_CONST", "19"},

    {"sb", "I", "TAKES_A_CONST", "20"},

    {"lw", "I", "TAKES_A_CONST", "21"},

    {"sw", "I", "TAKES_A_CONST", "22"},

    {"lh", "I", "TAKES_A_CONST", "23"},

    {"sh", "I", "TAKES_A_CONST", "24"},

    {"jmp", "J", "0", "30"},

    {"la", "J", "0", "31"},

    {"call", "J", "0", "32"},

    {"stop", "J", "0", "63"}

    };

    /*This is the instruction table from the project's pdf in string array format.*/

    char *iTable[6] = {".dh", ".dw", ".db", ".asciz", ".entry", ".extern"};

    /*If the given assembly program is valid, this will be the name holder of the output file.*/

    char *output_file = NULL;

    /* n serves as the index for the files.*/

    int n = 1, errs = 0, IC = 100, DC = 0;

    FILE *fp = NULL;

    bmc_ptr bmc = NULL;

    symbol_ptr symbols = NULL;

    data_ptr data = NULL;

    /*This pointer holds all the variables names the program comes across, to check if they all are valid and exist 
        before moving to the second passage.*/
    checklater_ptr standby = NULL;

    while (n < argc) 
    {

        /*Firstly, the program checks if the given file name from the command line argument is valid.*/
        if (valid_File_Name(argv[n]))
        {
            puts("Invalid file.");
            errs++;
        }
        else
        {
            /*Secondly, the program attempts to open the file and allocate memory for the output file in advance.*/
            fp = fopen(argv[n],"r");
            output_file = malloc(strlen(argv[n])+2);

            if (!fp ) 
            {
                printf("\nCould not open file.\n"); 
                errs++;
            }
            if (!output_file)
            {
                puts("Could not allocate memory for output file");
                errs++;
            }	 
        }

        if (!errs)
        {
            /*Thirdly, the program begins the first passage.*/
            puts("First Passage:");
            firstPassage(fp,&bmc,&symbols,&data,&standby,&errs,&IC,&DC,iTable,cTable);
        }

        if (!errs)
        {
            secondpassage_Setup(fp,&bmc,&symbols,&data,&standby,&errs,IC,DC,iTable,cTable); 

            if (!errs)
            {
                /*Finally, printing what the linked list held in the end and then beginning the final translation.*/
                print_List(symbols,SYMBOL);
                print_List(bmc,BMC);
                print_List(data,DATA);

                strcpy(output_file, strtok(argv[n], "."));
                final_Translation(bmc,data,symbols,output_file, IC, DC);
            }
            
        }

        reset_Parameters(&fp, &bmc, &symbols, &data, &standby, &output_file, &errs, &IC, &DC);

        n++;
    }    
}