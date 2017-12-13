#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* General simulation defines */
#define SEED 11234559
#define CAR_DATA_FILE "carData.csv"

/* Struct pr. road placement */
typedef struct road_s {
    int amountOfCars;
    int waitTime;
} road_t;

/* Struct for the traffic light */
typedef struct trafficLight_s {
    int lightDir;
    road_t *rLeftRight;
    road_t *rRightLeft;
    road_t *rUpDown;
    road_t *rDownUp;
} trafficLight_t;

/* Top-down programming declaration of functions */
void fillTrafficLight(trafficLight_t *trafficLightToBeFilled, road_t *rLeftRight,
                      road_t *rRightLeft, road_t *rUpDown, road_t *rDownUp);
void printVisualization(trafficLight_t *trafficLight, int i);
void removeCars(trafficLight_t *trafficLight, int *timeUntilRemove);
void removeCarsIndividual(int *amountRoad1, int *amountRoad2);
void trafficLightLogic(trafficLight_t *trafficLight, int *timer);
void spawnCars(trafficLight_t *trafficLight);
void printCsv(int rUpDown, int rDownUp,int rRightLeft, int rLeftRight, int totalWait);
void waitTimeLogic(trafficLight_t *trafficLight, int *totalWait);

/* The main function */
int main(void) {
    int desiredTicks = 0, i = 0, runs = 0, bDraw = 0, timer = 0, totalWait = 0,
        timeUntilRemove = 0, removeCounter = 0;
    road_t rLeftRight, rRightLeft, rUpDown, rDownUp;
    trafficLight_t trafficLight;

    /* Seed randomization */
    srand(SEED);

    /* Fill traffic light & initialize direction and time until remove */
    fillTrafficLight(&trafficLight, &rLeftRight, &rRightLeft, &rUpDown, &rDownUp);
    trafficLight.lightDir = 1;
    timeUntilRemove = 6;

    /* Ask user for input */
    do {
        printf("Please enter amount of ticks to run: ");
        scanf("%d", &desiredTicks);
    } while(desiredTicks < 0);
    printf("===============\n");
    printf("Do you want the program to visualize each tick?\n");
    do {
        printf("Enter bool value: ");
        scanf("%d", &bDraw);
    } while(bDraw > 1 || bDraw < 0);

    /* The loop for each tick */
    for (i = 0; i < desiredTicks; i++) {
        /* !!! PLEASE NOTICE THAT IT IS NOW USING LEGACY METHOD UNTIL WE CHANGE INT SECOND PER TICK !!! */
        /* Spawn cars on each road */
        spawnCars(&trafficLight);
        trafficLightLogic(&trafficLight, &timer);

        /* Incriment counter, and check if this is the time we remove cars */
        removeCounter++;
        if (removeCounter >= timeUntilRemove) {
            removeCars(&trafficLight, &timeUntilRemove);
            removeCounter = 0;
        }

        /* Prints wait time for lanes to CSV file*/
        waitTimeLogic(&trafficLight, &totalWait);
        printCsv(trafficLight.rUpDown->waitTime, trafficLight.rDownUp->waitTime,
                 trafficLight.rRightLeft->waitTime, trafficLight.rLeftRight->waitTime,
                 totalWait);

        /* If visualization is desired, then print */
        if(bDraw) {
            printVisualization(&trafficLight, i);
        }
        runs++;
    }
    printf("[Total runs: %d]\n", runs);
    return 0;
}

