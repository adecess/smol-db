#ifndef FILE_H
#define FILE_H

#define TMP "./tmp.db"

int create_db_file(char *filename);
int open_db_file(char *filename);
int replace_db_file(int oldfd, char *filename, int tmpfd, char *tmpfile);

#endif
