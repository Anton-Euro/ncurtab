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

bool remove_entity_fs(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) return false;

    if (S_ISDIR(st.st_mode)) {
        DIR* dir = opendir(path);
        if (!dir) return false;

        struct dirent* entry;
        while ((entry = readdir(dir))) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            char full_path[MAX_PATH];
            snprintf(full_path, MAX_PATH, "%s/%s", path, entry->d_name);
            remove_entity_fs(full_path);
        }
        closedir(dir);
        return rmdir(path) == 0;
    } else {
        return unlink(path) == 0;
    }
}

bool rename_entity_fs(const char* old_path, const char* new_path) {
    return rename(old_path, new_path) == 0;
}

bool copy_entity_fs(const char* src_path, const char* dest_path) {
    struct stat st;
    if (stat(src_path, &st) != 0) return false;

    if (S_ISDIR(st.st_mode)) {
        mkdir(dest_path, st.st_mode);
        DIR* dir = opendir(src_path);
        if (!dir) return false;

        struct dirent* entry;
        while ((entry = readdir(dir))) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            char src_full[MAX_PATH];
            char dest_full[MAX_PATH];
            snprintf(src_full, MAX_PATH, "%s/%s", src_path, entry->d_name);
            snprintf(dest_full, MAX_PATH, "%s/%s", dest_path, entry->d_name);
            copy_entity_fs(src_full, dest_full);
        }
        closedir(dir);
        return true;
    } else {
        FILE* src = fopen(src_path, "rb");
        FILE* dest = fopen(dest_path, "wb");
        if (!src || !dest) {
            if (src) fclose(src);
            if (dest) fclose(dest);
            return false;
        }

        char buffer[4096];
        size_t bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
            fwrite(buffer, 1, bytes, dest);
        }

        fclose(src);
        fclose(dest);
        return true;
    }
}

bool move_entity_fs(const char* src_path, const char* dest_path) {
    if (rename(src_path, dest_path) == 0) {
        return true;
    }
    if (copy_entity_fs(src_path, dest_path)) {
        remove_entity_fs(src_path);
        return true;
    }
    return false;
}