/*
* File: Encoder.c
* Author: Jacob Johnson
* Date: 09/23/2024
* Description: This program can encode plain text to numbers and decode numbers back into plain text.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Establish methods so they can be placed where I want them to be
void encode();
void decode();
int charToNumber(char c);
char numberToChar(int num);

int main() {
    int option;

    // Ask the user what they want to do
    printf("Choose an option:\n");
    printf("1. encode a file\n");
    printf("2. Decode a file\n");
    printf("Enter your option: ");
    scanf("%d", &option);
    
    if (option == 1) {
        encode();
    } else if (option == 2) {
        decode();
    } else {
        printf("Invalid option. Restart and try again.\n");
    }

    return 0;
}

void encode() {
    // Define Files and the charecter varible
    FILE *inputFile, *outputFile;
    char c;

    // Open files and always make encoded.txt if it does not exist
    inputFile = fopen("unencoded.txt", "r");
    outputFile = fopen("encoded.txt", "w");

    // If one of the files does not exist exit the program
    if (inputFile == NULL || outputFile == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    // iterate through each charecter and encode it with custom method
    while ((c = fgetc(inputFile)) != EOF) {
        if (isalnum(c) || c == ' ' || c == '.' || c == '?' || c == '!') {
            int encoded = charToNumber(c);
            if (encoded != -1) {
                fprintf(outputFile, "%d ", encoded);
            }
        }
    }

    // Close files so program can be run again
    fclose(inputFile);
    fclose(outputFile);

    printf("File successfully encoded and saved as 'encoded.txt'.\n");
}

void decode() {
    // Declare files and number varibles
    FILE *inputFile;
    int num;

    // Open file to decode
    inputFile = fopen("encoded.txt", "r");

    // If file does not exist exit the program
    if (inputFile == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    printf("Decoded message: ");

    // Decode the number to a charecter with custom method
    while (fscanf(inputFile, "%d", &num) != EOF) {
        char decoded = numberToChar(num);
        if (decoded != '\0') {
            printf("%c", decoded);
        }
    }

    fclose(inputFile);

    printf("\n");
}

// Custom method to convert a charecter to a number
int charToNumber(char c) {
    c = tolower(c);  // Ignore case by converting to lowercase

    switch (c) {
        case 'a': return 10;
        case 'b': return 11;
        case 'c': return 12;
        case 'd': return 13;
        case 'e': return 14;
        case 'f': return 15;
        case 'g': return 16;
        case 'h': return 17;
        case 'i': return 18;
        case 'j': return 19;
        case 'k': return 20;
        case 'l': return 21;
        case 'm': return 22;
        case 'n': return 23;
        case 'o': return 24;
        case 'p': return 25;
        case 'q': return 26;
        case 'r': return 27;
        case 's': return 28;
        case 't': return 29;
        case 'u': return 30;
        case 'v': return 31;
        case 'w': return 32;
        case 'x': return 33;
        case 'y': return 34;
        case 'z': return 35;
        case ' ': return 36;
        case '.': return 37;
        case '?': return 38;
        case '!': return 39;
        default: return -1; // Invalid character
    }
}

// Custom method to convert a number to a charecter
char numberToChar(int num) {
    switch (num) {
        case 10: return 'A';
        case 11: return 'B';
        case 12: return 'C';
        case 13: return 'D';
        case 14: return 'E';
        case 15: return 'F';
        case 16: return 'G';
        case 17: return 'H';
        case 18: return 'I';
        case 19: return 'J';
        case 20: return 'K';
        case 21: return 'L';
        case 22: return 'M';
        case 23: return 'N';
        case 24: return 'O';
        case 25: return 'P';
        case 26: return 'Q';
        case 27: return 'R';
        case 28: return 'S';
        case 29: return 'T';
        case 30: return 'U';
        case 31: return 'V';
        case 32: return 'W';
        case 33: return 'X';
        case 34: return 'Y';
        case 35: return 'Z';
        case 36: return ' ';
        case 37: return '.';
        case 38: return '?';
        case 39: return '!';
        default: return '\0'; // Invalid number
    }
}
