#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>



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
    int timer;
} trafficLight_t;


/********************************************************************************************/
/***********************************Reinforcement learning***********************************/
/********************************************************************************************/
/********************************************************************************************/
const float Gamma = 0.8; /* Horizon */
const float Aplha = 0.1; /* Learning rate */


/*States 4*3*4^4 = 3072 */
#define nActions 4
/*4[0] Current ligth direction: 0,1,2,3*/
#define nTimes 3
/*3[1] Time since last switch: 0: 0-9; 1: 10-179; 2: 180-999+*/
#define nLane1 4
/*4[2] lane 1 Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
#define nLane2 4
/*4[3] lane 2 Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
#define nLane3 4
/*4[4] lane 3 Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
#define nLane4 4
/*4[5] lane 4 Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/

/*This would give us states like 103112 meaning that we are green up/down we have just changed to this states
  there are alot of cares in lane 1, few cars in lane 2 and 3 and between 4-9 cars in lane 4*/
int i, z, j, k, l, m;
double R[nActions][nTimes][nLane1][nLane2][nLane3][nLane4]; /* Reward tabel*/

double Q[nActions][nTimes][nLane1][nLane2][nLane3][nLane4]; /* Q tabel */

double get_state(trafficLight_t trafficLight) {

}



int possible_action[nActions];
int possible_action_num;

/* get in state s, possible actions */
void get_possible_action(double R[nActions][nTimes][nLane1][nLane2][nLane3][nLane4], trafficLight_t trafficLight, int possible_action[nActions]){
    int cTime;
    int lane1;
    int lane2;
    int lane3;
    int lane4;

    switch (trafficLight.rLeftRight->amountOfCars) {
      case 0:
        lane1 = 0;
        break;
      case 1 ... 3:
        lane1 = 1;
        break;
      case 4 ... 9:
        lane1 = 2;
      default:
        lane1 = 3;
        break;
    }
    switch (trafficLight.rRightLeft->amountOfCars) {
      case 0:
        lane2 = 0;
        break;
      case 1 ... 3:
        lane2 = 1;
        break;
      case 4 ... 9:
        lane2 = 2;
      default:
        lane2 = 3;
        break;
    }
    switch (trafficLight.rUpDown->amountOfCars) {
      case 0:
        lane3 = 0;
        break;
      case 1 ... 3:
        lane3 = 1;
        break;
      case 4 ... 9:
        lane3 = 2;
      default:
        lane3 = 3;
        break;
    }
    switch (trafficLight.rDownUp->amountOfCars) {
      case 0:
        lane4 = 0;
        break;
      case 1 ... 3:
        lane4 = 1;
        break;
      case 4 ... 9:
        lane4 = 2;
      default:
        lane4 = 3;
        break;
    }
    switch (trafficLight.timer) {
      case 0 ... 9:
        cTime = 0;
        break;
      case 10 ... 179:
        cTime = 1;
        break;
      default:
        cTime = 2;
        break;
    }

    /* find R[i][j] value > 0 is action we can do later */
    possible_action_num = 0;
    for(i = 0; i < nActions; i++){
        if (R[i][cTime][lane1][lane2][lane3][lane4] >= 0){
            possible_action[possible_action_num] = i;
            possible_action_num++;
        }
    }
}

