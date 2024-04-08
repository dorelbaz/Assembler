#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "Data_File.h"

/*Examines whether a given label is legal.*/
int legal_Label(char *cTable[][TOTAL_COMMAND_COLS], char **iTable, char *input, int length, int line)
{
   int i;

   /*Checks to see if the first letter of the label is a capital letter or small letter. '\0' in case there is no label.*/

   if (!('a' <=  *input && *input <= 'z') && !('A' <=  *input && *input <= 'Z') && *input != '\0')
   {
      printf("First letter of the label is invalid at line %d.\n",line);
      return ERROR;
   }

   /*Checks to see that the label isn't a command.*/

   for (i = STOPCOMMAND; i >= 0; i--)
   {
      if (!strncmp(input,cTable[i][0],strlen(cTable[i][0])) )
      {
         /*This condition checks to see if the label is really a command and not something of this sort:  addy...
           which is a legal label*/
         if (*(input+strlen(cTable[i][0])) == '\0')
         {
            printf("Label can't be a reserved word at line %d.\n",line);
            return ERROR;
         }
      }
   }

   /*Checks to see that the label isn't an instruction.*/

   for (i = 0; i < INSTRUCTIONS; i++)
   {
      /*+1 to skip the dot at the beginning of an instruction, -1 to accommodate for it.*/

      if (!strncmp(input,iTable[i]+1,strlen(iTable[i])-1) )
      {
         if (*(input+strlen(iTable[i])-1) == '\0')
         {
            printf("Label can't be a reserved word at line %d.\n",line);
            return ERROR;
         }
      }
   }
 
   input++;
   
   /*Checks to see that all the characters in the label are either a capital letter, small letter or a number.*/

   while (*input != '\0' && *input != '\n')
   {

      if (!('a' <=  *input && *input <= 'z') && !('A' <=  *input && *input <= 'Z'))
      {
         if (!('0' <= *input && *input <= '9'))
         {
            printf("Illegal label at line %d.\n",line);
            return ERROR;
         }
      }

      input++;
   }

   return VALID;
}


