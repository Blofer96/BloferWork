#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>


#define SIZE 12
#define FETCH 0
#define DECODE 1
#define EXECUTE 2
#define MEMORY 3
#define WB 4
typedef struct
{
	int RegDst;
	int ALUop1;
	int ALUop0;
	int ALUSrc;
	int Branch;
	int MemRead;
	int MemWrite;
	int RegWrite;
	int MemToReg;
}ControlUnit;

typedef struct
{
	char  genus;
	int Address;
	char Opcode[SIZE];
	char Oprand1[SIZE];
	char Oprand2[SIZE];
	char Oprand3[SIZE];
	ControlUnit Controls;
}Command;

typedef struct
{
	Command Steps[5];
	float rotation;
	int forward;
	int branch;
}Pipeline;

float commandCounter = 0; //global var for command counter

void fprintrotation(Pipeline* pipeline); //print  rotation with pipeline struct
ControlUnit initControlSignals(char  genus); //update contorls signals with command  genus 
Command readCommand(FILE* trace); //read command from the trace file
char getCommandgenus(char* command); //enter  genus  of the command r,i,l,s,b...
void StartPipeline(Pipeline* pipeline, char* Filename); //start the pipeline
void enterStall(Pipeline* pipeline, int idx); //enter stall on the stage you enter in idx
void initPipeline(Pipeline* pipeline); //create pipeline with no command and initialize items
void flushPipeline(Pipeline* pipeline); //enter stall in EXECUTE stage for 2 rotations
void printCpi(int rotations); //print the cpi in the output file

void main(int argc, char* argv[])
{
	char* trace1 = "trace1.txt";
	char* trace2 = "trace2.txt";
	Pipeline pipeline;
	pipeline.forward = atoi(argv[1]);
	pipeline.branch = atoi(argv[2]);
	initPipeline(&pipeline);
	StartPipeline(&pipeline, trace2);
	printCpi(pipeline.rotation);
}

void StartPipeline(Pipeline* pipeline, char* Filename)
{
	int opener;
	Command newCommand;
	FILE* trace = fopen(Filename, "r");
	assert(trace); //Open file check

	while (!feof(trace))
	{
		newCommand = readCommand(trace);
		for (opener = 4; opener > 0; opener--)
			pipeline->Steps[opener] = pipeline->Steps[opener - 1];

		//enter the command to fetch
		pipeline->Steps[FETCH] = newCommand;
		pipeline->rotation++;
		fprintrotation(pipeline);

		if (pipeline->forward == 0) //forwarding disable
		{
			if ((pipeline->Steps[EXECUTE].Controls.RegWrite) && (!strcmp(pipeline->Steps[DECODE].Oprand2, pipeline->Steps[EXECUTE].Oprand1)) || (!strcmp(pipeline->Steps[DECODE].Oprand3, pipeline->Steps[EXECUTE].Oprand1)))
			{
				enterStall(pipeline, EXECUTE);
			}
			if ((pipeline->Steps[MEMORY].Controls.RegWrite) && (!strcmp(pipeline->Steps[DECODE].Oprand2, pipeline->Steps[MEMORY].Oprand1)) || (!strcmp(pipeline->Steps[DECODE].Oprand3, pipeline->Steps[MEMORY].Oprand1)))
			{
				enterStall(pipeline, EXECUTE);
			}
			if (pipeline->branch == 0)	/*Branch resolution in memory stage*/
			{
				if ((pipeline->Steps[DECODE].Controls.Branch) && pipeline->Steps[FETCH].Address != pipeline->Steps[DECODE].Address + 4)
				{
					flushPipeline(pipeline);
				}
			}
			else /*Branch resolution in decode stage*/
			{
				if ((pipeline->Steps[DECODE].Controls.Branch) && pipeline->Steps[FETCH].Address != pipeline->Steps[DECODE].Address + 4)
				{
					enterStall(pipeline, EXECUTE);
				}
			}
		}
		else //forwarding enable
		{
			if ((pipeline->Steps[EXECUTE].Controls.MemRead) && (!strcmp(pipeline->Steps[DECODE].Oprand2, pipeline->Steps[EXECUTE].Oprand1))
				|| (!strcmp(pipeline->Steps[DECODE].Oprand3, pipeline->Steps[EXECUTE].Oprand1)))
			{
				enterStall(pipeline, EXECUTE);
			}
			if (pipeline->branch == 0) /*Branch resolution in memory stage*/
			{
				if ((pipeline->Steps[DECODE].Controls.Branch) && pipeline->Steps[FETCH].Address != pipeline->Steps[DECODE].Address + 4)
				{
					flushPipeline(pipeline);
				}
			}
			else /*Branch resolution in decode stage*/
			{
				if ((pipeline->Steps[DECODE].Controls.Branch) && pipeline->Steps[FETCH].Address != pipeline->Steps[DECODE].Address + 4)
				{
					enterStall(pipeline, EXECUTE);
				}
			}
		}
	}
	strcpy(pipeline->Steps[FETCH].Opcode, "No command"); /*take care after the eof until the last command
														  arrive to the wb stage*/
	int i, j;
	for (j = 0; j < 3; j++)
	{
		for (i = 4; i > 1; i--)
		{
			pipeline->Steps[i] = pipeline->Steps[i - 1];
			strcpy(pipeline->Steps[i - 1].Opcode, "No command");
		}
		pipeline->rotation++;
		fprintrotation(pipeline);
	}
	fclose(trace);
}

