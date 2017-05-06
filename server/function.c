#include <string.h>
#include <stdlib.h>

#include "const.h"
#include "type.h"
#include "function.h"

/**
 * Clone a memory space
 * @param Pointer point to the memory space that need clone
 * @param Size of memory space that need clone
 * @param Padding space. Usually used for string termination character.
 * @return Pointer point to new memory space cloned from the input.
 */
char* clone(char* buffer, unsigned int size, unsigned int padding)
{
    /**
    * Example use:
    * char str1[] = "abc";
    * char* str2 = clone(str1, 3, 1);
    * str2[3] = '\0';
    * Sometime, str1 is in raw format without string termination character '\0'
    */
    return memcpy((char*)malloc(sizeof(char)*(size+padding)), buffer, size);
}

/*char* char_replace(char search, char replace, char* subject)
{
    for (int i = 0; i < strlen(subject); i++)
        if (subject[i] == search)
			subject[i] = replace;

	return subject;
}*/

/*
 * Search and replace a string with another string , in a string
 * Ref: http://www.binarytides.com/str_replace-for-c/
 */
/*char *str_replace(char *search , char *replace , char *subject)
{
    char  *p = NULL , *old = NULL , *new_subject = NULL ;
    int c = 0 , search_size;

    search_size = strlen(search);

    //Count how many occurences
    for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
    {
        c++;
    }

    //Final size
    c = ( strlen(replace) - search_size )*c + strlen(subject);

    //New subject with new size
    new_subject = malloc( c );

    //Set it to blank
    strcpy(new_subject , "");

    //The start position
    old = subject;

    for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
    {
        //move ahead and copy some text from original subject , from a certain position
        strncpy(new_subject + strlen(new_subject) , old , p - old);

        //move ahead and copy the replacement text
        strcpy(new_subject + strlen(new_subject) , replace);

        //The new start position after this search match
        old = p + search_size;
    }

    //Copy the part after the last search match
    strcpy(new_subject + strlen(new_subject) , old);

    return new_subject;
}*/

/*
int main()
{
	const char *url = "http%3A%2F%2ffoo+bar%2fabcd";
	char out[strlen(url) + 1];

	printf("length: %d\n", decode(url, 0));
	puts(decode(url, out) < 0 ? "bad string" : out);

	return 0;
}
*/

/**
 * Find a free thread handler in marking array
 * @param Marking array
 * @return index of the free thread, -1 if all handler is busy.
 */int findEmptyThread(int *thread)
{
    for (int i = 0; i < CONNMAX; i++)
        if (thread[i] == 0)
			return i;

	return -1;
}
