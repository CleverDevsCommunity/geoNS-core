#include "io.h"


uchar cwd[SYS_MAX_PATH_LENGTH];


ulong get_file_size(uchar *file_path) {
    FILE *fp = fopen(file_path, "rb");
    
    if (fp == NULL)
        return 0;

    if (fseek(fp, 0, SEEK_END) < 0) {
        fclose(fp);
        return 0;
    }

    long file_size = ftell(fp);
    fclose(fp);
    return file_size;
}


void get_cwd_path(uchar *file_path, ushort size_of_file_path) {
    strncpy(file_path, cwd, size_of_file_path);
}


uchar is_directory_exists(uchar *directory) {
    struct stat sb;
    return stat(directory, &sb) == 0 && S_ISDIR(sb.st_mode);
}


uchar is_sproc_exists(void) {
    return is_directory_exists("/proc");
}


void init_io_system(uchar *exec_path) {
    if (is_sproc_exists()) {
        #if OS_LINUX == 1
        readlink("/proc/self/exe", cwd, SYS_MAX_PATH_LENGTH);
        #elif OS_FREE_BSD == 1
        readlink("/proc/curproc/file", cwd, SYS_MAX_PATH_LENGTH);
        #elif OS_SOLARIS == 1
        readlink("/proc/self/path/a.out", cwd, SYS_MAX_PATH_LENGTH);
        #endif
    }
    else {
        if (exec_path[0] == '/') {
            strncpy(cwd, exec_path, SYS_MAX_PATH_LENGTH);
        }
        else if (strchr(exec_path, '/') != NULL) {
            #if OS_WINDOWS == 1
            // TODO: coding windows version of path handling
            #else
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                strncat(cwd, exec_path + 1, SYS_MAX_PATH_LENGTH - 1);
            }
            #endif
        }
    }
    
    for (ushort i = strlen(cwd) - 1; i >= 0; i--)
        if (cwd[i] == '/')
            break;
        else
            cwd[i] = '\0';
}