void initPipeline(Pipeline* pipeline)
{
	for (int opener = 0; opener < 5; opener++)
	{
		strcpy(pipeline->Steps[opener].Opcode, "No command");
		pipeline->Steps[opener].Address = 0;
		pipeline->Steps[opener].Oprand1[0] = '\0';
		pipeline->Steps[opener].Oprand2[0] = '\0';
		pipeline->Steps[opener].Oprand3[0] = '\0';
		pipeline->Steps[opener].Controls = initControlSignals('N');
	}
	pipeline->rotation = 0;
}

void enterStall(Pipeline* pipeline, int idx)
{
	for (int i = 4; i > idx; i--)
		pipeline->Steps[i] = pipeline->Steps[i - 1];

	strcpy(pipeline->Steps[idx].Opcode, "Stall");
	pipeline->Steps[idx].Address = 0;
	pipeline->Steps[idx].genus = 'N';
	pipeline->Steps[idx].Oprand1[0] = '\0';
	pipeline->Steps[idx].Oprand2[0] = '\0';
	pipeline->Steps[idx].Oprand3[0] = '\0';
	pipeline->Steps[idx].Controls = initControlSignals('N');
	pipeline->rotation++;
	fprintrotation(pipeline);
}

Command readCommand(FILE* trace)
{
	commandCounter++;
	Command newCommand;
	fscanf(trace, "%d %s ", &newCommand.Address, newCommand.Opcode);
	newCommand.genus = getCommandgenus(newCommand.Opcode);
	newCommand.Controls = initControlSignals(newCommand.genus);

	if (!strcmp(newCommand.Opcode, "lw"))
	{
		fscanf(trace, "%s %s %s", newCommand.Oprand1, newCommand.Oprand3, newCommand.Oprand2);
	}
	else if (!strcmp(newCommand.Opcode, "sw"))
	{
		fscanf(trace, "%s %s %s", newCommand.Oprand2, newCommand.Oprand3, newCommand.Oprand1);
	}
	else if (!strcmp(newCommand.Opcode, "add"))
	{
		fscanf(trace, "%s %s %s", newCommand.Oprand1, newCommand.Oprand2, newCommand.Oprand3);
	}
	else if (!strcmp(newCommand.Opcode, "addi") || (!strcmp(newCommand.Opcode, "subi")))
	{
		fscanf(trace, "%s %s %s", newCommand.Oprand1, newCommand.Oprand2, newCommand.Oprand3);
	}
	else if (!strcmp(newCommand.Opcode, "bneq"))
	{
		fscanf(trace, "%s %s %s", newCommand.Oprand2, newCommand.Oprand3, newCommand.Oprand1);
	}

	return newCommand;
}

