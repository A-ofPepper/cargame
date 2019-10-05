/*
 * You can add more of your own developed *.h files here.
 * However, you are not allowed to include driverlib, grlib, stdio or any other library in this file.
 */

#include <LED_HAL.h>
#include <Buttons_HAL.h>
#include <Timer_HAL.h>
#include <Display_HAL.h>
#include <ADC_HAL.h>
#define upLevel 0x3000
#define downLevel 0x1000
#define carY 112
#define OPENING_WAIT 3000 // 3 seconds or 10000 ms
#define fiftyms 50 //50 ms
#define rightBOUND 0x23
#define leftBOUND 0x02
#define carSTART 0x03
#define lane1 3
#define lane2 19
#define lane3 35
#define menuLINE2 2
#define menuLINE3 3
#define menuLINE4 4

extern Graphics_Context g_sContext;
extern tImage favicon8BPP_UNCOMP; //player car
extern tImage favicon__2_8BPP_UNCOMP; //opponent cars
extern tImage dark8BPP_UNCOMP; //black square to erase motion
int scoreTrack[3] = {0,0,0}; //keeps track of the 3 highest scores

int abV(int a, int b){ //calculates absolute value for collision
    if((a-b) >= 0){
        return (a-b);
    }else{
        return (b-a);
    }
}

void DrawOpeningScreen() //prints the opening screen
{
    LCDClearDisplay(MY_BLACK);
    PrintString("DRIVING GAME", 2, 2);
    PrintString("by", 3, 3);
    PrintString("Thomas Sand", 4, 2);
    PrintString("Wait 3 sec", 6, 1);
}

void DrawMenuScreen() //draw the menu with 3 options
{
    LCDClearDisplay(MY_BLACK);
    PrintString("MENU", 0, 6);
    PrintString("Play Game", 2, 3);
    PrintString("How to Play", 3, 3);
    PrintString("Score History", 4, 3);
}

void DrawScoresScreen() //print the top three scores
{

    LCDClearDisplay(MY_BLACK);
    PrintString("SCORES", 0, 5);
    PrintString("1st: ", 2, 1);
    LCDDrawChar(2, 9, (scoreTrack[0]+'0'));
    PrintString("2nd: ", 4, 1);
    LCDDrawChar(4, 9, (scoreTrack[1]+'0'));
    PrintString("3rd: ", 6, 1);
    LCDDrawChar(6, 9, (scoreTrack[2]+'0'));
    PrintString("BTN1-MENU", 7, 7);

}

void DrawIntructions() // draw the instruction of the game
{
    LCDClearDisplay(MY_BLACK);
    PrintString("INSTRUCTIONS", 0, 2);
    PrintString("Move the car", 2, 0);
    PrintString("around the trees", 3, 0);
    PrintString("to score points", 4, 0);
    PrintString("BTN1-MENU", 7, 7);
}

void gameScreen(){ //print the screen of the game

    LCDClearDisplay(MY_BLACK);
    PrintString("Score", 0, 9);
    PrintString("High", 3, 9);
    PrintString("Score ", 4, 9);
    LCDDrawChar(5, 15, (scoreTrack[0]+'0'));
    Graphics_drawLine(&g_sContext, 0x00, 0x00, 0x00, 0x7f);
    Graphics_drawLine(&g_sContext, 0x01, 0x00, 0x01, 0x7f);
    Graphics_drawLine(&g_sContext, 0x33, 0x00, 0x33, 0x7f);
    Graphics_drawLine(&g_sContext, 0x34, 0x00, 0x34, 0x7f);
}

void gameOver(int scoreVal){ //prints game over screen

    LCDClearDisplay(MY_BLACK);
    PrintString("GAME OVER", 0, 2);
    PrintString("Score", 2, 0);
    LCDDrawChar(2, 7, (scoreVal+'0'));
}

void pauseScreen(){ //prints pause menu with two options

    LCDClearDisplay(MY_BLACK);
    PrintString("GAME MENU", 0, 3);
    PrintString("Continue", 2, 3);
    PrintString("Menu", 3, 3);
}

