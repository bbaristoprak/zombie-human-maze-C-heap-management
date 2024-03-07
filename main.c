#include <stdio.h>
#include <stdlib.h>

#define EMPTY '-'
#define HUMAN 'H'
#define ZOMBIE 'Z'

// Function Prototypes
char** read_map_from_txt(const char* filename, int *width, int *height);
void print_grid(char** grid, int width, int height);
char** create_temp_grid(int width, int height);
void simulate(char** grid, int width, int height, int *humans_survived, int *humans_infected, int *zombies_dead);
void cleanup_grid(char** grid, int height);
void temp_to_original(char** temporary_grid, char** original_grid, int width, int height);
int no_humans_left(char** grid, int width, int height);
void human_movement(int round_count, char** grid, char** temporary_grid, int width, int height);
void zombie_movement(int round_count, char** grid, char** temporary_grid, int width, int height);
void is_escaped(int *humans_survived, char** grid, int width, int height);
void humans_kill_zombie(char** grid, int width, int height, int *zombies_dead);
void zombie_infect_human(char** grid, int width, int height, int *humans_infected);


int main(int argc, char **argv) {
    int width, height;
    char *filename = argv[1];
    char **grid = read_map_from_txt(filename, &width, &height);

    printf("Initial Grid:\n");
    print_grid(grid, width, height);
    int humans_infected=0;
    int humans_survived=0;
    int zombies_dead =0;

    simulate(grid, width, height, &humans_survived, &humans_infected, &zombies_dead);

    printf("Humans survived: %d\nHumans infected: %d\nZombies died: %d\n", humans_survived, humans_infected, zombies_dead);
    printf("Final Grid:\n");
    print_grid(grid, width, height);

    cleanup_grid(grid, height);

    return 0;
}

char** read_map_from_txt(const char* filename, int *width, int *height) {
    // This function should read width and height from the given filename and assign these values to the pointers given.
    // It also should allocate the memory for the grid and return a pointer for grid.

    FILE *pFile;
    pFile = fopen(filename, "r");
    if (pFile == NULL){
        printf("Error opening file in read_map_from_txt function.");
        free(pFile);
        exit(EXIT_FAILURE);
    }

    if(pFile != NULL){
        fscanf(pFile, "%d %d", width, height);
    }

    char **grid = (char**) malloc(*height * sizeof(char*));
    int i;
    for (i = 0; i < *height; i++) {
        grid[i] = (char*) malloc(*width * sizeof(char));
    }
    int j;
    for (j = 0; j < *height; j++) {
        int k;
        for (k = 0; k < *width; k++) {
            fscanf(pFile, " %c", &grid[j][k]);
        }
    }
    fclose(pFile);

    return grid;
}

