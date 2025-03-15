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
    for (int i = 0; i < dbhdr->count; i++) {
        printf("Employee %d\n", i);
        printf("\tName: %s\n", employees[i].name);
        printf("\tAddress: %s\n", employees[i].address);
        printf("\tHours: %u\n", employees[i].hours);
    }
}

// employees is a SINGLE pointer because we just want to read data
int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring)
{
    printf("%s\n", addstring);

    char *name = strtok(addstring, ",");
    char *address = strtok(NULL, ",");
    char *hours = strtok(NULL, ",");

    printf("%s %s %s\n", name, address, hours);

    strncpy(employees[dbhdr->count - 1].name, name, sizeof(employees[dbhdr->count - 1].name));
    strncpy(employees[dbhdr->count - 1].address, address, sizeof(employees[dbhdr->count - 1].address));

    employees[dbhdr->count - 1].hours = atoi(hours);

    return STATUS_SUCCESS;
}

// employeesOut is a DOUBLE pointer because we want to output it back to the caller (main.c)
int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut)
{
    if (fd < 0)
    {
        printf("Got a bad FD from user\n");
        return STATUS_ERROR;
    }

    int count = dbhdr->count;

    struct employee_t *employees = calloc(count, sizeof(struct employee_t));
    if (employees == NULL) {
        printf("calloc failed\n");
        return STATUS_ERROR;
    }

    // populate data onto employees array
    read(fd, employees, count * sizeof(struct employee_t));

    for (int i = 0; i < count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;
    return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees)
{
    if (fd < 0)
    {
        printf("Got a bad FD from user\n");
        return STATUS_ERROR;
    }

    int realcount = dbhdr->count;

    // data is converted from the host's native endianness to network byte order (big-endian)
    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    // reposition file cursor to the beginning of file
    lseek(fd, 0, SEEK_SET);

    // write the header back to the file on disk
    write(fd, dbhdr, sizeof(struct dbheader_t));

    for (int i = 0; i < realcount; i++) {
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));
    }

    return 0;
}

// headerOut is a DOUBLE pointer because we want to output it back to the caller (main.c)
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
    return STATUS_SUCCESS;
}

// headerOut is a DOUBLE pointer because we want to output it back to the caller (main.c)
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

    // assign the header to the output pointer so the caller (main.c) can use it
    *headerOut = header;

    return STATUS_SUCCESS;
}
