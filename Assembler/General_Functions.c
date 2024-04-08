#include "Data_File.h"

/*Checks the validity of the given file. Specifically, that it contains '.as' at its postfix.*/
int valid_File_Name(char *file)
{
   int i, found_dot = -1;

   for (i = 0; i < strlen(file); i++)
   {
      if (file[i] == '.')
      {
         found_dot = i;
         i = strlen(file);
      }
   }

   if (found_dot+3 <= strlen(file) && file[found_dot+1] == 'a' && file[found_dot+2] == 's' && file[found_dot+3] == '\0')
   {
      return VALID;
   }

   return ERROR;
}

/*Cleans the input from spaces and tabs.*/
void remove_space_tab(char *input)
{
   int n = 0, i = 0;

   while(input[i] != '\0') 
   {

      if (input[i] != ' ' && input[i] !=  '	') 
      {
         input[n] = input[i]; 
         n++; 
      }

      i++;

   }
	
   input[n] = '\0';

   return;
}

/*Returns the type of input.*/
int inputType(char *input, int line, int *flag, char *cTable[][TOTAL_COMMAND_COLS], char **iTable)
{
   int i = 0, label_length = 0;

   if (input[0] == '\n')
   {
      return IS_EMPTY_LINE;
   }
   else if (input[0] == ';')
   {
      return IS_COMMENT;
   }
   
   /*Checks to see if the input contains a label at its beginning.*/
   while (input[i] != '\0') 
   {
      if (input[i] == ':') 
      {
         label_length = i+1;
         (*flag)++;
      }

      i++;
   }

   /*Checks if the input is a command line. Starts from the end of cTable so as to not confuse sub, and, nor, or 
     add with subi, andi, nori, i, addi.*/
   for (i = STOPCOMMAND; i >= 0; i--)
   {

      if(!strncmp(input+label_length,cTable[i][0],strlen(cTable[i][0])))
      {
	      return IS_COMMAND;
      }
   } 

   /*Checks if the input is one of the instruction lines. */
   for (i = 0; i < INSTRUCTIONS; i++)
   {
      if(!strncmp(input+label_length,iTable[i],strlen(iTable[i])))
      {
         if (i == ENTRY_INSTRCTION)
         {
            return IS_ENTRY;
         }
         else if (i == EXTERN_INSTRUCTION)
         {
            return IS_EXTERN;
         }
         return IS_INSTRUCTION;
      }
   }

   return UNDEFINED;
}

/*Extracts the optional label from the beginning of the input line.*/
void extract_Label(char *label, char *input, int *errs, int line)
{
   char *temp;

   temp = strtok(input, ":\n");

   /*If the label's length is out of the given range of 31, then it is an error.*/

   if (strlen(temp) > MAX_LABEL_LENGTH)
   {
      printf("Label \"%s\" length is too long, defined at line %d.\n",temp,line);
      strcpy(label,"\0");
      (*errs)++;
      return;
   }

   strcpy(label, temp);

   return;
}

void print_Bits(int number,int bits)
{
    int i;

    for (i = bits-1; i >= 0; i--)
    {
        if (number & (1 << i))
        {
            putchar('1');
        }
        else
        {
            putchar('0');
        }
    }

    putchar(' ');

    return;
}


