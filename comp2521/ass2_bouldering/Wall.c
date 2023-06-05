// Implementation of the Wall ADT

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Wall.h"

struct wall {
   // TODO
   int height;
   int width;
   struct rock **wallGrid;
};

static int compareRocks(const void *ptr1, const void *ptr2);

/**
 * Creates a new blank wall with the given dimensions
 */
Wall WallNew(int height, int width) {
    // TODO
    Wall newWall = malloc(sizeof(struct wall));
    assert(newWall != NULL);
    newWall->height = height;
    newWall->width = width;
    
    // allocate memory for each row
    newWall->wallGrid = malloc(height * sizeof(struct rock *));
    assert(newWall->wallGrid != NULL);
    // allocate memory for each column and initialise with 0
    for (int i = 0; i < height; i++) {
      newWall->wallGrid[i] = calloc(width, sizeof(struct rock));
      assert(newWall->wallGrid[i] != NULL);
    }
    
    
    for (int i = 0; i < height ; i++)
    {  
        for (int j = 0; j < width; j++)
        {
            newWall->wallGrid[i][j].col = -1;
            newWall->wallGrid[i][j].colour = -1;
            newWall->wallGrid[i][j].row = -1;
              
        }
    }
    return newWall;
}

/**
 * Frees all memory allocated to the wall 
 */
void WallFree(Wall w) {
    // TODO
    assert(w != NULL);
    for(int i = 0; i < w->height ;i++)
    {
        free(w->wallGrid[i]);
    }
    free(w->wallGrid);
    free(w);
}

/**
 * Returns the height of the wall
 */
int WallHeight(Wall w) {
    // TODO
    return w->height;
}

/**
 * Returns the width of the wall
 */
int WallWidth(Wall w) {
    // TODO
    return w->width;
}

/**
 * Adds a rock to the wall
 * If there is already a rock at the given coordinates, replaces it
 */
void WallAddRock(Wall w, struct rock rock) {
    // TODO
    int row = rock.row;
    int col = rock.col;
    w->wallGrid[row][col] = rock;
}

/**
 * Returns the number of rocks on the wall
 */
int WallNumRocks(Wall w) {
    // TODO
    int rows = WallHeight(w);
    int cols = WallWidth(w);
    
    int count = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
           if ((w->wallGrid[i][j]).col != -1)
                count++;
        }
    }
    return count;
}

/**
 * Stores all rocks on the wall in the given `rocks` array and returns
 * the number of rocks stored. Assumes that the array is at least as
 * large as the number of rocks on the wall.
 */
int WallGetAllRocks(Wall w, struct rock rocks[]) {
    // TODO
    int rows = WallHeight(w);
    int cols = WallWidth(w);

    int index = 0;
    for (int i = 0; i < rows ; i++)
    {  
        for (int j = 0; j < cols; j++)
        {
            if (w->wallGrid[i][j].col != -1){
                rocks[index] = w->wallGrid[i][j];
                index++;
            }  
        }
    }
    
    return index;
}

/**
 * Stores all rocks that are within a distance of `dist` from the given
 * coordinates in the given `rocks` array and returns the number of rocks
 * stored. Assumes that the array is at least as large as the number of
 * rocks on the wall.
 */
int WallGetRocksInRange(Wall w, int row, int col, int dist,
                        struct rock rocks[])
{
    // TODO
    int rows = WallHeight(w);
    int cols = WallWidth(w);
    
    int index = 0;
    for (int i = 0; i < rows ; i++)
    {  
        for (int j = 0; j < cols; j++)
        {
            int y = w->wallGrid[i][j].row;
            int x = w->wallGrid[i][j].col;

            if(sqrt(pow((row-y),2) + pow((col-x),2)) <= dist){
                rocks[index] = w->wallGrid[i][j];
                index++;
            }
        }
    }   
    return index;
}

/**
 * Stores all rocks with the colour `colour` that are within a distance
 * of `dist` from the given coordinates in the given `rocks` array and
 * returns the number of rocks stored. Assumes that the array is at
 * least as large as the number of rocks on the wall.
 */
int WallGetColouredRocksInRange(Wall w, int row, int col, int dist,
                                Colour colour, struct rock rocks[])
{
    // TODO
    int rows = WallHeight(w);
    int cols = WallWidth(w);
    
    int index = 0;
    for (int i = 0; i < rows ; i++)
    {  
        for (int j = 0; j < cols; j++)
        {
            if( w->wallGrid[i][j].colour <= colour){
                int y = w->wallGrid[i][j].row;
                int x = w->wallGrid[i][j].col;
            
                int distBTP = sqrt(pow((row-y),2) + pow((col-x),2));
                if(distBTP <= dist){
                    rocks[index] = w->wallGrid[i][j];
                    index++;
                }
            }   
        }
    }
    return index;
}

////////////////////////////////////////////////////////////////////////

/**
 * Prints the wall out in a nice format
 * NOTE: DO NOT MODIFY THIS FUNCTION! This function will work once
 *       WallGetAllRocks and all the functions above it work.
 */
void WallPrint(Wall w) {
    int height = WallHeight(w);
    int width = WallWidth(w);
    int numRocks = WallNumRocks(w);
    struct rock *rocks = malloc(numRocks * sizeof(struct rock));
    WallGetAllRocks(w, rocks);
    qsort(rocks, numRocks, sizeof(struct rock), compareRocks);

    int i = 0;
    for (int y = height; y >= 0; y--) {
        for (int x = 0; x <= width; x++) {
            if ((y == 0 || y == height) && (x == 0 || x % 5 == 0)) {
                printf("+ ");
            } else if ((x == 0 || x == width) && (y == 0 || y % 5 == 0)) {
                printf("+ ");
            } else if (y == 0 || y == height) {
                printf("- ");
            } else if (x == 0 || x == width) {
                printf("| ");
            } else if (i < numRocks && y == rocks[i].row && x == rocks[i].col) {
                char *color;
                switch (rocks[i].colour) {
                    case GREEN: color = "\x1B[32m"; break;
                    case TEAL:  color = "\x1B[96m"; break;
                    case PINK:  color = "\x1B[95m"; break;
                    case RED:   color = "\x1B[91m"; break;
                    default:    color = "\x1B[0m";  break;
                }
                printf("%so %s", color, RESET);
                i++;
            } else {
                printf("\u00B7 ");
            }
        }
        printf("\n");
    }

    free(rocks);
}

static int compareRocks(const void *ptr1, const void *ptr2) {
    struct rock *r1 = (struct rock *)ptr1;
    struct rock *r2 = (struct rock *)ptr2;
    if (r1->row != r2->row) {
        return r2->row - r1->row;
    } else {
        return r1->col - r2->col;
    }
}

