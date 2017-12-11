/* Information about neural network primarily learned from the youtube */
/* educational video series made by "3Blue1Brown" */
/* youtube.com/watch?v=aircAruvnKk&list=PLZHQObOWTQDNU6R1_67000Dx_ZCJB-3pi */
/* And this video: youtube.com/watch?v=ILsA4nyG7I0 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* General simulation defines */
#define SEED 10
#define AMOUNT_OF_CARS_PASS_PER_TICK 3
#define PERCENT_3_CAR_SPAWN 1
#define PERCENT_2_CAR_SPAWN 2
#define PERCENT_1_CAR_SPAWN 3

/* Neural network defines */
#define MAX_NEURON_IN_LAYER 5
#define NUM_NEURON_LAYER_START 5
#define NUM_NEURON_LAYER_1 4
#define NUM_NEURON_LAYER_END 2
#define EULER 2.71828182845904523536028747135266249775724709369995

/* Struct pr. road placement */
typedef struct road_s{
    int amountOfCars;
    int waitTime;
} road_t;

/* Struct for the traffic light */
typedef struct trafficLight_s{
    int bVertical;
    road_t* rLeftRight;
    road_t* rRightLeft;
    road_t* rUpDown;
    road_t* rDownUp;
} trafficLight_t;

/* Struct of an individual */
typedef struct neuron_s{
    char neuronID[3];
    double weightToNext[MAX_NEURON_IN_LAYER];
    double inputValue; /* For use in first row */
} neuron_t;

/* The neural network struct */
typedef struct neuralNetwork_s{
    neuron_t startLayer[NUM_NEURON_LAYER_START];
    neuron_t firstLayer[NUM_NEURON_LAYER_1];
    neuron_t endLayer[NUM_NEURON_LAYER_END];
} neuralNetwork_t;

/* Top-down programming declaration of functions */
void fillTrafficLight(trafficLight_t* trafficLightToBeFilled, road_t* rLeftRight,
                      road_t* rRightLeft, road_t* rUpDown, road_t* rDownUp);
void spawnCars(road_t* roadToSpawnCarsOn);
void printVisualization(trafficLight_t* trafficLight, int i);
void removeCars(road_t* road1, road_t* road2);
void trafficLightLogic(trafficLight_t* trafficLight);
void fillNeuralNetwork(neuralNetwork_t* neuralNetworkToBeFilled, int bFromFile);
double randomDecimal();
double sigmoid(double x);

/* The main function */
int main(void) {
    int desiredTicks = 0, i = 0, runs = 0, bDraw = 0;
    road_t rLeftRight, rRightLeft, rUpDown, rDownUp;
    trafficLight_t trafficLight;
    neuralNetwork_t theNeuralNetwork;

    /* Seed randomization */
    srand(SEED);

    /* Fill traffic light & initialize direction */
    fillTrafficLight(&trafficLight, &rLeftRight, &rRightLeft, &rUpDown, &rDownUp);
    trafficLight.bVertical = 0;

    /* Fill neural the neural network */
    fillNeuralNetwork(&theNeuralNetwork, 0);

    /* Ask user for input */
    do{
        printf("Please enter amount of ticks to run: ");
        scanf("%d", &desiredTicks);
    } while (desiredTicks < 0);
    printf("===============\n");
    printf("Do you want the program to visualize each tick?\n");
    do{
        printf("Enter bool value: ");
        scanf("%d", &bDraw);
    } while (bDraw > 1 || bDraw < 0);

    /* The loop for each tick */
    for (i = 0; i < desiredTicks; i++) {
        /* Spawn cars on each road */
        spawnCars(&rUpDown);
        spawnCars(&rRightLeft);
        spawnCars(&rLeftRight);
        spawnCars(&rDownUp);

        /* Run the traffic light logic */
        trafficLightLogic(&trafficLight);

        /* Remove cars this tick */
        if (trafficLight.bVertical == 1){
            removeCars(trafficLight.rRightLeft, trafficLight.rLeftRight);
            trafficLight.rUpDown->waitTime++;
            trafficLight.rDownUp->waitTime++;
        } else if (trafficLight.bVertical == 0) {
            removeCars(trafficLight.rUpDown, trafficLight.rDownUp);
            trafficLight.rRightLeft->waitTime++;
            trafficLight.rLeftRight->waitTime++;
        } else {
            printf("ERROR - bVertical is a wrong value!!!\n");
        }

        /* If visualization is desired, then print */
        if(bDraw){
            printVisualization(&trafficLight, i);
        }
        runs++;
    }
    printf("[Total runs: %d]\n", runs);
    return 0;
}