void print_grid(char** grid, int width, int height) {

    int i;
    for (i = 0; i < height; i++) {
        int j;
        for (j = 0; j < width ; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}



char** create_temp_grid(int width, int height) {
    // Initialize a temporary grid with all cells empty. This function is not required but strongly advised for correct implementation.
    // It should return pointer to temporary grid.

    char** temporary_grid = (char**)malloc(height * sizeof(char*));
    int i;
    for (i = 0; i < height; i++) {
        temporary_grid[i] = (char*)malloc(width * sizeof(char));
        int j;
        for (j = 0; j < width; j++) {
            temporary_grid[i][j] = EMPTY;
        }
    }

    return temporary_grid;
}
void simulate(char** grid, int width, int height, int *humans_survived, int *humans_infected, int *zombies_dead) {
    // This function should simulate the grid according to rules until there is no human left.
    // It should call print_grid after each step of simulation. Don't forget to add blank lines to make your output clear.
    // 1- Write a helper function to check whether there are any human left in the grid. Then you can use the output of this function in your while loop.
    // 2- Write functions for human and zombie movements, collision checkers, and call them under this function for each iteration.
    // 3- Behavior of the movement changes for each step. So keep track of simulation steps, and call movement functions with simulation_step modulo 4.
    // 4- If you try to do each change in the original grid directly, then you'll see it is quite complicated. Therefore, do your changes in a temporary grid and then move to original grid for each step.
    // 5- Use Valgrind to ensure there is no memory leaks.

    int round_count = 1;
    char **temp;
    while (no_humans_left(grid, width, height) == 0){

        temp = create_temp_grid(width,height);
        human_movement(round_count, grid, temp, width, height);
        temp_to_original(temp,grid,width,height);
        cleanup_grid(temp, height);

        temp = create_temp_grid(width,height);
        zombie_movement(round_count,grid,temp,width,height);
        temp_to_original(temp,grid,width,height);
        cleanup_grid(temp,height);

        is_escaped(humans_survived, grid, width, height);
        humans_kill_zombie(grid, width, height, zombies_dead);
        zombie_infect_human(grid, width, height, humans_infected);

        printf("%d\n", round_count-1);
        print_grid(grid,width,height);
        round_count++;
    }
}


void cleanup_grid(char** grid, int height) {
    // This function should free the allocated memory for any given grid.

    int rowNum;
    for (rowNum = 0; rowNum < height; rowNum++) {
        free(grid[rowNum]);
    }
    free(grid); // to free the array of pointers representing the columns
}

void temp_to_original(char** temporary_grid, char** original_grid, int width, int height){
    // This function copies the temporary grid to the original grid.
    int i;
    for (i = 0; i < height; i++) {
        int j;
        for (j = 0; j < width; j++) {
            original_grid[i][j] = temporary_grid[i][j];
        }
    }
}

int no_humans_left(char** grid, int width, int height){
    // This function checks whether there are any humans left to continue the simulation.
    int i;
    for (i = 0; i < height; i++) {
        int j;
        for (j = 0; j < width ; j++) {
            if (grid[i][j]==HUMAN){
                return 0;
            }
        }
    }
    return 1;
}

void human_movement(int round_count, char** grid, char** temporary_grid, int width, int height){
    // This function moves the human
    // column = x-axis = width
    // row = y-axis = height
    int row;
    for (row = 0; row < height; row++) {
        int column;
        for(column = 0; column < width; column++){
            if (grid[row][column] == HUMAN){
                int new_row;
                int new_column;
                if (round_count % 2 == 1){ // right
                    new_row = row;
                    new_column = column+1;
                }
                else{ // down
                    new_row = row+1;
                    new_column = column;
                }

                if (new_row >= 0 && new_column >= 0 && new_row < height && new_column < width && grid[new_row][new_column] == EMPTY) {
                    temporary_grid[new_row][new_column] = HUMAN;
                    grid[row][column] = EMPTY;
                }
                else{
                    temporary_grid[row][column] = HUMAN;
                }
            }
            else if (grid[row][column] == ZOMBIE){
                temporary_grid[row][column] = ZOMBIE;
            }
        }
    }
}

void zombie_movement(int round_count, char** grid, char** temporary_grid, int width, int height){
    // This function moves the zombie
    // column = x-axis = width
    // row = y-axis = height
    int row;
    for (row = 0; row < height; row++) {
        int column;
        for(column = 0; column < width; column++){
            if (grid[row][column] == ZOMBIE){
                int new_row;
                int new_column;
                if (round_count % 4 == 1){ // right
                    new_row = row;
                    new_column = column+1;
                }
                else if (round_count % 4 == 2){ // down
                    new_row = row+1;
                    new_column = column;
                }
                else if (round_count % 4 == 3){ // left
                    new_row = row;
                    new_column = column-1;
                }
                else{ // up
                    new_row = row-1;
                    new_column = column;
                }

                if (new_row >= 0 && new_column >= 0 && new_row < height && new_column < width && grid[new_row][new_column] == EMPTY) {
                    temporary_grid[new_row][new_column] = ZOMBIE;
                    grid[row][column] = EMPTY;
                }
                else{
                    temporary_grid[row][column] = ZOMBIE;
                }
            }
            else if (grid[row][column] == HUMAN){
                temporary_grid[row][column] = HUMAN;
            }
        }
    }
}

void is_escaped(int *humans_survived, char** grid, int width, int height){
    // Function checks if the human made it to the bottom right cell. Which means the human escaped.
    if (grid[height-1][width-1] == HUMAN){
        grid[height-1][width-1]=EMPTY;
        (*humans_survived)+=1;
    }
}

void humans_kill_zombie(char** grid, int width, int height, int *zombies_dead){
    // This function checks the surroundings of a zombie, if there are at least 2 humans, zombie gets killed and replaced with empty cell.
    int humans;
    int row;
    for(row = 0; row < height; row++){
        int column;
        for(column = 0; column < width; column++){
            if (grid[row][column] == ZOMBIE){ // If it is a zombie checks the surroundings for humans.
                humans = 0;
                if (row > 0 && grid[row - 1][column] == HUMAN){
                    humans++;
                }
                if(row < height - 1 && grid[row + 1][column] == HUMAN){
                    humans++;
                }
                 if(column > 0 && grid[row][column - 1] == HUMAN){
                     humans++;
                 }
                 if(column < width - 1 && grid[row][column + 1] == HUMAN){
                    humans++;
                }
                if (humans >= 2){ // If enough human found it kills the zombie.
                    grid[row][column] = EMPTY;
                    (*zombies_dead)++;
                }
            }
        }
    }
}

void zombie_infect_human(char** grid, int width, int height, int *humans_infected){
    // This function checks the surroundings of a human, if there is at least 1 zombie, human gets infected and turns into zombie.
    int row;
    for(row = 0; row < height; row++){
        int column;
        for(column = 0; column < width; column++){
            if (grid[row][column] == HUMAN){ // If it is a zombie checks the surroundings for humans.
                if (row > 0 && grid[row - 1][column] == ZOMBIE){
                    grid[row][column] = ZOMBIE;
                    (*humans_infected)++;
                }
                else if(row+1 < height && grid[row + 1][column] == ZOMBIE){
                    grid[row][column] = ZOMBIE;
                    (*humans_infected)++;
                }
                else if(column > 0 && grid[row][column - 1] == ZOMBIE){
                    grid[row][column] = ZOMBIE;
                    (*humans_infected)++;
                }
                else if(column+1 < width && grid[row][column + 1] == ZOMBIE){
                    grid[row][column] = ZOMBIE;
                    (*humans_infected)++;
                }
            }

        }
    }
}

