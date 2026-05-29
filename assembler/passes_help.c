/* This file contains helping functions for the first and second pass */

#include "assembler.h"

/* global instructions list */
char instructions_list[16][5] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
/* global data list */
char guidance_list[4][8] = {".data", ".string", ".entry", ".extern"};


/* 
*  The function line_type determines the type of the received string by checking if it represents 
*  an instruction, a guidance sentence, or a label. It returns a character indicating the type.
*
*  @param word: The received string to be checked. This could be an instruction name, a guidance directive, or a label.
*
*  @return: 
*    - 'i' if the received word is identified as an instruction.
*    - 'g' if the received word is identified as a guidance.
*    - 'l' if the received word is identified as a valid label syntax.
*    - 'N' (null character) if the received word does not match any known types.
*/
char line_type(char *word, int lineNum)
{
    /* checking the line type by a specific function */
	
	
	if (instruction_validity(word) != ERROR)
	{
        	return 'i'; /* its instruction */
	}

	if((guidance_validity(word)) != ERROR)
	{
        	return 'g'; /* its guidance */
	}
	
    if (label_validity(word, 1, lineNum) != ERROR)
	{
        	return 'l'; /* its valid label syntax */
	}
	return '\0'; /* for any other line type */
} /* end of line_type function */


/* 
*  The function checks if the received string can be a valid label based on naming conventions and
*  whether it is a definition or an operand label.
*
*  @param word: The string to check if it can be a valid label. This string should conform to label naming rules.
*  @param def_bit: A bit indicating the type of label:
*                    1 if it is a definition label.
*                    0 if it is an operand label.
*
*  @return: 
*     0 if the string is a valid label based on naming conventions and the label type.
*     -1 if the string does not conform to valid label rules.
*/
int label_validity (char *word,int def_bit, int lineNum)
{
    /* variable declarations */
    int i;
    char d;
    char temp_word[MAX_LABEL_LEN]; /* to handle the two cases: label as an operand, label as a definition */
    
    /* variable initialization */
    i = 0;
    d = word[strlen(word)-1];

    
    memset(temp_word, '\0', sizeof(temp_word)); /* clear the string temp_word */
    
    if(word == NULL || (!islower(word[0]) && !isupper(word[0]))) /* the first character should be a-z or A-Z */
    {   
        if(d == ':') /* guidance */
        {
            printf("Error: in line %d, label should start with capital/small character only\n", lineNum);
        }
        return ERROR; /* not valid */
    }
    
    if(strlen(word) > MAX_LABEL_LEN) /* label max length is 31 */
    {
        if(def_bit == 1) /* label definition */
        {
            printf("Error: in line %d, label length should be less than 32\n", lineNum);
        }
        return ERROR;
    }
    
    if(def_bit == 1) /* defining a label contains ':' */
    {
        strncpy(temp_word,word,strlen(word)-1); /* copying to temp_word without ':' to check the label itself */
    }
    else
    {
        strncpy(temp_word,word,strlen(word)); /* label as an operand dosen't contain ':' */
    }
    /* note: word is still without modification, if we modified so we did for temp_word */
    if(word[strlen(word)-1] != ':' && def_bit == 1) /* defining label without ':' */
    {
        printf("Error: in line %d, label definition should end with :\n", lineNum);
        return ERROR;
    }
    
    if(instruction_validity(temp_word) != ERROR) /* label cant be name of instruction */
    {
        if(def_bit == 0) /* operand */
        {
            printf("Error: in line %d, label as operand can't be the same name of an instruction\n", lineNum);
        }
        if(d == ':')
        {
            printf("Error: in line %d, label definition can't be the same name of an instruction\n", lineNum);
        }
        return ERROR;
    }
    if(guidance_validity(temp_word) != ERROR) /* label cant be name of guidance */
    {
        if(def_bit == 0) /* operand */
        {
            printf("Error: in line %d, label can't be the same name of an guidance\n", lineNum);
        }
        if(d == ':')
        {
            printf("Error: in line %d, label definition can't be the same name of an guidance\n", lineNum);
        }
        return ERROR;
    }
    if(register_validity(temp_word) != ERROR)
    {
        if(def_bit == 1) /* not operand */
        {
            printf("Error: in line %d, label definition can't be the same name of a register\n", lineNum);
        }
        return ERROR;
    }
    
    /* first character has checked at the beginning */
    for(i=0;i<strlen(temp_word);i++) /* checking the validity of all the characters of the label */
    {
        if(!islower(word[i]) && !isupper(word[i]) && !isdigit(word[i])) /* if not a number or A-Z or a-z, its invalid label */
        {
            printf("Error: in line %d, invalid %s label, label template should be: starts with capital/small character only and after that can contain capital/small characters or digits\n", lineNum, def_bit == 0? "operand":"definition");
            return ERROR;
        }
    }
    
    return 0; /* valid label */
} /* end of label_validity function */


