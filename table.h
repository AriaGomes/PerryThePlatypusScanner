/****************************************
 Filename:		table.h
 Compiler:		MS Visual Studio 2019
 Author:		Aria Gomes & Nicholas King
 Course:		CST8152 - Compilers 013(Aria Gomes) & 011(Nicholas King)
 Assignment:	2 - Scanner(Transition Table)
 Date:			February 25
 Professor:		Svillan Ranev
 Purpose:		Transition Table and function declarations necessary for the scanner implementation
				as required for CST8152 - Assignment #2.
 ***************************************/

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

 /*   Source end-of-file (SEOF) sentinel symbol
  *    '\0' or one of 255,0xFF,EOF
  */

#define EOL 0x0a /* new line */
#define SEOF '\0' /* SEOF Character */
#define S_EOF (unsigned char)255

  /*  Special case tokens processed separately one by one
   *  in the token-driven part of the scanner
   *  '=' , ' ' , '(' , ')' , '{' , '}' , == , <> , '>' , '<' , ';',
   *  white space
   *  !!comment , ',' , ';' , '-' , '+' , '*' , '/', << ,
   *  .AND., .OR. , SEOF,
   */

#define NL '\n'	/* New Line character */
#define ES  11 /* Error state  with no retract */
#define ER  12 /* Error state  with retract */
#define IS -1    /* Inavalid state */

   /* State transition table definition */

/* State transition table definition */
#define TABLE_COLUMNS 8
/*transition table - type of states defined in separate table */
int  st_table[][TABLE_COLUMNS] = {
														/*			[a-zA-Z]	0	 [1-9]		 .		#		"	  SEOF	   other*/

	/* State 0, beginning state*/									  {1,		6,		4,		ES,		ES,		9,		9,		ES},
	/* State 1, digits or letters state */							  {1,		1,		1,		2,		3,		2,		2,		2}, 
	/* State 2, SVID accepting state with retract*/					  {IS,	    IS,		IS,		IS,		IS,		IS,		IS,		IS},
	/* State 3  SVID accepting statde without retract*/				  {IS,	    IS,		IS,		IS,		IS,		IS,		IS,		IS}, 
	/* State 4, non zero didgit state*/								  {ES,	    4,		4,		7,		5,		5,		5,		5},
	/* State 5, decimal int literal accepting state with retract*/    {IS,	    IS,		IS,		IS,		IS,		IS,		IS,		IS},
	/* State 6, 0 state*/											  {ES,	    6,		ES,		7,		5,		5,		ES,		5},
	/* State 7, digit state*/										  {8,	    7,		7,		8,		8,		8,		ES,		8}, 
	/* State 8, floating point literal accepting state with retract*/ {IS,	    IS,		IS,		IS,		IS,		IS,		IS,		IS}, 
	/* State 9, string literal state*/								  {9,		9,		9,		9,		9,		9,		10,		ES}, 
	/* State 10, string literal accept state with retract*/			  {IS,		IS,		IS,		IS,		IS,		IS,		IS,		IS}, 
	/* State 11, error state without retract*/						  {IS,		IS,		IS,		IS,		IS,		IS,		IS,		IS}, 
	/* State 12, error state with retract */						  {IS,		IS,		IS,		IS,		IS,		IS,		IS,		IS} 
};

/* Accepting state table definition */
#define ASWR     1  /* accepting state with retract */
#define ASNR     3  /* accepting state with no retract */
#define NOAS     0  /* not accepting state */

int as_table[] = 
{
	/* State 0 */ NOAS,
	/* State 1 */ NOAS,
	/* State 2 */ ASWR,
	/* State 3 */ ASNR,
	/* State 4 */ NOAS,
	/* State 5 */ ASWR,
	/* State 6 */ NOAS,
	/* State 7 */ NOAS,
	/* State 8 */ ASWR,
	/* State 9 */ NOAS,
	/* State 10 */ ASNR,
	/* State 11 */ ASNR,
	/* State 12 */ ASWR,
};

/* Accepting action function declarations */

Token aa_func02(char* lexeme);
Token aa_func03(char* lexeme);
Token aa_func05(char* lexeme);
Token aa_func08(char* lexeme);
Token aa_func10(char* lexeme);
Token aa_func12(char* lexeme);

/* defining a new type: pointer to function (of one char * argument)
   returning Token
*/

typedef Token(*PTR_AAF)(char* lexeme);


/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
 * Token (*aa_table[])(char lexeme[]) = {
 */

PTR_AAF aa_table[] = 
{
	/* State 0 */ NULL,
	/* State 1 */ NULL,
	/* State 2 */ aa_func02,
	/* State 3 */ aa_func03,
	/* State 4 */ NULL,
	/* State 5 */ aa_func05,
	/* State 6 */ NULL,
	/* State 7 */ NULL,
	/* State 8 */ aa_func08,
	/* State 9 */ NULL,
	/* State 10 */ aa_func10,
	/* State 11 */ aa_func12,
	/* State 12 */ aa_func12
	/* State 13 */ /*NULL*/
};

/* Keyword lookup table (.AND. and .OR. are not keywords) */

#define KWT_SIZE  10

char* kw_table[] =
{
"ELSE",
"FALSE",
"IF",
"PLATYPUS",
"READ",
"REPEAT",
"THEN",
"TRUE",
"WHILE",
"WRITE"
};

#endif