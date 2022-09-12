/**
 * Starter code for Project 2. Good luck!
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* HAL and Application includes */
#include <Application.h>
#include <HAL/HAL.h>
#include <HAL/Timer.h>

extern const Graphics_Image title_screen_art8BPP_UNCOMP; //title screen

//graphics used in program
extern const Graphics_Image bun_art4BPP_UNCOMP; //bun art
extern const Graphics_Image cheese_art4BPP_UNCOMP; //cheese art
extern const Graphics_Image condiments_art4BPP_UNCOMP; //condiments art
extern const Graphics_Image lettuce_art4BPP_UNCOMP; //lettuce art
extern const Graphics_Image patty_art4BPP_UNCOMP; //Patty art
extern const Graphics_Image tomato_art4BPP_UNCOMP; //tomato art

extern const Graphics_Image menu_lower_border8BPP_UNCOMP; //Main menu lower border (may be used in other areas)
extern const Graphics_Image menu_upper_border8BPP_UNCOMP; //Main menu upper border (may be used in other areas)

extern const Graphics_Image game_over_fail_screen4BPP_UNCOMP; //game over fail
extern const Graphics_Image game_over_success8BPP_UNCOMP; //game over success

//these are constant values used throughout the program

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void InitNonBlockingLED()
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void PollNonBlockingLED()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == 0)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}

int main(void)  //possibly remove void?
{
    WDT_A_holdTimer();

    // Initialize the system clock and background hardware timer, used to enable
    // software timers to time their measurements properly.
    InitSystemTiming();

    //Initialize the main Application object and the HAL
    HAL hal = HAL_construct();
    Application app = Application_construct(&hal);
    // Application_showTitleScreen(&hal.gfx);

    // Do not remove this line. This is your non-blocking check.
    InitNonBlockingLED();

    //displays title screen graphic
    showTitleScreen(&hal.gfx);
    while (1)
    {
        // Do not remove this line. This is your non-blocking check.
        PollNonBlockingLED();

        //Applications loop
        Application_loop(&app, &hal);
        HAL_refresh(&hal); //check the inputs
    }

}

/**
 * The main constructor for your application. This function should initialize
 * each of the FSMs which implement the application logic of your project.
 *
 * @return a completely initialized Application object
 */
Application Application_construct()
{
    Application app;

    // Initialize local application state variables here!

    //app.gameRoundEnded = false;
    app.cursorXcoord = 3;
    app.cursorYcoord = 6;
    app.cursorState = AT_HOW_TO_PLAY;
    app.numOfSecLeft = 30;
    app.currentPay = 0;
    app.topScoreNum1 = 0;
    app.topScoreNum2 = 0;
    app.topScoreNum3 = 0;

    app.hasHighScore = false;
    app.gamePlayHighlight = DOWN;
    //set up ingredient states
    app.addedPatty = false;
    app.addedCheese = false;
    app.addedLettuce = false;
    app.addedBun = false;
    app.addedTomato = false;
    app.addedCondiments = false;

    app.hasPatty = false;
    app.hasCheese = false;
    app.hasLettuce = false;
    app.hasBun = false;
    app.hasTomato = false;
    app.hasCondiments = false;

    //set up timers
    app.timer = SWTimer_construct(TITLE_SCREEN_WAIT);
    app.gamePlayTimer = SWTimer_construct(GAME_PLAY_TIME);
    app.secondCounter = SWTimer_construct(SECOND_COUNT);
    SWTimer_start(&app.timer);

    //set up intial game state
    app.gameState = TITLE_SCREEN;

    app.up = false;
    app.down = false;
    app.right = false;
    app.left = false;
    app.upleft = false;
    app.upright = false;
    app.middle = false;
    return app;
}

/**
 * The main loop where the majority of the program is run.
 *
 * contains a FSM of all of the different states that can be achieved.
 * State can be modified within the handle method related to each state
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 */
void Application_loop(Application *app_p, HAL *hal_p)
{

    switch (app_p->gameState)
    {
    case TITLE_SCREEN:

        HandleTitleScreen(app_p, hal_p);
        break;

    case MAIN_MENU:

        HandleMainMenu(app_p, hal_p);

        break;
    case INSTRUCTIONS:

        HandleInstructionsAndScores(app_p, hal_p);

        break;

    case TOP_SCORE:

        HandleInstructionsAndScores(app_p, hal_p);

        break;

    case GAMEPLAY:
        HandleGamePlayArea(app_p, hal_p, &hal_p->gfx);
        break;

    case GAMEOVER:

        HandleGameOver(app_p, hal_p, &hal_p->gfx);
        break;

    default:
        break;
    }
}

/**
 * Displays the base graphics for the title screen by calling
 * Graphics_drawImage which draws the Title screen art, and also using
 * GFX_print to add the creator name (Illa Rochez)
 *
 * @param gfx_p is a pointer to the graphics interface being used
 */
void showTitleScreen(GFX *gfx_p)
{

    Graphics_drawImage(gfx_p, &title_screen_art8BPP_UNCOMP, 0, 0); //adds title screen

    GFX_setForeground(gfx_p, gfx_p->defaultBackground);
    GFX_print(gfx_p, "By Illa Rochez", 3, 7); //adds creator name

    GFX_setForeground(gfx_p, gfx_p->defaultForeground);

}

