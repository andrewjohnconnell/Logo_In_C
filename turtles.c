/*
        Turtles - Logo Revisited

        Basic amends to the historic Logo language

        a.  Extend Grammar to cover different pen colours
        b.  Extend Grammar so pen can be turned on / off
        c.  Enable a "rainbow" mode at the command line
        d.  Enable a "camouflage" mode at the command line
        -- utterly useless but rather funny
                                                                                            */


/********************************************************
 ********************************************************
 ***********                                  ***********
 ***********           TURTLES IN C           ***********
 ***********                                  ***********
 ********************************************************
 ********************************************************/


/***********************
 **** INCLUDES LIST ****
 ***********************/

/*  Libraries that should be included to ensure
    proper and appropriate function of the program.                                         */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL_render.h>
#include "sdlShorthand.h"


/********************
 ********************
 ****            ****
 **** STRUCTURES ****
 ****            ****
 ********************
 ********************/

/*  These are all possible tokens allowed by the base grammar
    We will start the numbering of this enumerated type at -50 in order
    to avoid potential conflict with return values from functions.

    FD (-50)            LT (-49)        RT (-48)        DO (-47)        VAR (-46)
    VARNUM (-45)        SET (-44)       POLISH (-43)    OP (-42)        OPENBRACE (-41)
    CLOSEBRACE (-40)    SEMICOLON (-39) FROM (-38)      TO (-37)        COLON_EQUALS(-36)
    ADD (-35)           SUBTRACT (-34)  DIVIDE (-33)    MULTIPLY (-32)

    The enumerated type will be defined as tokenType                                        */

enum tokens {
    FD = -50, LT, RT, DO, VAR,
    VARNUM, SET, POLISH, OP,
    OPENBRACE, CLOSEBRACE, SEMICOLON,
    FROM, TO, COLON_EQUALS, ADD,
    SUBTRACT, DIVIDE, MULTIPLY, PEN,
    REDINK, BLUEINK, PURPLEINK, ORANGEINK, WHITE,
    YELLOWINK, GREENINK, OFF
};

typedef enum tokens tokenType;


/*  Each token within a file will be put on a stack of predefined size
    (seems extremely unlikely anyone will ever put in a one million word
    file, so we'll set the MAX_STACK size to that).

    The token object will be formed of the following structure.

    The enumerated struct will be defined as tokenStack.                                    */

struct tokenStack {

    int loopSet;                // Which level of loop are we in? (Base level: 1)
    char* token;                // What is the token?
    tokenType thisTokenType;    // What type of token is this?
    float thisTokenValue;       // What enumerated value is this tokenType?
    int setPlace;               // Is this tied to a SET command?
    char thisTokenLetter;       // If this is a VAR, which letter is it representing?
    float* thisLetterValue;     // If it is a VAR, point to correct section of VARSTACK
    int initialToken;           // Is this the initial token?
    int examining;              // Are we examining this token?
    int varToOp;                // Is this VAR representative of an OP?

};

typedef struct tokenStack tokenStack;



/**********************************
 **********************************
 ********                  ********
 ********    DEFINITIONS   ********
 ********                  ********
 ********    ARRANGED IN   ********
 ******                      ******
 ******  ------------------  ******
 ******  ALPHABETICAL ORDER  ******
 ******  ------------------  ******
 **********************************
 **********************************/

#define ACCESSING_FILE              "Accessing file.\n"
#define ADD_DESIRED                 theFile -> thisTokenType == ADD         // Send / Check if thisTokenType in a Token is ADD
#define AFTER_DECIMAL_POINT         3

/*  Number of letters in the alphabet                   */
#define ALPHABET                    26

/*  This can be increased as necessary
    to accommodate further variables that must
    be passed around the entire program.
    ---
    Note: At present, 29 represents the number
    of letters in the alphabet (which are
    used to store VAR values), the angle
    the turtle is pointing, and the turtle's
    X and Y co-ordinates.                               */
#define ALPHABET_AND_EXTRAS         33

/*  Location of the angle value inside VARSTACK         */
#define ANGLE_LOC                   26

#define ARITHMETIC_SWITCHES         cA cS cM cD
#define BLUE                        31
#define BLUE_MOD                    11
#define BR_CHECK                    if (switchForBraces(++COMMON_INPUTS) == VALID)
#define BRACE_LEGALITY              "Was expecting a legal brace expression.\n"
#define BRACES_ISSUE                "Error in file.  Brace sets not opened and closed correctly.\n"
#define BRACES_SWITCH               cP cF cL cR cDO cST cCB
#define PEN_SWITCH                  cBL cRE cOR cPU cWH

/*  Short-hand forms to call case checks in Switches
    ------------------------------------------------    */
#define cA                          case ADD:
#define cBL                         case BLUEINK:
#define cCB                         case CLOSEBRACE:
#define cCE                         case COLON_EQUALS:
#define cDO                         case DO:
#define cF                          case FD:
#define cFR                         case FROM:
#define cD                          case DIVIDE:
#define cL                          case LT:
#define cM                          case MULTIPLY:
#define cOB                         case OPENBRACE:
#define cOR                         case ORANGEINK:
#define cR                          case RT:
#define cRE                         case REDINK:
#define cP                          case PEN:
#define cPU                         case PURPLEINK:
#define cS                          case SUBTRACT:
#define cSC                         case SEMICOLON:
#define cST                         case SET:
#define cTO                         case TO:
#define cV                          case VAR:
#define cVN                         case VARNUM:
#define cWH                         case WHITE:
/*  -------------------------------------------------   */

#define CANNOT_READ_TOKEN           "Unable to read %s from file.\n"
#define CHAR_ALPHA_A                'A'
#define CHAR_ALPHA_Z                'Z'
#define CHAR_ARRAY_SIZE             20
#define CHAR_FULLSTOP               '.'
#define CHAR_NUMERIC_ZERO           '0'
#define CHAR_NUMERIC_NINE           '9'
#define CHECK_AGAIN                 "Check your file, correct it, and try again...\n"

/*  Check if the value of a VAR being checked in a DO loop is valid (e.g. <= FROM | >= TO)
    --------------------------------------------------------------------------------------      */
#define CHECK_WITHIN_BOUNDS         (USE_VAR_VAL < setFromToTest(theFile, varStack, L_BOUND) || USE_VAR_VAL > setFromToTest(theFile, varStack, U_BOUND))

#define CLOSING_FILE                "Closing loaded file.\n"
#define COLONEQ_INVALID                 "Invalid tokens after := in file.\n"
#define COLONEQ_LEGALITY            "Was expecting a legal instruction to follow a :=.\n"
#define COLONEQ_PREC_SWITCHES       cV cVN cA cS cM cD cSC
#define COMMON_INPUTS               theFile, loopSet_p, varStack
#define D_CHECK                     if (switchForDirectionals(++COMMON_INPUTS) == VALID)
#define DECIMAL_PLACES_ISSUE        "Error in file.  You are trying to pass a number with more\nthan two decimal places.\n"
#define DELAY_AT_QUIT               9000
#define DIRECTIONAL_VALIDITY        "Was expecting a legal instruction to follow a DIRECTIONAL command.\n"
#define DIV_DESIRED                 theFile -> thisTokenType == DIVIDE
#define DO_CHECK                    if (switchForDO(++COMMON_INPUTS) == VALID)
#define DO_LEGALITY                 "Was expecting a legal instruction to follow a DO.\n"
#define END_ARRAY_CHAR              '\0'
#define ERROR_EXIT                  1
#define EXCEED_RESET                "Value exceeds bounds of %lf.\nResetting to %lf.\n"
#define EXITING                     "Exiting Hurtle the Turtle\n"
#define FIRST                       1
#define FROM_LEGALITY               "Was expecting a legal instruction to follow a FROM.\n"
#define FROM_WITHOUT_DO             "FROM without preceding DO in file.\n"
#define FULL_CIRCLE                 360
#define GRAB_VAR_SHIFT_FORWARD      variables[(*varPos_p)++]
#define GREEN                       30
#define GREEN_MOD   7
#define HALF_CIRC                   180

