/* build with  https://github.com/jinfagang/Q-Learning/blob/master/main.cpp as an exampel*/

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
#define Gamma = 0.8 /* Horizon */
#define Aplha = 0.1 /* Learning rate */
#define MAX_EPISODE 1000


/*States 4*3*4^4 = 3072 */
#define nTimes 3
/*3[0] Time since last switch: 0: 0-9; 1: 10-179; 2: 180-999+*/
#define nLane1 4
/*4[1] lane 1 Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
#define nLane2 4
/*4[2] lane 2 Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
#define nLane3 4
/*4[3] lane 3 Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
#define nLane4 4
/*4[4] lane 4 Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
#define nActions 4
/*4[5] Ligth direction: 0,1,2,3*/ /*0: Updown; 1 LeftRight; 2 going to UpDown; 3 going to LeftRight;*/
#define nStates 23333 /* states are 233333 */
/*This would give us states like 103112 meaning that we are green up/down we have just changed to this states
  there are alot of cares in lane 1, few cars in lane 2 and 3 and between 4-9 cars in lane 4*/
int i, z, j, k, l, m;

double R[nStates][nActions]; /* Reward tabel*/

double Q[nStates][nActions]; /* Q tabel */

double get_reward(int state, int action) {
  int reward = 0;
  /*3[0] Time since last switch: 0: 0-9; 1: 10-179; 2: 180-999+*/
  switch (state) {
    case 0       ... 999999: /*We have just changed state*/
      if(state % 10 == action) {
        reward += 100;
      } else {
        reward -= 10000;
      }
      break;
    case 1000000 ... 1999999: /* We changed state some time ago*/
      reward += 0;
      break;
    case 2000000 ... 2999999:
      if(state % 10 != action) {
        reward += 100;
      } else {
        reward -= 1000;
      }
      break;
    default:
      printf("Unknown time since last change\n");
      break;
  }
  /*
  int rUpDown;
  int rDownUp;
  int rLeftRight;
  int rRightLeft;

   */
  /*4[1] lane rUpDown Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
  switch (state % 100000) {
    case 0     ... 9999: /*Zero cars wating*/
      reward += 100;
      break;
    case 10000 ... 19999: /* Between 1 and 3 cars waiting */
      switch (action) {
        case 0:
          reward += 30;
          break;
        case 1:
          reward -=10;
          break;
        case 2:
          reward +=15;
          break;
        case 3:
          reward -=15;
          break;
      }
      break;
    case 20000 ... 29999: /* Between 4 and 9 cars waiting */
      switch (action) {
        case 0:
          reward += 35;
          break;
        case 1:
          reward -=15;
          break;
        case 2:
          reward +=7;
          break;
        case 3:
          reward -=20;
          break;
      }
      break;
    case 30000 ... 39999: /* More than 9 cars waiting */
      switch (action) {
        case 0:
          reward += 40;
          break;
        case 1:
          reward -=20;
          break;
        case 2:
          reward +=15;
          break;
        case 3:
          reward -=25;
          break;
      }
      break;
    default:
      printf("Unknown number of cars in rUpDown\n");
      break;
  }

  /*4[2] lane rDownUp Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
  switch (state % 10000) {
    case 0    ... 999: /*Zero cars wating*/
      reward += 100;
      break;
    case 1000 ... 1999: /* Between 1 and 3 cars waiting */
      switch (action) {
        case 0:
          reward += 30;
          break;
        case 1:
          reward -=10;
          break;
        case 2:
          reward +=15;
          break;
        case 3:
          reward -=15;
          break;
      }
      break;
    case 2000 ... 2999: /* Between 4 and 9 cars waiting */
      switch (action) {
        case 0:
          reward += 35;
          break;
        case 1:
          reward -=15;
          break;
        case 2:
          reward +=7;
          break;
        case 3:
          reward -=20;
          break;
      }
      break;
    case 3000 ... 3999: /* More than 9 cars waiting */
      switch (action) {
        case 0:
          reward += 40;
          break;
        case 1:
          reward -=20;
          break;
        case 2:
          reward +=15;
          break;
        case 3:
          reward -=25;
          break;
      }
      break;
    default:
      printf("Unknown number of cars in rDownUp\n");
      break;
  }

  /*4[3] lane rLeftRight Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
  switch (state % 1000) {
    case 0   ... 99: /*Zero cars wating*/
      reward += 100;
      break;
    case 100 ... 199: /* Between 1 and 3 cars waiting */
      switch (action) {
        case 0:
          reward -=10;
          break;
        case 1:
          reward += 30;
          break;
        case 2:
          reward -=15;
          break;
        case 3:
          reward +=15;
          break;
      }
      break;
    case 200 ... 299: /* Between 4 and 9 cars waiting */
      switch (action) {
        case 0:
          reward -=15;
          break;
        case 1:
          reward += 35;
          break;
        case 2:
          reward -=20;
          break;
        case 3:
          reward +=7;
          break;
      }
      break;
    case 300 ... 399: /* More than 9 cars waiting */
      switch (action) {
        case 0:
          reward -=20;
          break;
        case 1:
          reward += 40;
          break;
        case 2:
          reward -=25;
          break;
        case 3:
          reward +=15;
          break;
      }
      break;
    default:
      printf("Unknown number of cars in rLeftRight\n");
      break;
  }

  /*4[4] lane rRightLeft Number of cars: 0: 0-0; 1:1-3; 2: 4-9; 3: 10-999+*/
  switch (state % 100) {
    case 0  ... 9: /*Zero cars wating*/
      reward += 100;
      break;
    case 10 ... 19: /* Between 1 and 3 cars waiting */
      switch (action) {
        case 0:
          reward -=10;
          break;
        case 1:
          reward += 30;
          break;
        case 2:
          reward -=15;
          break;
        case 3:
          reward +=15;
          break;
      }
      break;
    case 20 ... 29: /* Between 4 and 9 cars waiting */
      switch (action) {
        case 0:
          reward -=15;
          break;
        case 1:
          reward += 35;
          break;
        case 2:
          reward -=20;
          break;
        case 3:
          reward +=7;
          break;
      }
      break;
    case 30 ... 39: /* More than 9 cars waiting */
      switch (action) {
        case 0:
          reward -=20;
          break;
        case 1:
          reward += 40;
          break;
        case 2:
          reward -=25;
          break;
        case 3:
          reward +=15;
          break;
      }
      break;
    default:
      printf("Unknown number of cars in rRightLeft\n");
      break;
  }

  /*4[5] Ligth direction: 0,1,2,3*/ /*0: Updown; 1 LeftRight; 2 going to UpDown; 3 going to LeftRight;*/
  switch (state % 10) {
    case 0:
      if(action != 0 || action != 2) {
        reward -= 1000000;
      }
      break;
      case 1:
        if(action != 1 || action != 3) {
          reward -= 1000000;
        }
        break;
      case 2:
        if(action != 2 || action != 0) {
          reward -= 1000000;
        } else if(action == 0) {
          reward += 1000;
        }
        break;
      case 3:
        if(action != 3 || action != 1) {
          reward -= 1000000;
        } else if(action == 1) {
          reward += 1000;
        }
        break;
  }
  return reward;
}

