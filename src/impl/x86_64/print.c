#include "print.h"

// Buffer can hold 80 x 25 characters
const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;

// Represents a character with an ASCII character code,
// followed by an 8-bit color code
struct Char {
    uint8_t character;
    uint8_t color;
};

// Reference to video memory
struct Char* buffer = (struct Char*) 0xb8000;

// Keep track of the current column and row numbers that
// we're printing at
size_t col = 0;
size_t row = 0;

// Keep track of what the current color should be when
// we're printing
//
// First 4 bits - foreground color
// Next 4 bits - background color
uint8_t color = PRINT_COLOR_WHITE | PRINT_COLOR_BLACK << 4;

// Clears a row of text from the console
void clear_row(size_t row) {
    // Character with a space, print the background color
    struct Char empty = (struct Char){
        character : ' ',
        color : color,
    };

    // For each column in the row, set the value at that
    // co-ordinate to the empty character above
    for (size_t col = 0; col < NUM_COLS; col++) {
        buffer[col + NUM_COLS * row] = empty;
    }
}

// Goes to a new line within the console
void print_newline() {
    col = 0;

    if (row < NUM_ROWS - 1) {
        row++;
        return;
    }

    // Iterate through each co-ordinate
    // Move each row after the current row down one line
    for (size_t row = 1; row < NUM_ROWS; row++) {
        for (size_t col = 0; col < NUM_COLS; col++) {
            struct Char character = buffer[col + NUM_COLS * row];
            buffer[col + NUM_COLS * (row - 1)] = character;
        }
    }

    clear_row(NUM_COLS - 1);
}

// Clears a line of text from the console
void print_clear() {
    for (size_t i = 0; i < NUM_ROWS; i++) {
        clear_row(i);
    }
}

// Prints a character to the console
void print_char(char character) {
    // Print a new line if the newline character '\n' is
    // the character to be printed
    if (character == '\n') {
        print_newline();
        return;
    }

    // If the column exceeds the max width, go to a new line
    if (col > NUM_COLS)
        print_newline();

    // Write the new character to the console
    buffer[col + NUM_COLS * row] = (struct Char){
        character : (uint8_t) character,
        color : color,
    };

    // Go to the next column
    col++;
}

// Prints a string to the console
void print_str(char* str) {
    // Loop over each co-ordinate in the buffer, in the row to
    // be printed
    for (size_t i = 0; 1; i++) {
        // Set the character to each character in the string
        char character = (uint8_t) str[i];

        // Return from the function once a null-terminated
        // character is reached
        if (character == '\0')
            return;

        print_char(character);
    }
}

// Changes the foreground and background color of the console
void print_set_color(uint8_t foreground, uint8_t background) {
    // Populate the first 4 bits of the color with the foreground
    // color
    // Populate the last 4 bits with the background color
    color = foreground + (background << 4);
}