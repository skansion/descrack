#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input_messages.h"


// Messaggi di errore
static const char *INVALIDOPTIONERR = "Invalid option";
static const char *DUPLICATEDOPTIONERR = "Duplicated option";
static const char *LONGTEXTTYPEARGUMENTERR = "TEXT argument is too long ";
static const char *LONGHEXTYPEARGUMENTERR = "HEX argument is too long";
static const char *INVALIDHEXDIGITERR = "Invalid hex digit";
static const char *INVALIDNUMBERERR = "Invalid number";
static const char *LONGALPHABETARGUMENTERR = "ALPHABET argument is too long";
static const char *INVALIDALPHABETERR = "Invalid alphabet";
static const char *MINGREATERTHANMAXERR = "MIN value is greater than MAX value";
static const char *MISSINGARGUMENTERR = "Missing argument";
static const char *TOOMANYARGUMENTSERR = "Too many arguments";
static const char *INCOMPATIBLEFLAGSERR = "Cannot use \"-s\" and \"-X\" flags together";


/**
 * Stampa un messaggio su STDERR ed esci
 * 
 * @param message messaggio da stampare
 **/
static void exitInputWithMessage(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 * 
 * @param option opzione duplicata
 **/
void exitInvalidOption(const int option) {
    char buffer[19] = {0};

    strncat(buffer, INVALIDOPTIONERR, 15);
    strncat(buffer, ": - ", 5);
    buffer[17] = option;
    exitInputWithMessage(buffer);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 * 
 * @param option opzione non valida
 **/
void exitDuplicatedOption(const int option) {
    char buffer[22] = {0};

    strncat(buffer, DUPLICATEDOPTIONERR, 18);
    strncat(buffer, ": - ", 5);
    buffer[20] = option;
    exitInputWithMessage(buffer);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 **/
void exitTextArgumentTooLong() {
    exitInputWithMessage(LONGTEXTTYPEARGUMENTERR);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 **/
void exitHexArgumentTooLong() {
    exitInputWithMessage(LONGHEXTYPEARGUMENTERR);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 **/
void exitInvalidHexDigit() {
    exitInputWithMessage(INVALIDHEXDIGITERR);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 **/
void exitInvalidNumber() {
    exitInputWithMessage(INVALIDNUMBERERR);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 **/
void exitAlphabetArgumentTooLong() {
    exitInputWithMessage(LONGALPHABETARGUMENTERR);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 **/
void exitInvalidAlphabet() {
    exitInputWithMessage(INVALIDALPHABETERR);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 **/
void exitMinGreaterThanMax() {
    exitInputWithMessage(MINGREATERTHANMAXERR);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 **/
void exitMissingArgument() {
    exitInputWithMessage(MISSINGARGUMENTERR);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 **/
void exitTooManyArguments() {
    exitInputWithMessage(TOOMANYARGUMENTSERR);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci
 **/
void exitIncompatibleFlags() {
    exitInputWithMessage(INCOMPATIBLEFLAGSERR);
}