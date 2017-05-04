#ifndef _FUNCTION_H_
#define _FUNCTION_H_

/**
 * @file function.h
 * @brief Provide function prototype
 */

 /**
 * Clone a memory space
 * @param Pointer point to the memory space that need clone
 * @param Size of memory space that need clone
 * @param Padding space. Usually used for string termination character.
 * @return Pointer point to new memory space cloned from the input.
 */
char* clone(char* buffer, unsigned int size, unsigned int padding);


char* ask(char* server_ip, int server_port, char* msg);

#endif // _FUNCTION_H_
