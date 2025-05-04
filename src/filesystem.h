#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "types.h"


FileList* get_directory_contents(const char* path);
void free_file_list(FileList* list);
bool remove_entity_fs(const char* path);
bool rename_entity_fs(const char* old_path, const char* new_path);
bool copy_entity_fs(const char* src_path, const char* dest_path);
bool move_entity_fs(const char* src_path, const char* dest_path);

#endif