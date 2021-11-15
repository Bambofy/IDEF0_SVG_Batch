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
        if (strstr(dirent->d_name, ".idef") == NULL) {
            continue;
        } else {
            FILE* toc_process;
            char file_path[256U];
            char toc_command[256U];
            char toc_result_buffer[512U];
            unsigned int activity_number_stack[512U];
            size_t activity_number_stack_top;
            unsigned int current_activity_depth;
            unsigned int previous_activity_depth;
            unsigned int activity_number_counter;

            current_activity_depth = 0;
            previous_activity_depth = 0;
            activity_number_counter = 0;

            activity_number_stack_top = 0U;

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
                char activity_number[512U];

                for (unsigned int i = 0U; i < 512U; i++) {  // snip the spaces from the front of the name.
                    if (isspace(toc_result_buffer[i]) == 0) {
                        memcpy(activity_name, &(toc_result_buffer[i]), 512U-i);
                        break;
                    }
                }

                for (unsigned int i = 0U; i < 521U; i++) { // snip the \n \r from the end of the line.
                    if ((activity_name[i] == '\n') || (activity_name[i] == '\r')) {
                        activity_name[i] = '\0';
                    }
                }

                for (unsigned int i = 0U; i < 512U; i++) {  // calculate the node number from the depth.
                    if (isspace(toc_result_buffer[i]) == 0) {
                        previous_activity_depth = current_activity_depth;
                        current_activity_depth = i / 2U;

                        if (current_activity_depth > previous_activity_depth) {
                            activity_number_counter = 0U;
                            activity_number_stack_top++;
                            activity_number_stack[activity_number_stack_top] = activity_number_counter;
                        } else if (current_activity_depth < previous_activity_depth) {
                            activity_number_stack_top--;
                            activity_number_counter = activity_number_stack[activity_number_stack_top];
                            activity_number_counter++;
                            activity_number_stack[activity_number_stack_top] = activity_number_counter;
                        } else {
                            activity_number_counter++;
                            activity_number_stack[activity_number_stack_top] = activity_number_counter;
                        }

                        break;
                    }
                }

                memset(activity_number, 0, 512U);

                for (unsigned int i = 0U; i < activity_number_stack_top + 1U; i++) {
                    char activity_number_text[32U];

                    sprintf(activity_number_text, "%d", activity_number_stack[i]);

                    if (i == 0U) {
                        strcpy(activity_number, activity_number_text);
                    } else {
                        strcat(activity_number, activity_number_text);
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
                strcat(decompose_command, "/A");
                strcat(decompose_command, activity_number);
                strcat(decompose_command, "_");
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