void trackScore(int val){ //takes in score and determines where in the list it goes

    if(val > scoreTrack[0]){
        scoreTrack[2] = scoreTrack[1];
        scoreTrack[1] = scoreTrack[0];
        scoreTrack[0] = val;
    }else if(val == scoreTrack[0]){
        scoreTrack[2] = scoreTrack[1];
        scoreTrack[1] = val;
    }else if(val > scoreTrack[1] && val < scoreTrack[0]){
        scoreTrack[2] = scoreTrack[1];
        scoreTrack[1] = val;
    }else if(val == scoreTrack[1]){
        scoreTrack[2] = val;
    }else if(val > scoreTrack[2]){
        scoreTrack[2] = val;
    }
}

int triSelect(int pos, bool button){ //joystick control to handle three options, meant for menu

    static int initpos = menuLINE2;

    switch(pos){

    case 1: //based on up down or middle, draw the arrow up
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        PrintString(">", initpos, 2);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);

        initpos--;
        if(initpos < menuLINE2){
            initpos = menuLINE4;
        }
        break;

    case -1: //draw the arrow down
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        PrintString(">", initpos, 2);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        initpos++;
        if(initpos > menuLINE4){
            initpos = menuLINE2;
        }
        break;

    default:
        break;
    }
    PrintString(">", initpos, 2); //print the arrow in the new position

    if(button){ //if the button is pushed, return the value of the line it was pushed on
        switch(initpos){
        case menuLINE2:
            return menuLINE2;
            break;
        case menuLINE3:
            return menuLINE3;
            break;
        case menuLINE4:
            return menuLINE4;
            break;
        }
    }
    return 0;
}

int biSelect(int pos, bool button){ //chooses from two options

    static int initpos = 2; //sets initial row

    switch(pos){

    case 1: //erases, and moves up
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        PrintString(">", initpos, 2);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);

        initpos--;
        if(initpos < menuLINE2){
            initpos = menuLINE3;
        }
        break;

    case -1: //erases and moved down
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        PrintString(">", initpos, 2);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        initpos++;
        if(initpos > menuLINE3){
            initpos = menuLINE2;
        }
        break;

    default:
        break;
    }
    PrintString(">", initpos, 2); //prints new arrow

    if(button){ //returns the line that the button was pushed on
        switch(initpos){
        case menuLINE2:
            return menuLINE2;
            break;
        case menuLINE3:
            return menuLINE3;
            break;
        }
    }
    return 0;
}

int joystickAction(unsigned int y){ //fsm for moving the arrow

    static enum status {down, middle, up} stick = middle;

    switch(stick){
    case middle: //when the joystick is in the middle
        if(y >= upLevel){
            stick = up;
        }else if(y <= downLevel){
            stick = down;
        }
        break;
    case up://when the joystick is beyond the up threshhold
        if(y < upLevel){ //if the joystick moves down below the up threshhold
            stick = middle;
            return 1; //return that it was up
         }
        break;
    case down: //joystick is below the down threshhold
        if(y > downLevel){ //if the joystick moves back above the down threshhold
            stick = middle;
            return -1; //return that it was down
        }
        break;
    }
    return 0; //if not up or down return 0
}


int sideWays(unsigned int x, bool reset){ //controls the sideways operation of the joystick for the car

    static enum status {left, middle, right} joystick = middle;
    static OneShotSWTimer_t moveCar;
    InitOneShotSWTimer(&moveCar, TIMER32_1_BASE, fiftyms); //creates the timer for the car's motion
    static bool R = true;
    static bool L = true;

    if(!reset){ //if the variables are not being reset
            switch(joystick){ //get the joystick
            case middle: //if in the middle, check if up or down
               if(x >= upLevel){
                   joystick = right;
               }else if(x <= downLevel){
                   joystick = left;
               }
               break;
            case left: //if it went left, start the timer
                if(L){
                    StartOneShotSWTimer(&moveCar);
                    L = false;
                }
              if(x < upLevel){
                 joystick = middle;
              }
              if(OneShotSWTimerExpired(&moveCar)){ //if the timer expires, move left, then check if the joystick is still left or back in the middle
                L = true;
                return -1; //return left
              }
               break;
            case right:
                if(R){ //if on the right, start the timer
                    StartOneShotSWTimer(&moveCar);
                    R = false;
                }
                if(x < upLevel){
                  joystick = middle;
                }
               if(OneShotSWTimerExpired(&moveCar)){ //if the timer runs out, move right and check if the joystick is till on the right or in the middle
                   R = true;
                  return 1; //return right
               }
               break;
            }
    }else{ //reset variables if the code gets rerun without a reset
        R = true;
        L = true;
        joystick = middle;
    }
    return 0;
}

