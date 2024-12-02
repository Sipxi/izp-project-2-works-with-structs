// Made by Serhij Čepil
// FIT VUT Student
// https://github.com/sipxi
// Day of completion 10/27/2024
// Time spent: 20h+

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_REQUIRED_ARGS 2
#define MAX_ALLOWED_ARGS 3
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define NOT_FOUND_INDEX 0

// Enum for args
typedef enum {
    INVALID,
    HLINE,
    VLINE,
    SQUARE,
    HELP,
    TEST

} MODE;

// Define the Bitmap struct
// Data is stored in 1D array of chars
typedef struct {
    int rows;
    int columns;
    char *data;
} Bitmap;

// Define the Position struct
// Used to store row and column
typedef struct {
    int row;
    int col;
} Position;

// Define the ModeMap struct
// Maps modes to names
typedef struct {
    char *name;
    MODE mode;
} ModeMap;

/**
 * Parses user input and returns the mode
 * @param argc The number of arguments
 * @param argv The array of arguments
 * @return The mode
 */
MODE parseUserInput(int argc, char *argv[]) {
    if (argc < MIN_REQUIRED_ARGS || argc > MAX_ALLOWED_ARGS) {
        return INVALID;
    }
    ModeMap modeMap[] = {
        {"--help", HELP}, {"test", TEST},     {"hline", HLINE},
        {"vline", VLINE}, {"square", SQUARE},
    };
    // Get the size of the modeMap
    int modeMapSize = sizeof(modeMap) / sizeof(modeMap[0]);

    // Iterate over the modeMap
    for (int idx = 0; idx < modeMapSize; idx++) {
        // Check if the mode name matches
        if (strcmp(argv[1], modeMap[idx].name) == 0) {
            // If mode is help and argc is not equal to MIN_REQUIRED_ARGS
            if ((modeMap[idx].mode == HELP) && argc != MIN_REQUIRED_ARGS) {
                return INVALID;
            }
            // mode is hline, vline, square, or test and argc is not equal to
            // MAX_ALLOWED_ARGS
            if ((modeMap[idx].mode == HLINE || modeMap[idx].mode == VLINE ||
                 modeMap[idx].mode == SQUARE || modeMap[idx].mode == TEST) &&
                argc != MAX_ALLOWED_ARGS) {
                return INVALID;
            }
            // Return the mode if all conditions are met
            return modeMap[idx].mode;
        }
    }
    // Return invalid if no mode is found
    return INVALID;
}

/**
 * Gets value of bitmap at row and column
 * @param bitmap The bitmap
 * @param row The row
 * @param column The column
 * @return The value
 */
int getValue(Bitmap bitmap, int row, int column) {
    return bitmap.data[row * bitmap.columns + column];
}

/**
 * Updates the position
 * @param src The source position
 * @param dst The destination position
 */
void updatePos(Position *src, Position *dst) {
    dst->row = src->row;
    dst->col = src->col;
}

/**
 * Reads the dimentions of the bitmap from the file
 * @param bitmap The bitmap
 * @param file The file
 * @return true if successful
 */
bool readDimentions(Bitmap *bitmap, FILE *file) {
    // Read first two integers from the file
    // Store them in the bitmap
    if (fscanf(file, "%d %d", &bitmap->rows, &bitmap->columns) != 2) {
        fclose(file);
        return false;
    }
    return true;
}

/**
 * Reads the data of the bitmap from the file
 * @param bitmap The bitmap
 * @param file The file
 * @return true if successful
 */
bool readBitmapData(Bitmap *bitmap, FILE *file) {
    int value;
    for (int row = 0; row < bitmap->rows; row++) {
        for (int col = 0; col < bitmap->columns; col++) {
            // Scans the value from the file
            if (fscanf(file, "%d", &value) != 1) {
                return false;
            }
            // Stores the value in the bitmap
            bitmap->data[row * bitmap->columns + col] = (char)value;
        }
    }

    // Check for extra values in the file
    int extraValue;
    if (fscanf(file, "%d", &extraValue) == 1) {
        return false;
    }

    return true;
}

/**
 * Validates the bitmap for 1s and 0s
 * @param bitmap The bitmap
 * @return true if valid
 *
 */
bool validateBitmap(Bitmap bitmap) {
    if (bitmap.rows <= 0 || bitmap.columns <= 0) {
        return false;
    }
    int color;

    for (int row = 0; row < bitmap.rows; ++row) {
        for (int col = 0; col < bitmap.columns; ++col) {
            color = getValue(bitmap, row, col);
            if (color != 0 && color != 1) {
                return false;
            }
        }
    }

    return true;
}

