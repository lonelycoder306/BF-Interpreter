#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// readFile adapted from "Crafting Interpreters" clox.
char* readFile(const char* path)
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
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

void computeCode(const char* path, unsigned char* array)
{
    char* code = readFile(path);
	int ptr = 0;
	bool firstIO = true;
	int size = (int) strlen(code);

	for (int i = 0; i < size; i++)
	{
		int loopLevel;
		switch (code[i])
		{
			case '>':
				//Forward 1.
				ptr++;
				break;
			case '<':
				//Back 1.
				ptr--;
				break;
			case '+':
				//Add 1.
				array[ptr]++;
				break;
			case '-':
				//Minus 1.
				if (array[ptr] != 0) // Limit checking.
					array[ptr]--;
				break;
			case '.':
				//Print 1.
				printf("%c", array[ptr]);
				firstIO = false;
				break;
			case ',':
				//Input 1.
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
				break;
			case '[':
				if (array[ptr] == 0)
				{
					loopLevel = 1; // In case of nested loop.
					while (loopLevel > 0 && ++i < size)
					{
						if (code[i] == '[') loopLevel++;
						else if (code[i] == ']') loopLevel--;
					}
					if (i == size && loopLevel != 0)
					{
						perror("No matching ']' for loop.");
						exit(-1);
					}
				}
				break;
			case ']':
				if (array[ptr] != 0)
				{
					loopLevel = 1;
					while (loopLevel > 0 && --i >= 0)
					{
						if (code[i] == ']') loopLevel++;
						else if (code[i] == '[') loopLevel--;
					}
					if (i < 0 && loopLevel != 0)
					{
						perror("No matching '[' for loop.");
						exit(-1);
					}
				}
				break;
		}
	}
}

int main(int argc, char** argv)
{
	if (argc == 2)
	{
		unsigned char* array = calloc(30000, sizeof(unsigned char));
		computeCode(argv[1], array);
		free(array);
	}

	return 0;
}