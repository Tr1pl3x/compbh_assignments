// Implementation of boulder climbing algorithms

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Wall.h"
#include "climber.h"

// Climber Challenges
#define ENERGYSPEC 0
#define COLOURSPEC 1
#define TURNSPEC 2

//Path Data
struct PathData{
    struct rock *Path;
    int numRocks;
    int energy;
    int numTurns;
};

int FindSourceRocks(Wall w, int reach, struct rock *rocks, struct rock *srcRocks, Colour colour);
int SPTraversal(Wall w, struct rock src, int size, int reach, int index, struct rock *path, int colorOri);
int CountTurns(struct rock *path, int size, int maxEnergy, int *energyCosts);
void InsertPath(struct rock *path, struct rock *shpath, int numrk);
void InsertPathData(struct PathData *pdata, int numrk, int remainEnergy, int numTn, struct rock *path);
void ClearPath(struct rock *path, int numRocks);
/////////////////////////////////////////////////////////////////////////////////
struct path findShortestPath(Wall w, int reach, Colour colour) {
    // TODO - Task 1
    struct path p = {0, NULL};

    int height = WallHeight(w);
    int width = WallWidth(w);
    int numRocks = WallNumRocks(w);

    assert(reach >=0 && reach <= height );
    //get all rocks in wall
    struct rock *rocks = (struct rock*)malloc(numRocks * sizeof(struct rock));
   
    //find source rocks
    struct rock srcRocks[width];
    int numSrc = FindSourceRocks(w, reach, rocks, srcRocks, colour); 

    //detect error
    if(srcRocks[0].row == -1 ){
        return p;
    }
    
    //Shortest Path Traversal
    struct PathData pdata[numRocks];
    for(int i = 0; i < numRocks; i++){
        pdata[i].energy = 0;
        pdata[i].numRocks = 0;
        pdata[i].Path = NULL;
        pdata[i].numTurns = 0;
    }

    int numrk = 0;
    struct rock* path = (struct rock*) malloc(numRocks * sizeof(struct rock));
    for(int i = 0; i < numSrc; i++){
        numrk = SPTraversal(w, srcRocks[i], numRocks, reach, 0, path, COLOURSPEC);

        pdata[i].energy = 0;
        pdata[i].numRocks = numrk;
        pdata[i].Path = path;
        pdata[i].numTurns = 0;
    }

    //not found path
    if(pdata[0].numRocks == 0){
        return p;
    }

    //find less rocks path
    struct PathData LRpath = pdata[0];
    for(int i = 1; pdata[i].numRocks != 0; i++){
        if(pdata[i].numRocks < LRpath.numRocks)
            LRpath = pdata[i];
    }

    p.numRocks = LRpath.numRocks;
    p.rocks = LRpath.Path;

    free(rocks);
    return p;
}
struct path findMinEnergyPath(Wall w, int reach, int energyCosts[NUM_COLOURS]) {
    // TODO - Task 2
    struct path p = {0, NULL};

    int height = WallHeight(w);
    int width = WallWidth(w);
    int numRocks = WallNumRocks(w);

    assert(reach >=0 && reach <= height );
    //get all rocks in wall
    struct rock *rocks = (struct rock*)malloc(numRocks * sizeof(struct rock));
   
    //find source rocks
    struct rock srcRocks[width];
   
    int numSrc = FindSourceRocks(w, reach, rocks, srcRocks, -1); 

    //detect error
    if(srcRocks[0].row == -1 ){
        return p;
    }
    
    //Energy Path Traversal
    struct PathData pdata[numRocks];
    for(int i = 0; i < numRocks; i++){
        pdata[i].energy = 0;
        pdata[i].numRocks = 0;
        pdata[i].Path = NULL;
        pdata[i].numTurns = 0;
    }

