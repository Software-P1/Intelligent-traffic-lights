#include <stdio.h>
#include <stdlib.h>

/* General simulation defines */
#define SEED 11234559
#define CAR_DATA_FILE "carData.csv"
#define PRINT_MSG 1 /* 1 == print when spawn and remove */
#define GREEN_TIME 45
#define YELLOW_TIME 5

/* Struct pr. road placement */
typedef struct road_s {
    int amountOfCars;
    int waitTime;
    int waitTimeAccumulated;
    int totalCars;
    int totalWait;
} road_t;

/* Struct for the traffic light */
typedef struct trafficLight_s {
    int lightDir;
    int totalCars;
    int totalWait;
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
void spawnCars(trafficLight_t *trafficLight, double spawnFactor);
int randomIndividualSpawn(road_t *road, double chance);
void printFirstCsvLine(FILE *dataFile);
void printCsv(trafficLight_t *trafficLight, int tick, FILE *file);
void waitTimeLogic(trafficLight_t *trafficLight);
void accumilateWaitTime (road_t *zRoad1, road_t *zRoad2, road_t *aRoad3, road_t *aRoad4, int bNull);

/* The main function */
int main(void) {
    int desiredTicks = 0, i = 0, runs = 0, bDraw = 0, timer = 0,
        timeUntilRemove = 0, removeCounter = 0;
    double spawnFactor = 1.5;
    road_t rLeftRight, rRightLeft, rUpDown, rDownUp;
    trafficLight_t trafficLight;
    /* Create and open file for data saving */
    FILE *dataFile;
    dataFile = fopen(CAR_DATA_FILE, "w");
    printFirstCsvLine(dataFile);

    /* Seed the randomization */
    srand(SEED);

    /* Fill traffic light & initialize direction and time until remove */
    fillTrafficLight(&trafficLight, &rLeftRight, &rRightLeft, &rUpDown, &rDownUp);
    trafficLight.lightDir = 1;
    timeUntilRemove = 6;
    timer = GREEN_TIME;

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
        /* Spawn cars on each road & run trafficLightLogic */
        spawnCars(&trafficLight, spawnFactor);
        trafficLightLogic(&trafficLight, &timer);

        /* Incriment counter, and check if this is the time we remove cars */
        removeCounter++;
        if (removeCounter >= timeUntilRemove) {
            removeCars(&trafficLight, &timeUntilRemove);
            removeCounter = 0;
        }

        /* Print data into CSV file*/
        waitTimeLogic(&trafficLight);
        printCsv(&trafficLight, i, dataFile);

        /* If visualization is desired, then print */
        if(bDraw) {
            printVisualization(&trafficLight, i);
        }
        runs++;
    }
    printf("[Total runs: %d]\n", runs);
    fclose(dataFile);
    return 0;
}

/* Fill the traffic light with information */
void fillTrafficLight(trafficLight_t *trafficLightToBeFilled, road_t *rLeftRight,
                      road_t *rRightLeft, road_t *rUpDown, road_t *rDownUp) {
    int i = 0;
    /* Create array of all roads to use in for-loop */
    road_t *roads[4];
    roads[0] = rLeftRight;
    roads[1] = rRightLeft;
    roads[2] = rUpDown;
    roads[3] = rDownUp;

    /* Initialize trafficLight variables */
    trafficLightToBeFilled->rLeftRight = rLeftRight;
    trafficLightToBeFilled->rRightLeft = rRightLeft;
    trafficLightToBeFilled->rUpDown = rUpDown;
    trafficLightToBeFilled->rDownUp = rDownUp;
    trafficLightToBeFilled->totalCars = 0;
    trafficLightToBeFilled->totalWait = 0;

    /* Initialize road values */
    for (i = 0; i < 4; i++) {
         roads[i]->amountOfCars = 0;
         roads[i]->waitTime = 0;
         roads[i]->totalWait = 0;
         roads[i]->totalCars = 0;
         roads[i]->waitTimeAccumulated = 0;
    }
    return;
}

/* Print first data line of the csv */
void printFirstCsvLine(FILE *dataFile) {
    /* Write description of each value into first line of carData file */
    fprintf(dataFile,"%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n"
    , "Tick"
    , "Amount rUpDown"
    , "Amount rDownUp"
    , "Amount rRightLeft"
    , "Amount rLeftRight"
    , "Total wait (Total)"
    , "Total wait (rUpDown)"
    , "Total wait (rDownUp)"
    , "Total wait (rRightLeft)"
    , "Total wait (rLeftRight)"
    , "Total cars (Total)"
    , "Total cars (rUpDown)"
    , "Total cars (rDownUp)"
    , "Total cars (rRightLeft)"
    , "Total cars (rLeftRight)"
    , "Average wait time"
    , "Average cars pr. road");
    return;
}