/* Fill the traffic light with information */
void fillTrafficLight(trafficLight_t *trafficLightToBeFilled, road_t *rLeftRight,
                      road_t *rRightLeft, road_t *rUpDown, road_t *rDownUp) {
    trafficLightToBeFilled->rLeftRight = rLeftRight;
    trafficLightToBeFilled->rRightLeft = rRightLeft;
    trafficLightToBeFilled->rUpDown = rUpDown;
    trafficLightToBeFilled->rDownUp = rDownUp;
    /* NULL variables prior to use */
    trafficLightToBeFilled->rLeftRight->amountOfCars = 0;
    trafficLightToBeFilled->rRightLeft->amountOfCars = 0;
    trafficLightToBeFilled->rUpDown->amountOfCars = 0;
    trafficLightToBeFilled->rDownUp->amountOfCars = 0;

    trafficLightToBeFilled->rLeftRight->waitTime = 0;
    trafficLightToBeFilled->rRightLeft->waitTime = 0;
    trafficLightToBeFilled->rUpDown->waitTime = 0;
    trafficLightToBeFilled->rDownUp->waitTime = 0;
    return;
}

/* Creates a .csv that saves how many people are in each direction and total wait time */
void printCsv(int rUpDown, int rDownUp, int rRightLeft, int rLeftRight, int totalWait) {
    FILE *file;
    file = fopen(CAR_DATA_FILE, "a");

    fprintf(file,"%d;%d;%d;%d;%d\n", rUpDown, rDownUp,rRightLeft, rLeftRight, totalWait);

    fclose(file);
    return;
}

/* Print a visualization of the road */
void printVisualization(trafficLight_t *trafficLight, int i) {
    printf("========= TICK %-4d =========\n", i);
    printf("       |     |     |         \n");
    printf("_______| %-4d|     |_______  \n", trafficLight->rUpDown->amountOfCars);
    printf("                     %-4d    \n", trafficLight->rRightLeft->amountOfCars);
    printf("-------      %s      ------- \n", (trafficLight->lightDir < 0) ? "X" : (trafficLight->lightDir == 2) ? "_" : "|");
    printf(" %-4d                        \n", trafficLight->rLeftRight->amountOfCars);
    printf("_______             _______  \n");
    printf("       |     | %-4d|         \n", trafficLight->rDownUp->amountOfCars);
    printf("       |     |     |         \n");
    printf("=============================\n");
    return;
}

/* Removes cars from one tick */
void removeCars(trafficLight_t *trafficLight, int *timeUntilRemove) {
    int randomNumber = 0;
    /* Print when removing cars */
    if (trafficLight->lightDir == 1 || trafficLight->lightDir == 2){
        printf("[Cars removed on %s direction]\n", (trafficLight->lightDir == 1) ? "vertical" : "horisontal" );
    }

    /* Remove cars based on lightDir */
    if (trafficLight->lightDir == 1) {
        removeCarsIndividual(&trafficLight->rUpDown->amountOfCars, &trafficLight->rDownUp->amountOfCars);
    } else if (trafficLight->lightDir == 2){
        removeCarsIndividual(&trafficLight->rRightLeft->amountOfCars, &trafficLight->rLeftRight->amountOfCars);
    } else if (trafficLight->lightDir == -1 || trafficLight->lightDir == -2) {
        /* Do nothing */
    } else {
        printf("ERROR! Wrong lightDir value in removeCars\n");
    }

    /* Change when to remove cars next, based on our defined chances */
    randomNumber = rand() % 100;

    /* 5% chance of 10 sec */
    if (randomNumber <= 5) {
        *timeUntilRemove = 8;
    }
    /* 15% chance of 9 sec */
    else if (randomNumber <= 20 && randomNumber > 5) {
        *timeUntilRemove = 7;
    }
    /* 40% chance of 8 sec */
    else if (randomNumber <= 60 && randomNumber > 20) {
        *timeUntilRemove = 6;
    }
    /* 15% chance of 7 sec */
    else if (randomNumber <= 75 && randomNumber > 60) {
        *timeUntilRemove = 5;
    }
    /* 10% chance of 6 sec */
    else if (randomNumber <= 85 && randomNumber > 75) {
        *timeUntilRemove = 4;
    }
    /* 10% chance of 5 sec */
    else if (randomNumber <= 95 && randomNumber > 85) {
        *timeUntilRemove = 3;
    }
    /* 5% chance of 4 sec*/
    else if (randomNumber <= 100 && randomNumber > 95) {
        *timeUntilRemove = 2;
    } else {
        printf("ERROR in remove cars\n");
    }
    return;
}

