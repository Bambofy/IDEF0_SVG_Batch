#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

int main(int argc, char **argv) {
    struct dirent* dirent;
    DIR* dir;

    dir = opendir(argv[1U]);

    while ((dirent = readdir(dir)) != NULL) {
        FILE* toc_process;
        char file_path[256U];
        char toc_command[256U];
        char toc_result_buffer[512U];

        if (strstr(dirent->d_name, ".idef") == NULL) {
            continue;
        } else {
            strcpy(file_path, argv[1U]);
            strcat(file_path, "/");
            strcat(file_path, dirent->d_name);

            strcpy(toc_command, "toc");
            strcat(toc_command, " <");
            strcat(toc_command, file_path);

            toc_process = popen(toc_command, "r");

            while (fgets(toc_result_buffer, 512U, toc_process)) {
                FILE* decompose_process;
                char decompose_command[2048U];
                char activity_name[512U];

                for (unsigned int i = 0U; i < 512U; i++) {  // snip the spaces from the front of the name.
                    if (isspace(toc_result_buffer[i]) == 0) {
                        memcpy(activity_name, &(toc_result_buffer[i]), 512U-i);
                        break;
                    }
                }

                for (unsigned int i = 0U; i < 521U; i++) { // snip the \n \r from the end of the line.
                    if (isspace(activity_name[i]) != 0) {
                        activity_name[i] = '\0';
                    }
                }

                memset(toc_result_buffer, 0, 512U);

                strcpy(decompose_command, "decompose");
                strcat(decompose_command, " \"");
                strcat(decompose_command, activity_name);
                strcat(decompose_command, "\" ");
                strcat(decompose_command, " <\"");
                strcat(decompose_command, file_path);
                strcat(decompose_command, "\"");
                strcat(decompose_command, " >\"");
                strcat(decompose_command, argv[1U]);
                strcat(decompose_command, "/");
                strcat(decompose_command, activity_name);
                strcat(decompose_command, "\".svg");

                decompose_process = popen(decompose_command, "r");

                pclose(decompose_process);

            }

            pclose(toc_process);
        }
    }

    closedir(dir);

	return 0;
}
