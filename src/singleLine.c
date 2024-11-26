#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    int rows;
    int columns;
    char *data;
} Bitmap;

typedef enum
{
    INVALID = -1,
    HLINE = 0,
    VLINE = 1,
    SQUARE = 2,
    HELP = 3,
    TEST = 4

} MODE;

typedef struct
{
    char *name;
    MODE mode;
} ModeMap;

// Define ANSI color codes
#define ANSI_RESET "\x1b[0m"
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"

// Clear the console screen
void clearScreen()
{
    printf("\033[H\033[J");
}
MODE parseUserInput(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
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
    size_t modeMapSize = sizeof(modeMap) / sizeof(modeMap[0]);

    // Iterate over the mode mappings to find a match
    for (size_t i = 0; i < modeMapSize; i++)
    {
        if (strcmp(argv[1], modeMap[i].name) == 0)
        {
            // For modes requiring a second argument

            if ((modeMap[i].mode == HELP) && argc != 2)
            {
                return INVALID;
            }

            if (modeMap[i].mode == HLINE ||
                modeMap[i].mode == VLINE ||
                modeMap[i].mode == SQUARE ||
                modeMap[i].mode == TEST)
            {
                if (argc != 3)
                {
                    return INVALID;
                }
            }
            return modeMap[i].mode;
        }
    }

    return INVALID;
}

int getPixelValue(Bitmap *bitmap, int row, int column)
{
    return bitmap->data[row * bitmap->columns + column];
}

bool readDimentions(Bitmap *bitmap, FILE *file)
{
    // Read dimensions
    if (fscanf(file, "%d %d", &bitmap->rows, &bitmap->columns) != 2)
    {
        fprintf(stderr, "Failed to read bitmap dimensions\n");
        fclose(file);
        return false;
    }
    return true;
}

bool readBitmapData(Bitmap *bitmap, FILE *file)
{

    for (int i = 0; i < bitmap->rows; i++)
    {
        for (int j = 0; j < bitmap->columns; j++)
        {
            int value;
            if (fscanf(file, "%d", &value) != 1)
            {
                fprintf(stderr, "Error: Missing data for specified dimensions.\n");
                return false;
            }
            bitmap->data[i * bitmap->columns + j] = (char)value;
        }
    }

    // Check for extra values in the file
    int extraValue;
    if (fscanf(file, "%d", &extraValue) == 1)
    {
        fprintf(stderr, "Error: Extra data found beyond specified dimensions.\n");
        return false;
    }

    return true;
}

bool validateBitmap(Bitmap *bitmap)
{
    for (int i = 0; i < bitmap->rows; i++)
    {
        for (int j = 0; j < bitmap->columns; j++)
        {
            if (getPixelValue(bitmap, i, j) != 0 && getPixelValue(bitmap, i, j) != 1)
            {
                return false;
            }
        }
    }

    return true;
}