/*Prints the given list. This calls back to Mamman 12.*/
void print_List(void *list, int t)
{
   symbol_ptr s;
   data_ptr d;
   bmc_ptr b;
   checklater_ptr sb;

   switch (t)
   {
        case SYMBOL:

            s = (symbol_ptr) list;

            printf("\n");

            puts("Printing Symbol Table:");

            while (s)
            {
                printf("Address: %d   ", s->address);
                printf("Type: %s   ", s->type);
                printf("Second Type: %s   ", s->optional_type);
                printf("Symbol: %s\n", s->symbol);

                s = s->next;
            }

            break;

        case DATA:

            d = (data_ptr) list;

            printf("\n");

            puts("Printing Data Table:");

            while (d)
            {
                printf("Value: ");
                if (!strcmp(d->type,".dh"))
                {
                    print_Bits(d->value,BITS16);
                    printf("\t\t\tAddress: %d\n", d->address);
                }
                else if (!strcmp(d->type,".dw"))
                {
                    print_Bits(d->value,BITS32);
                    printf("\tAddress: %d\n", d->address);
                }
                else
                {
                    print_Bits(d->value,BITS8);
                    printf("\t\t\t\tAddress: %d\n", d->address);
                }

                d = d->next;
            }

            break;

        case BMC:

            b = (bmc_ptr) list;

            printf("\n");

            puts("Printing Binary Machine Code:");

            while (b)
            {
                printf("Address: %d   ", b->addr);
                printf("Mila: ");

                if (b->type == 'R')
                {
                    print_Bits(b->mila.R.opcode,OPCODE_FIELD);
                    print_Bits(b->mila.R.rs,RS_FIELD);
                    print_Bits(b->mila.R.rt,RT_FIELD);
                    print_Bits(b->mila.R.rd,RT_FIELD);
                    print_Bits(b->mila.R.funct,FUNCT_FIELD);
                    print_Bits(b->mila.R.not_in_use,NOT_IN_USE);
                }  
                else if (b->type == 'I')
                {
                    print_Bits(b->mila.I.opcode,OPCODE_FIELD);
                    print_Bits(b->mila.I.rs,RS_FIELD);
                    print_Bits(b->mila.I.rt,RT_FIELD);
                    printf("  ");
                    print_Bits(b->mila.I.immed,IMMED_FIELD);		
                }
                else
                {
                    print_Bits(b->mila.J.opcode,OPCODE_FIELD);
                    printf("%d    ",b->mila.J.reg);
                    print_Bits(b->mila.J.address,ADDRESS_FIELD);	
                }  
                    printf("   ");
                    printf("Variable: %s\n", b->var);

                b = b->next;
            }

            break;

        case STANDBY:

            sb = (checklater_ptr) list;

            printf("\n");

            puts("Printing Variables To Be Checked:");

            while (sb)
            {
                printf("\n%s used at line %d\n",sb->toCheck,sb->line);
                sb = sb->next;
            }

    }

   return;
}

/*Frees the given list. This calls back to Mamman 12.*/
void free_List(void **list, int t)
{
   symbol_ptr s, ps;
   data_ptr d, pd;
   bmc_ptr b, pb;
   checklater_ptr sb, psb;

   switch (t)
   {
        case SYMBOL:

            s = (symbol_ptr) *list;
            ps = s;

            while (s)
            {
                s = s->next;
                free(ps->symbol);
                ps->symbol = NULL;
                free(ps);
                ps = NULL;
                ps = s;
            }

            break;

        case DATA:

            d = (data_ptr) *list;
            pd = d;

            while (d)
            {
                d = d->next;
                free(pd);
                pd = NULL;
                pd = d;
            }

            break;

        case BMC:

            b = (bmc_ptr) *list;
            pb = b;

            while (b)
            {
                b = b->next;
                free(pb->var);
                pb->var = NULL;
                free(pb);
                pb = NULL;
                pb = b;
            }

            break;

        case STANDBY:

            sb = (checklater_ptr) *list;
            psb = sb;

            while (sb)
            {
                sb = sb->next;
                free(psb->toCheck);
                psb->toCheck = NULL;
                free(psb);
                psb = NULL;
                psb = sb;
            }
   }

   return;

}

void reset_Parameters(FILE **fp, bmc_ptr *bmc, symbol_ptr *symbols, data_ptr *data, checklater_ptr *standby, char **output_file, int *errs, int *IC, int *DC)
{
    /*Freeing/resetting parameters accordingly for the next potential file to be scanned.*/
    free_List((void **) &bmc, BMC);
    free_List((void **) &data, DATA);
    free_List((void **) &symbols, SYMBOL);
    free_List((void **) &standby, STANDBY);
	
    if (!(*output_file)) 
    {
        free((*output_file));
    }
    *output_file = NULL;
	
    *bmc = NULL;
    *symbols = NULL;
    *data = NULL;
    *standby = NULL;

    *errs = 0; 
    *IC = 100; 
    *DC = 0;

    if ((*fp)) 
    {
        fclose((*fp));
        fp = NULL;
    }     
}