/*Examines errors related to command of type R. */
int potential_Errors_For_R(char *input, char *type, int length, int line)
{
   long val = 0;
   char *temp;

   /*This function is structured the way it does to give a continual sense of "flow" or "rhythm".
     This is an example for a valid command line of type R
		OPTIONAL_LABEL: add $1, $5, $10
     What the function will get is the latter part of the input
		$1, $5, $10
     using this example, I will explain this function.*/

   /*If this condition will be evaluated as true it means the function just got a new line character
	instead of 3 registers like in the example.*/

   if (length <= 1)
   {
      printf("Missing registers at line %d.\n",line);
      return ERROR;
   }

   /*This means the function got this , $1, $5, $10 */

   if (*input == ',')
   {
      printf("Illegal comma at line %d.\n",line);
      return ERROR;
   }

   /*Missing first '$' sign $1, $5, $10 
		            ^	*/

   if (*input != '$')
   {
      printf("Invalid first register at line %d.\n",line);
      return ERROR;
   }

   /*First register is missing  $\n, $5, $10 
				 ^ */

   if (*(input+1) == '\n' || *(input+1) == '\0')
   {
      printf("Missing first register at line %d.\n",line);
      return ERROR;
   }

   /*First register is invalid  $x, $5, $10 
				 ^ */

   if (!('0' <= *(input+1) && *(input+1) <= '9'))
   {
      printf("Invalid first register at line %d.\n",line);
      return ERROR;
   }

   /*Examine the validity of the first register number.*/

   val = strtol(input+1, &temp, BASE10);

   if (val < 0 || REGISTER31 < val)
   {
      printf("First Register out of bounds at line %d.\n",line);
      return ERROR;
   } 

   /*$1\n$5, $10 First comma is missing.  
       ^ */

   if (*temp == '\n' || *temp == '\0')
   {
      printf("Missing registers at line %d.\n",line);
      return ERROR;
   }

   /*$1x$5, $10 First comma is missing. 
       ^ */

   if (*temp != ',')
   {
      printf("Missing the first comma between first and second register at line %d.\n",line);
      return ERROR;
   } 

   /*$1,,$5, $10   
        ^ */

   if (*(temp+1) == ',')
   {
      printf("Multiple consecutive commas at line %d.\n",line);
      return ERROR;
   }

   /*$1,\n$5, $10 The second '$' sign is missing.  
        ^ */

   if (*(temp+1) == '\n' || *(temp+1) == '\0')
   {
      printf("Missing registers at line %d.\n",line);
      return ERROR;
   }

   /*$1,x$5, $10 The second '$' sign is missing.  
        ^ */

   if (*(temp+1) != '$')
   {
      printf("Invalid second register at line %d.\n",line);
      return ERROR;
   } 

   /*$1,$\n, $10 The second register is missing.  
          ^ */

   if (*(temp+2) == '\n' || *(temp+2) == '\0')
   {
      printf("Missing second register at line %d.\n",line);
      return ERROR;
   }

   /*$1,$x, $10 This means the second register is invalid.  
         ^ */

   if (!('0' <= *(temp+2) && *(temp+2) <= '9'))
   {
      printf("Invalid second register at line %d.\n",line);
      return ERROR;
   } 

   /*Examine the validity of the second register number.*/

   val = strtol(temp+2, &temp, BASE10);

   if (val < 0 || REGISTER31 < val)
   {
      printf("Second register out of bounds at line %d.\n",line);
      return ERROR;
   } 

   /*If R command has opcode 1, it means the input needs to contain 2 registers.*/

   if (!strcmp(type, "1"))
   {
      /*At this point, the input needs to look like this $1,$5\n*/

      if (*temp == ',')
      {
         printf("Illegal comma after the second register (should be end of command) at line %d.\n",line);
         return ERROR;
      }

      if (*temp != '\n' && *temp != '\0')
      {
         printf("Extraneous input after end of command at line %d.\n",line);
         return ERROR;
      }

      return VALID;
   }

   /*$1,$5\n $10 The second comma is missing.  
          ^ */

   if (*temp == '\n' || *temp == '\0')
   {
      printf("Missing third register at line %d.\n",line);
      return ERROR;
   }

   /*$1,$5x $10 The second comma is missing.  
          ^ */

   if (*temp != ',')
   {
      printf("Missing the second comma between second and third register at line %d.\n",line);
      return ERROR;
   } 

   /*$1,$5,, $10  
           ^ */

   if (*(temp+1) == ',')
   {
      printf("Multiple consecutive commas at line %d.\n",line);
      return ERROR;
   }

   /*$1,$5,\n $10 The third '$' is missing
            ^ */

   if (*(temp+1) == '\n' || *(temp+1) == '\0')
   {
      printf("Missing third register at line %d.\n",line);
      return ERROR;
   }

   /*$1,$5,x$10 The third '$' is invalid.  
           ^ */

   if (*(temp+1) != '$')
   {
      printf("Invalid third register at line %d.\n",line);
      return ERROR;
   } 

   /*$1,$5,$\n10 The third register is missing.  
            ^ */

   if (*(temp+2) == '\n' || *(temp+2) == '\0')
   {
      printf("Missing third register at line %d.\n",line);
      return ERROR;
   }

   /*$1,$5,$x The third register is invalid.  
            ^ */

   if (!('0' <= *(temp+2) && *(temp+2) <= '9'))
   {
      printf("Invalid third register at line %d.\n",line);
      return ERROR;
   } 

   /*Examine the validity of the third register number.*/

   val = strtol(temp+2, &temp, BASE10);

   if (val < 0 || REGISTER31 < val)
   {
      printf("Third register out of bounds at line %d.\n",line);
      return ERROR;
   } 

   /*$1,$5,$10x Finally, the input needs to end on either a new line or null character.  
              ^ */

   if (*temp == ',')
   {
      printf("Illegal comma at the end at line %d.\n",line);
      return ERROR;
   }

   if (*temp != '\n' && *temp != '\0')
   {
      printf("Extraneous input after end of command at line %d.\n",line);
      return ERROR;
   }

   return VALID;
}


