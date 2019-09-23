#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Name: Luis Estevez
 */

//global variables for TM
int numOfStates;
int startState;
int endState;

struct Node{
    char data;
    struct Node* next;
    struct Node* prev;
};

typedef struct Instruction{
    char writeValue;
    char moveDirection;
    int newState;
}Instruction;

//head of list
struct Node* head;

void printListForward(struct Node* node){
    while (node != NULL){
        printf(" %c ", node->data);
        node = node->next;
    }
    printf("\n");
}

void printListReverse(struct Node* node){
    struct Node* tail = NULL;
    //gets last element on list(tail)
    while(node != NULL){
        tail = node;
        node = node->next;
    }
    while (tail != NULL){
        printf(" %c ", tail->data);
        tail = tail->prev;
    }
}

//add to front of list
void push(struct Node** hp, char d){

    //allocate space for node
    struct Node* tempNode = (struct Node*)malloc(sizeof(struct Node));

    tempNode->data = d;
    tempNode->next = *hp;
    tempNode->prev = NULL;

    //change prev of head node to new node
    if((*hp) != NULL){
        (*hp)->prev = tempNode;
    }

    *hp = tempNode;
}

//add node to tail
void append(struct Node** hp, char d) {

    //allocate space for node
    struct Node* tempNode = (struct Node*)malloc(sizeof(struct Node));
    struct Node* end = *hp;

    tempNode->data = d;
    tempNode->next = NULL;

    //checks if list is empty
    //if so it makes new node the head
    if(*hp == NULL){
        tempNode->prev = NULL;
        *hp = tempNode;
        return;
    }
    //goes through list to get the last node
    while(end->next != NULL) {
        end = end->next;
    }
    end->next = tempNode;
    tempNode->prev = end;

}

//(CurrentState,ReadVal)->(WriteVal,MoveDirection,NewState)
//2D array for instructions

void runMachine(struct Node** rw, struct Instruction instructionset[numOfStates][128], int curr) {
    //check if done
    if (curr == endState) {
        printf("Halt.\n");
        printf("final tape: ");
        printListForward(head);
        return;
    }

    struct Node *temp = *rw;
    int currSt = curr;
    //get ascii for array
    int ascii = (int) (temp->data);

    temp->data = instructionset[currSt][ascii].writeValue;

    if(instructionset[currSt][ascii].moveDirection == 'R'){
        if(temp->next == NULL){
            struct Node* tempNode = (struct Node*)malloc(sizeof(struct Node));
            //appending empty cell to the end of list
            tempNode->data = 'B';
            tempNode->next = NULL;
            tempNode->prev = temp;
            temp->next = tempNode;
        }
        runMachine(&temp->next, instructionset, instructionset[currSt][ascii].newState);
    }else if(instructionset[currSt][ascii].moveDirection == 'L'){
        runMachine(&temp->prev, instructionset, instructionset[currSt][ascii].newState);
    }
}

int main() {

    char input[50];
    char singleLine[60];
    FILE *fp;

    printf("Input file path:");
    fgets(input, 50, stdin);
    //gets rid of '\n' character
    input[strlen(input) - 1] = 0;

    printf("Path given: %s\n", input);
    fp = fopen(input, "r");
    if(fp == NULL){
        printf("\nFile not found!\n");
        exit(1);
    }

    /**
     * Assumes all input files will be in correct format
     * Line 1: Initial tape content (written after ‘A’ cell)
     * Line 2: Number of states
     * Line 3: Start state
     * Line 4: End state
     * Final Lines are commands
     * (CurrentState,ReadVal)->(WriteVal,MoveDirection,NewState)
     */
    int i = 0;

    //writing 'A' in first cell
    head = (struct Node*)malloc(sizeof(struct Node));
    head->data = 'A';
    head->prev = NULL;
    head->next = NULL;
    //read while not end of file
    while(!feof(fp)){
        fgets(singleLine, 60, fp);
        if(i == 0){
            //Line 1
            for(int x = 0; x < strlen(singleLine) - 1; x++){
                //creating list
                append(&head, (char)(singleLine[x]));

            }
            printf("Initial tape content:");
            printListForward(head);

        }else if(i == 1){
            //Line 2
            numOfStates = atoi(singleLine);

        }else if(i == 2){
            //Line 3
            startState = atoi(singleLine);

        }else if(i == 3){
            //Line 4
            endState = atoi(singleLine);
            //stop reading file to create the instruction set

            break;
        }
        i++;
    }


    //create 2d array instructiion set
    struct Instruction instructionSet[numOfStates][128];
    //continue reading file
    //(CurrentState,ReadVal)->(WriteVal,MoveDirection,NewState)
    while(!feof(fp)){

        fgets(singleLine, 60, fp);
        const char x[7] = "()->,\n";
        char *token;
        token = strtok(singleLine, x);

        int cnt = 0;

        int currentSt;
        char readVal;
        char writeVal;
        char move;
        int newSt;
        while(token != NULL){

            if(cnt == 0){
                //breaking up parenthesis
                //token = CurrentState
                currentSt = atoi(token);
                cnt = 1;

            }else if(cnt == 1){
                //token = ReadVal
                int temp = (int)(*token);
                readVal = (char)(temp);
                cnt = 2;

            }else if(cnt == 2){

                //token = WriteVal
                int temp = (int)(*token);
                writeVal = (char)(temp);
                cnt = 3;

            }else if(cnt == 3){

                //token = MoveDirection
                //move = token;
                int temp = (int)(*token);
                move = (char)(temp);
                cnt = 4;

            }else if(cnt == 4){
                //token = NewState
                newSt = atoi(token);
                int ascii = (int) (readVal);

                instructionSet[currentSt][ascii].writeValue = writeVal;
                instructionSet[currentSt][ascii].moveDirection = move;
                instructionSet[currentSt][ascii].newState = newSt;

                cnt = cnt - 4;
            }
            token = strtok(NULL, x);

        }
    }

    runMachine(&head, instructionSet, startState);

    fclose(fp);

    return 0;
}