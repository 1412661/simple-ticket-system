#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <regex.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "const.h"
#include "type.h"
#include "linklist.h"
#include "function.h"

/**
 * Alert and halt program for critical errors
 * @param format of output strings
 * @param list of variable for output
 */
void error(const char *format, ...)
{
    /**
    * Example use:
    * error("Could not open file", "test.txt", " for writing");
    * Output:
    * Critical error: Could not open file test.txt for writing
    */

    va_list arg;

    va_start(arg, format);
    fprintf(stderr, "Critical error: ");
    vfprintf(stderr, format, arg);
    fprintf(stderr, "\n");
    va_end(arg);

    exit(EXIT_FAILURE);
}

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

/**
 * Convert hex to dec by using strtol() without modified memory
 * @param Hex string
 * @param Pointer to the last character of the hex string
 * @return Dec value
 */
unsigned int hex(char* str, char* end)
{
    // Because strtol() will modify memory,
    // we have to use temporary memory space for input
    char* tmp = clone(str, end-str+1, 0);
    tmp[end-str] = '\0';

    unsigned int result = strtol(tmp, NULL, 16);

    free(tmp);

    return result;
}

/**
 * Check if string satisfy regular expression (POSIX extended standard)
 * @param Input string
 * @param Regular expression
 * @return 0 if matched, 1 if not
 */
int regexCheck(char* data, char* regexString)
{
    // Max matching group (only need two, one for full group, one for child group)
    int tmp = 2;

    struct List* list = parseByRegex(regexString, data, &tmp);
    if (list == NULL)
        return 0;

    freeList(&list);

    return 1;
}

/**
 * Extract URL (value of href property) in <a> tag in html document.
 * @param Pointer to address space that store the document
 * @param Size of the address space
 * @return Singly linked link that contain URL
 */
struct List* extractHref(char* data, unsigned int length)
{
    struct List* list = NULL;		// Result
    char* bcursor = data;			// pointer to first position of URL
    char* ecursor = NULL;			// Pointer to last position of URL
    // (last character right before the double quote)

    char pattern[] = "href=\"";		// Pattern to detec URL

    while (1)
    {
        bcursor = strstr(bcursor, pattern);	// Check if "href=" exist
        if (bcursor == NULL)
            break;
        else
            bcursor += sizeof(pattern)-1;	// Move pointer to the first character of URL string

        ecursor = strstr((char*)(bcursor+1), "\"");	// Check if URL end with double quote in "href" property
        if (ecursor == NULL)
            break;
        else
            ecursor -= 1;	// Move pointer to the last character of URL string
        // (right before the double quote)

        char* str = clone(bcursor, ecursor-bcursor+1, 1);	// Copy (clone) URL to temp string
        str[ecursor-bcursor+1] = '\0';

        if (!regexCheck(str, "(\\S*\\.\\w{1,4})"))	// Use regex to check if URL is link to a file (with file extentsion)
            free(str);
        else
        {
            insertList(&list, newList(str));		// Insert URL to linked list for return.
            bcursor = ecursor + 2;
        }
    }

    return list;
};

/**
 * Get hostname and path from input URL
 * @param input URL
 * @param hostname (domain name) to be returned
 * @param path to be returned
 * @return 0 if URL follow regular format, 1 if not
 */
int extractHostname(char* link, char* hostname, char* path)
{
	// Just init the variable, don't care about its value
    unsigned int nCaptureGroup = 0;
    struct List* l = parseByRegex("[.^\\/]*([a-zA-Z0-9.-]+)(\\/.*)", link, &nCaptureGroup);

    if (nCaptureGroup-1 != 2)
    {
        freeList(&l);
        return EXIT_FAILURE;
    }

    strcpy(hostname, l->next->data);
    strcpy(path, l->data);
    freeList(&l);

    return EXIT_SUCCESS;

}

