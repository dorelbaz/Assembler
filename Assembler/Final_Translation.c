#include "Data_File.h"

/*Translates what the linked lists hold in the end to "machine code" in hexadecimal.*/
void final_Translation(bmc_ptr b, data_ptr d, symbol_ptr s, char *output_file, int ic, int dc)
{
   bmc_ptr pb = b; 
   data_ptr pd = d;  
   symbol_ptr ps = s;
   int i, j, counter = 0, buffer = 0, data_addr = ic, remove_indicator = 0;
   FILE *f;

   /*Allocating memory for the 3 file names, .ob .ent .ext*/

   char *fob = malloc(strlen(output_file)+POSTFIX4);
   char *fext = malloc(strlen(output_file)+POSTFIX5);
   char *fent = malloc(strlen(output_file)+POSTFIX5);

   if (!fob)
   {
      puts("\nCould not allocate memory for object file.");
      exit(1);
   }
   if (!fext)
   {
      puts("\nCould not allocate memory for external file.");
      exit(1);
   }
   if (!fent)
   {
      puts("\nCould not allocate memory for entry file.");
      exit(1);
   }
   
   strcpy(fob, output_file);
   strcpy(fext, output_file);
   strcpy(fent, output_file);

   strcat(fob, ".ob");
   strcat(fext, ".ext");
   strcat(fent, ".ent");

   f = fopen(fob, "w");

   fprintf(f, "\t%d %d\t\n", ic-100, dc);

   /*This loop prints to file the binary machine code.*/

   while (pb)
   {
        fprintf(f, "%04d ", pb->addr);

        /*The project's requirements is to print the output in "reverse".
            So if we have 0000 1111 1000 0101 in hexdecimal this is 0F 85.
            The requirements want the output to be 85 0F 
            and this is what the following loop does. */

        for(j = 0; j < BMC_PRINT_FACTOR; j++)
        {
            for (i = 0; i <= BITS8-1; i++)
            {
                if (pb->mila.m[j] & (1 << i))
                {
                    buffer |= (1 << i);
                }
            }
            fprintf(f, "%02X ", buffer);
            buffer = 0;
        } 

        fprintf(f, "\n");
        
        pb = pb->next;
   }

   fprintf(f, "%04d ", data_addr);

   /*This loop prints to file the data table.*/
   while (pd)
   {
        /*The following loops copy to buffer the exact bits of the hexadecimal value. When 8 bits have been copied, 
            representing 2 hexadecimal numbers, buffer will be printed together with a single space and when counter == 4,
            meaning 4 couples have been printed, a new line together with the next address will be printed and the buffer will
            reset. */

        if (!strcmp(pd->type, ".dw"))
        {
            /*Because dw type contains 32 bits, the process mentioned above will have to repeat itself 4 times.*/

            for(j = 0; j < DW_ADDRESS_FACTOR; j++)
            {
	            if (counter == NEW_LINE_COUNTER)
                {
                    fprintf(f, "\n%04d ", data_addr += counter);
                    counter = 0;
                }

                for (i = 0; i <= BITS8-1; i++)
                {
                    if (pd->value & (1 << (i + BITS8*j)))
                    {
                        buffer |= (1 << i);
                    }
	            }

                fprintf(f, "%02X ", buffer);
	            counter++;
                buffer = 0;
            }
        }
        else if (!strcmp(pd->type, ".dh"))
        {
	        /*Because dh type contains 16 bits, the process mentioned above will have to repeat itself twice.*/
            for(j = 0; j < DH_ADDRESS_FACTOR; j++)
            {
	            if (counter == NEW_LINE_COUNTER)
	            {
   	                fprintf(f, "\n%04d ", data_addr += counter);
	                counter = 0;
	            }

                for (i = 0; i <= BITS8-1; i++)
                {
     	            if (pd->value & (1 << (i + BITS8*j)))
      	            {
                        buffer |= (1 << i);
       	            }
	            }

                fprintf(f, "%02X ", buffer);
                counter++;
                buffer = 0;
            }
        }
        else
        {
	        if (counter == NEW_LINE_COUNTER)
	        {
                fprintf(f, "\n%04d ", data_addr += counter);
                counter = 0;
            }

            for (i = 0; i <= BITS8-1; i++)
   	        {
                if (pd->value & (1 << i))
                {
                    buffer |= (1 << i);
                }
   	        }

            fprintf(f, "%02X ", buffer);
	        counter++;
            buffer = 0;
        }

        pd = pd->next;
   }

   fclose(f);
   f = fopen(fent, "w");

   /*Generates a .ent file, if any entry type variables are found.*/
   while (ps)
   {
        if(ps->type && !strcmp(ps->type,"entry"))
        {
            remove_indicator++;
            fprintf(f, "%s %04d\n",ps->symbol,ps->address);
        }
        else if (ps->optional_type && !strcmp(ps->optional_type,"entry"))
        {
            remove_indicator++;
            fprintf(f, "%s %04d\n",ps->symbol,ps->address);
        }

        ps = ps->next;
   }
   
   fclose(f);
   f = fopen(fext, "w");

   /* Removes the entry file that has been made by default, when fopen mode w is used, in case no entry types have been
      found.*/

   if (!remove_indicator)
   {
        remove_indicator = remove(fent);
        if (!remove_indicator)
        {
	        puts("No entry file has been made.");
        }
   }
   else
   {
        puts("Entry file has been made.");
   }

   remove_indicator = 0;
   pb = b;

   /*Generates a .ext file, if any extern type variables are found.*/
   while (pb)
   {
        if (pb->var)
        {
	        ps = s;

            while (ps)
            {
                if (!strcmp(ps->symbol,pb->var))
                {
                    if (!strcmp(ps->type,"external"))
                    {
                        remove_indicator++;
                        fprintf(f, "%s %04d\n",ps->symbol,pb->addr);
                    }
                    else if (ps->optional_type && !strcmp(ps->optional_type,"external"))
                    {
                        remove_indicator++;
                        fprintf(f, "%s %04d\n",ps->symbol,pb->addr);
                    }
                }	    
                ps = ps->next;
            }
        }

        pb = pb->next;
   }
  
   fclose(f);

   
   if (!remove_indicator)
   {
        remove_indicator = remove(fext);
        if (!remove_indicator)
        {
            puts("No external file has been made.");
        }
   }
   else
   {
        puts("External file has been made.");
   }

   free(fob);
   fob = NULL;
   free(fent);
   fent = NULL;
   free(fext);
   fext = NULL;
  
   return;
}
