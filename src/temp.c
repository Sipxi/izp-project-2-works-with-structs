#include <stdio.h>
#include "string.h"
#include "stdbool.h"
#include <stdlib.h>

//TODO What if input has more numbers than dimension of bitmap
//TODO Refactor code
//TODO Rework findsquare
//TODO Check memory leaks

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

typedef struct
{
    int rows;
    int columns;
    int **data;
} Bitmap;

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

            if (modeMap[i].mode == HLINE || modeMap[i].mode == VLINE || modeMap[i].mode == SQUARE || modeMap[i].mode == TEST)
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

void updatePos(int *src, int *dst)
{
    dst[0] = src[0];
    dst[1] = src[1];
}

int findLongestLine(Bitmap *bitmap, int start_index[],
                    int end_index[], bool search_horizonally, int *found_start_pos,
                    int *found_end_pos)
{
    int current_len;
    int longest_len = 0;
    int current_start_pos[2] = {-1, -1}; 
    int current_end_pos[2] = {-1, -1};
    int longest_start_pos[2] = {-1, -1};
    int longest_end_pos[2] = {-1, -1};

    int inner_start = search_horizonally ? start_index[1] : start_index[0];
    int inner_end = search_horizonally ? end_index[1] : end_index[0];
    int outer_start = search_horizonally ? start_index[0] : start_index[1];
    int outer_end = search_horizonally ? end_index[0] : end_index[1];

    for (int i = outer_start; i <= outer_end; i++)
    {
        int found_start = false;
        for (int j = inner_start; j <= inner_end; j++)
        {
            int row = search_horizonally ? i : j;
            int col = search_horizonally ? j : i;

            if (bitmap->data[row][col] == 1)
            {
                if (!found_start)
                {
                    current_len = 1;
                    current_start_pos[0] = row;
                    current_start_pos[1] = col;
                    found_start = true;
                }
                else
                {

                    current_len++;
                }
                current_end_pos[0] = row;
                current_end_pos[1] = col;
            }
            else
            {
                if (found_start && (current_len > longest_len))
                {
                    longest_len = current_len;
                    updatePos(longest_start_pos, current_start_pos);
                    updatePos(longest_end_pos, current_end_pos);
                }
                found_start = false;
                current_len = 0;
            }
        }
        if (found_start && (current_len > longest_len))
        {
            longest_len = current_len;
            updatePos(longest_start_pos, current_start_pos);
            updatePos(longest_end_pos, current_end_pos);
        }
    }

    // Set start and end pointers before returning
    updatePos(found_start_pos, longest_start_pos);
    updatePos(found_end_pos, longest_end_pos);

    return longest_len;
}

void findLongestLineBitmap(Bitmap *bitmap, int *start_position, int *end_position, bool search_horizonally)
{
    int longest_len, current_len = 0;
    int start_point[2], end_point[2] = {-1, -1};
    int current_start_pos[2];
    int current_end_pos[2];

    int outer_loop = search_horizonally ? bitmap->rows : bitmap->columns;

    for (int i = 0; i < outer_loop; i++)
    {
        if (search_horizonally)
        {
            start_point[0] = i;
            start_point[1] = 0;
            end_point[0] = i;
            end_point[1] = bitmap->columns - 1;
        }
        else
        {
            start_point[0] = 0;
            start_point[1] = i;
            end_point[0] = bitmap->rows - 1;
            end_point[1] = i;
        }

        current_len = findLongestLine(bitmap, start_point, end_point, search_horizonally, current_start_pos, current_end_pos);

        if (current_len > longest_len)
        {
            longest_len = current_len;
            updatePos(start_position, current_start_pos);
            updatePos(end_position, current_end_pos);
        }
    }

    printf("Longest line is %d, starting at (%d, %d) and ending at (%d, %d).\n",
           longest_len, start_position[0], start_position[1], end_position[0], end_position[1]);
}

void findHLINE(Bitmap *bitmap, int *start_position, int *end_position)
{
    findLongestLineBitmap(bitmap, start_position, end_position, true);
}

void findVLINE(Bitmap *bitmap, int *start_position, int *end_position)
{
    findLongestLineBitmap(bitmap, start_position, end_position, false);
}

/**
 * Frees the memory allocated for a bitmap
 * @param bitmap The bitmap to free
 * @return void
 */
void free_bitmap(Bitmap *bitmap)
{
    // Free each row
    for (int i = 0; i < bitmap->rows; i++)
    {
        free(bitmap->data[i]);
    }
    // Free the array of row pointers and the bitmap structure
    free(bitmap->data);
    free(bitmap);
}

/**
 * Reads the dimensions of a bitmap from a file
 * @param file The file to read from
 * @param bitmap The bitmap to read into
 * @return true if successful, false if an error occurs
 */
bool getDimentions(FILE *file, Bitmap *bitmap)
{
    // Read the first two integers (dimensions) from the file
    if (fscanf(file, "%d %d", &bitmap->rows, &bitmap->columns) != 2)
    {
        printf("Error reading dimensions from file.\n");
        return false;
    }
    return true;
}

/**
 * Allocates memory for the bitmap data
 * @param bitmap The bitmap to allocate memory for
 * @return true if successful, false if an error occurs
 */
