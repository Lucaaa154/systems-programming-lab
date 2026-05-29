/*
  The functions in this file are responsible for validating the syntax and semantics
  of instructions, constructing machine code representations, and ensuring correct
  handling of labels, operands, and directives during the assembly process.
*/
#include "assembler.h"


/*
*  Function instruction_word validates and processes an instruction in assembly language.
*  
*  The function takes the instruction name and the full line from the assembly code, extracts
*  the operands, and processes them to generate binary machine code. It uses helper functions 
*  for operand handling and ensures that the instruction and its operands are valid.
*  
*  the function returns 1 if the instruction word and its operands are valid.
*  otherwise, it returns -1 to indicate an error.
*  
*  @param line: The full line containing the instruction and operands
*  @param head: Pointer to the label list (symbol table)
*  @param buffer_inst: The instruction memory buffer (for storing machine code)
*  @param IC: Pointer to the instruction counter (its a pointer so we can send it to other functions and they can update its value)
*  @param DC: Pointer to the data counter (its a pointer so we can send it to other functions and they can update its value)
*  @param lineNum: The line number in the input for error reporting
*  
*  @return: Returns 1 for a valid instruction word and operands, otherwise returns -1
 */
int instruction_word(char *line,struct label_node **head, unsigned int buffer_inst[MEM_IMG_SIZE], int *IC, int *DC, int lineNum)
{
    /* variable declarations */
    int errInd;
    char *word;
    char word2[MAX_LINE_LEN]; /* word2 to store the operands */
    struct instruction inst; /* instruction object to store ARE, and opcode and the operands (if exist) */
    
    /* variable initialization */
    errInd = 0; /* error indicator */
    word = NULL; /* word to tokenize */

    
    word = strtok(line,"\r\n \t"); /* tokenize the first word */
	if(*(word + (strlen(word) - 1)) == ':') /* label before the guidance */
	{
	    word = strtok(NULL," \r\n\t");
	}
    
    /* initialization of the instruction fields */
    inst.ARE = 0;
    inst.op = instruction_validity(word); /* getting opcode by calling instruction_validity function */
    inst.source = 0;
    inst.target = 0;
    switch(inst.op) /* cases with operands(one or two) */ /* switch1 */
    {   
    	case(0): case(1): case(2): case(3): case(4): case(5): case(6): case(7):
    	case(8): case(9): case(10): case(11): case(12): case(13):

    	word = strtok(NULL,"\n\r"); /* tokenize operands */

    	if(word != NULL)
        {
        	memmove(word2,word,strlen(word)); /* saving all the operands in a string */
            word2[strlen(word)] = '\0';
        }
        
    	inst.ARE = 4; /* the ARE of the first word in instructions always give the A bit 1 and 0 for the other bits: 4 = 100 */
    	errInd = get_operands(head, buffer_inst, IC, DC, lineNum, &inst, word); /* to read the rest of operands */
    
    	if (errInd != ERROR)
    	{
        	word = strtok(word2,", \r\n\t"); /* first operand only in word*/
        	word = strtok(NULL," \t\n\r,"); /* second operand only in word */
    
        	switch(inst.op) /* instructions with 2 operands */ /* switch2 */
        	{   
            	case(0): case(1): case(2): case(3): case(4):
                	word = strtok(NULL,"\n\r \t"); /* tokenize after second operand to check if there any extra text*/
            	break;
        	} /* end of switch2 */
    
        	if(word != NULL)
        	{
        		printf("Error in line %d, extra text after command\n",lineNum);
        		errInd = ERROR;
        	}   
    	}

	    break;

        case(14): case(15): 
        	/* one time less tokenizing in case of instruction with one operand */
            word = strtok(NULL,"\r\n");
            
        	if(word != NULL)
        	{
            	printf("Error in line %d, extra text after command\n",lineNum);
            	errInd = ERROR;
        	}

        	else /* instruction with one operand */
        	{   
        	    /* updating the memory image by adding the ARE with 0 bits move, and op with OP_MOVE */
        	    inst.ARE = BIT_THREE; /* the ARE of the first word in instructions always give the A bit 1 and 0 for the other bits: 4 = 100 */
           		buffer_inst[*IC+MEM_FIRST_INDEX] += inst.ARE+(inst.op<<OP_MOVE);
           		(*IC) ++;
       		}
	    break;
    } /* end of switch1 */

    return errInd; /* returning error indicator */
} /* end of instruction_word function */