/**
 * This method controls the Title screen state by checking for when the main timer
 * has expired (ie 3 seconds has passed)
 *
 * after the timer has expired the state is updated and new graphics are displayed
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 */
void HandleTitleScreen(Application *app_p, HAL *hal_p)
{

    if (SWTimer_expired(&app_p->timer))
    {
        app_p->gameState = MAIN_MENU;
        ShowMainMenu(app_p, &hal_p->gfx);

    }
}

/**
 * ShowMainMenu clears the previous graphics using GFX_clear and
 * then uses drawImage to draw in some imported graphics. GFX_print
 * is also used to display text.
 *
 * @param app_p is a pointer to the application being used
 * @param gfx_p is a pointer to the graphics interface being used
 */
void ShowMainMenu(Application *app_p, GFX *gfx_p)
{
    GFX_clear(gfx_p);

    // draw in decorations
    Graphics_drawImage(gfx_p, &menu_lower_border8BPP_UNCOMP, 0, 110);
    Graphics_drawImage(gfx_p, &menu_lower_border8BPP_UNCOMP, 0, 0);

    //general text setup
    GFX_print(gfx_p, "MAIN MENU", 3, 6);
    GFX_print(gfx_p, "_________", 4, 6);
    GFX_print(gfx_p, "PLAY GAME", 8, 6);
    GFX_print(gfx_p, "HIGH SCORES", 10, 5);
    GFX_print(gfx_p, "HOW TO PLAY", 6, 5);

    //draw in cursor
    GFX_print(gfx_p, ">", app_p->cursorYcoord, app_p->cursorXcoord);
}

/**
 * Handle main menu is the looping function when the state is MainMenu
 *
 * This method checks the current cursorState and if the boosterpackJS is Tapped,
 * the current cursorState will be used to determine which state to switch the main FSM
 * to and what to update the state to.
 *
 * Additionally, this method redraws the position of the cursor based on if the joystick
 * is tapped up or tapped down.
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 */
void HandleMainMenu(Application *app_p, HAL *hal_p)
{

    _cursorState currentSelection = app_p->cursorState;

    if (Button_isTapped(&hal_p->boosterpackJS))
    {
        switch (currentSelection)
        {
        case AT_HOW_TO_PLAY:
            app_p->gameState = INSTRUCTIONS;
            ShowInstructions(app_p, &hal_p->gfx);
            break;
        case AT_PLAY_GAME:
            app_p->gameState = GAMEPLAY;
            ShowGamePlayArea(app_p, &hal_p->gfx);
            GenerateRandomTicket(app_p, hal_p);
            SWTimer_start(&app_p->gamePlayTimer);
            SWTimer_start(&app_p->secondCounter);
            break;
        case AT_TOP_SCORE:
            app_p->gameState = TOP_SCORE;
            ShowHighScores(app_p, &hal_p->gfx);
            break;

        }
    }

    if (Joystick_isTappedDown(&hal_p->joystick)) //Joystick_isTappedDown(&hal_p->joystick
    {
        redrawCursor(app_p, &hal_p->gfx, DOWN);

    }

    if (Joystick_isTappedUp(&hal_p->joystick))
    {
        redrawCursor(app_p, &hal_p->gfx, UP);

    }
}

/**
 * redrawCursor is a small FSM that will update the cursor location
 * based on the current state and what direction has been passed to the method
 * this is done by modifying cursorXcord and cursorYcoord (values for gfx calls only)
 * The current state is the main switch, and within each case there is an if-else statement
 * that determines if the directToMove is UP or DOWN
 *
 * @param app_p is a pointer to the application being used
 * @param gfx_p is a pointer to the graphics interface being used
 * @param directToMove is a enum that determines what direction the cursor will move
 *
 *
 */
void redrawCursor(Application *app_p, GFX *gfx_p,
                  _directToMoveCursor directToMove)
{

    _cursorState currentState = app_p->cursorState;

    if (app_p->gameState == MAIN_MENU)
    {
        //remove previous cursor
        GFX_setForeground(gfx_p, gfx_p->defaultBackground);
        GFX_print(gfx_p, ">", app_p->cursorYcoord, app_p->cursorXcoord);
        GFX_setForeground(gfx_p, gfx_p->defaultForeground);

        switch (currentState)
        {
        case AT_HOW_TO_PLAY:
            if (directToMove == DOWN)
            {
                app_p->cursorYcoord = 8;
                currentState = AT_PLAY_GAME;
            }

            else if (directToMove == UP)
            {

                app_p->cursorYcoord = 10;
                currentState = AT_TOP_SCORE;
            }
            break;
        case AT_PLAY_GAME:
            if (directToMove == UP)
            {
                app_p->cursorYcoord = 6;
                currentState = AT_HOW_TO_PLAY;
            }

            else if (directToMove == DOWN)
            {

                app_p->cursorYcoord = 10;
                currentState = AT_TOP_SCORE;

            }
            break;

        case AT_TOP_SCORE:
            if (directToMove == UP)
            {
                app_p->cursorYcoord = 8;
                currentState = AT_PLAY_GAME;
            }

            else if (directToMove == DOWN)
            {

                app_p->cursorYcoord = 6;
                currentState = AT_HOW_TO_PLAY;
            }
            break;
        }

        app_p->cursorState = currentState;
        GFX_print(gfx_p, ">", app_p->cursorYcoord, app_p->cursorXcoord);
    }
}
/**
 * This method updates the display using drawImage and GFX_print
 * to display graphics as well as gameplay instructions
 *
 * @param app_p is a pointer to the application being used
 * @param gfx_p is a pointer to the graphics interface being used
 */
