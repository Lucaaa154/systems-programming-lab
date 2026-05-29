/* 
   Preassembler to expand the macros
   The Preassembler checks macros definition, and keep the macros table
   to the next step to check if there is no label with the same name of the macro. 
*/

#include "assembler.h"

/*
*   Expands all macros defined in the given file and checks the validity of their definitions.
* 
*   This function processes the provided file to read and expand all macros based on their definitions. 
*   It updates the macro table with the expanded macros and writes the resulting content to a new file. 
*   The function also verifies that macro definitions are valid according to the specified criteria.
* 
*   @param fp: A pointer to the original file from which macros are read and expanded.
*   @param wr_fp: A pointer to the file where the expanded content will be written.
*   @param file_name: The name of the file being processed.
*   @param table_head: A pointer to the head of the macro table, which stores the defined macros.
* 
*   @return: An integer indicating the success or failure of the macro expansion process.
*/

int macro_expansion(FILE *fp, FILE *wr_fp, char *file_name, struct macro_att **table_head)
{
	/* variable declarations */
    struct macro_att *table_tail;
    struct macro_att *macro_pointer;
    char line[MAX_LINE_LEN]; /* a string to store every single line from the program */
    char bkup_line[MAX_LINE_LEN]; /* the same line to backup (strtok will modify line) */
    char *word;
    int error_flag;
    int lineNum;
    
    /* variable initialization */
    table_tail = NULL; /* tail of macros table */
    macro_pointer = NULL;
    error_flag = 0; /* 0 when no error found */
    lineNum = 1; /* the number of the line in the file with .as ending */
    	
    memset(line, '\0', sizeof(line)); /* clear the string line */
    memset(bkup_line, '\0', sizeof(bkup_line)); /* clear the string bkup_line */
    
	while(fgets(line, sizeof(line), fp)) /* reads the first character in the line */
	{	
	    memmove(bkup_line,line,strlen(line)); /* backup line in bkup_line */
	    bkup_line[strlen(line)] = '\0';
    	word = strtok(line," \r\n\t");
        
        /* if this is a macro name */
        if ((*table_head != NULL) && (word != NULL) && ((macro_pointer = defined_macro(word, *table_head)) != NULL)) 
    	{  /* assuming that the macro is defined before using it in the source file */

    	    word = strtok(NULL," \r\n\t"); /* after macro name */
    	    if (word == NULL)
    	    {
        	    expand_macro(wr_fp,macro_pointer);
    	    }
    	    else
    	    {
                printf("Error: in line %d, extra text after the name of the macro %s\n", lineNum, word);
    	        error_flag = ERROR;
    	    }
	    }
	    
    	else if(word != NULL && (strcmp(word,MACR_DEF) == 0)) /* defining new macro */
    	{
            word = strtok(NULL," \r\n\t"); /* macro name */
            
            /* a valid line length in this program is 80, so (80 - (macro definition word)) = (80 - 4) = 76 is the max macro name length */
            if(strlen(word) > 76)
            {
                error_flag = ERROR;
                printf("Error: in line %d, lenght of name of the macro \"%s\": shouldn't be greater than 76 (valid line is 80 characters)\n",lineNum, word);
            }
            
        	else if((instruction_validity(word) != ERROR)) /* macro name shouldn't be an instruction word */
        	{
        	    error_flag = ERROR;
        		printf("Error: in line %d, name of the macro \"%s\": shouldn't be an instruction word\n", lineNum, word);
        	}
        	
        	else if((guidance_validity(word) != ERROR)) /* macro name shouldn't be a data word */
        	{
        	    error_flag = ERROR;
        		printf("Error: in line %d, name of the macro \"%s\": shouldn't be a data word\n", lineNum, word);
        	}
        	
        	else if((word != NULL) && ((strcmp(word,MACR_DEF) == 0) || (strcmp(word,MACR_END) == 0))) /* macro name shouldn't be a macro definition word ("macr" or "endmacr") */
        	{
        	    error_flag = ERROR;
        		printf("Error: in line %d, name of macro \"%s\": shouldn't be macro definition words\n", lineNum, word);
        	}

			else if(defined_macro(word, *table_head) != NULL) /* multiple definition of a macro */
            {
                error_flag = ERROR;
        		printf("Error: in line %d, multiple definition of the macro \"%s\"\n", lineNum, word);
            }
            
        	word = strtok(NULL," \r\n\t"); 
        	if(word != NULL) /* extra text after macro name */
        	{
        	    error_flag = ERROR;
                word = strtok(bkup_line," \r\n\t"); /* word = "macr" */
                word = strtok(NULL," \r\n\t"); /* word = macro name */
            	printf("Error: in line %d, extra text after the definition of the macro %s\n", lineNum, word);
        	}
            
            if(error_flag != ERROR) /* it's a valid macro definition */
            {
                memmove(line,bkup_line,strlen(bkup_line));
                line[strlen(bkup_line)] = '\0';
                word = strtok(line," \r\n\t"); /* word = "macr" */
                word = strtok(NULL," \r\n\t"); /* word = macro name */
        	    error_flag = add_macro (fp, file_name, table_head, &table_tail, bkup_line, &lineNum); /* adding to the macro list with its lines (till endmacr) */
            }
        }
        else /* any other line will be printed automatically in the expanded file */
    	{
    	    /* we will use bkup_line because line has modified by strtok */
            fputs(bkup_line,wr_fp); /* the line itself */
    	    while((strchr(bkup_line,'\n') == NULL && (strchr(bkup_line,'\r') == NULL)) && !feof(fp))
    	    {
    	        memset(bkup_line, '\0', sizeof(bkup_line)); /* clear the string line */
    	        fgets(bkup_line, sizeof(bkup_line), fp); /* getting the rest of the line */
    	        /* maybe the line is more than 80 characters, we will check that in first pass */
                fputs(bkup_line,wr_fp); /* the word itself */
    	    }
        }
        lineNum++;
        memset(line, '\0', sizeof(line)); /* clear the string line before next iteration */
        memset(bkup_line, '\0', sizeof(bkup_line)); /* clear the string bkup_line before next iteration */
    } /* end of the first while loop */
    if (error_flag == ERROR)
    {
		return ERROR;
    }
    	
	return 0;
} /* end of macro_expansion */

    
