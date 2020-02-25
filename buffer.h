/**********************************************************************
Filename:				buffer.h
Compiler:				MS Visual Studio 2019
Author:					Aria Gomes, 040 878 659
Course Name/Number:		CST8152 - Compilers, Lab Section: [13]
Assignment #:			1
Date:					January 30 2020
Professor:				Svillen Ranev
Purpose:                Implement a buffer that can operate in three
						different modes: a “fixed-size” buffer, an
						“additive self-incrementing” buffer, and a
						“multiplicative self-incrementing” buffer.
Prototype list:	        b_allocate(); b_addc(); b_clear(); b_free(); b_isfull();
						b_limit(); b_capacity(); b_mark(); b_mode(); b_incfactor();
						b_load(); b_isempty(); b_eob(); b_getc(); b_print();
						b_compact(); b_rflag(); b_retract(); b_reset(); b_getcoffset();
						b_rewind();
Defined constants:      RT_FAIL_1 -1 
						RT_FAIL_2 -2 
						LOAD_FAIL -2 
						DEFAULT_INIT_CAPACITY 200
						DEFAULT_INC_FACTOR 15
						MODE_ADD 1
						MODE_MULTI (-1)
						INC_FACTOR_ADD_MAX 255
						INC_FACTOR_ADD_MIN 1
						INC_FACTOR_MULTI_MAX 100
						INC_FACTOR_MULTI_MIN 1
						INC_FACTOR_A_M 15
						INC_FACTOR_F 0
						MAX_CAP (SHRT_MAX - 1)
						DEFAULT_ZERO   0x0000 
						DEFAULT_FLAGS  0xFFF9
						SET_EOB        0x0004
						RESET_EOB      0xFFFB
						CHECK_EOB      0x0004
						SET_R_FLAG     0x0002
						RESET_R_FLAG   0xFFFD
						CHECK_R_FLAG   0x0002
*********************************************************************/
/*
* File Name: buffer.h
* Version: 1.20.1
* Author: S^R
* Date: 1 January 2020
* Preprocessor directives, type declarations and prototypes necessary for buffer implementation
* as required for CST8152-Assignment #1.
* The file is not completed.
* You must add your function declarations (prototypes).
* You must also add your constant definitions and macros,if any.
*/

#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

/* constant definitions */
#define RT_FAIL_1 (-1)         /* operation failure return value 1 */
#define RT_FAIL_2 (-2)         /* operation failure return value 2 */
#define LOAD_FAIL (-2)         /* load fail return value */
#define DEFAULT_INIT_CAPACITY 200   /* default initial buffer capacity */
#define DEFAULT_INC_FACTOR 15       /* default increment factor */

/* You should add your own constant definitions here */
/*For B_FULL macro function*/
/* #define B_FULL /* to activate the b_isfull macro expansion */
#if defined(B_FULL)
#define b_isfull(pBD) ( pBD == NULL ? RT_FAIL_1 : ( (*pBD).capacity == (*pBD).addc_offset ) ? RT_BUFFER_FULL_TRUE_1 : RT_BUFFER_FULL_FALSE_0 )
#endif

/* operation mode constant definitions*/
#define MODE_FIXED 0
#define MODE_ADD 1
#define MODE_MULTI (-1)
#define INC_FACTOR_ADD_MAX 255
#define INC_FACTOR_ADD_MIN 1
#define INC_FACTOR_MULTI_MAX 100
#define INC_FACTOR_MULTI_MIN 1
#define INC_FACTOR_A_M 15
#define INC_FACTOR_F 0
#define MAX_CAP (SHRT_MAX - 1)

/* Add your bit-masks constant definitions here */
#define DEFAULT_ZERO   0x0000 /*0000 0000 0000 0000*/
#define DEFAULT_FLAGS  0xFFF9 /*1111 1111 1111 1001*/
#define SET_EOB        0x0004 /*0000 0000 0000 0100*/
#define RESET_EOB      0xFFFB /*‭1111 1111 1111 1011‬*/
#define CHECK_EOB      0x0004 /*1111 1111 1111 1001*/
#define SET_R_FLAG     0x0002 /*‭0000 0000 0000 0010‬*/
#define RESET_R_FLAG   0xFFFD /*‭1111 1111 1111 1101‬*/
#define CHECK_R_FLAG   0x0002 /*‭0000 0000 0000 0010‬*/

/* user data type declarations */
typedef struct BufferDescriptor 
{
	char* cb_head;   /* pointer to the beginning of character array (character buffer) */
	short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
	short addc_offset;  /* the offset (in chars) to the add-character location */
	short getc_offset;  /* the offset (in chars) to the get-character location */
	short markc_offset; /* the offset (in chars) to the mark location */
	char  inc_factor; /* character array increment factor */
	char  mode;       /* operational mode indicator*/
	unsigned short flags;     /* contains character array reallocation flag and end-of-buffer flag */
} Buffer, * pBuffer;
/*typedef Buffer *pBuffer;*/

/* function declarations (Prototypes) */
Buffer* b_allocate(short init_capacity, char inc_factor, char o_mode);
pBuffer b_addc(pBuffer const pBD, char symbol);
int b_clear(Buffer* const pBD);
void b_free(Buffer* const pBD);
int b_isfull(Buffer* const pBD);
short b_limit(Buffer* const pBD);
short b_capacity(Buffer* const pBD);
char* b_markc(pBuffer const pBD, short mark);
int b_mode(Buffer* const pBD);
size_t b_incfactor(Buffer* const pBD);
int b_load(FILE* const fi, Buffer* const pBD);
int b_isempty(Buffer* const pBD);
char b_getc(Buffer* const pBD);
int b_eob(Buffer* const pBD);
int b_print(Buffer* const pBD, char nl);
Buffer* b_compact(Buffer* const pBD, char symbol);
char b_rflag(Buffer* const pBD);
short b_retract(Buffer* const pBD);
short b_reset(Buffer* const pBD);
short b_getcoffset(Buffer* const pBD);
int b_rewind(Buffer* const pBD);
char* b_location(Buffer* const pBD);
#endif