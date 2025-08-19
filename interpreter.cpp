#include <iostream>
#include <fstream>
#include <string>

void computeCode(std::ifstream& fileCin, unsigned char *array)
{
	// Read the entire file into a single string.
	std::string code((std::istreambuf_iterator<char>(fileCin)), std::istreambuf_iterator<char>());
	int ptr = 0; // Instruction pointer;
	bool firstIO = true;

	for (int i = 0; i < code.size(); i++)
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
				std::cout << array[ptr];
				firstIO = false;
				break;
			case ',':
				//Input 1.
				if (!firstIO)
					std::cout << std::endl << std::flush;
				array[ptr] = std::cin.get();
				if (array[ptr] == '\n')
					array[ptr] = std::cin.get();
				firstIO = false;
				break;
			case '[':
				if (array[ptr] == 0)
				{
					loopLevel = 1; // In case of nested loop.
					while (loopLevel > 0 && ++i < code.size())
					{
						if (code[i] == '[')
							loopLevel++;
						else if (code[i] == ']')
							loopLevel--;
					}
					if (i == code.size() and loopLevel != 0)
					{
						std::cerr << "No matching ']' for loop.";
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
						if (code[i] == ']')
							loopLevel++;
						else if (code[i] == '[')
							loopLevel--;
					}
					if (i < 0 and loopLevel != 0)
					{
						std::cerr << "No matching '[' for loop.";
						exit(-1);
					}
				}
				break;
			default:
				continue;
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		std::ifstream fileCin(argv[1]);

		if (fileCin.fail())
		{
			std::cerr << "Could not open file.";
			exit(-1);
		}

		unsigned char* array = new unsigned char[30000];
		for (int i = 0; i < 30000; i++)
		{
			array[i] = 0;
		}

		computeCode(fileCin, array);

		delete[] array;

		fileCin.close();
	}
}