/* Preassembler helping functions file */

#include "assembler.h"

/*
*   Handles file operations for each file specified in the arguments.
* 
*   This function manages the opening, processing, and closing of files. It processes each file based on
*   the provided filename, handles file endings related to macros, and prepares the files for further 
*   operations. After processing, it ensures that all opened files are properly closed.
* 
*   @param file: The name of the file without endings to be processed.
*/
void files_handle(char *file)
{
    /* variable declarations */
    int error_flag;
    int len;
    FILE *fp;
    FILE *wr_fp;
    char *file_name;
    struct macro_att *macros_table;
    
    /* variable initialization */
    error_flag = 0; /* error flag of the return value of function */
    len = strlen(file) + 5; /* 5 added for the files endings */
    fp = NULL; /* pointer of the file to read (.as ending) */
    wr_fp = NULL; /* pointer of the file to expand macros (.am ending) */
    file_name = (char*) malloc(sizeof(char) * len); /* allocating memory for the file name */
    macros_table = NULL; /* macros table pointer */

	
	if (file_name == NULL)
	{
		printf("can't allocate memory for file_name");
		return;
	}

	/* the .as ending addition */
    memset(file_name, '\0', sizeof(char) * len); /* set the string file_name to '\0's */
	memmove(file_name, file,strlen(file)); /* copying the received string to the file_name string */
	file_name[strlen(file)] = '\0';
	file_name[strlen(file)] = '\0';
	convert_ending(file_name,1); /* add the ".as" ending */
	fp = fopen(file_name,"r"); /* opening the file (with the ".as" ending) for read */

    /* openning failed */
	if (fp == NULL)
	{
		if(file_name != NULL) /* freeing the string that we allocated to store the file name */
		printf("the attempt to open the file %s has failed.\n", file_name);
		if(file_name != NULL)
		{
		    free(file_name);
		}
		return;
	}
	
	printf("Reviewing Errors and Warnings for file %s:\n",file_name);
	convert_ending(file_name,2); /* add the ".am" ending */
	wr_fp = fopen(file_name,"w+"); /* opening the file that we want write in it */
	if (wr_fp == NULL) /* macros expansion file openning failed */
	{
		printf("the attempt to open the file %s for the macros expansion has failed\n", file_name);
        if(file_name != NULL)
		{
		    free(file_name);
		}
        return;
    }
    
	error_flag = macro_expansion(fp, wr_fp, file_name, &macros_table); /* storing the returning value of the attempt of creating the expanded macros file */
	
	if(fp != NULL) /* closing the file with ".as" ending */
	{
	    fclose(fp); 
	}
	fp = NULL;
	
	if (error_flag == ERROR) /* error in macros expansion */
	{
        goto linefree; /* jump and don't continue for the first and second pass */
	}
	
	printf("Reviewing Errors and Warnings for file %s:\n",file_name);
	rewind(wr_fp); /* to start reading from the beginning of the expanded file */
	first_pass(wr_fp,file_name,macros_table); /* second pass called from first pass */
	
	/* from this line, the function starts freeing memory and closing files */
	linefree: if(macros_table != NULL)
	{
        free_macros(macros_table);
	}
	
    if(error_flag == ERROR && remove(file_name) != 0) /* removing the file has failed */
    { 
        printf("Warning: the file %s is a garbage file! \n", file_name);
    }
    
	if (file_name != NULL) /* free the allocated memory for the file name */
	{
		free(file_name);
	}
    
	if (wr_fp != NULL) 
	{
		fclose(wr_fp); /* with the .am ending */
	}
} /* end of function files_handle */

/*
*   Converts the file extension of the given file name to a new extension based on the specified step.
* 
*   This function modifies the file name by changing its extension to the appropriate type as defined by 
*   the step parameter. It assumes that the file name string has enough length to contain the new 
*   extension. The function handles different types of file endings such as ".as", ".am", ".ob", ".ent", 
*   and ".ext".
* 
*   @param file_name: The name of the file whose extension is to be converted. This string will be modified in place to reflect the new extension.
*   @param step: An integer indicating the new file extension to be used. The mapping is as follows:
*                 (1) for ".as", (2) for ".am", (3) for ".ob", (4) for ".ent", (5) for ".ext".
*/
void convert_ending(char *file_name, int step)
{

    /* variable declarations */
    int len;
    char *ind_ptr; /* finding the '.' index to modify the file ending */
    int index; /* index to the char '.' */
    char ending[5]; /* array to hold the ending */
    
    /* variable initialization */
    len = strlen(file_name); /* file name length */
    ind_ptr = NULL; /* initializing ind_ptr to NULL for safety */
    index = 0; /* initializing index to 0 */
    strcpy(ending, "."); /* initializing the ending with '.' */

    
    ind_ptr = strchr(file_name, '.'); /* points to the index where '.' apears */
    /* if '.' found in the string */
    if (ind_ptr != NULL)
    {
        index = strchr(file_name, '.') - file_name; /* index to the char '.' */
        /* removing the chars after the '.' */
        while((*(file_name + index) != '\0') && (len - index >= 0))
        {
            *(file_name + index) = '\0';
            index ++;
        }
    }
    
    switch(step)
    {
        /* .as ending */
        case(1): strcat(ending, "as"); break; /* add "as" ending */
        
        /* .am ending (after macros expansion) */
        case(2): strcat(ending, "am"); break; /* add "as" ending */
        
        /* .ob ending (object file) */
        case(3): strcat(ending, "ob"); break; /* add "as" ending */
        
        /* .ent ending (entry file) */
        case(4): strcat(ending, "ent"); break; /* add "ent" ending */
        
        /* .ext ending (extern file) */
        case(5): strcat(ending, "ext"); break; /* add "ext" ending */
    }
    
    strcat(file_name, ending);
} /* end of function convert_ending */

