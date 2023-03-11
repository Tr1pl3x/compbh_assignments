// CSE Valley
// cse_valley.c
//
// This program was written by Pyae Sone Oo (z5271704)
// on 14-2-2022
//
// Version 1.0.0 (2021-10-04): Assignment Released.
//
// TODO: Description of your program.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_SIZE 50
#define MAX_NUM_SEED_TYPES 26
#define LAND_SIZE 8
#define NO_SEED ' '
#define TRUE 1
#define FALSE 0

struct land {
    int is_watered;
    char seed_name;
};

struct seeds {
    char name;
    int amount;
};

struct farmer {
    int curr_col;
    int curr_row;
    char curr_dir;
};


// HINT: You will be using this function in stage 2!
void print_land(struct land farm_land[LAND_SIZE][LAND_SIZE], struct farmer cse_farmer);

// Provided functions use for game setup
// You do not need to use these functions yourself.
struct farmer initialise_farmer(struct farmer cse_farmer);
void initialise_seeds(struct seeds seed_collection[MAX_NUM_SEED_TYPES]);
void initialise_land(struct land farm_land[LAND_SIZE][LAND_SIZE]);
void print_top_row(struct land farm_land[LAND_SIZE][LAND_SIZE], int row);
void print_farmer_row(struct land farm_land[LAND_SIZE][LAND_SIZE], struct farmer cse_farmer);

// My functions
int find_seed(struct seeds seed_collection[], int size, int data );
int search_seed(struct seeds seed_collection[], int size, int data);
void plantWater(struct farmer cse_farmer, struct land farm_land[LAND_SIZE][LAND_SIZE]);
void plantSeed( struct farmer cse_farmer, struct land farm_land[LAND_SIZE][LAND_SIZE], struct seeds seed_collection[MAX_NUM_SEED_TYPES], char data, int max);
void seeds_data(struct seeds seed_collection[MAX_NUM_SEED_TYPES], char data, int max, int operation);
void dir_test(struct farmer cse_farmer, struct land farm_land[LAND_SIZE][LAND_SIZE], struct seeds seed_collection[MAX_NAME_SIZE], char plant_name, int item);
void scatter_row(struct farmer cse_farmer, struct land farm_land[LAND_SIZE][LAND_SIZE], struct seeds seed_collection[MAX_NUM_SEED_TYPES], char data, int item );
void scatter_col(struct farmer cse_farmer, struct land farm_land[LAND_SIZE][LAND_SIZE], struct seeds seed_collection[MAX_NUM_SEED_TYPES], char data, int item );




