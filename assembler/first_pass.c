/*
*  This file contains the implementation of the first pass (pass1) on the expanded input file.
*  During the first pass, the program validates the input file, including checking the validity
*  of labels, instructions, and guidance. 
*  The second pass (pass2) is invoked from pass1 and from there to generate the final binary machine code.
* 
*  @param file_ptr: Pointer to the expanded file after preprocessing by the preassembler
*  @param filename: The name of the input file
*/


#include "assembler.h"

int first_pass (FILE *file_ptr, char *filename, struct macro_att *macros_table) 
{
	/* variable declarations */
    unsigned int buffer_inst[MEM_IMG_SIZE]; /* the memory image of instructions  */
    unsigned int buffer_guide[MEM_IMG_SIZE]; /* the memory image of data  */
    int line_num; /* line number */
    int IC;
    int DC;
    int final_error;
    int result; /* returning value of functions */
    char *word; /* word to tokenize the lines */
    char temp[MAX_LINE_LEN]; /* for temporary use */
    char line[MAX_LINE_LEN]; /* to read the line */
    char bkup_line[MAX_LINE_LEN]; /* the same line to backup (strtok will modify line) */
    struct label_node *head; /* head of the labels list */
    struct label_node *current; /* a pointer to a node in the labels list */
    
    /* variable initialization */
    line_num = 1; /* line number counter */
    IC = 0; /* instructions counter */
    DC = 0; /* data counter */
    final_error = 0; /* error indicator */
    result = 0; /* function return values results */
    head = NULL; /* label list pointer */
    current = NULL; /* label list current pointer */

	
    memset(buffer_inst, 0, sizeof(buffer_inst));
	memset(buffer_guide, 0, sizeof(buffer_guide));
    memset(line, '\0', sizeof(line)); /* clear the string line */
	memset(bkup_line, '\0', sizeof(bkup_line)); /* clear the string line */
	
	/* while the file has'nt reach the end and the memory image not full  */
	while (((IC+DC+MEM_FIRST_INDEX) < MEM_IMG_SIZE) && (fgets(line, sizeof(line), file_ptr))) /* loop1 */
	{
		 /* get the line and copy it to backup string */
		memmove(bkup_line,line,strlen(line)); /* backup line in bkup_line */
		bkup_line[strlen(line)] = '\0';
		/* if line length more than 80 characters */
		if ((strlen(bkup_line) == (MAX_LINE_LEN - 1)) && (strchr(bkup_line,'\n') == NULL || (strchr(bkup_line,'\r') == NULL)))
		{
			printf("Error: invalid line: line %d is too long\n", line_num);
			final_error = ERROR; /* error indicator now will be ERROR (-1) */
		}
        
        word = strtok(line," \r\n\t");
		if (word == NULL || (*word) == ';') /* if new line or one of these chars " \r\n\t" or a comment sentence */
		{
			line_num++; /* going to next line */
			continue; /* to next iteration */
		}

		else /* checking line type  */
		{
			switch(line_type(word, line_num)) /* check the type of the first word */
			{   /* handle the case with label/without label */
				case('l'):  /* valid label syntax */
    				word[strlen(word)-1] = '\0';
    				memset(temp, '\0', sizeof(temp)); /* clear the string line */
    				memmove(temp,word,strlen(word)); /* storing the label to add if the next word is not .entry or .extern */
                    temp[strlen(word)] = '\0';
                    word = strtok(NULL," \r\n\t"); /* tokenize the next word */
                    if(word == NULL) /* label with empty commands */
    	            {  
    	                printf("Error: in line %d, no defined command after label\n", line_num);
    	                final_error = ERROR; /* updating the final error flag */
    	                line_num++; /* adding 1 to the line counter */
    	                continue;
    	            }
    	            if(defined_macro(word, macros_table) != NULL) /* the label is not a name of macro */
    	            {
    	                printf("Error: in line %d, label can't have the same name of a macro\n", line_num);
    	                final_error = ERROR;
    	                line_num++;
    	                continue;
    	            }
    	            /* temp now contains the label name */
                    if(strcmp(word,".entry") == 0 && strcmp(word,".extern") == 0) /* the label is not before .extern or .entry */
		            {
		                printf("Warning in line %d: label definition after .entry or .extern will be ignored\n", line_num);
		            }
		            else
		            {
		                if(strcmp(word,".data") == 0 || strcmp(word,".string") == 0)
		                {
		                    result = add_label(&head, temp, (DC+MEM_FIRST_INDEX), line_num, 'd'); /* we will add it to label list as data label */
		                }
		                else
		                {
			                result = add_label(&head, temp, (IC+MEM_FIRST_INDEX), line_num, 'i'); /* we will add it to label list as instruction label */
		                }
			            if(result == ERROR)
			            {
			                final_error = result;
			            }
                    }
    

    				switch(line_type(word,line_num)) /* the case without label */
    				{
				    	case('l'): /* more than one label in a row */
				            printf("Error in line %d: multiple labels in a row\n", line_num);
				            final_error = ERROR;
				            break;

			            case('i'):  /* instruction */
			            /* calling instruction function */
				            result = instruction_word(bkup_line, &head, buffer_inst, &IC, &DC, line_num); 
		    	            break;

			            case('g'): /* guidance */ 
			            /* calling guidance function */
			                result = guidance(buffer_guide, buffer_inst, &IC, &DC, line_num, bkup_line, &head, macros_table);
				            break;

		    	        default:
		    	        /* else undefined command */
			                printf("Error in line %d: undefined command\n", line_num);
			                final_error = ERROR;
				    	break;
				    }
			   break; /* break of case('l') */

			   /* no label at the beginning of the line */
			   case('i'):  /* instruction */
		            result = instruction_word(bkup_line, &head, buffer_inst, &IC, &DC, line_num);
			   break;

		       case('g'):  /* guidance */
		            result = guidance(buffer_guide, buffer_inst, &IC, &DC, line_num, bkup_line, &head, macros_table);
			   break;

			   default:    /* else undefined command */
		            printf("Error in line %d: undefined command\n", line_num);
		            final_error = ERROR;
			   break;
			}
		
		}

        if(final_error != ERROR) /* if no error has detected till now, update */
		{
			final_error = result;
		}
        memset(line, '\0', sizeof(line)); /* clear the string line */
		memset(bkup_line, '\0', sizeof(bkup_line)); /* clear the string bkup_line */
		line_num++; /* add 1 to the line counter */
	} /* end of loop1 */
    
	if (!feof(file_ptr)) /* if there any characters after reaching the max memory image size */
	{
		printf("Warning: reached to the maximum memory, the assembler won't check the text from line %d\n",line_num);
	}
    
    update_label_list(head, IC); /* adding the IC so the labels on the data will be after the instructions */
	if(final_error != ERROR) /*  calling the second pass if there are no errors in the first pass */
	{
		rewind(file_ptr); /*rewind to start from the beginning of the file to read it in the second pass */
		final_error = second_pass(file_ptr, filename, buffer_guide, buffer_inst, &head, IC, DC); /* calling second_pass */
	}
	
	/* free label list */
	while (head != NULL)  /* loop2 */
	{
    	current = head->next; /* storing the next node */
    	free(head);
    	head = current;
	} /* end of loop2 */

	return final_error;	
} /* end of first_pass function */

