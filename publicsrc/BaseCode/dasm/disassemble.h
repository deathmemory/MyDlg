#ifndef DISASSEMBLE_H
#define DISASSEMBLE_H
#include "setupapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define		MAX_MNEMONIC_LEN		32
#define		MAX_OPERAND_LEN		64
//#define		MAX_INSTRUCTION_LEN	128

typedef struct _INSTRUCTION
{
	/* prefixes */

	char	RepeatPrefix;
	char	SegmentPrefix;
	char	OperandPrefix;
	char	AddressPrefix;

	/* opcode */

	unsigned int	Opcode;

	/* ModR/M */

	char	ModRM;

	/* SIB */

	char	SIB;

	/* Displacement */

	unsigned int	Displacement;

	/* Immediate */

	unsigned int	Immediate;
	
	/* Linear address of this instruction */

	unsigned int	LinearAddress;
	
	/* Flags */

	char dFlag, wFlag, sFlag;


} INSTRUCTION, *PINSTRUCTION;


unsigned char *ParseModRM(unsigned char *Code, PINSTRUCTION Instruction, char *OperandRM);
unsigned char *ParseImmediate(unsigned char *Code, PINSTRUCTION Instruction, char *OperandImmediate);
unsigned char *ParseSIB(unsigned char *Code, PINSTRUCTION Instruction, char *SIBStr);
unsigned char *ParseRegModRM(unsigned char *Code, PINSTRUCTION Instruction, char *Operand1, char *Operand2);
unsigned char *Disassemble(unsigned int LinearAddress, unsigned char *Code, PINSTRUCTION Instruction, char *InstructionStr);

DWORD GetCodeSize(BYTE *opCode,DWORD BaseAddress);//获得指令长度
DWORD GetCodeSizeEx(char *CodeOut,DWORD BaseAddress);
DWORD ControlEIP_Code(char *buf,DWORD BaseAddress);
#endif