/* Fill the traffic light with information */
void fillTrafficLight(trafficLight_t* trafficLightToBeFilled, road_t* rLeftRight,
                      road_t* rRightLeft, road_t* rUpDown, road_t* rDownUp){
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
void spawnCars(road_t* roadToSpawnCarsOn){
    int randomNumber = 0;

    /* Get random number */
    randomNumber = rand() % 100;

    /* The 3% += PERCENT_1_CAR_SPAWN */
    if (randomNumber >= 0 && randomNumber <= 3){
        roadToSpawnCarsOn->amountOfCars += PERCENT_1_CAR_SPAWN;
    }
    /* The 2% += PERCENT_2_CAR_SPAWN */
    else if (randomNumber >= 4 && randomNumber <= 6){
        roadToSpawnCarsOn->amountOfCars += PERCENT_2_CAR_SPAWN;
    }
    /* The 1% += PERCENT_3_CAR_SPAWN */
    else if (randomNumber == 7){
        roadToSpawnCarsOn->amountOfCars += PERCENT_3_CAR_SPAWN;
    }
    return;
}

/* Print a visualization of the road */
void printVisualization(trafficLight_t* trafficLight, int i){
    printf("========== TICK %d ==========\n", i);
    printf("   |     |     |       \n");
    printf("___| %-4d|     |_______\n", trafficLight->rUpDown->amountOfCars);
    printf("                 %-4d  \n", trafficLight->rRightLeft->amountOfCars);
    printf("---      %s      ------\n", (trafficLight->bVertical == 1) ? "_" : "|");
    printf(" %-4d                  \n", trafficLight->rLeftRight->amountOfCars);
    printf("___             _______\n");
    printf("   |     | %-4d |      \n", trafficLight->rDownUp->amountOfCars);
    printf("   |     |      |      \n");
    printf("==============================\n");
    return;
}

/* Removes cars from one tick */
void removeCars(road_t* road1, road_t* road2){
    /* Null time, since it moved */
    road1->waitTime = road2->waitTime = 0;

    road1->amountOfCars -= AMOUNT_OF_CARS_PASS_PER_TICK;
    road2->amountOfCars -= AMOUNT_OF_CARS_PASS_PER_TICK;
    /* Null if negative */
    if (road1->amountOfCars < 0){
        road1->amountOfCars = 0;
    }
    if (road2->amountOfCars < 0){
        road2->amountOfCars = 0;
    }
    return;
}

/* Traffic light logic */
void trafficLightLogic(trafficLight_t* trafficLight){
    /*
    THINGS TO LOOK INTO:
    Maybe having weights in each direction, removing less and less each time?
    "Self organized maps"
    "Gradiant decend"
    "Cost function"
    "Neural network"
    Having different times different traffic loads?
    Maybe having ozolation?
    print a .csv file with outcome?
    "Back probagation with gradiant decend"
    "Generic algoritims?"
    Most likely "ANN" - find youtube video from "3blue1brown"
    */
    return;
}

/* Function to fill the neural network - either from file or random */
void fillNeuralNetwork(neuralNetwork_t* neuralNetworkToBeFilled, int bFromFile){
    int i = 0, n = 0;

    if (bFromFile == 0){
        /* Start layer: Randomize inital weights and bias */
        for (i = 0; i < NUM_NEURON_LAYER_START; i++) {
            for (n = 0; n < NUM_NEURON_LAYER_1; n++) {
                neuralNetworkToBeFilled->startLayer[i].weightToNext[n] = randomDecimal();
            }
        }
        /* First layer: Randomize inital weights and bias */
        for (i = 0; i < NUM_NEURON_LAYER_1; i++) {
            for (n = 0; n < NUM_NEURON_LAYER_END; n++) {
                neuralNetworkToBeFilled->firstLayer[i].weightToNext[n] = randomDecimal();
            }
        }
    } else {
        /* !!! OPEN from file, once functionality has been made !!! */
    }
    return;
}

/* Generates a random decimal number between -1 and 1 */
double randomDecimal(){
  return (double)(rand() % (10000 + 1 - (-10000)) + (-10000))/10000;
}

/* Normalizes a number between ~-14 & ~14 to a number between -1 & 1 */
/* More info: https://en.wikipedia.org/wiki/Sigmoid_function */
double sigmoid(double x){
    double euler = 0;
    euler = EULER;
    return 1/(1+(pow(euler,(-x))));
}