/*Examines errors related to command of type I. */
int potential_Errors_For_I(checklater_ptr *standby, char *input, char *type, int length, int line)
{

   long val = 0;
   char *temp;

   /*This function is structured the way it does to give a continual sense of "flow" or "rhythm".
     These are examples of a valid command line of type I, depending on the repurposed funct section of the string matrix
		OPTIONAL_LABEL: ori $1, -5, $10
		OPTIONAL_LABEL: bne $10, $20, var
     What the function will get is the latter part of the input
		$1, -5, $10
		$10, $20, var
     using these examples, I will explain this function.*/

   /*If this condition will be evaluated as true it means the function just got a new line character
	instead of 2 registers a variable or a constant like in the examples.*/

   if (length <= 1)
   {
      printf("Missing registers at line %d.\n",line);
      return ERROR;
   }
 
   /* 		,$1, -5, $10
                ^
		,$10, $20, var 
                ^ 
		An out of place comma. */

   if (*input == ',')
   {
      printf("Illegal comma at line %d.\n",line);
      return ERROR;
   }

   /* 		x1, -5, $10
                ^
		missing '$' sign */

   if (*input != '$')
   {
      printf("Invalid first register at line %d.\n",line);
      return ERROR;
   }

   /* 		$\n1, -5, $10
                 ^
		missing register number */

   if (*(input+1) == '\n' || *(input+1) == '\0')
   {
      printf("Missing first register at line %d.\n",line);
      return ERROR;
   }

   /* 		$x, -5, $10
                 ^
		invalid register number */

   if (!('0' <= *(input+1) && *(input+1) <= '9'))
   {
      printf("Invalid first register at line %d.\n",line);
      return ERROR;
   }

   /*Checking the validity of the first register number.*/

   val = strtol(input+1, &temp, BASE10);

   if (val < 0 || REGISTER31 < val)
   {
      printf("First register out of bounds at line %d.\n",line);
      return ERROR;
   } 

   /* 		$1\n -5, $10
                   ^
		$10\n $20, var 
                    ^ 
		Missing parameters, either a register or a constant */

   if (*temp == '\n' || *temp == '\0')
   {
      printf("Missing parameters at line %d.\n",line);
      return ERROR;
   }

   /* 		$1x -5, $10
                  ^
		Missing comma */

   if (*temp != ',')
   {
      printf("Missing the first comma at line %d.\n",line);
      return ERROR;
   }


   /* 		$1,, -5, $10
                   ^  */

   if (*(temp+1) == ',')
   {
      printf("Multiple consecutive commas at line %d.\n",line);
      return ERROR;
   }

   /*Using the repurposed funct section, to indicate to the program whether it should expect a constant or a register that
     is followed by a variable. */

   if (!strcmp(type,"TAKES_A_CONST"))  
   {

   /* 		$1,\n -5, $10
                   ^
		In this course of action, the program should expect a constant. */

      if (*(temp+1) == '\n' || *(temp+1) == '\0')
      {
         printf("Missing a constant after the first register, at line %d.\n",line);
         return ERROR;
      }

   /* 		$1,x-5, $10
                   ^
		In this course of action, the program should expect a constant. */

      if (*(temp+1) != '+' && *(temp+1) != '-' && !('0' <= *(temp+1) && *(temp+1) <= '9'))
      {
         printf("Invalid constant after the first register, at line %d.\n",line);
         return ERROR;
      } 

      val = strtol(temp+1, &temp, BASE10);

   /* 		$1,-5, $10
                   ^
		Since the immed field in the type I command takes 16 bits,
		the constant should be between short min -32768 and unsigned short max 65535. */

      if ( val < SHRT_MIN || USHRT_MAX < val) 
      {
         printf("Constant out of bounds at line %d.\n",line);
         return ERROR;
      } 

   /* 		$1,-5\n, $10
                     ^
		After the constant, the program should expect a register. */


      if (*temp == '\n' || *temp == '\0')
      {
         printf("Missing second register at line %d.\n",line);
         return ERROR;
      }

   /* 		$1,-5x, $10
                     ^
		Missing second comma. */

      if (*temp != ',')
      {
         printf("Missing second comma between the constant and the second register at line %d.\n",line);
         return ERROR;
      }

   /* 		$1,-5,, $10
                      ^ */

      if (*(temp+1) == ',')
      {
         printf("Multiple consecutive commas at line %d.\n",line);
         return ERROR;
      }

   /* 		$1,-5,\n$10
                      ^  */

      if (*(temp+1) == '\n' || *(temp+1) == '\0')
      {
         printf("Missing second register at line %d.\n",line);
         return ERROR;
      }

   /* 		$1,-5,x10
                      ^
		Invalid second '$' sign. */


      if (*(temp+1) != '$')
      {
         printf("Invalid second register at line %d.\n",line);
         return ERROR;
      }

   /* 		$1,-5,$\n
                       ^
		Missing second register. */

      if (*(temp+2) == '\n' || *(temp+2) == '\0')
      {
         printf("Missing second register at line %d.\n",line);
         return ERROR;
      }

   /* 		$1,-5,$x
                       ^
		Invalid second register number. */

      if (!('0' <= *(temp+2) && *(temp+2) <= '9'))
      {
         printf("Invalid second register at line %d.\n",line);
         return ERROR;
      }

      val = strtol(temp+2, &temp, BASE10);

	/*Validity of the second register.*/

      if (val < 0 || REGISTER31 < val) 
      {
         printf("Second register out of bounds at line %d.\n",line);
         return ERROR;
      }

   /* 		$1,-5, $10\n
                          ^
		At this point, the program should expect either a new line or null character. */

      if (*temp == ',')
      {
         printf("Illegal comma at the end at line %d.\n",line);
         return ERROR;
      }

      if (*temp != '\n' && *temp != '\0') 
      {
         printf("Extraneous input after end of command at line %d.\n",line);
         return ERROR;
      }
   }

   /*Using the repurposed funct section, to indicate to the program whether it should expect a constant or a register
     followed by a variable. */

   if (!strcmp(type, "NEEDS_VARIABLE_ADDRESS")) 
   {

   /* 		$10,\n$20, var 
                    ^ 
		In this course of action, the program should expect a register. */

      if (*(temp+1) == '\n' || *(temp+1) == '\0')
      {
         printf("Missing second register at line %d.\n",line);
         return ERROR;
      }

  /* 		$10,x20, var 
                    ^ 
		Missing second '$' sign. */

      if (*(temp+1) != '$')
      {
         printf("Invalid second register at line %d.\n",line);
         return ERROR;
      } 

  /* 		$10,$\n, var 
                     ^ 
		Missing register number. */

      if (*(temp+2) == '\n' || *(temp+2) == '\0')
      {
        printf("Missing second register at line %d.\n",line);
        return ERROR;
      }

  /* 		$10,$x, var 
                     ^ 
		Invalid register. */

      if (!('0' <= *(temp+2) && *(temp+2) <= '9'))
      {
         printf("Invalid second register at line %d.\n",line);
         return ERROR;
      } 

      val = strtol(temp+2, &temp, BASE10);
 
	/*Validity of the second register.*/

      if (val < 0 || REGISTER31 < val)
      {
         printf("Register out of bounds at line %d.\n",line);
         return ERROR;
      } 

  /* 		$10,$20\n, var 
                       ^ 
		At this point, the program should expect a variable. */

      if (*temp == '\n' || *temp == '\0')
      {
         printf("Missing variable at the end, at line %d.\n",line);
         return ERROR;
      }

  /* 		$10,$20x, var 
                       ^ 
		Missing comma. */

      if (*temp != ',')
      {
         printf("Missing second comma between the second register and variable at line %d.\n",line);
         return ERROR;
      }

  /* 		$10,$20,, var 
                        ^ */

      if (*(temp+1) == ',')
      {
         printf("Multiple consecutive commas at line %d.\n",line);
         return ERROR;
      }

  /* 		$10,$20,\n var 
                        ^ 
		Missing variable. */

      if (*(temp+1) == '\n' || *(temp+1) == '\0')
      {
         printf("Missing a variable at the end, at line %d.\n",line);
         return ERROR;
      }

      /*The variable should start with either a capital letter or small letter. */

      if (!('a' <=  *(temp+1) && *(temp+1) <= 'z') && !('A' <=  *(temp+1) && *(temp+1) <= 'Z'))
      {
         printf("Invalid variable at the end, at line %d.\n",line);
         return ERROR;
      }

      /*Finally, the program extracts the variable, 
        and adds it to the check later list, to check the variable validity and if it exists within the program. */

      temp = strtok(temp+1,"\n");
  
      if (strlen(temp) > MAX_LABEL_LENGTH-1)
      {
         printf("Variable length is too long at line %d.\n",line);
         return ERROR;
      }

      put_On_Stand_By(standby, temp, line);
   }


   return VALID;
}


   /*Examines errors related to command of type J. */
