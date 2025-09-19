#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char* array;
char* code;
int ptr;

// readFile adapted from "Crafting Interpreters" clox.
static char* readFile(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char *) malloc(fileSize + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static bool readIn(unsigned char* array, int ptr, bool firstIO)
{
    if (!firstIO)
    {
        printf("\n");
        // Flush the buffer.
        char buffer[1024];
        fgets(buffer, sizeof(buffer), stdin);
    }
    array[ptr] = (unsigned char) getchar();
    if (array[ptr] == '\n') array[ptr] = (unsigned char) getchar();
    firstIO = false;
    return firstIO;
}

static void loopOpen(int* pos, int size, int* loopLevel)
{
    if (array[ptr] == 0)
    {
        *loopLevel = 1; // In case of nested loop.
        while (*loopLevel > 0 && ++(*pos) < size)
        {
            if (code[*pos] == '[') (*loopLevel)++;
            else if (code[*pos] == ']') (*loopLevel)--;
        }
        if (*pos == size && *loopLevel != 0)
        {
            fprintf(stderr, "No matching ']' for loop.");
            exit(EXIT_FAILURE);
        }
    }
}

static void loopClose(int* pos, int size, int* loopLevel)
{
    if (array[ptr] != 0)
    {
        *loopLevel = 1;
        while (*loopLevel > 0 && --(*pos) >= 0)
        {
            if (code[*pos] == ']') (*loopLevel)++;
            else if (code[*pos] == '[') (*loopLevel)--;
        }
        if (*pos < 0 && *loopLevel != 0)
        {
            fprintf(stderr, "No matching '[' for loop.");
            exit(EXIT_FAILURE);
        }
    }
}

static void computeCode(const char* path, unsigned char* array)
{
    code = readFile(path);
	ptr = 0;
	bool firstIO = true;
	int size = (int) strlen(code);

	for (int i = 0; i < size; i++)
	{
		int loopLevel;
		switch (code[i])
		{
			case '>':
                // Forward 1.
                if (++ptr >= 30000)
                {
                    fprintf(stderr, "Pointer overflow.");
                    exit(EXIT_FAILURE);
                }
                break;
			case '<':
                // Back 1.
                if (--ptr < 0)
                {
                    fprintf(stderr, "Pointer overflow.");
                    exit(EXIT_FAILURE);
                }
                break;
			case '+': array[ptr]++; break; // Add 1.
			case '-': array[ptr]--; break; // Minus 1.
			case '.':
				//Print 1.
				printf("%c", array[ptr]);
				firstIO = false;
				break;
			case ',':
				//Input 1.
				firstIO = readIn(array, ptr, firstIO);
				break;
			case '[':
                loopOpen(&i, size, &loopLevel);
				break;
			case ']':
				loopClose(&i, size, &loopLevel);
				break;
		}
	}

    free(code);
}

int main(int argc, char** argv)
{
	if (argc == 2)
	{
		array = calloc(30000, sizeof(unsigned char));
		computeCode(argv[1], array);
		free(array);
	}

	return 0;
}
