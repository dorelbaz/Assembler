#include "Data_File.h"

void secondpassage_Setup(FILE *fp, bmc_ptr *bmc, symbol_ptr *symbols, data_ptr *data, checklater_ptr *standby, int *errs, int IC, int DC, char **iTable, char *cTable[][TOTAL_COMMAND_COLS])
{
    /*Printing what the linked lists hold so far.*/
    print_List(standby, STANDBY);
    print_List(symbols,SYMBOL);
    print_List(bmc,BMC);
    print_List(data,DATA);

    /*A pre second passage error examination.*/
    all_Variables_Exist(*symbols, *standby, errs);
    all_Variables_Defined_Once(*symbols, errs);
    valid_Variables_In_Checklater_List(*standby, errs, cTable, iTable);
    valid_Conditional_Branching(*symbols, *bmc, errs);

    if (!errs)
	{
        puts("\nSecond Passage:");

        /*Fourthly, now that the program is about to start the second passage, it resets the file pointer.*/
        fseek(fp,0,SEEK_SET);

        secondPassage(fp,bmc,symbols,data,errs,IC,DC,iTable,cTable);
    }
}