/* Creates a .csv that saves how many cars are in each direction and total wait time */
void printCsv(trafficLight_t *trafficLight, int tick, FILE *file) {
    fprintf(file,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f;%f\n"
            , tick
            , trafficLight->rUpDown->amountOfCars
            , trafficLight->rDownUp->amountOfCars
            , trafficLight->rRightLeft->amountOfCars
            , trafficLight->rLeftRight->amountOfCars
            , trafficLight->totalWait
            , trafficLight->rUpDown->totalWait
            , trafficLight->rDownUp->totalWait
            , trafficLight->rRightLeft->totalWait
            , trafficLight->rLeftRight->totalWait
            , trafficLight->totalCars
            , trafficLight->rUpDown->totalCars
            , trafficLight->rDownUp->totalCars
            , trafficLight->rRightLeft->totalCars
            , trafficLight->rLeftRight->totalCars
            , (trafficLight->totalWait > 0) ? ((float)(trafficLight->totalWait) / (float)(trafficLight->totalCars)) : 0
            , (float)(trafficLight->totalCars) / 4);
    return;
}

/* Print a visualization of the road */
void printVisualization(trafficLight_t *trafficLight, int i) {
    printf("========= TICK %-4d =========\n", i + 1);
    printf("       |     |     |         \n");
    printf("_______| %-4d|     |_______  \n", trafficLight->rUpDown->amountOfCars);
    printf("             %s       %-4d    \n",(trafficLight->lightDir == 1) ? "^" : " ", trafficLight->rRightLeft->amountOfCars);
    printf("-------     %s     ------- \n", (trafficLight->lightDir < 0) ? " X " : (trafficLight->lightDir == 2) ? "<->" : " | ");
    printf(" %-4d        %s                \n", trafficLight->rLeftRight->amountOfCars, (trafficLight->lightDir == 1) ? "v" : " ");
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
    if (PRINT_MSG == 1) {
        if (trafficLight->lightDir == 1 || trafficLight->lightDir == 2) {
            printf("[Cars removed on %s direction]\n", (trafficLight->lightDir == 1) ? "vertical" : "horisontal" );
        }
    }

    /* Remove cars based on lightDir */
    if (trafficLight->lightDir == 1) {
        removeCarsIndividual(&trafficLight->rUpDown->amountOfCars, &trafficLight->rDownUp->amountOfCars);
    } else if (trafficLight->lightDir == 2) {
        removeCarsIndividual(&trafficLight->rRightLeft->amountOfCars, &trafficLight->rLeftRight->amountOfCars);
    } else if (trafficLight->lightDir == -1 || trafficLight->lightDir == -2) {
        /* Do nothing if yellow */
    } else {
        printf("ERROR! Wrong lightDir value in removeCars\n");
    }

    /* Change when to remove cars next, based on chances defined based on our collected data  */
    randomNumber = rand() % 100;

    /* 5% chance of 8 sec */
    if (randomNumber <= 5) {
        *timeUntilRemove = 8;
    }
    /* 15% chance of 7 sec */
    else if (randomNumber <= 20 && randomNumber > 5) {
        *timeUntilRemove = 7;
    }
    /* 40% chance of 6 sec */
    else if (randomNumber <= 60 && randomNumber > 20) {
        *timeUntilRemove = 6;
    }
    /* 15% chance of 5 sec */
    else if (randomNumber <= 75 && randomNumber > 60) {
        *timeUntilRemove = 5;
    }
    /* 10% chance of 4 sec */
    else if (randomNumber <= 85 && randomNumber > 75) {
        *timeUntilRemove = 4;
    }
    /* 10% chance of 3 sec */
    else if (randomNumber <= 95 && randomNumber > 85) {
        *timeUntilRemove = 3;
    }
    /* 5% chance of 2 sec*/
    else if (randomNumber <= 100 && randomNumber > 95) {
        *timeUntilRemove = 2;
    } else {
        printf("ERROR in remove cars\n");
    }
    return;
}

/* Removes cars on two roads if they are above 1 car */
void removeCarsIndividual(int *amountRoad1, int *amountRoad2) {
    if (*amountRoad1 > 0) {
        *amountRoad1 -= 1;
    }
    if (*amountRoad2 > 0) {
        *amountRoad2 -= 1;
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
        case -2: /* Yellow from horisontal direction */
            trafficLight->lightDir = 1;
            *timer = GREEN_TIME;
            break;
        case -1: /* Yellow from vertical direction */
            trafficLight->lightDir = 2;
            *timer = GREEN_TIME;
            break;
        case 1: /* Vertical direction */
            trafficLight->lightDir = -1;
            *timer = YELLOW_TIME;
            break;
        case 2: /* Horisontal direction */
            trafficLight->lightDir = -2;
            *timer = YELLOW_TIME;
            break;
        default:
            printf("ERROR! Wrong lightDir value in trafficLightLogic\n");
            break;
        }
    }
    return;
}