/*  Initialisation values for VARSTACK
    ----------------------------------              */

#define INIT_VARSTACK               { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, INIT_X, INIT_Y, 0, 0, 0, 0 };

#define INIT_X                      400
#define INIT_Y                      300
#define INVALID_ARGS                "Invalid number of arguments.\n"

/*  If returning a value that is clearly NOT a
    valid INSTRUCTION, send INVALID_IN (which is
    defined as one less than FD - and therefore,
    with a standard expansion of the INSTRUCTIONS
    we would be bug-free unless someone decided to
    very specifically label the instruction's enum
    value as -51.                                   */
#define INVALID_IN                  -51

#define INVALID_OPEN_COMM           "Invalid opening command.\n"
#define INVALID_OPENER              "Invalid Opening Character.  Expected \"{\""
#define INVALID_SET_OPS             "Invalid number of operators to operands in SET operation.\n"

/* Position of FROM VAR|VARNUM relative to DO VAR in grammar */
#define L_BOUND                     2
#define LINE_COORDS                 varStack[ALPHABET + ONE_VAR], varStack[ALPHABET + L_BOUND], varStack[ALPHABET + ONE_VAR] + xPosEnd, varStack[ALPHABET + L_BOUND] + yPosEnd
#define LOOP_INVALID                "{ is not preceded by TO --> VAR|VARNUM.\n"
#define MAX_LIMIT                   999999999.99
#define MAX_STACK                   1000000
#define MLT_DESIRED                 theFile -> thisTokenType == MULTIPLY
#define NEXT_TOKEN                  theFile + 1
#define NEXT_TOKEN_VAR_VAL          varStack[(int)(NEXT_TOKEN) -> thisTokenLetter - (int)'A']
#define NEW_XPOS                    varStack[ALPHABET + ONE_VAR] = varStack[ALPHABET + ONE_VAR] + xPosEnd;
#define NEW_YPOS                    varStack[ALPHABET + L_BOUND] = varStack[ALPHABET + L_BOUND] + yPosEnd;
#define NO_FILE                     "File does not exist.\n"
#define ONE_OP                      1
#define ONE_VAR                     1
#define OP_PREC_SWITCHES            cA cS cM cD cV cVN
#define PI                          3.14159265
#define POLISH_LEGALITY             "Was expecting a legal instruction to follow a + - * / ;.\n"
#define POLISH_SWITCHES             cA cS cM cD cSC
#define PRECEDER_REQS               theFile, loopSet_p, varStack, notValid_p, variables, operators, varPos_p, opPos_p
#define QUICK_DELAY                 20
#define RAINBOW                     32
#define RAINBOW_BASE_R              55
#define RAINBOW_BASE_G              60
#define RAINBOW_BASE_B              65
#define RAINBOW_BLUE_A              7
#define RAINBOW_GREEN_A             17
#define RAINBOW_MOD_R               3
#define RAINBOW_MOD_G               3
#define RAINBOW_MOD_B               3
#define RAINBOW_RED_A               7
#define RAINBOW_RED_INC             22
#define RED                         29
#define RED_MOD                     4
#define RGB_WHITE                   255
#define SEMICOLON_LEGALITY          "Was expecting a legal instruction to follow a SEMICOLON.\n"
#define SEMICOLON_SWITCH            cCB cDO cST cF cL cR cP
#define SEMIERROR                   "SEMICOLON placed improperly in file.\n"
#define SHORT_DELAY                 10
#define SET_LEGALITY                "Was expecting a legal instruction to follow a SET.\n"
#define SET_CHECK                   if (switchForSET(++COMMON_INPUTS) == VALID)
#define SINGLE_DECIMAL_POINT        1
#define SUB_DESIRED                 theFile -> thisTokenType == SUBTRACT
#define THREE_ARGS                  3
#define TO_LEGALITY                 "Was expecting a legal instruction to follow a TO.\n"
#define TO_RADIANS                  (PI / 180)
#define TO_WITHOUT_DO               "TO without preceding DO in file.\n"
#define TOKENIZED_PROCEEDING        "Have fully tokenized the file. \nProceeding to check grammar.\n\n"
#define U_BOUND                     4  // Position of TO VAR|VARNUM relative to DO VAR in grammar
#define UNKNOWN_TOKEN               "Unrecognised token received.\n"
#define USE_VAR_VAL                 varStack[(int)theFile -> thisTokenLetter - (int)'A']
#define VALID                       1
#define VAR_OP_ARRAY_CAPACITY       999
#define VAR_SET_ONLY_ONCE           "Using the same VAR for two separate DO calls is not allowed.\n"
#define VAR_LEGALITY                "Was expecting a legal instruction to follow a VAR.\n"
#define VAR_VARNUM_OP               "Only a VAR, VARNUM, or OPERATOR may precede an OPERATOR.\n"
#define VARS_CHECK                  if (switchForVARS(++COMMON_INPUTS) == VALID)
#define VARS_SWITCH                 cF cL cR cV cVN cA cS cM cD cSC cOB cCB cDO cFR cTO cCE cP
#define VARNUM_LEGALITY             "Was expecting a legal instruction to follow a VARNUM.\n"
#define VARNUMS_CHECK               if (switchForVARNUMS(++COMMON_INPUTS) == VALID)
#define VARNUMS_SWITCH              cF cL cR cV cVN cA cS cM cD cSC cOB cCB cDO cTO cST cP
#define VARSTACK_BACK               varStack = varStack - (((int)theFile -> thisTokenLetter) - (int)'A')
#define VARSTACK_FORWARD            varStack = varStack + (((int)theFile -> thisTokenLetter) - (int)'A')
#define XLOC                        27
#define YLOC                        28
#define ZERO                        0


/* EXTENSION: COLOUR DEFINITIONS FOR PEN INK */
#define R_RED           RGB_WHITE
#define G_RED           50
#define B_RED           50
#define R_BLUE          50
#define G_BLUE          150
#define B_BLUE          RGB_WHITE
#define R_PURP          170
#define G_PURP          20
#define B_PURP          170
#define R_ORAN          220
#define G_ORAN          110
#define B_ORAN          0
#define R_YELL          RGB_WHITE
#define G_YELL          RGB_WHITE
#define B_YELL          50
#define R_GREE          30
#define G_GREE          205
#define B_GREE          30



/*****************************
 **** FUNCTION PROTOTYPES ****
 *****************************/


/*  FOR THE INITIALISATION OF THE PROGRAM
    -------------------------------------   */
int attemptToOpen(FILE* fp, char* thisFileName, int* loopSet_p, int rainbow);
int handleArgC(int argc);
int run(char* thisFileName, int* loopSet_p, int rainbow);
int readFile(FILE* fp, int* loopSet_p, int rainbow);

void validateDO (tokenStack* theFile);
void wrapUp(FILE* fp);


/*  FOR THE TOKENIZATION OF A FILE
    ------------------------------  */
float checkVarNum(char* tokenText);

int checkLoneChars(char* tokenText);
int checkToken(char* tokenText);
int singleCharChecker(char singleChar);

void tokenizeFile (FILE *fp, tokenStack* theFile, int* loopSet_p, float* varStack);


/*  FOR THE PARSING AND INTERPRETATION OF FILES
    ------------------------------------------- */