/* Removes cars on two roads if they are above 1 car */
void removeCarsIndividual(int *amountRoad1, int *amountRoad2){
    if (*amountRoad1 > 0) {
        *amountRoad1 = *amountRoad1 - 1;
        printf("removed 1\n");
    }
    if (*amountRoad2 > 0) {
        *amountRoad2 = *amountRoad2 - 1;
        printf("removed 2\n");
    }
    return;
}

/* Traffic light logic */
void trafficLightLogic(trafficLight_t *trafficLight, int *timer) {
    /* If timer is positive, then reduce it by one... */
    /* ...else look at the lightDir value */
    if (*timer > 0) {
        *timer = *timer -1 ;
    } else {
        switch(trafficLight->lightDir) {
        case -2:
            trafficLight->lightDir = 1;
            *timer = 45;
            break;
        case -1:
            trafficLight->lightDir = 2;
            *timer = 45;
            break;
        case 1:
            trafficLight->lightDir = -1;
            *timer = 5;
            break;
        case 2:
            trafficLight->lightDir = -2;
            *timer = 5;
            break;
        default:
            printf("ERROR! Wrong lightDir value in trafficLightLogic\n");
            break;
        }
    }
    return;
}

/* Called each tick, spawns a car based on chances from our data*/
/* Spawn random amount of cars between 0 and 4 */
void spawnCars(trafficLight_t *trafficLight) {
    int randomNumber = 0;

    /* Generate number between 0 and 100000 (100% with 2 decimals in intiger form) */
    randomNumber = rand() % 100;

    /*25,88 chance for spawning a car*/
    if (randomNumber <= 25) {
        /* Generate number between 0 and 100000 (100% with 3 decimals in intiger form) */
        randomNumber = rand() % 100;
                                                                                    /*!!!Change % calculations!!!x_X_x*/
        /*35,47% chance for vertical = ~18% pr road*/
        if (randomNumber <= 18) {
            trafficLight->rLeftRight->amountOfCars += 1;
            printf("[Spawned cars on rLeftRight]\n");
        } else if (randomNumber <= 36 && randomNumber > 18) {
            trafficLight->rRightLeft->amountOfCars += 1;
            printf("[Spawned cars on rRightLeft]\n");
        }
        /*64,53% chance for horisontal = ~32% pr road*/
        else if (randomNumber <= 68 && randomNumber > 36) {
            trafficLight->rUpDown->amountOfCars += 1;
            printf("[Spawned cars on rUpDown]\n");
        } else if (randomNumber > 68 && randomNumber <= 100) {
            trafficLight->rDownUp->amountOfCars += 1;
            printf("[Spawned cars on rDownUp]\n");
        } else {
            printf("ERROR! Wrong random number in spawn function\n");
        }
    }
    return;
}

/* Logic for the waitTime */
void waitTimeLogic(trafficLight_t *trafficLight, int *totalWait){
    /* Updates wait timer: 1 sec pr. car*/
    trafficLight->rUpDown->waitTime += trafficLight->rUpDown->amountOfCars;
    trafficLight->rDownUp->waitTime += trafficLight->rDownUp->amountOfCars;
    trafficLight->rRightLeft->waitTime += trafficLight->rRightLeft->amountOfCars;
    trafficLight->rLeftRight->waitTime += trafficLight->rLeftRight->amountOfCars;

    /* Accumilated wait time for all lanes*/
    *totalWait += trafficLight->rUpDown->amountOfCars
               + trafficLight->rDownUp->amountOfCars
               + trafficLight->rRightLeft->amountOfCars
               + trafficLight->rLeftRight->amountOfCars;
    return;
}