/* 
   The function checks if the received string matches a defined instruction name and
   returns the corresponding opcode. If the string does not match any defined instruction,
   the function returns -1.

   @param word: A string representing the potential instruction name to be checked.

   @return: 
     The opcode associated with the valid instruction name if the string is a valid instruction.
     -1 if the string does not correspond to a valid instruction name.
*/
int instruction_validity(char *word)
{
    /* variable declarations */
    int i;
    int opcodes_num;
    
    /* variable initialization */
    opcodes_num = 16; /* 16 is the number of the instructions. opcodes (0-15) */

    for(i=0;i<opcodes_num;i++) /* passing over all the array of instructions names */
    {
        if((word != NULL) && (strcmp(word, instructions_list[i]) == 0)) /* if we  find it we will return its opcode */
	{
        return i; /* the appropriate opcode */
	}
}
    return ERROR; /* not a name of instruction */
    
} /* end of instruction function */


/* 
   The function checks if the received string matches one of the guidance types 
   and returns its index in the global `guidance_list`. If the string does not 
   correspond to a defined guidance type, the function returns -1.

   @param word: A string representing the potential guidance type to be checked.

   @return: 
     The index in the `guidance_list` array if the string matches a valid guidance.
     -1 if the string does not correspond to a defined guidance type.
*/
int guidance_validity(char *word)
{
   /* variable declarations */
    int i;
    int guidance_num;
    
    /* variable initialization */
    guidance_num = 4; /* number of the data types */

    
    for(i=0;i<guidance_num;i++) /* passing over all the list */
    {
        if ((word != NULL) && (strcmp(word, guidance_list[i]) == 0 || strcmp(word, guidance_list[i] + 1) == 0)) 
	    {
        	return i; /* returns the appropriate index of the guidance type */
	    }
    }

    return ERROR;
} /* end of guidance_validity function */

/*
 * The function num_validity checks if the received string represents a valid integer. 
 * The integer may include optional signs (+ or -) and should fall within a specified range 
 * depending on its intended use (e.g., instruction or guidance).
 * 
 * @param word: A string to check if it is a valid integer.
 * @param t: Indicates the context in which the number is used. This can affect the maximum 
 *           allowable value for the integer. (instruction or guidance).
 * 
 * @return: 
 *   - 1 if the string represents a valid integer within the acceptable range.
 *   - ERROR if the string is not a valid integer or is out of range.
 */