double get_max_q(double Q[nActions][nTimes][nLane1][nLane2][nLane3][nLane4], trafficLight_t trafficLight) {
  int cTime;
  int lane1;
  int lane2;
  int lane3;
  int lane4;
  int i;
  double temp_max = 0;

  switch (trafficLight.rLeftRight->amountOfCars) {
    case 0:
      lane1 = 0;
      break;
    case 1 ... 3:
      lane1 = 1;
      break;
    case 4 ... 9:
      lane1 = 2;
    default:
      lane1 = 3;
      break;
  }
  switch (trafficLight.rRightLeft->amountOfCars) {
    case 0:
      lane2 = 0;
      break;
    case 1 ... 3:
      lane2 = 1;
      break;
    case 4 ... 9:
      lane2 = 2;
    default:
      lane2 = 3;
      break;
  }
  switch (trafficLight.rUpDown->amountOfCars) {
    case 0:
      lane3 = 0;
      break;
    case 1 ... 3:
      lane3 = 1;
      break;
    case 4 ... 9:
      lane3 = 2;
    default:
      lane3 = 3;
      break;
  }
  switch (trafficLight.rDownUp->amountOfCars) {
    case 0:
      lane4 = 0;
      break;
    case 1 ... 3:
      lane4 = 1;
      break;
    case 4 ... 9:
      lane4 = 2;
    default:
      lane4 = 3;
      break;
  }
  switch (trafficLight.timer) {
    case 0 ... 9:
      cTime = 0;
      break;
    case 10 ... 179:
      cTime = 1;
      break;
    default:
      cTime = 2;
      break;
  }

  for (i = 0; i < nActions; ++i) {
    if ((R[i][cTime][lane1][lane2][lane3][lane4] >= 0) && (Q[i][cTime][lane1][lane2][lane3][lane4] > temp_max)){
      temp_max = Q[i][cTime][lane1][lane2][lane3][lane4];
    }
  }
  return temp_max;
}

int episode_iterator(trafficLight_t trafficLight, double Q[nActions][nTimes][nLane1][nLane2][nLane3][nLane4], double R[nActions][nTimes][nLane1][nLane2][nLane3][nLane4]){

    double Q_before, Q_after;
    /* next action */
    int next_action;
    double max_q;

    /* start series event loop */
    int step=0;
    while(1) {
        printf("-- step %d : initial state: \n", step, trafficLight);
        /* memset possible_action array */
        memset(possible_action, 0, nActions*sizeof(int));
        get_possible_action(R, trafficLight, possible_action);

        /* get next action */
        next_action = possible_action[rand()%possible_action_num];
        printf("-- step %d : next action: \n", step, next_action);
        /* treat next action as state, and we can get max{Q(s', a')} */
        max_q = get_max_q(Q, next_action);

        Q_before = Q[init_state][next_action];
        // update formula Q(s,a)=R(s,a)+alpha * max{Q(s', a')}
        Q[init_state][next_action] = R[init_state][next_action] + alpha * max_q;
        Q_after = Q[init_state][next_action];

        // next episode rules
        // if next_action is destination state, then go next episode
        // if not, then go back to this loop
        if (next_action == DES_STATE){
            init_state = rand()%STATE_NUM;
            break;
        }else{
            // if not destination state, then next action becomes initial state
            init_state = next_action;
        }
        step++;
    }
    return init_state;
}

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/



/* Top-down programming declaration of functions */
void fillTrafficLight(trafficLight_t *trafficLightToBeFilled, road_t *rLeftRight,
                      road_t *rRightLeft, road_t *rUpDown, road_t *rDownUp);
void legacySpawnCars(road_t *roadTolegacySpawnCarsOn);
void printVisualization(trafficLight_t *trafficLight, int i);
void removeCars(road_t *road1, road_t *road2);
void trafficLightLogic(trafficLight_t *trafficLight, int *timer);
void spawnCars(road_t *roadToSpawnOn);
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

      /*TODO move this to a better place*/
    for(z = 0; z < nActions; z++) {
      for(j = 0; j < nTimes; j++) {
        for(k = 0; k < nLane1; k++) {
          for(l = 0; l < nLane2; l++) {
            for(m = 0; m < nLane3; m++) {
              memset(R[z][j][k][l][m], 0, nActions*sizeof(double));
              memset(Q[z][j][k][l][m], 0, nActions*sizeof(double));
            }
          }
        }
      }
    }







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

        /* Reinfocement learning traffic light logic */

        /* Remove cars this tick */
        switch(trafficLight.bVertical) {
            case 1:
                removeCars(trafficLight.rUpDown, trafficLight.rDownUp);
                break;
            case 2:
                removeCars(trafficLight.rRightLeft, trafficLight.rLeftRight);
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
    int lastState = trafficLight->bVertical;
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

    if(lastState == trafficLight->bVertical) {
      trafficLight->timer++;
    } else {
      trafficLight->timer = 0;
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