/**
 * Parse string into several parts by regular expression (POSIX extended standard)
 * @param Regular expressing string
 * @param String need to be parsed
 * @param Maximum capture group. If null, REGEX_MAX_MATCH is used.
 *        Return actual capture groups when finish.
 * @return Linked list that store both full capture group and child capture group.
 */
struct List* parseByRegex(char* regexString, char* string, unsigned int* nCaptureGroup)
{
	// If nCaptureGroup != 0, max capture group (regexMaxMatch) is nCaptureGroup
	// otherwise use REGEX_MAX_MATCH
    unsigned int regexMaxMatch = REGEX_MAX_MATCH;
    if (nCaptureGroup != NULL)
        if (*nCaptureGroup != 0)
        {
            regexMaxMatch = *nCaptureGroup;
            *nCaptureGroup = 0;
        }

    regex_t regex;
    regmatch_t *pmatch = (regmatch_t*)malloc(sizeof(regmatch_t)*regexMaxMatch);

    //REG_EXTENDED: Use POSIX Extended Regular Expression
    if (regcomp(&regex, regexString, REG_EXTENDED))
        error("parseByRegex(): Could not compile regex: ", regexString);

    if (regexec(&regex, string, regexMaxMatch, pmatch, 0))
    {
        regfree(&regex);
        free(pmatch);
        return NULL;
    }

    regfree(&regex);

    struct List* r = NULL;
    for (int i = 0; i < regexMaxMatch; i++)
    {
        if (pmatch[i].rm_so < 0)
            break;

        int len = pmatch[i].rm_eo - pmatch[i].rm_so;			// Get length of matching group
        char* str = clone(string + pmatch[i].rm_so, len, 1);	// Clone matching group into new address space
        str[len] = '\0';

        struct List* tmp = newList(str);						// Insert matching group to linked list
        insertList(&r, tmp);

        if (nCaptureGroup != NULL)								// nCaptureGroup is used to count captured matching groups
            (*nCaptureGroup)++;
    }

    free(pmatch);

    return r;
}

/**
 * Download file by using HTTP/GET request and write it to memory.
 * Usually used with extractHostname()
 * @param Hostname (domain name)
 * @param Path
 * @param IPv4 address in string format (xxx.xxx.xxx.xxx)
 * @return struct FILE that contain data and its size
 */
struct File* download(char* hostname, char* path, char* ip)
{
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(80);

    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == 0)
        error("download(): Cound not not create socket.");

    if (connect(socket_desc,(struct sockaddr*)&server, sizeof(server)) < EXIT_SUCCESS)
        error("download(): Could not establish socket connection to ", ip, ":80");

    char request[BUFFSIZE];
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n Connection: keep-alive\r\n\r\n Keep-Alive: 300\r\n", path, hostname);

    if (send(socket_desc, request, strlen(request), 0) < EXIT_SUCCESS)
        error("download(): Could not send HTTP/GET request");

	// Buffer that store data from recv()
    char buffer[BUFFSIZE];
    int nbuffer;

    struct File* f = (struct File*)malloc(sizeof(struct File));
    f->data = NULL;
    f->length = 0;

    // Memory stream
    // Ref: https://www.gnu.org/software/libc/manual/html_node/String-Streams.html
    FILE* stream = open_memstream((char**)&(f->data), (size_t*)&(f->length));

    while (1)
    {
        nbuffer = recv(socket_desc, buffer, sizeof(buffer), 0);
        if (nbuffer <= 0)
            break;
        else
            fwrite(&buffer, nbuffer, 1, stream);
    }

    fclose(stream);

    // Ignore HTTP response from received data
    char* dataPosition = strstr(f->data, "\r\n\r\n") + 4;
    char* httpHeader = clone(f->data, dataPosition - f->data, 1);
    httpHeader[dataPosition - f->data] = '\0';

    struct List* regexContentLength = parseByRegex("Content-Length: ([0-9]*)", httpHeader, NULL);
    if (regexContentLength == NULL)
        f->length -= (unsigned int)dataPosition - (unsigned int)f->data;
    else
    {
        f->length = atoi(regexContentLength->data);
        freeList(&regexContentLength);
    }

    printf("download(): content length is %d\n", f->length);

    char* tmp = f->data;
    f->data = clone(dataPosition, f->length, 0);
    free(tmp);

    // If transfer-encoding is chunked, clean \r\n
    if (strstr(httpHeader, "Transfer-Encoding: chunked") != NULL)
        cleanChunk(&(f->data), &(f->length));

    free(httpHeader);

    return f;
}

