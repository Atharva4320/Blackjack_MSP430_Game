/************** ECE2049 DEMO CODE ******************/
/************** 14 May 2018   **********************/
/***************************************************/

#include <msp430.h>
#include <stdlib.h>

#include "peripherals.h"
#include "utils/debug_assert.h"

// Function Prototypes
void swDelay(char numLoops);
struct card getCard();
int getScore(struct card arr[], int size);

// Declare globals here
#define BUTTON_LEFT 0x01
#define BUTTON_RIGHT 0x02

#define LED_LEFT 0x02
#define LED_RIGHT  0x01
#define LED_NONE 0x00

#define TARGET_NUM 21

enum game_state {
    START = 0,
    CUT = 1,
    DEAL = 2,
    DISPLAY_HAND = 3,
    CHECK_SCORE = 4,
    HIT = 5,
    DEALER_TURN = 6,
    WIN = 7,
    LOSE = 8,
    PROMPT = 9,
    RESET = 10,
};

struct card {
    char suit;
    char value;
};

struct card deck[52] = {
                        {'H','A'}, {'H','2'}, {'H','3'}, {'H','4'}, {'H','5'},{'H','6'},{'H','7'},{'H','8'}, {'H','9'}, {'H','X'}, {'H','J'}, {'H','Q'}, {'H','K'},
                        {'C','A'}, {'C','2'}, {'C','3'}, {'C','4'}, {'C','5'},{'C','6'},{'C','7'},{'C','8'}, {'C','9'}, {'C','X'}, {'C','J'}, {'C','Q'}, {'C','K'},
                        {'D','A'}, {'D','2'}, {'D','3'}, {'D','4'}, {'D','5'},{'D','6'},{'D','7'},{'D','8'}, {'D','9'}, {'D','X'}, {'D','J'}, {'D','Q'}, {'D','K'},
                        {'S','A'}, {'S','2'}, {'S','3'}, {'S','4'}, {'S','5'},{'S','6'},{'S','7'},{'S','8'}, {'S','9'}, {'S','X'}, {'S','J'}, {'S','Q'}, {'S','K'},
};

