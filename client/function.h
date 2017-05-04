#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <stdarg.h>
#include "const.h"
#include "type.h"

/**
 * @file function.h
 * @brief Provide function prototype
 */

/**
 * Alert and halt program for critical errors
 * @param format of output strings
 * @param list of variable for output
 */
void error(const char *format, ...);

/**
 * Get hostname and path from input URL
 * @param input URL
 * @param hostname (domain name) to be returned
 * @param path to be returned
 * @return 0 if URL follow regular format, 1 if not
 */
int extractHostname(char* link, char* hostname, char* path);

/**
 * Clone a memory space
 * @param Pointer point to the memory space that need clone
 * @param Size of memory space that need clone
 * @param Padding space. Usually used for string termination character.
 * @return Pointer point to new memory space cloned from the input.
 */
char* clone(char* buffer, unsigned int size, unsigned int padding);

/**
 * Parse string into several parts by regular expression (POSIX extended standard)
 * @param Regular expressing string
 * @param String need to be parsed
 * @param Maximum capture group. If null, REGEX_MAX_MATCH is used.
 *        Return actual capture groups when finish.
 * @return Linked list that store both full capture group and child capture group.
 */
struct List* parseByRegex(char* regexString, char* string, unsigned int* nCaptureGroup);

/**
 * Download file by using HTTP/GET request and write it to memory.
 * Usually used with extractHostname()
 * @param Hostname (domain name)
 * @param Path
 * @param IPv4 address in string format (xxx.xxx.xxx.xxx)
 * @return struct FILE that contain data and its size
 */
struct File* download(char* hostname, char* path, char* ip);

/**
 * Get HTTP return code and content type of a URL by using HTTP/HEAD request
 * @param Hostname (domain name)
 * @param Path
 * @param IPv4 address in string format (xxx.xxx.xxx.xxx)
 * @return HTTP return code and content type
 */
struct HTTP_Response* checkLink(char* hostname, char* path, char* ip);

/**
 * When a web server return a webpage whose transfer encoding is chunked,
 * use this function to clean "\r\n"
 * @param Pointer to address space that store the web page
 * @param Size in byte of the web page
 */
void cleanChunk(char** data, unsigned int* length);

/**
 * Convert hex to dec by using strtol() without modified memory
 * @param Hex string
 * @param Pointer to the last character of the hex string
 * @return Dec value
 */
unsigned int hex(char* str, char* end);

/**
 * Extract URL (value of href property) in <a> tag in html document.
 * @param Pointer to address space that store the document
 * @param Size of the address space
 * @return Singly linked link that contain URL
 */
struct List* extractHref(char* data, unsigned int length);

/**
 * Check if string satisfy regular expression (POSIX extended standard)
 * @param Input string
 * @param Regular expression
 * @return 0 if matched, 1 if not
 */
int regexCheck(char* data, char* regexString);


#endif // _FUNCTION_H_
