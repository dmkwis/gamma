#include "batch_mode.h"
#include "interactive_mode.h"

int main() {
    uint32_t *parsed_command = malloc(MAX_NUMBER_OF_COMMANDS * sizeof(uint32_t));
    char *line = NULL;
    size_t size_of_line = 1;
    uint32_t line_count = 0;

    gamma_t *board;
    while(getline(&line, &size_of_line, stdin) != EOF) {
	line_count++;
        process_line(line, parsed_command);
        if(parsed_command[0] == BATCH_MODE || parsed_command[0] == INTERACTIVE_MODE) {
            board = gamma_new(parsed_command[1], parsed_command[2], parsed_command[3], parsed_command[4]);
            if(board == NULL) {
                fprintf(stderr,"ERROR %u\n", line_count);
            }
            else {
                printf("OK %u\n", line_count);

                if(parsed_command[0] == BATCH_MODE) {
                    batch_mode_start(parsed_command, line_count, board);
                }
                else if (parsed_command[0] == INTERACTIVE_MODE) {
                    interactive_mode_start(board, parsed_command[3]);
                }

                break;
            }
        }
        else if(parsed_command[0] != COMMENT && parsed_command[0] != EMPTY_LINE) {
            fprintf(stderr,"ERROR %u\n", line_count);
        }
        
    }

    free(line);
    free(parsed_command);
    return 0;
}