int main(void) {

    struct farmer cse_farmer = {};
    cse_farmer = initialise_farmer(cse_farmer);

    struct land farm_land[LAND_SIZE][LAND_SIZE];
    initialise_land(farm_land);

    struct seeds seed_collection[MAX_NUM_SEED_TYPES];
    initialise_seeds(seed_collection);

    printf("Welcome to CSE Valley, farmer!\n");
    printf("Congratulations, you have received 60 seeds.\n");
    printf("How many different seeds do you wish to have? ");
    
    // STAGE 1.1
    // TODO: Scan the number of different seeds here!
    // Note: Please remove all TODO's in your submission (including this comment)

    int num_seeds , startNumSeed;
    scanf("%d", &num_seeds);
    startNumSeed = 60 / num_seeds;

    printf("Enter the names of the seeds to be given:\n");
    for ( int i = 0; i < num_seeds; i++) {

        char temp_name;
        scanf(" %c", &temp_name);
        seed_collection[i].name = temp_name;
        seed_collection[i].amount = startNumSeed;
    
    }

    /*
    // Test for stage 1.1
    for ( int i = 0; i < num_seeds; i++) {

        if ( i == 0 ) {
            printf("The farmer has:\n");
        }
        printf(" %d of seed named '%c'\n", startNumSeed, seed_collection[i].name);
    }

    */

   

    printf("Game Started!\n");

    //STAGE 1.2
    // TODO: When you start stage 1.2, you will need to replace the below
    // with a loop that reads and executes commands until EOF.
    // See the assignment specification for more information!
    // for more information!

    printf("Enter command: ");
    getchar(); // to elimiate taking in the ENTER from the previous prompt.

    int cmd = getchar();
    while ( cmd != EOF ) {
        

        if (cmd == 'a') {

            for ( int x = 0; x < num_seeds; x++) {

                if ( x == 0) {

                    printf("  Seeds at your disposal:\n");
                }
                printf("  - %d seed(s) with the name '%c'\n", seed_collection[x].amount, seed_collection[x].name);
            }

        } else if ( cmd == 's') {

            getchar();
            int the_seed = getchar();
            int found = find_seed(seed_collection, num_seeds, the_seed);

            if ( found > 0 ) {

                printf("  There are %d seeds with the name '%c'\n", seed_collection[found].amount, the_seed);

            } else if ( found == -1) {

                printf("  Seed name has to be a lowercase letter\n");

            } else {

                printf("  There is no seed with the name '%c'\n", the_seed );
            }

        } else if ( cmd == 'l') {

            print_land(farm_land, cse_farmer);
            
        } else if ( cmd == '>') {

            if ( cse_farmer.curr_col < LAND_SIZE -1 && cse_farmer.curr_dir == '>' ) {

                cse_farmer.curr_col++;
            } else {

                cse_farmer.curr_dir = '>';
            }

        } else if ( cmd == '<')  {

            if ( cse_farmer.curr_col > 0 && cse_farmer.curr_dir == '<' ) {
                
                cse_farmer.curr_col--;
            } else {

                cse_farmer.curr_dir = '<';

            }

        } else if ( cmd == '^') {

            if ( cse_farmer.curr_row > 0 && cse_farmer.curr_dir == '^') {
            
                cse_farmer.curr_row--;
            } else {

                cse_farmer.curr_dir = '^';
            }

        } else if ( cmd == 'v') {

            if ( cse_farmer.curr_row < LAND_SIZE - 1  && cse_farmer.curr_dir == 'v' ) {

                cse_farmer.curr_row++;
            } else {

                cse_farmer.curr_dir = 'v';
            }

        } else if ( cmd == 'o') {

            char choice;
            scanf(" %c", &choice);

            if ( choice == 'w') {
                
                plantWater(cse_farmer, farm_land);


            } else if ( choice == 'p') {

                char plant_name;
                scanf(" %c", &plant_name);
                plantSeed( cse_farmer, farm_land, seed_collection, plant_name, num_seeds);

            }

        } else if ( cmd == 'p' ) {

            char plant_name;
            scanf(" %c", &plant_name);
            
            if (  plant_name >= 'a' && plant_name <= 'z' ) {

                int found = find_seed(seed_collection, num_seeds, plant_name);
                if ( found == 1 ) {

                    int item = search_seed(seed_collection,num_seeds, plant_name);
                    dir_test(cse_farmer, farm_land, seed_collection, plant_name, item);

                } else { 

                    printf("  There is no seed with the name '%c'\n", plant_name);
                }

            } else { 

                printf("  Seed name has to be a lowercase letter\n");
            }
           
        }
        getchar();
        printf("Enter command: ");
        cmd = getchar();

    }

   // printf("\n");


    return 0;
}


////////////////////////////////
//     Provided Functions     //
////////////////////////////////

// Prints the structs land (including locating where the
// farmer is)
// You will need this function in Stage 2.
void print_land(struct land farm_land[LAND_SIZE][LAND_SIZE],
                struct farmer cse_farmer) {
    printf("|---|---|---|---|---|---|---|---|\n");
    int i = 0;
    while (i < LAND_SIZE) {
        print_top_row(farm_land, i);
        // only prints mid and bottom row when the farmer
        // is in that row
        if (i == cse_farmer.curr_row) {
            print_farmer_row(farm_land, cse_farmer);
        }
        printf("|---|---|---|---|---|---|---|---|\n");
        i++;
    }
}