void ShowInstructions(Application *app_p, GFX *gfx_p)
{
    GFX_clear(gfx_p);
    Graphics_drawImage(gfx_p, &menu_upper_border8BPP_UNCOMP, 100, 3);
    GFX_print(gfx_p, "INSTRUCTIONS", 1, 3);
    GFX_print(gfx_p, "____________", 2, 3);
    GFX_print(gfx_p, "Move joystick towards", 3, 0);
    GFX_print(gfx_p, "an ingredient to", 4, 0);
    GFX_print(gfx_p, "highlight it.", 5, 0);
    GFX_print(gfx_p, "Press BoostS1 to add", 7, 0);
    GFX_print(gfx_p, "one serving to dish.", 8, 0);
    GFX_print(gfx_p, "Press BoostS2 to", 9, 0);
    GFX_print(gfx_p, "reset dish", 10, 0);
    GFX_print(gfx_p, "Press joystick to", 11, 0);
    GFX_print(gfx_p, "serve dish.", 12, 0);
    GFX_print(gfx_p, "Follow the recipe", 14, 0);
    GFX_print(gfx_p, "ticket carefully!", 15, 0);
}

/**
 * This method is used to display the top 3 player scores
 * drawImage is used to add decorative elements and GFX_print is used
 * to display the decorative text. the PrintInt method is used to
 * convert the stored values topScoreNum1 - topScoreNum3 into strings
 * and to display them
 *
 * @param app_p is a pointer to the application being used
 * @param gfx_p is a pointer to the graphics interface being used
 */
void ShowHighScores(Application *app_p, GFX *gfx_p)
{
    GFX_clear(gfx_p);
    Graphics_drawImage(gfx_p, &menu_lower_border8BPP_UNCOMP, 0, 110);

    Graphics_drawImage(gfx_p, &menu_upper_border8BPP_UNCOMP, 5, 5);
    Graphics_drawImage(gfx_p, &menu_upper_border8BPP_UNCOMP, 100, 5);

    GFX_print(gfx_p, "HIGH SCORES", 1, 5);
    GFX_print(gfx_p, "___________", 2, 5);
    GFX_print(gfx_p, "1st: $ ", 5, 6);
    PrintInt(gfx_p, "1st: $ ", app_p->topScoreNum1, 5, 6);

    GFX_print(gfx_p, "2nd: $ ", 8, 6);
    PrintInt(gfx_p, "2nd: $ ", app_p->topScoreNum2, 8, 6);

    GFX_print(gfx_p, "3rd: $ ", 11, 6);
    PrintInt(gfx_p, "3rd: $ ", app_p->topScoreNum3, 11, 6);
}

/**
 * This method ensures that if the user is on the Instruction or the
 * High score screen, that if they tap boosterpackJS the gamestate is
 * updated and the main menu display is shown
 *
 * @param app_p is a pointer to the application being used
 * @param gfx_p is a pointer to the graphics interface being used
 */
void HandleInstructionsAndScores(Application *app_p, HAL *hal_p)
{
    if (Button_isTapped(&hal_p->boosterpackJS))
    {
        app_p->gameState = MAIN_MENU;
        ShowMainMenu(app_p, &hal_p->gfx);

    }
}

/**
 * This method displays the game Play area by calling GFX methods and
 * drawImage to display the play area and it's related text.
 *
 * PrintInt and PrintDoubleDigit are also used to convert numeric values
 * into strings
 *
 * @param app_p is a pointer to the application being used
 * @param gfx_p is a pointer to the graphics interface being used
 */
void ShowGamePlayArea(Application *app_p, GFX *gfx_p)
{
    //clear previous screen
    GFX_clear(gfx_p);
    GFX_setForeground(gfx_p, GRAPHICS_COLOR_CHOCOLATE);
    GFX_drawSolidRectangle(gfx_p, 0, 0, 128, 16);
    GFX_setForeground(gfx_p, gfx_p->defaultForeground);
    //draw the boxes containing items (base boxes)
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1, 40, 40); //patty
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2, 40, 40); //lettuce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2, 40, 40); //cheese

    //additonal boxes
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1, 40, 40); //tomato
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1, 40, 40); //sauce

    //draw highlighted square?
    GFX_setForeground(gfx_p, GRAPHICS_COLOR_LIME_GREEN);
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2, 40, 40); //bun
    GFX_setForeground(gfx_p, gfx_p->defaultForeground);

    //draw the images
    Graphics_drawImage(gfx_p, &patty_art4BPP_UNCOMP, 55, 30);
    Graphics_drawImage(gfx_p, &lettuce_art4BPP_UNCOMP, 13, 70);
    Graphics_drawImage(gfx_p, &cheese_art4BPP_UNCOMP, 95, 72);
    Graphics_drawImage(gfx_p, &tomato_art4BPP_UNCOMP, 13, 30);
    Graphics_drawImage(gfx_p, &condiments_art4BPP_UNCOMP, 95, 30);
    Graphics_drawImage(gfx_p, &bun_art4BPP_UNCOMP, 55, 72);

    //draw the text
    GFX_print(gfx_p, "$", 1, 1);
    PrintInt(gfx_p, "$", app_p->currentPay, 1, 1);
    GFX_print(gfx_p, "0:", 1, 9);
    PrintDoubleDigit(gfx_p, "0:", app_p->numOfSecLeft, 1, 9);
    GFX_print(gfx_p, "Spinch", 8, 1);
    GFX_print(gfx_p, " Cheese", 8, 14);
    GFX_print(gfx_p, " Pattty", 3, 7);
    GFX_print(gfx_p, "Tomato", 3, 1);
    GFX_print(gfx_p, " Soauce", 3, 14);
    GFX_print(gfx_p, " Bunnnn", 8, 7);

}

