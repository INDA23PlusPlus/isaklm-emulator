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
	MOV, LDR, ADD, SUB, MUL, DIV, MOD, INC, AND, OR, XOR, NOT, SHL, SHR, JMP, JE, JNE, JG, JGE, JL, JLE, CMP, PUSH, POP, CALL, RET
};

enum Addressing_mode
{
	IMPLIED, REG, CONST, RAM
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

void call_parse_error(int i)
{
	std::cout << "Could not parse instruction at line: " << i << '\n';

	while (true);
}

uint16_t get_opcode(uint16_t instruction, uint16_t addressing_mode)
{
	return (instruction << 8) | addressing_mode;
}

uint16_t get_constant(std::string str, int index)
{
	if (str[0] != '#' || str.size() == 1 || str.size() > 5)
	{
		call_parse_error(index);
	}

	std::string binary = "";

	for (int i = 1; i < str.size(); ++i)
	{
		char digit = str[i];

		switch (digit)
		{
		case '0': binary += "0000"; break;
		case '1': binary += "0001"; break;
		case '2': binary += "0010"; break;
		case '3': binary += "0011"; break;
		case '4': binary += "0100"; break;
		case '5': binary += "0101"; break;
		case '6': binary += "0110"; break;
		case '7': binary += "0111"; break;
		case '8': binary += "1000"; break;
		case '9': binary += "1001"; break;
		case 'A': binary += "1010"; break;
		case 'B': binary += "1011"; break;
		case 'C': binary += "1100"; break;
		case 'D': binary += "1101"; break;
		case 'E': binary += "1110"; break;
		case 'F': binary += "1111"; break;
		default: call_parse_error(index); break;
		}
	}

	uint16_t integer = 0;

	for (int i = 0; i < binary.size(); ++i)
	{
		if (binary[binary.size() - i - 1] == '1')
		{
			integer += (1 << i);
		}
	}

	return integer;
}

uint16_t get_register(std::string str, int i)
{
	if (str == "A")
	{
		return A;
	}
	else if (str == "B")
	{
		return B;
	}
	else if (str == "C")
	{
		return C;
	}
	else if (str == "D")
	{
		return D;
	}
	else if (str == "E")
	{
		return E;
	}
	else if (str == "F")
	{
		return F;
	}
	else if (str == "X")
	{
		return X;
	}
	else if (str == "Y")
	{
		return Y;
	}
	else
	{
		call_parse_error(i);
	}
}

uint16_t get_pointer(std::string str, int i)
{
	if (str == "&A")
	{
		return A;
	}
	else if (str == "&B")
	{
		return B;
	}
	else if (str == "&C")
	{
		return C;
	}
	else if (str == "&D")
	{
		return D;
	}
	else if (str == "&E")
	{
		return E;
	}
	else if (str == "&F")
	{
		return F;
	}
	else if (str == "&X")
	{
		return X;
	}
	else if (str == "&Y")
	{
		return Y;
	}
	else
	{
		call_parse_error(i);
	}
}

struct Label
{
	std::string name;
	uint16_t program_counter;
};

void add_label(uint16_t PC, std::string name, std::vector<Label>& labels, int i)
{
	for (Label label : labels)
	{
		if (label.name == name)
		{
			call_parse_error(i);
		}
	}

	if (name.back() != '\"')
	{
		call_parse_error(i);
	}

	labels.push_back({ name, PC });
}

uint16_t get_label(std::string name, std::vector<Label> labels, int i)
{
	for (Label label : labels)
	{
		if (label.name == name)
		{
			return label.program_counter;
		}
	}

	call_parse_error(i);
}

std::vector<Label> load_labels(std::string file_name)
{
	std::vector<Label> labels = {};


	std::ifstream file(file_name);

	std::string line = "";

	uint16_t PC = 0;

	int i = 0;

	while (getline(file, line))
	{
		++i;

		if (line == "")
		{
			continue;
		}

		std::vector<std::string> strings = split(line, ' ');

		std::string identifier = strings[0];

		if (identifier[0] == '\"' && strings.size() == 1)
		{
			add_label(PC, identifier, labels, i);
		}
		else
		{
			PC += strings.size();
		}
	}

	file.close();

	return labels;
}

std::vector<uint16_t> load_program(std::string file_name)
{
	std::vector<Label> labels = load_labels(file_name);

	std::vector<uint16_t> program = {};


	std::ifstream file(file_name);

	std::string line = "";

	int i = 0;

	while (getline(file, line))
	{
		++i;

		if (line == "")
		{
			continue;
		}

		std::vector<std::string> strings = split(line, ' ');

		std::string identifier = strings[0];

		if (identifier[0] == '\"')
		{
			continue;
		}
	    else if (identifier == "MOV" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg1[0] == '&')
			{
				program.push_back(get_opcode(MOV, RAM));
				program.push_back(get_pointer(arg1, i));
				program.push_back(get_register(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(MOV, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_pointer(arg2, i));
			}
		}
		else if (identifier == "LDR" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(LDR, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(LDR, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "ADD" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(ADD, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(ADD, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "SUB" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(SUB, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(SUB, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "MUL" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(MUL, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(MUL, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "DIV" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(DIV, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(DIV, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "MOD" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(MOD, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(MOD, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "INC" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			program.push_back(get_opcode(INC, IMPLIED));
			program.push_back(get_register(arg1, i));
		}
		else if (identifier == "AND" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(AND, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(AND, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "OR" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(OR, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(OR, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "XOR" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(XOR, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(XOR, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "NOT" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			program.push_back(get_opcode(NOT, IMPLIED));
			program.push_back(get_register(arg1, i));
			program.push_back(get_register(arg2, i));
		}
		else if (identifier == "SHL" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(SHL, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(SHL, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "SHR" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(SHR, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(SHR, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "JMP" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			program.push_back(get_opcode(JMP, IMPLIED));
			program.push_back(get_label(arg1, labels, i));
		}
		else if (identifier == "JE" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			program.push_back(get_opcode(JE, IMPLIED));
			program.push_back(get_label(arg1, labels, i));
		}
		else if (identifier == "JNE" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			program.push_back(get_opcode(JNE, IMPLIED));
			program.push_back(get_label(arg1, labels, i));
		}
		else if (identifier == "JG" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			program.push_back(get_opcode(JG, IMPLIED));
			program.push_back(get_label(arg1, labels, i));
		}
		else if (identifier == "JGE" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			program.push_back(get_opcode(JGE, IMPLIED));
			program.push_back(get_label(arg1, labels, i));
		}
		else if (identifier == "JL" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			program.push_back(get_opcode(JL, IMPLIED));
			program.push_back(get_label(arg1, labels, i));
		}
		else if (identifier == "JLE" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			program.push_back(get_opcode(JLE, IMPLIED));
			program.push_back(get_label(arg1, labels, i));
		}
		else if (identifier == "CMP" && strings.size() == 3)
		{
			std::string arg1 = strings[1];
			std::string arg2 = strings[2];

			if (arg2[0] == '#')
			{
				program.push_back(get_opcode(CMP, CONST));
				program.push_back(get_register(arg1, i));
				program.push_back(get_constant(arg2, i));
			}
			else
			{
				program.push_back(get_opcode(CMP, REG));
				program.push_back(get_register(arg1, i));
				program.push_back(get_register(arg2, i));
			}
		}
		else if (identifier == "PUSH" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			if (arg1[0] == '#')
			{
				program.push_back(get_opcode(PUSH, CONST));
				program.push_back(get_constant(arg1, i));
			}
			else
			{
				program.push_back(get_opcode(PUSH, REG));
				program.push_back(get_register(arg1, i));
			}
		}
		else if (identifier == "POP" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			program.push_back(get_opcode(POP, IMPLIED));
			program.push_back(get_register(arg1, i));
		}
		else if (identifier == "CALL" && strings.size() == 2)
		{
			std::string arg1 = strings[1];

			program.push_back(get_opcode(CALL, IMPLIED));
			program.push_back(get_label(arg1, labels, i));
		}
		else if (identifier == "RET" && strings.size() == 1)
		{
			program.push_back(get_opcode(RET, IMPLIED));
		}
		else
		{
			call_parse_error(i);
		}
	}

	file.close();

	return program;
}

int main()
{
	// registers
	int registers[8] = { 0 };

	uint16_t SP = 0; // stack pointer
	uint16_t PC = 0; // program counter
	
	bool less_than_flag = false;
	bool greater_than_flag = false;

	std::vector<uint16_t> stack((1 << 16), 0);
	std::vector<uint16_t> memory((1 << 16), 0);


	std::vector<uint16_t> program = load_program("primes.txt");

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
			case REG:
				registers[program[PC + 1]] = memory[registers[program[PC + 2]]];
				break;

			case RAM:
				memory[registers[program[PC + 1]]] = registers[program[PC + 2]];
				break;
			}

			PC += 3;
			break;

		case LDR:
			switch (addressing_mode)
			{
			case REG:
				registers[program[PC + 1]] = registers[program[PC + 2]];
				break;

			case CONST:
				registers[program[PC + 1]] = program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case ADD:
			switch (addressing_mode)
			{
			case REG:
				registers[program[PC + 1]] += registers[program[PC + 2]];
				break;

			case CONST:
				registers[program[PC + 1]] += program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case SUB:
			switch (addressing_mode)
			{
			case REG:
				registers[program[PC + 1]] -= registers[program[PC + 2]];
				break;

			case CONST:
				registers[program[PC + 1]] -= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case MUL:
			switch (addressing_mode)
			{
			case REG:
				registers[program[PC + 1]] *= registers[program[PC + 2]];
				break;

			case CONST:
				registers[program[PC + 1]] *= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case DIV:
			switch (addressing_mode)
			{
			case REG:
				registers[program[PC + 1]] /= registers[program[PC + 2]];
				break;

			case CONST:
				registers[program[PC + 1]] /= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case MOD:
			switch (addressing_mode)
			{
			case REG:
				registers[program[PC + 1]] %= registers[program[PC + 2]];
				break;

			case CONST:
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
			case REG:
				registers[program[PC + 1]] &= registers[program[PC + 2]];
				break;

			case CONST:
				registers[program[PC + 1]] &= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case OR:
			switch (addressing_mode)
			{
			case REG:
				registers[program[PC + 1]] |= registers[program[PC + 2]];
				break;

			case CONST:
				registers[program[PC + 1]] |= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case XOR:
			switch (addressing_mode)
			{
			case REG:
				registers[program[PC + 1]] ^= registers[program[PC + 2]];
				break;

			case CONST:
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
			case REG:
				registers[program[PC + 1]] <<= registers[program[PC + 2]];
				break;

			case CONST:
				registers[program[PC + 1]] <<= program[PC + 2];
				break;
			}

			PC += 3;
			break;

		case SHR:
			switch (addressing_mode)
			{
			case REG:
				registers[program[PC + 1]] >>= registers[program[PC + 2]];
				break;

			case CONST:
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
			case REG:
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

			case CONST:
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
			PC = stack[SP];
			break;
		}
	}

	
	std::cout << "Register A: " << registers[0] << '\n';
	std::cout << "Register B: " << registers[1] << '\n';
	std::cout << "Register C: " << registers[2] << '\n';
	std::cout << "Register D: " << registers[3] << '\n';
	std::cout << "Register E: " << registers[4] << '\n';
	std::cout << "Register F: " << registers[5] << '\n';
	std::cout << "Register X: " << registers[6] << '\n';
	std::cout << "Register Y: " << registers[7] << '\n';

	// print values in firt 100 words of RAM
	std::cout << "\n\n" << "RAM:\n\n";

	for (int i = 0; i < 100; ++i)
	{
		std::cout << memory[i] << '\n';
	}

	while (true);

	return 0;
}