long get_state(trafficLight_t trafficLight) { /* simplyfy state to a number*/
  int cTime;
  int rLeftRight;
  int rRightLeft;
  int rUpDown;
  int rDownUp;
  int tafficDirection;
  int state;

  switch (trafficLight.rLeftRight->amountOfCars) {
    case 0:
      rLeftRight = 0;
      break;
    case 1 ... 3:
      rLeftRight = 1;
      break;
    case 4 ... 9:
      rLeftRight = 2;
    default:
      rLeftRight = 3;
      break;
  }
  switch (trafficLight.rRightLeft->amountOfCars) {
    case 0:
      rRightLeft = 0;
      break;
    case 1 ... 3:
      rRightLeft = 1;
      break;
    case 4 ... 9:
      rRightLeft = 2;
    default:
      rRightLeft = 3;
      break;
  }
  switch (trafficLight.rUpDown->amountOfCars) {
    case 0:
      rUpDown = 0;
      break;
    case 1 ... 3:
      rUpDown = 1;
      break;
    case 4 ... 9:
      rUpDown = 2;
    default:
      rUpDown = 3;
      break;
  }
  switch (trafficLight.rDownUp->amountOfCars) {
    case 0:
      rDownUp = 0;
      break;
    case 1 ... 3:
      rDownUp = 1;
      break;
    case 4 ... 9:
      rDownUp = 2;
    default:
      rDownUp = 3;
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
  switch (trafficLight.bVertical) {
    case 1: /*UpDown*/
      tafficDirection = 0;
      break;
    case 2: /* LeftRight*/
      tafficDirection = 1;
      break;
    case -1: /*Yellow Going to UpDown*/
      tafficDirection = 2;
      break;
    case -2: /*Yellow  Going to LeftRight*/
      tafficDirection = 3;
      break;

  }
  /*Move numbers*/
  cTime       *= 100000;
  rUpDown     *= 10000;
  rDownUp     *= 1000;
  rLeftRight  *= 100;
  rRightLeft  *= 10;
  tafficDirection;
  state = cTime + rUpDown + rDownUp + rLeftRight + rRightLeft + tafficDirection;
  /*nTime nLane1 nLane2 nLane3 nLane4 */
  return state;
}

int possible_action[nActions];
int possible_action_num;

/* get in state s, possible actions */
void get_possible_action(double R[nStates][nActions], int state, int possible_action[nActions]){
    /* find R[i][j] value > 0 is action we can do later */
    possible_action_num = 0;
    for(i = 0; i < nActions; i++){
        if (R[state][i] >= 0){
            possible_action[possible_action_num] = i;
            possible_action_num++;
        }
    }
}

double get_max_q(double Q[nStates][nActions], int state) {
  double temp_max = 0;
  for (i = 0; i < nActions; ++i) {
    if ((R[state][i] >= 0) && (Q[state][i] > temp_max)){
      temp_max = Q[state][i];
    }
  }
  return temp_max;
}

int episode_iterator(int init_state, double Q[nStates][nActions], double R[nStates][nActions]){

    double Q_before, Q_after;
    /* next action */
    int next_action;
    double max_q;
    float alpha = 0.8;

    /* start series event loop */
    int step=0;
    printf("-- step %d : initial state: \n", step, init_state);
    /* memset possible_action array */
    memset(possible_action, 0, nActions*sizeof(int));
    get_possible_action(R, init_state, possible_action);

    /* get next action */
    next_action = possible_action[rand()%possible_action_num];
    printf("-- step %d : next action: \n", step, next_action);
    /* treat next action as state, and we can get max{Q(s', a')} */
    max_q = get_max_q(Q, next_action);

    Q_before = Q[init_state][next_action];
    /* update formula Q(s,a)=R(s,a)+alpha * max{Q(s', a')} */
    Q[init_state][next_action] = R[init_state][next_action] + alpha * max_q;
    Q_after = Q[init_state][next_action];

    /* next episode rules
    // if next_action is destination state, then go next episode
    // if not, then go back to this loop*/
    /*if (next_action == DES_STATE){
        init_state = rand()%STATE_NUM;
        break;
    }else{*/
        /* if not destination state, then next action becomes initial state*/
    init_state = next_action;
    /*}*/
    return init_state;
}

int inference_best_action(int now_state, double Q[nStates][nActions]){
    /* get the max value of Q corresponding action when state is nw_state*/
    double temp_max_q=0;
    int best_action=0;
    int i;
    for (i = 0; i < nActions; ++i) {
        if (Q[now_state][i] > temp_max_q){
            temp_max_q = Q[now_state][i];
            best_action = i;
        }
    }
    return best_action;
}

void print_matrix(double m[nStates][nActions]) {
    int i, j;
    for (i = 0; i < nStates; ++i) {
        for (j = 0; j < nActions; ++j) {
            printf("%d\t", m[i][j]);
        }
        printf("\n");
    }
}


void run_training(int init_state) {
    int initial_state = init_state;
    int count = 0;
    int i;
    /* start random */
    srand((unsigned)time(NULL));
    printf("%d %s\n", count, "[INFO] start training...");
    for (i = 0; i < MAX_EPISODE; ++i) {
        printf("%d %s %d\n", count, "[INFO] Episode: ", i);
        initial_state = episode_iterator(initial_state, Q, R);
        printf("%d %s %d\n", count, "-- updated Q matrix: ");
        print_matrix(Q);
        count++;
    }
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

    /* Fill traffic light & initialize direction */
    fillTrafficLight(&trafficLight, &rLeftRight, &rRightLeft, &rUpDown, &rDownUp);
    trafficLight.bVertical = 1;

    /*TODO move this to a better place*/
    for (i = 0; i < nStates; i++) {
      /* code */
      memset(R[i], 0, nActions*sizeof(double));
      memset(Q[i], 0, nActions*sizeof(double));
    }


    run_training(get_state(trafficLight));

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