bool alloc_bitmap_data(Bitmap *bitmap)
{
    // Allocate memory for the array of row
    bitmap->data = (int **)malloc(bitmap->rows * sizeof(int *));
    if (bitmap->data == NULL)
    {
        printf("Memory allocation failed for bitmap rows.\n");
        return false;
    }

    // Allocate memory for each column
    for (int i = 0; i < bitmap->rows; i++)
    {
        bitmap->data[i] = (int *)malloc(bitmap->columns * sizeof(int));
        if (bitmap->data[i] == NULL)
        {
            printf("Memory allocation failed for bitmap[%d] columns.\n", i);
            return false;
        }
    }
    return true;
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
                if (!bitmap->data[y_start][i] || !bitmap->data[j][x_start])
                { // checking if the horizontal or vertical lines are not continuous
                    break;
                }
                else
                {
                    checker = 1;
                    for (k = x_start, l = y_start; k < i + 1 && l < j + 1; k++, l++)
                    { // check if square
                        if (!bitmap->data[j][k] || !bitmap->data[l][i])
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

void findSquares(Bitmap *bitmap)
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
                if (!bitmap->data[y_start][i] || !bitmap->data[j][x_start])
                { // checking if the horizontal or vertical lines are not continuous
                    break;
                }
                else
                {
                    checker = 1;
                    for (k = x_start, l = y_start; k < i + 1 && l < j + 1; k++, l++)
                    { // check if square
                        if (!bitmap->data[j][k] || !bitmap->data[l][i])
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
 * Reads the bitmap data from a file
 * @param file The file to read from
 * @param bitmap The bitmap to read into
 * @return true if successful, false if an error occurs
 */
bool read_bitmap_data(FILE *file, Bitmap *bitmap)
{
    for (int i = 0; i < bitmap->rows; i++)
    {
        for (int j = 0; j < bitmap->columns; j++)
        {
            // Read the data in integer format and store it in the bitmap
            if (fscanf(file, "%d", &bitmap->data[i][j]) != 1)
            {
                printf("Error reading data at (%d, %d) from file.\n", i, j);
                return false;
            }
            // Check if the data is valid, 0 or 1
            if (bitmap->data[i][j] != 0 && bitmap->data[i][j] != 1)
            {
                printf("Invalid data at (%d, %d) from file.\n", i, j);
                return false;
            }
        }
    }
    return true;
}

/**
 * Initializes a bitmap from a file.
 * @param filename The name of the file to read the bitmap from.
 * @return Bitmap* A pointer to the initialized bitmap, or NULL if an error occurs.
 */
Bitmap *init_bitmap(char *filename)
{
    // Allocate memory for the bitmap
    Bitmap *bitmap = malloc(sizeof(Bitmap));
    if (bitmap == NULL)
    {
        printf("Memory allocation failed for Bitmap.\n");
        return NULL;
    }

    FILE *file = fopen(filename, "r");
    // Check if the file was successfully parsed
    if (!getDimentions(file, bitmap) ||
        !alloc_bitmap_data(bitmap) ||
        !read_bitmap_data(file, bitmap) ||
        file == NULL)
    {
        // Free the bitmap if there was an error
        free_bitmap(bitmap);
        return NULL;
    }

    fclose(file);
    return bitmap;
}

void print_bitmap(Bitmap *bitmap)
{
    for (int i = 0; i < bitmap->rows; i++)
    {
        for (int j = 0; j < bitmap->columns; j++)
        {
            printf("%d ", bitmap->data[i][j]);
        }
        printf("\n");
    }
}

bool validateBitmap(Bitmap *bitmap)
{
    int summ = bitmap->rows * bitmap->columns;
    for (int i = 0; i < bitmap->rows; i++)
    {
        for (int j = 0; j < bitmap->columns; j++)
        {
            if (!(bitmap->data[i][j] != 0 && bitmap->data[i][j] != 1))
            {
                summ--;
            }
        }
    }
    if (summ != 0)
    {
        printf("Invalid bitmap\n");
        return false;
    }
    


    return true;
}


/*
// Function to print the bitmap with colors
void printBitmapWithColors(Bitmap bitmap, Position line_start, Position line_end)
{
    printf("\nColorful Bitmap (%d x %d):\n", bitmap.rows, bitmap.columns);
    int pixel_value;

    for (int i = 0; i < bitmap.rows; i++)
    {
        for (int j = 0; j < bitmap.columns; j++)
        {
            pixel_value = getValue(bitmap, i, j);

            // Check if this is part of the horizontal line
            if (i == line_start.row && j >= line_start.col && j <= line_end.col)
            {
                printf(ANSI_GREEN "%d " ANSI_RESET, pixel_value);
            }
            // Check if this is part of the vertical line
            else if (j == line_start.col && i >= line_start.row && i <= line_end.row)
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
*/

int main(int argc, char *argv[])
{
    MODE mode = parseUserInput(argc, argv);
    Bitmap *bitmap;

    int start_position[2], end_position[2];
    switch (mode)
    {
    case HLINE:
        bitmap = init_bitmap(argv[2]);
        findHLINE(bitmap, start_position, end_position);
        printf("Start position: (%d, %d)\n", start_position[0], start_position[1]);
        printf("End position: (%d, %d)\n", end_position[0], end_position[1]);
        break;
    case VLINE:
        bitmap = init_bitmap(argv[2]);
        findVLINE(bitmap, start_position, end_position);
        printf("Start position: (%d, %d)\n", start_position[0], start_position[1]);
        printf("End position: (%d, %d)\n", end_position[0], end_position[1]);
        break;
    case SQUARE:
        bitmap = init_bitmap(argv[2]);
        findSquares(bitmap);
        // test(bitmap);
        break;
    case TEST:
        bitmap = init_bitmap(argv[2]);
        validateBitmap(bitmap);
        print_bitmap(bitmap); // Print the bitmap's 2D array
        break;
    case INVALID:
        printf("Invalid mode.\n");
        break;
    case HELP:
        printf("Usage: figsearch [mode] [filename]\n");
        break;
    }
    if (bitmap)
    {
        free_bitmap(bitmap); // Free the allocated memory
    }

    return 0;
}