int appropriateBoundChecking(tokenStack* theFile, float* varStack);
int ascertainCOLONEQ(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainDirectional(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainDO(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainPEN(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainFROM(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainOpenBrace(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainPOLISH(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainSEMICOLON(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainSET(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainTO(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainVAR(tokenStack* theFile, int* loopSet_p, float* varStack);
int ascertainVARNUM(tokenStack* theFile, int* loopSet_p, float* varStack);
int setFromToTest(tokenStack* theFile, float* varStack, int adjuster);
int switchForBraces(tokenStack* theFile, int* loopSet_p, float* varStack);
int switchForCOLONEQ(tokenStack* theFile, int* loopSet_p, float* varStack);
int switchForDirectionals(tokenStack* theFile, int* loopSet_p, float* varStack);
int switchForDO(tokenStack* theFile, int* loopSet_p, float* varStack);
int switchForFROM(tokenStack* theFile, int* loopSet_p, float* varStack);
int switchForPOLISH(tokenStack* theFile, int* loopSet_p, float* varStack);
int switchForSEMICOLON(tokenStack* theFile, int* loopSet_p, float* varStack);
int switchForSET(tokenStack* theFile, int* loopSet_p, float* varStack);
int switchForTO(tokenStack* theFile, int* loopSet_p, float* varStack);
int switchForVARS(tokenStack* theFile, int* loopSet_p, float* varStack);
int switchForVARNUMS(tokenStack* theFile, int* loopSet_p, float* varStack);

tokenStack* ascertainCloseBrace(tokenStack* theFile, int* loopSet_p, float* varStack, int* teotwawki_p);
tokenStack* ascertainNextValidity(tokenStack* theFile, int* loopSet_p, float* varStack, int* teotwawki_p);
tokenStack* nextVarCheck(tokenStack* theFile, int* loopSet_p, float* varStack, tokenStack* revertToHere);

void conductAllocationOfFromVal(tokenStack* theFile, float* varStack);
void fromPreceder(tokenStack* theFile, int* loopSet_p, float* varStack);
void openbracePreceder(tokenStack* theFile, int* loopSet_p, float* varStack);
void opPreceder(tokenStack* theFile, int* loopSet_p, float* varStack);
void semiColonPreceder(tokenStack* theFile, int* loopSet_p, float* varStack, int* notValid_p, float* variables, int* operators, int* varPos_p, int* opPos_p);
void toPreceder(tokenStack* theFile, int* loopSet_p, float* varStack);


/*  FOR THE TERMINAL BASED PRINTING OF TEST RESULTS DURING BUILD PHASES
    ------------------------------------------------------------------- */
void printTokenType(tokenStack* theFile);


/*  FOR THE SDL BASED DISPLAY OF GRAPHICS DURING INTERPRETATION PHASE
    -----------------------------------------------------------------   */
int displayGraphics(int* loopSet_p, float* varStack, FILE* fp, int rainbow);

void amendCol(float* varStack, int* colour, int BASE_COL, int BASE_MOD_NUM, int BASE_COL_NUM, int AMEND_COL_NUM);
void amendRainbowPens(int rainbow, float* varStack, int* red, int* green, int* blue);
void amendTheWindow (tokenStack* printToScreen, float hypLen, float yPosEnd, float xPosEnd, float cosOut, float sinOut, int* r_p, int* g_p, int* b_p, int rainbow, float* varStack, SDL_Simplewin theWindow);
void camouflageCheck (int rainbow, float* varStack);
void finalPresent (SDL_Simplewin theWindow);

/*************************
 *************************
 ******             ******
 ******  FUNCTIONS  ******
 ******             ******
 *************************
 *************************/


/*  Main function receives filename at command line;
    passes number of arguments to function to check they're
  valid: if valid, call run function.                                   */
int main(int numOfArgs, char* argStrings[]) {

    int loopSet = ZERO, rainbow = ZERO;
    int *loopSet_p = &loopSet;
    handleArgC(numOfArgs);

    if (numOfArgs == THREE_ARGS) {
        if (strcmp(argStrings[L_BOUND], "-rainbow") == ZERO) {
            rainbow = VALID;
        } else {
            if (strcmp(argStrings[L_BOUND], "-camouflage") == ZERO) {
                rainbow = L_BOUND;
            } else {
                printf(INVALID_ARGS);
                exit(ERROR_EXIT);
            }
        }
    }

    run(argStrings[FIRST], loopSet_p, rainbow);

    return ZERO;

}


/*  First, attempt to open file referenced by main's argStrings[1]
    called at command line.  If file can be found, attempt to parse it
    to a linked list.                                                   */
int run(char* thisFileName, int* loopSet_p, int rainbow) {

    FILE* fp = NULL;

    attemptToOpen(fp, thisFileName, loopSet_p, rainbow);
    fclose(fp);

    return ZERO;

}


/*  Attempt to open the file referenced in argStrings[1] when main
    is called.                                                          */
int attemptToOpen(FILE* fp, char* thisFileName, int* loopSet_p, int rainbow) {

    if (fopen(thisFileName, "r") != NULL) {
        printf(ACCESSING_FILE);
        fp = fopen(thisFileName, "r");
        readFile(fp, loopSet_p, rainbow);
    } else {
        printf(NO_FILE);
        exit(ERROR_EXIT);
    }

    return ZERO;

}


/*  Check the number of arguments entered at the command line.
    If not in line with standard ./turtles <filename>,
    return error message and exit.                                      */
int handleArgC(int argc) {

    if (argc != L_BOUND && argc != L_BOUND + ONE_OP) {
        printf(INVALID_ARGS);
        exit(ERROR_EXIT);
    }

    return ZERO;

}


/*  Read the file referenced by fp and tokenize to array as we go.
    If the file proves invalid at any point, free the memory and
    close the file / program.
    teotwawki := The End Of The World As We Know It.                    */
int readFile(FILE* fp, int* loopSet_p, int rainbow) {

    int teotwawki = ZERO, *teotwawki_p = &teotwawki;
    float varStack[ALPHABET_AND_EXTRAS] = INIT_VARSTACK

    varStack[RED] = RGB_WHITE, varStack[GREEN] = RGB_WHITE, varStack[BLUE] = RGB_WHITE;

    if (rainbow == VALID) {
        varStack[RAINBOW] = VALID;
        varStack[RED] = RAINBOW_BASE_R, varStack[GREEN] = RAINBOW_BASE_G, varStack[BLUE] = RAINBOW_BASE_B;
    }

    if (rainbow == L_BOUND) {
        varStack[RAINBOW] = L_BOUND;
        varStack[RED] = ZERO, varStack[GREEN] = ZERO, varStack[BLUE] = ZERO;
    }

    tokenStack* theFile = (tokenStack*)malloc(sizeof(tokenStack)*MAX_STACK);

    tokenizeFile(fp, theFile, loopSet_p, varStack);
    validateDO(theFile);
    while (!teotwawki) { theFile = ascertainNextValidity(theFile, loopSet_p, varStack, teotwawki_p); }
    displayGraphics(loopSet_p, varStack, fp, rainbow);
    wrapUp(fp);

    return ZERO;

}


/*  This function will ensure file is closed and program exits,
    forcibly freeing up any and all malloc'd space.                     */
void wrapUp(FILE* fp) {

    printf(CLOSING_FILE);
    fclose(fp);
    exit(ZERO);

}


/*  This function will check that no single variable has been
    used more than once in a DO call.                                   */
void validateDO (tokenStack* theFile) {

    int doCheck[ALPHABET] = {ZERO}, incrementCheckAt = ZERO;

    while (theFile -> loopSet != ZERO) {
        if (theFile -> thisTokenType == DO) {
            incrementCheckAt = (int)(NEXT_TOKEN) -> thisTokenLetter - (int)'A';

            if (doCheck[incrementCheckAt] == FIRST) {
                printf(VAR_SET_ONLY_ONCE CHECK_AGAIN EXITING);
                exit(ERROR_EXIT);
            }
            doCheck[incrementCheckAt]++;

        }
        theFile++;
    }

}


/*  This function will scan through the file that has been loaded and put
 each word (as a token) in to a separate section of the 'theFile' array. */
void putInTokens(FILE* fp, tokenStack* theFile, int* loopSet_p, float* varStack, char* initial, char* tokenText, char throwOver) {
    while (fscanf(fp, "%s", tokenText) != EOF) {


        (++theFile) -> token = tokenText;
        theFile -> thisTokenType = checkToken(tokenText);
        theFile -> initialToken = ZERO;
        if (theFile -> thisTokenType == OPENBRACE) {
            (*loopSet_p)++;
        }
        if (theFile -> thisTokenType == CLOSEBRACE) {
            (*loopSet_p)--;
        }
        if (theFile -> thisTokenType == VAR) {
            throwOver = tokenText[ZERO];
            theFile -> thisTokenLetter = throwOver;
            VARSTACK_FORWARD;
            theFile -> thisLetterValue = varStack;
            VARSTACK_BACK;
        }
        theFile -> loopSet = *loopSet_p;
        theFile -> examining = ZERO;
        theFile -> varToOp = ZERO;
        if (theFile -> thisTokenType == VARNUM) {
            theFile -> thisTokenValue = atof(tokenText);
        }


    }

}


/*  If we successfully tokenize the whole file being read in, proceed to parsing    */
void loopDecision(FILE* fp, tokenStack* theFile, int* loopSet_p) {

    if (*loopSet_p == ZERO) {
        printf(TOKENIZED_PROCEEDING);
    } else {
        printf(BRACES_ISSUE CHECK_AGAIN EXITING);
        fclose(fp);
        exit(ERROR_EXIT);
    }

}


/*  Look through each word in the file, check it against grammar, and
    tokenize it.                                                                    */
void tokenizeFile (FILE *fp, tokenStack* theFile, int* loopSet_p, float* varStack) {

    char initial[FIRST], tokenText[CHAR_ARRAY_SIZE], throwOver = END_ARRAY_CHAR;
    fscanf(fp, "%s", initial);

    if (initial[ZERO] != '{') {
        printf(INVALID_OPENER CHECK_AGAIN EXITING), fclose(fp), exit(ERROR_EXIT);
    } else {
        (*loopSet_p)++;
        theFile -> loopSet = FIRST, theFile -> token = initial, theFile -> thisTokenType = checkToken(initial);
        putInTokens(fp, theFile, loopSet_p, varStack, initial, tokenText, throwOver);
        loopDecision(fp, theFile, loopSet_p);
    }

}


/*  Check the token being looked at to see if it is a valid term
    as defined in the language's definition                         */
int checkToken(char* tokenText) {

    if (strlen(tokenText) == FIRST)                 { return checkLoneChars(tokenText); }
    if (strcmp(tokenText,"FD") == ZERO)             { return FD; }
    if (strcmp(tokenText,"LT") == ZERO)             { return LT; }
    if (strcmp(tokenText,"RT") == ZERO)             { return RT; }
    if (strcmp(tokenText,"DO") == ZERO)             { return DO; }
    if (strcmp(tokenText,"SET") == ZERO)            { return SET; }
    if (strcmp(tokenText,"FROM") == ZERO)           { return FROM; }
    if (strcmp(tokenText,"TO") == ZERO)             { return TO; }
    if (strcmp(tokenText,":=") == ZERO)             { return COLON_EQUALS; }
    if (strcmp(tokenText,"PEN") == ZERO)            { return PEN; }
    if (strcmp(tokenText,"RED") == ZERO)            { return REDINK; }
    if (strcmp(tokenText,"BLUE") == ZERO)           { return BLUEINK; }
    if (strcmp(tokenText,"PURPLE") == ZERO)         { return PURPLEINK; }
    if (strcmp(tokenText,"ORANGE") == ZERO)         { return ORANGEINK; }
    if (strcmp(tokenText,"YELLOW") == ZERO)         { return YELLOWINK; }
    if (strcmp(tokenText,"GREEN") == ZERO)          { return GREENINK; }
    if (strcmp(tokenText,"WHITE") == ZERO)          { return WHITE; }

    if (strcmp(tokenText,"OFF") == ZERO)             { return OFF; }

    if (checkVarNum(tokenText) != INVALID_IN)       { return VARNUM; }

    printf(CANNOT_READ_TOKEN CHECK_AGAIN EXITING, tokenText), exit(ERROR_EXIT);

    return INVALID_IN;
}


/*  If the token being examined is a single character, check whether it
    is a brace, operator, or semi-colon (at this point, we will already
    have checked for single letters or numbers; therefore, if token does
    not match what's on this list, return an 'invalid' flag and exit the
    program.                                                                */
int singleCharChecker(char singleChar) {

    switch((int)singleChar) {
        case ((int)'{'): return OPENBRACE;
        case ((int)'}'): return CLOSEBRACE;
        case ((int)'+'): return ADD;
        case ((int)'-'): return SUBTRACT;
        case ((int)'*'): return MULTIPLY;
        case ((int)'/'): return DIVIDE;
        case ((int)';'): return SEMICOLON;

        default: return INVALID_IN;
    }

    return INVALID_IN;
}

/*  If the token being looked at holds a string of only one character,
    check if that token is a number from 0 to 9 or a letter from A - Z.
    If it is not, check if it is a "special" character.                     */
int checkLoneChars(char* tokenText) {

    char singleChar = tokenText[ZERO];

    if (singleChar >= CHAR_NUMERIC_ZERO && singleChar <= CHAR_NUMERIC_NINE) { return VARNUM; }
    if (singleChar >= CHAR_ALPHA_A && singleChar <= CHAR_ALPHA_Z) { return VAR; }

    return singleCharChecker(singleChar);
}


/*  Check if a token begins with a number, check if it is a valid VARNUM    */
float checkVarNum(char* tokenText) {

    int atoiLen = (int)strlen(tokenText), cntFail = ZERO, decimalCnt = ZERO, afterDecimal = ZERO;

    for (int i = ZERO; i < atoiLen; i++) {
        tokenText += i;
        if ((*tokenText >= CHAR_NUMERIC_ZERO && *tokenText <= CHAR_NUMERIC_NINE) || (*tokenText == CHAR_FULLSTOP)) {
            if (*tokenText == CHAR_FULLSTOP) { decimalCnt++; }
            if (afterDecimal == AFTER_DECIMAL_POINT) {
                printf(DECIMAL_PLACES_ISSUE CHECK_AGAIN EXITING), exit(ERROR_EXIT);
            }
            if (decimalCnt == SINGLE_DECIMAL_POINT) { afterDecimal++; }
        } else {
            cntFail++;
        }
        tokenText -= i;
    }

    if (decimalCnt > SINGLE_DECIMAL_POINT) { return INVALID_IN; }
    if (cntFail == ZERO) { return VARNUM; }

    return INVALID_IN;
}


/*  If a token is an OPEN or CLOSE BRACE, return a valid signal  */
int switchForBraces(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch (theFile -> thisTokenType) {
        BRACES_SWITCH
            return VALID;
            break;
        default:
            printf(BRACE_LEGALITY CHECK_AGAIN EXITING);
            exit(ERROR_EXIT);
            break;
    }

}



/*  If a token is a VAR, return a valid signal  */
int switchForVARS(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch(theFile -> thisTokenType) {
            VARS_SWITCH
            return VALID;
            break;
        default:
            printf(VAR_LEGALITY CHECK_AGAIN EXITING);
            exit(ERROR_EXIT);
            break;
    }

}


/*  Valid switch calls for tokens that may follow a VARNUM.
    Note: THIS IS IDENTICAL TO switchForVAR WITH THE EXCEPTION
    OF "FROM" ----> THIS IS AN IMPERMISSABLE SWITCH FOR VARNUM. */
int switchForVARNUMS(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch(theFile -> thisTokenType) {
        VARNUMS_SWITCH
            return VALID;
            break;
        default:
            printf(VARNUM_LEGALITY CHECK_AGAIN EXITING);
            exit(ERROR_EXIT);
            break;
    }

}


/*  If the turtle turns left or right, feed the correct NEW angle
    to the appropriate place in the VARSTACK                        */
void sortAngle(tokenStack* theFile, float* varStack, float amendBy) {

    if ((theFile - ONE_VAR) -> thisTokenType == LT) {
        varStack[ANGLE_LOC] = varStack[ANGLE_LOC] + (FULL_CIRCLE - amendBy);
    }
    if ((theFile - ONE_VAR) -> thisTokenType == RT) {
        varStack[ANGLE_LOC] = varStack[ANGLE_LOC] + amendBy;
    }
    varStack[ANGLE_LOC] = (int)varStack[ANGLE_LOC]%FULL_CIRCLE;

}


/*  Where a token is a direction (FD, LT, RT), return a valid signal
    based on the value following that token (VAR | VARNUM)              */
int switchForDirectionals(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch (theFile -> thisTokenType) {
        case VAR:
            sortAngle(theFile, varStack, USE_VAR_VAL);
            return VALID;
        case VARNUM:
            sortAngle(theFile, varStack, theFile -> thisTokenValue);
            return VALID;
        default:
            printf(DIRECTIONAL_VALIDITY CHECK_AGAIN EXITING), exit(ERROR_EXIT);
    }
    return INVALID_IN;

}


/*  Check the token following a DO token is a VAR - if so, return valid signal  */
int switchForDO(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch (theFile -> thisTokenType) {
        case VAR: return VALID;
            break;
        default:
            printf(DO_LEGALITY CHECK_AGAIN EXITING), exit(ERROR_EXIT);
    }

}


/*  Check the token following a FROM token.  If it is a VAR | VARNUM then
    return a valid signal                                                       */
int switchForFROM(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch (theFile -> thisTokenType) {
        case VAR: case VARNUM: return VALID;
            break;
        default:
            printf(FROM_LEGALITY CHECK_AGAIN EXITING), exit(ERROR_EXIT);
    }

}


/*  If the token being examined is TO, check that the next token is a VAR | VARNUM,
    if so - return a valid signal.                                                  */
int switchForTO(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch (theFile -> thisTokenType) {
        case VAR: case VARNUM:
            return VALID;
            break;
        default:
            printf(TO_LEGALITY CHECK_AGAIN EXITING), exit(ERROR_EXIT);
    }

}


/*  If the token being examined is a SET, ensure that the next token is a VAR -
    if it is, return a valid signal                                                 */
int switchForSET(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch (theFile -> thisTokenType) {
        case VAR: return VALID;
            break;
        default:
            printf(SET_LEGALITY CHECK_AGAIN EXITING), exit(ERROR_EXIT);
    }

}


/*  If a semi-colon is found, ensure that the preceding commands form a valid
    grammatical clause                                                              */
int switchForSEMICOLON(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch (theFile -> thisTokenType) {
        SEMICOLON_SWITCH return VALID;
            break;
        default:
            printf(SEMICOLON_LEGALITY CHECK_AGAIN EXITING), exit(ERROR_EXIT);
    }

}


/*  If the token being examined is a COLON_EQUALS, then ensure that the next
    token is a VAR | VARNUM.  If so, return a valid signal.                         */
int switchForCOLONEQ(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch (theFile -> thisTokenType) {
        case VAR: case VARNUM: return VALID;
            break;
        default:
            printf(COLONEQ_LEGALITY CHECK_AGAIN EXITING), exit(ERROR_EXIT);
    }

}


/*  If the token being examined is an OPERATOR, ensure that it sits as part of
    a valid calculation.  If so, return a valid signal.                             */
int switchForPOLISH(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch (theFile -> thisTokenType) {
            POLISH_SWITCHES return VALID;
            break;
        default:
            printf(POLISH_LEGALITY CHECK_AGAIN EXITING), exit(ERROR_EXIT);
    }

}


/*  This function calls all functions designed to ascertain L -> R validity
    of tokens.                                                                      */
tokenStack* ascertainNextValidity(tokenStack* theFile, int* loopSet_p, float* varStack, int* teotwawki_p) {

    if(ascertainPOLISH(COMMON_INPUTS) == VALID) {
        opPreceder(COMMON_INPUTS);
        if (switchForPOLISH(++COMMON_INPUTS) == VALID) { return theFile; } }

    if(ascertainSET(COMMON_INPUTS) == VALID) { SET_CHECK { return theFile; } }
    if(ascertainVAR(COMMON_INPUTS) == VALID) { VARS_CHECK { return theFile; } }
    if(ascertainVARNUM(COMMON_INPUTS) == VALID) { VARNUMS_CHECK { return theFile; } }
    if(ascertainOpenBrace(COMMON_INPUTS) == VALID) { BR_CHECK { return theFile; } }
    if(ascertainDirectional(COMMON_INPUTS) == VALID) { D_CHECK { return theFile; } }
    if (ascertainDO(COMMON_INPUTS) == VALID) { DO_CHECK { return theFile; } }
    if (ascertainPEN(COMMON_INPUTS) == VALID) { return (theFile + L_BOUND); }
    if (ascertainFROM(COMMON_INPUTS) == VALID) {
        if (switchForFROM(++COMMON_INPUTS) == VALID) { return theFile; } }

    if (ascertainTO(COMMON_INPUTS) == VALID) {
        if (switchForTO(++COMMON_INPUTS) == VALID) { return theFile; } }

    if (ascertainCOLONEQ(COMMON_INPUTS) == VALID) {
        if (switchForCOLONEQ(++COMMON_INPUTS) == VALID) { return theFile; } }

    if (ascertainSEMICOLON(COMMON_INPUTS) == VALID) {
        if (switchForSEMICOLON(++COMMON_INPUTS) == VALID) { return theFile; } }

    theFile = ascertainCloseBrace(COMMON_INPUTS, teotwawki_p);

    if (*teotwawki_p == VALID) {
        return theFile;
    }

    if (theFile -> thisTokenType == CLOSEBRACE) {
        if (switchForBraces(++theFile, loopSet_p, varStack) == VALID) {
            return theFile;
        }
    }

    if (theFile -> thisTokenType == DO) {
        if (switchForDO(++theFile, loopSet_p, varStack) == VALID) {
            return theFile;
        }
    }

    if (theFile == NULL) {
        printf("Unrecognised token received.\nCheck your file, correct it, and try again...\nExiting parser\n");
        exit(ERROR_EXIT);
    }

    return theFile;
}


/*  Cycle the variables and operators provided after :=
    And store these in relevant arrays to perform calculations upon.    */
void applyVarsAndOps (tokenStack* theFile, int* loopSet_p, float* varStack, int* notValid_p, float* variables, int* operators, int* varPos_p, int* opPos_p) {

    while (theFile -> thisTokenType != SEMICOLON) {
        switch (theFile -> thisTokenType) {
            cV  variables[(*varPos_p)++] = USE_VAR_VAL;
                break;
            cVN variables[(*varPos_p)++] = theFile -> thisTokenValue;
                break;
            cA cS cD cM operators[(*opPos_p)++] = theFile -> thisTokenType;
                break;
            cCE break;
            default:
                (*notValid_p)++;
                break;
        }
        theFile++;
    }

}


/* Set the token being looked at to the COLONEQUALS
   before the SEMICOLON.  If incorrect token found,
   declare error and exit.                          */
tokenStack* fetchCOLONEQ(tokenStack* theFile) {

    while (theFile -> thisTokenType != COLON_EQUALS) {
        if (theFile - ONE_VAR == NULL) {
            printf(INVALID_OPEN_COMM CHECK_AGAIN EXITING);
            exit(ERROR_EXIT);
        }
        theFile--;
    }

    return theFile;
}


/*  Switch to check the preceding character(s) for a SEMICOLON  */
void semiColonPreceder(tokenStack* theFile, int* loopSet_p, float* varStack, int* notValid_p, float* variables, int* operators, int* varPos_p, int* opPos_p) {

    theFile = fetchCOLONEQ(theFile);
    applyVarsAndOps(theFile, loopSet_p, varStack, notValid_p, variables, operators, varPos_p, opPos_p);

    if (*opPos_p != ((*varPos_p) - ONE_VAR) || (*opPos_p > ONE_OP && varPos_p == ZERO)) {
        printf(INVALID_SET_OPS CHECK_AGAIN EXITING);
        exit(ERROR_EXIT);
    }

}


/*  If sub-total goes beyond MAX_LIMIT, reset the value to MAX_LIMIT
    and post message to terminal.                                       */
void notBloodyLikely(float* subTotal_p) {

    if (*subTotal_p > MAX_LIMIT) {
        printf(EXCEED_RESET, MAX_LIMIT, MAX_LIMIT);
        *subTotal_p = MAX_LIMIT;
    }

}


/*  Perform calculation on operators/operands before the SEMICOLON in a SET statement.  */
void performCalculation(tokenStack* theFile, int* loopSet_p, float* varStack, int* notValid_p, float* variables, int* operators, int* varPos_p, int* opPos_p, int* numOfCalcs_p, float* subTotal_p){

    while (*opPos_p != *numOfCalcs_p) {
        switch (operators[(*opPos_p)++]) {
            cA  *subTotal_p += GRAB_VAR_SHIFT_FORWARD;
                notBloodyLikely(subTotal_p);
                break;
            cS  *subTotal_p -= GRAB_VAR_SHIFT_FORWARD;
                notBloodyLikely(subTotal_p);
                break;
            cM  *subTotal_p *= GRAB_VAR_SHIFT_FORWARD;
                notBloodyLikely(subTotal_p);
                break;
            cD  *subTotal_p /= GRAB_VAR_SHIFT_FORWARD;
                notBloodyLikely(subTotal_p);
                break;
            default:
                break;
        }
    }

}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow PEN                                             */
int ascertainPEN(tokenStack* theFile, int* loopSet_p, float* varStack) {

    if (theFile -> thisTokenType == PEN) {
        switch((theFile + VALID) -> thisTokenType) {
            case REDINK:
                varStack[RED] = R_RED, varStack[GREEN] = G_RED, varStack[BLUE] = B_RED;
                return VALID;
            case BLUEINK:
                varStack[RED] = R_BLUE, varStack[GREEN] = G_BLUE, varStack[BLUE] = B_BLUE;
                return VALID;
            case PURPLEINK:
                varStack[RED] = R_PURP, varStack[GREEN] = G_PURP, varStack[BLUE] = B_PURP;
                return VALID;
            case ORANGEINK:
                varStack[RED] = R_ORAN, varStack[GREEN] = G_ORAN, varStack[BLUE] = B_ORAN;
                return VALID;
            case YELLOWINK:
                varStack[RED] = R_YELL, varStack[GREEN] = G_YELL, varStack[BLUE] = B_YELL;
                return VALID;
            case GREENINK:
                varStack[RED] = R_GREE, varStack[GREEN] = G_GREE, varStack[BLUE] = B_GREE;
                return VALID;
            case WHITE:
                varStack[RED] = RGB_WHITE, varStack[GREEN] = RGB_WHITE, varStack[BLUE] = RGB_WHITE;
                return VALID;
            case OFF:
                varStack[RED] = -VALID, varStack[GREEN] = -VALID, varStack[BLUE] = -VALID;
                return VALID;
            default:
                printf("Invalid Colour registered in file.\n" CHECK_AGAIN EXITING);
                break;
        }
    }

    return ZERO;
}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a SEMICOLON.                                    */
int ascertainSEMICOLON(tokenStack* theFile, int* loopSet_p, float* varStack) {

    float variables[VAR_OP_ARRAY_CAPACITY], subTotal = ZERO, *subTotal_p = &subTotal;

    int operators[VAR_OP_ARRAY_CAPACITY], varPos = ZERO, *varPos_p = &varPos, opPos = ZERO, *opPos_p = &opPos, numOfCalcs = ZERO, *numOfCalcs_p = &numOfCalcs, notValid = ZERO, *notValid_p = &notValid;

    if (theFile -> thisTokenType == SEMICOLON) {

        semiColonPreceder(PRECEDER_REQS);
        if (notValid) { printf(SEMIERROR CHECK_AGAIN EXITING), exit(ERROR_EXIT); }

        while ((theFile - ONE_VAR) -> thisTokenType != SET) { theFile--; }

        numOfCalcs = opPos, varPos = ZERO, opPos = ZERO, subTotal = variables[varPos++];

        if (varPos >= ONE_VAR) {
            performCalculation(theFile, loopSet_p, varStack, notValid_p, variables, operators, varPos_p, opPos_p, numOfCalcs_p, subTotal_p);
            USE_VAR_VAL = subTotal;
        }

        if (varPos == ZERO && opPos == ONE_OP) {
            theFile -> varToOp = singleCharChecker(operators[ZERO]);
        }

        while (theFile -> thisTokenType != SEMICOLON) { theFile++; }
        return VALID;
    }

    return ZERO;
}


/*  Switch to check the preceding character for an OPERATOR     */
void opPreceder(tokenStack* theFile, int* loopSet_p, float* varStack) {

    switch ((theFile - ONE_OP) -> thisTokenType) {
            OP_PREC_SWITCHES
            break;
        default:
            printf(VAR_VARNUM_OP CHECK_AGAIN EXITING);
    }

}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a POLISH.                                                   */
int ascertainPOLISH(tokenStack* theFile, int* loopSet_p, float* varStack) {

    if (ADD_DESIRED || SUB_DESIRED || MLT_DESIRED || DIV_DESIRED) {
        switch(theFile -> thisTokenType) {
            ARITHMETIC_SWITCHES
                return VALID;
            default:
                break;
        }
    }

    return ZERO;
}



/*  Ascertains whether a token to the right of a COLON_EQUALS is valid.
    If it is not, increases the int linked to the pointer by 1.                             */
void colonEqPreceder(tokenStack* theFile, int* loopSet_p, float* varStack, int* notValid_p) {

    while (theFile -> thisTokenType != SEMICOLON) {
        theFile++;
        switch(theFile -> thisTokenType) {
                COLONEQ_PREC_SWITCHES
                break;
            default:
                (notValid_p)++;
                break;
        }
    }

}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a COLON_EQUALS.                                             */
int ascertainCOLONEQ(tokenStack* theFile, int* loopSet_p, float* varStack) {

    int notValid = ZERO, *notValid_p = &notValid;

    if (theFile -> thisTokenType == COLON_EQUALS) {
        colonEqPreceder(theFile, loopSet_p, varStack, notValid_p);
        if (notValid) { printf(COLONEQ_INVALID CHECK_AGAIN EXITING), exit(ERROR_EXIT); }

        while (theFile -> thisTokenType != COLON_EQUALS) { --theFile; }

        return VALID;
    }

    return ZERO;
}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a SET.                                                      */
int ascertainSET(tokenStack* theFile, int* loopSet_p, float* varStack) {

    if (theFile -> thisTokenType == SET) { return VALID; }

    return ZERO;
}


/*  Checks that a FROM is in the correct place before a TO command.
    If not, exits the program with an error message.                                        */
void toPreceder(tokenStack* theFile, int* loopSet_p, float* varStack) {

    if (theFile -> thisTokenType != FROM) {
        printf(TO_WITHOUT_DO CHECK_AGAIN EXITING);
        exit(ERROR_EXIT);
    }

}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a FROM.                                                     */
int ascertainTO(tokenStack* theFile, int* loopSet_p, float* varStack) {

    if (theFile -> thisTokenType == TO) {
        toPreceder((theFile - L_BOUND), loopSet_p, varStack);
        return VALID;
    }

    return ZERO;
}


/*  Checks that a DO is in the correct place before a FROM command.
    If not, exits the program with an error message.                                        */
void fromPreceder(tokenStack* theFile, int* loopSet_p, float* varStack) {

    if (theFile -> thisTokenType != DO) {
        printf(FROM_WITHOUT_DO CHECK_AGAIN EXITING);
        exit(ERROR_EXIT);
    }

}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a FROM.                                                     */
int ascertainFROM(tokenStack* theFile, int* loopSet_p, float* varStack) {

    if (theFile -> thisTokenType == FROM) {
        fromPreceder((theFile - L_BOUND), loopSet_p, varStack);
        return VALID;
    }

    return ZERO;
}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a DO.                                                       */
int ascertainDO(tokenStack* theFile, int* loopSet_p, float* varStack) {

    int setFrom = setFromToTest(theFile, varStack, L_BOUND + ONE_VAR);

    // Set the varStack of the following VAR to the FROM value
    if (theFile -> thisTokenType == DO) {
        NEXT_TOKEN_VAR_VAL = setFrom;
        return VALID;
    }
    return ZERO;
}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a VARNUM.                                                   */
int ascertainVARNUM(tokenStack* theFile, int* loopSet_p, float* varStack) {

    if (theFile -> thisTokenType == VARNUM) {
        return VALID;
    }
    return ZERO;
}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a DIRECTIONAL COMMAND.                                      */
int ascertainDirectional(tokenStack* theFile, int* loopSet_p, float* varStack) {

    // Check FD | RT | LT)
    if (theFile -> thisTokenType == FD) { return VALID; }
    if (theFile -> thisTokenType == LT) { return VALID; }
    if (theFile -> thisTokenType == RT) { return VALID; }
    return ZERO;
}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a VAR.                                                      */
int ascertainVAR(tokenStack* theFile, int* loopSet_p, float* varStack) {
    if (theFile -> thisTokenType == VAR) {
        return VALID;
    }
    return ZERO;
}


/*  Ensure that TO precedes a { token                                                       */
void openbracePreceder(tokenStack* theFile, int* loopSet_p, float* varStack) {

    if (*loopSet_p > VALID) {
        if ((theFile - L_BOUND) -> thisTokenType != TO) {
            printf(LOOP_INVALID CHECK_AGAIN EXITING);
            exit(ERROR_EXIT);
        }
    }

}


/*  Set value for fromTest variable                                                         */
int setFromToTest(tokenStack* theFile, float* varStack, int adjuster) {

    if ((theFile + adjuster) -> thisTokenType == VAR) {
        return varStack[(int)((theFile + adjuster) -> thisTokenLetter) - (int)'A'];
    } else {
        return (theFile + adjuster) -> thisTokenValue;
    }
    return ZERO;
}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow an OPENBRACE.                                               */
int ascertainOpenBrace(tokenStack* theFile, int* loopSet_p, float* varStack) {


    if (theFile -> thisTokenType == OPENBRACE) {

        (*loopSet_p)++;
        openbracePreceder(theFile, loopSet_p, varStack);

        if (*loopSet_p > FIRST) {
            while ((theFile - ONE_VAR) -> thisTokenType != DO) { theFile--; }

            appropriateBoundChecking(theFile, varStack);

        }
        return VALID;
    }

    return ZERO;
}


/*  Ensure that token hunting is within legal bounds for loopSets                   */
int appropriateBoundChecking(tokenStack* theFile, float* varStack) {

    tokenStack* backHere = NULL;

    if CHECK_WITHIN_BOUNDS {

        while (theFile -> loopSet == FIRST) { theFile++; }
        while (theFile -> loopSet != FIRST) { theFile++; }

        return VALID;

    } else {

        if (USE_VAR_VAL < setFromToTest(theFile, varStack, U_BOUND)) { USE_VAR_VAL++; }

        backHere = theFile;
        conductAllocationOfFromVal(theFile, varStack);
        theFile = backHere;

        while (theFile -> thisTokenType != OPENBRACE) { theFile++; }

    }

    return VALID;
}


/*  Allocate values to VAR following DO commands ahead of next cycle of a DO loop       */
void conductAllocationOfFromVal(tokenStack* theFile, float* varStack) {

    while (theFile -> loopSet > ZERO) {
        theFile++;
        if (theFile -> thisTokenType == DO) {
            varStack[(int)(NEXT_TOKEN) -> thisTokenLetter - (int) 'A'] = setFromToTest(theFile, varStack, L_BOUND + ONE_VAR);
        }
    }

}


/*  Print the values associated to each alphabetical character          */
void printAlphabet(float* varStack) {

    for (int i = ZERO; i < ALPHABET; i++) {
        if (i > ZERO && !(i% (L_BOUND + VALID))) {
            printf("\n");
        }

        varStack = varStack + i;
        printf("%c: %20.2f   ",i+'A',*varStack);
        varStack = varStack - i;
    }
    printf("\n\n");

}


/****************************************************************************************************/
/****************************************************************************************************/

/*  The following functions will push a pointer to the tokenized file back to the main body of the
    file, outside of a DO loop.                                                                     */

tokenStack* pushFileToSubLoop(tokenStack* theFile, int* loopSet_p, float* varStack, int subLoop) {

    while (theFile -> loopSet < subLoop) { theFile-- ; }

    return theFile;
}


tokenStack* pushFileToMainBody(tokenStack* theFile, int* loopSet_p, float* varStack) {

    while (theFile -> loopSet > ONE_VAR) { theFile-- ; }

    return theFile;
}


tokenStack* pushBackToDO(tokenStack* theFile, int* loopSet_p, float* varStack) {

    while (theFile -> thisTokenType != DO) { theFile-- ; }

    return theFile;

}

/******************************************************************************************************/
/******************************************************************************************************/


/*  Check that the next VAR checked during a FROM / TO test in a DO loop is valid.
    If it is, continue through the loop cycle                                       */
tokenStack* nextVarCheck(tokenStack* theFile, int* loopSet_p, float* varStack, tokenStack* revertToHere) {

    if (NEXT_TOKEN_VAR_VAL < setFromToTest(theFile, varStack, L_BOUND + ONE_VAR) ||
        NEXT_TOKEN_VAR_VAL >= setFromToTest(theFile, varStack, U_BOUND + ONE_VAR)) {

        if (NEXT_TOKEN_VAR_VAL > setFromToTest(theFile, varStack, U_BOUND + ONE_VAR)) {
            NEXT_TOKEN_VAR_VAL--;
        }

        theFile = revertToHere + ONE_VAR;
        return theFile;
    }

    return theFile;
}


/*  Ascertains whether the next token fits in line with the grammatical rules
    for what can legally follow a CLOSEBRACE.                                       */
tokenStack* ascertainCloseBrace(tokenStack* theFile, int* loopSet_p, float* varStack, int* teotwawki_p) {

    tokenStack* revertToHere = theFile - ONE_VAR;

    int thisLoopSet;

    if (theFile -> thisTokenType == CLOSEBRACE) {
        if (*loopSet_p != ZERO) { (*loopSet_p)--; }

        //printf("Decreasing loop.\nLoop %d\n", *loopSet_p);
        if (*loopSet_p == ZERO) {

            if ((theFile) -> thisTokenType == CLOSEBRACE) {
                printf("Successfully parsed file.\n");
                // Test by printing values of alphabet
                // printAlphabet(varStack);

                *teotwawki_p = VALID;

                return theFile;
            }

        } else {

            if (*loopSet_p == FIRST) {

                theFile = pushBackToDO(pushFileToMainBody(pushFileToSubLoop(theFile, loopSet_p, varStack, 2), loopSet_p, varStack), loopSet_p, varStack);
                theFile = nextVarCheck(theFile, loopSet_p, varStack, revertToHere);

                return theFile;
            }

            if (*loopSet_p > FIRST) {
                thisLoopSet = *loopSet_p;

                theFile--;

                while (theFile -> loopSet != thisLoopSet) { theFile--; }
                while (theFile -> thisTokenType != DO) { theFile--; }

                if (NEXT_TOKEN_VAR_VAL < setFromToTest(theFile, varStack, L_BOUND + ONE_VAR) ||
                    NEXT_TOKEN_VAR_VAL >= setFromToTest(theFile, varStack, U_BOUND + ONE_VAR)) {

                    theFile = revertToHere + ONE_VAR;
                    return theFile;
                }

                return theFile;
            }

            return theFile;
        }

    }

    return NULL;
}


void printTokenType(tokenStack* theFile) {

    switch (theFile -> thisTokenType) {
        cF cL cR printf("Directional Move "); break;
        cSC printf(";\n");          break;
        cV  printf("VAR\n");        break;
        cVN printf("VARNUM\n");     break;
        cCE printf(":= ");          break;
        cST printf("SET ");         break;
        cDO printf("DO ");          break;
        cFR printf("FROM ");        break;
        cTO printf("TO ");          break;
        cOB printf("{\n");          break;
        cCB printf("}\n");          break;
        cA cS cM cD printf("Operator\n");    break;

        default:
            break;
    }

}


/* Send display output to SDL Window */
int displayGraphics(int* loopSet_p, float* varStack, FILE* fp, int rainbow) {

    int teotwawki = ZERO, *teotwawki_p = &teotwawki;
    int red = (int)varStack[RED], green = (int)varStack[GREEN], blue = (int)varStack[BLUE], *r_p = &red, *g_p = &green, *b_p = &blue;

    double cosOut = ZERO, sinOut = ZERO, hypLen = ZERO, xPosEnd = ZERO, yPosEnd = ZERO;

    tokenStack* printToScreen = (tokenStack*)malloc(sizeof(tokenStack)*MAX_STACK);

    for (int i = ZERO; i < ALPHABET; i++) { varStack[i] = ZERO; }
    varStack[ALPHABET] = ZERO;

    rewind(fp), tokenizeFile(fp, printToScreen, loopSet_p, varStack);

    SDL_Simplewin theWindow;
    Shorthand_SDL_Init(&theWindow);
    SDL_SetWindowTitle(theWindow.win, "The Turtle Strikes Back");

    varStack[RED] = RGB_WHITE, varStack[GREEN] = RGB_WHITE, varStack[BLUE] = RGB_WHITE;

    do {
        printToScreen = ascertainNextValidity(printToScreen, loopSet_p, varStack, teotwawki_p);

        if (*teotwawki_p == VALID) {
            finalPresent(theWindow);
            return ZERO;
        }

        cosOut = cos((double)varStack[ALPHABET] * TO_RADIANS), sinOut = sin((double)varStack[ALPHABET] * TO_RADIANS);

        amendTheWindow(printToScreen, hypLen, yPosEnd, xPosEnd, cosOut, sinOut, r_p, g_p, b_p, rainbow, varStack, theWindow);

    } while (!theWindow.finished);

    return ZERO;
}


/* Generate final presentation of image on window, then destroy and close */
void finalPresent (SDL_Simplewin theWindow) {

    SDL_RenderPresent(theWindow.renderer);
    SDL_UpdateWindowSurface(theWindow.win);
    SDL_Delay(DELAY_AT_QUIT), SDL_RenderClear(theWindow.renderer), atexit(SDL_Quit);

}


/* Make necessary amends to window to reflect changes to drawn surface */
void amendTheWindow (tokenStack* printToScreen, float hypLen, float yPosEnd, float xPosEnd, float cosOut, float sinOut, int* r_p, int* g_p, int* b_p, int rainbow, float* varStack, SDL_Simplewin theWindow) {
    if ((printToScreen - ONE_VAR) -> thisTokenType == FD) {
        if (printToScreen -> thisTokenType == VAR) { hypLen = varStack[(int)printToScreen -> thisTokenLetter - (int)'A']; }
        if (printToScreen-> thisTokenType == VARNUM) { hypLen = printToScreen -> thisTokenValue; }

        yPosEnd = (hypLen * cosOut) * -VALID, xPosEnd = hypLen * sinOut;

        amendRainbowPens(rainbow, varStack, r_p, g_p, b_p);
        camouflageCheck(rainbow, varStack);

        Shorthand_SDL_SetDrawColour(&theWindow, varStack[RED], varStack[GREEN], varStack[BLUE]);

        if (varStack[RED] != -VALID && varStack[GREEN] != -VALID && varStack[BLUE] != -VALID) {
            if(SDL_RenderDrawLine(theWindow.renderer, LINE_COORDS) == VALID) {
                printf("Cannot draw line. SDL Error: %s\nExiting Turtles.\n", SDL_GetError());
            }
        }

        varStack[XLOC] = varStack[XLOC] + xPosEnd;
        varStack[YLOC] = varStack[YLOC] + yPosEnd;
        SDL_Delay(QUICK_DELAY);
        SDL_RenderPresent(theWindow.renderer);
        Shorthand_SDL_Events(&theWindow);
    }
}

/* If "-rainbow" has been invoked on the command line, automatically set the pen filter values. */
void amendRainbowPens(int rainbow, float* varStack, int* red, int* green, int* blue) {

    if (rainbow == VALID) {
        amendCol(varStack, red, RED, RAINBOW_MOD_R, RAINBOW_BASE_R, RAINBOW_RED_A);
        amendCol(varStack, green, GREEN, RAINBOW_MOD_G, RAINBOW_BASE_G, RAINBOW_GREEN_A);
        amendCol(varStack, blue, BLUE, RAINBOW_MOD_B, RAINBOW_BASE_B, RAINBOW_BLUE_A);
    }

}

// Call to amend the colour of RED/GREEN/BLUE filters for pen
void amendCol (float* varStack, int* colour, int BASE_COL, int BASE_MOD_NUM, int BASE_COL_NUM, int AMEND_COL_NUM) {

    if (varStack[BASE_COL] >= RGB_WHITE) {
        varStack[BASE_COL] = BASE_COL_NUM, *colour = BASE_COL_NUM;
    } else {
        varStack[BASE_COL]++;
    }

}

// Call to invoke camouflage, if flag has been raised at invocation.
void camouflageCheck (int rainbow, float* varStack) {

    if (rainbow == L_BOUND) {
        varStack[RED] = ZERO, varStack[GREEN] = ZERO, varStack[BLUE] = ZERO;
    }
}


