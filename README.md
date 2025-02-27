# 📐 Project: FigSearch - Shape Detection in Bitmap 🖼️

# ✔️ Mark: 12.5/14:00 (-1.5 was only because of my own inattention, explained down below)

## 🚀 Project Description

The goal of this project is to create a program that detects the **longest horizontal and vertical lines** 🔴, and the **largest squares** 🔳, in a given **monochromatic image**. The image is stored as a **bitmap** in a text file in the form of a **rectangular matrix** of zeros and ones. The program should print the starting and ending coordinates of the detected line or square.

## 💻 Implementation

Implement the program in the source file **figsearch.c**. Submit the source file through the information system.

### 🔧 Compilation and Submission

To compile and test the program, use the following command:

```bash
$ cc -std=c11 -Wall -Wextra -Werror figsearch.c -o figsearch
```

## 🖥️ Command Syntax
Help message
```bash
./figsearch --help
```

## 🧰 Program Arguments
- --help: 📚 Displays help information about how to use the program and exits.
- test: 🧐 Verifies that the file provided as the second argument contains a valid bitmap definition. If the format is correct, it prints "Valid". Otherwise, it prints "Invalid".
- hline: 🔴 Finds and prints the starting and ending coordinates of the first longest horizontal line.
- vline: 🔵 Finds and prints the starting and ending coordinates of the first longest vertical line.
- square: 🔳 Finds and prints the starting and ending coordinates of the first largest square.
- FILE: 📂 The name of the file containing the bitmap image.


### Bitmap Format

The bitmap is stored in a text file containing numerical values separated by whitespace. The first two numerical values represent the image dimensions (height and width). Following these are rows containing pixel color values, where each pixel is either 0 (white) or 1 (black). The coordinates (0, 0) represent the top-left corner.

### Shape Definitions

- **Horizontal/Vertical Line**: A continuous sequence of adjacent black pixels on the same row/column.
- **Longest Horizontal/Vertical Line**: The line with the most black pixels.
- **Largest Square**: A sequence of adjacent black pixels forming the boundary of the largest possible square.

### Program Output

The program will print the coordinates in the following format:
```bash
R1 C1 R2 C2
```

Where R1, C1 is the starting coordinate, and R2, C2 is the ending coordinate.

If the file does not contain any black pixels, the program will print "Not found".

### Input and Output Examples

**Example Bitmap (image.txt):**
```bash
4 5 0 0 1 1 1 0 0 1 0 1 1 0 1 1 1 1 1 1 1 1
```


**Example Program Usage:**

**First longest horizontal line:**

```bash
$ ./figsearch hline image.txt
3 0 3 4
```




