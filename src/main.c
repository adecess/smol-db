#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[])
{
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file\n");
    return;
}

int main(int argc, char *argv[])
{
    char *filepath = NULL;
    char *addstring = NULL;
    char *removestring = NULL;
    bool newfile = false;
    bool list = false;
    bool removefile = false;
    int c;

    int dbfd = -1;
    int tmpfd = -1;
    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employees = NULL;

    // get option character from command line argument list
    while ((c = getopt(argc, argv, "nf:a:r:l")) != -1)
    {
        switch (c)
        {
        case 'n':
            newfile = true;
            break;
        case 'f':
            // optarg points to the string following the option character f, the filepath in this case
            filepath = optarg;
            break;
        case 'a':
            addstring = optarg;
            break;
        case 'r':
            removefile = true;
            removestring = optarg;
            break;
        case 'l':
            list = true;
            break;
        case '?':
            printf("Unknown option -%c\n", c);
            break;
        default:
            return -1;
        }
    }

    if (filepath == NULL)
    {
        printf("Filepath is a required argument\n");
        print_usage(argv);

        return 0;
    }

    if (newfile)
    {
        dbfd = create_db_file(filepath);
        if (dbfd == -1)
        {
            printf("Unable to create database file\n");
            return -1;
        }

        if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR)
        {
            printf("Failed to create database header\n");
            return -1;
        }
    }
    else
    {
        dbfd = open_db_file(filepath);

        if (dbfd == -1)
        {
            printf("Unable to open database file\n");
            return -1;
        }

        if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR)
        {
            printf("Failed to validate database header\n");
            return -1;
        }
    }

    if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
        printf("Failed to read employees");
        return 0;
    }

    if (addstring) {
        dbhdr->count++;
        employees = realloc(employees, dbhdr->count * (sizeof(struct employee_t)));
        add_employee(dbhdr, employees, addstring);
    }

    if (removestring) {
        if (remove_employee(dbhdr, employees, removestring) == STATUS_ERROR) {
            printf("Unable to remove employee\n");
            return -1;
        }
        else {
            dbhdr->count--;
            employees = realloc(employees, dbhdr->count * (sizeof(struct employee_t)));
        };
    }

    if (list) {
        list_employees(dbhdr, employees);
    }

    if (removefile) {
        tmpfd = create_db_file(TMP);

        if (tmpfd == -1)
        {
            printf("Unable to create tmp file\n");
            return -1;
        }

        output_file(tmpfd, dbhdr, employees);

        replace_db_file(dbfd, filepath, tmpfd, TMP);
    }
    else {
        output_file(dbfd, dbhdr, employees);
    }


    return 0;
}
