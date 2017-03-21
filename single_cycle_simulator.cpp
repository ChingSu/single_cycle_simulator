#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>
#include <sstream>
#include <math.h>

using namespace std;

string iImageFile = "iimage.bin";
string dImageFile = "dimage.bin";
string snapshot = "snapshot.rpt";
string errorMessage = "error_dump.rpt";

int PC = 0;
int stackPointer = 0;
int insAmount =0;
int dataAmount = 0;
int cycleCnt =0;
bool imemory[1024][8];
bool dmemory[1024][8];

const int opcodeLength = 6;
const int functcodeLength = 6;
const int rsLength = 5;
const int rtLength = 5;
const int rdLength = 5;
const int shamtCLength = 5;
const int immediateCLength = 26;
const int addressCLength = 26;
const int instructionLength = 32;

bool instruction[32];
bool functCode[6];
bool rs[5];
bool rt[5];
bool rd[5];
bool shamtC[5];
bool immediateC[26];
bool addressC[26];

bool FINISH = false;
bool HILO_OCCUPIED = false;

bitset<32> registers[32];
bitset<32> HI=0;
bitset<32> LO=0;
bitset<32> immc = 0;
bitset<32> dMemory[1024];
bitset<32> memoryConst = 0x000000FF;

void readIimage(string);
void readDimage(string);
void decodeIns(int);
void operateRtypeIns();
void operateItypeIns(int);
void operateJtypeIns(int);
int extractUnsignedIns(int, int, bool);
int extractSignedIns(int, int, bool);
int countSignedValue(bitset<32>);
int countUnsignedValue(bitset<32>);
void outputRegisterContent(string);
void outputErrorMessage(string);
void writeTo0Detected();
void numberOverflowDetected();
void overwriteHILODetected();
void addressOverflowDetected();
void misalignmentErrorDetected();

ofstream myfile;
ofstream errordump;

unsigned long BOUNDARY =0;

int main()
{

	BOUNDARY = pow(2,32);
	cout << "BOUNDARY = "<< BOUNDARY << endl;

	readIimage(iImageFile);
	cout<< "Imemory content : "<<endl;
	for(int i=PC; i<PC+insAmount*4; i++){
		for(int j=0; j<8; j++){
			cout<< imemory[i][j] << " ";
		}
		cout<<endl;
	}


	readDimage(dImageFile);
	decodeIns(PC);

	return 0;
}

