#include "disassemble.h"

const char *RepeatPrefixes[]			= {"LOCK", "REP", "REPE", "REPZ", "REPNE", "REPNZ"};
const char *AddressPrefixes[]			= {"QWORD", "FWORD", "DWORD", "WORD", "BYTE"};
const char *SegmentRegisters[]			= {"ES", "CS", "SS", "DS", "FS", "GS"};
const char *Register32[]				= {"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"};
const char *Register16[]				= {"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI"};
const char *Register8[]					= {"AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH"};
const char *Address16[]					= {"BX+SI", "BX+DI", "BP+SI", "BP+DI", "SI", "DI", "BP", "BX"};

const char *ArithmeticMnemonic[]		= {"ADD", "OR", "ADC", "SBB", "AND", "SUB", "XOR", "CMP" };
const char *BCDAdjustMnemonic[]			= {"DAA", "DAS", "AAA", "AAS", "AAM", "AAD"};
const char *JxxxMnemonic[]				= {"JO", "JB", "JZ", "JBE", "JS", "JP", "JL", "JLE"};
const char *JnxxMnemonic[]				= {"JNO", "JNB", "JNZ", "JA", "JNS", "JNP", "JGE", "JG"};
const char *StrMnemonic[]				= {"", "", "MOVS", "CMPS", "", "STOS", "LODS", "SCAS"};
const char *LogicalShiftMnemonic[]		= {"ROL", "ROR", "RCL", "RCR", "SHL", "SHR", "SAL", "SAR"};
const char *LoopMnemonic[]				= {"LOOPNZ", "LOOPZ", "LOOP"};
const char *LogicalArithmeticMnemonic[] = {"TEST", "???", "NOT", "NEG", "MUL", "IMUL", "DIV", "IDIV"};
const char *FlagMnemonic[]				= {"CLC", "STC", "CLI", "STI", "CLD", "STD"};
const char *FFOpcodeMnemonic[]			= {"INC", "DEC", "CALL", "CALL", "JMP", "JMP", "PUSH"};
const char *SetxxxMnemonic[]			= {"SETO", "SETB", "SETE", "SETBE", "SETS", "SETPE", "SETL", "SETLE"};
const char *SetnxxMnemonic[]			= {"SETNO", "SETNB", "SETNE", "SETA", "SETNS", "SETPO", "SETGE", "SETG"};
const char *BTMnemonic[]				= {"BT", "BTS", "BTR", "BTC"};
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#pragma warning(disable:4800)
unsigned char *DisassembleSize(unsigned int LinearAddress, unsigned char *Code, PINSTRUCTION Instruction, char *InstructionStr);
DWORD ControlEIP_Code(char *buf,DWORD BaseAddress)
{
	unsigned char *code=(unsigned char*)buf;
	//char cout[257]={NULL};
//	DWORD BaseAddress;
	unsigned char *currentInstruction, *lastInstruction;
	INSTRUCTION Instruction;
	char InstructionStr[MAX_INSTRUCTION_LEN];
//	BaseAddress = addr;

	currentInstruction = lastInstruction = code;
	currentInstruction = Disassemble((unsigned int)(BaseAddress + lastInstruction - code), lastInstruction, &Instruction, InstructionStr);
		char prefix[MAX_OPERAND_LEN] = "";
	char mnemonic[MAX_MNEMONIC_LEN] = "";
	char operand1[MAX_OPERAND_LEN] = "";
	char operand2[MAX_OPERAND_LEN] = "";
	char operand3[MAX_OPERAND_LEN] = "";
				/* call jmp*/
	BYTE *currentCode = code;
	switch(currentCode[0])
	{
		case 0xE8://call
			currentCode++;
			return *(DWORD*)currentCode;

		case 0xE9://jmp

			currentCode++;
			return *(DWORD*)(currentCode)+(DWORD)(BaseAddress+currentInstruction - lastInstruction);
			
		case 0xEA://jmp
			currentCode++;
			currentCode = ParseImmediate(currentCode, &Instruction, operand2);
			currentCode = ParseImmediate(currentCode, &Instruction, operand3);
			wsprintfA(operand1, "FAR %s:%s", operand3, operand2);
			*operand2 = '\0';
			*operand3 = '\0';

			break;
		
		case 0xEB://jmp
			//wsprintfA(mnemonic, "JMP");
			currentCode++;
			wsprintfA(operand1, "SHORT %X", Instruction.LinearAddress + *currentCode + currentCode - lastInstruction + 1);
			currentCode++;

			break;
		case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
		case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E: case 0x7F:
		
			
			
			//wsprintfA(mnemonic, "%s", *currentCode & 1 ? JnxxMnemonic[(*currentCode >> 1) & 7] : JxxxMnemonic[(*currentCode >> 1) & 7]);
			currentCode++;
			wsprintfA(operand1, "SHORT %X", Instruction.LinearAddress + *((char*)currentCode) + currentCode - lastInstruction + 1);
			//currentCode++;

			break;
		
		}
	return 0;
}
DWORD GetCodeSizeEx(char *CodeOut,DWORD BaseAddress)
{

	unsigned char *code=(unsigned char *)CodeOut;//=(unsigned char*)buf;
	__asm
	{
		MOV EBX,BaseAddress
		MOV EDX,code
		MOV EAX, [EBX]
		MOV [EDX],EAX
		MOV EAX, [EBX+4]
		MOV [EDX+4],EAX
		MOV EAX, [EBX+8]
		MOV [EDX+8],EAX
	}
	//char cout[257]={NULL};
	//DWORD BaseAddress;
	unsigned char *currentInstruction, *lastInstruction;
	INSTRUCTION Instruction;
	char InstructionStr[MAX_INSTRUCTION_LEN];
	//BaseAddress = addr;
	
	//int cnt=0;
	//for(cnt = 0; cnt < sectionNumber; cnt++)
	//{
			/* ·´»ã±à */

			currentInstruction = lastInstruction = (unsigned char*)code;
		//	while(lastInstruction - code <= sizeof(code))
		//	{
				currentInstruction = DisassembleSize((unsigned int)(BaseAddress + lastInstruction - code), lastInstruction, &Instruction, InstructionStr);

		
				//printf("%X\t", BaseAddress + lastInstruction - code);
			//	for( ; lastInstruction <currentInstruction; lastInstruction++)
			//	{
			//		sprintf(cout,"%X%X ", (*lastInstruction >> 4) & 0xF, (*lastInstruction) & 0xF);
			//	}
				//printf(":\t\t\t");

				//printf("%s\n", InstructionStr);
		//	}
		//	break;
		
	//}
	return (DWORD)(currentInstruction - lastInstruction);
}
DWORD GetCodeSize(BYTE *opCode,DWORD BaseAddress)
{

	unsigned char *code=(unsigned char*)opCode;
	//char cout[257]={NULL};
	//DWORD BaseAddress;
	unsigned char *currentInstruction, *lastInstruction;
	INSTRUCTION Instruction;
	char InstructionStr[MAX_INSTRUCTION_LEN];
	//BaseAddress = addr;
	
	//int cnt=0;
	//for(cnt = 0; cnt < sectionNumber; cnt++)
	//{
			/* ·´»ã±à */

			currentInstruction = lastInstruction = code;
		//	while(lastInstruction - code <= sizeof(code))
		//	{
				currentInstruction = Disassemble((unsigned int)(BaseAddress + lastInstruction - code), lastInstruction, &Instruction, InstructionStr);

		
				//printf("%X\t", BaseAddress + lastInstruction - code);
			//	for( ; lastInstruction <currentInstruction; lastInstruction++)
			//	{
			//		sprintf(cout,"%X%X ", (*lastInstruction >> 4) & 0xF, (*lastInstruction) & 0xF);
			//	}
				//printf(":\t\t\t");

				//printf("%s\n", InstructionStr);
		//	}
		//	break;
		
	//}
	return (DWORD)(currentInstruction - lastInstruction);
}
unsigned char *Disassemble(unsigned int LinearAddress, unsigned char *Code, PINSTRUCTION Instruction, char *InstructionStr)
{
	char prefix[MAX_OPERAND_LEN] = "";
	char mnemonic[MAX_MNEMONIC_LEN] = "";
	char operand1[MAX_OPERAND_LEN] = "";
	char operand2[MAX_OPERAND_LEN] = "";
	char operand3[MAX_OPERAND_LEN] = "";
	
	unsigned char *currentCode;
	char prefixFlag; 

	currentCode = Code;
	prefixFlag = 0;
	
	/* initialize Instrction structure */

	Instruction->RepeatPrefix = -1;
	Instruction->SegmentPrefix = -1;
	Instruction->OperandPrefix = -1;
	Instruction->AddressPrefix = -1;

	Instruction->Opcode = -1;
	Instruction->ModRM = -1;
	Instruction->SIB = -1;
	
	Instruction->Displacement = -1;
	Instruction->Immediate = -1;

	Instruction->dFlag = Instruction->wFlag = Instruction->sFlag = -1;

	Instruction->LinearAddress = LinearAddress;

	/* check PREFIXES and save the values, note that prefixes in the same group can only appear once */
	/* and the values assigned to xxxxPrefix in Instruction structure are the indexes of the names */
	/* in corresponding name tables */
	
	while(*currentCode == 0xF0 || *currentCode == 0xF2 || *currentCode == 0xF3 ||
		*currentCode == 0x2E || *currentCode == 0x36 || *currentCode == 0x3E ||
		*currentCode == 0x26 || *currentCode == 0x64 || *currentCode == 0x65 ||
		*currentCode == 0x66 || *currentCode == 0x67)
	{
		switch(*currentCode)
		{
			/* group1: lock and repeat prefixes */

			case 0xF0 :
			{	
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX LOCK:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->RepeatPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->RepeatPrefix = 0;
						currentCode++;
					}
				}
				
				break;
			}
			case 0xF2 :
			{
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX REPNZ:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->RepeatPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->RepeatPrefix = 5;
						currentCode++;
					}
				}

				break;
			}
			case 0xF3 :
			{
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX REP:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->RepeatPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->RepeatPrefix = 1;
						currentCode++;
					}
				}

				break;
			}

			/* group2: segment override prefixes */

			case 0x2E :
			{
				
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX CS:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 1;
						currentCode++;
					}
				}

				break;
			}
			case 0x36 :
			{
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX SS:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 2;
						currentCode++;
					}
				}

				break;
			}
			case 0x3E :
			{
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX DS:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 3;
						currentCode++;
					}
				}

				break;
			}
			case 0x26 :
			{	
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX ES:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 0;
						currentCode++;
					}
				}

				break;
			}
			case 0x64 :
			{
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX FS:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */
				
					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 4;
						currentCode++;
					}
				}

				break;
			}
			case 0x65 :
			{	
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX GS:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 5;
						currentCode++;
					}
				}

				break;
			}

			/* group3: Operand-size override prefixes */

			case 0x66 :
			{
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX DATASIZE:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->OperandPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->OperandPrefix = 0;
						currentCode++;
					}
				}

				break;
			}

			/* group4: Address-size override prefixes */

			case 0x67 :
			{
				if(prefixFlag)
				{
					lstrcpyA(InstructionStr, "PREFIX ADDRSIZE:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->AddressPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->AddressPrefix = 0;
						currentCode++;
					}
				}
				
				break;
			}
		}
	}

	/* CODE map */

	switch(*currentCode)
	{
		/* Arithmetic operations */

		case 0x00: case 0x01: case 0x02: case 0x03: 
		case 0x08: case 0x09: case 0x0A: case 0x0B: 
		case 0x10: case 0x11: case 0x12: case 0x13: 
		case 0x18: case 0x19: case 0x1A: case 0x1B: 
		case 0x20: case 0x21: case 0x22: case 0x23: 
		case 0x28: case 0x29: case 0x2A: case 0x2B: 
		case 0x30: case 0x31: case 0x32: case 0x33: 
		case 0x38: case 0x39: case 0x3A: case 0x3B: 
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >> 1) & 1;
			Instruction->wFlag = (*currentCode) & 1;

			wsprintfA(mnemonic, ArithmeticMnemonic[(*currentCode >> 3) & 0x1F]);
			
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			
			break;
		}
		case 0x04: case 0x05: case 0x0C: case 0x0D:	
		case 0x14: case 0x15: case 0x1C: case 0x1D:
		case 0x24: case 0x25: case 0x2C: case 0x2D:
		case 0x34: case 0x35: case 0x3C: case 0x3D:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = (*currentCode & 1);
			Instruction->sFlag = ! Instruction->wFlag;

			wsprintfA(mnemonic, ArithmeticMnemonic[(*currentCode >> 3) & 0x1F]);
			
			wsprintfA(operand1, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "AX" : "EAX")  : "AL");

			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);
			
			break;
		}
		case 0x80: case 0x81: case 0x82: case 0x83:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = (*currentCode & 1);
			Instruction->sFlag = (*currentCode >> 1) & 1;
			
			/* special cases */

			if(*currentCode == 0x80)
			{
				Instruction->sFlag = 1;
			}

			wsprintfA(mnemonic, ArithmeticMnemonic[(*(currentCode + 1) >> 3) & 7]);

			currentCode++;
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			currentCode = ParseImmediate(currentCode, Instruction, operand2);

			/* special cases */

			if(Instruction->Opcode == 0x83)
			{
				wsprintfA(operand2, "%X%X", (Instruction->Immediate >> 7 & 1) ? (Instruction->OperandPrefix >= 0 ? 0xFF: 0xFFFFFF) : 0x0, Instruction->Immediate);
			}

			break;
		}
		
		/* push pop operations */

		case 0x06: case 0x07 : case 0x0E: case 0x16: case 0x17: case 0x1E : case 0x1F:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", *currentCode & 1 ? "POP" : "PUSH");
			wsprintfA(operand1, "%s", SegmentRegisters[(*currentCode >> 3) & 3]);
			currentCode++;

			break;
		}
		case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
		case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 1 ? "POP" : "PUSH");
			wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[*currentCode & 7] : Register32[*currentCode & 7]);
			currentCode++;
			
			break;
		}
		case 0x60: case 0x61: case 0x9C: case 0x9D:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s%c%c", (*currentCode & 1) ? "POP" : "PUSH", 
				(*currentCode >> 7) & 1 ? 'F' : 'A', Instruction->OperandPrefix >= 0 ? 'W' : 'D');
			currentCode++;

			break;
		}
		case 0x68: case 0x6A:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = (*currentCode >> 1) & 1;
			
			wsprintfA(mnemonic, "%s", "PUSH");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);

			break;
		}
		case 0x8F:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = (*currentCode) & 1;

			wsprintfA(mnemonic, "%s", "POP");
			currentCode++;
			currentCode = ParseModRM(currentCode, Instruction, operand1);

			break;
		}
		case 0x27: case 0x2F: case 0x37: case 0x3F:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", BCDAdjustMnemonic[(*currentCode >> 3) & 3]);
			currentCode++;

			break;
		}

		/* inc and dec */
		
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 1 ? "DEC" : "INC");
			wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[*currentCode & 7] : Register32[*currentCode & 7]);
			currentCode++;
			
			break;
		}

		/* bound */

		case 0x62:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = (*currentCode >> 1) & 1;

			wsprintfA(mnemonic, "%s", "BOUND");
			wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[(*(currentCode + 1) >> 3) & 7] :
				Register32[(*(currentCode + 1) >> 3) & 7]);
			currentCode++;
			currentCode = ParseModRM(currentCode, Instruction, operand2);
			
			/* special case, the size of memory address must be twice the size of register */

			wsprintfA(operand3, "%s%s", Instruction->OperandPrefix >= 0 ? "DWORD " : "QWORD ",
				strstr(operand2, "PTR"));
			wsprintfA(operand2, "%s", operand3);
			*operand3 = '\0';

			break;
		}

		/* arpl */

		case 0x63:
		{
			Instruction->Opcode = *currentCode;

			/* special case, the operand size of arpl must be 16-bits */

			Instruction->OperandPrefix = 0;
			Instruction->wFlag = 1;
			Instruction->dFlag = 0;

			wsprintfA(mnemonic, "%s", "ARPL");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);

			break;
		}
		
		/* imul */

		case 0x69: case 0x6B:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = (*currentCode >> 1) & 1;
			Instruction->dFlag = (*currentCode >> 1) & 1;

			wsprintfA(mnemonic, "%s", "IMUL");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			currentCode = ParseImmediate(currentCode, Instruction, operand3);

			break;
		}
		
		/* ins and outs */

		case 0x6C: case 0x6D: case 0x6E: case 0x6F:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->dFlag = (*currentCode >> 1) & 1;
			
			wsprintfA(prefix, "%s", Instruction->RepeatPrefix > 0 ? RepeatPrefixes[Instruction->RepeatPrefix] : "");
			wsprintfA(mnemonic, "%s%c", Instruction->dFlag ? "OUTS" : "INS", Instruction->wFlag ? 
				(Instruction->OperandPrefix >= 0 ? 'W' : 'D') : 'B');
			currentCode++;

			break;
		}
		
		/* jmp instructions j(n)xx */
		
		case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
		case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E: case 0x7F:
		{
			Instruction->Opcode = *currentCode;
			
			wsprintfA(mnemonic, "%s", *currentCode & 1 ? JnxxMnemonic[(*currentCode >> 1) & 7] : JxxxMnemonic[(*currentCode >> 1) & 7]);
			currentCode++;
			wsprintfA(operand1, "SHORT %X", Instruction->LinearAddress + *((char*)currentCode) + currentCode - Code + 1);
			currentCode++;

			break;
		}

		/* test */

		case 0x84: case 0x85:
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >>1) & 1;
			Instruction->wFlag = (*currentCode) & 1;

			wsprintfA(mnemonic, "%s", "TEST");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);

			break;
		}

		/* xchg */

		case 0x86: case 0x87:
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >>1) & 1;
			Instruction->wFlag = (*currentCode) & 1;

			wsprintfA(mnemonic, "%s", "XCHG");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			
			break;
		}
		case 0x90:
		{
			Instruction->Opcode = *currentCode;
			
			wsprintfA(mnemonic, "%s", Instruction->RepeatPrefix == 1 ? "PAUSE" : "NOP");
			currentCode++;

			break;
		}
		case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", "XCHG");
			wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[*currentCode & 7] : Register32[*currentCode & 7]);
			wsprintfA(operand2, "%s", Instruction->OperandPrefix >= 0 ? "AX" :"EAX");
			currentCode++;

			break;
		}

		/* mov */

		case 0x88: case 0x89: case 0x8A: case 0x8B:
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >>1) & 1;
			Instruction->wFlag = (*currentCode) & 1;

			wsprintfA(mnemonic, "%s", "MOV");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			
			break;
		}
		case 0x8C: case 0x8E:
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >> 1) & 1;

			/* special cases code segment registers are 16-bits long */
			
			Instruction->OperandPrefix = 0;
			Instruction->wFlag = 1;

			wsprintfA(mnemonic, "%s", "MOV");
			currentCode++;
			wsprintfA(Instruction->dFlag ? operand1 : operand2, "%s", SegmentRegisters[(*currentCode >> 3) & 7]);
			currentCode = ParseModRM(currentCode, Instruction, Instruction->dFlag ? operand2 : operand1);

			break;
		}
		case 0xA0: case 0xA1: case 0xA2: case 0xA3:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->dFlag = (*currentCode >> 1) & 1;

			wsprintfA(mnemonic, "%s", "MOV");
			currentCode++;
			wsprintfA(Instruction->dFlag ? operand1 : operand2, "%s%s%s[%X]", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "WORD PTR " : "DWORD PTR ") : "BYTE PTR ",
				Instruction->SegmentPrefix >= 0 ? SegmentRegisters[Instruction->SegmentPrefix] : "", 
				Instruction->SegmentPrefix >= 0 ? ":" : "",
				*((unsigned int *)currentCode));
			wsprintfA(Instruction->dFlag ? operand2 : operand1, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "AX" : "EAX") : "AL");
			currentCode += 4;

			break;
		}
		case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7:
		case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: case 0xBF:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = (*currentCode >>3) & 1;
			Instruction->sFlag = !(Instruction->wFlag);

			wsprintfA(mnemonic, "%s", "MOV");
			wsprintfA(operand1, Instruction->wFlag ? Instruction->OperandPrefix >= 0 ? Register16[*currentCode & 7] : Register32[*currentCode & 7] : Register8[*currentCode & 7]);
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);

			break;
		}

		/* lea */

		case 0x8D:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = (*currentCode) & 1;

			/*special cases */

			Instruction->dFlag = 1;
			
			wsprintfA(mnemonic, "%s", "LEA");
			currentCode++;	
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);

			break;
		}
		
		/* cbw, cwd, cwde, cdq*/

		case 0x98:
		{
			Instruction->Opcode = *currentCode;
			
			wsprintfA(mnemonic, "%s", Instruction->OperandPrefix >= 0 ? "CBW" : "CWDE");
			currentCode++;

			break;
		}
		case 0x99:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", Instruction->OperandPrefix >= 0 ? "CWD" : "CDQ");
			currentCode++;

			break;
		}

		/* call far ptr16 : 32 */

		case 0x9A:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 0;

			wsprintfA(mnemonic, "%s", "CALL");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);
			Instruction->OperandPrefix = 0;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);
			wsprintfA(operand3, "FAR %s : %s", operand2, operand1);
			lstrcpynA(operand1, operand3, MAX_OPERAND_LEN);
			*operand2 = '\0';
			*operand3 = '\0';

			break;
		}

		/* wait */

		case 0x9B:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", "WAIT");
			currentCode++;

			break;
		}
		
		/* lahf and sahf */

		case 0x9E:
		{
			Instruction->Opcode = *currentCode;
			
			wsprintfA(mnemonic, "%s", "ASHF");
			currentCode++;

			break;
		}
		case 0x9F:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", "LAHF");
			currentCode++;
			
			break;
		}
		
		/* string operations */

		case 0xA4: case 0xA5: case 0xA6: case 0xA7:
		case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAE: case 0xAF:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			
			wsprintfA(prefix, "%s", Instruction->RepeatPrefix > 0 ? RepeatPrefixes[Instruction->RepeatPrefix] : "");
			wsprintfA(mnemonic, "%s%c", StrMnemonic[(*currentCode >> 1) & 7], Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? 'W' : 'D') : 'B');
			currentCode++;

			break;
		}

		/* test */

		case 0xA8: case 0xA9:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = !(*currentCode & 1);

			wsprintfA(mnemonic, "%s", "TEST");
			wsprintfA(operand1, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "AX" : "EAX") : "AL");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);
			
			break;
		}
		
		/* logical shift */

		case 0xC0: case 0xC1:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = 1;
			
			currentCode++;
			wsprintfA(mnemonic, "%s", LogicalShiftMnemonic[(*currentCode >> 3) & 7]);
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			currentCode = ParseImmediate(currentCode, Instruction, operand2);

			break;
		}
		
		/* retn */

		case 0xC2 :
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 0;
			Instruction->OperandPrefix = 0;

			wsprintfA(mnemonic, "RETN");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);

			break;
		}
		case 0xC3:
		{
			Instruction->Opcode = *currentCode;
			
			wsprintfA(mnemonic, "RETN");
			currentCode++;

			break;
		}
		
		/* les, lds */

		case 0xC4: 
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = 1;
			Instruction->dFlag = 1;

			wsprintfA(mnemonic, "LES");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			wsprintfA(operand3, "%s%s", Instruction->OperandPrefix >= 0 ? "DWORD " : "FWORD ", strstr(operand2, "PTR"));
			wsprintfA(operand2, "%s", operand3);
			*operand3 = '\0';

			break;
		}
		case 0xC5:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = 1;
			Instruction->dFlag = 1;

			wsprintfA(mnemonic, "LDS");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			wsprintfA(operand3, "%s%s", Instruction->OperandPrefix >= 0 ? "DWORD " : "FWORD ", strstr(operand2, "PTR"));
			wsprintfA(operand2, "%s", operand3);
			*operand3 = '\0';

			break;
		}
		case 0xC6: case 0xC7:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = ((*currentCode & 1) ^ 1) & 1;

			wsprintfA(mnemonic, "MOV");
			currentCode++;
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			currentCode = ParseImmediate(currentCode, Instruction, operand2);

			break;
		}
		
		/* enter leave */

		case 0xC8:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 0;
			Instruction->OperandPrefix = 0;

			wsprintfA(mnemonic, "ENTER");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);
			Instruction->sFlag = 1;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);

			break;
		}
		case 0xC9:
		{
			Instruction->Opcode = *currentCode;
			
			wsprintfA(mnemonic, "LEAVE");
			currentCode++;

			break;
		}

		/* retx */

		case 0xCA:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 0;
			Instruction->OperandPrefix = 0;

			wsprintfA(mnemonic, "RETF");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);
			
			break;
		}
		case 0xCB:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "RETF");
			currentCode++;

			break;
		}
		
		/* int */

		case 0xCC:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "INT3");
			currentCode++;

			break;
		}
		case 0xCD:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 1;

			wsprintfA(mnemonic, "INT");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);
			
			break;
		}
		case 0xCE:
		{
			Instruction->Opcode = *currentCode;
			
			wsprintfA(mnemonic, "INTO");
			currentCode++;

			break;
		}
		case 0xCF:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s%c", "IRET", Instruction->OperandPrefix >= 0 ? 'W' : 'D');
			currentCode++;

			break;
		}

		/* rol ror rcl rcr shl sal shr shl sar */
		
		case 0xD0: case 0xD1:
		{
			Instruction->Opcode  = *currentCode;
			Instruction->wFlag = *currentCode & 1;

			currentCode++;
			wsprintfA(mnemonic, "%s", LogicalShiftMnemonic[(*currentCode >> 3) & 7]);
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			wsprintfA(operand2, "1");

			break;
		}
		case 0xD2: case 0xD3:
		{
			Instruction->Opcode  = *currentCode;
			Instruction->wFlag = *currentCode & 1;

			currentCode++;
			wsprintfA(mnemonic, "%s", LogicalShiftMnemonic[(*currentCode >> 3) & 7]);
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			wsprintfA(operand2, "CL");

			break;
		}

		/* aam aad */

		case 0xD4: case 0xD5:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 1;

			wsprintfA(mnemonic, BCDAdjustMnemonic[*currentCode & 7]);
			currentCode++;
			if(*currentCode == 0x0A)
			{
				currentCode++;
			}
			else
			{
				currentCode = ParseImmediate(currentCode, Instruction, operand1);
			}

			break;
		}
		
		/* setalc */

		case 0xD6:
		{
			Instruction->Opcode = *currentCode;
			
			wsprintfA(mnemonic, "SALC");
			currentCode++;

			break;
		}
		case 0xD7:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "XLAT");
			wsprintfA(operand1, "BYTE PTR %s%s[EBX + AL]", Instruction->SegmentPrefix >= 0 ? SegmentRegisters[Instruction->SegmentPrefix] : "", Instruction->SegmentPrefix >= 0 ? ":" : "");
			currentCode++;

			break;
		}
		
		/* loopxx */
		
		case 0xE0: case 0xE1: case 0xE2:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", LoopMnemonic[*currentCode & 7]);
			currentCode++;
			wsprintfA(operand1, "SHORT %X", Instruction->LinearAddress + *((char*)currentCode) + currentCode - Code + 1);
			currentCode++;

			break;
		}
		case 0xE3:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", "JCXZ");
			currentCode++;
			wsprintfA(operand1, "SHORT %X", Instruction->LinearAddress + *((char*)currentCode) + currentCode - Code + 1);
			currentCode++;

			break;
		}
		
		/* in out */

		case 0xE4: case 0xE5: case 0xE6: case 0xE7:
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >> 1) & 1;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = 1;
			
			currentCode++;
			wsprintfA(mnemonic, "%s", Instruction->dFlag ? "OUT" : "IN");
			currentCode = ParseImmediate(currentCode, Instruction, Instruction->dFlag ? operand1 : operand2);
			wsprintfA(Instruction->dFlag ? operand2 : operand1, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "AX" : "EAX") : "AL");

			break;
		}
		case 0xEC: case 0xED: case 0xEE: case 0xEF:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->dFlag = (*currentCode >> 1) & 1;

			wsprintfA(mnemonic, "%s", Instruction->dFlag ? "OUT" : "IN");
			wsprintfA(Instruction->dFlag ? operand2 : operand1, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "AX" : "EAX") : "AL");
			wsprintfA(Instruction->dFlag ? operand1 : operand2, "DX");

			currentCode++;

			break;
		}

		/* call jmp*/

		case 0xE8:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag;

			wsprintfA(mnemonic, "CALL");
			currentCode++;

			if(Instruction->OperandPrefix >= 0)
			{
				wsprintfA(operand1, "%X", (Instruction->LinearAddress & 0xFFFF) + (*((int *)currentCode) & 0xFFFF) + currentCode - Code + 2);
				currentCode += 2;
			}
			else
			{
				wsprintfA(operand1, "%X", Instruction->LinearAddress + *((int *)currentCode) + currentCode - Code + 4);
				currentCode += 4;
			}

			break;
		}
		case 0xE9:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag;

			wsprintfA(mnemonic, "JMP");
			currentCode++;

			if(Instruction->OperandPrefix >= 0)
			{
				wsprintfA(operand1, "%X", (Instruction->LinearAddress & 0xFFFF) + (*((int *)currentCode) & 0xFFFF) + currentCode - Code + 2);
				currentCode += 2;
			}
			else
			{
				wsprintfA(operand1, "%X", Instruction->LinearAddress + *((int *)currentCode) + currentCode - Code + 4);
				currentCode += 4;
			}

			break;
		}
		case 0xEA:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 0;

			wsprintfA(mnemonic, "JMP");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);
			Instruction->OperandPrefix = 0;
			currentCode = ParseImmediate(currentCode, Instruction, operand3);
			wsprintfA(operand1, "FAR %s:%s", operand3, operand2);
			*operand2 = '\0';
			*operand3 = '\0';

			break;
		}
		case 0xEB:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 1;

			wsprintfA(mnemonic, "JMP");
			currentCode++;
			wsprintfA(operand1, "SHORT %X", Instruction->LinearAddress + *currentCode + currentCode - Code + 1);
			currentCode++;

			break;
		}
		
		case 0xF1:
		{
			Instruction->Opcode = *currentCode;
			
			wsprintfA(mnemonic, "INT1");
			currentCode++;

			break;
		}
		case 0xF4:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "HLT");
			currentCode++;

			break;
		}
		case 0xF5:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "CMC");
			currentCode++;

			break;
		}
		case 0xF6: case 0xF7:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = (~(*currentCode & 1)) & 1;
			
			currentCode++;
			wsprintfA(mnemonic, "%s", LogicalArithmeticMnemonic[(*currentCode >> 3) & 7]);
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			if(lstrcmpA(mnemonic, "TEST") == 0)
			{
				currentCode = ParseImmediate(currentCode, Instruction, operand2);
			}

			break;
		}
		
		/* flag set */

		case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD:
		{
			Instruction->Opcode = *currentCode;

			wsprintfA(mnemonic, "%s", FlagMnemonic[*currentCode & 7]);
			currentCode++;

			break;
		}
		
		case 0xFE:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;

			currentCode++;
			wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 7 ? "DEC" : "INC");
			currentCode = ParseModRM(currentCode, Instruction, operand1);

			break;
		}

		case 0xFF:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;

			currentCode++;
			switch((*currentCode >> 3) & 7)
			{
				case 0 : case 1: case 2 : case 4: case 6:
				{
					wsprintfA(mnemonic, FFOpcodeMnemonic[(*currentCode >> 3) & 7]);
					currentCode = ParseModRM(currentCode, Instruction, operand1);

					break;
				}
				case 3 : case 5:
				{
					wsprintfA(mnemonic, FFOpcodeMnemonic[(*currentCode >> 3) & 7]);
					currentCode = ParseModRM(currentCode, Instruction, operand1);
					wsprintfA(operand2, "FAR %s%s", Instruction->OperandPrefix >= 0 ? "DWORD " : "FWORD ",
						strstr(operand1, "PTR"));
					wsprintfA(operand1, "%s", operand2);
					*operand2 = '\0';

					break;
				}
				default:
				{
					wsprintfA(mnemonic, "???");
					currentCode++;
				}
			}

			break;
		}
		
		/* 2-bytes instructions */

		case 0x0F:
		{
			Instruction->Opcode = *((int *)currentCode) & 0xFFFF;
			currentCode ++;

			switch(*currentCode)
			{
				/* jxxx */
				
				case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
				case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8E: case 0x8F:
				{
					wsprintfA(mnemonic, "%s", *currentCode & 1 ? JnxxMnemonic[(*currentCode >> 1) & 7] : JxxxMnemonic[(*currentCode >> 1) & 7]);
					currentCode++;
					wsprintfA(operand1, "%X", Instruction->OperandPrefix >= 0 ? (Instruction->LinearAddress & 0xFFFF) + (*((int*)currentCode) & 0xFFFF) + currentCode - Code + 2 :
						Instruction->LinearAddress + *((int *)currentCode) + currentCode - Code + 4);
					Instruction->OperandPrefix >= 0 ? (currentCode += 2) : (currentCode += 4);

					break;
				}
				
				/* setxxx */

				case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
				case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9E: case 0x9F:
				{
					Instruction->wFlag = 0;

					wsprintfA(mnemonic, "%s", *currentCode & 1 ? SetnxxMnemonic[(*currentCode >> 1) & 7] : SetxxxMnemonic[(*currentCode >> 1) & 7]);
					currentCode++;
					currentCode = ParseModRM(currentCode, Instruction, operand1);

					break;
				}
				
				/* push pop fs gs*/
				
				case 0xA0: case 0xA1: case 0xA8: case 0xA9:
				{
					wsprintfA(mnemonic, "%s", *currentCode & 1 ? "POP" : "PUSH");
					wsprintfA(operand1, "%s", (*currentCode >> 3 & 1) ? "GS" : "FS");
					currentCode++;
					
					break;
				}
				
				/* bt bts btr btc bsf bsr */

				case 0xA3: case 0xAB: case 0xB3: case 0xBB:
				{
					Instruction->dFlag = (*currentCode >> 1) & 1;
					Instruction->wFlag = *currentCode & 1;
					
					wsprintfA(mnemonic, "%s", BTMnemonic[(*currentCode >> 3) & 3]);
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand2, operand1);

					break;
				}
				case 0xBA:
				{
					Instruction->wFlag = 1;
					Instruction->sFlag = 1;

					wsprintfA(mnemonic, "%s", BTMnemonic[(*(currentCode + 1) >> 3) & 3]);
					currentCode++;
					currentCode = ParseModRM(currentCode, Instruction, operand1);
					currentCode = ParseImmediate(currentCode, Instruction, operand2);

					break;
				}
				case 0xBC: case 0xBD:
				{
					Instruction->wFlag = 1;
					Instruction->dFlag = 1;

					wsprintfA(mnemonic, "%s", *currentCode & 1 ? "BSR" : "BSF");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);

					break;
				}
				
				/* cpuid */

				case 0xA2:
				{
					wsprintfA(mnemonic, "CPUID");
					currentCode++;

					break;
				}

				/* shld, shrd */

				case 0xA4: case 0xAC:
				{
					Instruction->wFlag = 1;
					Instruction->dFlag = 1;
					Instruction->sFlag = 1;

					wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 1 ? "SHRD" : "SHLD");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand2, operand1);
					currentCode = ParseImmediate(currentCode, Instruction, operand3);

					break;
				}
				case 0xA5: case 0xAD:
				{
					Instruction->wFlag = 1;
					Instruction->dFlag = 1;

					wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 1 ? "SHRD" : "SHLD");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand2, operand1);
					wsprintfA(operand3, "CL");

					break;
				}

				/* imul */

				case 0xAF:
				{
					Instruction->wFlag = 1;
					Instruction->dFlag = 1;

					wsprintfA(mnemonic, "%s", "IMUL");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);

					break;
				}
				
				/* cmpxchg */

				case 0xB0: case 0xB1:
				{
					Instruction->wFlag = *currentCode & 1;
					Instruction->dFlag = 1;

					wsprintfA(mnemonic, "CMPXCHG");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand2, operand1);

					break;
				}

				/* lss lfs lgs */

				case 0xB2: case 0xB4: case 0xB5:
				{
					Instruction->wFlag = 1;
					Instruction->dFlag = 1;

					wsprintfA(mnemonic, "l%s", SegmentRegisters[*currentCode & 7]);
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
					wsprintfA(operand3, "%s%s", Instruction->OperandPrefix >= 0 ? "DWORD " : "FWORD ",
						strstr(operand2, "PTR"));
					wsprintfA(operand2, "%s", operand3);
					*operand3 = '\0';

					break;
				}
				
				/* movzx movsx */
				
				case 0xB6: case 0xB7: case 0xBE: case 0xBF:
				{
					Instruction->wFlag = *currentCode & 1;

					wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 1 ? "MOVSX" : "MOVZX");
					currentCode++;
					wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[(*currentCode >> 3) & 7] : Register32[(*currentCode >> 3) & 7]);
					currentCode = ParseModRM(currentCode, Instruction, operand2);

					break;
				}
				case 0xC0: case 0xC1:
				{
					Instruction->wFlag = *currentCode & 1;
					Instruction->dFlag = 1;

					wsprintfA(mnemonic, "XADD");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand2, operand1);

					break;
				}
				case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF:
				{
					wsprintfA(mnemonic, "BSWAP");
					wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[*currentCode & 7] : Register32[*currentCode & 7]);
					currentCode++;

					break;
				}

				default:
				{
					wsprintfA(mnemonic, "%s", "???");
					currentCode++;
				}
			}

			break;
		}

		default :
		{
			currentCode++;
			wsprintfA(mnemonic, "%s", "???");
		}
	}

	/* instruction str = [prefix] mnemonic [operand1,] [operand2,] [operand3] */

	wsprintfA(InstructionStr, "%s%s%s%s%s%s%s%s%s", 
		prefix, lstrlenA(prefix) > 0 ? " " : "",
		mnemonic, " ",
		operand1,
		lstrlenA(operand2) > 0 ? ", " : "", operand2,
		lstrlenA(operand3) > 0 ? ", " : "", operand3);
	
	return currentCode;
}