/**
 * This method handles the looping functions when the gameState is equal to
 * GAMEPLAY.

 * the method checks to see if the gamePlayTime is expired and if it is it will
 * change the state and update the display using ShowGameOver();
 *
 * The method also updates the display to show what "food items" the user has
 * highlighted and selected using GamePlayHighlingOperations and GamePlaySelectionOptions.
 *
 * If the user taps boosterpackJS the saved selections from the user are compared
 * to the current Order and the display is updated accordingly
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 * @param gfx_p is a pointer to the graphics interface being used
 */
void HandleGamePlayArea(Application *app_p, HAL *hal_p, GFX *gfx_p)
{

    if (SWTimer_expired(&app_p->gamePlayTimer))
    {
        ClearCurrentOrder(app_p, hal_p, gfx_p);
        app_p->gameState = GAMEOVER;
        ShowGameOver(app_p, gfx_p);
    }
    if (app_p->gameState == GAMEPLAY)
    {

        if (SWTimer_expired(&app_p->secondCounter))
        {
            //update display
            GFX_setForeground(gfx_p, GRAPHICS_COLOR_CHOCOLATE);
            PrintDoubleDigit(gfx_p, "0:", app_p->numOfSecLeft, 1, 9);
            GFX_setForeground(gfx_p, gfx_p->defaultForeground);
            app_p->numOfSecLeft--;
            PrintDoubleDigit(gfx_p, "0:", app_p->numOfSecLeft, 1, 9);

            //restart timer
            SWTimer_start(&app_p->secondCounter);

        }
        GamePlayHighlightingOperations(app_p, gfx_p, hal_p);
        GamePlaySelectionOperations(app_p, gfx_p, hal_p);

        if (Button_isTapped(&hal_p->boosterpackJS))
        {
            compareTicketToOrder(app_p, gfx_p, hal_p);
        }

    }

}

/**
 * This method updates the display for GAMEPLAY by drawing a lime green square
 * around the box that is related to the direction that the joystick is pressed
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 * @param gfx_p is a pointer to the graphics interface being used
 */
void GamePlayHighlightingOperations(Application *app_p, GFX *gfx_p, HAL *hal_p)
{
    //up left
    if (Joystick_isPressedUp(&hal_p->joystick)
            && Joystick_isPressedToLeft(&hal_p->joystick))
    {
        if (app_p->upleft == false){
            makeAllFalse(app_p);
            app_p->upleft = true;
            makeWhiteBoxUpLeft(gfx_p);
        }
    }
    //up right
    else if (Joystick_isPressedUp(&hal_p->joystick)
            && Joystick_isPressedToRight(&hal_p->joystick)){
        if (app_p->upright == false) {
            makeAllFalse(app_p);
            app_p->upright = true;
            makeWhiteBoxUpRight(gfx_p);
        }
    }
    //up
    else if (Joystick_isPressedUp(&hal_p->joystick)
            && !(Joystick_isPressedToLeft(&hal_p->joystick)
                    || Joystick_isPressedToRight(&hal_p->joystick))){
        if (app_p->up == false){
            makeAllFalse(app_p);
            app_p->up = true;
            makeWhiteBoxUp(gfx_p);
        }
    }
    //left
    else if (Joystick_isPressedToLeft(&hal_p->joystick)
            && !Joystick_isPressedUp(&hal_p->joystick)){

        if (app_p->left == false){
            makeAllFalse(app_p);
            app_p->left = true;
            makeWhiteBoxLeft(gfx_p);
        }
    }
    //right
    else if (Joystick_isPressedToRight(&hal_p->joystick)
            && !Joystick_isPressedUp(&hal_p->joystick)){

        if (app_p->right == false){

            makeAllFalse(app_p);
            app_p->right = true;
            makeWhiteBoxRight(gfx_p);
        }

    }
    //middle
    else if (Joystick_isPressedDown(&hal_p->joystick)){

        if (app_p->middle == false){

            makeAllFalse(app_p);
            app_p->middle = true;
            makeWhiteBoxRest(gfx_p);
        }
    }

    //no selection
    else{
        makeAllFalse(app_p);
        makeWhiteBox(gfx_p);
    }
}

void makeAllFalse(Application *app_p)
{
    app_p->up = false;
    app_p->down = false;
    app_p->right = false;
    app_p->left = false;
    app_p->upleft = false;
    app_p->upright = false;
    app_p->middle = false;
}
/**
 * This method makes it easier to highlight the display by drawing
 * all of the white boxes on the display and the highlighted green one
 *
 * @param gfx_p is a pointer to the graphics interface being used
 */
