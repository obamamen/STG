#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "ansi.h"

#include <time.h>

#define MAX_WORDS 40
#define MAX_WORD_LENGTH 20

int readWordList(const char *filename, char words[MAX_WORDS][MAX_WORD_LENGTH]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Could not open word list file: %s\n", filename);
        return 0;
    }
    int count = 0;
    while (fscanf(file, "%49s", words[count]) == 1) {
        count++;
        if (count >= MAX_WORDS) {
            break;
        }
    }
    fclose(file);
    return count;
}

void shuffleWords(char words[MAX_WORDS][MAX_WORD_LENGTH], int count) {
    for (int i = 0; i < count; i++) {
        int j = rand() % count;
        char temp[MAX_WORD_LENGTH];
        strcpy(temp, words[i]);
        strcpy(words[i], words[j]);
        strcpy(words[j], temp);
    }
}


void convertTo1DArray(char words[MAX_WORDS][MAX_WORD_LENGTH], char words1D[], int count) {
    int index = 0;
    for (int i = 0; i < count; i++) {
        int j = 0;
        while (words[i][j] != '\0') {
            words1D[index++] = words[i][j++];
        }
        words1D[index++] = ' ';
    }
    words1D[index - 1] = '\0';
}

#ifdef _WIN32
#include <windows.h>
void enableANSIColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut == INVALID_HANDLE_VALUE) return;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= 0x0004;
    SetConsoleMode(hOut, dwMode);
}
#else
void enableANSIColors() {}
#endif

void printChar(char c, char* buffer, int* buffer_pos) {
    if (c == ' ') {
        sprintf(&buffer[*buffer_pos], " | ");
        *buffer_pos += 3;
    } else {
        buffer[*buffer_pos] = c;  
        *buffer_pos += 1;
    }
}

int main() {
    enableANSIColors();

    srand(time(NULL));

    char words[MAX_WORDS][MAX_WORD_LENGTH];  
    char words1D[MAX_WORDS * MAX_WORD_LENGTH]; 

    int wordCount = readWordList("text.txt", words);
    if (wordCount == 0) {
        return 1;
    }

    shuffleWords(words, wordCount);
    convertTo1DArray(words, words1D, wordCount); 


    char* text = words1D;

    size_t textLength = strlen(text);
    char* input = calloc(textLength + 1, sizeof(char));
    
    unsigned int inputPointer = 0;
    char* buffer = malloc(14 * textLength);  // buffersize = text * fgColor + text * bgColor + textLength; 13 x textLength
    int buffer_pos = 0;  

    for (int i = 0; i < textLength; i++) {
        if (input[i] != 0) {
            if (input[i] == text[i]) {
                sprintf(&buffer[buffer_pos], GREEN);
                buffer_pos += strlen(GREEN);
                printChar(input[i], buffer, &buffer_pos);
            } else {
                sprintf(&buffer[buffer_pos], RED);
                buffer_pos += strlen(RED);
                printChar(text[i], buffer, &buffer_pos);
            }
            if (i == inputPointer) {
                sprintf(&buffer[buffer_pos], BG_BRIGHT_BLACK);
                buffer_pos += strlen(BG_BRIGHT_BLACK);
            }
            sprintf(&buffer[buffer_pos], RESET);
            buffer_pos += strlen(RESET);
        } else {
            sprintf(&buffer[buffer_pos], BRIGHT_WHITE);
            buffer_pos += strlen(BRIGHT_WHITE);
            if (i == inputPointer) {
                sprintf(&buffer[buffer_pos], BG_BRIGHT_BLACK);
                buffer_pos += strlen(BG_BRIGHT_BLACK);
            }
            printChar(text[i], buffer, &buffer_pos);
            sprintf(&buffer[buffer_pos], RESET);
            buffer_pos += strlen(RESET);
        }
    }

    printf("%s", buffer);


    while (1) {
        if (_kbhit()) {  
            system("cls");
            char inputChar = _getch();
            

            if (inputChar == 8 && inputPointer > 0) {
                input[--inputPointer] = 0;
            } else if (inputPointer < textLength) {
                input[inputPointer++] = inputChar;
            }

            if (inputPointer == textLength) {
                int win = 1;
                for (int w = 0; w < textLength; w++) {
                    if (text[w] != input[w]) {
                        win = 0;
                    }
                }
                if (win) {
                    printf(GREEN "\nYou completed the text correctly!\n" RESET);
                } else {
                    printf(RED "\nYou made a mistake somewhere!\n" RESET);
                }
            }
            buffer_pos = 0;


            for (int i = 0; i < textLength; i++) {
                if (input[i] != 0) {
                    if (input[i] == text[i]) {
                        sprintf(&buffer[buffer_pos], GREEN);
                        buffer_pos += strlen(GREEN);
                        printChar(input[i], buffer, &buffer_pos);
                    } else {
                        sprintf(&buffer[buffer_pos], RED);
                        buffer_pos += strlen(RED);
                        printChar(text[i], buffer, &buffer_pos);
                    }
                    if (i == inputPointer) {
                        sprintf(&buffer[buffer_pos], BG_BRIGHT_BLACK);
                        buffer_pos += strlen(BG_BRIGHT_BLACK);
                    }
                    sprintf(&buffer[buffer_pos], RESET);
                    buffer_pos += strlen(RESET);
                } else {
                    sprintf(&buffer[buffer_pos], BRIGHT_WHITE);
                    buffer_pos += strlen(BRIGHT_WHITE);
                    if (i == inputPointer) {
                        sprintf(&buffer[buffer_pos], BG_BRIGHT_BLACK);
                        buffer_pos += strlen(BG_BRIGHT_BLACK);
                    }
                    printChar(text[i], buffer, &buffer_pos);
                    sprintf(&buffer[buffer_pos], RESET);
                    buffer_pos += strlen(RESET);
                }
            }

            printf("%s", buffer);

            if (inputChar == 27) {
                break;
            }
        }
    }

    free(input);  
    free(buffer);
    printf(RESET);
    return 0;
}