    struct rock* path = (struct rock*) malloc(numRocks * sizeof(struct rock));
    for(int i = 0; i < numSrc; i++){
        int numrk = SPTraversal(w, srcRocks[i], numRocks, reach, 0, path, ENERGYSPEC);
        
        int totalEnergy = 0;
        for(int i = 0; i < numrk; i++){
            switch (path[i].colour)
            {
                case GREEN: totalEnergy += energyCosts[GREEN]; break;
                case TEAL:  totalEnergy += energyCosts[TEAL]; break;
                case PINK:  totalEnergy += energyCosts[PINK]; break;
                case RED:   totalEnergy += energyCosts[RED]; break;
                default:    totalEnergy += 0; break;
            }
        }
        pdata[i].energy = totalEnergy;
        pdata[i].numRocks = numrk;
        pdata[i].Path = path;
        pdata[i].numTurns = 0;
    }

    //not found path
    if(pdata[0].numRocks == 0){
        return p;
    }

    //find least energy path
    struct PathData LEpath = pdata[0];
    for(int i = 1; pdata[i].numRocks != 0; i++){
        if(pdata[i].energy < LEpath.energy)
            LEpath = pdata[i];
    }

    p.numRocks = LEpath.numRocks;
    p.rocks = LEpath.Path;

    free(rocks);
    return p;
}

struct path findMinTurnsPath(Wall w, int reach, int energyCosts[NUM_COLOURS],
                             int maxEnergy) {
    // TODO - Task 3
    struct path p = {0, NULL};

    int height = WallHeight(w);
    int width = WallWidth(w);
    int numRocks = WallNumRocks(w);

    assert(reach >=0 && reach <= height );
    //get all rocks in wall
    struct rock *rocks = (struct rock*)malloc(numRocks * sizeof(struct rock));
   
    //find source rocks
    struct rock srcRocks[width];
   
    int numSrc = FindSourceRocks(w, reach, rocks, srcRocks, -1); 

    //detect error
    if(srcRocks[0].row == -1 ){
        return p;
    }
    
    //Energy Path Traversal
    struct PathData pdata;
    for(int i = 0; i < numRocks; i++){
        pdata.energy = 0;
        pdata.numRocks = 0;
        pdata.numTurns = 0;
        pdata.Path = NULL;
    }

    struct rock* path = (struct rock*) malloc(numRocks * sizeof(struct rock));
    for(int i = 0; i < numRocks; i++){
        path[i].col = -1;
        path[i].row = -1;
        path[i].colour = -1;
    }

    for(int i = 0; i < numSrc; i++){

        struct rock* shpath = (struct rock*) malloc(numRocks * sizeof(struct rock));
        for(int i = 0; i < numRocks; i++){
            shpath[i].col = -1;
            shpath[i].row = -1;
            shpath[i].colour = -1;
        }

        int numrk = SPTraversal(w, srcRocks[i], numRocks, reach, 0, shpath, TURNSPEC);
        
        //find rest rocks
        int remainEnergy = CountTurns(shpath, numrk, maxEnergy, energyCosts);
       

        //find turns count
        int numTn = 0;
        int size = 0; 
        while(shpath[size].row != -1){
            if((shpath[size].row == shpath[size+1].row) && (shpath[size].col == shpath[size+1].col)){
                numTn = numTn + 1;
            }
            size++;
        }
	
        if(pdata.numRocks == 0){
            InsertPath(path, shpath, size);
            //free(shpath);
            InsertPathData(&pdata, size, remainEnergy, numTn, path);
            }
        else{
            if(size < pdata.numRocks){
                ClearPath(path, pdata.numRocks);
                InsertPath(path, shpath, size);
                //free(shpath);
                InsertPathData(&pdata, size, remainEnergy, numTn, path);
            }
            else if (size == pdata.numRocks){
                if(numTn < pdata.numTurns){
                    ClearPath(path, pdata.numRocks);
                    InsertPath(path, shpath, size);
                    //free(shpath);
                    InsertPathData(&pdata, size, remainEnergy, numTn, path);
                }
                else{
                    if(remainEnergy > pdata.energy){
                        ClearPath(path, pdata.numRocks);
                        InsertPath(path, shpath, size);
                        //free(shpath);
                        InsertPathData(&pdata, size, remainEnergy, numTn, path);
                    } 
                }
            }
			
        }

        free(shpath);
    }

    //not found path
    if(pdata.numRocks == 0){
        return p;
    }

   

    p.numRocks = pdata.numRocks;
    p.rocks = pdata.Path;
    
    free(rocks);
    return p;
}