void makeWhiteBox(GFX *gfx_p)
{
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1, 40, 40); //patty
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2, 40, 40); //lettuce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2, 40, 40); //cheese
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1, 40, 40); //tomato
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1, 40, 40); //sauce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2, 40, 40); //bun
}

/**
 * This method makes it easier to highlight the display by drawing
 * all of the white boxes on the display and the highlighted green one
 *
 * @param gfx_p is a pointer to the graphics interface being used
 */
void makeWhiteBoxUp(GFX *gfx_p)
{
    GFX_setForeground(gfx_p, GRAPHICS_COLOR_LIME_GREEN);
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1, 40, 40);
    //patty
    GFX_setForeground(gfx_p, gfx_p->defaultForeground);

    //GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1, 40, 40); //patty
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2, 40, 40); //lettuce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2, 40, 40); //cheese
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1, 40, 40); //tomato
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1, 40, 40); //sauce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2, 40, 40); //bun
}

/**
 * This method makes it easier to highlight the display by drawing
 * all of the white boxes on the display and the highlighted green one
 *
 * @param gfx_p is a pointer to the graphics interface being used
 */
void makeWhiteBoxLeft(GFX *gfx_p)
{
    GFX_setForeground(gfx_p, GRAPHICS_COLOR_LIME_GREEN);
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2, 40, 40); //lettuce
    GFX_setForeground(gfx_p, gfx_p->defaultForeground);

    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1, 40, 40); //patty
    //GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2, 40, 40); //lettuce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2, 40, 40); //cheese
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1, 40, 40); //tomato
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1, 40, 40); //sauce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2, 40, 40); //bun
}

/**
 * This method makes it easier to highlight the display by drawing
 * all of the white boxes on the display and the highlighted green one
 *
 * @param gfx_p is a pointer to the graphics interface being used
 */
void makeWhiteBoxRight(GFX *gfx_p) //problem child
{
    GFX_setForeground(gfx_p, GRAPHICS_COLOR_LIME_GREEN);
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2, 40, 40); //cheese
    GFX_setForeground(gfx_p, gfx_p->defaultForeground);

    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1, 40, 40); //patty
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2, 40, 40); //lettuce
    //GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2, 40, 40); //cheese
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1, 40, 40); //tomato
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1, 40, 40); //sauce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2, 40, 40); //bun
}

/**
 * This method makes it easier to highlight the display by drawing
 * all of the white boxes on the display and the highlighted green one
 *
 * @param gfx_p is a pointer to the graphics interface being used
 */
void makeWhiteBoxUpLeft(GFX *gfx_p)
{
    GFX_setForeground(gfx_p, GRAPHICS_COLOR_LIME_GREEN);
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1, 40, 40); //tomato
    GFX_setForeground(gfx_p, gfx_p->defaultForeground);

    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1, 40, 40); //patty
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2, 40, 40); //lettuce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2, 40, 40); //cheese
    //GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1, 40, 40); //tomato
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1, 40, 40); //sauce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2, 40, 40); //bun
}

/**
 * This method makes it easier to highlight the display by drawing
 * all of the white boxes on the display and the highlighted green one
 *
 * @param gfx_p is a pointer to the graphics interface being used
 */
void makeWhiteBoxUpRight(GFX *gfx_p)
{
    GFX_setForeground(gfx_p, GRAPHICS_COLOR_LIME_GREEN);
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1, 40, 40); //sauce
    GFX_setForeground(gfx_p, gfx_p->defaultForeground);

    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1, 40, 40); //patty
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2, 40, 40); //lettuce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2, 40, 40); //cheese
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1, 40, 40); //tomato
    //GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1, 40, 40); //sauce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2, 40, 40); //bun
}

/**
 * This method makes it easier to highlight the display by drawing
 * all of the white boxes on the display and the highlighted green one
 *
 * @param gfx_p is a pointer to the graphics interface being used
 */
void makeWhiteBoxRest(GFX *gfx_p)
{
    GFX_setForeground(gfx_p, GRAPHICS_COLOR_LIME_GREEN);
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2, 40, 40); //bun
    GFX_setForeground(gfx_p, gfx_p->defaultForeground);

    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1, 40, 40); //patty
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2, 40, 40); //lettuce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2, 40, 40); //cheese
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1, 40, 40); //tomato
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1, 40, 40); //sauce
    //GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2, 40, 40); //bun
}

/**
 * This method checks which button is pushed and then uses the updateAddedItems
 * method to update the display to show the selection (Booster S1) or de-selection (booster S2)
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 * @param gfx_p is a pointer to the graphics interface being used
 */
void GamePlaySelectionOperations(Application *app_p, GFX *gfx_p, HAL *hal_p)
{

    if (Button_isPressed(&hal_p->boosterpackS1))
    {
        updateAddedItems(app_p, hal_p, gfx_p, true);
    }

    else if (Button_isPressed(&hal_p->boosterpackS2))
    {
        GFX_setForeground(gfx_p, gfx_p->defaultBackground);
        updateAddedItems(app_p, hal_p, gfx_p, false);
        GFX_setForeground(gfx_p, gfx_p->defaultForeground);

    }

}