void car(unsigned int xVal, unsigned int *carVal, bool reset){ //controls the car's motion

if(!reset){ //run when the variables are not being reset
    switch(sideWays(xVal, reset)){ //switch on the operation found by the sideways motion FSM
            case 1:
                if(*carVal < rightBOUND){ //if the value of the car's x value is less than the right bound
                    Graphics_drawImage(&g_sContext, &dark8BPP_UNCOMP, *carVal, 112); //erase, increment and reprint
                    (*carVal)++;
                    Graphics_drawImage(&g_sContext, &favicon8BPP_UNCOMP, *carVal, 112);
                }
                break;
            case -1:
                if(*carVal > leftBOUND){ //if the car is greater than the left bound of the screen, move to the right
                    Graphics_drawImage(&g_sContext, &dark8BPP_UNCOMP, *carVal, 112);//erase, move the arrow, then reprint
                    (*carVal)--;
                    Graphics_drawImage(&g_sContext, &favicon8BPP_UNCOMP, *carVal, 112);
                }
                break;
            default: //just to catch the 0 case
                break;
            }
    }else{ //reset variables in sideways
        sideWays(xVal, reset);
    }
}

int randVal(){ //generates a random value for the x position of the obstacles

    unsigned vx, vy;
    static int randomX = 0;
    int randBit = 0;
    int i;

    for(i = 0; i < 16; i++){ //creates a 16 bit int
        getSampleJoyStick(&vx, &vy);

        randBit = (vx%2) ^ (vy%2);
        randomX = randomX << 1; //shifts bits over
        randomX += randBit; //adds on random bit
    }
    return (randomX % 3); //return a value of 0,1, or 2

}

void obstacles(unsigned int rand, unsigned int array[3][2], bool *reset, bool *scoreInc){ //controls the three obstacles on the screen

    static OneShotSWTimer_t moveTREE;
    InitOneShotSWTimer(&moveTREE, TIMER32_1_BASE, fiftyms); //creates timer for the obstacles

    static int treeNum = 0; //controls what obstacle is being controlled
    static bool row1, row2, row3 = false; //says which obstacle is on
    static bool begin = true; //controls if a new obstacle is created
    static int count = 0; //controls the counter for every 50 pixels

    if(!(*reset)){ //if not being reset
        if(begin){ //create a new obstacle
            begin = false;
            switch(rand){ //set x val based on the random number
            case 0:
                array[treeNum][0] = lane1;
                array[treeNum][1] = 0;
                break;
            case 1:
                array[treeNum][0] = lane2;
                array[treeNum][1] = 0;
                break;
            case 2:
                array[treeNum][0] = lane3;
                array[treeNum][1] = 0;
                break;
            default:
                array[treeNum][0] = lane3; //catches the rare case when the random value created is not 0 1 2
                array[treeNum][1] = 0;
                break;
            }
            switch(treeNum){ //turns on the new obstacle that got created
            case 0:
                row1 = true;
                break;
            case 1:
                row2 = true;
                break;
            case 2:
                row3 = true;
                break;
            }
            Graphics_drawImage(&g_sContext, &favicon__2_8BPP_UNCOMP, array[treeNum][0], array[treeNum][1]); //prints the new object
            StartOneShotSWTimer(&moveTREE); //begins to time
        }
        if(OneShotSWTimerExpired(&moveTREE)){ //every 50ms...
            if(row1){ //increment 1st object if it exists
                Graphics_drawImage(&g_sContext, &dark8BPP_UNCOMP, array[0][0], array[0][1]);
                array[0][1]++;
                Graphics_drawImage(&g_sContext, &favicon__2_8BPP_UNCOMP, array[0][0], array[0][1]);
            }
            if(row2){ //increment second object if it exists
                Graphics_drawImage(&g_sContext, &dark8BPP_UNCOMP, array[1][0], array[1][1]);
                array[1][1]++;
                Graphics_drawImage(&g_sContext, &favicon__2_8BPP_UNCOMP, array[1][0], array[1][1]);
            }
            if(row3){ //increment 3rd object if it exists
                Graphics_drawImage(&g_sContext, &dark8BPP_UNCOMP, array[2][0], array[2][1]);
                array[2][1]++;
                Graphics_drawImage(&g_sContext, &favicon__2_8BPP_UNCOMP, array[2][0], array[2][1]);
            }
            count++; //increments the counter to determine if 50 pixels are reached
            StartOneShotSWTimer(&moveTREE);
        }
        if(array[0][1]>128){ //resets the obstacles if they move off the screen, and determine whether the score can be incremented
            row1 = false;
            array[0][1] = 0;
            (*scoreInc) = true;
        }
        if(array[1][1]>128){
            row2 = false;
            array[1][1] = 0;
            (*scoreInc) = true;
        }
        if(array[2][1]>128){
            row3 = false;
            array[2][1] = 0;
            (*scoreInc) = true;
        }
        if(count >= 50){ //checks if the count is at 50, turns on the object creator and increments the object number
            count = 0;
            begin = true;
            treeNum++;
            if(treeNum > 2){
                treeNum = 0;
            }
        }
    }else{ //resets the variables
        *reset = false;
        begin = true;
        treeNum = 0;
        row1 = false;
        row2 = false;
        row3 = false;
        count = 0;
    }
}

