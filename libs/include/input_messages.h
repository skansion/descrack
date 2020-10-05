#ifndef __INPUT_MESSAGES_H__
#define __INPUT_MESSAGES_H__


void exitInvalidOption(const int option);
void exitDuplicatedOption(const int option);
void exitTextArgumentTooLong();
void exitHexArgumentTooLong();
void exitInvalidHexDigit();
void exitInvalidNumber();
void exitAlphabetArgumentTooLong();
void exitInvalidAlphabet();
void exitMinGreaterThanMax();
void exitMissingArgument();
void exitTooManyArguments();
void exitIncompatibleFlags();


#endif