/*
*  Function get_operands receives the first operand and checks if its valid and if registered.
*  Eventually returns non negative value for valid operand, else returns -1.
*
*  @param head: the label list
*  @param buffer_inst: the instruction memory image
*  @param IC: instruction counter 
*  @param DC: data counter
*  @param lineNum: the number of the current line 
*  @param inst: the instruction list
*  @param word: the recieved operands
*
*  @return  The non negative value for valid operand, else returns -1
*/
int get_operands(struct label_node **head,  unsigned int buffer_inst[MEM_IMG_SIZE], int *IC, int *DC, int lineNum, struct instruction *inst, char *word)
{
	/* variable declarations */
    char *operand1;
    char *operand2;
    int i;
    int comma;
    
    /* variable initialization */
    operand1 = NULL; /* for first operand */
    operand2 = NULL; /* for second operand */
    i = 0;
    comma = 0; /* commas counter between the operands */

    
	switch((*inst).op) /* instructions with one operand will two operands */
	{   
		case(0): case(1): case(2): case(3): case(4):

    		operand1 = strtok(word,"\n\r"); /* tokenizing first operand */

            while (*operand1 && isspace(*operand1)) /* ignoring spaces before operands */
            {
                operand1++;
            }
    		if(operand1 == NULL) /* no operands */
    		{
        		printf("Error in line %d: Missing operands\n",lineNum);
        		return ERROR;
    		}
            
            if((*operand1) == ',') /* operands strarts with comma */
            {
                printf("Error in line %d: comma before first operand\n",lineNum);
                return ERROR;
            }
    		while(*(operand1+i) != '\0') /* passing over the paramters to count commas */ /* loop1 */
    		{
    			if(*(operand1+(i++)) == ',') /* first comma has found */
        		{
        		    while (*(operand1+i) && isspace(*(operand1+i))) /* ignoring spaces */
        		    {
        		        i++;
        		    }
        		    
        			if(*(operand1+(i++)) == ',') /* second comma in a row */
        			{
            			printf("Error in line %d: more than one comma after first operand\n",lineNum);
            			return ERROR;
        			}

        		comma ++; /* count the commas between operands */
        	    break; /* only one comma after first operand */
		        }
	        } /* end of loop1 */

    		if(comma == 0) /* no commas */
    		{
        		printf("Error in line %d: invalid instruction syntax: (1)paramter (2)comma (3)paramter \n",lineNum);
        		return ERROR;
    		}
    		operand1 = strtok(operand1," \t\n\r,"); /* tokenizing 1st operand */
		break;
		
		default: /* the rest of cases with one operands */
    		operand1 = strtok(word," \t\n\r"); /* tokenizing 1st operand in the case of instruction that don't have 2 operands (without commas) */
		break;
    } /* end of switch1 */

	/* first operand is valid address in register */
	if((*(operand1) == '*') && (register_validity(operand1 + 1) != ERROR))
	{
	    operand1++;
	    address_method(inst, (*inst).op, BIT_THREE); /* addressing op field by 0100 */
	}
	else if(register_validity(operand1) != ERROR) /* first operand is valid register */
	{
	    address_method(inst, (*inst).op, BIT_FOUR); /* addressing op field by 1000 */
    }

    else if(*operand1 == '#') /* first operand is valid number */
	{
	    operand1++;
	    if(num_validity(operand1,lineNum, 'i') != ERROR)
	    {
	        address_method(inst, (*inst).op, BIT_ONE); /* addressing op field by 0001 */
	    }
	}

	else if(label_validity(operand1, 0, lineNum) == ERROR)  /* first operand is valid label */
	{
	    printf("Error in line %d: invalid first operand\n",lineNum);
		return ERROR;
	}
	else
	{
        address_method(inst, (*inst).op, BIT_TWO); /* addressing op field by 0010 */
	}
	
	switch((*inst).op) /* continue to read the 2nd operand of instructions with two operands */
    {   
    	case(0): case(1): case(2): case(3): case(4):
    	/* invoking the function to get the second operand */
        	if((get_sec_operand(&operand2, buffer_inst, IC, DC, lineNum, inst)) == ERROR)
        	{
        		return ERROR;
        	}

		break;
    } /* end of switch5 */

	/* creating the memory image invoking the function binary_encode */
	return binary_encode(buffer_inst, head, inst, operand1, operand2, IC, DC, lineNum); /* checking if the operands are appropriate to this instruction */

    
} /* end of get_operand function */