/**
 * Frees the memory allocated for a bitmap
 * @param bitmap The bitmap to free
 *
 * This function releases the memory allocated for the bitmap's data and resets
 * the bitmap's rows and columns to 0
 */
void freeBitmap(Bitmap *bitmap) {
    if (bitmap == NULL) return;

    free(bitmap->data);
    bitmap->data = NULL;
    bitmap->rows = 0;
    bitmap->columns = 0;
}

/**
 * Prints the contents of a bitmap to the console
 * @param bitmap The bitmap to print
 */
void printBitmap(Bitmap bitmap) {
    for (int row = 0; row < bitmap.rows; row++) {
        for (int col = 0; col < bitmap.columns; col++) {
            printf("%d ", getValue(bitmap, row, col));
        }
        printf("\n");
    }
}

/**
 * Loads a bitmap from a file
 * @param bitmap The bitmap
 * @param filename The filename
 */
bool loadBitmap(Bitmap *bitmap, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return false;
    }
    if (readDimentions(bitmap, file) != true) {
        fclose(file);
        return false;
    }

    // Allocate memory for bitmap data
    bitmap->data = malloc(bitmap->rows * bitmap->columns * sizeof(char));
    if (bitmap->data == NULL) {
        fclose(file);
        return false;
    }
    // Read bitmap data
    if (readBitmapData(bitmap, file) != true) {
        freeBitmap(bitmap);
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}

/**
 * Find longest horizontal line in a bitmap
 * @param bitmap The bitmap in which to search
 * @param start_pos output parameter for the starting position of the longest
 * line
 * @param end_pos output parameter for the ending position of the longest line
 */
void findHline(Bitmap bitmap, Position *start_pos, Position *end_pos) {
    int max_len = 0;
    Position start = {-1, -1};
    Position end = {-1, -1};

    for (int row = 0; row < bitmap.rows; row++) {
        int current_len = 0;
        int current_col_start = -1;

        for (int col = 0; col < bitmap.columns; col++) {
            if (getValue(bitmap, row, col) == 1) {
                // Check if this is the start of a new line
                if (current_len == 0) {
                    current_col_start = col;
                }
                current_len++;
                // Check if this is the longest line
                if (current_len > max_len) {
                    max_len = current_len;
                    start.row = row;
                    start.col = current_col_start;
                    end.row = row;
                    end.col = col;
                }
            } else {
                current_len = 0;
            }
        }
    }
    // Update the output positions
    updatePos(&start, start_pos);
    updatePos(&end, end_pos);
}

/**
 * Find longest vertical line in a bitmap
 * @param bitmap The bitmap in which to search
 * @param found_start_pos output parameter for the starting position of the
 * longest line
 * @param found_end_pos output parameter for the ending position of the longest line
 * @param bitmap The bitmap in which to search
 * This function searches for the longest vertical line in a bitmap
 * Updates the output positions
 */
void findVline(Bitmap bitmap, Position *found_start_pos,
               Position *found_end_pos) {
    // Initialize variables
    int longest_len = 0;
    Position start_pos = {-1, -1};
    Position end_pos = {-1, -1};

    for (int col = 0; col < bitmap.columns; col++) {
        int current_len = 0;
        int current_start_row = -1;
        for (int row = 0; row < bitmap.rows; row++) {
            if (getValue(bitmap, row, col) == 1) {
                // Check if this is the start of a new line
                if (current_len == 0) {
                    current_start_row = row;
                }
                current_len++;
                if (current_len > longest_len) {
                    // Update the longest line and its indices
                    longest_len = current_len;
                    start_pos.row = current_start_row;
                    start_pos.col = col;
                    end_pos.row = row;
                    end_pos.col = col;
                } else if (current_len == longest_len) {
                    // If the current line is as long as the longest line
                    // save the one with the smallest starting row
                    if (row < end_pos.row) {
                        start_pos.row = current_start_row;
                        start_pos.col = col;
                        end_pos.row = row;
                        end_pos.col = col;
                    }
                }
            } else {
                current_len = 0;
            }
        }
    }
    // Update the output positions
    updatePos(&start_pos, found_start_pos);
    updatePos(&end_pos, found_end_pos);
}

/**
 * Finds the largest square in a bitmap
 * @param bitmap The bitmap to search
 *
 * This function searches for the largest square in a bitmap by iterating over
 * each pixel and checking for lines horizontally and vertically
 *          idx
 *      - - - - →
 *  jdx |
 *      |
 *      |
 *      ↓
 * then connect with sqr_col and sqr_row
 */
