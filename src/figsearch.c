#include <stdio.h>
#include "string.h"
#include "stdbool.h"
#include <stdlib.h>

//TODO What if input has more numbers than dimension of bitmap
//TODO Refactor code
//TODO Rework findsquare
//TODO Check memory leaks


#define INVALID_POS -1

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

int findLongestLine(Bitmap *bitmap, int start_pos[], int end_pos[],
                    bool is_horizontal, int *longest_start, int *longest_end)
{
    int max_length = 0, current_length = 0;
    int start[2] = {INVALID_POS, INVALID_POS};
    int end[2] = {INVALID_POS, INVALID_POS};
    int longest_start_pos[2] = {INVALID_POS, INVALID_POS};
    int longest_end_pos[2] = {INVALID_POS, INVALID_POS};

    int outer_start = start_pos[is_horizontal ? 0 : 1];
    int outer_end = end_pos[is_horizontal ? 0 : 1];
    int inner_start = start_pos[is_horizontal ? 1 : 0];
    int inner_end = end_pos[is_horizontal ? 1 : 0];

    for (int i = outer_start; i <= outer_end; i++)
    {
        current_length = 0;
        for (int j = inner_start; j <= inner_end; j++)
        {
            int row = is_horizontal ? i : j;
            int col = is_horizontal ? j : i;

            if (bitmap->data[row][col] == 1)
            {
                if (current_length == 0)
                {
                    start[0] = row;
                    start[1] = col;
                }
                current_length++;
                end[0] = row;
                end[1] = col;
            }
            else if (current_length > 0)
            {
                if (current_length > max_length)
                {
                    max_length = current_length;
                    memcpy(longest_start_pos, start, sizeof(longest_start_pos));
                    memcpy(longest_end_pos, end, sizeof(longest_end_pos));
                }
                current_length = 0;
            }
        }
        if (current_length > max_length)
        {
            max_length = current_length;
            memcpy(longest_start_pos, start, sizeof(longest_start_pos));
            memcpy(longest_end_pos, end, sizeof(longest_end_pos));
        }
    }
    memcpy(longest_start, longest_start_pos, sizeof(longest_start_pos));
    memcpy(longest_end, longest_end_pos, sizeof(longest_end_pos));
    return max_length;
}

void findLongestLineBitmap(Bitmap *bitmap, int *start_position, int *end_position, bool search_horizonally)
{
    int longest_length = 0;
    int current_length = 0;
    int start_point[2], end_point[2] = {INVALID_POS, INVALID_POS};
    int current_start_pos[2], current_end_pos[2] = {INVALID_POS, INVALID_POS};

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

        current_length = findLongestLine(bitmap, start_point, end_point, search_horizonally, current_start_pos, current_end_pos);

        if (current_length > longest_length)
        {
            longest_length = current_length;
            memcpy(start_position, current_start_pos, sizeof(current_start_pos));
            memcpy(end_position, current_end_pos, sizeof(current_end_pos));
        }
    }

    printf("Longest line is %d, starting at (%d, %d) and ending at (%d, %d).\n",
           longest_length, start_position[0], start_position[1], end_position[0], end_position[1]);
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
            for (int j = 0; j < i; j++)
            {
                free(bitmap->data[j]);
            }
            free(bitmap->data);
            printf("Memory allocation failed for bitmap[%d] columns.\n", i);
            return false;
        }
    }
    return true;
}

void findSquare(Bitmap *bitmap, int *start_index, int *end_index)
{
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
    if (file == NULL)
    {
        printf("Error opening file: %s\n", filename);
        free(bitmap);
        return NULL;
    }
    // Check if the file was successfully parsed
    if (!getDimentions(file, bitmap) ||
        !alloc_bitmap_data(bitmap) ||
        !read_bitmap_data(file, bitmap))
    {
        // Free the bitmap if there was an error
        fclose(file);
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
//TODO
}

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
        // printf("Start position: (%d, %d)\n", start_position[0], start_position[1]);
        // printf("End position: (%d, %d)\n", end_position[0], end_position[1]);
        break;
    case VLINE:
        bitmap = init_bitmap(argv[2]);
        findVLINE(bitmap, start_position, end_position);
        printf("Start position: (%d, %d)\n", start_position[0], start_position[1]);
        printf("End position: (%d, %d)\n", end_position[0], end_position[1]);
        break;
    case SQUARE:
        bitmap = init_bitmap(argv[2]);
        // findSQUARE(bitmap, start_position, end_position);
        break;
    case TEST:
        bitmap = init_bitmap(argv[2]);
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