/*
*  Function get_sec_operand receives the second operand and checks its validity..
*
*  @param operand2: the received pointer to store the second operand 
*  @param buffer_inst: the instruction memory image
*  @param IC: the instruction counter 
*  @param DC: the data counter
*  @param lineNum: the number of the current line 
*  @param inst: the instruction struct
*
*  @return  The non negative value for valid operand, else returns -1
*/
int get_sec_operand(char **operand2 , unsigned int buffer_inst[MEM_IMG_SIZE], int *IC, int *DC, int lineNum, struct instruction *inst)
{
    /* variable declarations */
	char *word;
    
	/* reading next operand */
	word = strtok(NULL," \r\t\n");

	if(word != NULL && strcmp(word,",") == 0) /* ignoring comma between operands if its hasn't been ignored */
	{
        word = strtok(NULL," \r\t\n"); /* tokenizing the second operand */
	}

   	if(word == NULL) /* no operand2 */
	{
    	printf("Error in line %d: Missing second operand\n",lineNum);
	    return ERROR;                
	}

	if(*word == ',') /* removing the comma for operand test */
	{
        *word = '\0'; /* null terminating to remove the comma*/
    	word ++;
	}

	*operand2 = strtok(word,"\r\n"); /* tokenizing the whole word */

	if(*operand2 == NULL)
	{
    	printf("Error in line %d: Missing second operand\n",lineNum);
    	return ERROR;
	}

    /* first operand is valid address in register */
	if((**(operand2) == '*') && (register_validity(*operand2 + 1) != ERROR))
	{
	    (*operand2)++;
	    (*inst).target = 4;
	}
	/* second operand is a valid register */
	else if(register_validity(*operand2) != ERROR)
	{
    	(*inst).target = 8; /* to encode the appropriate address */
	}

	else if(**operand2 == '#') /* second operand is a valid number */
    {
        (*operand2)++;
        if(num_validity(*operand2,lineNum, 'i') != ERROR)
        {
            (*inst).target = 1;
        }
    }

	else if(label_validity(*operand2, 0, lineNum) == ERROR) /* second operand is unvalid label*/
    {
        printf("Error in line %d: invalid second operand\n",lineNum); /* no appropriate type */
        return ERROR;
    }
    else
    {
        (*inst).target = 2;
    }
    
	return 0; /* no errors */
} /* end of get_sec_operand function */