/**
 * When a web server return a webpage whose transfer encoding is chunked,
 * use this function to clean "\r\n"
 * @param Pointer to address space that store the web page
 * @param Size in byte of the web page
 */
void cleanChunk(char** data, unsigned int* length)
{
    char* cursor = *data;

    char* cleanedData = (char*)malloc(sizeof(char)*(*length));
    unsigned int cleanedLength = 0;

    unsigned int chunkLength;
    char* nextPosBegin;

    while (1)
    {
        chunkLength = hex(cursor, strstr(cursor, "\r\n"));		// Get size of chunk
        if (chunkLength == 0)
            break;
        else
        {
            nextPosBegin = strstr(cursor, "\r\n") + 2;          // Get pointer to the first character of data
            memcpy(cleanedData + cleanedLength, nextPosBegin, chunkLength);	// Copy data with chunk length
            cleanedLength += chunkLength;						// Jump over data to the next chunk
            cursor = (char*)(nextPosBegin + chunkLength + 2);	// Push cursor forward
        }
    }
    cleanedData[cleanedLength] = '\0';

    cursor = *data;
    *data = cleanedData;
    *length = cleanedLength;
    free(cursor);
}

/**
 * Get HTTP return code and content type of a URL by using HTTP/HEAD request
 * @param Hostname (domain name)
 * @param Path
 * @param IPv4 address in string format (xxx.xxx.xxx.xxx)
 * @return HTTP return code and content type
 */
struct HTTP_Response* checkLink(char* hostname, char* path, char* ip)
{
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(80);

    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == 0)
        error("Can't not create socket.");

    if (connect(socket_desc,(struct sockaddr*)&server, sizeof(server)) < EXIT_SUCCESS)
        error("checkContentType(): Could not establish socket connection to ", ip, ":80");
    else
        printf("checkContentType(): Socket connection established.\n");

    char request[BUFFSIZE];
    sprintf(request, "HEAD %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, hostname);
    if (send(socket_desc, request, strlen(request), 0) < EXIT_SUCCESS)
        error("checkContentType(): Could not send HTTP/HEAD request");
    else
        printf("checkContentType(): HTTP/HEAD request sent.\n");

    char server_reply[BUFFSIZE];
    char* response = (char*)malloc(sizeof(char)*BUFFSIZE*10);
    int total_len = 0;

    while (1)
    {
        int received_len = recv(socket_desc, server_reply, BUFFSIZE, 0);
        if (received_len <= 0)
            break;
        else
        {
            memcpy(response + total_len, server_reply, received_len);
            total_len += received_len;
        }

        printf("checkContentType(): Receiving data...\n");
    }

    response[total_len] = '\0';

    struct HTTP_Response* http = (struct HTTP_Response*)malloc(sizeof(struct HTTP_Response));

    // Use regular expression to get HTTP return code
    struct List* regexHTTPCode = parseByRegex("HTTP/1.1 ([0-9]*) ", response, NULL);
    http->code = atoi(regexHTTPCode->data);
    freeList(&regexHTTPCode);

    // Use regular expressigon to get content type
    struct List* regexContentType = parseByRegex("Content-Type: ([^;]*)", response, NULL);
    strcpy(http->type, regexContentType->data);
    freeList(&regexContentType);

    free(response);

    return http;
}
