#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include "const.h"
#include "type.h"
#include "linklist.h"
#include "function.h"

// Test link 1: http://students.iitk.ac.in/programmingclub/course/lectures/
// Test link 2: www.axmag.com/download/pdfurl-guide.pdf
int main(int argc, char *argv[])
{
    //if (argc != 2)
    //	return 0;

    //char* link = "http://students.iitk.ac.in/programmingclub/course/lectures/";
    char* link = "www.axmag.com/download/pdfurl-guide.pdf?abc=def&asd=123";
    //char* link = argv[1];

    char hostname[BUFFSIZE] = "";
    char path[BUFFSIZE] = "";
    char* ip = NULL;

    if (extractHostname(link, hostname, path))
        error("main(): Could not extract hostname and path from link: ", link);

    struct hostent* hostdb = gethostbyname(hostname);
    if (hostdb == NULL)
        error("main(): Cound not resolve ", hostname, " into IP address");

    ip = inet_ntoa(*(struct in_addr*)(hostdb->h_addr_list[0]));	// Convert IPv4 address to string format (xxx.xxx.xxx.xxx)

    printf("hostname:    %s\n", hostname);
    printf("path:        %s\n", path);
    printf("IP address:  %s\n", ip);

    struct HTTP_Response* http = checkLink(hostname, path, ip);

    // Break the program if HTTP return code is 4xx (Client error) or 5xx (Server error)
    if (http->code % 100 == 4 | http->code % 100 == 5)
        //error("Web server return HTTP code ", itoa(http->code, NULL, 10));
        return 0;

    printf("http-type: %s\n", http->type);

    if (strcmp("text/html", http->type))
    {
        struct File* file = download(hostname, path, ip);

        struct List* list = parseByRegex("[a-zA-Z0-9-]*\\.[a-zA-Z0-9]{1,4}", strrchr(path, '/') + 1, NULL);

        unsigned int sizeFileName = strlen(list->data)+1 + strlen(MSSV) + 1;
        char* filename = (char*)calloc(sizeof(char), sizeFileName);
        sprintf(filename, "%s_%s", MSSV, list->data);
        //char* filename = clone(list->data, strlen(list->data)+1, strlen(MSSV));
        freeList(&list);

        FILE* f = fopen(filename, "wb");
        if (f == NULL)
            error("main(): Could not write data with filename: ", filename);
        fwrite(file->data, file->length, 1, f);
        fclose(f);

        free(filename);
        free(file->data);
        free(file);
    }
    else
    {
        struct File* file = download(hostname, path, ip);

        struct List* listFileName = extractHref(file->data, file->length);

        char pattern[] = "/";
        char* tmpPath = strdup(path);
		char *subNameFolder = strtok(tmpPath, pattern);
		struct List* tokenList = NULL;
		while (subNameFolder != NULL)
		{
            insertList(&tokenList, newList(strdup(subNameFolder)));
			subNameFolder = strtok(NULL, pattern);
		}
        subNameFolder = strdup(tokenList->data);
        freeList(&tokenList);


		char folderName[BUFFSIZE];
        sprintf(folderName, "%s_%s", MSSV, subNameFolder);
        mkdir(folderName, 0777);

		char fileNamePath[BUFFSIZE];
		char downloadPath[BUFFSIZE];
        struct List* tmpFileName = listFileName;
        while (tmpFileName != NULL)
        {
			sprintf(downloadPath, "%s/%s", path, tmpFileName->data);
			sprintf(fileNamePath, "%s_%s/%s", MSSV, subNameFolder, tmpFileName->data);
        	struct File* childFile = download(hostname, downloadPath, ip);

			FILE* f = fopen(fileNamePath, "wb");
			if (f == NULL)
				error("main(): Could not write data with filename: ", tmpFileName->data);
			fwrite(childFile->data, childFile->length, 1, f);
			fclose(f);
			free(childFile->data);
			free(childFile);
        	tmpFileName = tmpFileName->next;
        }

        free(tmpPath);
		free(subNameFolder);
        free(file->data);
        free(file);

    }

    free(http);

    return EXIT_SUCCESS;
}