/*
*  Function binary_encode receives two operands and check if they are valid for instruction word.
*  if they are valid so the function will create the memory image for these operands.
* 
*  @param buffer_inst: the instruction memory image
*  @param head: the label list
*  @param inst: the instruction list
*  @param operand1: the received first operand 
*  @param operand2: the received second operand 
*  @param IC: the instruction counter 
*  @param DC: the data counter
*  @param lineNum: the number line of this line 
*
*  @return  The non negative value for valid operands for instruction word, else returns -1
*/
int binary_encode(unsigned int buffer_inst[MEM_IMG_SIZE], struct label_node **head, struct instruction *inst, char *operand1, char *operand2, int *IC, int *DC, int lineNum)
{
	/* variable declaration */
    int errInd;
    
    /* variable initialization */
    errInd = 0; /* error indicator */


	switch((*inst).op) /* checking if the operands types are appropriate to the current instruction */ /* switch1 */
	{   
		case(0): /* mov */
    		if((*inst).target == 1) /* mov instruction can't have a number in second operand */
    		{
        		printf("Error in line %d: in instruction mov the target operand can not be a number\n",lineNum);
        		errInd = ERROR;
    		}
		break;
        
        case(2): /* add */
        {
            if((*inst).target == 1) /* add instruction can't have a number in second operand */
    		{
        		printf("Error in line %d: in instruction add the target operand can not be a number\n",lineNum);
        		errInd = ERROR;
    		}
        }
        break;
        
        case(3): /* sub */
        {
            if((*inst).target == 1) /* sub instruction can't have a number in second operand */
    		{
        		printf("Error in line %d: in instruction sub the target operand can not be a number\n",lineNum);
        		errInd = ERROR;
    		}
        }
        break;
        
    	case(4): /* lea */
    		if((*inst).source != 2) /* lea instruction's first operand have to be only a label */
    		{
        		printf("Error in line %d: in instruction lea the source operand have to be an address\n",lineNum);
        		errInd = ERROR;
    		}

    		if((*inst).target == 1) /* lea instruction can't have a number in second operand */
    		{
        		printf("Error in line %d: in instruction lea the target operand can not be a number\n",lineNum);
        		errInd = ERROR;
    		}
		break;

    	case(5): /* clr */
    		if((*inst).target == 1) /* clr instruction can't have a number in second operand */
    		{
        		printf("Error in line %d: in instruction clr the operand can not be a number\n",lineNum);
    		    errInd = ERROR;
    		}
		break;
		
		case(6): /* not */
    		if((*inst).target == 1) /* not instruction can't have a number in second operand */
    		{
        		printf("Error in line %d: in instruction not the operand can not be a number\n",lineNum);
    		    errInd = ERROR;
    		}
		break;
		
   		case(7): /* inc */
    		if((*inst).target == 1) /* inc instruction can't have a number in second operand */
    		{
                printf("Error in line %d: in instruction inc the operand can not be a number\n",lineNum);
    		    errInd = ERROR;    	
    		}
		break;

    	case(8): /* dec */
    		if((*inst).target == 1) /* dec instruction can't have a number in second operand */
    		{
                printf("Error in line %d: in instruction dec the operand can not be a number\n",lineNum);
    		    errInd = ERROR;
    		}
		break;
		
		case(9): /* jmp */
    		if((*inst).target == 1 || (*inst).target == 8) /* jmp instruction can't have a number in second operand */
    		{
                printf("Error in line %d: in instruction jmp the operand can only be a label or an address in register\n",lineNum);
    		    errInd = ERROR;
    		}
		break;
		
		case(10): /* bne */
    		if((*inst).target == 1 || (*inst).target == 8) /* bne instruction can't have a number in second operand */
    		{
                printf("Error in line %d: in instruction bne the operand can only be a label or an address in register\n",lineNum);
    		    errInd = ERROR;
    		}
		break;
		
    	case(11): /* red */
    		if((*inst).target == 1) /* red instruction can't have a number in second operand */
    		{
                printf("Error in line %d: in instruction red the operand can not be a number\n",lineNum);
        		errInd = ERROR;
    		}
		break;
		
		case(13): /* jsr */
    		if((*inst).target == 1 || (*inst).target == 8) /* jsr instruction can't have a number in second operand */
    		{
                printf("Error in line %d: in instruction jsr the operand can only be a label or an address in register\n",lineNum);
    		    errInd = ERROR;
    		}
		break;

	} /* end of switch1 */

    if(errInd == ERROR) /* stopping encoding if there is any error */
	{
        return errInd;
    }

    if((*inst).op <= 4) /* instructions with two operands */
    {   /* inserting the binary code with using bitwise and moving to proper position according to instruction memory distribution */
    	buffer_inst[*IC+MEM_FIRST_INDEX] += (*inst).ARE+((*inst).source<<SOURCE_MOVE)+((*inst).target<<TARGET_MOVE)+((*inst).op<<OP_MOVE);
        (*IC) ++;
        
    	switch((*inst).source) /* switch2 */
    	{   /* first operand encoding */
        	case(1): /* the operand is a number */
        		buffer_inst[*IC+MEM_FIRST_INDEX] = twos_complement_value(atoi(operand1)); /* using atoi to convert the string to integer */
        		buffer_inst[*IC+MEM_FIRST_INDEX] <<= TARGET_MOVE; /* move 3 bits left (bits 0-2 are served for A.R.E)*/
        		buffer_inst[*IC+MEM_FIRST_INDEX] += BIT_THREE; /* A bit is on */
        		(*IC)++;
    		break;

        	case(2): /* the operand is a label */
        		(*IC)++; /* the A.R.E and the label address will be inserted in second pass */
    		break;
            
            case(4):    
        	case(8): /* the operand is a register or address in register */
        		buffer_inst[*IC+MEM_FIRST_INDEX] = (operand1[1] - '0'); /* inserting register number */
        		buffer_inst[*IC+MEM_FIRST_INDEX] <<= SOURCE_REGISTER_MOVE; /* moving it to bits 6-8 */
                buffer_inst[*IC+MEM_FIRST_INDEX] += BIT_THREE; /* A bit is on */

        		if((*inst).target == BIT_THREE || (*inst).target == BIT_FOUR)
        		{   /* if the second operand also a register so the register number will be inserted in the same memory image index */
           			 buffer_inst[*IC+MEM_FIRST_INDEX] += ((operand2[1] - '0')<<3); /* inserting to bits 3-5 */
        		}
        		(*IC)++;
        	break;
        		
    	} /* end of switch2 */

    	/* second operand encoding */
    	switch((*inst).target) /* switch3 */
    	{
        	case(1): /* the operand is a number */
        		buffer_inst[*IC+MEM_FIRST_INDEX] = twos_complement_value(atoi(operand2));
        		buffer_inst[*IC+MEM_FIRST_INDEX] <<= TARGET_MOVE; /* to move to 3 bits left */
        		buffer_inst[*IC+MEM_FIRST_INDEX] += BIT_THREE; /* A bit is on */
        		(*IC) ++;
        	break;

        	case(2): /* the operand is a label */
        		(*IC)++;
    		break;

            case(4):
        	case(8): /* the operand is a register */
        		if((*inst).source != 4 && (*inst).source != 8) /* we finished the case when source and target are registers/address in registers */
        		{
           	 		buffer_inst[*IC+MEM_FIRST_INDEX] = (operand2[1] - '0');
            		buffer_inst[*IC+MEM_FIRST_INDEX] <<= TARGET_MOVE; /* inserting to bits 2-6 */
            		buffer_inst[*IC+MEM_FIRST_INDEX] += BIT_THREE; /* A bit is on */
            		(*IC) ++;
        		}
    		break;
    	} /* end of switch3 */
            
    }

    else /* instructions with one operand (instructions with no operands will not reach this function) */
    {   /* encoding the collected opcode and operands types and ARE to the memory image */
    	buffer_inst[*IC+MEM_FIRST_INDEX] += (*inst).ARE+((*inst).target<<TARGET_MOVE)+((*inst).op<<OP_MOVE)+((*inst).source<<SOURCE_MOVE);
    	(*IC) ++;
    	switch((*inst).target) /* switch4 */
    	{
        	case(1): /* the operand is a number */
        		buffer_inst[*IC+MEM_FIRST_INDEX] = twos_complement_value(atoi(operand1));
        		buffer_inst[*IC+MEM_FIRST_INDEX] <<= TARGET_MOVE; /* move 3 bits left */
        		buffer_inst[*IC+MEM_FIRST_INDEX] += BIT_THREE; /* A bit is on */
        		(*IC)++;
    		break;

        	case(2): /* the operand is a label */
        		(*IC)++;
    		break;

            case(4):
        	case(8): /* the operand is a register */
        		buffer_inst[*IC+MEM_FIRST_INDEX] = (operand1[1] - '0');
        		buffer_inst[*IC+MEM_FIRST_INDEX] <<= TARGET_MOVE;
        		buffer_inst[*IC+MEM_FIRST_INDEX] += BIT_THREE; /* A bit is on */
        		(*IC)++;
    		break;
    	} /* end of switch4 */
    }
    	return errInd;
} /* end of binary_encode function */


