/*
	This program checks if the input is a valid guidance word with valid parameters.
*/

#include "assembler.h"


/*
 *  The `guidance` function processes a line of input that includes a guidance word followed by its parameters.
 *  It first checks if the initial word in the line is a valid guidance keyword. If the keyword is valid, 
 *  the function verifies the subsequent parameters by invoking the appropriate validation functions.
 * 
 *  @param buffer_guide: Memory image for storing data
 *  @param buffer_inst: Memory image for storing instructions
 *  @param IC: Pointer to the instruction counter
 *  @param DC: Pointer to the data counter
 *  @param lineNum: Line number of the current line in the input file
 *  @param word: String containing the guidance keyword and the entire line
 *  @param head: Pointer to the label list
 *
 *  @return  Returns 0 if the guidance keyword and parameters are valid; otherwise, returns -1.
 */
int guidance(unsigned int buffer_guide[MEM_IMG_SIZE], unsigned int buffer_inst[MEM_IMG_SIZE], int *IC, int *DC, int lineNum, char *line, struct label_node **head, struct macro_att *macros_table) 
{
	/* variable declarations */
    int i;
    char *param;
    char c; /* will indicate extern or entry label */
    
    /* variable initialization */
    i = 0;

	
    param = strtok(line," \r\n\t"); /* param = ".data" or ".string" or ".entry" or ".extern" or something else */
	if(*(param + (strlen(param) - 1)) == ':') /* label befor the guidance */
	{
	    param = strtok(NULL," \r\n\t");
	}
	/* .data check */
	if (strcmp(param, ".data") == 0) 
	{
	    param = strtok(NULL,"\r\n"); /* param = the parameters if they are valid */
	    
        if(data_params_validity(param, lineNum) == ERROR)
		{
		    return ERROR;
		}
		
		param = strtok(param," \t\r\n,"); /* tokenize ignoring comma */

    	while (param != NULL) 
	    {
			buffer_guide[*DC+MEM_FIRST_INDEX] = twos_complement_value(atoi(param));
    		(*DC)++;
		    param = strtok(NULL," \t\r\n,");
    	}
    }

	/* .string check */
	else if (strcmp(param, ".string") == 0) 
	{ 
	    param = strtok(NULL,"\r\n"); /* reading the parameter (the string) */
	    while (isspace(*param)) 
	    {
            param++;
        }
        
	    if(param == NULL)
		{
			printf("Error: in line %d: Missing parameter\n",lineNum);
			return ERROR;
		}
		
	    if(string_params_validity(param, lineNum) == ERROR)
		{
		    return ERROR;
		}
		
		else
		{
			i = 1;
    		while (param[i] != '"')
    		{
    			buffer_guide[*DC+MEM_FIRST_INDEX] = param[i]; /* the ascii code will be inserted */
    			(*DC)++;
    			i++;
    	    }

    	    /* null have to be at the end of the string */
    	    buffer_guide[*DC+MEM_FIRST_INDEX] = 0;
    		(*DC)++;
	    }
	} /* end of .string check */

	/* .entry or .extern check */  
  	else if (strcmp(param, ".entry") == 0 || strcmp(param, ".extern") == 0) 
	{
	    c = param[2]; /* 'n' for entry and 'x' for extern */
		param = strtok(NULL,"\r\n");
		
        /* no parameters after guidance word */
		if (param == NULL) 
		{
			printf("Error: in line %d: Missing parameter\n",lineNum);
			return ERROR;
		}

        /* invalid label */
		if (label_validity(param, 0, lineNum) == ERROR) 
		{
			printf("Error in line %d: Invalid label syntax\n",lineNum);
			return ERROR;
		}
		
		if(c == 'x')
		{
	        if(defined_macro(param, macros_table) != NULL)
            {
                printf("Error: in line %d, extern label can't have the same name of a macro\n", lineNum);
                return ERROR;
            }
		    add_label(head, param, -1, lineNum, 'x');
		}
	    param = strtok(NULL,"\n \t\r");
	    if(param != NULL)
        {
    		printf("Error in line %d: Extra text after label\n",lineNum);
       	 	return ERROR;
    	}  
	}  /* end of .entry or .extern check */

    return 0;   
} /* end of guidance function */

/*
*  Validates the parameters passed with a `.data` directive in assembly code.
*  
*  This function checks if the input string contains valid numeric parameters 
*  separated by commas. It ensures proper formatting (no misplaced commas, and only valid 
*  numeric characters or '+' and '-' signs). 
*  If an error is found, a corresponding error message is printed with the line number.
*  
*  @param param: The input string to be validated
*  @param lineNum: The current line number for error reporting
*  @return  Returns `ERROR` if the parameters are invalid, otherwise 0
*/
 