void runGame(bool *reset, bool *paused, int *playerScore, bool *collide){ //runs the car and obstacles, checks for collisions, and increments the score

    static unsigned int carX = carSTART; //sets the beginning position of the car
    static unsigned jX, jY; //holds the joystick values
    static unsigned int trees[3][2] = {{0,0},{0,0},{0,0}}; //creates obstacle's xs and ys
    unsigned int rand = randVal(); //gets a random value
    static bool print = true; //send scoreInc into obstabcles to determine if the score can be inc
    static bool scoreInc = false; //determines when the increment the score

    if(!(*reset)){ //if not being reset
        if(*paused == true){ //if the game was paused, reprint the car
            print = true;
            *paused = false;
        }
        if(print){ //print the car
          Graphics_drawImage(&g_sContext, &favicon8BPP_UNCOMP, carX, carY);
          print = false;
        }
        getSampleJoyStick(&jX, &jY); //get joystick vals
        car(jX, &carX, *reset); //control the car
        obstacles(rand, trees, &(*reset), &scoreInc); //control the obstacles

        if((abV(carX, trees[0][0]) < 16) && (abV(carY, trees[0][1]) < 16)){ //check collision with object 1
            (*collide) = true;
        }else if((abV(carY, trees[1][1]) < 16) && (abV(trees[1][0], carX) < 16)){ //check collision with object 2
            (*collide) = true;
        }else if((abV(carY, trees[2][1]) < 16) && (abV(trees[2][0], carX) < 16)){ //check collision with object 3
            (*collide) = true;
        }

        if(scoreInc){ //increment the score when allowed
            (*playerScore)++;
            scoreInc = false;
        }
        LCDDrawChar(2, 15, (*playerScore +'0')); //print the score

    }else{ //reset the variables
        carX = carSTART;
        print = true;
        *playerScore = 0;
        trees[0][0] = 0;
        trees[0][1] = 0;
        trees[1][0] = 0;
        trees[1][1] = 0;
        trees[2][0] = 0;
        trees[2][1] = 0;
        car(jX, &carX, reset);
        obstacles(rand, trees, &(*reset), &scoreInc);
    }
}