/**
 *  This method updates the display by drawing either a white rectangle
 *  to show a selected object or to draw a black rectangle to de-select an
 *  object. The added items are then updated based on the parameter addOrRemove
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 * @param gfx_p is a pointer to the graphics interface being used
 * @param addOrRemove is used to change whether the referenced food item (determined by joystick direct) is added or removed
 */
void updateAddedItems(Application *app_p, HAL *hal_p, GFX *gfx_p,
                      bool addOrRemove)
{

    if (Joystick_isPressedUp(&hal_p->joystick)
            && !(Joystick_isPressedToLeft(&hal_p->joystick)
                    || Joystick_isPressedToRight(&hal_p->joystick))) //up
    {
        app_p->addedPatty = addOrRemove;
        GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1 + 30, 40,
                               10);
    }

    //left
    else if (Joystick_isPressedToLeft(&hal_p->joystick)
            && !Joystick_isPressedUp(&hal_p->joystick))
    {
        app_p->addedLettuce = addOrRemove;
        GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2 + 30, 40,
                               10);
    }

    //right
    else if (Joystick_isPressedToRight(&hal_p->joystick)
            && !Joystick_isPressedUp(&hal_p->joystick))
    {
        app_p->addedCheese = addOrRemove;
        GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2 + 30, 40,
                               10);
    }
    //up left)
    else if (Joystick_isPressedUp(&hal_p->joystick)
            && Joystick_isPressedToLeft(&hal_p->joystick))
    {
        app_p->addedTomato = addOrRemove;
        GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1 + 30, 40,
                               10);
    }

    //up right
    else if (Joystick_isPressedUp(&hal_p->joystick)
            && Joystick_isPressedToRight(&hal_p->joystick))
    {
        app_p->addedCondiments = addOrRemove;
        GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1 + 30, 40,
                               10);
    }

    //mid
    else if (Joystick_isPressedDown(&hal_p->joystick))
    {
        app_p->addedBun = addOrRemove;
        GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2 + 30, 40,
                               10);
    }
}

/**
 * This method is called when the user pushes BoosterJS.
 *
 * This method determines if the user inputs for added ingredients
 * match the order ticket ingredients. If they match, the pay is incremented by PAY_RAISE
 * A new Ticket is then generated after the current order is cleared
 *
 * IF they do not match, the current order is cleared, a new ticket is generated and
 * the number of strikes is updated.
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 * @param gfx_p is a pointer to the graphics interface being used
 */
void compareTicketToOrder(Application *app_p, GFX *gfx_p, HAL *hal_p)
{

    if (app_p->hasPatty == app_p->addedPatty
            && app_p->hasCheese == app_p->addedCheese
            && app_p->hasLettuce == app_p->addedLettuce
            && app_p->hasBun == app_p->addedBun
            && app_p->hasTomato == app_p->addedTomato
            && app_p->hasCondiments == app_p->addedCondiments)
    {
        GFX_setForeground(gfx_p, gfx_p->defaultBackground);
        PrintInt(gfx_p, "$", app_p->currentPay, 1, 1);
        app_p->currentPay += PAY_RAISE;
        GFX_setForeground(gfx_p, gfx_p->defaultForeground);
        PrintInt(gfx_p, "$", app_p->currentPay, 1, 1);
        ClearCurrentOrder(app_p, hal_p, gfx_p);
        GenerateRandomTicket(app_p, hal_p);

    }

    else
    {
        ClearCurrentOrder(app_p, hal_p, gfx_p); //can not call after due to possible change of state by calling UpdateStrikes()
        GenerateRandomTicket(app_p, hal_p);
        UpdateStrikes(app_p, hal_p, gfx_p);

    }

}

