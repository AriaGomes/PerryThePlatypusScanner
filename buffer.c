#include "buffer.h"
#include <stdio.h>
/**********************************************************************
Filename:		        buffer.c
Compiler:		        MS Visual Studio 2019
Author:                 Aria Gomes, 040 878 659
Course Name/Number:     CST8152 - Compilers, Lab Section: [13]
Assignment #:           1 - Buffer
Date:			        Februaray 29 2020
Professor:              Svillen Ranev
Purpose:                Implement a buffer that can operate in three
                        different modes: a “fixed-size” buffer, an
                        “additive self-incrementing” buffer, and a
                        “multiplicative self-incrementing” buffer.
Function list:	        b_allocate(); b_addc(); b_clear(); b_free(); b_isfull();
                        b_limit(); b_capacity(); b_markc(); b_mode(); b_incfactor();
                        b_load(); b_isempty(); b_eob(); b_getc(); b_print():
                        b_compact(); b_rflag(); b_retract(); b_reset(); b_getcoffset();
                        b_rewind();
*********************************************************************/
/********************************************************************
Purpose:               This function creates a new buffer in memory.
Author:				   Aria Gomes
Version:               1.0
Parameters:            short init_capacity, char inc_factor , char o_mode
Called functions:	   b_free();
Returns:               NULL, pBuffer
Algorithm:			   Allocate memory for one Buffer using calloc
                       Allocate memory for one dynamic character Buffer
                       Set the buffer's operational mode and inc_factor
                       Copies the givin inc_capacity into the one Buffer
**********************************************************************/
Buffer* b_allocate(short init_capacity, char inc_factor, char o_mode) 
{
    /*Allocate memory for one Buffer structure*/
    Buffer* pBuffer; /*Pointer to the Buffer*/
    pBuffer = (Buffer*)calloc(1, sizeof(Buffer));

    /*check if pointerBuffer is allocated*/
    if (!pBuffer)
        return NULL;

    /*Check if init_capacity is within the boundaries*/
    if (init_capacity < 0 || init_capacity > MAX_CAP) {
        b_free(pBuffer);
        pBuffer = NULL;
        return NULL;
    }

    /*Allocate memory for one dynamic character*/
    /*If initial capacity is 0, create a character buffer with 200 characters*/
    if (init_capacity == 0) 
    {

        init_capacity = DEFAULT_INIT_CAPACITY;
        pBuffer->cb_head = (char*)malloc(sizeof(char) * DEFAULT_INIT_CAPACITY);

        if (pBuffer->cb_head == NULL) {
            b_free(pBuffer);
            pBuffer = NULL;
            return NULL;
        }

        /*Set inc_factor to 15 in mode a and m, 0 in mode f and none if no mode*/
        if (o_mode == 'a')
        {
            pBuffer->inc_factor = INC_FACTOR_A_M;
            pBuffer->mode = MODE_ADD;
        }
        else if (o_mode == 'm') {
            pBuffer->inc_factor = INC_FACTOR_A_M;
            pBuffer->mode = MODE_MULTI;
        }
        else if (o_mode == 'f') {
            pBuffer->inc_factor = INC_FACTOR_F;
            pBuffer->mode = MODE_FIXED;
        }
        else {
            free(pBuffer->cb_head);
            pBuffer->cb_head = NULL;
            b_free(pBuffer);
            pBuffer = NULL;
            return NULL;
        }
        /*Copy the init_capacity value to Buffer strucutre capacity variable*/
        pBuffer->capacity = DEFAULT_INIT_CAPACITY;
    }

    /*If initial capacity is not 0 and in boundaries, create a character buffer with the initial capacity*/
    if (init_capacity != 0) 
    {
        /*Using the given initial capacity*/
        pBuffer->cb_head = (char*)malloc(sizeof(char) * init_capacity);
        if (pBuffer->cb_head == NULL) 
        {
            b_free(pBuffer);
            return NULL;
        }

        /*Set the mode and inc_factor*/
        if (o_mode == 'f' || (unsigned char)inc_factor == INC_FACTOR_F) {
            pBuffer->mode = MODE_FIXED;
            pBuffer->inc_factor = INC_FACTOR_F;
        }
        else if (o_mode == 'a' && (unsigned char)inc_factor <= INC_FACTOR_ADD_MAX && (unsigned char)inc_factor >= INC_FACTOR_ADD_MIN) {

            pBuffer->mode = MODE_ADD;
            pBuffer->inc_factor = inc_factor;
        }
        else if (o_mode == 'm' && (unsigned char)inc_factor <= INC_FACTOR_MULTI_MAX && (unsigned char)inc_factor >= INC_FACTOR_MULTI_MIN) {
            pBuffer->mode = MODE_MULTI;
            pBuffer->inc_factor = inc_factor;
        }
        else 
        {
            free(pBuffer->cb_head);
            pBuffer->cb_head = NULL;
            b_free(pBuffer);
            pBuffer = NULL;
            return NULL;
        }
        /*Copy the init_capacity value to Buffer strucutre capacity variable*/
        pBuffer->capacity = init_capacity;
    }


    /*Set the flags to the default value 0xFFFC*/
    pBuffer->flags = pBuffer->flags & DEFAULT_ZERO;
    pBuffer->flags = pBuffer->flags | DEFAULT_FLAGS;

    return pBuffer;
}
/********************************************************************
Purpose:               The function resets r_flag to 0 and tries to
                       add the character symbol to the character array
                       of the given buffer pointed by pBD.
Author:				   Aria Gomes
Version:               1.0
Parameters:            pBuffer const pBD, char	symbol
Returns:               NULL, pBD
Called functions:	   None
Algorithm:			   If the Buffer is not full the symbol can be stored.
                       If the Buffer is not full it will resize the Buffer.
                       If the operational mode is 0, it returns NULL.
                       If the operational mode is 1, it tries to increase
                       the current capacity.
                       If the operational mode is -1,  it tries to increase
                       the current capacity by using a formula.
**********************************************************************/
pBuffer b_addc(pBuffer const pBD, char symbol) 
{
    short newCapacity; /*New memory size for the array reallocation*/
    short availableSpace; /*Available space inside the array*/
    short newIncrement; /*Value of the expanded size of the array*/
    char* newBuffer; /*point to the head of the new buffer array*/
    char* oldBuffer; /*point to the head of the old buffer array*/
    newCapacity = 0; /*initialize*/

    /*Check if pointer to the Buffer is not NULL*/
    if (!pBD) 
        return NULL;

    /*Reset the Flag r_flag to 0*/
    pBD->flags &= RESET_R_FLAG;

    /*If buffer is full, increase the the current capacity based on the operation mode*/
    if ((short)(pBD->addc_offset * sizeof(char)) == pBD->capacity) 
    {

        /*Fixed-size Mode*/
        if (pBD->mode == MODE_FIXED) 
            return NULL;
        

        /*Additive self-incrementing mode*/
        if (pBD->mode == MODE_ADD) 
        {
            /*Adding inc_factor (converted in bytes) to capacity for increment*/
            newCapacity = pBD->capacity + (unsigned char)pBD->inc_factor;

            if (newCapacity > 0 && newCapacity > MAX_CAP)
                newCapacity = MAX_CAP;
            else if (newCapacity < 0)
                return NULL;
        }

        /*Multiplicative self-incrementing mode*/
        if (pBD->mode == MODE_MULTI) 
        {
            /*Reached maximum capacity*/
            if (pBD->capacity == MAX_CAP)
                return NULL;

            /*Calculate the new capacity*/
            availableSpace = MAX_CAP - pBD->capacity;
            newIncrement = (short)(availableSpace * ((float)pBD->inc_factor / 100));
            newCapacity = pBD->capacity + newIncrement;

            /*Check if increment is okay*/
            if (newIncrement == 0 && pBD->capacity < MAX_CAP) 
                newCapacity = MAX_CAP;
           
            else if (newCapacity > MAX_CAP || newCapacity < 0) 
                return NULL;
        }

        /*When capacity increment in 1 or -1 is successful*/
        oldBuffer = pBD->cb_head;
        newBuffer = (char*)realloc(pBD->cb_head, newCapacity);

        /*Check if the new buffer allocation is done properly or not*/
        if (!newBuffer) 
            return NULL;

        /*Check if the memory is reallocated or not*/
        if (newBuffer != oldBuffer) 
            pBD->flags |= SET_R_FLAG;

        /*Update the new capacity and pointer to the head of the array*/
        pBD->capacity = newCapacity;
        pBD->cb_head = newBuffer;

    }
    /*Add the character to the Buffer Character Array*/
    pBD->cb_head[pBD->addc_offset] = symbol;
    pBD->addc_offset++;
    return pBD;
}
/********************************************************************
Purpose:               The function retains the memory space currently
					   allocated to the buffer.
Author:				   Aria Gomes
Version:               1.0
Parameters:            Buffer * const pBD
Returns:               RT_FAIL_1, 1
Called functions:	   None
Algorithm:			   Re-initializes all appropriate data members of the
					   given Buffer structure (buffer descriptor), such
					   that the buffer will appear empty and the next
					   call to b_addc() will put the character at
					   the beginning of the character buffer.
**********************************************************************/
int b_clear(Buffer* const pBD) 
{
    if (!pBD)
        return RT_FAIL_1;
    pBD->addc_offset = 0;
    pBD->getc_offset = 0;
    pBD->markc_offset = 0;
    return 1;
}
/********************************************************************
Purpose:               The function de-allocates (frees) the memory.
Author:				   Aria Gomes
Version:               1.0
Parameters:            Buffer * const pBD
Returns:               None
Called functions:	   None
Algorithm:			   The function de-allocates (frees) the memory
					   occupied by the character buffer and the Buffer
					   structure (buffer descriptor).
**********************************************************************/
void b_free(Buffer* const pBD) 
{
    /*Check the pointers to the array*/
    if (pBD != NULL) 
    {
        free(pBD->cb_head);
        pBD->cb_head = NULL;
    }

    /*Free the Buffer structure*/
    free(pBD);
}
/********************************************************************
Purpose:               The function  returns 1 if the character buffer
					   is full.
Author:				   Aria Gomes
Version:               1.0
Parameters:            Buffer * const pBD
Returns:               RT_FAIL_1, 1, 0
Called functions:	   None
Algorithm:			   The function returns 1 if the character buffer is full.
					   It returns 0 otherwise. If a run-time error is
					   possible, the function should return –1.
**********************************************************************/
int b_isfull(Buffer* const pBD) 
{
    if (!pBD)
        return RT_FAIL_1;
    if (pBD->addc_offset == pBD->capacity)
        return 1;
    return 0;
}
/********************************************************************
Purpose:               The function returns the current limit of the
					   character buffer.
Author:				   Aria Gomes
Version:               1.0
Parameters:            Buffer * const pBD
Called functions:	   None
Returns:               RT_FAIL_1, pBD->addc_offset
Algorithm:			   The current limit is the amount of space measured
					   in chars that is currently being used by all
					   added (stored) characters. If a run-time error
					   is possible, the function should return –1.
**********************************************************************/
short b_limit(Buffer* const pBD)
{
    if (!pBD)
        return RT_FAIL_1;

    return pBD->addc_offset;
}
/********************************************************************
Purpose:               The funnction returns the current capacity of
					   the character buffer.
Author:				   Aria Gomes
Version:               1.0
Parameters:            Buffer * const pBD
Called functions:	   None
Returns:               RT_FAIL_1, pBD->capacity
Algorithm:			   The funnction returns the current capacity of
					   the character buffer. If a run -time error is
					   possible, the function should return –1.
**********************************************************************/
short b_capacity(Buffer* const pBD) 
{
    if (!pBD)
        return RT_FAIL_1;
    return pBD->capacity;
}
/********************************************************************
Purpose:               The function sets markc_offset to mark.
Author:				   Aria Gomes
Version:               1.0
Parameters:            Buffer * const pBD
Returns:               RT_FAIL_1, pBD->markc_offset
Called functions:	   None
Algorithm:			   The parameter mark must be within the current
					   limit of the buffer (0 to addc_offset inclusive).
					   The function returns the currently set markc_offset.
					   If a run-time error is possible, the function should
					   return -1.
**********************************************************************/
char* b_markc(pBuffer const pBD, short mark)
{
    if (!pBD) 
        return (char*)RT_FAIL_1;
    else if (mark >= 0 && mark <= pBD->addc_offset) 
    {
        /* check to see if mark is within the current limit of the buffer */
        pBD->markc_offset = mark;
        return pBD->cb_head + pBD->markc_offset;
    }

    /* mark is out of bounds */
    return (char*)RT_FAIL_1;
}
/********************************************************************
Purpose:               The function returns the value of mode
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:          RT_FAIL_1, pBD->mode
Algorithm:			   The function returns the value of mode to the
                       calling function. If a run-time error is possible,
                       the function should notify the calling function
                       about the failure.
**********************************************************************/
int b_mode(Buffer* const pBD) 
{
    if (!pBD)
        return RT_FAIL_1;
    return pBD->mode;
}
/********************************************************************
Purpose:               The function returns the non-negative value
                       of inc_factor
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:          (unsigned char)pBD->inc_factor, 256
Algorithm:			   The function returns the non-negative value of
                       inc_factor to the calling function. If a run-time
                       error is possible, the function should return 0x100.
**********************************************************************/
size_t b_incfactor(Buffer* const pBD) 
{
    if (!pBD)
        return (size_t)0x100;

    return (unsigned char)pBD->inc_factor;
}
/********************************************************************
Purpose:               The function loads (reads) an open input file
                       specified by fi
Version:               1.0
Author:				   Aria Gomes
Called functions:	   b_addc();
Parameters:            FILE * const fi, Buffer * const pBD
Return value:          RT_FAIL_1, -2, numOfchars
Algorithm:			   The function loads (reads) an open input file
                       specified by fi into a buffer specified by pBD.
                       The function must use the standard function fgetc(fi)
                       to read one character at a time and the function
                       b_addc() to add the character to the buffer.
**********************************************************************/
int b_load(FILE* const fi, Buffer* const pBD) 
{
    /*Establishing my variables and their values*/
    int numOfChars = 0;         /*Number of characters to return*/
    char charPlaceholder;   /*Serves as a temporary placeholder for each character read from the file*/

    /*To account for any errors*/
    if (!pBD || !fi) 
        return RT_FAIL_1;

    /*Creating an infinite loop to count all characters until the end of file*/
    while (1) 
    {
        charPlaceholder = (char)fgetc(fi);

        if (feof(fi)) 
            break;
        else 
        {
            if (b_addc(pBD, charPlaceholder) == NULL) 
            {
                ungetc(charPlaceholder, fi);
                return LOAD_FAIL;
            }

            /*Incrementing the number of characters at each pass*/
            numOfChars++;
        }
    }

    /*Close file*/
    fclose(fi);

    /*Returning the number of characters*/
    return numOfChars;
}
/********************************************************************
Purpose:               If the addc_offset is 0, the function returns 1
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:          RT_FAIL_1, 0, 1
Algorithm:			   If the addc_offset is 0, the function returns 1.
                       Otherwise it returns 0. If a run -time error is
                       possible, it should return –1.
**********************************************************************/
int b_isempty(Buffer* const pBD) 
{
    if (!pBD)
        return RT_FAIL_1;
    if (pBD->addc_offset == 0)
        return  1;
    return 0;
}
/********************************************************************
Purpose:               The function check if getc_offset and addc_offset are equal.
                       and returns the character at getc_offset.
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL_2, pBD->getc_offset, 0
Algorithm:			   Checks the argument for validity (possible run -time error).
                       If it is not valid, it returns-2.
                       If getc_offset and addc_offset are equal, it sets eob to 1
                       and returns -1. Otherwise it sets eob to 0.
                       Returns the character located at getc_offset.
                       Before returning it increments getc_offset by 1.
**********************************************************************/
char b_getc(Buffer* const pBD)
{
    if (!pBD) 
        return RT_FAIL_2;

    /*Check if we reached the end of the buffer. Set the flags EOB bit to 1 if reached the end, else 0*/
    if (pBD->getc_offset == pBD->addc_offset) 
    {
        pBD->flags |= SET_EOB;
        return 0;
    }
    else 
        pBD->flags &= RESET_EOB;

    /*Increment getc_offset by one. Return character at position getc_offset inside the character array*/
    return pBD->cb_head[pBD->getc_offset++];
}
/********************************************************************
Purpose:               The function returns eob.
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:	       RT_FAIL_1, pBD->eob
Algorithm:			   The function returns eob to the calling function.
                       If a run-time error is possible, it should return –1
**********************************************************************/
int b_eob(Buffer* const pBD) 
{
    /* check to make sure the buffer isn't NULL */
    if (!pBD)
        return RT_FAIL_1;

    /* return the flags value with the CHECK_EOB definition */
    return pBD->flags & CHECK_EOB;
}
/********************************************************************
Purpose:               Using the printf() library function the function prints
                       character by character the contents of the character buffer.
Version:               1.0
Author:				   Aria Gomes
Called functions:	   b_isempty(); b_getcoffset(); b_getc(); b_eob;
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL_1, char_num
Algorithm:			   Before printing the content the function checks if
                       the buffer is empty, and if it is, it prints the
                       following message Empty buffer. Adding a new
                       line at the end and returns. Next, the function
                       prints the content calling b_getc() in a loop and using
                       b_eob() to detect the end of the buffer content.
                       Finally, it prints a new line character.
                       It returns the number of characters printed.
                       The function returns –1 on failure.
**********************************************************************/
int b_print(Buffer* const pBD, char nl) 
{
    short tmp = 0;
    int char_num = 0;
    char temp = 0;
    if (pBD == NULL || pBD->cb_head == NULL) 
    {
        printf("\nbuffer is empty\n");
        return RT_FAIL_1;
    }  // Check to see if the buffer empty
    tmp = b_getcoffset(pBD);
    pBD->getc_offset = 0;
    for (;;) //Print the contents of the file given to the buffer
    {
        temp = b_getc(pBD);
        if (b_eob(pBD))
            break;
        else
            printf("%c", temp);
        char_num++;
    }
    if (nl)
        printf("\n");// Print the newline character if the nl parameter does not equal 0
    return char_num;
}