unsigned char *ParseModRM(unsigned char *Code, PINSTRUCTION Instruction, char *OperandRM)
{
	unsigned char *currentCode;
	char Mod, RM;
	char DisplacementStr[MAX_OPERAND_LEN] = "";
	char RMStr[MAX_OPERAND_LEN] = "";
	char SizeStr[MAX_OPERAND_LEN] = "";
	char AddressStr[MAX_OPERAND_LEN] = "";
	char SegmentPrefixStr[MAX_OPERAND_LEN] ="";

	currentCode = Code;
	Instruction->ModRM = *currentCode;
	
	/* get mod, reg/code, rm */

	Mod = (*currentCode >> 6) & 3;
	RM = (*currentCode) & 7;

	wsprintfA(SegmentPrefixStr, "%s%s", Instruction->SegmentPrefix >= 0 ? SegmentRegisters[Instruction->SegmentPrefix] : "", 
		Instruction->SegmentPrefix >= 0 ? ":" : "");
	wsprintfA(SizeStr, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "WORD PTR" : "DWORD PTR") : "BYTE PTR");
	wsprintfA(AddressStr, "%s", Instruction->AddressPrefix >=0 ? Address16[RM] : Register32[RM]);

	switch(Mod)
	{
		case 0 :		/* 00 */
		{
			if(RM != 4 && RM != 5 && RM != 6)			/* 100, 101,110 */
			{
				wsprintfA(RMStr, "%s", AddressStr);
				
				currentCode++;
			}
			else if(RM == 4)				/* 100 */
			{
				if(Instruction->AddressPrefix >= 0)			/* 16 bits mode */
				{
					wsprintfA(RMStr, "%s", Address16[RM]);

					currentCode++;
				}
				else														/*32 bits mode */
				{
					/* SIB */
					
					currentCode++;
					currentCode = ParseSIB(currentCode, Instruction, RMStr);
					currentCode++;
				}
			}
			else if(RM == 5)				/* 101 */
			{
				if(Instruction->AddressPrefix >= 0)			/* 16 bits mode */
				{
					wsprintfA(RMStr, "%s", Address16[RM]);

					currentCode++;
				}
				else														/*32 bits mode */
				{
					/* displacement 32 */

					currentCode++;
					Instruction->Displacement = *((unsigned int *)currentCode);
					
					wsprintfA(RMStr, "%X", *((unsigned int *)currentCode));
					currentCode += 4;
				}
			}
			else if(RM == 6)				/* 110 */ 
			{
				if(Instruction->AddressPrefix >= 0)			/* 16 bits mode */
				{
					/* displacement 16 */

					currentCode++;
					lstrcpynA((char*)(&Instruction->Displacement), (char*)currentCode, 2);
					
					wsprintfA(RMStr, "%X%X%X%X", (*(currentCode + 1) >> 4) & 0xF,
						(*(currentCode + 1) & 0xF), (*currentCode >> 4) & 0xF, (*currentCode) & 0xF);
					
					currentCode += 2;
				}
				else														/* 32 bits mode */
				{
					wsprintfA(RMStr, "%s", Register32[RM]);

					currentCode++;
				}
			}
			
			wsprintfA(OperandRM, "%s %s[%s]", SizeStr, SegmentPrefixStr, RMStr);
			
			break;
		}
		case 1 :		/* 01 */
		{
			
			if(RM != 4)
			{
				wsprintfA(RMStr, "%s", Register32[RM]);
			}
			else
			{
				/* SIB */
				
				if(Instruction->AddressPrefix >= 0)
				{
					wsprintfA(RMStr, "%s", Address16[RM]);
				}
				else
				{
					currentCode++;
					currentCode = ParseSIB(currentCode, Instruction, RMStr);
				}
			}
			
			/* displacement 8 */

			currentCode++;
			Instruction->Displacement = *currentCode;
			wsprintfA(DisplacementStr, "%s%X%X", (*currentCode >>7) & 1 ? " - " : " + ", (*currentCode >> 7) & 1 ? (~(*currentCode) + 1) >> 4 & 0xF : (*currentCode >> 4) & 0xF, 
				(*currentCode >> 7) & 1 ? (~(*currentCode) + 1) & 0xF : (*currentCode) & 0xF);
			currentCode++;

			wsprintfA(OperandRM, "%s %s[%s%s]", SizeStr, SegmentPrefixStr, RMStr, DisplacementStr);

			break;
		}
		case 2 :		/* 10 */
		{
			if(RM != 4)
			{
				wsprintfA(RMStr, "%s", Register32[RM]);
			}
			else
			{
				/* SIB */
				
				if(Instruction->AddressPrefix >= 0)
				{
					wsprintfA(RMStr, "%s", Address16[RM]);
				}
				else
				{
					currentCode++;
					currentCode = ParseSIB(currentCode, Instruction, RMStr);
				}
			}
			
			/* displacement 32 */
			
			currentCode++;
			Instruction->Displacement = *((int *)currentCode);
			wsprintfA(DisplacementStr, " %c %X", ((*((int *)currentCode)) >> 31) & 1 ? '-' : '+', ((*((int *)currentCode)) >> 31) & 1?  ~(*((int *)currentCode)) + 1 : *((int *)currentCode));
			currentCode += 4;

			wsprintfA(OperandRM, "%s %s[%s%s]", SizeStr, SegmentPrefixStr, RMStr, DisplacementStr);

			break;
		}
		case 3 :		/* 11 */
		{
			wsprintfA(OperandRM, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? Register16[RM] : Register32[RM]) 
				 : Register8[RM]);

			currentCode++;

			break;
		}
	}

	return currentCode;
}

