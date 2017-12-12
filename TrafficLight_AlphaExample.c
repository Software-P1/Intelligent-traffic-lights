#include <stdio.h>
#include <stdlib.h>

/* General simulation defines */
#define SEED 10
#define AMOUNT_OF_CARS_PASS_PER_TICK 3 /* !!! REMOVE WHEN WE CHANGE TO SECONDS !!! */
#define PERCENT_3_CAR_SPAWN 1 /* !!! REMOVE WHEN WE CHANGE TO SECONDS !!! */
#define PERCENT_2_CAR_SPAWN 2 /* !!! REMOVE WHEN WE CHANGE TO SECONDS !!! */
#define PERCENT_1_CAR_SPAWN 3 /* !!! REMOVE WHEN WE CHANGE TO SECONDS !!! */

/* Struct pr. road placement */
typedef struct road_s {
    int amountOfCars;
    int waitTime;
} road_t;

/* Struct for the traffic light */
typedef struct trafficLight_s {
    int bVertical;
    road_t *rLeftRight;
    road_t *rRightLeft;
    road_t *rUpDown;
    road_t *rDownUp;
} trafficLight_t;

/* Top-down programming declaration of functions */
void fillTrafficLight(trafficLight_t *trafficLightToBeFilled, road_t *rLeftRight,
                      road_t *rRightLeft, road_t *rUpDown, road_t *rDownUp);
void legacySpawnCars(road_t *roadTolegacySpawnCarsOn);
void printVisualization(trafficLight_t *trafficLight, int i);
void removeCars(road_t *road1, road_t *road2);
void trafficLightLogic(trafficLight_t *trafficLight, int *timer);
void spawnCars(road_t *roadToSpawnOn);

/* The main function */
int main(void) {
    int desiredTicks = 0, i = 0, runs = 0, bDraw = 0;
    road_t rLeftRight, rRightLeft, rUpDown, rDownUp;
    trafficLight_t trafficLight;
    int timer = 0;

    /* Seed randomization */
    srand(SEED);

    /* Fill traffic light & initialize direction */
    fillTrafficLight(&trafficLight, &rLeftRight, &rRightLeft, &rUpDown, &rDownUp);
    trafficLight.bVertical = 1;

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
        legacySpawnCars(&rUpDown);
        legacySpawnCars(&rRightLeft);
        legacySpawnCars(&rLeftRight);
        legacySpawnCars(&rDownUp);

        /* Run the traffic light logic */
        trafficLightLogic(&trafficLight, &timer);

        /* Remove cars this tick */
        if(trafficLight.bVertical == 2) {
            removeCars(trafficLight.rRightLeft, trafficLight.rLeftRight);
            trafficLight.rUpDown->waitTime++;
            trafficLight.rDownUp->waitTime++;
        } else if(trafficLight.bVertical == 1) {
            removeCars(trafficLight.rUpDown, trafficLight.rDownUp);
            trafficLight.rRightLeft->waitTime++;
            trafficLight.rLeftRight->waitTime++;
        } else if(trafficLight.bVertical == -1 || trafficLight.bVertical == -2) {
        /* Intentiually left blank */
        } else {
            printf("ERROR - bVertical is a wrong value!!!\n");
        }

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

/* Spawns cars on a single road */
void legacySpawnCars(road_t *roadTolegacySpawnCarsOn) {
    int randomNumber = 0;

    /* Get random number */
    randomNumber = rand() % 100;

    /* The 3% += PERCENT_1_CAR_SPAWN */
    if(randomNumber >= 0 && randomNumber <= 3) {
        roadTolegacySpawnCarsOn->amountOfCars += PERCENT_1_CAR_SPAWN;
    }
    /* The 2% += PERCENT_2_CAR_SPAWN */
    else if(randomNumber >= 4 && randomNumber <= 6) {
        roadTolegacySpawnCarsOn->amountOfCars += PERCENT_2_CAR_SPAWN;
    }
    /* The 1% += PERCENT_3_CAR_SPAWN */
    else if(randomNumber == 7) {
        roadTolegacySpawnCarsOn->amountOfCars += PERCENT_3_CAR_SPAWN;
    }
    return;
}

/* Print a visualization of the road */
void printVisualization(trafficLight_t *trafficLight, int i) {
    printf("========= TICK %-4d =========\n", i);
    printf("       |     |     |         \n");
    printf("_______| %-4d|     |_______  \n", trafficLight->rUpDown->amountOfCars);
    printf("                     %-4d    \n", trafficLight->rRightLeft->amountOfCars);
    printf("-------      %s      ------- \n", (trafficLight->bVertical < 0) ? "X" : (trafficLight->bVertical == 2) ? "_" : "|");
    printf(" %-4d                        \n", trafficLight->rLeftRight->amountOfCars);
    printf("_______             _______  \n");
    printf("       |     | %-4d|         \n", trafficLight->rDownUp->amountOfCars);
    printf("       |     |     |         \n");
    printf("=============================\n");
    return;
}

/* Removes cars from one tick */
void removeCars(road_t *road1, road_t *road2) {
    /* Null time, since it moved */
    road1->waitTime = road2->waitTime = 0;

    road1->amountOfCars -= AMOUNT_OF_CARS_PASS_PER_TICK;
    road2->amountOfCars -= AMOUNT_OF_CARS_PASS_PER_TICK;
    /* Null if negative */
    if(road1->amountOfCars < 0) {
        road1->amountOfCars = 0;
    }
    if(road2->amountOfCars < 0) {
        road2->amountOfCars = 0;
    }
    return;
}

/* Traffic light logic */
void trafficLightLogic(trafficLight_t *trafficLight, int *timer) {
    if(*timer > 0) {
        *timer = *timer - 1;
    } else {
        switch(trafficLight->bVertical) {
            case -2:
                trafficLight->bVertical = 1;
                *timer = 3;
                break;
            case -1:
                trafficLight->bVertical = 2;
                *timer = 3;
                break;
            case 1:
                trafficLight->bVertical = -1;
                break;
            case 2:
                trafficLight->bVertical = -2;
                break;
        }
    }

    return;
}

/* !!! DO FIRST USE THIS FUNCTION WHEN WE HAVE changed TICK TO SECONDS !!!*/
/* Spawn random amount of cars between 0 and 4 */
void spawnCars(road_t *roadToSpawnOn) {
    int randomNumber = 0;

    /* Generate number between 0 and 100 */
    randomNumber = rand() % 100;

    /* 40% chance for 3 car spawn */
    if(randomNumber <= 40) {
        roadToSpawnOn->amountOfCars += 3;
        printf("3 . . .\n");
        return;
    }
    /* 20% chance of 4 car spawn */
    else if(randomNumber > 40 && randomNumber <= 60) {
        roadToSpawnOn->amountOfCars += 4;
        printf("4 . . . .\n");
        return;
    }
    /* 20% chance of 2 car spawn */
    else if(randomNumber > 60 && randomNumber <= 80) {
        roadToSpawnOn->amountOfCars += 2;
        printf("2 . .\n");
        return;
    }
    /* 15% chance of 1 car spawn */
    else if(randomNumber > 80 && randomNumber <= 95) {
        roadToSpawnOn->amountOfCars += 1;
        printf("1 .\n");
        return;
    }
    /* 5% chance of 0 car spawn */
    else if(randomNumber > 95) {
        printf("0\n");
        return;
    } else {
        printf("ERROR! Wrong random number for spawn\n");
        return;
    }
}