int data_params_validity(char param[MAX_LINE_LEN], int lineNum)
{
    /* variable declarations */
    int i;
    int para_num;
    char recent_char; /* the last char we read */
    
    /* variable initialization */
    i = 0;
    para_num = 1;

    if(param[0] == '\0')
    {
        printf("Error in line %d, Missing parameters after .data.\n",lineNum);
    }
    /* skip spaces before the parameters */
    while (isspace(param[i])) 
    {
        i++;
    }

    /* check if the first character is a comma or an invalid character */
    if (param[i] == ',') 
    {
        printf("Error in line %d, comma before parameters.\n", lineNum);
        return ERROR;  /* invalid string */
    }

    while (param[i] != '\0') 
    {
        /* skip spaces before a comma or number */
        while (isspace(param[i]))
        {
            i++;
        }

        /* check if the current character is a digit */
        if (isdigit(param[i]) || param[i] == '+' || param[i] == '-')
        {
            i++;
            /* extract number to result string */
            while (isdigit(param[i])) 
            {
                i++;
            }

            /* skip spaces after the number */
            while (isspace(param[i])) 
            {
                i++;
            }

            /* check if the next character is a comma or end of string */
            if (param[i] == ',')
            {
                recent_char = param[i]; /* storing the most recent character */
                i++;  /* include the comma in the result */
                
                /* skip spaces after the comma */
                while (isspace(param[i]))
                {
                    i++;
                }

                /* check if there is a number or end of string after spaces */
                if (param[i] == '\0') 
                {
                    para_num++;
                    continue;  /* valid end of string after a comma */
                } 
                else if (!isdigit(param[i]) && param[i] != '+' && param[i] != '-') 
                {
                    if(param[i] != ',')
                    {
                        printf("Error in line %d, comma not followed by a number.\n", lineNum);
                    }
                    else
                    {
                        printf("Error in line %d, multiple commas between parameter %d and parameter %d.\n", lineNum, para_num, para_num+1);
                    }
                    return ERROR;  /* invalid string */
                }

            } 
            else if (param[i] == '\0') /* ended a with number */
            {
                return 0;
            } 
            else 
            {
                if(isdigit(param[i]) || param[i] == '+' || param[i] == '-')
                printf("Error in line %d, bad attempt to insert 2 parameters (%d and %d) without comma\n", lineNum, para_num, para_num+1);
                return ERROR;  /* invalid string */
            }
        } 
        else 
        {
            printf("Error in line %d, invalid character found as parameter number %d.\n", lineNum, para_num);
            return ERROR;  /* invalid string */
        }
    }

    /* if we finish processing and end with a comma or invalid characters */
    if(recent_char == ',')
    {
        printf("Error in line %d, comma no allowed after parameters.\n", lineNum);
    }
    else
    {
        printf("Error: String ends with an invalid character.\n");
    }
    return ERROR;  /* invalid string */
} /* end of data_params_validity function */

/*
*  The `string_params_validity` function validates the format of a string parameter 
*  from the input. It ensures that the string is properly enclosed in quotation marks 
*  and that there are no extra characters after the parameter.
* 
*  @param param: The input string containing the parameter to be validated
*  @param lineNum: The line number of the current line in the input file
* 
*  @return  Returns 0 if the string parameter is valid; otherwise, returns -1.
*/
int string_params_validity(char param[MAX_LINE_LEN], int lineNum)
{
    /* variable declarations */
    char *param2;
    
    param2 = strtok(param,"\r\n");
	if (param2 == NULL) /* no parameters after guidance word */
	{ 
		printf("Error: in line %d: Missing parameter\n",lineNum);
		return ERROR;
	}

	if(param2[0] != '"') /* quotation have to be at the first of the string */
	{
    	printf("Error in line %d: Missing quotation marks before string\n",lineNum);
	    return ERROR;
	}
    
	if(param2[strlen(param2)-1] != '"') /* quotation have to be at the end of the string */
   	{
        printf("Error in line %d: Missing quotation marks at the end of the string\n",lineNum);
    	return ERROR;
	}
	
    param2 = strtok(NULL," \r\n\t"); /* check extra text */
    if(param2 != NULL)
    {
        printf("Error in line %d: Extra text after the parameter\n",lineNum);
		return ERROR;
    }

    return 0;
} /* end of string_params_validity function */