void findSquare(Bitmap bitmap, Position *found_start_pos,
                Position *found_end_pos) {
    Position square_start = {NOT_FOUND_INDEX, NOT_FOUND_INDEX};
    Position square_end = {NOT_FOUND_INDEX, NOT_FOUND_INDEX};
    Position end_pos = {NOT_FOUND_INDEX, NOT_FOUND_INDEX};
    int longest_len = 0, current_len = 0;
    bool is_square = false, initial_square = false;

    // Loop through all possible starting points
    for (int row = 0; row < bitmap.rows; row++) {
        for (int col = 0; col < bitmap.columns; col++) {
            // Skip if the starting point is not 1
            if (!getValue(bitmap, row, col)) continue;

            // Try to expand the square from the current starting point
            end_pos.row = col;
            end_pos.col = row;
            if (!initial_square) {
                square_start.row = row;
                square_start.col = col;
                square_end.row = end_pos.col;
                square_end.col = end_pos.row;
                initial_square = true;
            }

            for (int idx = col, j_idx = row;
                 idx < bitmap.columns && j_idx < bitmap.rows; idx++, j_idx++) {
                // Check if the horizontal or vertical line is not 1
                if (!getValue(bitmap, row, idx) ||
                    !getValue(bitmap, j_idx, col)) {
                    break;
                }
                // Check if it forms a square

                is_square = true;
                for (int sqr_col = col, sqr_row = row;
                     sqr_col <= idx && sqr_row <= j_idx; sqr_col++, sqr_row++) {
                    if (!getValue(bitmap, j_idx, sqr_col) ||
                        !getValue(bitmap, sqr_row, idx)) {
                        is_square = false;
                        break;
                    }
                }

                // If a square is found, update the end positions
                if (is_square) {
                    end_pos.row = idx;
                    end_pos.col = j_idx;
                }
            }

            // Calculate the current square's size
            current_len = end_pos.row - col;
            if (current_len > longest_len) {
                // Update largest square if the current one is larger
                square_start.row = row;
                square_start.col = col;
                square_end.row = end_pos.col;
                square_end.col = end_pos.row;
                longest_len = current_len;
            }
        }
    }

    // Update the output positions with the largest square found
    updatePos(&square_start, found_start_pos);
    updatePos(&square_end, found_end_pos);
}

void displayHelp() {
    printf("Usage: figsearch [--help] <mode> <bitmap>\n");
    printf("Options:\n");
    printf("\t--help\t\tDisplay this help message\n");
    printf("Modes:\n");
    printf("\ttest\t\tTest the program with a bitmap\n");
    printf("\tvline\t\tFind a vertical line\n");
    printf("\thline\t\tFind a horizontal line\n");
    printf("\tsquare\t\tFind a square\n");
}

/**
 * Function to handle the mode and call the appropriate function
 *
 * @param mode The mode to handle
 * @param bitmap The bitmap to search
 * @param found_start_pos output parameter for the starting position of the
 * longest line
 * @param found_end_pos output parameter for the ending position of the longest
 * line
 */
void handleMode(MODE mode, Bitmap bitmap, Position *found_start_pos,
                Position *found_end_pos) {
    if (mode == TEST) {
        validateBitmap(bitmap);
        printBitmap(bitmap);
    }
    if (mode == VLINE) {
        findVline(bitmap, found_start_pos, found_end_pos);
    }
    if (mode == HLINE) {
        findHline(bitmap, found_start_pos, found_end_pos);
    }
    if (mode == SQUARE) {
        findSquare(bitmap, found_start_pos, found_end_pos);
    }
}

int main(int argc, char *argv[]) {
    // Parse mode and validate arguments
    MODE mode = parseUserInput(argc, argv);

    if (mode == HELP) {
        displayHelp();
        return EXIT_SUCCESS;
    }

    if (mode == INVALID || argc < MAX_ALLOWED_ARGS) {
        fprintf(stderr, "Invalid");
        return EXIT_FAILURE;
    }

    // Load and validate the bitmap
    Bitmap bitmap;
    if (!loadBitmap(&bitmap, argv[2]) || !validateBitmap(bitmap)) {
        fprintf(stderr, "Invalid");
        freeBitmap(&bitmap);
        return EXIT_FAILURE;
    }

    // Initialize positions
    Position found_start_pos = {NOT_FOUND_INDEX, NOT_FOUND_INDEX};
    Position found_end_pos = {NOT_FOUND_INDEX, NOT_FOUND_INDEX};

    // Handle the selected mode
    handleMode(mode, bitmap, &found_start_pos, &found_end_pos);

    // Print results for modes
    if (mode != TEST) {
        printf("%d %d %d %d", found_start_pos.row, found_start_pos.col,
               found_end_pos.row, found_end_pos.col);
    }

    // Free the allocated memory for the bitmap
    freeBitmap(&bitmap);

    return EXIT_SUCCESS;
}
