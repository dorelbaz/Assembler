#include "Data_File.h"

void firstPassage(FILE *fp, bmc_ptr *bmc, symbol_ptr *symbols, data_ptr *data, checklater_ptr *standby, int *errs, int *IC, int *DC, char **iTable, char *cTable[][TOTAL_COMMAND_COLS])
{
   int label_length = 0, command_length = 0, i = 0, label_flag = 0, line = 1;

   /*+1 for the null character. */

   char input[MAX_INPUT_LENGTH+1], label[MAX_LABEL_LENGTH+1];

   char content[MAX_INPUT_LENGTH+1];

   fgets(input,MAX_INPUT_LENGTH+1, fp);

   while (!feof(fp))
   {
        remove_space_tab(input);

        /*Checks to see if the input is within the given length of 80, not including the new line.*/

        if (strlen(input) == MAX_INPUT_LENGTH && input[strlen(input)-1] != '\n' && input[strlen(input)-1] != '\0')
        {
            printf("input's length is too long at line %d.\n",line);
            (*errs)++;
        }

        printf("\n\t\tline: %d   input: %s\n",line,input);

        /*Checks to see what is the input's type:
            Empty Line, 
            A Comment,
            An Entry Instruction Line,
            An External Instruction Line,
            A General Instrction Line,
            A Command Line
            Anything else is undefined. */

        switch (inputType(input, line, &label_flag, cTable, iTable))
        {

            case IS_EMPTY_LINE:
            case IS_COMMENT:

                break;

            case IS_ENTRY:

                if (label_flag)
                {
                    /*As per the project's requirements. Any labels at the beginning of a entry and extern instruction lines,
                    will be ignored.*/
                    puts("\nWarning: Label at beginning of \".entry\" instruction, the program will ignore it.");
                    extract_Label(label,input,errs,line);

                    /*Takes the length of the optional label.*/
                    label_length = strlen(label);
                }
                else
                {
                    label_length = 0;
                }

                /*Takes the variable that comes after the optional label and .entry and adds it to the checks if it is legal. */
                extract_Label(label,input+label_length+strlen(".entry"),errs,line);

                if (legal_Label(cTable, iTable, label, strlen(label), line))
                {
                    (*errs)++;
                }
                
                break;

            case IS_EXTERN:

                if (label_flag)
                {
                    puts("\nWarning: Label at beginning of \".extern\" instruction, the program will ignore it.");
                    extract_Label(label,input,errs,line);

                
                    label_length = strlen(label);
                }
                else
                {
                    label_length = 0;
                }

                /*Takes the variable that comes after the optional label and .extern and checks it is legal, if so,
                the program adds it to the symbol table. */

                extract_Label(label,input+label_length+strlen(".extern"),errs,line);
                if (legal_Label(cTable, iTable, label, strlen(label), line))
                {
                    (*errs)++;
                }	
                else 
                {
                    add2SymbolList(label,IS_EXTERN,0,symbols);   
                }    

                break;
                    
            case IS_COMMAND:

                if (label_flag)
                {
                    /*Takes the the optional label and adds it to the symbol table, if it is legal. */
                    extract_Label(label,input,errs,line);
                    if (legal_Label(cTable, iTable, label, strlen(label), line))
                    {
                        (*errs)++;
                        break;
                    }
                    add2SymbolList(label,IS_COMMAND,*IC,symbols);

                    /*+1 to accommodate for ':'.*/

                    label_length = strlen(label)+1;
                }
                else 
                {
                    label_length = 0;
                }

                for (i = STOPCOMMAND; i >= 0; i--)
                {
                    if(!strncmp(input+label_length,cTable[i][0],strlen(cTable[i][0])))
                    {
                        /*Total length of the label + the length of the command.*/
                        command_length = label_length + strlen(cTable[i][0]);

                        /*According to the index of the string matrix, the program directs the error examination. 
                        if i < 8 command is of type R
                        if i < 23 command is of type I
                        if i <= 26 command is of type J*/
                        if (i < ICOMMANDSTART)
                        {
                            if (potential_Errors_For_R(input+command_length,cTable[i][OPCODE],strlen(input+command_length), line))
                            {
                                (*errs)++;
                            }
                        }
                        else if (i < JCOMMANDSTART)
                        {
                            if(potential_Errors_For_I(standby,input+command_length,cTable[i][FUNCT],strlen(input+command_length),line))
                            {
                                (*errs)++;
                            }
                        }
                        else if (i <= STOPCOMMAND)
                        {
                            if(potential_Errors_For_J(standby,iTable,cTable,input+command_length,cTable[i][OPCODE],strlen(input+command_length),line))
                            {
                                (*errs)++;
                            }
                        }
                    
                        /*If the error examination didn't find an error, the content of the command will be copied 
                            and sent to be converted to binary. */
                        if (!(*errs))
                        {
                            strcpy(content, input+command_length);
                            convert(cTable[i][TYPE],cTable[i][FUNCT],cTable[i][OPCODE],content,*IC,bmc);
                        }

                        /* To end the loop. */
                        i = -1;
                    }
                } 

                (*IC) += COMMAND_MILA_SIZE;
                break;

            case IS_INSTRUCTION:

                if (label_flag)
                {
                    /*Takes the the optional label and adds it to the symbol table, if it is legal. */
                    extract_Label(label,input,errs,line);
                    if (legal_Label(cTable, iTable, label, strlen(label), line))
                    {
                        (*errs)++;
                        break;
                    }
                    add2SymbolList(label,IS_INSTRUCTION,*DC,symbols);
                    label_length = strlen(label)+1;
                }
                else 
                {
                    label_length = 0;
                }

                /*Minus the 2 positions holding entry and extern.*/

                for (i = 0; i < INSTRUCTIONS-2; i++)
                {
                    if(!strncmp(input+label_length,iTable[i],strlen(iTable[i])))
                    {

                        /*According to the index of the instruction array, the program directs the error examination. 
                        if i = 0 instruction is of type dh
                        if i = 1 instruction is of type dw
                        if i = 2 instruction is of type db
                        if i = 3 instruction is of type asciz */
                        if (potential_Errors_For_Data(input+label_length+strlen(iTable[i]),i,line))
                        {
                            (*errs)++;
                        }
                        else
                        {
                            /*If the error examination didn't find an error, the content of the instruction will be copied 
                            and in the function, the data will be retrieved and converted to binary. */
                            strcpy(content, input+label_length+strlen(iTable[i]));
                            retrieve(content,i,DC,data);
                        }

                        /*To end the loop.*/
                        i = 6;
                    }
                } 

                break;

            default:

                printf("\nUndefined code at line %d.\n",line);
                (*errs)++;
        }

        fgets(input,MAX_INPUT_LENGTH+1, fp);

        /*In case that the address will exceed the given limit of 2^25-1.*/
        if (*IC > MEMORY_LIMIT)
        {
            puts("Memory limit exceeded.");
            (*errs)++;
        }

        line++;
        label_length = 0;
        label_flag = 0;
        strcpy(label,"\0");
   }

    /*After all lines of input are read and analysed, the program updates the addresses of the variables of data types
     in the symbol table and it updates the addresses in the data table, with the final IC.*/
    update_Symbol_Address(symbols, *IC);
    update_Data_Address(data, *IC);

    /*NOTE: that in my project I decided that a given variable:
    CAN'T BE data and code
    CAN'T BE external and code
    CAN'T BE external and data 
    CAN'T BE of type entry alone
    Since these statements and how to react to them aren't mentioned anywhere in the project's pdf.*/
  
    return;
}