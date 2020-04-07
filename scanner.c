/*
Filename:		scanner.c
Compiler:		MS Visual Studio 2019
Author:			Aria Gomes & Nicholas King
Course:			CST 8152 - Compilers 013(Aria Gomes) & 011(Nicholas King)
Assignment:		2 - Scanner
Date:			February 25
Professor:		Sv. Ranev
Purpose:		Creates method definitions for a scanner.
Function List:	scanner_init(pBuffer), malar_next_token(void), get_next_state(int, char), char_class(char), aa_func02(char[]),
				aa_func03(char[]), aa_func05(char[]), aa_func08(char[]), aa_func10(char[]), aa_func12(char[])
******************************************************/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
 * to suppress the warnings about using "unsafe" functions like fopen()
 * The define does not have any effect in Borland compiler projects.
 */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

 /*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
   It is defined in platy_st.c */
extern pBuffer str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

/* Local(file) global objects - variables */
static pBuffer lex_buf;/*pointer to temporary lexeme buffer*/
static pBuffer sc_buf; /*pointer to input source buffer*/
/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */
static int char_class(char c); /* character class function */
static int get_next_state(int, char); /* state machine function */
static int iskeyword(char* kw_lexeme); /*keywords lookup functuion */

/*****************************************************
Function Name:		scanner_init()
Author:				Aria Gomes & Nicholas King
History/Version:	February 25		v.1.2
Called Functions:	b_isempty(), b_rewind(), b_clear(),
Parameters:			pBuffer
Return Value:		int (Exit success or exit failure)
Algorithm:			call functions and initialize variables to initalize the scanner
*****************************************************/
int scanner_init(pBuffer psc_buf)
{
	if (b_isempty(psc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	b_rewind(psc_buf);
	b_clear(str_LTBL);
	line = 1;
	sc_buf = psc_buf;
	return EXIT_SUCCESS;/*0*/
/*   scerrnum = 0;  *//*no need - global ANSI C */
}

/*****************************************************
Function Name:		malar_next_token()
Purpose:			Get the next token and check to see what Token it is, returns the Token
Author:				Aria Gomes & Nicholas King
History/Version:	February 25			v.1.2
Called Functions:	b_getc(), b_retract(), b_mark(), b_reset(), isdigit(), isalpha(), b_getcoffset(), b_allocate(), b_addc(), b_free()
Parameters:			void
Return Value:		Token t
Algorithm:
					Get character from buffer
			PART 1
					Handle character if it is a symbol to be used, returning a token each time.
					If the character is a space, tab, or new line continue the loop with the next character
			PART 2
					The character is a ", a letter, or a digit. Read all the characters
					until the state is no longer accepting. Send the character to one
					of the aa_funcXX functions for processing then return the Token
*****************************************************/
Token malar_next_token(void)
{
	Token t = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c; /* input symbol */
	int state = 0; /* initial state of the FSM */
	short lexstart;  /*start offset of a lexeme in the input char buffer (array) */
	short lexend;    /*end   offset of a lexeme in the input char buffer (array)*/
	int i; /* a variable to iterate for loops */

	while (1) /* endless loop broken by token returns it will generate a warning */
	{ 

		c = b_getc(sc_buf);

		/**************************************************
		PART 1: Implementation of token driven scanner
		every token is possessed by its own dedicated code
		***************************************************/

		switch (c) {
		case SEOF:				/* Source end-of-file token */
		case S_EOF:					/* Unsigned char 255 */
			t.code = SEOF_T;
			t.attribute.seof = SEOF_0;
			return t;
		case NL:	    /* New Line Character/counter */
			line++;
			continue;
		case '\t':		/* Tab Escape Sequence */
			continue;
		case '\f':
			continue;
		case ' ':		/* White Space */
			continue;
		case '(':		/* Open parenthesis */
			t.code = LPR_T;
			return t;
		case ')':	    /* Close parenthesis */
			t.code = RPR_T;
			return t;
		case '{':		/* Open Bracket */
			t.code = LBR_T;
			return t;
		case '}':		/* Close Bracket */
			t.code = RBR_T;
			return t;
		case ',':		/* Comma */
			t.code = COM_T;
			return t;
		case ';':		/* Semicolon */
			t.code = EOS_T;
			return t;
			/* Arithmetic operator token */
		case '*':		/* Asterisks - Multiplication */
			t.code = ART_OP_T;
			t.attribute.arr_op = MULT;
			return t;
		case '/':		/* Forward Slash - Division */
			t.code = ART_OP_T;
			t.attribute.arr_op = DIV;
			return t;
		case '-':		/* Hyphen - Subtraction */
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS;
			return t;
		case '+':		/* Plus Sign - Addition */
			t.code = ART_OP_T;
			t.attribute.arr_op = PLUS;
			return t;
		case '=':		/* Equals Sign */
			/*Check if another = has occured*/
			if ((c = b_getc(sc_buf)) == '=') 
			{
				t.code = REL_OP_T;
				/*Equal to zero*/
				t.attribute.rel_op = EQ;
				return t;
			}
			b_retract(sc_buf);
			t.code = ASS_OP_T;
			return t;
		case '<': /* Less Than */
			/*check if token is different*/
			if ((c = b_getc(sc_buf)) == '>') /* Relational operator token -- Not Equal */
			{	
				t.code = REL_OP_T;
				/*Not equal to 1*/
				t.attribute.rel_op = NE;
				return t;
			}
			/*if double catenate*/
			else if (c == '<') 
			{
				t.code = SCC_OP_T;
				return t;
			}
			/*return the next character*/
			b_retract(sc_buf);
			t.code = REL_OP_T;
			t.attribute.rel_op = LT;
			return t;
		case '>':
			t.code = REL_OP_T;
			t.attribute.rel_op = GT;
			return t;
		case '.':
			/*Set mark to the first character*/
			b_markc(sc_buf, b_getcoffset(sc_buf));
			c = b_getc(sc_buf);
			/*Check if first and after cahracter is  AND or OR*/
			if (c == 'A' && b_getc(sc_buf) == 'N' && b_getc(sc_buf) == 'D' && b_getc(sc_buf) == '.') 
			{
				t.code = LOG_OP_T;
				t.attribute.log_op = AND;
				return t;
			}
			else if (c == 'O' && b_getc(sc_buf) == 'R' && b_getc(sc_buf) == '.') 
			{
				t.code = LOG_OP_T;
				t.attribute.log_op = OR;
				return t;
			}
			/*If its not AND or OR set error STATE*/
			else 
			{
				b_reset(sc_buf);
				t.code = ERR_T;
				t.attribute.err_lex[0] = '.';
				t.attribute.err_lex[1] = '\0';
				return t;
			}
		case '#':
			if ((c = b_getc(sc_buf)) == '#')
			{
				c = b_getc(sc_buf);
				t.code = SCC_OP_T;
				return t;
			}
			else
			{
				c = '#';
				b_retract(sc_buf);
				t.attribute.err_lex[0] = c;
				t.code = ERR_T;
				return t;
			}
		case '!':
			/*Check if its a comment*/
			if ((c = b_getc(sc_buf)) == '!')  
			{	
				c = b_getc(sc_buf);
				/*Check if c is not a new line*/
				while (c != NL) 
				{
					c = b_getc(sc_buf);
					/*Check if c is EOF*/
					if (c == SEOF || c == S_EOF) 
					{
						t.code = ERR_T;
						t.attribute.err_lex[0] = c;
						t.attribute.err_lex[1] = SEOF;
						return t;
					}
				}
				line++;
				continue;
			}
			else /* ERROR */
			{	
				t.code = ERR_T;
				t.attribute.err_lex[0] = '!';
				t.attribute.err_lex[1] = c;
				t.attribute.err_lex[2] = SEOF;
				c = b_getc(sc_buf);
				while (c != NL && SEOF) { c = b_getc(sc_buf); }
				line++;
				return t;
			}
		}

		/************************************************************
		PART 2: Implementation of Finite State Machine (DFA)
		or Transition Table driven Scanner
		*************************************************************/

		if (isdigit(c) || isalpha(c) || c == '"') 
		{
			/* retract input so that it has the last character */
			lexstart = b_retract(sc_buf);
			b_markc(sc_buf, lexstart); /* set markc offset so that we know the beginning of this lexeme*/
			state = 0;
			c = b_getc(sc_buf);

			/* If state is NOAS continue until state is not NOAS*/
			while (as_table[state] == NOAS) 
			{
				state = get_next_state(state, c); /* get next state */
				if (as_table[state] != NOAS) /* Break loop since now AS state */
					break;
				c = b_getc(sc_buf); /* get next character */
			}

			/* if ASWR then retract buffer */
			if (as_table[state] == ASWR)
				b_retract(sc_buf);

			/* Accepting state found so set lexend using getc_offset */
			lexend = b_getcoffset(sc_buf);

			/* create Temporary Lexeme Buffer */
			lex_buf = b_allocate((lexend - lexstart) + 1, 0, 'f');
			if (lex_buf == NULL) /* buffer not created correctly*/
			{ 
				scerrnum = 1;
				aa_func12("RUN TIME ERROR");
			}

			b_reset(sc_buf); /* reset the lexeme to the mark set from before */
			/* add all the characters to the buffer */
			for (i = lexstart; i < lexend; i++)
				b_addc(lex_buf, b_getc(sc_buf));

			/* add the SEOF character */
			b_addc(lex_buf, SEOF);
			/* run the aa_funcXX(char*) function depending on the end state */
			t = aa_table[state](b_location(lex_buf));
			/* free the temporary buffer */
			b_free(lex_buf);
			return t;
		}

		t.code = ERR_T;
		t.attribute.err_lex[0] = c;
		t.attribute.err_lex[1] = SEOF;
		return t;
	}
	return t;
}

/* DO NOT MODIFY THE CODE OF THIS FUNCTION
YOU CAN REMOVE THE COMMENTS ONLY */

/*****************************************************
Function Name:		get_next_state()
Author:				Aria Gomes & Nicholas King
History/Version:	February 25	    v.1.2
Called Functions:	char_class(), assert()
Parameters:			int state, char c
Return Value:		int next
Algorithm:			sets the columns with char_calss(). Initalize the next state
					from the transition table. Run Diagnostic with assert(). Then
					return int next.
*****************************************************/
int get_next_state(int state, char c)
{
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	assert(next != IS);

#ifdef DEBUG
	if (next == IS)
	{
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	return next;
}

/*****************************************************
Function Name:		char_class()
Purpose:			Returns the column that the next state will be in from the table
Author:				Aria Gomes & Nicholas King
History/Version:	February 25			  v.1.2
Called Functions:	isalpha(), isdigit()
Parameters:			char c
Return Value:		int val
Algorithm: converts function parameter to integer.
*****************************************************/
int char_class(char c)
{
	/*Returns the column number in the transition table for the input character*/
	int val = 5;

	if (isalpha(c)) /* [a-zA-Z] */
		val = 0;
	else if (c == '0') /* 0 */
		val = 1;
	else if (isdigit(c)) /* [0-9] */
		val = 2;
	else if (c == '.') /* . */
		val = 3;
	else if (c == '#') /* # */
		val = 4;
	else if (c == '"') /* " */
		val = 6;
	else if (c == SEOF || c == S_EOF) /* SEOF (Source End Of File)*/
		val = 7;

	return val;
}

/************************************************************
HERE YOU WRITE THE DEFINITIONS FOR YOUR ACCEPTING FUNCTIONS.
************************************************************/

/*****************************************************
Function Name:		aa_func02()
Purpose:			Accepts either a keyword or another word, checks the keyword database and then returns a Token
Author:				Aria Gomes & Nicholas King
History/Version:	February 25			 v.1.2
Called Functions:	strcmp(), strlen()
Parameters:			char lexeme[]
Return Value:		Token t
Algorithm:			Check to make sure lexeme is not a keyword, if it is returns the keyword Token.
					Returns the variable token
*****************************************************/

Token aa_func02(char lexeme[]) 
{
	Token t;
	unsigned int i;

	/*Check if the lexeme is a Keyword*/
	for (i = 0; i < KWT_SIZE; i++) {
		if ((strcmp(lexeme, kw_table[i])) == 0) 
		{
			t.code = KW_T; 
			t.attribute.kwt_idx = i; 
			return t; 
		}
	}

	/*If lexeme is not a keyowrd Set a AVID Token*/
	t.code = AVID_T;

	/*If the lexeme is longer than VID_LEN*/
	if (strlen(lexeme) > VID_LEN) 
	{
		for (i = 0; i < VID_LEN; i++) 
			t.attribute.vid_lex[i] = lexeme[i]; 
		
		/*Add \0 at the end to male a C-type STRING*/
		t.attribute.vid_lex[i] = '\0';
	}
	else 
	{ 
		/*Else if lexeme is shorter*/
		for (i = 0; i < strlen(lexeme); i++) 
			t.attribute.vid_lex[i] = lexeme[i]; 
		t.attribute.vid_lex[i] = '\0'; 
	}
	return t;
}


/*****************************************************
Function Name:		aa_func03
Purpose:			Returns the token for a variable name
Author:				Aria Gomes & Nicholas King
History/Version:	February 25			 v.1.2
Called Functions:	strlen()
Parameters:			char lexeme[]
Return Value:		Token t
Algorithm:			Verify the lexeme is valid and return the token
*****************************************************/

Token aa_func03(char lexeme[])
{
	Token t;
	unsigned int i;

	/*Set a SVID TOKEN*/
	t.code = SVID_T;
	/*If the lexeme is longer than VID_LEN characters*/
	if (strlen(lexeme) > VID_LEN)
	{ 
		for (i = 0; i < VID_LEN; i++) 
			t.attribute.vid_lex[i] = lexeme[i]; 
		/*Only first VID_LEN -1 char are stroing into the var attribute array*/
		t.attribute.vid_lex[VID_LEN - 1] = '#';
		t.attribute.vid_lex[VID_LEN] = '\0';
	}
	else /*Else if lexeme is shorter*/
	{ 
		for (i = 0; i < strlen(lexeme); i++) 
			t.attribute.vid_lex[i] = lexeme[i];
		t.attribute.vid_lex[i] = '\0';
	}
	return t;
}

/*****************************************************
Function Name:		aa_func05
Purpose:			Returns Integer Token
Author:				Aria Gomes & Nicholas King
History/Version:	February 25			v.1.2
Called Functions:	atoi(), aa_func()
Parameters:			char lexeme[]
Return Value:		Token t
Algorithm:			Set token attributes and returns the token
*****************************************************/
Token aa_func05(char lexeme[])
{
	Token t;
	int value;
	/*Convert the lexeme decimal costant to a decimal interger value*/
	value = atoi(lexeme);

	/*Make sure the value must be in the same range as the value of 2-byte integer*/
	if (value > SHRT_MAX || value < SHRT_MIN)
		return aa_func12(lexeme);

	/* set the apropriate token code and value */
	t.code = INL_T;
	t.attribute.int_value = value;

	return t;
}

/*****************************************************
Function Name:		aa_func08
Purpose:			Returns a floating point token
Author:				Aria Gomes & Nicholas King
History/Version:	February 25			   v.1.2
Called Functions:	atoff(), strlen(), aa_func12()
Parameters:			char lexeme[]
Return Value:		Token t
Algorithm:			Turns lexeme into a floating point number. and checks if that
					values length is greater than 32 and is between the floating point
					limits.
*****************************************************/
Token aa_func08(char lexeme[])
{
	Token t;
	double value;
	/*Convert the lexeme to a floating point value*/
	value = atof(lexeme);

	/* Check to see if the value os in the same range as the value of 4-byte float */
	if ((value >= 0 && strlen(lexeme) > 32) && (value < FLT_MIN || value > FLT_MAX) || value < 0)
		return aa_func12(lexeme);

	/* set the apropriate token code and assign the value */
	t.code = FPL_T;
	t.attribute.flt_value = (float)value;

	return t;
}

/*****************************************************
Function Name:		aa_func11
Purpose:			Returns a String Token
Author:				Aria Gomes & Nicholas King
History/Version:	February 25			   v.1.2
Called Functions:	b_limit(), strlen(), b_addc()
Parameters:			char lexeme[]
Return Value:		Token t
Algorithm:			Setting the attribute of the string offset to the location where
					the first char of lexeme content will be added to buffer.During the
					copying process the quotations first and second are ignored stores
					the char lexeme into the string literal table and also checks
					for new line terminator to then increment the stream adds 0 to
					end of the lexeme string by using the buffer addc function.
					Returning the generated token after assigning STR_T to the string token.
*****************************************************/
Token aa_func10(char lexeme[])
{
	Token t;
	unsigned int i;
	/* Store the lexmeme paramter into the string literal table */
	t.code = STR_T; 

	/* Attribute for the token is set */
	t.attribute.str_offset = b_limit(str_LTBL);
	for (i = 0; i < strlen(lexeme); i++)
	{
		/* Copy the lexme content into str_LTBLS */
		if (lexeme[i] != '"') 
			b_addc(str_LTBL, lexeme[i]);

		if (lexeme[i] == '\n')
			line++;
	}

	/* add SEOF */
	b_addc(str_LTBL, '\0');

	return t;
}

/*****************************************************
Function Name:		aa_func12
Purpose:			Returns an error token
Author:				Aria Gomes & Nicholas King
History/Version:	February 25			 v.1.2
Called Functions:	strlen()
Parameters:			char lexeme[]
Return Value:		Token t
Algorithm:			Adds the attributes of the tokens against several casses
					and returns the token
*****************************************************/
Token aa_func12(char lexeme[])
{
	Token t;
	unsigned int i;

	/* set code */
	t.code = ERR_T;

	/* check length of lexeme */
	if (strlen(lexeme) > ERR_LEN) 
	{
		/*Only first 3 characters are stroed in err_lexx*/
		for (i = 0; i < ERR_LEN - 3; i++) 
		{ 
			if (lexeme[i] == '\n')
				line++;
			t.attribute.err_lex[i] = lexeme[i];
		}
		/*Then three dots are addted to the end of err_Lex*/
		for (i; i < ERR_LEN; i++)
			t.attribute.err_lex[i] = '.'; /* add .'s for the final 3 */
		t.attribute.err_lex[i] = '\0'; /* add SEOF */
	}
	else 
	{
		/* If the error lexeme contains line terminators */
		for (i = 0; i < strlen(lexeme); i++) /* add lexeme to err_lex */
		{
			/*Counter must be incremented*/
			if (lexeme[i] == '\n')
				line++;
			t.attribute.err_lex[i] = lexeme[i];
		}
		/* set the apropriate token code*/
		t.attribute.err_lex[i] = '\0';
	}
	return t;
}

/* HERE YOU WRITE YOUR ADDITIONAL FUNCTIONS(IF ANY)
FOR EXAMPLE
*/

int iskeyword(char* kw_lexeme) 
{
	int i;
	/*checks if a match of the keyword lexeme and the kw_table is present than increments*/
	for (i = 0; i < KWT_SIZE; i++) 
	{
		if (strcmp(kw_table[i], kw_lexeme) == 0) 
			return i;
		
	}
	/*If fails return -1*/
	return RT_FAIL_1;
}