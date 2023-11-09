#include <iostream>
#include <fstream>
#include <vector>
#include <string>

enum Registers
{
	A, B, C, D, E, F, X, Y
};

enum Instruction
{
	EMPTY, MOV, ADD, SUB, MUL, DIV, MOD, INC, AND, OR, XOR, NOT, SHL, SHR, JMP, JE, JNE, JG, JGE, JL, JLE, CMP, PUSH, POP, CALL, RET
};

enum Addressing_mode
{
	NONE, REG_REG, REG_CONST, REG_RAM, RAM_REG, RAM_CONST, REG, CONST
};

// The c++ standard library contains everything, except for things that are useful
std::vector<std::string> split(std::string line, char delimiter)
{
	std::vector<std::string> strings = {};

	std::string string = "";

	for (int i = 0; i < line.size(); ++i)
	{
		char character = line[i];

		if (character == delimiter)
		{
			if (string != "")
			{
				strings.push_back(string);

				string = "";
			}
		}
		else
		{
			string += character;
		}
	}

	if (string != "")
	{
		strings.push_back(string);
	}

	return strings;
}

std::vector<uint16_t> load_program(std::string file_name)
{
	std::vector<uint16_t> program = {};

	std::ifstream file(file_name);

	std::string line = "";

	while (getline(file, line))
	{
		if (line == "")
		{
			break;
		}


		std::vector<std::string> strings = split(line, ' ');

		for (std::string string : strings)
		{
			uint16_t word = 0;
			uint16_t bit_count = 0;

			for (char character : string)
			{
				if (character == '1')
				{
					word = word | ((1 << 15) >> bit_count);

					++bit_count;
				}
				else if (character == '0')
				{
					++bit_count;
				}
				else if (character != '.')
				{
					std::cout << "Invalid character." << '\n';

					while (true);
				}
			}

			program.push_back(word);
		}
	}

	return program;

	file.close();
}

int main()
{
	// registers
	int registers[6] = { 0 };

	uint16_t SP = 0; // stack pointer
	uint16_t PC = 0; // program counter
	
	bool less_than_flag = false;
	bool greater_than_flag = false;

	std::vector<uint16_t> stack((1 << 16), 0);
	std::vector<uint16_t> RAM((1 << 16), 0);


	std::vector<uint16_t> program = load_program("example_program.txt");

	while (true)
	{
		if (SP < 0 || SP >= stack.size())
		{
			std::cout << "The program went outside the stack." << '\n';
			
			while (true);
		}

		if (PC >= program.size())
		{
			break;
		}

		uint16_t opcode = program[PC];

		Instruction instruction = Instruction(opcode >> 8);
		Addressing_mode addressing_mode = Addressing_mode(opcode & 0x00FF);

		switch (instruction)
		{
		case MOV:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] = registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] = program[PC + 2];
				break;

			case REG_RAM:
				registers[program[PC + 1]] = RAM[registers[program[PC + 2]]];
				break;

			case RAM_REG:
				RAM[registers[program[PC + 1]]] = registers[program[PC + 2]];
				break;

			case RAM_CONST:
				RAM[registers[program[PC + 1]]] = program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case ADD:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] += registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] += program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case SUB:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] -= registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] -= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case MUL:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] *= registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] *= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case DIV:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] /= registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] /= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case MOD:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] %= registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] %= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case INC:
			++registers[program[PC + 1]];

			PC += 2;
			break;

		case AND:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] &= registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] &= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case OR:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] |= registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] |= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case XOR:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] ^= registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] ^= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case NOT:
			registers[program[PC + 1]] = ~registers[program[PC + 2]];
			break;

		case SHL:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] <<= registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] <<= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case SHR:
			switch (addressing_mode)
			{
			case REG_REG:
				registers[program[PC + 1]] >>= registers[program[PC + 2]];
				break;

			case REG_CONST:
				registers[program[PC + 1]] >>= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case JMP:
			PC = program[PC + 1];
			break;

		case JE:
			if (!less_than_flag && !greater_than_flag)
			{
				PC = program[PC + 1];
			}
			else
			{
				PC += 2;
			}
			break;

		case JNE:
			if (less_than_flag || greater_than_flag)
			{
				PC = program[PC + 1];
			}
			else
			{
				PC += 2;
			}
			break;

		case JG:
			if (greater_than_flag)
			{
				PC = program[PC + 1];
			}
			else
			{
				PC += 2;
			}
			break;

		case JGE:
			if (!less_than_flag)
			{
				PC = program[PC + 1];
			}
			else
			{
				PC += 2;
			}
			break;

		case JL:
			if (less_than_flag)
			{
				PC = program[PC + 1];
			}
			else
			{
				PC += 2;
			}
			break;

		case JLE:
			if (!greater_than_flag)
			{
				PC = program[PC + 1];
			}
			else
			{
				PC += 2;
			}
			break;

		case CMP:
			switch (addressing_mode)
			{
			case REG_REG:
				greater_than_flag = false;
				less_than_flag = false;

				if (registers[program[PC + 1]] - registers[program[PC + 2]] > 0)
				{
					greater_than_flag = true;
				}
				if (registers[program[PC + 1]] - registers[program[PC + 2]] < 0)
				{
					less_than_flag = true;
				}
				break;

			case REG_CONST:
				greater_than_flag = false;
				less_than_flag = false;

				if (registers[program[PC + 1]] - program[PC + 2] > 0)
				{
					greater_than_flag = true;
				}
				if (registers[program[PC + 1]] - program[PC + 2] < 0)
				{
					less_than_flag = true;
				}
				break;
			}

			PC += 3;
			break;

		case PUSH:
			switch (addressing_mode)
			{
			case REG:
				++SP;
				stack[SP] = registers[program[PC + 1]];
				break;

			case CONST:
				++SP;
				stack[SP] = program[PC + 1];
				break;
			}

			PC += 2;
			break;

		case POP:
			registers[program[PC + 1]] = stack[SP];
			--SP;

			PC += 2;
			break;

		case CALL:
			++SP;
			stack[SP] = PC + 2;

			PC = program[PC + 1];
			break;

		case RET:
			registers[program[PC + 1]] = stack[SP];

			PC += 2;
			break;
		}
	}

	// print values in registers
	for (int i = 0; i < 6; ++i)
	{
		std::cout << "Register " << i << ": " << registers[i] << '\n';
	}

	// print values in firt 100 words of RAM
	std::cout << "\n\n" << "RAM:\n\n";

	for (int i = 0; i < 100; ++i)
	{
		std::cout << RAM[i] << '\n';
	}

	while (true);

	return 0;
}