/**
 *  this method randomly selects a ticket by setting srand to
 *  Reference the joystick x position. The selected recipe is then
 *  displayed using displayRandomTicket() and the recipe is stored
 *  in the relevant app_p values
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 */
void GenerateRandomTicket(Application *app_p, HAL *hal_p)
{
    GFX_setForeground(&hal_p->gfx, GRAPHICS_COLOR_CHOCOLATE);
    GFX_drawSolidRectangle(&hal_p->gfx, 0, 103, 128, 30);
    GFX_setForeground(&hal_p->gfx, hal_p->gfx.defaultForeground);
    //set srand with the joystick x value
    srand(hal_p->joystick.x);
    int randomRecipe = rand() % 8;
    if (randomRecipe == 0){

         makeAllOrderTrue(app_p);
        //DisplayTicket Info Title: Fully Involved
        displayRandomTicket(app_p, &hal_p->gfx, "Fully Involved","Patty, Cheese, Spinch", "Tomato, Bunnn, Soauce");
    }
    else if (randomRecipe == 1){
        //DisplayTicket Info Title: The Vegetarian
        makeAllOrderTrue(app_p);
        app_p->hasPatty = false;
        app_p->hasCondiments = false;
        displayRandomTicket(app_p, &hal_p->gfx, "The Vegetarian","Cheese, Spinch, Bunnn", "Tomato");
    }

    else if (randomRecipe == 2){
        //DisplayTicket Info Title: The Salad
        makeAllOrderFalse(app_p);
        app_p->hasCheese = true;
        app_p->hasLettuce = true;
        app_p->hasTomato = true;
        displayRandomTicket(app_p, &hal_p->gfx, "The Salad","Cheese, Spinch, ", "Tomato");
    }

    else if (randomRecipe == 3){
        //DisplayTicket Info Title: Manly Cheesy Burg
        makeAllOrderTrue(app_p);
        app_p->hasTomato = false;
        app_p->hasLettuce = false;
        displayRandomTicket(app_p, &hal_p->gfx, "Manly Cheesy Burg", "Cheese, Patty, Bunnn", "Soauce");
    }

    else if (randomRecipe == 4)
    {
        //DisplayTicket Info Title: Basic Boi
        makeAllOrderFalse(app_p);
        app_p->hasPatty = true;
        app_p->hasBun = true;
        displayRandomTicket(app_p, &hal_p->gfx, "Basic Boi","Patty, Bunnn", " ");
    }

    else if (randomRecipe == 5){
        //DisplayTicket Info Title: The Weirdo
        makeAllOrderFalse(app_p);
        app_p->hasCheese = true;
        app_p->hasBun = true;
        displayRandomTicket(app_p, &hal_p->gfx, "The Weirdo","Cheese, Bunnn", "");
    }

    else if (randomRecipe == 6){
        //DisplayTicket Info Title: What the Hell?
        makeAllOrderFalse(app_p);
        app_p->hasLettuce = true;
        displayRandomTicket(app_p, &hal_p->gfx, "What the Hell","Spinch", " ");
    }

    else{
        //DisplayTicket Info Title: Boring Bob
        makeAllOrderFalse(app_p);
        app_p->hasPatty = true;
        displayRandomTicket(app_p, &hal_p->gfx, "Boring Bob", "Pattty", " ");
    }
}


/**
 * makes all order booleans false
 * @param app_p is a pointer to the application being used
 */
void makeAllOrderFalse(Application *app_p){
    app_p->hasPatty = false;
    app_p->hasCheese = false;
    app_p->hasLettuce = false;
    app_p->hasTomato = false;
    app_p->hasBun = false;
    app_p->hasCondiments = false;
}

/**
 * makes all order booleans true
 * @param app_p is a pointer to the application being used
 */
void makeAllOrderTrue(Application *app_p){
    app_p->hasPatty = true;
    app_p->hasCheese = true;
    app_p->hasLettuce = true;
    app_p->hasTomato = true;
    app_p->hasBun = true;
    app_p->hasCondiments = true;
}

/**
 * This method uses GFX_print to display provided Char*
 *
 * @param app_p is a pointer to the application being used
 * @param gfx_p is a pointer to the graphics interface being used
 * @param title is the title of the recipe ticket
 * @param ingreidentsLine1 is the first line of the order
 * @param ingredientsLine2 is the second line of the order (may be null or just a space if unused)
 */
void displayRandomTicket(Application *app_p, GFX *gfx_p, char *title,
                         char *ingredientsLine1, char *ingredientsLine2)
{
    GFX_print(gfx_p, title, 13, 0);
    GFX_print(gfx_p, ingredientsLine1, 14, 0);
    GFX_print(gfx_p, ingredientsLine2, 15, 0);
}

/**
 * This method is used to clear any saved user inputs and to update the
 * display to reflect this as well
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 * @param gfx_p is a pointer to the graphics interface being used
 */
void ClearCurrentOrder(Application *app_p, HAL *hal_p, GFX *gfx_p)
{
//update user selections to show nothing is selected
    app_p->addedPatty = false;
    app_p->addedCheese = false;
    app_p->addedLettuce = false;

    //update screen so that no selection boxes are present
    GFX_setForeground(gfx_p, gfx_p->defaultBackground);
    GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1 + 30, 40, 10);
    GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1 + 30, 40, 10);
    GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1 + 30, 40, 10);
    GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2 + 30, 40, 10);
    GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2 + 30, 40, 10);
    GFX_drawSolidRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2 + 30, 40, 10);

    GFX_setForeground(gfx_p, gfx_p->defaultForeground);
    //redraw rectangles because of damage
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW1, 40, 40); //patty
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW2, 40, 40); //lettuce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW2, 40, 40); //cheese
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL1, GAMEPLAY_ROW1, 40, 40); //tomato
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL3, GAMEPLAY_ROW1, 40, 40); //sauce
    GFX_drawHollowRectangle(gfx_p, GAMEPLAY_COL2, GAMEPLAY_ROW2, 40, 40); //bun

}

/**
 * This method updates the number of strikes by incrementing it.
 * The method also checks if the number of strikes goes beyond
 * the limit of 3 strikes. If it does, the gameState is changed
 * and the gameOver screen is shown.
 *
 * Otherwise a new X is displayed on the screen
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 * @param gfx_p is a pointer to the graphics interface being used
 */
void UpdateStrikes(Application *app_p, HAL *hal_p, GFX *gfx_p)
{
    app_p->numOfStrikes++;
    if (app_p->numOfStrikes >= 3)
    {
        app_p->gameState = GAMEOVER;
        ShowGameOver(app_p, gfx_p);
    }
    else
    {
        PrintChar(gfx_p, 'X', 1, STRIKE_START_X + app_p->numOfStrikes);
    }

}

