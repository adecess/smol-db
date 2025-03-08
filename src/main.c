#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file\n");
    return;
}

int main(int argc, char *argv[]) { 
	char *filepath = NULL;
    bool newfile = false;
    int c;

    int dbfd = -1;

    // get option character from command line argument list
    while((c = getopt(argc, argv, "nf:")) != -1) {
        switch(c) {
            case 'n':
                newfile = true;
                break;
            case 'f':
                // optarg points to the string following the option character f, the filepath in this case
                filepath = optarg;
                break;
            case '?':
                printf("Unknown option -%c\n", c);
                break;
            default:
                return -1;
        }
    }

    if (filepath == NULL) {
        printf("Filepath is a required argument\n");
        print_usage(argv);

        return 0;
    }

    if (newfile) {
        dbfd = create_db_file(filepath);
        if (dbfd == -1) {
            printf("Unable to create database file\n");
            return -1;
        }
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd == -1) {
            printf("Unable to create database file\n");
            return -1;
        }
    }

    printf("Newfile: %d\n", newfile);
    printf("Filepath: %s\n", filepath);
}
