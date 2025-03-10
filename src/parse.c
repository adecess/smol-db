#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees)
{
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring)
{
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut)
{
}

int output_file(int fd, struct dbheader_t *dbhdr)
{
    if (fd < 0)
    {
        printf("Got a bad FD from user\n");
        return STATUS_ERROR;
    }

    // data is converted from the host's native endianness to network byte order (big-endian)
    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->filesize = htonl(dbhdr->filesize);
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    // reposition file cursor to the beginning of file
    lseek(fd, 0, SEEK_SET);

    // write the header back to the file on disk
    write(fd, dbhdr, sizeof(struct dbheader_t));

    return 0;
}

int validate_db_header(int fd, struct dbheader_t **headerOut)
{
    if (fd < 0)
    {
        printf("Got a bad FD from user\n");
        return STATUS_ERROR;
    }

    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL)
    {
        printf("calloc failed to create a db header\n");
        return STATUS_ERROR;
    }

    if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t))
    {
        perror("read");
        free(header);
        return STATUS_ERROR;
    }

    // The data in the header is in network byte order (big-endian) when you receive it 
    // because that's how it was stored in the file.
    // This is a common practice in binary file formats and network protocols
    // to ensure consistency across different systems.

    // While your program is working with the data,
    // it uses the CPU's native endianness (host byte order),
    // which is typically little-endian on most modern systems like x86/x86_64.
    header->count = ntohs(header->count);
    header->version = ntohs(header->version);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if (header->magic != HEADER_MAGIC)
    {
        printf("Improper header magic\n");
        free(header);
        return -1;
    }

    if (header->version != 1)
    {
        printf("Improper header version\n");
        free(header);
        return -1;
    }

    struct stat dbstat = { 0 };
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size)
    {
        printf("Corrupted database\n");
        free(header);
        return STATUS_ERROR;
    }

    // assign the header to the output pointer so the caller (main.c) can use it
    *headerOut = header;
}

int create_db_header(int fd, struct dbheader_t **headerOut)
{
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL)
    {
        printf("calloc failed to create db header pointer\n");
        return STATUS_ERROR;
    }

    header->version = 1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;

    return STATUS_SUCCESS;
}