// NOTE: You do not need to directly call any of the functions
// below this point. You are allowed to modify them, but you
// do not need to.

// Initialises struct farmer to its default state upon starting
// in which the farmer will be on the top left of the farm
// facing to the right (as noted by '>')
struct farmer initialise_farmer(struct farmer cse_farmer) {
    cse_farmer.curr_col = 0;
    cse_farmer.curr_row = 0;
    cse_farmer.curr_dir = '>';
    return cse_farmer;
}

// Initialises a 2d array of struct land to its default state 
// upon starting, which is that all land is unwatered and
// contains no seed
void initialise_land(struct land farm_land[LAND_SIZE][LAND_SIZE]) {
    int i = 0;
    while (i < LAND_SIZE) {
        int j = 0;
        while (j < LAND_SIZE) {
            farm_land[i][j].is_watered = FALSE;
            farm_land[i][j].seed_name = NO_SEED;
            j++;
        }
        i++;
    }
}

// Initialises struct farmer to its default state upon starting,
// which that all names are initialised as NO_SEED and its
// amount is 0
void initialise_seeds(struct seeds seed_collection[MAX_NUM_SEED_TYPES]) {
    int i = 0;
    while (i < MAX_NUM_SEED_TYPES) {
        seed_collection[i].amount = 0;
        seed_collection[i].name = NO_SEED;
        i++;
    }
}

////////////////////////////////
//      Helper Functions      //
////////////////////////////////

// prints the top row of a particular struct land
void print_top_row(struct land farm_land[LAND_SIZE][LAND_SIZE], int row) {
    int j = 0;
    while (j < LAND_SIZE) {
        printf("|");
        printf("%c", farm_land[row][j].seed_name);
        printf(" ");
        if (farm_land[row][j].is_watered == TRUE) {
            printf("W");
        } else {
            printf(" ");
        }
        j++;
    }
    printf("|");
    printf("\n");
}

// prints the 2 additional row when a farmer is in
// a particular row
void print_farmer_row(struct land farm_land[LAND_SIZE][LAND_SIZE], 
                      struct farmer cse_farmer)  {
    int j = 0;
    while (j < LAND_SIZE) {
        printf("|");
        if (j == cse_farmer.curr_col) {
            if (cse_farmer.curr_dir == '<') {
                printf("<");
            } else {
                printf(" ");
            }
            if (cse_farmer.curr_dir == '^') {
                printf("^");
            } else {
                printf("f");
            }
            if (cse_farmer.curr_dir == '>') {
                printf(">");
            } else {
                printf(" ");
            }
        } else {
            printf("   ");
        }
        j++;
    }
    printf("|");
    printf("\n");
    j = 0;
    while (j < LAND_SIZE) {
        printf("|");
        if (j == cse_farmer.curr_col) {
            printf(" ");
            if (cse_farmer.curr_dir == 'v') {
                printf("v");
            } else if (cse_farmer.curr_dir == '^') {
                printf("f");
            } else {
                printf(" ");
            }
            printf(" ");
        } else {
            printf("   ");
        }
        j++;
    }
    printf("|");
    printf("\n");
}

//find_seed(seed_collection, num_seeds, the_seed); 

int find_seed(struct seeds seed_collection[], int size, int data ) {

    if ( data >= 'a' && data <= 'z' ) {

        for ( int i = 0; i < size; i++ ) {

            if ( seed_collection[i].name == data) {

                return 1;

            }
        }
    } else {

        return -1;
    }

    return 0;
}

int search_seed(struct seeds seed_collection[], int size, int data) {

    for ( int i = 0; i < size; i++ ) {

        if ( seed_collection[i].name == data) {

            return i;

        }
    }

    return -1;
}    

