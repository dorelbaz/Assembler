#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Data_File.h"

/*Retrieves the data from the given instruction line and converts it to binary. */
void retrieve(char *content, int table_idx, int *addr, data_ptr *d)
{
   data_ptr p = *d, t = NULL;
   int i = 1;
   long b = 0;

   /*Sets the pointer to the end of the data table.*/

   if (*d)
   {
      while (p->next)
      {
         p = p->next;
      }
   }

   /*Selects a course of action according to the index of the instruction table.*/
	
   switch (table_idx)
   {
      	case DH:

			/*Extracts from the input the values between the commas and assign it and fills the other parameters in each
			of their respective fields.*/

			b = strtol(content, &content, BASE10);

			while (*content != '\0' && *content != '\n')
			{
				content++;
				t = malloc(sizeof(struct DataList));
				if (!t)
				{
					puts("Could not allocate memory for data table");
					exit(1);
				}
				t->type = ".dh";
			
				t->address = *addr;
				(*addr) += DH_ADDRESS_FACTOR;
				t->value = b;
				t->next = NULL;

				/*Adding the node t to the list and prepares the pointer for another potential node.*/

				if (!(*d))   
				{
					*d = t;	
					p = *d;
				}
				else
				{
					p->next = t; 
					p = p->next;
				}

				b = strtol(content, &content, BASE10);
			}
			
			/*This segment is for the last node of data extracted from the input.*/

			t = malloc(sizeof(struct DataList));
			if (!t)
			{	
				puts("Could not allocate memory for data table");
				exit(1);
			}
			t->type = ".dh";

			t->address = *addr;
			(*addr) += DH_ADDRESS_FACTOR;
			t->value = b;
			t->next = NULL;

			if (!(*d))   
			{
				*d = t;	
			}
			else
			{
				p->next = t; 
			}

			break;

     	case DW:
	
	 		/*Same as DH.*/

	 		b = strtol(content, &content, BASE10);

         	while (*content != '\0' && *content != '\n')
	 		{
				content++;
				t = malloc(sizeof(struct DataList));
				if (!t)
				{
					puts("Could not allocate memory for data table");
					exit(1);
				}
	    		t->type = ".dw";
	  
				t->address = *addr;
				(*addr) += DW_ADDRESS_FACTOR;
				t->value = b; 
				t->next = NULL;

				if (!(*d))   
				{
					*d = t;	
					p = *d;
				}
				else
				{
				p->next = t; 
				p = p->next;
				}

				b = strtol(content, &content, BASE10);
			}
	
         	t = malloc(sizeof(struct DataList));
         	if (!t)
   		 	{
				puts("Could not allocate memory for data table");
				exit(1);
         	}
			t->type = ".dw";

			t->address = *addr;
			(*addr) += DW_ADDRESS_FACTOR;
			t->value = b;
			t->next = NULL;

			if (!(*d))   
			{
				*d = t;
			}
			else
			{
				p->next = t; 
			}
			break;

      	case DB:

			/*Same as DH.*/

			b = strtol(content, &content, BASE10);

			while (*content != '\0' && *content != '\n')
			{
				content++;
				t = malloc(sizeof(struct DataList));
				if (!t)
				{
					puts("Could not allocate memory for data table");
					exit(1);
				}
				t->type = ".db";
			
				t->address = *addr;
				(*addr)++;
				t->value = b; 
				t->next = NULL;

				if (!(*d))   
				{
					*d = t;	
					p = *d;
				}
				else
				{
					p->next = t; 
					p = p->next;
				}

				b = strtol(content, &content, BASE10);
			}

			t = malloc(sizeof(struct DataList));
			if (!t)
			{
				puts("Could not allocate memory for data table");
				exit(1);
			}
			t->type = ".db";

			t->address = *addr;
			(*addr)++;
			t->value = b;
			t->next = NULL;

			if (!(*d))   
			{
				*d = t;	
			}
			else
			{
				p->next = t; 
			}

			break;

    	case ASCIZ:

			/*Same as DH.*/

			while (content[i] != '"')
			{
				t = malloc(sizeof(struct DataList));
				if (!t)
				{
					puts("Could not allocate memory for data table");
					exit(1);
				}
				t->type = ".asciz";

				t->address = *addr;
				(*addr)++;
				t->value = (int) content[i]; 
				t->next = NULL;

				if (!(*d))   
				{
					*d = t;
					p = *d;	
				}
				else
				{
					p->next = t; 
					p = p->next;
				}

				i++;
			}

			t = malloc(sizeof(struct DataList));
			if (!t)
			{
				puts("Could not allocate memory for data table");
				exit(1);
			}
			t->type = ".asciz";
			t->address = *addr;
			(*addr)++;
			t->value = 0; 
			t->next = NULL;

			if (!(*d))   
			{
				*d = t;	
			}
			else
			{
				p->next = t; 
			} 
   }

   return;
}

/*Adds labels to the symbol table.*/
void add2SymbolList(char *sym, int line_type, int addr, symbol_ptr *s)
{
   char *types[] = {"external", "entry", "code", "data"};

   symbol_ptr p = *s, t = malloc (sizeof(struct SymbolList));
   if (!t)
   {
      puts("Could not allocate memory for symbol table");
      exit(1);
   }
   t->optional_type = NULL;

   /*Copys the symbol name and address.*/

   t->symbol = malloc(strlen(sym));
   strcpy(t->symbol, sym);
   t->address = addr;

   /*Setting the symbol type.*/

   switch (line_type)
   {
      case IS_EXTERN:

			t->type = types[0];
			break;

      case IS_ENTRY:

			t->type = types[1];
			break;

      case IS_COMMAND:

			t->type = types[2];
			break;

      case IS_INSTRUCTION:

			t->type = types[3];
			break;

      default:

	 		t->type = NULL;
   }

   /*If the table is empty, the program sets the first node (t) as the table head.*/

   if (!(*s))
   {
      t->next = NULL;
      *s = t;
      return;
   }

   /*Otherwise, the program brings the pointer to the end of the table and add the node.*/

   while (p->next)
   {
      p = p->next;
   }

   t->next = NULL;
   p->next = t; 

   return;
}

/*Adds the final IC to the data addresses.*/
void update_Data_Address(data_ptr *data, int addr)
{
   data_ptr p = *data;

   while (p)
   {
      p->address += addr;
      p = p->next;
   }

   return;
}