void fprintrotation(Pipeline* pipeline)
{
	FILE* output = fopen("Output.txt", "a");
	assert(output);

	char instructions[5][20] = { "Fetch","Decode","Execute","Memory","Writeback" };
	fprintf(output, "\trotation %.0f:\n", pipeline->rotation);
	for (int opener = 0; opener < 5; opener++)
	{
		if (!strcmp(pipeline->Steps[opener].Opcode, "Stall") || !strcmp(pipeline->Steps[opener].Opcode, "No command"))
			fprintf(output, "%s: %s\n", instructions[opener], pipeline->Steps[opener].Opcode);

		else if (!strcmp(pipeline->Steps[opener].Opcode, "lw"))
			fprintf(output, "%s: %s %s %s %s\n", instructions[opener], pipeline->Steps[opener].Opcode, pipeline->Steps[opener].Oprand1, pipeline->Steps[opener].Oprand3, pipeline->Steps[opener].Oprand2);

		else if (!strcmp(pipeline->Steps[opener].Opcode, "sw"))
			fprintf(output, "%s: %s %s %s %s\n", instructions[opener], pipeline->Steps[opener].Opcode, pipeline->Steps[opener].Oprand2, pipeline->Steps[opener].Oprand3, pipeline->Steps[opener].Oprand1);

		else if (!strcmp(pipeline->Steps[opener].Opcode, "add"))
			fprintf(output, "%s: %s %s %s %s\n", instructions[opener], pipeline->Steps[opener].Opcode, pipeline->Steps[opener].Oprand1, pipeline->Steps[opener].Oprand2, pipeline->Steps[opener].Oprand3);

		else if (!strcmp(pipeline->Steps[opener].Opcode, "addi") || (!strcmp(pipeline->Steps[opener].Opcode, "subi")))
			fprintf(output, "%s: %s %s %s %s\n", instructions[opener], pipeline->Steps[opener].Opcode, pipeline->Steps[opener].Oprand1, pipeline->Steps[opener].Oprand2, pipeline->Steps[opener].Oprand3);

		else if (!strcmp(pipeline->Steps[opener].Opcode, "bneq"))
			fprintf(output, "%s: %s %s %s %s\n", instructions[opener], pipeline->Steps[opener].Opcode, pipeline->Steps[opener].Oprand2, pipeline->Steps[opener].Oprand3, pipeline->Steps[opener].Oprand1);

	}

	fclose(output);
}

char getCommandgenus(char* command)
{
	/*add=R,sub=R,or=R,and=R
	addi=I,subi=I,ori=I,andi=I
	lw=L
	sw=S
	beq,bneq=B*/

	if (strcmp(command, "add") == 0 || strcmp(command, "sub") == 0 || strcmp(command, "or") == 0 || strcmp(command, "and") == 0)
	{
		return 'R';
	}
	else if (strcmp(command, "addi") == 0 || strcmp(command, "subi") == 0 || strcmp(command, "ori") == 0 || strcmp(command, "andi") == 0)
	{
		return 'I';
	}
	else if (strcmp(command, "lw") == 0)
	{
		return 'L';
	}
	else if (strcmp(command, "sw") == 0)
	{
		return 'S';
	}
	else if (strcmp(command, "beq") == 0 || strcmp(command, "bneq") == 0)
	{
		return 'B';
	}
	else
	{
		return 'N'; //for stall  genus 
	}
}

ControlUnit initControlSignals(char genus)
{
	ControlUnit Rgenus = { 1,1,0,0,0,0,0,1,0 };
	ControlUnit Igenus = { 0,0,0,1,0,0,0,1,0 };
	ControlUnit Sgenus = { 0,0,0,1,0,0,1,0,0 };
	ControlUnit Lgenus = { 0,0,0,1,0,1,0,1,1 };
	ControlUnit Bgenus = { 0,0,1,0,1,0,0,0,0 };
	ControlUnit Stallgenus = { 0,0,0,0,0,0,0,0,0 };

	if (genus ='R')
	{
		return  Rgenus;
	}
	else if (genus = 'I')
	{
		return  Igenus;
	}
	else if (genus = 'S')
	{
		return Sgenus;
	}
	else if (genus = 'L')
	{
		return  Lgenus;
	}
	else if (genus = 'B')
	{
		return  Bgenus;
	}
	else if (genus = 0)
	{
		return  Stallgenus;
	}
}

void flushPipeline(Pipeline* pipeline)
{
	int i, j;
	for (j = 0; j < 2; j++)
	{
		for (i = 4; i > 1; i--)
		{
			pipeline->Steps[i] = pipeline->Steps[i - 1];
		}
		enterStall(pipeline, EXECUTE);
	}
}

void printCpi(int rotations)
{
	FILE* output = fopen("Output.txt", "a");
	assert(output);
	float cpi = rotations / commandCounter;
	fprintf(output, "\n\tCPI: %lf\n", cpi);
	fclose(output);
}