/**
 * This method displays the game over graphics. First the method updates
 * the high score, which also updates the boolean hasHighScore.
 *
 * If the player hasHighScore for this round of the game, the success display Image
 * is show using drawImage. Otherwise the fail display Image is shown.
 *
 * the final score of the user is also displayed.
 *
 * @param app_p is a pointer to the application being used
 * @param gfx_p is a pointer to the graphics interface being used
 */
void ShowGameOver(Application *app_p, GFX *gfx_p)
{

    GFX_clear(gfx_p);
    updateHighScore(app_p);

    if (app_p->hasHighScore)
    {
        //print victory screen
        Graphics_drawImage(gfx_p, &game_over_success8BPP_UNCOMP, 0, 10);
    }
    else
    {
        //print failure screen
        Graphics_drawImage(gfx_p, &game_over_fail_screen4BPP_UNCOMP, 0, 10);
    }
    //print score
    GFX_print(gfx_p, "Final Score: $", 12, 3);
    PrintInt(gfx_p, "Final Score: $", app_p->currentPay, 12, 3);
}

/**
 * This method is used as the main FSM loop for when the state is GAMEOVER.
 * If the user taps boosterpackJS the state is updated and the main menu is shown
 * again.
 *
 * This method also resets the variables in app_p related to gamePlay
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 * @param gfx_p is a pointer to the graphics interface being used
 */
void HandleGameOver(Application *app_p, HAL *hal_p, GFX *gfx_p)
{

    if (Button_isTapped(&hal_p->boosterpackJS))
    {
        resetGamePlay(app_p, hal_p);
        app_p->gameState = MAIN_MENU;
        ShowMainMenu(app_p, gfx_p);
    }
}

/**
 * This method resets the variables in app_p that are related to the GamePlay state
 * they are reinitalized to their base values.
 *
 * @param app_p is a pointer to the application being used
 * @param hal_p is a pointer to the hal file being used (contains all of the peripherals)
 */
void resetGamePlay(Application *app_p, HAL *hal_p)
{
    app_p->numOfSecLeft = 30;
    app_p->currentPay = 0;
    app_p->numOfStrikes = 0;
    app_p->hasHighScore = false;
}

/**
 * This method updates the high score values stored in app_p
 *
 * The method compares the currentPay (aka the player score) to the
 * values stored in topScoreNum1 - topScoreNum3 and if it is greater
 * than one of the values, the values are shifted accordingly and
 * the currentPay is stored in the score value.
 *
 * @param app_p is a pointer to the application being used
 */
void updateHighScore(Application *app_p)
{
    if (app_p->currentPay > app_p->topScoreNum3)
    {
        app_p->hasHighScore = true;

        if (app_p->currentPay > app_p->topScoreNum1)
        {
            app_p->topScoreNum3 = app_p->topScoreNum2;
            app_p->topScoreNum2 = app_p->topScoreNum1;
            app_p->topScoreNum1 = app_p->currentPay;

        }
        else if (app_p->currentPay > app_p->topScoreNum2)
        {
            app_p->topScoreNum3 = app_p->topScoreNum2;
            app_p->topScoreNum2 = app_p->currentPay;
        }
        else
        {
            app_p->topScoreNum3 = app_p->currentPay;
        }
    }
}
/**
 * this method converts a given number into a string using snprintf
 * and it refrences the length of the string it will be next to position it
 *
 * @param gfx_p is a pointer to the graphics interface being used
 * @param string is the character array containing the string to use in merge
 * @param toMerge is the interger to merge with the provided character array
 * @param y is the row value for the call to GFX_print
 * @param x is the column value for the call to GFX_print
 */
void PrintInt(GFX *gfx_p, char *string, int toMerge, int y, int x)
{

    int buffer = strlen(string);
    char toMergeChar[MaxLengthVal];
    snprintf(toMergeChar, MaxLengthVal, "%d", toMerge);
    GFX_print(gfx_p, toMergeChar, y, x + buffer);

}

/**
 * this method converts a given number into a string using snprintf
 * and it refrences the length of the string it will be next to position it
 *
 * @param gfx_p is a pointer to the graphics interface being used
 * @param string is the character array containing the string to use in merge
 * @param toMerge is the interger to merge with the provided character array
 * @param y is the row value for the call to GFX_print
 * @param x is the column value for the call to GFX_print
 */
void PrintDoubleDigit(GFX *gfx_p, char *string, int toMerge, int y, int x)
{
    int buffer = strlen(string);
    char toMergeChar[MaxLengthVal];
    snprintf(toMergeChar, MaxLengthVal, "%02d", toMerge);
    GFX_print(gfx_p, toMergeChar, y, x + buffer);
}

/**
 * this method converts a given char into a string using snprintf
 * and it refrences the length of the string it will be next to position it
 *
 * @param gfx_p is a pointer to the graphics interface being used
 * @param string is the character array containing the string to use in merge
 * @param toMerge is the character to merge with the provided character array
 * @param y is the row value for the call to GFX_print
 * @param x is the column value for the call to GFX_print
 */
void PrintChar(GFX *gfx_p, char toMerge, int y, int x)
{
    char toMergeChar[MaxLengthVal];
    snprintf(toMergeChar, MaxLengthVal, "%c", toMerge);
    GFX_print(gfx_p, toMergeChar, y, x);

}
