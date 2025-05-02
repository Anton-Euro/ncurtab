#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>

#define MAX_FILES 256
#define MAX_PATH 256
#define MAX_NAME 64

typedef struct {
    char name[MAX_NAME];
    bool is_dir;
} FileEntry;

typedef struct {
    FileEntry entries[MAX_FILES];
    int count;
} FileList;

FileList* get_directory_contents(const char* path);
void free_file_list(FileList* list);
bool remove_file_or_dir(const char* path);
bool rename_file_or_dir(const char* old_path, const char* new_path);
bool copy_file_or_dir(const char* src_path, const char* dest_path);
bool move_file_or_dir(const char* src_path, const char* dest_path);

#endif