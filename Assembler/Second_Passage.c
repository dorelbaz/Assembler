#include "Data_File.h"

void secondPassage(FILE *fp, bmc_ptr *bmc, symbol_ptr *symbols, data_ptr *data, int *errs, int IC, int DC, char **iTable, char *cTable[][TOTAL_COMMAND_COLS])
{
   int label_length = 0, label_flag = 0, line = 1;

   char input[MAX_INPUT_LENGTH+1], label[MAX_LABEL_LENGTH+1];

   fgets(input,MAX_INPUT_LENGTH, fp);

   remove_space_tab(input);
   
   while (!feof(fp))
   {
        printf("\n\t\tline: %d   input: %s\n",line,input);

        if (inputType(input, line, &label_flag, cTable, iTable) == IS_ENTRY)
        {
	        if (label_flag)
	        {
	            extract_Label(label,input,errs,line);
	            label_length = strlen(label);
	        }
	        else
	        {
	            label_length = 0;
	        }    
  
            /*Extracting the variable from the input and updating the symbol table that the aforementioned variable 
            is also of type entry (which will be assigned to opt_type)*/
	        extract_Label(label,input+label_length+strlen(".entry"),errs,line);	    
	        update_Symbols_Entry(label, symbols, errs);	 		
        }

        fgets(input,MAX_INPUT_LENGTH, fp);
        remove_space_tab(input);
        line++;
        label_length = 0;
        label_flag = 0;
        label[0] = '\0';
   }

   /*Updates the addresses of the variables the program couldn't have known untill it had the "full picture".*/
   update_BMC_Adresses(*symbols,bmc);

   return;
}