/********************************************************************
Purpose:               For all operational modes of the buffer the
                       function shrinks (or in some cases may expand)
                       the buffer to a new capacity.
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD, char symbol
Return value:		   NULL, pBD
Algorithm:			   The new capacity is the current limit plus a space
                       for one more character. In other words the new capacity
                       is addc_offset + 1 converted to bytes. The function uses
                       realloc() to adjust the new capacity, and then updates
                       all the necessary members of the buffer descriptor
                       structure. Before returning a pointer to Buffer,
                       the function adds the symbol to the end of the character
                       buffer (do not use b_addc(), use addc_offset to add
                       the symbol) and increments addc_offset. The function
                       must return NULL if for some reason it cannot to perform
                       the required operation. It must set the buffer r_flag
                       appropriately.
**********************************************************************/
Buffer* b_compact(Buffer* const pBD, char symbol) 
{
    short newCapacity; /*New capacity for the buffer array reallocation*/
    char* oldBuffer; /*Pointer to the head of the old array*/
    char* newBuffer; /*Pointer to the head of the new array*/
    newCapacity = 0; /*Initializes capacity*/

    /*Check the pointers of the Buffer and the character array*/
    if (!pBD || !pBD->cb_head) 
        return NULL;
    

    /*Calculate the new capacity*/
    newCapacity = (short)(sizeof(char) * (pBD->addc_offset + 1));

    /*Check newCapacity*/
    if (newCapacity <= 0) 
        return NULL;

    /*Readjust the characer array size*/
    oldBuffer = pBD->cb_head;
    newBuffer = (char*)realloc(pBD->cb_head, newCapacity);

    /*Check if the new buffer is allocated properly*/
    if (newBuffer == NULL) {
        return NULL;
    }

    /*Check if the Buffer is reallocated or not*/
    if (newBuffer != pBD->cb_head)
        pBD->flags |= SET_R_FLAG;

    /*Update the pointer to the character array and the capacity. */
    pBD->cb_head = newBuffer;
    pBD->cb_head[pBD->addc_offset] = symbol;
    pBD->addc_offset++;
    pBD->capacity = newCapacity;
    return pBD;
}
/********************************************************************
Purpose:               The function returns r_flag.
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL_1, pBD->r_flag
Algorithm:			   The function returns r_flag to the calling function.
                       If a run-time error is possible, it should return –1.
**********************************************************************/
char b_rflag(Buffer* const pBD) 
{
    if (!pBD)
        return RT_FAIL_1;
    return pBD->flags & CHECK_R_FLAG;
}
/********************************************************************
Purpose:               The function decrements getc_offset by 1
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL_1, pBD->getc_offset
Algorithm:			   The function decrements getc_offset by 1. If a
                       run-time error is possible, it should return –1;
                       otherwise it returns getc_offset.
**********************************************************************/
short b_retract(Buffer* const pBD) 
{
    /* check for NULL pointer and ensure that getc_offset - 1 will be a valid getc_offset */
    if (!pBD || ((pBD->getc_offset - 1) < 0)) return RT_FAIL_1;

    /* pre-decrement and return the retracted getc_offset*/
    return --pBD->getc_offset;
}
/********************************************************************
Purpose:               The function sets getc_offset to the value of the current
                       markc_offset.
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL_1, pBD->getc_offset
Algorithm:			   The function sets getc_offset to the value of the current
                       markc_offset. If a run-time error is possible, it should
                       return –1; otherwise it returns getc_offset.
**********************************************************************/
short b_reset(Buffer* const pBD) 
{
    if (!pBD)
        return RT_FAIL_1;
    pBD->getc_offset = pBD->markc_offset;
    return pBD->getc_offset;
}
/********************************************************************
Purpose:               The function returns getc_offset
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL_1, pBD->getc_offset
Algorithm:			   The function returns getc_offset to the calling function.
                       If a run-time error is possible, it should return –1.
**********************************************************************/
short b_getcoffset(Buffer* const pBD) 
{
    if (!pBD)
        return RT_FAIL_1;
    return pBD->getc_offset;
}
/********************************************************************
Purpose:			   The function set the getc_offset and markc_offset
                       to 0.
Version:               1.0
Author:				   Aria Gomes
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL_1, 0
Algorithm:			   The function set the getc_offset and markc_offset
                       to 0, so that the buffer can be reread again. If a
                       run-time error is possible, it should return –1;
                       otherwise it returns 0;
**********************************************************************/
int b_rewind(Buffer* const pBD) 
{
    if (!pBD)
        return RT_FAIL_1;
    pBD->getc_offset = 0;
    pBD->markc_offset = 0;
    return 0;
}

/*
Purpose: Get the location of the current
Author: Aria Gomes
Version: 1.0
Called Functions: None
Parameters: Buffer* const pBD
Return Value: pBD->cb_head + pBD->markc_offset
Algorithm: Takes Buffer pointer head and adds the offset to get the location
*/
char* b_location(Buffer* const pBD) {

    if (!pBD || pBD->markc_offset > pBD->capacity || pBD->markc_offset < 0) 
        return NULL;

    if (!pBD) 
        return NULL;

    /*return pointer to location of char buffer*/
    return (pBD->cb_head + pBD->markc_offset);
}