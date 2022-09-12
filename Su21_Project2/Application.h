/*
 * Application.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_
#define TITLE_SCREEN_WAIT 3000
#define GAME_PLAY_TIME 30000
#define SECOND_COUNT 1000
#define PAY_RAISE 5
#define GAMEPLAY_ROW1 18
#define GAMEPLAY_ROW2 61
#define GAMEPLAY_COL1 2
#define GAMEPLAY_COL2 44
#define GAMEPLAY_COL3 86

#define MaxLengthVal 3  //the max length of a int or char to be merged with a string
#define MoveExplorerGraphicBy 20 //the amount that the explorer is moved by on the display
#define MainMenuSpacing 2

#define MAX_RECIPE_LENGTH 3
#define STRIKE_START_X 16

#include <HAL/HAL.h>

typedef enum {UP, DOWN, LEFT, RIGHT, UPLEFT, UPRIGHT} _directToMoveCursor;

typedef enum{TITLE_SCREEN, MAIN_MENU, INSTRUCTIONS, TOP_SCORE, GAMEPLAY, GAMEOVER} _dinnerDashGameState;

typedef enum{AT_HOW_TO_PLAY, AT_PLAY_GAME, AT_TOP_SCORE} _cursorState;




struct _Application
{
    // Put your application members and FSM state variables here!
    // =========================================================================


    bool up;
    bool down;
    bool right;
    bool left;
    bool upleft;
    bool upright;
    bool middle;
    //User Selections
    bool addedPatty;
    bool addedLettuce;
    bool addedCheese;
    bool addedBun;
    bool addedTomato;
    bool addedCondiments;

    //Order Contents
    bool hasPatty;
    bool hasLettuce;
    bool hasCheese;
    bool hasBun;
    bool hasTomato;
    bool hasCondiments;

    //Cursor positioning
    int cursorXcoord;
    int cursorYcoord;
    _directToMoveCursor gamePlayHighlight;


    //game play data
    int numOfStrikes;
    int numOfSecLeft;
    int currentPay;
    bool hasHighScore;

    //High Score values
    int topScoreNum1;
    int topScoreNum2;
    int topScoreNum3;


    //Timers
    SWTimer timer; //general purpose timer for when screens must disappear
    SWTimer gamePlayTimer; //time for when gameplay ends
    SWTimer secondCounter; //counts one second and is used to update countdown on display

    //States for FSMs
    _dinnerDashGameState gameState; //Determines which screen is currently shown
    _cursorState cursorState; //determines position of cursor






};
typedef struct _Application Application;

// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

// Called once per super-loop of the main application.
void Application_loop(Application* app, HAL* hal);


//Title Screen set up and states
void showTitleScreen(GFX* gfx_p);
void HandleTitleScreen(Application* app_p, HAL* hal_p);

//Main Menu set up and states
void ShowMainMenu(Application* app_p, GFX* gfx_p);
void HandleMainMenu(Application* app_p, HAL* hal_p);

//redraws the cursor for the Main Menu
void redrawCursor(Application *app_p, GFX *gfx_p, _directToMoveCursor directToMove);

//High Scores set up, Instruction set up and their states
void ShowHighScores(Application* app_p, GFX* gfx_p);
void HandleInstructionsAndScores(Application* app_p, HAL* hal_p);
void ShowInstructions(Application* app_p, GFX* gfx_p);

//Game Play set up, and states
void ShowGamePlayArea(Application* app_p, GFX *gfx_p);
void HandleGamePlayArea(Application* app_p, HAL *hal_p, GFX *gfx_p);

//GameOver setup and states
void  HandleGameOver(Application* app_p, HAL *hal_p, GFX *gfx_p);
void  ShowGameOver(Application* app_p, GFX *gfx_p);

//handles selecting food items in GamePlay state
void GamePlaySelectionOperations(Application *app_p, GFX *gfx_p, HAL *hal_p);

//handles highlighting food items in GamePlay state
void GamePlayHighlightingOperations(Application *app_p, GFX *gfx_p, HAL *hal_p);

//compares user inputs to saved ticket
void compareTicketToOrder(Application *app_p, GFX *gfx_p, HAL *hal_p);

//randomly selects an order to display. Also stores order in app values
void GenerateRandomTicket(Application *app_p, HAL *hal_p);

//displays the order information on the screen
void displayRandomTicket(Application *app_p, GFX *gfx_p, char* title, char* ingredientsLine1, char* ingredientsLine2);

//clears all values related to the most recent order
void ClearCurrentOrder(Application *app_p, HAL *hal_p, GFX *gfx_p);

//updates the user inputs. Either adds new items or removes a previous selection
void updateAddedItems(Application *app_p, HAL *hal_p, GFX *gfx_p, bool addOrRemove);

//updates number of strikes and if gameplay state switches to gameover state
void UpdateStrikes(Application *app_p, HAL *hal_p, GFX *gfx_p);

//returns values related to GamePlay state to their inital values
void resetGamePlay(Application *app_p, HAL *hal_p);

//updates the highscore values used by the HighScores state
void updateHighScore(Application *app_p);

//draws the white boxes used as part of the gameplay interface as well as the green highlight
void makeWhiteBox(GFX *gfx_p);
void makeWhiteBoxRest(GFX *gfx_p);
void makeWhiteBoxUp(GFX *gfx_p);
void makeWhiteBoxLeft(GFX *gfx_p);
void makeWhiteBoxRight(GFX *gfx_p);
void makeWhiteBoxUpLeft(GFX *gfx_p);
void makeWhiteBoxUpRight(GFX *gfx_p);

//make all selection bools false
void makeAllFalse(Application *app_p);

//make all order bools false
void makeAllOrderFalse(Application *app_p);

//make all order bools true
void makeAllOrderTrue(Application *app_p);

//merges strings together with characters and integers
void PrintInt(GFX* gfx_p, char* string, int toMerge, int y, int x);
void PrintDoubleDigit(GFX* gfx_p, char* string, int toMerge, int y, int x);
void PrintChar(GFX* gfx_p, char toMerge, int y, int x);

#endif /* APPLICATION_H_ */
