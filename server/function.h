#ifndef _FUNCTION_H_
#define _FUNCTION_H_

/**
 * @file function.h
 * @brief Function prototype
 */

/**
 * Clone a memory space
 * @param Pointer point to the memory space that need clone
 * @param Size of memory space that need clone
 * @param Padding space. Usually used for string termination character.
 * @return Pointer point to new memory space cloned from the input.
 */
char* clone(char* buffer, unsigned int size, unsigned int padding);

/**
 * Find a free thread handler in marking array
 * @param Marking array
 * @return index of the free thread, -1 if all handler is busy.
 */
int findEmptyThread(int *thread);


//char* char_replace(char search, char replace, char* subject)
//char *str_replace(char *search , char *replace , char *subject);

#endif // _FUNCTION_H_