int potential_Errors_For_J(checklater_ptr *standby, char **iTable, char *cTable[][TOTAL_COMMAND_COLS], char *input, char *opcode, int length, int line)
{
   long val = 0;
   char *temp = NULL;
   int i = 0;

   /*This function is structured the way it does to give a continual sense of "flow" or "rhythm".
     These are examples of a valid command line of type J
		OPTIONAL_LABEL: jmp $25
		OPTIONAL_LABEL: jmp var
		OPTIONAL_LABEL: la var
		OPTIONAL_LABEL: stop\n
     What the function will get is the latter part of the input
		$25
		var
		\n
     using these examples, I will explain this function.*/

   /* The 'stop' command, whose opcode is 63, should be followed by a new line charater or a null character.*/

   if (!strcmp(opcode,"63"))
   {

      if (input[0] == ',')
      {
         printf("Illegal comma at line %d.\n",line);
         return ERROR;
      }

      if (input[0] != '\0' && input[0] != '\n')
      {
         printf("Extraneous input after end of command at line %d.\n",line);
         return ERROR;
      }
      return VALID;
   }
  
   /* \n
       ^ 
       Got a new line instead of a parameter. */

   if (length <= 1)
   {
      printf("Missing parameter at line %d.\n",line);
      return ERROR;
   }

   /*Checks for illegal commas.*/

   for (i = 0; i < length; i++)
   {
      if (input[i] == ',')
      {
         printf("Illegal comma at line %d.\n",line);
         return ERROR;
      }
   }

   /*The command 'jmp', whose opcode is 30, is the only command that accepts both registers and variables. */

   if (!strcmp(opcode, "30"))
   {
   /*  $25
       ^
       var
       ^
	The program should expect either a '$' or capital letter or small letter. */

      if (*input != '$' && !('a' <=  *input && *input <= 'z') && !('A' <=  *input && *input <= 'Z'))
      {
         printf("Invalid parameter at line %d.\n",line);
         return ERROR;
      } 
      else if (*input == '$')
      {
         /*  $\n
             ^
             Missing number. */

         if (*(input+1) == '\n' || *(input+1) == '\0')
         {
               printf("Missing register at line %d.\n",line);
               return ERROR;
         }

         /*  $x
               ^
               Invalid number. */

         if (!('0' <= *(input+1) && *(input+1) <= '9'))
         {
               printf("Invalid register at line %d.\n",line);
               return ERROR;
         }

 	      /*Validity of the register.*/

         val = strtol(input+1, &temp, BASE10);

         if (val < 0 || REGISTER31 < val) 
         {
            printf("Register out of bounds at line %d.\n",line);
            return ERROR;
         }

        /*  $25\n
               ^
             The program should expect either a null or new line. */

         if (*temp == ',')
         {
            printf("Illegal comma at the end at line %d.\n",line);
            return ERROR;
         }

         if (*temp != '\n' && *temp != '\0')
         {
               printf("Extraneous input after end of command at line %d.\n",line);
               return ERROR;
         }

      }
      else if (('a' <=  *input && *input <= 'z') || ('A' <=  *input && *input <= 'Z'))
      {

      /*The program extracts the variable, 
        and adds it to the check later list, to check the variable validity and if it exists within the program. */

         temp = strtok(input,"\n");

         if (strlen(temp) > MAX_LABEL_LENGTH-1)
         {
            printf("Variable length is too long at line %d.\n",line);
            return ERROR;
         }

         put_On_Stand_By(standby, temp, line);
      } 
 
   }
   else if (!strcmp(opcode, "31") || !strcmp(opcode, "32"))
   {

      /* var
	 ^ 
	 Variable should start with capital or small letter. */

      if (!('a' <=  *input && *input <= 'z') && !('A' <=  *input && *input <= 'Z'))
      {
         printf("Invalid variable at line %d.\n",line);
         return ERROR;
      } 

      /*The 'la' and 'call' commands accept only a variable, so program extracts the variable
       and adds it to the check later list, to check the variable validity and if it exists within the program. */

      temp = strtok(input,"\n");

      if (strlen(temp) > MAX_LABEL_LENGTH)
      {
         printf("Variable length is too long at line %d.\n",line);
         return ERROR;
      }

      put_On_Stand_By(standby, temp, line);

   }


   return VALID;
}

