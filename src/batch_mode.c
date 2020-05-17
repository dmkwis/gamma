/** @file
 * Implementacja interfejsu batch_mode.h
 *
 * @author Dominik Wisniewski <dw418484@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 16.05.2020
 */

#include "batch_mode.h"

void batch_mode_start(uint32_t *parsed_command, uint32_t line_count, gamma_t *board) {
    char *line = NULL;
    size_t sizeOfLine = 1;

    while(getline(&line, &sizeOfLine, stdin) != EOF) {
	    line_count++;
        process_line_batch_mode(line, parsed_command);

        if(parsed_command[0] == GAMMA_MOVE)
        {
            printf("%d\n", gamma_move(board,parsed_command[1],parsed_command[2],parsed_command[3]));
        }
        else if(parsed_command[0] == GAMMA_GOLDEN_MOVE)
        {
            printf("%d\n", gamma_golden_move(board,parsed_command[1],parsed_command[2],parsed_command[3]));
        }
        else if(parsed_command[0] == GAMMA_BUSY_FIELDS)
        {
            printf("%lu\n", gamma_busy_fields(board,parsed_command[1]));
        }
        else if(parsed_command[0] == GAMMA_FREE_FIELDS)
        {
            printf("%lu\n", gamma_free_fields(board,parsed_command[1]));
        }
        else if(parsed_command[0] == GAMMA_GOLDEN_POSSIBLE)
        {
            printf("%d\n", gamma_golden_possible(board,parsed_command[1]));
        }
        else if(parsed_command[0] == GAMMA_BOARD)
        {
            char *b = gamma_board(board);
            if(b != NULL) {
                printf("%s",b);
            }
            else if(b == NULL) {
                fprintf(stderr,"ERROR %u\n", line_count);
            }
            free(b);
        }
        else if(parsed_command[0] != COMMENT && parsed_command[0] != EMPTY_LINE) {
            fprintf(stderr,"ERROR %u\n", line_count);
        }
        
    }
    
    free(line);
    gamma_delete(board);
}
