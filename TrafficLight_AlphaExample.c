/* Information about neural network primarily learned from the youtube */
/* educational video series made by "3Blue1Brown" */
/* youtube.com/watch?v=aircAruvnKk&list=PLZHQObOWTQDNU6R1_67000Dx_ZCJB-3pi */
/* And this video: youtube.com/watch?v=ILsA4nyG7I0 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* General simulation defines */
#define SEED 55
#define AMOUNT_OF_CARS_PASS_PER_TICK 3
#define PERCENT_3_CAR_SPAWN 3
#define PERCENT_2_CAR_SPAWN 4
#define PERCENT_1_CAR_SPAWN 5

/* Neural network defines */
#define MAX_NEURON_IN_LAYER 40
#define NUM_NEURON_LAYER_START 5
#define NUM_NEURON_LAYER_1 40
#define NUM_NEURON_LAYER_2 20
#define NUM_NEURON_LAYER_END 2
#define EULER 2.71828182845904523536028747135266249775724709369995
#define NEURAL_SAVE_FILE_NAME "networkSave.csv"

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
    double weightToNext[MAX_NEURON_IN_LAYER];
    double value;
    double sumOfPrev;
} neuron_t;

/* The neural network struct */
typedef struct neuralNetwork_s{
    neuron_t startLayer[NUM_NEURON_LAYER_START];
    neuron_t firstLayer[NUM_NEURON_LAYER_1];
    neuron_t secondLayer[NUM_NEURON_LAYER_2];
    neuron_t endLayer[NUM_NEURON_LAYER_END];
} neuralNetwork_t;

/* Top-down programming declaration of functions */
void fillTrafficLight(trafficLight_t* trafficLightToBeFilled, road_t* rLeftRight,
                      road_t* rRightLeft, road_t* rUpDown, road_t* rDownUp);
void spawnCars(road_t* roadToSpawnCarsOn);
void printVisualization(trafficLight_t* trafficLight, int i);
void removeCars(road_t* road1, road_t* road2);
void trafficLightLogic(trafficLight_t* trafficLight, neuralNetwork_t* theNeuralNetwork);
void fillNeuralNetwork(neuralNetwork_t* neuralNetworkToBeFilled, int bFromFile);
void fillIndividualLayer(int numThis, int numNext, neuron_t layerToFill[]);
double randomDecimal();
void sigmoid(double* x);
int runNeuralNetwork(neuralNetwork_t* theNeuralNetwork, trafficLight_t* theTrafficLight);
double neuronWeightedValue(neuron_t neuron, int idInNext);
void fillNeuronLayer(int layerSize, int prevLayerSize, neuron_t thisLayer[], neuron_t prevLayer[]);
void saveNeuralNetwork(neuralNetwork_t* neuralNetworkToSave);
void saveIndividualLayer(FILE* file, int thisNum, int nextNum, neuron_t layer[]);

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

    /* Fill & save the neural network */
    fillNeuralNetwork(&theNeuralNetwork, 0);
    saveNeuralNetwork(&theNeuralNetwork);

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
        trafficLightLogic(&trafficLight, &theNeuralNetwork);

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
void trafficLightLogic(trafficLight_t* trafficLight, neuralNetwork_t* theNeuralNetwork){
    trafficLight->bVertical = runNeuralNetwork(theNeuralNetwork, trafficLight);

    printf("Neural %d\n", trafficLight->bVertical);
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
    /* Look if they shall be randomly generated or 1read from file */
    if (bFromFile == 0){
        printf("[Randomizing neural network weights]\n");

        fillIndividualLayer(NUM_NEURON_LAYER_START, NUM_NEURON_LAYER_1, neuralNetworkToBeFilled->startLayer);
        fillIndividualLayer(NUM_NEURON_LAYER_1, NUM_NEURON_LAYER_2, neuralNetworkToBeFilled->firstLayer);
        fillIndividualLayer(NUM_NEURON_LAYER_2, NUM_NEURON_LAYER_END, neuralNetworkToBeFilled->secondLayer);
    } else {
        /* !!! OPEN from file, once functionality has been made !!! */
    }
    return;
}

/* Fill an individual neuron layer */
void fillIndividualLayer(int numThis, int numNext, neuron_t layerToFill[]){
    int i = 0, n = 0;

    /* Loop first for each neuron in layer, then fill "weightToNext" */
    for (i = 0; i < numThis; i++) {
        for (n = 0; n < numNext; n++) {
            layerToFill[i].weightToNext[n] = randomDecimal();
        }
    }
    return;
}

/* Generates a random decimal number between -1 and 1 */
double randomDecimal(){
  return (double)(rand() % (10000 + 1 - (-10000)) + (-10000))/10000;
}

/* Normalizes a number x, to a number between 0 & 1 */
/* More info: https://en.wikipedia.org/wiki/Sigmoid_function */
void sigmoid(double* x){
    double euler = 0;
    euler = EULER;
    *x = 1/(1+(pow(euler,(-*x))));
    return;
}

