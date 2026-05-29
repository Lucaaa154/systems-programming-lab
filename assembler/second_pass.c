/*
*  This program is the second pass on the input file after the first pass.
*  The program executed by the first pass after memory allocation and adress assignments and gets their values for
*  binary machine code creation.
*/

#include "assembler.h"

/*
*  The `second_pass` function performs the second pass of the assembler process 
*  on the input file. It processes each line to validate and convert the rest of the labels 
*  into binary code, updates label addresses, and handles entries in the label list.
* 
*  @param file_ptr: Pointer to the file containing the expanded assembly code
*  @param filename: Name of the file for output purposes
*  @param buffer_guide: Array for storing guidance binary code
*  @param buffer_inst: Array for storing instruction binary code
*  @param head: Pointer to the head of the labels list
*  @param IC: Instruction counter
*  @param DC: Data counter
* 
*  @return  Returns 0 if the second pass is successful; otherwise, returns -1.
*/
int second_pass (FILE *file_ptr, char *filename, unsigned int buffer_guide[MEM_IMG_SIZE] , unsigned int buffer_inst[MEM_IMG_SIZE], struct label_node **head, int IC, int DC)
{
	/* variable declarations */
    char line[MAX_LINE_LEN]; /* to store the whole line */
    char *word; /* to tokenize the line */
    int error_flag; /* error flag */
    int temp;
    int mask;
    struct label_node *label_ptr; /* to point at a label */
    int lineNum; /* the number of the line */
    int dec_num; /* address decimal number */
    int inst_code; /* instruction code */
    int mem_final_size; /* constant value based on other variables */
    
    /* variable initialization */
    error_flag = 0; /* error indicator */
    temp = 0; /* different temporary uses */
    mask = 2040;
    lineNum = 1; /* lines counter */
    dec_num = MEM_FIRST_INDEX; /* memory image counter */
    inst_code = 0; /* initialize to default value (if needed) */
    mem_final_size = IC + DC + MEM_FIRST_INDEX; /* constant value based on other variables */


    memset(line, '\0', sizeof(line)); /* clear the string line */	
	while((dec_num <= mem_final_size) && fgets(line, sizeof(line), file_ptr)) /* loop1 */
	{
		word = strtok(line," \r\n\t");
		if (word == NULL || (*word) == ';') /* if new line or one of these chars " \r\n\t" or a comment sentence */
		{
			lineNum++; /* going to next line */
			continue; /* to next iteration */
		}
    
		if(line_type(word,lineNum) == 'l') /* if this is a label we will tokenize the next word */
		{
		    word = strtok(NULL," \r\n\t,"); 
		}
		
		if((temp = instruction_validity(word)) != ERROR) /* instruction case */
    	{
        	inst_code = buffer_inst[dec_num]; /* the instruction 15 bits */
        	dec_num++; /* the additional word */

    		if(temp < 14) /* instructions with operands(1 or 2) */
    		{ 
    		    word = strtok(NULL," \r\n\t,"); /* tokenize the first operand */
        		if(label_validity(word, 0, lineNum) != ERROR) /* first operand is a label */
        		{
        		    label_ptr = defined_label(head, word);
        		    if(label_ptr == NULL) /* getting its address */
        			{
            			printf("Error in line %d, the label in first operand has not defined\n", lineNum);
            			error_flag = ERROR;
        			}
        		    
        		    if(label_ptr->label_type == 'x') /* adding the apearance address of the extern label to the labels list */
        		    {
        		        add_label(head, word, dec_num, lineNum, 'x');
        		        buffer_inst[dec_num] += 1;
        		    }
        			else
        			{
                        /* inserting the address with the ARE to memory image */
            			buffer_inst[dec_num] = label_ptr->dec_num; 
                        buffer_inst[dec_num] <<= TARGET_MOVE; /* 3 bits reserved for A.R.E */
                        buffer_inst[dec_num] += BIT_TWO; /* R bit is on */
        			}
    			}
    		    dec_num ++; /* if its not a label so its handled before, so we will continue to check the second operand */
                
        		switch(temp) /* instructions with two operands */ /* switch1 */
        		{ 
        			case(0): case(1): case(2): case(3): case(4):
            			word = strtok(NULL," \r\n\t,"); /* tokenize the second operand */
            
            			if(label_validity(word, 0, lineNum) != ERROR) /* second operand is a label */
            			{
               				label_ptr = defined_label(head, word); /* check if has defined in the first pass */
                		    if(label_ptr == NULL) /* getting its address */
                			{
                    			printf("Error in line %d, the label in second operand has not defined\n", lineNum);
                    			error_flag = ERROR;
                			}
                		    
                		    if(label_ptr->label_type == 'x') /* extern label */
                		    {
                		        add_label(head, word, dec_num, lineNum, 'x');
                		        buffer_inst[dec_num] += 1; /* E bit is on */
                		    }
                			else
                			{
                                /* inserting the address with the ARE to memory image */
                    			buffer_inst[dec_num] = label_ptr->dec_num; 
                                buffer_inst[dec_num] <<= TARGET_MOVE; /* moving to target bits */
                                buffer_inst[dec_num] += BIT_TWO; /* R bit is on */
                			}
                		}
                        
            			/* checking if two operands are registers or adresses in registers to know if they share the same memory address to know how many memory words to move forward*/
            			temp = mask & inst_code;
            			/* these numbers are represented with 1 in the bits that indicates a register or address in register and the other bits are 0 */
            			if(temp != 1088 && temp != 1056 && temp != 576 && temp != 544) /* not a one of the combination of register/address in register only */
            			{
                			dec_num++;
            			}
    	            break;
    	       } /* end of switch */
    	    } 
    	}

    	else if(strcmp(word,".entry") == 0) /* entry */
    	{
        	word = strtok(NULL," \r\n\t,");
        
        	if(add_label(head, word, (IC+DC+MEM_FIRST_INDEX), lineNum, 'n') == ERROR) /* updating to entry if it exist in the labels list */
        	{
            	error_flag = ERROR;
        	}
    	}

    	/* ignoring and skipping the guidance because they have handled in first pass */
	    lineNum ++;
	    memset(line, '\0', sizeof(line)); /* clear the string line */
    } /* end of loop */

	/* if there is no error the outputs functions will be called */
	if(error_flag != ERROR)
	{
	    convert_ending(filename,3); /* add the ".ob" ending */
    	octal_print(filename, buffer_guide, buffer_inst, IC, DC); /* print the .ob file */
    	label_print(head, filename); /* print the .ent and .ext files */
	}
	return error_flag; /* returning error indicator */
} /* end of second_pass function */

