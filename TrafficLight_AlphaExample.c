#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* General simulation defines */
#define SEED 10
#define AMOUNT_OF_CARS_PASS_PER_TICK 3 /* !!! REMOVE WHEN WE CHANGE TO SECONDS !!! */
#define PERCENT_3_CAR_SPAWN 1 /* !!! REMOVE WHEN WE CHANGE TO SECONDS !!! */
#define PERCENT_2_CAR_SPAWN 2 /* !!! REMOVE WHEN WE CHANGE TO SECONDS !!! */
#define PERCENT_1_CAR_SPAWN 3 /* !!! REMOVE WHEN WE CHANGE TO SECONDS !!! */
#define CAR_DATA_FILE "carData.csv"

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
void printVisualization(trafficLight_t *trafficLight, int i);
void removeCars(road_t *road1, road_t *road2);
void trafficLightLogic(trafficLight_t *trafficLight, int *timer);
void spawnCars(trafficLight_t *trafficLight);
void printCsv(int rUpDown, int rDownUp,int rRightLeft, int rLeftRight, int totalWait);


/* The main function */
int main(void) {
    int desiredTicks = 0, i = 0, runs = 0, bDraw = 0;
    road_t rLeftRight, rRightLeft, rUpDown, rDownUp;
    trafficLight_t trafficLight;
    int timer = 0;
    int totalWait = 0;

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
        spawnCars(&trafficLight);


        /* Run the traffic light logic */
        trafficLightLogic(&trafficLight, &timer);

        /* Remove cars this tick */
        switch(trafficLight.bVertical) {
        case 1:
            /*removeCars(trafficLight.rUpDown, trafficLight.rDownUp);*/
            break;
        case 2:
            /*removeCars(trafficLight.rRightLeft, trafficLight.rLeftRight);*/
            break;
        }
        /* Updates wait timer*/
        trafficLight.rUpDown->waitTime += trafficLight.rUpDown->amountOfCars;
        trafficLight.rDownUp->waitTime += trafficLight.rDownUp->amountOfCars;
        trafficLight.rRightLeft->waitTime += trafficLight.rRightLeft->amountOfCars;
        trafficLight.rLeftRight->waitTime += trafficLight.rLeftRight->amountOfCars;
        /* Accumilated wait time for all lanes*/
        totalWait += trafficLight.rUpDown->amountOfCars
                  + trafficLight.rDownUp->amountOfCars
                  + trafficLight.rRightLeft->amountOfCars
                  + trafficLight.rLeftRight->amountOfCars;

        /* Prints wait time for lanes to CSV file*/
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
    /* If timer is positive, then reduce it by one... */
    /* ...else look at the bVertical value */
    if (*timer > 0) {
        *timer = *timer -1 ;
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
        default:
            printf("ERROR! Wrong bVertical value in trafficLightLogic\n");
            break;
        }
    }
    return;
}

/* !!! DO FIRST USE THIS FUNCTION WHEN WE HAVE changed TICK TO SECONDS !!!*/
/* Spawn random amount of cars between 0 and 4 */
void spawnCars(trafficLight_t *trafficLight) {
    int randomNumber = 0;

    /* Generate number between 0 and 100000 (100% with 2 decimals in intiger form) */
    randomNumber = rand() % 100;

    /*25,88 chance for spawning a car*/
    if (randomNumber <= 25) {
        /* Generate number between 0 and 100000 (100% with 3 decimals in intiger form) */
        randomNumber = rand() % 100;

        /*35,47% chance for vertical = ~18% pr road*/
        if (randomNumber <= 18) {
            trafficLight->rLeftRight->amountOfCars += 1;
        } else if (randomNumber <= 36 && randomNumber > 18) {
            trafficLight->rRightLeft->amountOfCars += 1;
        }
        /*64,53% chance for horisontal = ~32% pr road*/
        else if (randomNumber <= 68 && randomNumber > 36) {
            trafficLight->rUpDown->amountOfCars += 1;
        } else if (randomNumber > 68 && randomNumber <= 100) {
            trafficLight->rDownUp->amountOfCars += 1;
        } else {
            printf("ERROR! Wrong random number in spawn function\n");
        }
    }
    return;
}