int num_validity(char *word,int lineNum,char t)
{
	/* variable declarations */
    int i;

    /* variable initialization */
    i = 0;

	if(word[i] == '-' || word[i] == '+') /* checks if this is negative number so we can continue checking */
	{
		i++;
	}    

	while(word[i] != '\0') /* number validity test */ /* loop1 */
	{
    	if(!(isdigit(word[i]))) /* using isdigit() function */
	{
    		return ERROR; /* we give a sign that this is not valid number */
	}
    
    	i++;
	} /* end of loop1 */

	if ((t == 'g') && (atoi(word) > MAX_GUIDANCE_NUM || atoi(word) < -MAX_GUIDANCE_NUM-1)) /* if the number is not in the range of 10 bits (two other bits for A.R.E)*/
	{
	    printf("Error: the number in line %d can not be represented in 15 bits\n", lineNum);
    	return ERROR;
	}
	
	if ((t == 'i') && (atoi(word) > MAX_INSTRUCTION_NUM || atoi(word) < -MAX_INSTRUCTION_NUM-1)) /* if the number is not in the range of 10 bits (two other bits for A.R.E)*/
	{
	    printf("Error: the number in line %d can not be represented in 12 bits\n", lineNum);
    	return ERROR;
	}

	return 0; /* we give a sign that this is valid number */
} /* end of num_validity function */

/*
 * The function add_label adds a valid label to the symbol table.
 * The label is added with its name, address, and type. The function assumes the label is valid.
 * 
 * @param head: A pointer to the head of the symbol table (labels list).
 * @param label: The name of the label to be added.
 * @param dec_num: The address associated with the label.
 * @param line_num: The line number in the source file where the label was defined.
 * @param c: The type of the label. This indicates whether the label is an entry (n), external(x), instruction(i) or data(d).
 * 
 * @return: 
 *   0 if the label was successfully added to the symbol table.
 *   ERROR (-1) if there was an issue adding the label.
 */
int add_label(struct label_node **head, char* label, int dec_num, int line_num,char c) 
{  
    /* variable declarations */
	/* checks if the label already exists in the list */
	struct label_node* current = *head; /* pointer to current label node */
	struct label_node* last; /* last node */
	struct label_node* new_node; /* new node that may be created */

	if(c == 'n') /* in the case of .entry. note: assuming that entry label will be declared as entry only after defining */
	{
    	while (current != NULL) /* till the last label node */ /* loop1 */
    	{
    		if ((strcmp(current->label, label) == 0)) /* label name found in the labels list */
    		{
    		    if((current->label_type) == 'n') /* when this label has declared as entry also before */
        	    {
    		    	printf("Warning: Label from line %d has set as multiple times entry label\n", line_num);
    			    return 0;
        	    }
        	    
    		    else if((current->label_type) == 'x') /* when this label has declared as .extern before */
        	    {
    		    	printf("Error: Label from line %d has declared before as extern label, it can't be declared also as entry label\n", line_num);
    			    return ERROR;
        	    }
        	    else
        	    {
                    (current->label_type) = 'n'; /* set it as entry label */
            		return 0; 
        	    }
    		}
        	current = current->next; /* going to next label node */
    	} /* end of loop */
    printf("Error: Label from line %d is not defined in the source code\n", line_num);
	return ERROR; /* the label is not defined in the source code */
	}
    
	else if(c == 'x') /* in the case of .extern */
	{
    	while (current != NULL) /* loop2 */
    	{
    		if (strcmp(current->label, label) == 0) /* label found*/
    		{
    		    switch((current->label_type))
    		    {
            		case('n'): /* if has set before as entry */
            		{
            			printf("Error: Label from line %d has set before as entry label, it can't be set also as extern label\n", line_num);
            			return ERROR;
            		}
                    break;

            		case('x'): /* it has declared, or updated */
            		{
            		    if(dec_num == -1) /* if its declared one more time as .extern we will print a warning */
            		    {
            		        printf("Warning: Label from line %d has set as multiple times extern label\n", line_num);
                		    return 0;
            		    }
            		    else if(current->dec_num == -1)
            		    {
            		        current->dec_num = dec_num;
            		        return 0;
            		    }
            		}
                    break;
            		case('i'): /* label without extern or entry */
            		case('d'):
            		{
                		printf("Error: Label from line %d has set before, it can't be set also as extern label\n", line_num);
                		return ERROR;
            		}
            		break;
    		    }
   		    }

		current = current->next;
    	} /* end of loop2 */
	}
    
    else if(c == 'i' || c == 'd') /* it's a label at the beginning of the line "label:" */
    {
    	while (current != NULL) /* loop3 */
    	{
    	    if(strcmp(current->label, label) == 0) /* the label has defined before */
			{
			    if((current->label_type) == 'x')
			    {
			        printf("Error: Label from line %d has declared before as extern label, it can't be declared also in the source file\n", line_num);
    			    return ERROR;
			    }
            	printf("Error: Label from line %d, multiple label definition\n", line_num);
            	return ERROR;
            }
            
            current = current->next;
    	}
    } /* end of loop3 */
    
	/* creates new node */
	new_node = (struct label_node*) malloc(sizeof(struct label_node)); /* allocating memory for new label node */

	if (new_node == NULL) /* if allocating memory failed */
	{
    	printf("No enough memory to allocate in line %d, in function \"add_label\"\n", line_num);
    	return ERROR;
	}
  
	/* adds data to the new node */
	memmove(new_node->label, label, strlen(label)); /* giving the name of the label to the new node */
	new_node->label[strlen(label)] = '\0';
	new_node->label[strlen(label)] = '\0';
    new_node->dec_num = dec_num; /* giving the label address to the new node */
   	new_node->next = NULL; /* because it is the last node */
   	new_node->label_type = c; /* updating the label type */

	/* the new node is the first node in the list */
	if (*head == NULL) 
	{ /* adds the node to the head of the list */
    	*head = new_node;
    	return 0;
	}
	
	/* the new node isn't the first node in the list */
  	last = *head;
  	while (last->next != NULL) 
  	{  /* traverse to the end of the list */
  		last = last->next;
  	}

	last->next = new_node; /* adds the new node to the end of the list */
	return 0;
} /* end of add_label function */