/* Called each tick, spawns a car based on chances from our data*/
/* Tries to spawn a car on each road, based on gathered data * spawnFactor */
void spawnCars(trafficLight_t *trafficLight, double spawnFactor) {
    /* Error mssage if it will not spawn cars */
    if (spawnFactor <= 0) {
        printf("ERROR! spawnFactor is less than or equal to 0 == no car spawn\n");
        return;
    }

    /* 3 % chance of spawn on rLeftRight & rRightLeft */
    if (randomIndividualSpawn(trafficLight->rLeftRight, (30 * spawnFactor)) && PRINT_MSG == 1) {
        printf("[Car spawn rLeftRight]\n");
        trafficLight->totalCars += 1;
    }
    if (randomIndividualSpawn(trafficLight->rRightLeft, (30 * spawnFactor)) && PRINT_MSG == 1) {
        printf("[Car spawn rRightLeft]\n");
        trafficLight->totalCars += 1;
    }

    /* 4,5 % chance of spawn on rUpDown & rDownUp */
    if (randomIndividualSpawn(trafficLight->rUpDown, (45 * spawnFactor)) && PRINT_MSG == 1) {
        printf("[Car spawn rUpDown]\n");
        trafficLight->totalCars += 1;
    }
    if (randomIndividualSpawn(trafficLight->rDownUp, (45 * spawnFactor)) && PRINT_MSG == 1) {
        printf("[Car spawn rDownUp]\n");
        trafficLight->totalCars += 1;
    }
    return;
}

/* Spawn car on individual road, based on chance (100 and 1 decimal in whole numbers) */
/* Returns 1 if sucsessfull and 0 if not*/
int randomIndividualSpawn(road_t *road, double chance) {
    double randomNumber = 0;
    randomNumber = (double)(rand() % 1000);

    /* Error correct chance */
    if (chance < 0) {
        chance = 0;
        printf("ERROR! Negative chance has been error corrected to 0!\n");
    }

    /* If the chance is less then add a car */
    if (randomNumber <= chance) {
        road->amountOfCars += 1;
        road->totalCars += 1;
        return 1;
    }
    return 0;
}

/* Logic for the waitTime */
void waitTimeLogic(trafficLight_t *trafficLight) {
    /* waitTime logic for vertical direction */
    if (trafficLight->lightDir == 1) {
        accumilateWaitTime(trafficLight->rUpDown, trafficLight->rDownUp, trafficLight->rLeftRight, trafficLight->rRightLeft, 0);
    }
    /* waitTime logic for horisontal direction */
    else if (trafficLight->lightDir == 2) {
        accumilateWaitTime(trafficLight->rLeftRight, trafficLight->rRightLeft, trafficLight->rUpDown, trafficLight->rDownUp, 0);
    }
    /* Yellow lights (only nulls everything) */
    else if (trafficLight->lightDir == -1 || trafficLight->lightDir == -2) {
        accumilateWaitTime(trafficLight->rUpDown, trafficLight->rDownUp, trafficLight->rLeftRight, trafficLight->rRightLeft, 1);
        accumilateWaitTime(trafficLight->rLeftRight, trafficLight->rRightLeft, trafficLight->rUpDown, trafficLight->rDownUp, 1);
    } else {
        printf("ERROR! Wrong lightDir in waitTimeLogic!\n");
    }

    /* Accumilated wait time for all lanes*/
    trafficLight->totalWait += trafficLight->rUpDown->amountOfCars
                            + trafficLight->rDownUp->amountOfCars
                            + trafficLight->rRightLeft->amountOfCars
                            + trafficLight->rLeftRight->amountOfCars;
    return;
}

/* Accumilate individual waitTime */
/* z = zero | a = Accumilate | road = direction*/
void accumilateWaitTime (road_t *zRoad1, road_t *zRoad2, road_t *aRoad3, road_t *aRoad4, int bNull) {
    if (bNull != 1) {
        /* Set current direction variables to 0 */
        zRoad1->waitTime = 0;
        zRoad2->waitTime = 0;
        zRoad1->waitTimeAccumulated = 0;
        zRoad2->waitTimeAccumulated = 0;
    }

    /* Accumilate wait time variables for other direction */
    aRoad3->waitTime += aRoad3->amountOfCars;
    aRoad4->waitTime += aRoad4->amountOfCars;
    aRoad3->waitTimeAccumulated += 1;
    aRoad4->waitTimeAccumulated += 1;
    aRoad3->totalWait += 1;
    aRoad4->totalWait += 1;
    return;
}