void readIimage(string s){
	cout << "Start Read Iimage\n";
	char ch;
	ifstream fin(s, ios::in);
	  if(!fin) {
	    cout << "Read I Image : Cannot open file for input.\n";
	  }

	  int char_cnt = 0;
	  int ins_cnt =0; 
	  int ch_int;
	  while (fin.get(ch)) {
	  	// cout << "Start Read Char\n";
		if(char_cnt<4){
		    if (char_cnt==0){
		 		PC += ch<<24;
		 		// ch_int = ch;
		 		// cout << "PC = " << PC << ", ch = " << ch_int << "\n";
		 	}
		 	else if (char_cnt==1){
		 		PC += ch<<16;
		 		// ch_int = ch;
		 		// cout << "PC = " << PC << ", ch = " << ch_int << "\n";
		 	}
		 	else if (char_cnt==2){
		 		PC += ch<<8;
		 		// ch_int = ch;
		 		// cout << "PC = " << PC << ", ch = " << ch_int << "\n";
		 	}
		 	else if (char_cnt==3)
		 	{
		 		PC += ch;
		 		ins_cnt= PC;
		 		// ch_int = ch;
		 		// cout << "PC = " << PC << ", ch = " << ch_int << "\n";
		 	}
	    }
	    else if(char_cnt<8){
	    	if (char_cnt==4){
		 		insAmount += ch<<24;
		 		// printf("%d\n", ch);
		 		// cout << "Instruction Amount = "<< insAmount <<endl;
		 	}
		 	else if (char_cnt==5){
		 		insAmount += ch<<16;
		 		// cout << "Instruction Amount = "<< insAmount <<endl;
		 	}
		 	else if (char_cnt==6){
		 		insAmount += ch<<8;
		 		// cout << "Instruction Amount = "<< insAmount <<endl;
		 	}
		 	else
		 	{
		 		insAmount += ch;
		 		// cout << "Instruction Amount = "<< insAmount <<endl;
		 	}
	    }
	    else{
	    	char real_inst[2];
		    real_inst[0] = (ch & 0xf0) >> 4;
		    real_inst[1] = (ch & 0x0f);
		    for(int j = 0; j < 2; j = j + 1)
		    {
		 		switch(real_inst[j])
			 	{
			 		case 0x00:
			 			{
				 			imemory[ins_cnt][j*4] = false;
				 			imemory[ins_cnt][j*4+1] = false;
				 			imemory[ins_cnt][j*4+2] =false;
				 			imemory[ins_cnt][j*4+3] = false;
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		case 0x01:
			 			{
				 			imemory[ins_cnt][j * 4] = false;
				 			imemory[ins_cnt][j * 4 + 1] = false;
				 			imemory[ins_cnt][j * 4 + 2] =false;
				 			imemory[ins_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);

			 			}
			 			break;
			 		 case 0x02:
			 			{
				 			imemory[ins_cnt][j * 4] = false;
				 			imemory[ins_cnt][j * 4 + 1] = false;
				 			imemory[ins_cnt][j * 4 + 2] =true;
				 			imemory[ins_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x03:
			 			{
				 			imemory[ins_cnt][j * 4] = false;
				 			imemory[ins_cnt][j * 4 + 1] = false;
				 			imemory[ins_cnt][j * 4 + 2] =true;
				 			imemory[ins_cnt][j * 4 + 3] = true;	
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		 case 0x04:
			 			{
				 			imemory[ins_cnt][j * 4] = false;
				 			imemory[ins_cnt][j * 4 + 1] = true;
				 			imemory[ins_cnt][j * 4 + 2] =false;
				 			imemory[ins_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		 case 0x05:
			 			{
				 			imemory[ins_cnt][j * 4] = false;
				 			imemory[ins_cnt][j * 4 + 1] = true;
				 			imemory[ins_cnt][j * 4 + 2] =false;
				 			imemory[ins_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x06:
			 			{
				 			imemory[ins_cnt][j * 4] = false;
				 			imemory[ins_cnt][j * 4 + 1] = true;
				 			imemory[ins_cnt][j * 4 + 2] =true;
				 			imemory[ins_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x07:
			 			{
				 			imemory[ins_cnt][j * 4] = false;
				 			imemory[ins_cnt][j * 4 + 1] = true;
				 			imemory[ins_cnt][j * 4 + 2] =true;
				 			imemory[ins_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		 case 0x08:
			 			{
				 			imemory[ins_cnt][j * 4] = true;
				 			imemory[ins_cnt][j * 4 + 1] = false;
				 			imemory[ins_cnt][j * 4 + 2] =false;
				 			imemory[ins_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x09:
			 			{
				 			imemory[ins_cnt][j * 4] = true;
				 			imemory[ins_cnt][j * 4 + 1] = false;
				 			imemory[ins_cnt][j * 4 + 2] =false;
				 			imemory[ins_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x0A:
			 			{
				 			imemory[ins_cnt][j * 4] = true;
				 			imemory[ins_cnt][j * 4 + 1] = false;
				 			imemory[ins_cnt][j * 4 + 2] =true;
				 			imemory[ins_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x0B:
			 			{
				 			imemory[ins_cnt][j * 4] = true;
				 			imemory[ins_cnt][j * 4 + 1] = false;
				 			imemory[ins_cnt][j * 4 + 2] =true;
				 			imemory[ins_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x0C:
			 			{
				 			imemory[ins_cnt][j * 4] = true;
				 			imemory[ins_cnt][j * 4 + 1] = true;
				 			imemory[ins_cnt][j * 4 + 2] =false;
				 			imemory[ins_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x0D:
			 			{
				 			imemory[ins_cnt][j * 4] = true;
				 			imemory[ins_cnt][j * 4 + 1] = true;
				 			imemory[ins_cnt][j * 4 + 2] =false;
				 			imemory[ins_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x0E:
			 			{
				 			imemory[ins_cnt][j * 4] = true;
				 			imemory[ins_cnt][j * 4 + 1] = true;
				 			imemory[ins_cnt][j * 4 + 2] =true;
				 			imemory[ins_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		 case 0x0F:
			 			{
				 			imemory[ins_cnt][j * 4] = true;
				 			imemory[ins_cnt][j * 4 + 1] = true;
				 			imemory[ins_cnt][j * 4 + 2] =true;
				 			imemory[ins_cnt][j * 4 + 3] = true;	
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		default:
			 			printf("Read I Image : WTF!!!");
			 		 	break;
			 	} //switch

		 	} //for
		 	ins_cnt++;
	    } //else
	    char_cnt++;
	 }
	 cout << "Read I Image : PC = "<< PC <<endl;
	 cout << "Read I Image : Instruction Amount = "<< insAmount <<endl;
	 // cout << "Imemory[0][1] = "<< imemory[0][1] <<endl;
}

void readDimage(string s){
	char ch;
	ifstream fin(s, ios::in);
	  if(!fin) {
	    cout << "Read D Image : Cannot open file for input.\n";
	  }
	  int char_cnt = 0;
	  int data_cnt = 0;

	  while (fin.get(ch)){

	  	if(char_cnt<4){
		    if (char_cnt==0){
		 		stackPointer += ch<<24;
		 	}
		 	else if (char_cnt==1){
		 		stackPointer += ch<<16;
		 	}
		 	else if (char_cnt==2){
		 		stackPointer += ch<<8;
		 	}
		 	else if (char_cnt==3)
		 	{
		 		stackPointer += ch;
		 	}
	    }
	    else if(char_cnt<8){
	    	if (char_cnt==4){
		 		dataAmount += ch<<24;
		 	}
		 	else if (char_cnt==5){
		 		dataAmount += ch<<16;
		 	}
		 	else if (char_cnt==6){
		 		dataAmount += ch<<8;
		 	}
		 	else
		 	{
		 		dataAmount += ch;
		 	}
	    }
	    else{
	    	char real_inst[2];
		    real_inst[0] = (ch & 0xf0) >> 4;
		    real_inst[1] = (ch & 0x0f);
		    for(int j = 0; j < 2; j = j + 1)
		    {
		 		switch(real_inst[j])
			 	{
			 		case 0x00:
			 			{
				 			dmemory[data_cnt][j*4] = false;
				 			dmemory[data_cnt][j*4+1] = false;
				 			dmemory[data_cnt][j*4+2] =false;
				 			dmemory[data_cnt][j*4+3] = false;
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		case 0x01:
			 			{
				 			dmemory[data_cnt][j * 4] = false;
				 			dmemory[data_cnt][j * 4 + 1] = false;
				 			dmemory[data_cnt][j * 4 + 1] = false;
				 			dmemory[data_cnt][j * 4 + 2] =false;
				 			dmemory[data_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);

			 			}
			 			break;
			 		 case 0x02:
			 			{
				 			dmemory[data_cnt][j * 4] = false;
				 			dmemory[data_cnt][j * 4 + 1] = false;
				 			dmemory[data_cnt][j * 4 + 2] =true;
				 			dmemory[data_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x03:
			 			{
				 			dmemory[data_cnt][j * 4] = false;
				 			dmemory[data_cnt][j * 4 + 1] = false;
				 			dmemory[data_cnt][j * 4 + 2] =true;
				 			dmemory[data_cnt][j * 4 + 3] = true;	
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		 case 0x04:
			 			{
				 			dmemory[data_cnt][j * 4] = false;
				 			dmemory[data_cnt][j * 4 + 1] = true;
				 			dmemory[data_cnt][j * 4 + 2] =false;
				 			dmemory[data_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		 case 0x05:
			 			{
				 			dmemory[data_cnt][j * 4] = false;
				 			dmemory[data_cnt][j * 4 + 1] = true;
				 			dmemory[data_cnt][j * 4 + 2] =false;
				 			dmemory[data_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x06:
			 			{
				 			dmemory[data_cnt][j * 4] = false;
				 			dmemory[data_cnt][j * 4 + 1] = true;
				 			dmemory[data_cnt][j * 4 + 2] =true;
				 			dmemory[data_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x07:
			 			{
				 			dmemory[data_cnt][j * 4] = false;
				 			dmemory[data_cnt][j * 4 + 1] = true;
				 			dmemory[data_cnt][j * 4 + 2] =true;
				 			dmemory[data_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		 case 0x08:
			 			{
				 			dmemory[data_cnt][j * 4] = true;
				 			dmemory[data_cnt][j * 4 + 1] = false;
				 			dmemory[data_cnt][j * 4 + 2] =false;
				 			dmemory[data_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x09:
			 			{
				 			dmemory[data_cnt][j * 4] = true;
				 			dmemory[data_cnt][j * 4 + 1] = false;
				 			dmemory[data_cnt][j * 4 + 2] =false;
				 			dmemory[data_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x0A:
			 			{
				 			dmemory[data_cnt][j * 4] = true;
				 			dmemory[data_cnt][j * 4 + 1] = false;
				 			dmemory[data_cnt][j * 4 + 2] =true;
				 			dmemory[data_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x0B:
			 			{
				 			dmemory[data_cnt][j * 4] = true;
				 			dmemory[data_cnt][j * 4 + 1] = false;
				 			dmemory[data_cnt][j * 4 + 2] =true;
				 			dmemory[data_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x0C:
			 			{
				 			dmemory[data_cnt][j * 4] = true;
				 			dmemory[data_cnt][j * 4 + 1] = true;
				 			dmemory[data_cnt][j * 4 + 2] =false;
				 			dmemory[data_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x0D:
			 			{
				 			dmemory[data_cnt][j * 4] = true;
				 			dmemory[data_cnt][j * 4 + 1] = true;
				 			dmemory[data_cnt][j * 4 + 2] =false;
				 			dmemory[data_cnt][j * 4 + 3] = true;
				 			// printf("%d ", real_inst[j]);	
			 			}
			 			break;
			 		 case 0x0E:
			 			{
				 			dmemory[data_cnt][j * 4] = true;
				 			dmemory[data_cnt][j * 4 + 1] = true;
				 			dmemory[data_cnt][j * 4 + 2] =true;
				 			dmemory[data_cnt][j * 4 + 3] = false;
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		 case 0x0F:
			 			{
				 			dmemory[data_cnt][j * 4] = true;
				 			dmemory[data_cnt][j * 4 + 1] = true;
				 			dmemory[data_cnt][j * 4 + 2] =true;
				 			dmemory[data_cnt][j * 4 + 3] = true;	
				 			// printf("%d ", real_inst[j]);
			 			}
			 			break;
			 		default:
			 			printf("WTF!!!");
			 		 	break;
			 	} //switch

		 	} //for
		 	data_cnt++;

	    }//else
	    char_cnt++;
	    // cout << "Dmemory[0][1] = "<< dmemory[0][1] <<endl;

	  }//while
	  registers[29]=stackPointer;
	    cout << "Read D Image : stack pointer = "<< stackPointer <<endl;
	    cout << "Read D Image : data Amount = "<< dataAmount <<endl;
}

void decodeIns(int PC){

	outputRegisterContent(snapshot);
	outputErrorMessage(errorMessage);

	cout << "Decode Inst. : Start Decode!\n";
	if(FINISH){
		cout << "Decode Inst. : Success Decode!\n";
	}
	while(!FINISH){
		
		int counter = 0;
		int opc = 0;

		cycleCnt ++;
		myfile << endl;
		myfile << endl;
		myfile << "cycle "<< cycleCnt << endl;

		for(int a=0; a<4; a++){
			for(int b=0; b<8; b++){
				instruction[counter]=imemory[PC+a][b];
				counter++;
			}
		}
		for(int j=0; j< opcodeLength; j++){
			opc += instruction[j] ? (1 << (opcodeLength - 1 - j)) : 0;
			cout << instruction[j] << " ";
			cout << "Decode Inst. : Imemory index = "<< PC * 4 << ", opCode = " << opc << endl;
		}
		cout << "\nDecode Inst. : Final opCode = " << opc << "\n";
		if(opc==0x00){
			operateRtypeIns();
			//cout << "Decode Inst. : currently, skip R type instruction\n";
		}
		else if((opc==0x08) || (opc==0x09) || (opc==0x23) || (opc==0x21) || (opc==0x25) 
			|| (opc==0x20) || (opc==0x24) || (opc==0x2B) || (opc==0x29) || (opc==0x28)
			||(opc==0x0F) || (opc==0x0C) || (opc==0x0D)|| (opc==0x0E) || (opc==0x0A) 
			|| (opc==0x04) || (opc==0x05) || (opc==0x07)){
			cout << "Decode Inst. : instruction = ";
			for(int i = 0; i < 32; i++)
				cout << instruction[i] << " ";
			cout << endl;
			operateItypeIns(opc);
		}
		else if((opc==0x02)|| (opc==0x03)||(opc==0x3F)){
			operateJtypeIns(opc);
		}
		else{
			cout << "Decode Inst. : Error : OPCODE NOT FOUND" << endl;
		}	
		PC+=4;
		myfile << "PC: 0x"<< hex << PC << endl;
	}
	cout << "Decode Inst. : Success Decode!\n";
	myfile.close();
	errordump.close();
}

int extractUnsignedIns(int end, int start, bool b[]){
	cout << "Ext. U Inst. : Start extract Unsigned ins!\n";

	int length = end - start;
	int cnt = length-1;
	int value = 0;
	for(int i= end; i>start; i--){
		value += b[i] ? (1<<(length-cnt-1)) : 0;
		cnt--;
	}
	cout << "Ext. U Inst. : Return Value = " << value << endl;
	cout << "Ext. U Inst. : Success extract unsigned ins!\n";
	return value;
}

int extractSignedIns(int end, int start, bool b[]){

	cout << "Ext. S Inst. : Start extract signed ins!\n";
	int length = end - start;
	int cnt = length-1;
	int value = 0;
	for(int i= end; i>start+1; i--){
		value += b[i] ? (1<<(length-cnt-1)) : 0;
		//cout << "    -->     b[" << i << "] = " << b[i] << ", value = " << value << endl;
		cnt--;
	}

	value = value - (b[start + 1] ? (1<<(length-1)) : 0);
	//cout << "    -->     b[" << start + 1 << "] = " << b[start + 1] << ", value = " << value << endl;

	cout << "Ext. S Inst. : Retuen Value = " << value << endl;
	cout << "Ext. S Inst. : Success extract signed ins!\n"; 
	return value;
}

int countSignedValue(bitset<32> b){
	cout << "Start count signed value!!\n";
	int value = 0;
	for(int i=0; i<32; i++){
		value+= b[i]<<i;
	}
	cout << "Success count signed value!\n";
	return value;
}

int countUnsignedValue(bitset<32> b){
	cout << "Start count signed value!\n";
	int value = 0;
	for(int i=0; i<31; i++){
		value+= b[i]<<i;
	}
	value = -(b[32]<<31)+value;
	cout << "Success count signed value!\n";
	return value;
}

void  operateRtypeIns(){

	cout << "Start operate R-type ins!\n";

	int functc = extractUnsignedIns(31, 25, instruction);
	int shamtc = extractUnsignedIns(25, 20, instruction);
	int rdc = extractUnsignedIns(20, 15, instruction);
	int rtc = extractUnsignedIns(15, 10, instruction);
	int rsc = extractUnsignedIns(10, 5, instruction);

	switch(functc){

		case 0x20://add 
		{
			if(rdc==0){
				writeTo0Detected();
			}
			else{		
				registers[rdc]= countSignedValue(registers[rtc])+countSignedValue(registers[rsc]);
				myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;
			}
			if(((registers[rsc].to_ulong()>0)&&(registers[rtc].to_ulong()>0)&&(registers[rsc].to_ulong()+registers[rtc].to_ulong()<0))||((registers[rsc].to_ulong()<0)&&(registers[rtc].to_ulong()<0)&&(registers[rsc].to_ulong()+registers[rtc].to_ulong()>0))){
				numberOverflowDetected();
			}				
        }
        break;
        case 0x21://addu (rt could be unsigned)
        {
        	if(rdc==0){
				writeTo0Detected();
			}
			else{
				registers[rdc]= countUnsignedValue(registers[rtc])+countUnsignedValue(registers[rsc]);
	       		myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;			
       		}     		
        }
        break;
        case 0x22://sub
        {
        	if(rdc==0){
				writeTo0Detected();
			}
			else{
				registers[rdc]= countSignedValue(registers[rtc])+ countSignedValue(registers[rsc]);
	        	myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;  
			}
			if(((registers[rsc].to_ulong()>0)&&(registers[rtc].to_ulong()>0)&&(registers[rsc].to_ulong()+registers[rtc].to_ulong()<0))||((registers[rsc].to_ulong()<0)&&(registers[rtc].to_ulong()<0)&&(registers[rsc].to_ulong()+registers[rtc].to_ulong()>0))){
				numberOverflowDetected();
			}
        }
        break;
        case 0x24://and
        {
        	if(rdc==0){
				writeTo0Detected();
			}
			else{
	        	registers[rdc]= registers[rtc] & registers[rsc];
	        	myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;
			}  
        }
        break;
        case 0x25://or
        {
        	if(rdc==0){
				writeTo0Detected();
			}
			else{
	        	registers[rdc]= registers[rtc]|registers[rsc];
	        	myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;  				
	        }
        }
        break;
        case 0x26://xor
        {
        	if(rdc==0){
				writeTo0Detected();
			}
			else{
				registers[rdc]= registers[rtc]^registers[rsc];
        	    myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;  
			}
        }
        break;
        case 0x27://nor
        {
        	if(rdc==0){
				writeTo0Detected();
			}
			else{
	        	registers[rdc]= ~(registers[rtc]|registers[rsc]);
	        	myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;  
			}
        }
        break;
        case 0x28://nand
        {
        	if(rdc==0){
				writeTo0Detected();
			}
			else{
	            registers[rdc]= ~(registers[rtc]&registers[rsc]);
        	    myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;
			}  
        }
        break;
        case 0x2A://slt
        {
        	if(rdc==0){
				writeTo0Detected();
			}
			else{
	        	if(countSignedValue(registers[rsc])<countSignedValue(registers[rtc])){
	        		registers[rdc]=1;
	        	} 
	        	else{
	        		registers[rdc]=0;
	        	}
	        	myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;
			} 
        }
        break;
        case 0x00://sll
        {
        	if(rdc==0){
        		if(!(rtc==0 && shamtc ==0)){
        			writeTo0Detected();
        		}
			}
			else{
	        	registers[rdc] = registers[rtc] << shamtc;
	        	myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;
			}
        }
        break;
        case 0x02://srl
        {
        	if(rdc==0){
				writeTo0Detected();
			}
			else{
	        	registers[rdc] = registers[rtc] >> shamtc;
	        	myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;
			}	
        }
        break;
        case 0x03://sra (with sign bit shifted in)
        {
        	if(rdc==0){
				writeTo0Detected();
			}
			else{
			    if(registers[rtc][31]!=1){
	        		registers[rdc] = registers[rtc] >> shamtc;	
	        	}
	        	else{
	        		registers[rdc] = registers[rtc] >> shamtc;
	        		for(int i=31; i>(31-shamtc); i--){
	        			registers[rdc][i]=1;
	        		}
	        	} 
	        	myfile << "$"  <<rdc<<": 0x"<< hex <<registers[rdc].to_ulong()<< endl;
			}       	
        }
        break;
        case 0x08://jr
        {
        	PC = countSignedValue(registers[rsc]);
        }
        break;
        case 0x18://mult
        {
        	bitset<64> b = 0;
        	b= countSignedValue(registers[rsc])*countSignedValue(registers[rtc]);
        	for(int i=0; i<32; i++){
        		LO[i]= b[i];
        	}
        	for(int i=32; i<64; i++){
        		HI[i-32]= b[i];
        	}
        	if(HI.to_ulong()!=0){
        		myfile << "HI: "<<": 0x"<< hex <<HI.to_ulong()<< endl;     		
        	}
        	if(((registers[rsc].to_ulong()>0)&&(registers[rtc].to_ulong()>0)&&(registers[rsc].to_ulong()*registers[rtc].to_ulong()<0))||((registers[rsc].to_ulong()<0)&&(registers[rtc].to_ulong()<0)&&(registers[rsc].to_ulong()*registers[rtc].to_ulong()>0))){
				numberOverflowDetected();
			}
			if(HILO_OCCUPIED){
        		overwriteHILODetected();
        	}
        	myfile << "LO: "<<": 0x"<< hex <<LO.to_ulong()<< endl;
        	HILO_OCCUPIED = true;
        }
        break;
        case 0x19://multu
        {
			bitset<64> b = 0;
			b = countUnsignedValue(registers[rsc])*countUnsignedValue(registers[rsc]);
			for(int i=0; i<32; i++){
        		LO[i]= b[i];
        	}
        	for(int i=32; i<64; i++){
        		HI[i-32]= b[i];
        	}
        	if(HI.to_ulong()!=0){
        		myfile << "HI: "<<": 0x"<< hex <<HI.to_ulong()<< endl;     		
        	}
        	if(HILO_OCCUPIED){
        		overwriteHILODetected();
        	}
        	myfile << "LO: "<<": 0x"<< hex <<LO.to_ulong()<< endl;
        	HILO_OCCUPIED = true;
        }
        break;
        case 0x10://mfhi
        {
        	HILO_OCCUPIED = false;
        	if(rdc==0){
				writeTo0Detected();
			}
        	HI = registers[rdc];
        	myfile << "HI: "<<": 0x"<< hex <<HI.to_ulong()<< endl; 
        }
        break;
        case 0x12://mflo
        {
        	HILO_OCCUPIED = false;
        	if(rdc==0){
				writeTo0Detected();
			}
        	LO = registers[rdc];
        	myfile << "LO: "<<": 0x"<< hex <<LO.to_ulong()<< endl; 
        }
        default:
        // cout << "illegal instruction found at 0x" << address;
        //TODO halt;
        break;
    }
    cout << "Success operate R-type ins!\n";
}

void operateItypeIns(int opc){
	cout << "I type Inst. : Start operate i-type in! opc = " << opc <<"\n";
	int unsignedImmc = extractUnsignedIns(31, 15, instruction);
	int signedImmc = extractSignedIns(31, 15, instruction);
	int rtc = extractUnsignedIns(15, 10, instruction);
	int rsc = extractUnsignedIns(10, 5, instruction);

	for(int i=0; i<16; i++){
		immc[i] =instruction[31-i];
	}
	for(int i=16; i<32; i++){
		immc[i]=0;
	}

	cout<<"current immc = "<< immc <<endl;

	cout << "I type Inst. : the first I-type ins's rs is : " << rsc << endl;

      switch (opc)
      {          
         case 0x08://I-type instructions, addi 
         {
         	if(rtc==0){
				writeTo0Detected();
			}
			else{		
				registers[rtc]=countSignedValue(registers[rsc])+signedImmc;
				myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl;
			}
			if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			}
         }
         break;
         case 0x09://I-type instructions, addiu 
         {
         	if(rtc==0){
				writeTo0Detected();
			}
			else{
	       		registers[rtc]= countSignedValue(registers[rtc])+unsignedImmc-BOUNDARY;
				myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl;
			}        	
         }
         break;
         case 0x23://I-type instructions, lw
         {
         	if(rtc==0){
				writeTo0Detected();
			}
			else{
	         	int tempRtc =0;
	         	for(int i=3; i>=0; i--){
	         		cout << "opCode 0x23 : rsc = " << rsc << ", signedImmc * 4 = " << signedImmc * 4 << ", 3 - i = " << 3 - i << endl;
	         		cout << "opCode 0x23 : Dmemory index = [" << registers[rsc].to_ulong()+signedImmc*4+(3-i) << "]\n";
	         		tempRtc += extractUnsignedIns(7, -1, dmemory[registers[rsc].to_ulong()+signedImmc*4+(3-i)])<<(i*8);
	         	}
	         	registers[rtc]= tempRtc;

	         	myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl;
			} 
			if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			}
			if(!((4*signedImmc+registers[rsc].to_ulong())< BOUNDARY)){
				addressOverflowDetected();
			}
			cout << "--> " << (4*signedImmc+registers[rsc].to_ulong())%4 << "\n";
			if((4*signedImmc+registers[rsc].to_ulong())%4!=0){
				misalignmentErrorDetected();
			}
         }
         break;
         case 0x21://I-type instructions, lh
         {
         	if(rtc==0){
				writeTo0Detected();
			}
			else{
	         	int tempRtc =0;
	         	bitset<32> temp =  0xffff0000;
	         	for(int i=1; i>=0; i--){
	         		tempRtc += extractUnsignedIns(7, -1, dmemory[registers[rsc].to_ulong()+signedImmc*2+(1-i)])<<(i*8);
	         	}
	         	registers[rtc]= tempRtc;
	         	if(registers[rtc][15]==1){
	         		registers[rtc] = registers[rtc] | temp;
	         	}

	         	myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl; 
			}
			if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			}
			if(!((2*signedImmc+registers[rsc].to_ulong())< BOUNDARY)){
				addressOverflowDetected();
			}
			if((2*signedImmc+registers[rsc].to_ulong())%2!=0){
				misalignmentErrorDetected();
			}
         }
         break; 
         case 0x25://I-type instructions, lhu
         {
         	if(rtc==0){
				writeTo0Detected();
			}
			else{
				int tempRtc =0;
	         	for(int i=1; i>=0; i--){
	         		tempRtc += extractUnsignedIns(7, -1, dmemory[registers[rsc].to_ulong()+signedImmc*2+(1-i)])<<(i*8);
	         	}
	         	registers[rtc]= tempRtc;
	         	myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl;
			} 
		    if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			}
			if(!((2*signedImmc+registers[rsc].to_ulong())< BOUNDARY)){
				addressOverflowDetected();
			}
			if((2*signedImmc+registers[rsc].to_ulong())%2!=0){
				misalignmentErrorDetected();
			}
         }
         break;
         case 0x20://I-type instructions, lb 
         {
          	if(rtc==0){
				writeTo0Detected();
			} 
			else{
	         	int tempRtc =0;
	         	bitset<32> temp =  0xffffff00;
	         	tempRtc += extractUnsignedIns(7, -1, dmemory[registers[rsc].to_ulong()+signedImmc]);
	         	registers[rtc]= tempRtc;
	         	if(registers[rtc][7]==1){
	         		registers[rtc] = registers[rtc] | temp;
	         	}
	         	myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl; 
			}
	        if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			}
			if(!((signedImmc+registers[rsc].to_ulong())< BOUNDARY)){
				addressOverflowDetected();
			}      
         }
         break;
         case 0x24://I-type instructions, lbu 
         {
         	if(rtc==0){
				writeTo0Detected();
			}    
			else{
	         	int tempRtc =0;
	         	tempRtc += extractUnsignedIns(7, -1, dmemory[registers[rsc].to_ulong()+signedImmc]);
	         	registers[rtc]= tempRtc;
	         	myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl; 
			}
	        if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			}
			if(!((signedImmc+registers[rsc].to_ulong())< BOUNDARY)){
				addressOverflowDetected();
			} 	
         }
         break;
         case 0x2B://I-type instructions, sw
         {
         	for(int i=0; i<4; i++){
         		if(i==0){
         			for(int j=0; j<8; j++){
         				cout << "opCode 0x2B : Dmemory index = [" << 4*signedImmc+registers[rsc].to_ulong()+i << "][" << i << "]\n";
         				dmemory[4*signedImmc+registers[rsc].to_ulong()+i][j]= registers[rtc][31-j];
         			}
         		}
         		else if(i==1){
         			for(int j=0; j<8; j++){
         				cout << "opCode 0x2B : Dmemory index = [" << 4*signedImmc+registers[rsc].to_ulong()+i << "][" << i << "]\n";
         				dmemory[4*signedImmc+registers[rsc].to_ulong()+i][j]= registers[rtc][23-j];
         			}
         		}
         		else if(i==2){
         			for(int j=0; j<8; j++){
         				cout << "opCode 0x2B : Dmemory index = [" << 4*signedImmc+registers[rsc].to_ulong()+i << "][" << i << "]\n";
         				dmemory[4*signedImmc+registers[rsc].to_ulong()+i][j]= registers[rtc][15-j];
         			}
         		}
         		else{
         			for(int j=0; j<8; j++){
         				cout << "opCode 0x2B : Dmemory index = [" << 4*signedImmc+registers[rsc].to_ulong()+i << "][" << i << "]\n";
         				dmemory[4*signedImmc+registers[rsc].to_ulong()+i][j]= registers[rtc][7-j];
         			}
         		}
         	}
         	if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			}
			if(!((4*signedImmc+registers[rsc].to_ulong())< BOUNDARY)){
				addressOverflowDetected();
			} 
         }
         break;
         case 0x29://I-type instructions, sh 
         {
         	for(int i=0; i<2; i++){
         		if(i==0){
         			for(int j=0; j<8; j++){
         				dmemory[signedImmc+registers[rsc].to_ulong()+i][j]= registers[rtc][15-j];
         			}
         		}
         		else{
         			for(int j=0; j<8; j++){
         				dmemory[signedImmc+registers[rsc].to_ulong()+i][j]= registers[rtc][7-j];
         			}
         		}
         	}
         	if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			}
			if(!((2*signedImmc+registers[rsc].to_ulong())< BOUNDARY)){
				addressOverflowDetected();
			} 
         }
         break;
         case 0x28://I-type instructions, sb
         {
         	for(int i=0; i<8; i++){
         		dmemory[signedImmc+registers[rsc].to_ulong()][i]=registers[rtc][7-i];
         	}
         	if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			}
			if(!((signedImmc+registers[rsc].to_ulong())< BOUNDARY)){
				addressOverflowDetected();
			} 
         }
         break;
         case 0x0F://I-type instructions, lui 
         {
          	if(rtc==0){
				writeTo0Detected();
			} 
			else{
	         	registers[rtc]= signedImmc << 16;
	         	myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl;
			}        
         }
         break;
         case 0x0C://I-type instructions, andi 
         {
         	if(rtc==0){
				writeTo0Detected();
			}
			else{
	         	registers[rtc]= registers[rsc] & immc;
	         	myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl; 				
         	}         	
         }
         break;
         case 0x0D://I-type instructions, ori 
         {
         	if(rtc==0){
				writeTo0Detected();
			}  
			else{
	         	registers[rtc]= registers[rsc] | immc;
	         	myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl;				
			}       	
         }
         break;
         case 0x0E://I-type instructions, nori 
         {
         	if(rtc==0){
				writeTo0Detected();
			}
			else{
	         	registers[rtc]= ~(registers[rsc] | immc);
	         	myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl; 
			}      	
         }
         break;          
         case 0x0A://I-type instructions, slti 
         {
         	if(rtc==0){
				writeTo0Detected();
			}
			else{
	        	if(countSignedValue(registers[rsc])<signedImmc){
	        		registers[rtc]=1;
	        	} 
	        	else{
	        		registers[rtc]=0;
	        	}
	        	myfile << "$"  <<rtc<<": 0x"<< hex <<registers[rtc].to_ulong()<< endl; 				
			}         	
         }
         break;
         case 0x04://I-type instructions, beq 
         {
        	if(countSignedValue(registers[rsc])==countSignedValue(registers[rtc])){
        		PC = countUnsignedValue(PC) + 4 + 4*signedImmc;
        	}
        	if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			} 
         }
         break;
         case 0x05://I-type instructions, bne
         {
        	if(countSignedValue(registers[rsc])!=countSignedValue(registers[rtc])){
        		PC = countUnsignedValue(PC) + 4 + 4*signedImmc;
        	} 
        	if(((registers[rsc].to_ulong()>0)&&(signedImmc>0)&&(registers[rsc].to_ulong()+signedImmc<0))||((registers[rsc].to_ulong()<0)&&(signedImmc<0)&&(registers[rsc].to_ulong()+signedImmc>0))){
				numberOverflowDetected();
			}
         }
         break;
         case 0x07://I-type instructions, bgtz
         {
        	if(countSignedValue(registers[rsc])>0){
        		PC = countUnsignedValue(PC) + 4 + 4*signedImmc;
        	} 
         }
         break;
         default:  
            // cout << "illegal instruction found at 0x" + address;
            //TODO halt;
            break;
      }
      cout << "I type Inst. : Success operate i-type ins!\n";

}

void operateJtypeIns(int opc){

	cout << "Start operate J-type ins!\n";

	int unsignedAddressc = extractUnsignedIns(31, 5, instruction);

	switch(opc){
		case 0x02:
		{
			PC = PC+4;
			bitset <4> frontPC;
			frontPC = (PC&0xffffffff)>>28;
			bitset <32> tempPC;
			tempPC = unsignedAddressc <<2;
			for(int j=31; j>27; j--){
				tempPC[j]= frontPC[j-28];
			}
			PC = countUnsignedValue(tempPC);
		}
		break;
		case 0x03:
		{
			PC = PC+4;
			registers[31]= PC;
			bitset <4> frontPC;
			frontPC = (PC&0xffffffff)>>28;
			bitset <32> tempPC;
			tempPC = unsignedAddressc <<2;
			for(int j=31; j>27; j--){
				tempPC[j]= frontPC[j-28];
			}
			PC = countUnsignedValue(tempPC);
		}
		break;
		case 0x3F:
		{
			FINISH= true;
		}
		break;
		default:
		break;
	}
	cout << "Success operate J-type ins!\n";
}

void outputRegisterContent(string s){
cout << "Out Register : start\n";
	// ofstream output(s.c_str(), ios::app);
	myfile.open (s);
	myfile << "cycle "<< cycleCnt << endl;
	char buffer_regIndex[3];
	char buffer_regContent[9];

	for(int i=0; i< 32; i++){

		sprintf(buffer_regIndex, "%02d", i);
		sprintf(buffer_regContent, "%08lx", registers[i].to_ulong());

		myfile << "$" << buffer_regIndex << ": 0x" << buffer_regContent << endl;

	}
	sprintf(buffer_regContent, "%08lx", HI.to_ulong());
	myfile << "HI: 0x"<< buffer_regContent << endl;
	sprintf(buffer_regContent, "%08lx", LO.to_ulong());
	myfile << "LO: 0x"<< buffer_regContent << endl;
	sprintf(buffer_regContent, "%08x", PC);
	myfile << "PC: 0x"<< buffer_regContent << endl;

}

void outputErrorMessage(string s){	
	errordump.open(s);
}

void writeTo0Detected(){
	errordump << "In cycle "<< cycleCnt <<": Write $0 Error" <<endl;
}

void numberOverflowDetected(){
	errordump << "In cycle: "<< cycleCnt <<": Number Overflow" <<endl;
}

void overwriteHILODetected(){
	errordump << "In cycle: "<< cycleCnt <<": Overwrite HI-LO registers" <<endl;
}

void addressOverflowDetected(){	
	errordump << "In cycle: "<< cycleCnt <<": Address Overflow" <<endl;
	FINISH = true;
}

void misalignmentErrorDetected(){
	errordump << "In cycle: "<< cycleCnt <<": Misalignment Error" <<endl;
	FINISH = true;
}