/*
 * Function to update the data labels after the first pass and add the Instruction Counter (IC) to labels on data or strings.
 * This function iterates through the list of labels and updates their addresses by adding the Instruction Counter (IC) to those
 * labels that are used in data or string definitions.
 * 
 * @param head: A pointer to the head of the labels list (symbol table).
 * @param IC: The Instruction Counter value to be added to the addresses of data labels.
 */

void update_label_list(struct label_node *head, int IC)
{
    /* variable declaration */
    struct label_node *current;
    
    /* variable initialization */
    current = head; /* start from the head */

    
    while(current != NULL) /* update all the data labels */
    {
        if(current->label_type == 'd') /* label type is data */
        {
            current->dec_num += IC; /* adding the ic to every data label */
        }
        current = current->next;
    }
}

/*
* Function to calculate the two's complement representation of a given integer.
* If the input integer is non-negative, the function simply returns it as is.
* If the input integer is negative, it calculates and returns its two's complement representation in decimal.
* 
* @param value: The input integer to be converted to two's complement if necessary.
*
* @return: The decimal representation of the integer in two's complement form if negative, otherwise the integer itself.
*/
int twos_complement_value(int value)
{
	if(value >= 0) /* if the n=received number is not negatve so the received value will be returned */
	{
		return value;
	}
    
    return ~(-value) + 1; /* not bitwise on the positive number and after that adding 1 */

}


/*
*  The function `defined_label` searches for a label with the specified name in the label list.
*  If the label is found, it returns its address. If the label does not exist, it returns ERROR (-1).
* 
*  @param head: Pointer to the head of the label list.
*  @param label: The name of the label to search for.
* 
*  @return The address of the label if it is defined; otherwise, returns ERROR (-1).
*/

struct label_node* defined_label(struct label_node **head, char *label)
{ 
    /* variable declaration */
    struct label_node *current;
    
    /* variable initialization */
    current = *head; /* current label node to check them */

    while (current != NULL) 
    {
        if ((strcmp(current->label, label) == 0)) /* label name found */
        {
            return current; /* returns the address */
        }
        current = current->next;
    }

    return NULL; /* not found */
}