// Function to load a bitmap from a file
bool loadBitmap(const char *filename, Bitmap *bitmap)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file\n");
        return false;
    }
    if (readDimentions(bitmap, file) != true)
    {
        fprintf(stderr, "Failed to read bitmap dimensions\n");
        fclose(file);
        return false;
    }

    // Allocate memory for bitmap data
    bitmap->data = malloc(bitmap->rows * bitmap->columns * sizeof(char));
    if (bitmap->data == NULL)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        fclose(file);
        return false;
    }
    if (readBitmapData(bitmap, file) != true)
    {
        fprintf(stderr, "Failed to read bitmap data\n");
        free(bitmap->data);
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

// Print bitmap with live updates
void printBitmapLive(Bitmap *bitmap, int currentRow, int currentStartCol, int currentEndCol, int line_start[2], int line_end[2])
{
    clearScreen();
    printf("Bitmap (%d x %d):\n", bitmap->rows, bitmap->columns);
    for (int i = 0; i < bitmap->rows; i++)
    {
        for (int j = 0; j < bitmap->columns; j++)
        {
            int value = getPixelValue(bitmap, i, j);

            if (i == line_start[0] && j >= line_start[1] && j <= line_end[1])
            {
                // Longest horizontal line found so far in green
                printf(ANSI_GREEN "%d " ANSI_RESET, value);
            } // Check if this is part of the vertical line
            else if (j == line_start[1] && i >= line_start[0] && i <= line_end[0])
            {
                printf(ANSI_GREEN "%d " ANSI_RESET, value);
            }
            else if (i == currentRow && j >= currentStartCol && j <= currentEndCol)
            {
                // Current line being evaluated in yellow
                printf(ANSI_YELLOW "%d " ANSI_RESET, value);
            }
            else if (j == currentRow && i >= currentStartCol && i <= currentEndCol)
            {
                // Current line being evaluated in yellow
                printf(ANSI_YELLOW "%d " ANSI_RESET, value);
            }
            else if (value == 1)
            {
                // Other 1s in red
                printf(ANSI_RED "%d " ANSI_RESET, value);
            }
            else
            {
                // Zeros in default color
                printf("%d ", value);
            }
        }
        printf("\n");
    }
    usleep(200000); // Pause for 200 milliseconds
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
void printBitmap(Bitmap *bitmap)
{
    printf("Bitmap (%d x %d):\n", bitmap->rows, bitmap->columns);
    for (int i = 0; i < bitmap->rows; i++)
    {
        for (int j = 0; j < bitmap->columns; j++)
        {
            printf("%d ", getPixelValue(bitmap, i, j));
        }
        printf("\n");
    }
}

void findHline(Bitmap *bitmap, int found_start_pos[2], int found_end_pos[2])
{
    int longest_len = 0;
    int start_pos[2] = {-1, -1}, end_pos[2] = {-1, -1};

    // Find the longest horizontal line and track its indices
    for (int row = 0; row < bitmap->rows; row++)
    {
        int current_len = 0;
        int current_start_col = -1;
        for (int col = 0; col < bitmap->columns; col++)
        {
            if (getPixelValue(bitmap, row, col) == 1)
            {
                if (current_len == 0)
                {
                    current_start_col = col;
                }
                current_len++;
                if (current_len > longest_len)
                {
                    longest_len = current_len;
                    start_pos[0] = row;
                    start_pos[1] = current_start_col;
                    end_pos[0] = row;
                    end_pos[1] = col;
                }
            }
            else
            {
                current_len = 0;
            }
            printBitmapLive(bitmap, row, current_start_col, col, start_pos, end_pos);
        }
    }
    found_start_pos[0] = start_pos[0];
    found_start_pos[1] = start_pos[1];
    found_end_pos[0] = end_pos[0];
    found_end_pos[1] = end_pos[1];
    printf("Longest horizontal line: %d\n", longest_len);
    printf("Start position: (%d, %d)\n", start_pos[0], start_pos[1]);
    printf("End position: (%d, %d)\n", end_pos[0], end_pos[1]);
}

void findVline(Bitmap *bitmap, int found_start_pos[2], int found_end_pos[2])
{
    int longest_len = 0;
    int start_pos[2] = {-1, -1}, end_pos[2] = {-1, -1};

    // Find the longest horizontal line and track its indices
    for (int col = 0; col < bitmap->columns; col++)
    {
        int current_len = 0;
        int current_start_row = -1;
        for (int row = 0; row < bitmap->rows; row++)
        {
            if (getPixelValue(bitmap, row, col) == 1)
            {
                if (current_len == 0)
                {
                    current_start_row = row;
                }
                current_len++;
                if (current_len > longest_len)
                {
                    longest_len = current_len;
                    start_pos[0] = current_start_row;
                    start_pos[1] = col;
                    end_pos[0] = row;
                    end_pos[1] = col;
                }
            }
            else
            {
                current_len = 0;
            }
            printBitmapLive(bitmap, col, current_start_row, row, start_pos, end_pos);
        }
    }
    found_start_pos[0] = start_pos[0];
    found_start_pos[1] = start_pos[1];
    found_end_pos[0] = end_pos[0];
    found_end_pos[1] = end_pos[1];
    printf("Longest Vertical line: %d\n", longest_len);
    printf("Start position: (%d, %d)\n", start_pos[0], start_pos[1]);
    printf("End position: (%d, %d)\n", end_pos[0], end_pos[1]);
}

bool calculateDiagonals(Bitmap *bitmap, int start_pos[])
{
    printf("Start position: (%d, %d)\n", start_pos[0], start_pos[1]);
    int length = bitmap->rows > bitmap->columns ? bitmap->columns : bitmap->rows;
    int length_to_go;
    printf("Length: %d\n", length);
    for (int i = 0; i < length; i++)
    {
        if (getPixelValue(bitmap, start_pos[0] + length - i, start_pos[1] + length - i) == 1)
        {
            printf("(%d, %d)\n", start_pos[0] + length - i, start_pos[1] + length - i);
            length_to_go = (start_pos[0] + length - i) * 2 - 1;

            for (int row = start_pos[0] + length - i; row > 0; row--)
            {
                for (int col = start_pos[1] + length - i; col > 0; col--)
                {
                    if (getPixelValue(bitmap, row, col) == 1 && getPixelValue(bitmap, col, row) == 1)
                    {
                        length_to_go--;
                        printf("length to go: %d\n", length_to_go);
                    }
                    if (length_to_go == 0)
                    {
                        printf("Found diagonal\n");
                        printf("Start position: (%d, %d)\n", start_pos[0], start_pos[1]);
                        printf("End position: (%d, %d)\n", start_pos[0] + length - i, start_pos[1] + length - i);
                        return true;
                    }
                }
            }
        }

        length_to_go = length * 2;
    }
    return false;
}

bool findSquares(Bitmap *bitmap, int found_start_pos[2], int found_end_pos[2])
{
    int current_start_row;
    int current_start_col;
    int start_pos[2] = {-1, -1}, end_pos[2] = {-1, -1};
    int current_len;
    int longest_len = 0;
    for (int row = 0; row < bitmap->rows-1; row++)
    {
        current_start_row = -1;
        current_start_col = -1;
        current_len = 0;
        for (int col = 0; col < bitmap->columns-1; col++)
        {
            if ((getPixelValue(bitmap, row, col) == 1) && (getPixelValue(bitmap, col, row) == 1))
            {
                if (current_len == 0)
                {
                    current_start_row = row;
                    current_start_col = col;
                }
                current_len++;
                if (current_len > longest_len)
                {
                    longest_len = current_len;
                    start_pos[0] = current_start_row;
                    start_pos[1] = current_start_col;
                }
            }
            else
            {
                current_len = 0;
            }
            // printBitmapLive(bitmap, row, current_start_col, col, start_pos, end_pos);
        }
    }
    printf("Longest square: %d\n", longest_len);
    printf("Longest Start position: (%d, %d)\n", start_pos[0], start_pos[1]);
    return false;
}

// Function to print the bitmap with colors
void printBitmapWithColors(Bitmap *bitmap, int line_start[2], int line_end[2])
{
    printf("\nColorful Bitmap (%d x %d):\n", bitmap->rows, bitmap->columns);
    int pixel_value;

    for (int i = 0; i < bitmap->rows; i++)
    {
        for (int j = 0; j < bitmap->columns; j++)
        {
            pixel_value = getPixelValue(bitmap, i, j);

            // Check if this is part of the horizontal line
            if (i == line_start[0] && j >= line_start[1] && j <= line_end[1])
            {
                printf(ANSI_GREEN "%d " ANSI_RESET, pixel_value);
            }
            // Check if this is part of the vertical line
            else if (j == line_start[1] && i >= line_start[0] && i <= line_end[0])
            {
                printf(ANSI_GREEN "%d " ANSI_RESET, pixel_value);
            }
            else
            {
                // Use red for `1` and default for `0`
                if (pixel_value == 1)
                {
                    printf(ANSI_RED "%d " ANSI_RESET, pixel_value);
                }
                else
                {
                    printf("%d ", pixel_value);
                }
            }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    Bitmap bitmap;
    // MODE mode = parseUserInput(argc, argv);

    // Load the bitmap from the file
    if (loadBitmap(argv[2], &bitmap) != true)
    {
        fprintf(stderr, "Failed to load bitmap: invalid data or dimensions\n");
        return 1;
    }
    if (!validateBitmap(&bitmap))
    {
        fprintf(stderr, "Failed to load bitmap: invalid data or dimensions\n");
        return 1;
    }

    // Print the bitmap
    printBitmap(&bitmap);
    int found_start_pos[2], found_end_pos[2];

    findSquares(&bitmap, found_start_pos, found_end_pos);
    printBitmapWithColors(&bitmap, found_start_pos, found_end_pos);

    // Free the allocated memory
    freeBitmap(&bitmap);

    return 0;
}