/*
 *  Function register_validity receives a string and checks if the string represents a valid register name.
 *  Valid registers follow the pattern 'r0' to 'r7' (3 characters, starting with 'r').
 *  The function returns the corresponding register number if valid, otherwise returns -1 to indicate an error.
 * 
 *  @param word: the received string that might represent a register
 *
 *  @return The value of the register as an integer (0-7) if the string is valid, otherwise returns -1 (ERROR).
 */
int register_validity(char *word)
{
    /* register name is about @r with a number 0-7 (3 characters)*/
    if(strlen(word) == 2)
    {
        if(word[0] == 'r' && word[1]>= '0' && word[1] <= '7')
	    {
            return atoi(&word[1]); /* returns the register number by using atoi function to return it as an integer */
	    }
    }    

    return ERROR; /* not a  name of register */
} /* end of register_validity function */


/* 
*  This function inserts the appropriate addressing method into the corresponding field of the instruction structure.
*  If the instruction uses two operands (source and target), this function inserts the addressing method into the source bits.
*  If the instruction uses one operand (target), the insertion will be to the target bits and the source bits are cleared.
*
*  @param opcode: The opcode of the instruction.
*  @param inst: Pointer to the struct that represents the instruction.
*  @param address: The address to be inserted (source or target).
*/
void address_method(struct instruction *inst, int opcode, int address)
{
switch(opcode) /* encoding per opcode */ /* switch2 */
	{   
		case(0): case(1): case(2): case(3): case(4): /* 2 operands */
			(*inst).source = address;
		break;

		case(5): case(6): case(7): case(8): case(9): case(10): case(11): case(12): case(13):
			(*inst).target = address;
    		(*inst).source = 0;
		break;
	} /* end of switch2 */
}
