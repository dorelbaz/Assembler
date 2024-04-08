#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Data_File.h"

/*Converts the given command into binary. */
void convert(char *ty, char *ft, char *oc, char *content, int addr, bmc_ptr *bmc)
{
   bmc_ptr p = *bmc, t;
   long val; 
  	 
   t = malloc(sizeof(struct CodeList));
   if (!t)
   {
      puts("Could not allocate memory for command table");
      exit(1);
   }

   /*Manually resetting or setting the bmc parameters.*/

   t->mila.m[0] = '\0';
   t->mila.m[1] = '\0';
   t->mila.m[2] = '\0';
   t->mila.m[3] = '\0';
   t->addr = addr;
   t->type = *ty; 
   t->var= NULL;
	
   /*Selecting a course of action according to the type of command.*/

   switch (*ty)
   {
      case 'R':

         /*Assigning each value in its respective field.*/

         val = atol(ft);
         t->mila.R.funct = val;
         val = atol(oc);
         t->mila.R.opcode = val;
         t->mila.R.not_in_use = 0;

         /*If opcode is 0, it means this command takes 3 registers.*/

         if (*oc == '0')
         {
            val = strtol(content+1, &content, BASE10);
            t->mila.R.rs = val;

            val = strtol(content+2, &content, BASE10);
            t->mila.R.rt = val;

            val = strtol(content+2, &content, BASE10);
            t->mila.R.rd = val;

         }

         /*Else opcode is 1, it means this command takes 2 registers.*/

         else
         {
            val = strtol(content+1, &content, BASE10);
            t->mila.R.rs = val;

            val = strtol(content+2, &content, BASE10);
            t->mila.R.rd = val;

         }

         break;

      case 'I':   

         val = atol(oc);
         t->mila.I.opcode = val;

         /* Takes the constant and assigns in immed field.*/

         if (!strcmp(ft,"TAKES_A_CONST"))  
         {
            val = strtol(content+1, &content, BASE10);
            t->mila.I.rs = val;

            val = strtol(content+1, &content, BASE10);
            t->mila.I.immed = val;

            val = strtol(content+2, &content, BASE10);
            t->mila.I.rt = val;

         }

         /* Will fill in the immed field in the second passage. Assigns the needed variable in the var field as a mark.*/

         if (!strcmp(ft,"NEEDS_VARIABLE_ADDRESS"))
         {
            val = strtol(content+1, &content, BASE10);
            t->mila.I.rs = val;

            val = strtol(content+2, &content, BASE10);
            t->mila.I.rt = val;

                  t->var = malloc(strlen(strtok(content+1,"\n")));
            strcpy(t->var,strtok(content+1,"\n"));
         }

         break;

      case 'J':

         val = atol(oc);
         t->mila.J.opcode = val;

         /* Will fill in the immed field in the second passage. Assigns the needed variable in the var field as a mark.
            Else, the program assigns each value in its respective field.*/

         if (content[0] == '$' && !strcmp(oc,"30"))
         {
            t->mila.J.reg = 1;

            val = strtol(content+1, &content, BASE10);

            t->mila.J.address = val;

         }
         else if (strcmp(oc, "63"))
         {
            t->var = malloc(strlen(strtok(content,"\n")));
            strcpy(t->var,strtok(content,"\n"));
         }
   }

   /*Sets t as the head.*/

   if (!(*bmc))
   {
      t->next = NULL;
      *bmc = t;
      return;
   }

   /*Sets the pointer to the end of the list and adds the node.*/

   while (p->next)
   {
      p = p->next;
   }

   t->next = NULL;
   p->next = t; 

   return;
}

/* Puts the variables it has found so far on stand by, to be check prior to the second passage.*/
void put_On_Stand_By(checklater_ptr *standby, char *temp, int line)
{
   checklater_ptr sbp = *standby, t;

   /*If list is empty, the program sets the first variable the program encountered so far as the head.*/

   if (!(*standby))
   {
      t = malloc(sizeof(struct CheckLaterList));
      if (!t)
      {
         puts("Could not allocate memory for check later list");
         exit(1);
      }
      t->next = NULL;
      t->toCheck = malloc(strlen(temp));
      strcpy(t->toCheck, temp);
      t->line = line;
      *standby = t;
      return;
   }

   /*Otherwise, the program sets the pointer to the end of the list, there the program adds the variable to it.*/

   while (sbp->next)
   {
	   sbp = sbp->next;
   }

   t = malloc(sizeof(struct CheckLaterList));
   if (!t)
   {
      puts("Could not allocate memory for check later list");
      exit(1);
   }
   t->next = NULL;
   t->toCheck = malloc(strlen(temp));
   strcpy(t->toCheck, temp);
   t->line = line;
   sbp->next = t;
   
   return;
}

/*Updates the addresses, of the variables of type data in the symbol table.*/
void update_Symbol_Address(symbol_ptr *symbols, int addr)
{
   symbol_ptr p = *symbols;

   while (p)
   {
      if (!strcmp(p->type,"data"))
      {
	      p->address += addr;
      }

      p = p->next;
   }

   return;
}
  
/*Updates the optional_type in the symbol list in case the given variable is also entry.*/
void update_Symbols_Entry(char *sym, symbol_ptr *s, int *errs)
{ 
   symbol_ptr ps = *s;

   while (ps)
   {
      /*In case the given variable is already of type external, then it is an error.*/

      if (!strcmp(ps->type,"external") && !strcmp(ps->symbol,sym))
      {
         (*errs)++;
         printf("%s can't be entry and external.\n",ps->symbol);
         return;
      }
      else if (!strcmp(ps->symbol,sym))
      {
	      ps->optional_type = "entry";
      }
      ps = ps->next;
   }

   return;
}  

/*Updates the addresses of the variables the program didn't know of untill the end of the second passage.*/
void update_BMC_Adresses(symbol_ptr s, bmc_ptr *b)
{
   symbol_ptr ps = s;
   bmc_ptr pb = *b;

   while (pb)
   {
      if (pb->var)
      {
	      ps = s;
         while (ps)
         {
            /*Variables the program couldn't know the addresses of, were marked at the first passage by being assigned to
             the 'var' field in the linked list of bmc. Now, that the program has the "full picture" it starts updating the
            bmc linked list according to the project's pdf requirements.*/

            if (!strcmp(ps->symbol,pb->var))
            {
               if (pb->type == 'J')
               {
                  pb->mila.J.address = ps->address;
               }
               else
               {
                  pb->mila.I.immed = ps->address - pb->addr;
               }
            }
            ps = ps->next;
         }
      }

      pb = pb->next;
   }

   return;
}