/*Examines errors related to data. */
int potential_Errors_For_Data(char *input, int table_idx, int line)
{
   double val;
   char *temp;
   int double_quotas = 0;

   /*Selects a course of action according to the index of the instruction table.*/

   switch (table_idx)
   {
     
      /*.dh	27056, 18000 This is an example for a valid instruction line of type .dh
	 This function will get the later part of the input (27056,18000).*/

      case DH:

            /*\n
               ^ 
               Missing parameters.*/

            if (*input == ',')
            {
               printf("Illegal comma at the beginning, at line %d.\n",line);
               return ERROR;
            }

            if (*input == '\0' || *input == '\n')
            {
               printf("Missing parameters at line %d.\n",line);
               return ERROR;
            }

            val = strtod(input, &temp);

            /* 		27056,18000
               ^
               Since the instruction of type dh takes 16 bits,
               the values should be between short min -32768 and unsigned short max 65535. */

            if ( val < SHRT_MIN || USHRT_MAX < val) 
            {
               printf("Integer out of bounds at line %d.\n",line);
               return ERROR;
            } 

            while (*temp != '\0' && *temp != '\n')
            {

                     /* 		27056x18000
                           ^
                  After each value is analysed, there should be a comma. */

               if (*temp != ',')
               {
                     printf("Invalid integer at line %d.\n",line);
                     return ERROR;
               }

               /* 		27056,,18000
                        ^ */

               if (*(temp+1) == ',')
               {
                     printf("Multiple consecutive commas at line %d.\n",line);
                     return ERROR;
               }

               if (*temp == ',' && *(temp+1) == '\n')
               {
                     printf("Illegal comma at the end, at line %d.\n",line);
                     return ERROR;
               }

               val = strtod(temp+1, &temp);

               if ( val < SHRT_MIN || USHRT_MAX < val) 
               {
                  printf("Integer out of bounds at line %d.\n",line);
                  return ERROR;
               } 
            }

            if (*temp != '\0' && *temp != '\n')
            {
               printf("Extraneous input after end of data at line %d.\n",line);
               return ERROR;
            }

            break;
 
      case DW:

               /*.dw	31,-12 This is an example for a valid instruction line of type .dw
            This function will get the later part of the input (31,-12).*/

            if (*input == ',')
            {
               printf("Illegal comma at the beginning, at line %d.\n",line);
               return ERROR;
            }

            if (*input == '\0' || *input == '\n')
            {
               printf("Missing parameters at line %d.\n",line);
               return ERROR;
            }

            val = strtod(input, &temp);

         /* 		31,-12
            ^
            Since the instruction of type dw takes 32 bits,
            the values should be between int min -2,147,483,648 and unsigned int max 4,294,967,295. */

            if ( val < INT_MIN || UINT_MAX < val) 
            {
               printf("Integer out of bounds at line %d.\n",line);
               return ERROR;
            } 

            while (*temp != '\0' && *temp != '\n')
            {
                  if (*temp != ',')
                  {
                        printf("Invalid integer at line %d.\n",line);
                        return ERROR;
                  }

                  if (*(temp+1) == ',')
                  {
                        printf("Multiple consecutive commas at line %d.\n",line);
                        return ERROR;
                  }

                  if (*temp == ',' && *(temp+1) == '\n')
                  {
                        printf("Illegal comma at the end, at line %d.\n",line);
                        return ERROR;
                  }

                  val = strtod(temp+1, &temp);

                  if ( val < INT_MIN || UINT_MAX < val ) 
                  {
                     printf("Integer out of bounds at line %d.\n",line);
                     return ERROR;
                  } 
            }

            if (*temp != '\0' && *temp != '\n')
            {
               printf("Extraneous input after end of data at line %d.\n",line);
               return ERROR;
            }

            break;

      case DB:

               /*.db	6,-9 This is an example for a valid instruction line of type .db
            This function will get the later part of the input (6,-9).*/

            if (*input == ',')
            {
               printf("Illegal comma at the beginning, at line %d.\n",line);
               return ERROR;
            }

            if (*input == '\0' || *input == '\n')
            {
               printf("Missing parameters at line %d.\n",line);
               return ERROR;
            }

               val = strtod(input, &temp);

            /* 		6,-9
               ^
               Since the instruction of type dw takes 8 bits,
               the values should be between char min -128 and unsigned char max 255. */

            if ( val < CHAR_MIN || UCHAR_MAX < val) 
            {
               printf("Integer out of bounds at line %d.\n",line);
               return ERROR;
            } 

            while (*temp != '\0' && *temp != '\n')
            {

                  if (*temp != ',')
                  {
                        printf("Invalid integer at line %d.\n",line);
                        return ERROR;
                  }

                  if (*(temp+1) == ',')
                  {
                        printf("Multiple consecutive commas at line %d.\n",line);
                        return ERROR;
                  }

                  if (*temp == ',' && *(temp+1) == '\n')
                  {
                        printf("Illegal comma at the end, at line %d.\n",line);
                        return ERROR;
                  }

                  val = strtod(temp+1, &temp);

                  if ( val < SCHAR_MIN || UCHAR_MAX < val) 
                  {
                     printf("Integer out of bounds at line %d.\n",line);
                     return ERROR;
                  } 
            }

            if (*temp == ',')
            {
               printf("Illegal comma at the end, at line %d.\n",line);
               return ERROR;
            }

            if (*temp != '\0' && *temp != '\n')
            {
               printf("Extraneous input after end of data at line %d.\n",line);
               return ERROR;
            }

            break;

      case ASCIZ:

            /*.asciz    "aBcd" This is an example for a valid instruction line of type .asciz
         This function will get the later part of the input ("aBcd").*/

         if (*input == '\0' || *input == '\n')
         {
            printf("Missing parameters at line %d.\n",line);
            return ERROR;
         }

         /*The input should start with ".*/

         if (*input == ',')
         {
            printf("Illegal comma at the beginning of a string, at line %d.\n",line);
            return ERROR;
         } 

         if (*input != '"')
         {
            printf("Missing \" at the beginning of a string, at line %d.\n",line);
            return ERROR;
         } 

         input++;

         while (*input != '\0' && *input != '\n')
         {
            /*The input should end with " and have new line or null after it.*/

            if (double_quotas)
            {
                     printf("Extraneous input after end of string at line %d.\n",line);
                     return ERROR;
            }

            if (*input == '"')
            {
                  double_quotas++;
            }

            input++;
         }

         if (!double_quotas)
         {
                  printf("String not closed properly at line %d.\n",line);
                  return ERROR;
         }

         if (*input != '\0' && *input != '\n')
         {
            printf("Extraneous input after end of data at line %d.\n",line);
            return ERROR;
         }

   }

   return VALID;
}