/////////////////////////////////////////////////////////////////////////////

int FindSourceRocks(Wall w, int reach, struct rock *rocks, struct rock *srcRocks, Colour colour){
    int numRocks = WallGetAllRocks(w, rocks);
       
    //initialize source rock
    int width = WallWidth(w);
    for(int i = 0; i < width; i++){
        srcRocks[i].col = -1;
        srcRocks[i].colour = -1;
        srcRocks[i].row = -1;
    }

    //define source rocks
    int srcindex = 0;
    for(int i = 0; i < numRocks; i++){
        if(colour != -1){
            if(rocks[i].row <= reach && rocks[i].colour == colour){
                srcRocks[srcindex] = rocks[i];
                srcindex++;
            }
        }
        else{
            if(rocks[i].row <= reach){
                srcRocks[srcindex] = rocks[i];
                srcindex++;
            }
        }
    }

    return srcindex;
}

int SPTraversal(Wall w, struct rock src, int size, int reach, int index, struct rock *path, int challenge)
{
    //start from source
    path[index] = src;
   
    index = index + 1;
    //find adj of source
    struct rock *adjRocks = (struct rock *)malloc(size * sizeof(struct rock));
   
    int numadj = 0;
    if(challenge == COLOURSPEC){
        numadj = WallGetColouredRocksInRange(w, src.row, src.col, reach, src.colour, adjRocks);
    }
    else{
        numadj = WallGetRocksInRange(w,src.row, src.col, reach, adjRocks);
    }
   
    // detect error
    if(numadj == 0){
        return 0;
    }

    //choose path to next step
    struct rock maxrow = adjRocks[0];
    for(int i=1; i<numadj; i++){
        if(adjRocks[i].row > maxrow.row){
            maxrow = adjRocks[i];
        }
    }
    src = maxrow;

    free(adjRocks);
    if(src.row >= (WallHeight(w) - reach)){
        path[index] = src;
        return index + 1;
    }
    else{
        return SPTraversal(w, src, size, reach, index, path, challenge);
    }
}

int CountTurns(struct rock *shpath, int size, int maxEnergy, int *energyCosts){
    int energy = maxEnergy;

    int i = 0; 
    while (i < size){
        switch(shpath[i].colour){
            case GREEN: energy = energy - energyCosts[GREEN]; break;
            case TEAL:  energy = energy - energyCosts[TEAL]; break;
            case PINK:  energy = energy - energyCosts[PINK]; break;
            case RED:   energy = energy - energyCosts[RED]; break;
            default:    break;
        }

        if(shpath[i+1].colour != -1){
            Colour nextRC = shpath[i+1].colour;

            if(energy < energyCosts[nextRC]){
                //move rock to next room
                int j = size-1; 
                while (j >= i+1){
                    shpath[j+1] = shpath[j];
                    j--;
                }

                //insert the rest rock
                shpath[i+1] = shpath[i];
                i = i + 1;
                size = size + 1;
                energy = maxEnergy;
            }
        }
        i = i + 1;
    }
    
    return energy;
}

void InsertPath(struct rock *path, struct rock *shpath, int size){
    for(int i=0; i < size; i++){
        path[i].col = shpath[i].col;
        path[i].row = shpath[i].row;
        path[i].colour = shpath[i].colour;
    }
}

void InsertPathData(struct PathData *pdata, int size, int remainEnergy, int numTn, struct rock *path){
    pdata->energy = remainEnergy; // wrong
    pdata->numRocks = size; //wrong
    pdata->Path = path; //wrong
    pdata->numTurns = numTn;
}

void ClearPath(struct rock *path, int numRocks){
    for(int i = 0; i < numRocks; i++){
            path[i].col = -1;
            path[i].row = -1;
            path[i].colour = -1;
    }
}