void plantWater(struct farmer cse_farmer, struct land farm_land[LAND_SIZE][LAND_SIZE]) {

    if ( cse_farmer.curr_dir == '>' && cse_farmer.curr_col < LAND_SIZE-1) {

        farm_land[cse_farmer.curr_row][cse_farmer.curr_col+1].is_watered = TRUE;

    } else if (cse_farmer.curr_dir == '<' && cse_farmer.curr_col > 0) {

        farm_land[cse_farmer.curr_row][cse_farmer.curr_col-1].is_watered = TRUE;

    } else if (cse_farmer.curr_dir == '^' && cse_farmer.curr_row > 0) {

        farm_land[cse_farmer.curr_row-1][cse_farmer.curr_col].is_watered = TRUE;

    } else if (cse_farmer.curr_dir == 'v' && cse_farmer.curr_row < LAND_SIZE-1) {

        farm_land[cse_farmer.curr_row+1][cse_farmer.curr_col].is_watered = TRUE;
        
    }

}

void plantSeed( struct farmer cse_farmer, struct land farm_land[LAND_SIZE][LAND_SIZE], struct seeds seed_collection[MAX_NUM_SEED_TYPES], char data, int max) {

    if ( cse_farmer.curr_dir == '>' && cse_farmer.curr_col < LAND_SIZE-1) {

        farm_land[cse_farmer.curr_row][cse_farmer.curr_col+1].seed_name = data;
        seeds_data( seed_collection, data, max, 1);

    } else if (cse_farmer.curr_dir == '<' && cse_farmer.curr_col > 0) {

        farm_land[cse_farmer.curr_row][cse_farmer.curr_col-1].seed_name = data;
        seeds_data( seed_collection, data, max, 1);

    } else if (cse_farmer.curr_dir == '^' && cse_farmer.curr_row > 0) {

        farm_land[cse_farmer.curr_row-1][cse_farmer.curr_col].seed_name = data;
        seeds_data( seed_collection, data, max, 1);

    } else if (cse_farmer.curr_dir == 'v' && cse_farmer.curr_row < LAND_SIZE-1) {

        farm_land[cse_farmer.curr_row+1][cse_farmer.curr_col].seed_name = data;
        seeds_data( seed_collection, data, max, 1);
        
    }

}

void seeds_data(struct seeds seed_collection[MAX_NUM_SEED_TYPES], char data, int max, int operation) {

    if ( operation == 1) {

        for( int i = 0; i < max; i++) {

            if ( seed_collection[i].name == data) {

                seed_collection[i].amount = seed_collection[i].amount - 1;
                break;
            }
        }
    }
}

void dir_test(struct farmer cse_farmer, struct land farm_land[LAND_SIZE][LAND_SIZE], struct seeds seed_collection[MAX_NAME_SIZE], char data, int item){

    if (cse_farmer.curr_dir == 'v') {

        scatter_col( cse_farmer, farm_land, seed_collection, data, item);


    } else if ( cse_farmer.curr_dir == '>') {

        scatter_row( cse_farmer, farm_land, seed_collection, data, item);

    } else { 

        printf(" You cannot scatter seeds ^ or <\n");
    }

}

void scatter_row(struct farmer cse_farmer, struct land farm_land[LAND_SIZE][LAND_SIZE], struct seeds seed_collection[MAX_NUM_SEED_TYPES], char data, int item ) {

    if ( cse_farmer.curr_col < LAND_SIZE && cse_farmer.curr_dir == '>' ) {

        int temp_row = cse_farmer.curr_col;
        while ( temp_row < LAND_SIZE && seed_collection[item].amount != 0 ) {

            farm_land[cse_farmer.curr_row][temp_row].seed_name = data;
            temp_row++;
            seed_collection[item].amount--;

        }

    } 

}

void scatter_col(struct farmer cse_farmer, struct land farm_land[LAND_SIZE][LAND_SIZE], struct seeds seed_collection[MAX_NUM_SEED_TYPES], char data, int item ) {

    if ( cse_farmer.curr_col < LAND_SIZE && cse_farmer.curr_dir == 'v' ) {

        int temp_row = cse_farmer.curr_row;
        while ( temp_row< LAND_SIZE && seed_collection[item].amount != 0 ) {

            farm_land[temp_row][cse_farmer.curr_col].seed_name = data;
            temp_row++;
            seed_collection[item].amount--;

        }
    } 
}

