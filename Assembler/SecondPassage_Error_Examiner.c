#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Data_File.h"

/*Checks to see that all the labels at the symbol list exist within the check later list.*/
void all_Variables_Exist(symbol_ptr s, checklater_ptr sb, int *errs)
{
   symbol_ptr ps = s;

   /*If psb reaches NULL without finding its counterpart in symbol table, then it is an error.*/

   while (sb)
   {
      if (!strcmp(ps->symbol,sb->toCheck))
      {
         sb = sb->next;
         ps = s;
      }
      else if (!ps->next)
      {
         printf("\"%s\" does not exist.\n",sb->toCheck);
         (*errs)++;
         sb = sb->next;
         ps = s;
      }
      else
      {
         ps = ps->next;
      }
   }

   return;
}

/*Checks to see that all the variables have been defined once.*/
   void all_Variables_Defined_Once(symbol_ptr s, int *errs)
{
   symbol_ptr ps;

   while (s)
   {
      ps = s->next;

      /*If this loop encounters a variable under the same name and different type, then it is an error.
        NOTE: that a given variable can be defined more than once as long as it is under the same type as
        previously defined. */

      while (ps)
      {
         if (!strcmp(s->symbol,ps->symbol) && strcmp(s->type,ps->type))
         {
            printf("%s has already been defined as %s.\n",s->symbol,s->type);
            (*errs)++;
         }
         ps = ps->next;
      }

      s = s->next;
   }

   return;
}

/*Checks the validity of the variable names held within check later list, the list also holds the line in which
the variable was defined.*/
void valid_Variables_In_Checklater_List(checklater_ptr standby, int *errs, char *cTable[][TOTAL_COMMAND_COLS], char **iTable)
{

   while (standby)
   {
      if (legal_Label(cTable, iTable, standby->toCheck, strlen(standby->toCheck), standby->line))
      {
         (*errs)++;
      }
      standby = standby->next;
   }

   return;
}

/*Commands beq, bne, blt, bgt can't accept a variable of type extern.*/
void valid_Conditional_Branching(symbol_ptr s, bmc_ptr b, int *errs)
{
   symbol_ptr ps;

   while (b)
   {
      if (BNE_OPCODE <= b->mila.I.opcode && b->mila.I.opcode <= BGT_OPCODE)
      {
         ps = s;

         while (ps)
         {
            if (!strcmp(b->var,ps->symbol) && !strcmp(ps->type,"external"))
            {
               printf("The command given at address %d cannot recieve extern type variable.\n",b->addr);
               (*errs)++;
            }
            ps = ps->next;
         }
      }

      b = b->next;
   }

   return;
}
