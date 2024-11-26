#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//TODO VLINE START FROM TOP LEFT
//TODO SQUARE
//TODO CLEAN

#define MIN_REQUIRED_ARGS 2
#define MAX_ALLOWED_ARGS 3
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define NOT_FOUND_INDEX 0

// Define the Bitmap struct
// Data is stored in 1D array of chars
typedef struct
{
    int rows;
    int columns;
    char *data;
} Bitmap;

// Define the Position struct
// Used to store row and column
typedef struct
{
    int row;
    int col;
} Position;

typedef enum
{
    INVALID = -1,
    HLINE = 0,
    VLINE = 1,
    SQUARE = 2,
    HELP = 3,
    TEST = 4

} MODE;

// Define the ModeMap struct
// Maps modes to names
typedef struct
{
    char *name;
    MODE mode;
} ModeMap;

/**
 * Parses user input and returns the mode
 * @param argc The number of arguments
 * @param argv The array of arguments
 * @return The mode
 */
MODE parseUserInput(int argc, char *argv[])
{
    if (argc < MIN_REQUIRED_ARGS || argc > MAX_ALLOWED_ARGS)
    {
        return INVALID;
    }
    ModeMap modeMap[] = {
        {"--help", HELP},
        {"test", TEST},
        {"hline", HLINE},
        {"vline", VLINE},
        {"square", SQUARE},
    };
    // Get the size of the modeMap
    int modeMapSize = sizeof(modeMap) / sizeof(modeMap[0]);

    // Iterate over the modeMap
    for (int idx = 0; idx < modeMapSize; idx++)
    {
        // Check if the mode name matches
        if (strcmp(argv[1], modeMap[idx].name) == 0)
        {
            // If mode is help and argc is not equal to MIN_REQUIRED_ARGS
            if ((modeMap[idx].mode == HELP) && argc != MIN_REQUIRED_ARGS)
            {
                return INVALID;
            }
            // mode is hline, vline, square, or test and argc is not equal to MAX_ALLOWED_ARGS
            if ((modeMap[idx].mode == HLINE ||
                 modeMap[idx].mode == VLINE ||
                 modeMap[idx].mode == SQUARE ||
                 modeMap[idx].mode == TEST) &&
                argc != MAX_ALLOWED_ARGS)
            {
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
int getValue(Bitmap bitmap, int row, int column)
{
    return bitmap.data[row * bitmap.columns + column];
}

/**
 * Updates the position
 * @param src The source position
 * @param dst The destination position
 */
void updatePos(Position *src, Position *dst)
{
    dst->row = src->row;
    dst->col = src->col;
}

/**
 * Reads the dimentions of the bitmap from the file
 * @param bitmap The bitmap
 * @param file The file
 * @return true if successful
 */
bool readDimentions(Bitmap *bitmap, FILE *file)
{
    // Read first two integers from the file
    // Store them in the bitmap
    if (fscanf(file, "%d %d", &bitmap->rows, &bitmap->columns) != 2)
    {
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
bool readBitmapData(Bitmap *bitmap, FILE *file)
{
    int value;
    for (int row = 0; row < bitmap->rows; row++)
    {
        for (int col = 0; col < bitmap->columns; col++)
        {
            // Scans the value from the file
            if (fscanf(file, "%d", &value) != 1)
            {
                return false;
            }
            // Stores the value in the bitmap
            bitmap->data[row * bitmap->columns + col] = (char)value;
        }
    }

    // Check for extra values in the file
    int extraValue;
    if (fscanf(file, "%d", &extraValue) == 1)
    {
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
bool validateBitmap(Bitmap bitmap)
{
    if (bitmap.rows <= 0 || bitmap.columns <= 0)
    {
        return false;
    }
    
    for (int row = 0; row < bitmap.rows; row++)
    {
        for (int col = 0; col < bitmap.columns; col++)
        {

            if (getValue(bitmap, row, col) != 0 && getValue(bitmap, row, col) != 1)
            {
                return false;
            }
        }
    }
    return true;
}

/**
 * Loads a bitmap from a file
 * @param bitmap The bitmap
 * @param filename The filename
 */
bool loadBitmap(Bitmap *bitmap, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return false;
    }
    if (readDimentions(bitmap, file) != true)
    {
        fclose(file);
        return false;
    }

    // Allocate memory for bitmap data
    bitmap->data = malloc(bitmap->rows * bitmap->columns * sizeof(char));
    if (bitmap->data == NULL)
    {
        fclose(file);
        return false;
    }
    // Read bitmap data
    if (readBitmapData(bitmap, file) != true)
    {
        free(bitmap->data);
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}

/**
 * Frees the memory allocated for a bitmap
 *
 * @param bitmap The bitmap to free
 *
 * This function releases the memory allocated for the bitmap's data and resets
 * the bitmap's rows and columns to 0
 */
void freeBitmap(Bitmap *bitmap)
{
    free(bitmap->data);
    bitmap->data = NULL;
    bitmap->rows = 0;
    bitmap->columns = 0;
}

/**
 * Prints the contents of a bitmap to the console
 *
 * @param bitmap The bitmap to print
 *
 * This function prints the bitmap's dimensions and then iterates over each pixel,
 * printing its value to the console
 */
void printBitmap(Bitmap bitmap)
{
    for (int row = 0; row < bitmap.rows; row++)
    {
        for (int col = 0; col < bitmap.columns; col++)
        {

            printf("%d ", getValue(bitmap, row, col));
        }
        printf("\n");
    }
}

/**
 * Find longest horizontal line in a bitmap
 *
 * @param bitmap The bitmap in which to search
 * @param found_start_pos output parameter for the starting position of the longest line
 * @param found_end_pos output parameter for the ending position of the longest line.
 *
 * This function searches for the longest horizontal line in a bitmap. It iterates
 * over each row and keeps track of the current length of the line and its starting
 * column. If a line is longer than the current longest, it updates the longest line
 * and its indices. Finally, it updates the output positions.
 */
void findHline(Bitmap bitmap, Position *found_start_pos, Position *found_end_pos)
{
    // Initialize variables
    int longest_len = 0;
    Position start_pos;
    Position end_pos;
    start_pos.row = -1;
    start_pos.col = -1;
    end_pos.row = -1;
    end_pos.col = -1;

    for (int row = 0; row < bitmap.rows; row++)
    {
        int current_len = 0;
        int current_start_col = -1;

        for (int col = 0; col < bitmap.columns; col++)
        {
            if (getValue(bitmap, row, col) == 1)
            {
                // Check if this is the start of a new line
                if (current_len == 0)
                {

                    current_start_col = col;
                }
                current_len++;
                // Check if this line is longer than the longest
                if (current_len > longest_len)
                {

                    // Update the longest line and its indices
                    longest_len = current_len;
                    start_pos.row = row;
                    start_pos.col = current_start_col;
                    end_pos.row = row;
                    end_pos.col = col;
                }
            }
            else
            {

                current_len = 0;
            }
        }
    }
    // Update the output positions
    updatePos(&start_pos, found_start_pos);
    updatePos(&end_pos, found_end_pos);
}

/**
 * Find longest vertical line in a bitmap
 *
 * @param bitmap The bitmap in which to search
 * @param found_start_pos output parameter for the starting position of the longest line
 * @param found_end_pos output parameter for the ending position of the longest line.
 *
 * This function searches for the longest vertical line in a bitmap. It iterates
 * over each column and keeps track of the current length of the line and its starting
 * row. If a line is longer than the current longest, it updates the longest line
 * and its indices. Finally, it updates the output positions.
 */
void findVline(Bitmap bitmap, Position *found_start_pos, Position *found_end_pos)
{
    // Initialize variables
    int longest_len = 0;
    Position start_pos;
    Position end_pos;
    start_pos.row = -1;
    start_pos.col = -1;
    end_pos.row = -1;
    end_pos.col = -1;

    for (int col = 0; col < bitmap.columns; col++)
    {

        int current_len = 0;
        int current_start_row = -1;
        for (int row = 0; row < bitmap.rows; row++)
        {

            if (getValue(bitmap, row, col) == 1)
            {

                // Check if this is the start of a new line
                if (current_len == 0)
                {

                    current_start_row = row;
                }
                current_len++;
                if (current_len > longest_len)
                {

                    // Update the longest line and its indices
                    longest_len = current_len;
                    start_pos.row = current_start_row;
                    start_pos.col = col;
                    end_pos.row = row;
                    end_pos.col = col;
                }
            }
            else
            {
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
 *
 * This function searches for the largest square in a bitmap by iterating over each
 * pixel and checking for continuous lines horizontally and vertically. It then
 * checks if the area forms a square and updates the starting and ending positions
 * of the largest square found.
 *
 * @param bitmap The bitmap to search.
 */
void findSquare(Bitmap bitmap, Position *found_start_pos, Position *found_end_pos)
{
    Position square_start;
    Position square_end;
    Position current_start;
    square_start.row = NOT_FOUND_INDEX;
    square_start.col = NOT_FOUND_INDEX;
    square_end.row = NOT_FOUND_INDEX;
    square_end.col = NOT_FOUND_INDEX;
    current_start.row = NOT_FOUND_INDEX;
    current_start.col = NOT_FOUND_INDEX;

    int longest_len = 0;
    bool isSquare;

    for (int row = 0; row < bitmap.rows; row++)
    {
        for (int col = 0; col < bitmap.columns; col++)
        {
            current_start.row = row;
            current_start.col = col;
            for (int idx = col, j_idx = row; idx < bitmap.columns && j_idx < bitmap.rows; idx++, j_idx++)
            { // moving horizontally and vertically
                if (!getValue(bitmap, row, idx) || !getValue(bitmap, j_idx, col))
                { // checking if the horizontal or vertical lines are not continuous
                    break;
                }
                else
                {
                    isSquare = true;
                    for (int sqr_row = row, sqr_col = col; sqr_row < j_idx + 1 && sqr_col < idx + 1; sqr_col++, sqr_row++)
                    { // check if square
                        if (!getValue(bitmap, j_idx, sqr_col) || !getValue(bitmap, idx, sqr_row))
                        {
                            isSquare = false;
                            break;
                        }
                    }
                    if (isSquare)
                    { // if square then
                        current_start.row = j_idx;
                        current_start.col = idx;
                    }
                }
            }
            if ((current_start.col - col) > longest_len)
            {
                square_start.row = row;
                square_start.col = col;

                updatePos(&current_start, &square_end);
                longest_len = square_end.col - square_start.col;
            }
        }
    }
        // Update the output positions
    updatePos(&square_start, found_start_pos);
    updatePos(&square_end, found_end_pos);

}

void test(Bitmap *bitmap)
{

    int x_start, y_start, x_end, y_end, d_max = 0;
    int i, j, k, l;
    int col_start, line_start, col_end, line_end, checker;

    for (y_start = 0; y_start < bitmap->rows; y_start++)
    {
        for (x_start = 0; x_start < bitmap->columns; x_start++)
        {
            x_end = x_start;
            y_end = y_start;
            for (i = x_start, j = y_start; i < bitmap->columns && j < bitmap->rows; i++, j++)
            { // moving horizontally and vertically
                if (!getValue(*bitmap, i, y_start) || !getValue(*bitmap, x_start, j))
                { // checking if the horizontal or vertical lines are not continuous
                    break;
                }
                else
                {
                    checker = 1;
                    for (k = x_start, l = y_start; k < i + 1 && l < j + 1; k++, l++)
                    { // check if square
                        if (!getValue(*bitmap, k, j) || !getValue(*bitmap, i, l))
                        {
                            checker = 0;
                            break;
                        }
                    }
                    if (checker)
                    { // if square then
                        x_end = i;
                        y_end = j;
                    }
                }
            }
            if ((x_end - x_start) > d_max)
            {
                col_start = x_start;
                line_start = y_start;
                col_end = x_end;
                line_end = y_end;
                d_max = col_end - col_start;
            }
        }
    }
    printf("The largest square is:\n[%d][%d] x [%d][%d]\n", line_start, col_start, line_end, col_end);
}


/**
 * Function to handle the mode and call the appropriate function
 *
 * @param mode The mode to handle
 * @param bitmap The bitmap to search
 * @param found_start_pos output parameter for the starting position of the longest line
 * @param found_end_pos output parameter for the ending position of the longest line.
 */
void handleMode(MODE mode, Bitmap bitmap, Position *found_start_pos, Position *found_end_pos)
{
    if (mode == TEST){
        validateBitmap(bitmap);
        printBitmap(bitmap);
        }
    if (mode == VLINE){
        findVline(bitmap, found_start_pos, found_end_pos);
        }
    if (mode == HLINE){
        findHline(bitmap, found_start_pos, found_end_pos);
        }
    if (mode == SQUARE){
        //test(&bitmap);
        findSquare(bitmap, found_start_pos, found_end_pos);
        }
}

int main(int argc, char *argv[])
{
    // Parse mode and validate arguments
    MODE mode = parseUserInput(argc, argv);

    if (mode == HELP)
    {
        // Handle HELP mode early
        printf("Usage: figsearch [mode] [filename]\n"
       "Modes:\n"
       "    hline: Find a horizontal line\n"
       "    vline: Find a vertical line\n"
       "    square: Find a square\n"
       "    test: Test the program with a bitmap\n"
       "    help: Display this help message\n");
        return EXIT_SUCCESS;
    }

    if (mode == INVALID)
    {
        fprintf(stderr, "Invalid");
        return EXIT_FAILURE;
    }


    // Ensure a file argument is provided for modes requiring a bitmap
    if (argc < MAX_ALLOWED_ARGS)
    {
        fprintf(stderr, "Invalid");
        return EXIT_FAILURE;
    }
    // Load and validate the bitmap
    Bitmap bitmap;
    if (!loadBitmap(&bitmap, argv[2]))
    {
        fprintf(stderr, "Invalid");
        return EXIT_FAILURE;
    }


    if (!validateBitmap(bitmap))
    {
        fprintf(stderr, "Invalid");
        freeBitmap(&bitmap);
        return EXIT_FAILURE;
    }

    // Initialize positions
    Position found_start_pos = {NOT_FOUND_INDEX, NOT_FOUND_INDEX};
    Position found_end_pos = {NOT_FOUND_INDEX, NOT_FOUND_INDEX};

    // Handle the selected mode
    handleMode(mode, bitmap, &found_start_pos, &found_end_pos);

    // Print results for modes that produce coordinates
    if (mode != TEST){
        printf("%d %d %d %d", found_start_pos.row, found_start_pos.col, found_end_pos.row, found_end_pos.col);
    }


    // Free the allocated memory for the bitmap
    freeBitmap(&bitmap);

    return EXIT_SUCCESS;
}