/* Runs the neural network. Returns 0 or 1 depending on which direction needs to be green */
/* Error return value == -1 */
/* [0] = vert (return 0) | [1] = hori (return 1) */
int runNeuralNetwork(neuralNetwork_t* theNeuralNetwork, trafficLight_t* theTrafficLight){
    int i = 0;

    /* Fill and normalize input values */
    /* First four will be filled with amountOfCars */
    /* Last will be filled with current direction: vert == 0 | hori == 1 */
    for (i = 0; i < NUM_NEURON_LAYER_START; i++) {
        switch (i) {
        case 0:
            theNeuralNetwork->startLayer[i].value = (double)(theTrafficLight->rUpDown->amountOfCars)/10;
            break;
        case 1:
            theNeuralNetwork->startLayer[i].value = (double)(theTrafficLight->rDownUp->amountOfCars)/10;
            break;
        case 2:
            theNeuralNetwork->startLayer[i].value = (double)(theTrafficLight->rLeftRight->amountOfCars)/10;
            break;
        case 3:
            theNeuralNetwork->startLayer[i].value = (double)(theTrafficLight->rRightLeft->amountOfCars)/10;
            break;
        case 4:
            theNeuralNetwork->startLayer[i].value = (((double)(theTrafficLight->bVertical)) == 1) ? 1.0 : -1.0;
            break;
        default:
            printf("ERROR - ran too many times in start set input value of neurons!\n");
            break;
        }
    }

    /* Fill layers */
    fillNeuronLayer(NUM_NEURON_LAYER_1, NUM_NEURON_LAYER_START, theNeuralNetwork->firstLayer, theNeuralNetwork->startLayer);
    fillNeuronLayer(NUM_NEURON_LAYER_2, NUM_NEURON_LAYER_1, theNeuralNetwork->secondLayer, theNeuralNetwork->firstLayer);
    fillNeuronLayer(NUM_NEURON_LAYER_END, NUM_NEURON_LAYER_2, theNeuralNetwork->endLayer, theNeuralNetwork->secondLayer);

    /* Decide return value, based on largest end value */
    if (theNeuralNetwork->endLayer[0].value > theNeuralNetwork->endLayer[1].value){
        return 0;
    } else if (theNeuralNetwork->endLayer[0].value == theNeuralNetwork->endLayer[1].value){
        printf("EQUAL RESULT IN NEURAL, returned 0\n");
        return 0;
    } else if (theNeuralNetwork->endLayer[0].value < theNeuralNetwork->endLayer[1].value){
        return 1;
    } else return -1;
}

/* Returns the weighted value of a given neuron */
double neuronWeightedValue(neuron_t neuron, int idInNext){
    return (neuron.value * neuron.weightToNext[idInNext]);
}

/* Function used to fill a given layer */
void fillNeuronLayer(int layerSize, int prevLayerSize, neuron_t thisLayer[], neuron_t prevLayer[]){
    int i = 0, n = 0;

    for (i = 0; i < layerSize; i++) {
        /* First null the sumOfPrev */
        thisLayer[i].sumOfPrev = 0;

        /* Sum the values of each previous neuron (input) * their weight to this neuron */
        for (n = 0; n < prevLayerSize; n++) {
            thisLayer[i].sumOfPrev += neuronWeightedValue(prevLayer[n], i);
        }

        /* Squish sum into -1 to 1 range using tanh function */
        thisLayer[i].sumOfPrev = tanh(thisLayer[i].sumOfPrev);

        /* Set value to sum */
        thisLayer[i].value = thisLayer[i].sumOfPrev;
    }
    return;
}

/* Saves the weights of each neuron */
/* Each layer is split by a newline */
/* First line is sizes - to be able to verify that it is the same */
/* Neuron format: Neurons seperated by ; weights seperated by , */
void saveNeuralNetwork(neuralNetwork_t* neuralNetworkToSave){
    /* Create file and open it in writing mode*/
    FILE* file;
    file = fopen(NEURAL_SAVE_FILE_NAME, "w");

    /* Save sizes */
    fprintf(file, " %d;%d;%d;%d\n", NUM_NEURON_LAYER_START, NUM_NEURON_LAYER_1, NUM_NEURON_LAYER_2, NUM_NEURON_LAYER_END);

    /* Save each layer */
    saveIndividualLayer(file, NUM_NEURON_LAYER_START, NUM_NEURON_LAYER_1, neuralNetworkToSave->startLayer);
    saveIndividualLayer(file, NUM_NEURON_LAYER_1, NUM_NEURON_LAYER_2, neuralNetworkToSave->firstLayer);
    saveIndividualLayer(file, NUM_NEURON_LAYER_2, NUM_NEURON_LAYER_END, neuralNetworkToSave->secondLayer);
    return;
}

/* Save individual layer */
void saveIndividualLayer(FILE* file, int thisNum, int nextNum, neuron_t layer[]){
    int i = 0, n = 0;
    /* Save weights for layer  */
    for (i = 0; i < thisNum; i++) {
        for (n = 0; n < nextNum; n++) {
            fprintf(file, "%lf%c", layer[i].weightToNext[n], (n != (nextNum-1)) ? ',' : '0');
        }
        fprintf(file, ";");
    }
    fprintf(file, "\n");
    return;
}