unsigned char *ParseRegModRM(unsigned char *Code, PINSTRUCTION Instruction, char *Operand1, char *Operand2)
{
	unsigned char *currentCode;
	char RegOpcode;

	currentCode = Code;

	RegOpcode = (*currentCode >> 3) & 7;
	
	currentCode = ParseModRM(currentCode, Instruction, Instruction->dFlag ? Operand2 : Operand1);
	wsprintfA(Instruction->dFlag ? Operand1 : Operand2, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? Register16[RegOpcode] : Register32[RegOpcode]) 
				 : Register8[RegOpcode]);

	return currentCode;
}

unsigned char *ParseImmediate(unsigned char *Code, PINSTRUCTION Instruction, char *OperandImmediate)
{
	unsigned char *currentCode;
	
	currentCode = Code;

	if(!Instruction->sFlag)
	{
		if(Instruction->OperandPrefix >= 0)
		{
			/* 16 bits immediate value */
			
			memcpy(&(Instruction->Immediate), currentCode, 2);
			wsprintfA(OperandImmediate, "%X%X%X%X", (*(currentCode + 1) >> 4) & 0xF, 
				(*(currentCode + 1) & 0xF), (*currentCode >> 4) & 0xF, (*currentCode) & 0xF);

			currentCode += 2;
		}
		else
		{
			/* 32 bits immediate value */

			Instruction->Immediate = *((unsigned int *)currentCode);
			wsprintfA(OperandImmediate, "%X", *((unsigned int *)currentCode));

			currentCode += 4;
		}
	}
	else
	{
		/* 8 bits immediate value */

		Instruction->Immediate = *currentCode;
		wsprintfA(OperandImmediate, "%X%X", (*currentCode >> 4) & 0xF, (*currentCode) & 0xF);
		
		currentCode++;
	}

	return currentCode;
}