struct card player[10]; // Declaring player array of 10 bytes
struct card cpu[10]; // Declaring cpu array of 10 bytes
int usedCards[52]; // To keep track of used cards
int numUsedCards = 0; // To keep track of the used cards
int hit = 0; // Flag to denote if the player chose hit
int stay = 0; // Flag to denote if the player chose stay
int ind = 1; // To keep track of the cpu index
int plrInd = 2; // To keep track of player index
int dealer_score = 0;// To keep track of the dealer score
int card_score = 0;
int no = 0; // When player doesn't want to play again
int intVal;
int i;
char cardVal;
size_t playerSize;
size_t cpuSize;
// Main
void main(void)
{
    enum game_state state = RESET; // Declaring the game state
    int input_num = 0;
    int playerScore = 0; // To check player score
    int compScore = 0; // To check computer score


    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                                 // You can then configure it properly, if desired

    // *** System initialization ***
    initLaunchpadButtons();
    initLaunchpadLeds();
    configDisplay();

    // *** Intro Screen ***

    Graphics_clearDisplay(&g_sContext); // Clear the display
    Graphics_flushBuffer(&g_sContext);

    unsigned char buffer[3];
    buffer[0] = '<'; // The first and last character of the buffer make "< >" prompt
    buffer[2] = '>';

    unsigned char cardDisp[3]; // The cards displayed will be A-B
    cardDisp[1] = '-';

    // To uncomment when ready to display the nums:
    unsigned char plyr_scoreDisp[4];
    plyr_scoreDisp[0] = '<';
    plyr_scoreDisp[1] = '0';
    plyr_scoreDisp[3] = '>';

    unsigned char comp_scoreDisp[4];
    comp_scoreDisp[0] = '<';
    comp_scoreDisp[1] = '0';
    comp_scoreDisp[3] = '>';

    while (1)    // Forever loop
    {
        switch(state)
        {
        case START:

            setLaunchpadLeds(LED_LEFT);

            Graphics_drawStringCentered(&g_sContext, "Blackjack",
                                        AUTO_STRING_LENGTH, 64, 15,
                                        OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "Please press any",
                                        AUTO_STRING_LENGTH, 65, 55,
                                        OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "below button",
                                                    AUTO_STRING_LENGTH, 65, 65,
                                                    OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);
            char button_state = readLaunchpadButtons();

            if ((button_state & BUTTON_RIGHT) || (button_state & BUTTON_LEFT)) {
                Graphics_clearDisplay(&g_sContext); // Clear the display
                setLaunchpadLeds(LED_NONE);
                state = CUT;
            }
            else {
                state = START;
            }
            break;
        case CUT:

            setLaunchpadLeds(LED_LEFT);

            Graphics_drawStringCentered(&g_sContext, "Where would you",
                                                    AUTO_STRING_LENGTH, 64, 15,
                                                    OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "like to",
                                                               AUTO_STRING_LENGTH, 64, 25,
                                                               OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "cut the deck?",
                                                               AUTO_STRING_LENGTH, 64, 35,
                                                               OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "(Please press the",
                                                               AUTO_STRING_LENGTH, 64, 55,
                                                               OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "right button)",
                                                               AUTO_STRING_LENGTH, 64, 65,
                                                               OPAQUE_TEXT);
            buffer[1] = input_num + '0';
            Graphics_drawStringCentered(&g_sContext, buffer,3, 60, 80, OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);

            setLaunchpadLeds(LED_NONE);

            state = DEAL;

            break;
        case DEAL:

            setLaunchpadLeds(LED_NONE);

            button_state = readLaunchpadButtons();

            if (button_state == 1) {
                // Initial Dealing
                Graphics_clearDisplay(&g_sContext); // Clear the display
                srand(input_num);
                int i = 0;
                for (i = 0; i < 2; i++) {
                    struct card aCard;
                    aCard = getCard();
                    player[i] = aCard;
                }
                struct card aCard = getCard();
                cpu[0] = aCard;

                state = DISPLAY_HAND;
            }
            else {
                input_num = (input_num + 1) % 10; // loop through 0-9
                state = CUT;
            }
            break;
        case DISPLAY_HAND:

            Graphics_drawStringCentered(&g_sContext, "The cards are:",
                                                                AUTO_STRING_LENGTH, 64, 15,
                                                                OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "Plyr:",
                                                                AUTO_STRING_LENGTH, 20, 35,
                                                                OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "Comp:",
                                                                AUTO_STRING_LENGTH, 85, 35,
                                                                OPAQUE_TEXT);
            size_t size_player = sizeof(player)/sizeof(player[0]);
            int j;
            for (j = 0; j < size_player; j++){
                struct card playerCard = player[j];
                cardDisp[0] = playerCard.suit;
                cardDisp[2] = playerCard.value;

                Graphics_drawStringCentered(&g_sContext, cardDisp,3, 25, (45+j*10), OPAQUE_TEXT);

            }
            size_t size_cpu = sizeof(cpu)/sizeof(cpu[0]);
            int k;
            for (k = 0; k < size_cpu; k++){
                struct card cpuCard = cpu[k];
                cardDisp[0] = cpuCard.suit;
                cardDisp[2] = cpuCard.value;

                Graphics_drawStringCentered(&g_sContext, cardDisp,3, 95, (45+k*10), OPAQUE_TEXT);
            }

            // EXTRA CREDIT: To display points

            char player_score = getScore(player,10);
            char result1 = (player_score / 10) + '0';
            char result2 = (player_score % 10) + '0';
            plyr_scoreDisp[1] = result1;
            plyr_scoreDisp[2] = result2;


            Graphics_drawStringCentered(&g_sContext, plyr_scoreDisp,4, 47, 35, OPAQUE_TEXT);

            char comp_score = getScore(cpu,10);
            char result3 = (comp_score / 10) + '0';
            char result4 = (comp_score % 10) + '0';
            comp_scoreDisp[1] = result3;
            comp_scoreDisp[2] = result4;


            setLaunchpadLeds(LED_LEFT);

            Graphics_drawStringCentered(&g_sContext, comp_scoreDisp,4, 112, 35, OPAQUE_TEXT);



            Graphics_drawStringCentered(&g_sContext, "HIT",
                                                                AUTO_STRING_LENGTH, 15, 115,
                                                                OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "STAY",
                                                                AUTO_STRING_LENGTH, 115, 115,
                                                                OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);

            button_state = readLaunchpadButtons();


            if (button_state == 1) { // When stay
                setLaunchpadLeds(LED_NONE);
                Graphics_clearDisplay(&g_sContext); // Clear the display
                stay = 1;
                setLaunchpadLeds(LED_RIGHT);
                state = DEALER_TURN;
                break;
            }

            if (stay == 1) {
                setLaunchpadLeds(LED_RIGHT);
                state = CHECK_SCORE;
            }

            if (button_state == 2) { // When hit
                Graphics_clearDisplay(&g_sContext); // Clear the display
                setLaunchpadLeds(LED_LEFT);
                hit = 1;
                state = HIT;
            }

            break;
        case CHECK_SCORE:
            setLaunchpadLeds(LED_NONE);
            playerSize = sizeof(player)/sizeof(player[0]);
            cpuSize = sizeof(cpu)/sizeof(cpu[0]);
            card_score = 0;
            playerScore = getScore(player,playerSize);
            card_score = 0;
            compScore = getScore(cpu,cpuSize);

            if ((playerScore > compScore) && (playerScore < 21)) {
                state = WIN;
            }
            else if (playerScore == 21){
                state = WIN;
            }
            else if ((compScore > 21) && (playerScore < 21)) {
                state = WIN;
            }
            else if (playerScore > 21){
                state = LOSE;
            }
            else {
                state = LOSE;
            }

            break;
        case HIT:
            setLaunchpadLeds(LED_LEFT);
            if (hit == 1) {

                hit = 0;
                struct card aCard = getCard();
                player[plrInd] = aCard;
                plrInd++;
                state = DISPLAY_HAND;
            }
            break;
        case DEALER_TURN:
            setLaunchpadLeds(LED_RIGHT);
            //dealer_score = getScore(cpu);
            while (dealer_score < 17) {
                struct card aCard = getCard();
                cpu[ind] = aCard;
                size_t deckSize = sizeof(cpu)/sizeof(cpu[0]);
                dealer_score = getScore(cpu, deckSize);
                ind++;
            }
            state = DISPLAY_HAND;

            break;
        case WIN:

            for (i = 0; i < 5; i++) {
                setLaunchpadLeds(LED_LEFT);
                swDelay(1);
                setLaunchpadLeds(LED_NONE);
                swDelay(1);
            }
            Graphics_drawStringCentered(&g_sContext, "Player Wins!!",
                                                                AUTO_STRING_LENGTH, 64, 85,
                                                                OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);
            swDelay(3);
            Graphics_clearDisplay(&g_sContext); // Clear the display
            state = PROMPT;
            break;
        case LOSE:

            for (j = 0; j < 5; j++) {
                setLaunchpadLeds(LED_RIGHT);
                swDelay(1);
                setLaunchpadLeds(LED_NONE);
                swDelay(1);
            }
            Graphics_drawStringCentered(&g_sContext, "Computer Wins!!",
                                                                AUTO_STRING_LENGTH, 64, 85,
                                                                OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);
            swDelay(3);
            Graphics_clearDisplay(&g_sContext); // Clear the display
            state = PROMPT;
            break;
        case PROMPT:
            if (no == 0){
                setLaunchpadLeds(LED_LEFT);

                Graphics_drawStringCentered(&g_sContext, "Would you like to",
                                                                    AUTO_STRING_LENGTH, 64, 25,
                                                                    OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "play again?",
                                                                    AUTO_STRING_LENGTH, 60, 35,
                                                                    OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "YES",
                                                                    AUTO_STRING_LENGTH, 15, 115,
                                                                    OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "NO",
                                                                    AUTO_STRING_LENGTH, 115, 115,
                                                                    OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);

                button_state = readLaunchpadButtons();

                if (button_state == 1) { // When player says NO
                    no = 1;
                    Graphics_clearDisplay(&g_sContext); // Clear the display
                    swDelay(3);
                    Graphics_drawStringCentered(&g_sContext, "GAME OVER",
                                                                    AUTO_STRING_LENGTH, 64, 55,
                                                                    OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    for (j = 0; j < 5; j++) {
                        setLaunchpadLeds(LED_RIGHT);
                        swDelay(1);
                        setLaunchpadLeds(LED_LEFT);
                        swDelay(1);
//                        setLaunchpadLeds(LED_NONE);
//                        swDelay(1);
                    }
                    setLaunchpadLeds(LED_NONE);
                }

                if (button_state == 2) { // When player says YES
                    Graphics_clearDisplay(&g_sContext); // Clear the display
                    //reset = 1;
                    state = RESET;
                }
            }
            break;

        case RESET:

            // resetting player array:
            for (i = 0; i < 10; i++) {
                struct card resetPlyrCard;
                resetPlyrCard.suit = 0;
                resetPlyrCard.value = 0;
                player[i] = resetPlyrCard;
            }
            // resetting computer array:
            for (i = 0; i < 10; i++){
                struct card resetCpuCard;
                resetCpuCard.suit = 0;
                resetCpuCard.value = 0;
                cpu[i] = resetCpuCard;
            }
            //resetting used cards array:
            for (i = 0; i < 52; i++) {
                usedCards[i] = 0;
            }
                numUsedCards = 0; // To keep track of the used cards
                hit = 0; // Flag to denote if the player chose hit
                stay = 0; // Flag to denote if the player chose stay
                ind = 1; // To keep track of the cpu index
                plrInd = 2; // To keep track of player index
                dealer_score = 0;// To keep track of the dealer score
                card_score = 0;

                // Resetting input number:
                input_num = 0;

                // Resetting all scores:
                playerScore = 0;
                compScore = 0;

                // Resetting player score display
                plyr_scoreDisp[1] = '0';
                plyr_scoreDisp[2] = '0';

                // Resetting computer score display:
                comp_scoreDisp[1] = '0';
                comp_scoreDisp[2] = '0';

                state = START;
            break;
        }


    }  // end while (1)
}

// Helper functions

struct card getCard() {
    int found = 0;
    int randNum = rand() % 52;
    int i;

    while (!found) {

        randNum = rand() % 52;
        int ok = 1;
        for (i = 0; i < numUsedCards; i++) {
            if (usedCards[i] == randNum) {
                ok = 0; // Found a repeated card
                break;
            }
        }
        if (ok) {
            found = 1;
        }
    }
    // randNum is not in usedCards
    usedCards[numUsedCards] = randNum;
    numUsedCards++;

    return deck[randNum];
}

int getScore(struct card arr[],int size) {

    //size_t arr_size = sizeof(arr)/sizeof(arr[0]);
    card_score = 0;
    if (card_score != 0) {
        card_score = 0;
    }
    for (i = 0; i < size; i++) {
        struct card someCard = arr[i];
        cardVal = someCard.value;
        if (cardVal > 64) {
            if (cardVal == 'A') {
                intVal = 1;
            }
            else if (cardVal == 'X'){
                intVal = 10;
            }
            else if (cardVal == 'J') {
                intVal = 11;
            }
            else if (cardVal == 'Q') {
                intVal  = 12;
            }
            else if (cardVal == 'K') {
                intVal = 13;
            }
        }
        else if (cardVal == 0){
            break;
        }
        else {
         intVal = cardVal - '0';
        }
        card_score += intVal;
    }
    return card_score;
}

void swDelay(char numLoops)
{
	// This function is a software delay. It performs
	// useless loops to waste a bit of time
	//
	// Input: numLoops = number of delay loops to execute
	// Output: none
	//
	// smj, ECE2049, 25 Aug 2013

	volatile unsigned int i,j;	// volatile to prevent removal in optimization
			                    // by compiler. Functionally this is useless code

	for (j=0; j<numLoops; j++)
    {
    	i = 50000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}
