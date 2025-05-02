#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "filesystem.h"

FileList* get_directory_contents(const char* path) {
    FileList* list = malloc(sizeof(FileList));
    list->count = 0;

    DIR* dir = opendir(path);
    if (!dir) {
        return list;
    }

    if (strcmp(path, "/") != 0) {
        strncpy(list->entries[list->count].name, "..", MAX_NAME - 1);
        list->entries[list->count].name[MAX_NAME - 1] = '\0';
        list->entries[list->count].is_dir = true;
        list->count++;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) && list->count < MAX_FILES) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        strncpy(list->entries[list->count].name, entry->d_name, MAX_NAME - 1);
        list->entries[list->count].name[MAX_NAME - 1] = '\0';

        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", path, entry->d_name);
        struct stat st;
        stat(full_path, &st);
        list->entries[list->count].is_dir = S_ISDIR(st.st_mode);

        list->count++;
    }

    closedir(dir);
    return list;
}

void free_file_list(FileList* list) {
    free(list);
}