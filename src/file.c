#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "common.h"


int create_db_file(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd != -1) {
        close(fd);
        printf("File already exists\n");
        return STATUS_ERROR;
    }

    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return STATUS_ERROR;
    }

    return fd;
}

int open_db_file(char *filename) {
    int fd = open(filename, O_RDWR, 0644);
    if (fd == -1) {
        perror("open");
        return STATUS_ERROR;
    }

    return fd;
}

int replace_db_file(int oldfd, char *filename, int tmpfd, char *tmpfile) {
    // Close both files to ensure all data is flushed
    close(oldfd);
    close(tmpfd);

    // Replace the original file with the new one
    if (rename(tmpfile, filename) == -1) {
        perror("Error replacing original file");
        unlink(tmpfile); // Clean up temp file
        return -1;
    }

    return 0;
}