void ScreensFSM(){ //controls the total flow of the game
    static enum states {INCEPTION, OPENING, MENU, INSTRUCT, SCORE, GAMESCREEN, GAME, PAUSESCREEN, PAUSE, GAMEOVER} state = INCEPTION;
    static OneShotSWTimer_t OST;
    InitOneShotSWTimer(&OST, TIMER32_1_BASE, OPENING_WAIT);
    bool drawOpening, drawMenu, scores, instruct, game = false;
    static bool reset, paused, collide = false;
    static int scoreCount = 0;
    unsigned xVal, yVal;
    switch (state) {
    case INCEPTION: //beginning of the game, prints opening screen
        StartOneShotSWTimer(&OST);
        state = OPENING;
        drawOpening = true;
        break;
    case OPENING: //prints menu
        if (OneShotSWTimerExpired(&OST)){
            state = MENU;
            drawMenu = true;
        }
        break;
    case MENU: //controls cursor and option select
        getSampleJoyStick(&xVal, &yVal);
        switch(triSelect(joystickAction(yVal), Joystick_Button_Pushed())){ //chooses action based on button and current position
        case menuLINE2:
            game = true;
            state = GAME; //goes to game screen
            break;
        case menuLINE3:
            instruct = true;
            state = INSTRUCT; //goes to instructions
            break;
        case menuLINE4:
            scores = true;
            state = SCORE; //goes to score screen
            break;
        default:
            break;
        }
        break;
     case INSTRUCT:
         if(Booster_Top_Button_Pushed()){ //checks if the button is pushed to go back to the menu
             state = MENU;
             drawMenu = true;
         }
        break;
     case SCORE:
         if(Booster_Top_Button_Pushed()){ //checks if the button is pushed to go back to the menu
             state = MENU;
             drawMenu = true;
         }
         break;
     case GAMESCREEN: //prints game screen and moves to run the game
         game = true;
         state = GAME;
         break;
     case GAME:
         if(reset){ //if the game is to be reset, run the game with the reset bool active to make all statics back to their original value
             runGame(&reset, &paused, &scoreCount, &collide);
         }
         if(!Booster_Top_Button_Pushed()){ //if the top button is pushed, go to the pause menu
             runGame(&reset, &paused, &scoreCount, &collide);
         }else{
             state = PAUSESCREEN;
         }
         if(collide) //if there is a collision, go to the game over
             state = GAMEOVER;
         break;
     case PAUSESCREEN: //print the pause screen
         pauseScreen(); state = PAUSE;
         break;
     case PAUSE: //get joystick values to control the cursor on the pause menu
         getSampleJoyStick(&xVal, &yVal);
         switch(biSelect(joystickAction(yVal), Joystick_Button_Pushed())){
         case menuLINE2:
             state = GAMESCREEN; //if continue is chose, go back to print the game screen
             paused = true;
             break;
         case menuLINE3:
             state = OPENING; //if menu is chosen, go back to printing the menu
             reset = true;
             scoreCount = 0;
             break;
         }
         break;
     case GAMEOVER: //if the game ends, it goes back to print the menu, sets the game to be reset, and shows the game over screen
         collide = false;
         gameOver(scoreCount);
         state = OPENING;
         trackScore(scoreCount);
         reset = true;
         StartOneShotSWTimer(&OST);
         break;
    }
    if (drawOpening) //prints the opening
        DrawOpeningScreen();

    if (drawMenu) //prints the menu
        DrawMenuScreen();

    if(scores)//prints the scores
        DrawScoresScreen();

    if(instruct) //prints the instructions
        DrawIntructions();

    if(game) //prints the game screen
        gameScreen();
}

int main(void) {

    WDT_A_hold(WDT_A_BASE);

    BSP_Clock_InitFastest(); //initalizes graphics, timers, leds, and joystick functions
    InitGraphics();
    InitHWTimers();
    InitButtons();
    InitLEDs();
    initADC_Multi();
    initJoyStick();
    startADC();

    while (1) {
        // Do not delete this statement. We will use this function to check if your program does not block on anything.
        if (Booster_Bottom_Button_Pushed())
            Toggle_Launchpad_Right_Red_LED();

        ScreensFSM(); //runs all functions of the screen
    }
}