unsigned char *ParseSIB(unsigned char *Code, PINSTRUCTION Instruction, char *SIBStr)
{
	char *currentCode;
	char Mod;
	char Scale, Index, Base;
	char BaseStr[MAX_OPERAND_LEN] = "";
	char ScaledIndexStr[MAX_OPERAND_LEN] = "";
	char DisplacementStr[MAX_OPERAND_LEN] = "";
	
	currentCode = (char*)Code;
	Instruction->SIB = *currentCode;
	
	Mod = ((Instruction->ModRM) >> 6) & 3;
	Scale = (*currentCode >> 6) & 3;
	Index = (*currentCode >> 3) & 7;
	Base = (*currentCode) & 7;

	/* base */

	wsprintfA(BaseStr, "%s", (Base ==5 && Mod == 0) ?  "" : Register32[Base]);

	/* when Mod == 00b and Base = 101b, there is a special displacement 32 */

	if(Base == 5 && Mod == 0)
	{
		currentCode ++;
		Instruction->Displacement = *((long *)currentCode);

		wsprintfA(DisplacementStr, "%X", *((int *)currentCode));
		currentCode += 3;
	}

	/* scaled index */
	
	Index == 4 ? wsprintfA(ScaledIndexStr, "") : 
		(Scale ? wsprintfA(ScaledIndexStr, "%s * %d", Register32[Index], 1 << Scale) : 
		wsprintfA(ScaledIndexStr, "%s", Register32[Index]));

	/* merge them into SIB */

	wsprintfA(SIBStr, "%s%s%s%s%s", BaseStr,
		(lstrlenA(BaseStr) > 0 && lstrlenA(ScaledIndexStr) > 0) ? " + " : "",
		ScaledIndexStr,
		(lstrlenA(BaseStr) > 0 && lstrlenA(ScaledIndexStr) > 0 && lstrlenA(DisplacementStr) > 0) ? " + " : "",
		DisplacementStr);

	return (unsigned char*)currentCode;
}
unsigned char *DisassembleSize(unsigned int LinearAddress, unsigned char *Code, PINSTRUCTION Instruction, char *InstructionStr)
{
	char prefix[MAX_OPERAND_LEN] = "";
	char mnemonic[MAX_MNEMONIC_LEN] = "";
	char operand1[MAX_OPERAND_LEN] = "";
	char operand2[MAX_OPERAND_LEN] = "";
	char operand3[MAX_OPERAND_LEN] = "";
	
	unsigned char *currentCode;
	char prefixFlag; 

	currentCode = Code;
	prefixFlag = 0;
	
	/* initialize Instrction structure */

	Instruction->RepeatPrefix = -1;
	Instruction->SegmentPrefix = -1;
	Instruction->OperandPrefix = -1;
	Instruction->AddressPrefix = -1;

	Instruction->Opcode = -1;
	Instruction->ModRM = -1;
	Instruction->SIB = -1;
	
	Instruction->Displacement = -1;
	Instruction->Immediate = -1;

	Instruction->dFlag = Instruction->wFlag = Instruction->sFlag = -1;

	Instruction->LinearAddress = LinearAddress;

	/* check PREFIXES and save the values, note that prefixes in the same group can only appear once */
	/* and the values assigned to xxxxPrefix in Instruction structure are the indexes of the names */
	/* in corresponding name tables */
	
	while(*currentCode == 0xF0 || *currentCode == 0xF2 || *currentCode == 0xF3 ||
		*currentCode == 0x2E || *currentCode == 0x36 || *currentCode == 0x3E ||
		*currentCode == 0x26 || *currentCode == 0x64 || *currentCode == 0x65 ||
		*currentCode == 0x66 || *currentCode == 0x67)
	{
		switch(*currentCode)
		{
			/* group1: lock and repeat prefixes */

			case 0xF0 :
			{	
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX LOCK:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->RepeatPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->RepeatPrefix = 0;
						currentCode++;
					}
				}
				
				break;
			}
			case 0xF2 :
			{
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX REPNZ:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->RepeatPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->RepeatPrefix = 5;
						currentCode++;
					}
				}

				break;
			}
			case 0xF3 :
			{
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX REP:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->RepeatPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->RepeatPrefix = 1;
						currentCode++;
					}
				}

				break;
			}

			/* group2: segment override prefixes */

			case 0x2E :
			{
				
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX CS:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 1;
						currentCode++;
					}
				}

				break;
			}
			case 0x36 :
			{
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX SS:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 2;
						currentCode++;
					}
				}

				break;
			}
			case 0x3E :
			{
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX DS:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 3;
						currentCode++;
					}
				}

				break;
			}
			case 0x26 :
			{	
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX ES:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 0;
						currentCode++;
					}
				}

				break;
			}
			case 0x64 :
			{
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX FS:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */
				
					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 4;
						currentCode++;
					}
				}

				break;
			}
			case 0x65 :
			{	
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX GS:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->SegmentPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->SegmentPrefix = 5;
						currentCode++;
					}
				}

				break;
			}

			/* group3: Operand-size override prefixes */

			case 0x66 :
			{
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX DATASIZE:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->OperandPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->OperandPrefix = 0;
						currentCode++;
					}
				}

				break;
			}

			/* group4: Address-size override prefixes */

			case 0x67 :
			{
				if(prefixFlag)
				{
					//lstrcpyA(InstructionStr, "PREFIX ADDRSIZE:");
					currentCode++;
					return currentCode;
				}
				else
				{
					/* rescan */

					if(Instruction->AddressPrefix >= 0)
					{
						currentCode = Code;
						prefixFlag = 1;
					}
					else
					{
						Instruction->AddressPrefix = 0;
						currentCode++;
					}
				}
				
				break;
			}
		}
	}

	/* CODE map */

	switch(*currentCode)
	{
		/* Arithmetic operations */

		case 0x00: case 0x01: case 0x02: case 0x03: 
		case 0x08: case 0x09: case 0x0A: case 0x0B: 
		case 0x10: case 0x11: case 0x12: case 0x13: 
		case 0x18: case 0x19: case 0x1A: case 0x1B: 
		case 0x20: case 0x21: case 0x22: case 0x23: 
		case 0x28: case 0x29: case 0x2A: case 0x2B: 
		case 0x30: case 0x31: case 0x32: case 0x33: 
		case 0x38: case 0x39: case 0x3A: case 0x3B: 
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >> 1) & 1;
			Instruction->wFlag = (*currentCode) & 1;

			//wsprintfA(mnemonic, ArithmeticMnemonic[(*currentCode >> 3) & 0x1F]);
			
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			
			break;
		}
		case 0x04: case 0x05: case 0x0C: case 0x0D:	
		case 0x14: case 0x15: case 0x1C: case 0x1D:
		case 0x24: case 0x25: case 0x2C: case 0x2D:
		case 0x34: case 0x35: case 0x3C: case 0x3D:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = (*currentCode & 1);
			Instruction->sFlag = ! Instruction->wFlag;

			//wsprintfA(mnemonic, ArithmeticMnemonic[(*currentCode >> 3) & 0x1F]);
			
			//wsprintfA(operand1, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "AX" : "EAX")  : "AL");

			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);
			
			break;
		}
		case 0x80: case 0x81: case 0x82: case 0x83:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = (*currentCode & 1);
			Instruction->sFlag = (*currentCode >> 1) & 1;
			
			/* special cases */

			if(*currentCode == 0x80)
			{
				Instruction->sFlag = 1;
			}

			//wsprintfA(mnemonic, ArithmeticMnemonic[(*(currentCode + 1) >> 3) & 7]);

			currentCode++;
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			currentCode = ParseImmediate(currentCode, Instruction, operand2);

			/* special cases */

			if(Instruction->Opcode == 0x83)
			{
				//wsprintfA(operand2, "%X%X", (Instruction->Immediate >> 7 & 1) ? (Instruction->OperandPrefix >= 0 ? 0xFF: 0xFFFFFF) : 0x0, Instruction->Immediate);
			}

			break;
		}
		
		/* push pop operations */

		case 0x06: case 0x07 : case 0x0E: case 0x16: case 0x17: case 0x1E : case 0x1F:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", *currentCode & 1 ? "POP" : "PUSH");
			//wsprintfA(operand1, "%s", SegmentRegisters[(*currentCode >> 3) & 3]);
			currentCode++;

			break;
		}
		case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
		case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 1 ? "POP" : "PUSH");
			//wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[*currentCode & 7] : Register32[*currentCode & 7]);
			currentCode++;
			
			break;
		}
		case 0x60: case 0x61: case 0x9C: case 0x9D:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s%c%c", (*currentCode & 1) ? "POP" : "PUSH",(*currentCode >> 7) & 1 ? 'F' : 'A', Instruction->OperandPrefix >= 0 ? 'W' : 'D');
			currentCode++;

			break;
		}
		case 0x68: case 0x6A:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = (*currentCode >> 1) & 1;
			
			//wsprintfA(mnemonic, "%s", "PUSH");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);

			break;
		}
		case 0x8F:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = (*currentCode) & 1;

			//wsprintfA(mnemonic, "%s", "POP");
			currentCode++;
			currentCode = ParseModRM(currentCode, Instruction, operand1);

			break;
		}
		case 0x27: case 0x2F: case 0x37: case 0x3F:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", BCDAdjustMnemonic[(*currentCode >> 3) & 3]);
			currentCode++;

			break;
		}

		/* inc and dec */
		
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 1 ? "DEC" : "INC");
			//wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[*currentCode & 7] : Register32[*currentCode & 7]);
			currentCode++;
			
			break;
		}

		/* bound */

		case 0x62:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = (*currentCode >> 1) & 1;

			//wsprintfA(mnemonic, "%s", "BOUND");
			//wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[(*(currentCode + 1) >> 3) & 7] :Register32[(*(currentCode + 1) >> 3) & 7]);
			currentCode++;
			currentCode = ParseModRM(currentCode, Instruction, operand2);
			
			/* special case, the size of memory address must be twice the size of register */

			//wsprintfA(operand3, "%s%s", Instruction->OperandPrefix >= 0 ? "DWORD " : "QWORD ",strstr(operand2, "PTR"));
			//wsprintfA(operand2, "%s", operand3);
			*operand3 = '\0';

			break;
		}

		/* arpl */

		case 0x63:
		{
			Instruction->Opcode = *currentCode;

			/* special case, the operand size of arpl must be 16-bits */

			Instruction->OperandPrefix = 0;
			Instruction->wFlag = 1;
			Instruction->dFlag = 0;

			//wsprintfA(mnemonic, "%s", "ARPL");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);

			break;
		}
		
		/* imul */

		case 0x69: case 0x6B:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = (*currentCode >> 1) & 1;
			Instruction->dFlag = (*currentCode >> 1) & 1;

			//wsprintfA(mnemonic, "%s", "IMUL");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			currentCode = ParseImmediate(currentCode, Instruction, operand3);

			break;
		}
		
		/* ins and outs */

		case 0x6C: case 0x6D: case 0x6E: case 0x6F:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->dFlag = (*currentCode >> 1) & 1;
			
			//wsprintfA(prefix, "%s", Instruction->RepeatPrefix > 0 ? RepeatPrefixes[Instruction->RepeatPrefix] : "");
			//wsprintfA(mnemonic, "%s%c", Instruction->dFlag ? "OUTS" : "INS", Instruction->wFlag ?(Instruction->OperandPrefix >= 0 ? 'W' : 'D') : 'B');
			currentCode++;

			break;
		}
		
		/* jmp instructions j(n)xx */
		
		case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
		case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E: case 0x7F:
		{
			Instruction->Opcode = *currentCode;
			
			//wsprintfA(mnemonic, "%s", *currentCode & 1 ? JnxxMnemonic[(*currentCode >> 1) & 7] : JxxxMnemonic[(*currentCode >> 1) & 7]);
			currentCode++;
			//wsprintfA(operand1, "SHORT %X", Instruction->LinearAddress + *((char*)currentCode) + currentCode - Code + 1);
			currentCode++;

			break;
		}

		/* test */

		case 0x84: case 0x85:
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >>1) & 1;
			Instruction->wFlag = (*currentCode) & 1;

			//wsprintfA(mnemonic, "%s", "TEST");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);

			break;
		}

		/* xchg */

		case 0x86: case 0x87:
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >>1) & 1;
			Instruction->wFlag = (*currentCode) & 1;

			//wsprintfA(mnemonic, "%s", "XCHG");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			
			break;
		}
		case 0x90:
		{
			Instruction->Opcode = *currentCode;
			
			//wsprintfA(mnemonic, "%s", Instruction->RepeatPrefix == 1 ? "PAUSE" : "NOP");
			currentCode++;

			break;
		}
		case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", "XCHG");
			//wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[*currentCode & 7] : Register32[*currentCode & 7]);
			//wsprintfA(operand2, "%s", Instruction->OperandPrefix >= 0 ? "AX" :"EAX");
			currentCode++;

			break;
		}

		/* mov */

		case 0x88: case 0x89: case 0x8A: case 0x8B:
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >>1) & 1;
			Instruction->wFlag = (*currentCode) & 1;

			//wsprintfA(mnemonic, "%s", "MOV");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			
			break;
		}
		case 0x8C: case 0x8E:
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >> 1) & 1;

			/* special cases code segment registers are 16-bits long */
			
			Instruction->OperandPrefix = 0;
			Instruction->wFlag = 1;

			//wsprintfA(mnemonic, "%s", "MOV");
			currentCode++;
			//wsprintfA(Instruction->dFlag ? operand1 : operand2, "%s", SegmentRegisters[(*currentCode >> 3) & 7]);
			currentCode = ParseModRM(currentCode, Instruction, Instruction->dFlag ? operand2 : operand1);

			break;
		}
		case 0xA0: case 0xA1: case 0xA2: case 0xA3:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->dFlag = (*currentCode >> 1) & 1;

			//wsprintfA(mnemonic, "%s", "MOV");
			currentCode++;
			//wsprintfA(Instruction->dFlag ? operand1 : operand2, "%s%s%s[%X]", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "WORD PTR " : "DWORD PTR ") : "BYTE PTR ",Instruction->SegmentPrefix >= 0 ? SegmentRegisters[Instruction->SegmentPrefix] : "",Instruction->SegmentPrefix >= 0 ? ":" : "",*((unsigned int *)currentCode));
			//wsprintfA(Instruction->dFlag ? operand2 : operand1, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "AX" : "EAX") : "AL");
			currentCode += 4;

			break;
		}
		case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7:
		case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: case 0xBF:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = (*currentCode >>3) & 1;
			Instruction->sFlag = !(Instruction->wFlag);

			//wsprintfA(mnemonic, "%s", "MOV");
			//wsprintfA(operand1, Instruction->wFlag ? Instruction->OperandPrefix >= 0 ? Register16[*currentCode & 7] : Register32[*currentCode & 7] : Register8[*currentCode & 7]);
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);

			break;
		}

		/* lea */

		case 0x8D:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = (*currentCode) & 1;

			/*special cases */

			Instruction->dFlag = 1;
			
			//wsprintfA(mnemonic, "%s", "LEA");
			currentCode++;	
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);

			break;
		}
		
		/* cbw, cwd, cwde, cdq*/

		case 0x98:
		{
			Instruction->Opcode = *currentCode;
			
			//wsprintfA(mnemonic, "%s", Instruction->OperandPrefix >= 0 ? "CBW" : "CWDE");
			currentCode++;

			break;
		}
		case 0x99:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", Instruction->OperandPrefix >= 0 ? "CWD" : "CDQ");
			currentCode++;

			break;
		}

		/* call far ptr16 : 32 */

		case 0x9A:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 0;

			//wsprintfA(mnemonic, "%s", "CALL");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);
			Instruction->OperandPrefix = 0;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);
			//wsprintfA(operand3, "FAR %s : %s", operand2, operand1);
			//lstrcpyAn(operand1, operand3, MAX_OPERAND_LEN);
			*operand2 = '\0';
			*operand3 = '\0';

			break;
		}

		/* wait */

		case 0x9B:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", "WAIT");
			currentCode++;

			break;
		}
		
		/* lahf and sahf */

		case 0x9E:
		{
			Instruction->Opcode = *currentCode;
			
			//wsprintfA(mnemonic, "%s", "ASHF");
			currentCode++;

			break;
		}
		case 0x9F:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", "LAHF");
			currentCode++;
			
			break;
		}
		
		/* string operations */

		case 0xA4: case 0xA5: case 0xA6: case 0xA7:
		case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAE: case 0xAF:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			
			//wsprintfA(prefix, "%s", Instruction->RepeatPrefix > 0 ? RepeatPrefixes[Instruction->RepeatPrefix] : "");
			//wsprintfA(mnemonic, "%s%c", StrMnemonic[(*currentCode >> 1) & 7], Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? 'W' : 'D') : 'B');
			currentCode++;

			break;
		}

		/* test */

		case 0xA8: case 0xA9:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = !(*currentCode & 1);

			//wsprintfA(mnemonic, "%s", "TEST");
			//wsprintfA(operand1, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "AX" : "EAX") : "AL");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);
			
			break;
		}
		
		/* logical shift */

		case 0xC0: case 0xC1:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = 1;
			
			currentCode++;
			//wsprintfA(mnemonic, "%s", LogicalShiftMnemonic[(*currentCode >> 3) & 7]);
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			currentCode = ParseImmediate(currentCode, Instruction, operand2);

			break;
		}
		
		/* retn */

		case 0xC2 :
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 0;
			Instruction->OperandPrefix = 0;

			//wsprintfA(mnemonic, "RETN");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);

			break;
		}
		case 0xC3:
		{
			Instruction->Opcode = *currentCode;
			
			//wsprintfA(mnemonic, "RETN");
			currentCode++;

			break;
		}
		
		/* les, lds */

		case 0xC4: 
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = 1;
			Instruction->dFlag = 1;

			//wsprintfA(mnemonic, "LES");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			//wsprintfA(operand3, "%s%s", Instruction->OperandPrefix >= 0 ? "DWORD " : "FWORD ", strstr(operand2, "PTR"));
			//wsprintfA(operand2, "%s", operand3);
			*operand3 = '\0';

			break;
		}
		case 0xC5:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = 1;
			Instruction->dFlag = 1;

			//wsprintfA(mnemonic, "LDS");
			currentCode++;
			currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
			//wsprintfA(operand3, "%s%s", Instruction->OperandPrefix >= 0 ? "DWORD " : "FWORD ", strstr(operand2, "PTR"));
			//wsprintfA(operand2, "%s", operand3);
			*operand3 = '\0';

			break;
		}
		case 0xC6: case 0xC7:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = ((*currentCode & 1) ^ 1) & 1;

			//wsprintfA(mnemonic, "MOV");
			currentCode++;
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			currentCode = ParseImmediate(currentCode, Instruction, operand2);

			break;
		}
		
		/* enter leave */

		case 0xC8:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 0;
			Instruction->OperandPrefix = 0;

			//wsprintfA(mnemonic, "ENTER");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);
			Instruction->sFlag = 1;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);

			break;
		}
		case 0xC9:
		{
			Instruction->Opcode = *currentCode;
			
			//wsprintfA(mnemonic, "LEAVE");
			currentCode++;

			break;
		}

		/* retx */

		case 0xCA:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 0;
			Instruction->OperandPrefix = 0;

			//wsprintfA(mnemonic, "RETF");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);
			
			break;
		}
		case 0xCB:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "RETF");
			currentCode++;

			break;
		}
		
		/* int */

		case 0xCC:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "INT3");
			currentCode++;

			break;
		}
		case 0xCD:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 1;

			//wsprintfA(mnemonic, "INT");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand1);
			
			break;
		}
		case 0xCE:
		{
			Instruction->Opcode = *currentCode;
			
			//wsprintfA(mnemonic, "INTO");
			currentCode++;

			break;
		}
		case 0xCF:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s%c", "IRET", Instruction->OperandPrefix >= 0 ? 'W' : 'D');
			currentCode++;

			break;
		}

		/* rol ror rcl rcr shl sal shr shl sar */
		
		case 0xD0: case 0xD1:
		{
			Instruction->Opcode  = *currentCode;
			Instruction->wFlag = *currentCode & 1;

			currentCode++;
			//wsprintfA(mnemonic, "%s", LogicalShiftMnemonic[(*currentCode >> 3) & 7]);
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			//wsprintfA(operand2, "1");

			break;
		}
		case 0xD2: case 0xD3:
		{
			Instruction->Opcode  = *currentCode;
			Instruction->wFlag = *currentCode & 1;

			currentCode++;
			//wsprintfA(mnemonic, "%s", LogicalShiftMnemonic[(*currentCode >> 3) & 7]);
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			//wsprintfA(operand2, "CL");

			break;
		}

		/* aam aad */

		case 0xD4: case 0xD5:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 1;

			//wsprintfA(mnemonic, BCDAdjustMnemonic[*currentCode & 7]);
			currentCode++;
			if(*currentCode == 0x0A)
			{
				currentCode++;
			}
			else
			{
				currentCode = ParseImmediate(currentCode, Instruction, operand1);
			}

			break;
		}
		
		/* setalc */

		case 0xD6:
		{
			Instruction->Opcode = *currentCode;
			
			//wsprintfA(mnemonic, "SALC");
			currentCode++;

			break;
		}
		case 0xD7:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "XLAT");
			//wsprintfA(operand1, "BYTE PTR %s%s[EBX + AL]", Instruction->SegmentPrefix >= 0 ? SegmentRegisters[Instruction->SegmentPrefix] : "", Instruction->SegmentPrefix >= 0 ? ":" : "");
			currentCode++;

			break;
		}
		
		/* loopxx */
		
		case 0xE0: case 0xE1: case 0xE2:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", LoopMnemonic[*currentCode & 7]);
			currentCode++;
			//wsprintfA(operand1, "SHORT %X", Instruction->LinearAddress + *((char*)currentCode) + currentCode - Code + 1);
			currentCode++;

			break;
		}
		case 0xE3:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", "JCXZ");
			currentCode++;
			//wsprintfA(operand1, "SHORT %X", Instruction->LinearAddress + *((char*)currentCode) + currentCode - Code + 1);
			currentCode++;

			break;
		}
		
		/* in out */

		case 0xE4: case 0xE5: case 0xE6: case 0xE7:
		{
			Instruction->Opcode = *currentCode;
			Instruction->dFlag = (*currentCode >> 1) & 1;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = 1;
			
			currentCode++;
			//wsprintfA(mnemonic, "%s", Instruction->dFlag ? "OUT" : "IN");
			currentCode = ParseImmediate(currentCode, Instruction, Instruction->dFlag ? operand1 : operand2);
			//wsprintfA(Instruction->dFlag ? operand2 : operand1, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "AX" : "EAX") : "AL");

			break;
		}
		case 0xEC: case 0xED: case 0xEE: case 0xEF:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->dFlag = (*currentCode >> 1) & 1;

			//wsprintfA(mnemonic, "%s", Instruction->dFlag ? "OUT" : "IN");
			//wsprintfA(Instruction->dFlag ? operand2 : operand1, "%s", Instruction->wFlag ? (Instruction->OperandPrefix >= 0 ? "AX" : "EAX") : "AL");
			//wsprintfA(Instruction->dFlag ? operand1 : operand2, "DX");

			currentCode++;

			break;
		}

		/* call jmp*/

		case 0xE8:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag;

			//wsprintfA(mnemonic, "CALL");
			currentCode++;

			if(Instruction->OperandPrefix >= 0)
			{
				//wsprintfA(operand1, "%X", (Instruction->LinearAddress & 0xFFFF) + (*((int *)currentCode) & 0xFFFF) + currentCode - Code + 2);
				currentCode += 2;
			}
			else
			{
				//wsprintfA(operand1, "%X", Instruction->LinearAddress + *((int *)currentCode) + currentCode - Code + 4);
				currentCode += 4;
			}

			break;
		}
		case 0xE9:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag;

			//wsprintfA(mnemonic, "JMP");
			currentCode++;

			if(Instruction->OperandPrefix >= 0)
			{
				//wsprintfA(operand1, "%X", (Instruction->LinearAddress & 0xFFFF) + (*((int *)currentCode) & 0xFFFF) + currentCode - Code + 2);
				currentCode += 2;
			}
			else
			{
				//wsprintfA(operand1, "%X", Instruction->LinearAddress + *((int *)currentCode) + currentCode - Code + 4);
				currentCode += 4;
			}

			break;
		}
		case 0xEA:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 0;

			//wsprintfA(mnemonic, "JMP");
			currentCode++;
			currentCode = ParseImmediate(currentCode, Instruction, operand2);
			Instruction->OperandPrefix = 0;
			currentCode = ParseImmediate(currentCode, Instruction, operand3);
			//wsprintfA(operand1, "FAR %s:%s", operand3, operand2);
			*operand2 = '\0';
			*operand3 = '\0';

			break;
		}
		case 0xEB:
		{
			Instruction->Opcode = *currentCode;
			Instruction->sFlag = 1;

			//wsprintfA(mnemonic, "JMP");
			currentCode++;
			//wsprintfA(operand1, "SHORT %X", Instruction->LinearAddress + *currentCode + currentCode - Code + 1);
			currentCode++;

			break;
		}
		
		case 0xF1:
		{
			Instruction->Opcode = *currentCode;
			
			//wsprintfA(mnemonic, "INT1");
			currentCode++;

			break;
		}
		case 0xF4:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "HLT");
			currentCode++;

			break;
		}
		case 0xF5:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "CMC");
			currentCode++;

			break;
		}
		case 0xF6: case 0xF7:
		{
			DWORD codel;
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;
			Instruction->sFlag = (~(*currentCode & 1)) & 1;
			
			currentCode++;
			codel=(*currentCode >> 3) & 7;
			//wsprintfA(mnemonic, "%s", LogicalArithmeticMnemonic[(*currentCode >> 3) & 7]);
			currentCode = ParseModRM(currentCode, Instruction, operand1);
			//if(lstrcmp(mnemonic, "TEST") == 0)
			if(codel == 0)
			{
				currentCode = ParseImmediate(currentCode, Instruction, operand2);
			}

			break;
		}
		
		/* flag set */

		case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD:
		{
			Instruction->Opcode = *currentCode;

			//wsprintfA(mnemonic, "%s", FlagMnemonic[*currentCode & 7]);
			currentCode++;

			break;
		}
		
		case 0xFE:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;

			currentCode++;
			//wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 7 ? "DEC" : "INC");
			currentCode = ParseModRM(currentCode, Instruction, operand1);

			break;
		}

		case 0xFF:
		{
			Instruction->Opcode = *currentCode;
			Instruction->wFlag = *currentCode & 1;

			currentCode++;
			switch((*currentCode >> 3) & 7)
			{
				case 0 : case 1: case 2 : case 4: case 6:
				{
					//wsprintfA(mnemonic, FFOpcodeMnemonic[(*currentCode >> 3) & 7]);
					currentCode = ParseModRM(currentCode, Instruction, operand1);

					break;
				}
				case 3 : case 5:
				{
					//wsprintfA(mnemonic, FFOpcodeMnemonic[(*currentCode >> 3) & 7]);
					currentCode = ParseModRM(currentCode, Instruction, operand1);
					//wsprintfA(operand2, "FAR %s%s", Instruction->OperandPrefix >= 0 ? "DWORD " : "FWORD ",strstr(operand1, "PTR"));
					//wsprintfA(operand1, "%s", operand2);
					*operand2 = '\0';

					break;
				}
				default:
				{
					//wsprintfA(mnemonic, "???");
					currentCode++;
				}
			}

			break;
		}
		
		/* 2-bytes instructions */

		case 0x0F:
		{
			Instruction->Opcode = *((int *)currentCode) & 0xFFFF;
			currentCode ++;

			switch(*currentCode)
			{
				/* jxxx */
				
				case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
				case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8E: case 0x8F:
				{
					//wsprintfA(mnemonic, "%s", *currentCode & 1 ? JnxxMnemonic[(*currentCode >> 1) & 7] : JxxxMnemonic[(*currentCode >> 1) & 7]);
					currentCode++;
					//wsprintfA(operand1, "%X", Instruction->OperandPrefix >= 0 ? (Instruction->LinearAddress & 0xFFFF) + (*((int*)currentCode) & 0xFFFF) + currentCode - Code + 2 :	Instruction->LinearAddress + *((int *)currentCode) + currentCode - Code + 4);
					Instruction->OperandPrefix >= 0 ? (currentCode += 2) : (currentCode += 4);

					break;
				}
				
				/* setxxx */

				case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
				case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9E: case 0x9F:
				{
					Instruction->wFlag = 0;

					//wsprintfA(mnemonic, "%s", *currentCode & 1 ? SetnxxMnemonic[(*currentCode >> 1) & 7] : SetxxxMnemonic[(*currentCode >> 1) & 7]);
					currentCode++;
					currentCode = ParseModRM(currentCode, Instruction, operand1);

					break;
				}
				
				/* push pop fs gs*/
				
				case 0xA0: case 0xA1: case 0xA8: case 0xA9:
				{
					//wsprintfA(mnemonic, "%s", *currentCode & 1 ? "POP" : "PUSH");
					//wsprintfA(operand1, "%s", (*currentCode >> 3 & 1) ? "GS" : "FS");
					currentCode++;
					
					break;
				}
				
				/* bt bts btr btc bsf bsr */

				case 0xA3: case 0xAB: case 0xB3: case 0xBB:
				{
					Instruction->dFlag = (*currentCode >> 1) & 1;
					Instruction->wFlag = *currentCode & 1;
					
					//wsprintfA(mnemonic, "%s", BTMnemonic[(*currentCode >> 3) & 3]);
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand2, operand1);

					break;
				}
				case 0xBA:
				{
					Instruction->wFlag = 1;
					Instruction->sFlag = 1;

					//wsprintfA(mnemonic, "%s", BTMnemonic[(*(currentCode + 1) >> 3) & 3]);
					currentCode++;
					currentCode = ParseModRM(currentCode, Instruction, operand1);
					currentCode = ParseImmediate(currentCode, Instruction, operand2);

					break;
				}
				case 0xBC: case 0xBD:
				{
					Instruction->wFlag = 1;
					Instruction->dFlag = 1;

					//wsprintfA(mnemonic, "%s", *currentCode & 1 ? "BSR" : "BSF");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);

					break;
				}
				
				/* cpuid */

				case 0xA2:
				{
					//wsprintfA(mnemonic, "CPUID");
					currentCode++;

					break;
				}

				/* shld, shrd */

				case 0xA4: case 0xAC:
				{
					Instruction->wFlag = 1;
					Instruction->dFlag = 1;
					Instruction->sFlag = 1;

					//wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 1 ? "SHRD" : "SHLD");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand2, operand1);
					currentCode = ParseImmediate(currentCode, Instruction, operand3);

					break;
				}
				case 0xA5: case 0xAD:
				{
					Instruction->wFlag = 1;
					Instruction->dFlag = 1;

					//wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 1 ? "SHRD" : "SHLD");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand2, operand1);
					//wsprintfA(operand3, "CL");

					break;
				}

				/* imul */

				case 0xAF:
				{
					Instruction->wFlag = 1;
					Instruction->dFlag = 1;

					//wsprintfA(mnemonic, "%s", "IMUL");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);

					break;
				}
				
				/* cmpxchg */

				case 0xB0: case 0xB1:
				{
					Instruction->wFlag = *currentCode & 1;
					Instruction->dFlag = 1;

					//wsprintfA(mnemonic, "CMPXCHG");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand2, operand1);

					break;
				}

				/* lss lfs lgs */

				case 0xB2: case 0xB4: case 0xB5:
				{
					Instruction->wFlag = 1;
					Instruction->dFlag = 1;

					//wsprintfA(mnemonic, "l%s", SegmentRegisters[*currentCode & 7]);
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand1, operand2);
					//wsprintfA(operand3, "%s%s", Instruction->OperandPrefix >= 0 ? "DWORD " : "FWORD ",	strstr(operand2, "PTR"));
					//wsprintfA(operand2, "%s", operand3);
					*operand3 = '\0';

					break;
				}
				
				/* movzx movsx */
				
				case 0xB6: case 0xB7: case 0xBE: case 0xBF:
				{
					Instruction->wFlag = *currentCode & 1;

					//wsprintfA(mnemonic, "%s", (*currentCode >> 3) & 1 ? "MOVSX" : "MOVZX");
					currentCode++;
					//wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[(*currentCode >> 3) & 7] : Register32[(*currentCode >> 3) & 7]);
					currentCode = ParseModRM(currentCode, Instruction, operand2);

					break;
				}
				case 0xC0: case 0xC1:
				{
					Instruction->wFlag = *currentCode & 1;
					Instruction->dFlag = 1;

					//wsprintfA(mnemonic, "XADD");
					currentCode++;
					currentCode = ParseRegModRM(currentCode, Instruction, operand2, operand1);

					break;
				}
				case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF:
				{
					//wsprintfA(mnemonic, "BSWAP");
					//wsprintfA(operand1, "%s", Instruction->OperandPrefix >= 0 ? Register16[*currentCode & 7] : Register32[*currentCode & 7]);
					currentCode++;

					break;
				}

				default:
				{
					//wsprintfA(mnemonic, "%s", "???");
					currentCode++;
				}
			}

			break;
		}

		default :
		{
			currentCode++;
			//wsprintfA(mnemonic, "%s", "???");
		}
	}

	/* instruction str = [prefix] mnemonic [operand1,] [operand2,] [operand3] */

	
	//wsprintfA(InstructionStr, "%s%s%s%s%s%s%s%s%s", 
		/*
		prefix, lstrlenA(prefix) > 0 ? " " : "",
		mnemonic, " ",
		operand1,
		lstrlenA(operand2) > 0 ? ", " : "", operand2,
		lstrlenA(operand3) > 0 ? ", " : "", operand3);
		*/
	
	return currentCode;
}
