nclude <fstream>
#include <iterator>
#include <vector>
#include <iostream> 
#include <string.h>
#include <stdio.h>

using namespace std;

string iimageFile = "iimage.bin";
string dimageFile = "dimage.bin";
string registerSnapshot = "snapshot.rpt";
string errorMessageFile = "error_dump.rpt";

int pcCounter =0;
int stackPointer =0;
int instructionAmount =0;
int dataSpace =0;
int iMemory[256];//1word=4byte, 246word=1k
int dMemory[256]; 
int address =0; 
bool opcode [6];
bool functCode[6];
bool rs[5];
bool rt[5];
bool rd[5];
bool shamtC[5];
bool immediateC[26];
bool addressC[26];

bitset<32> registers[32];
bitset<32> HI=0;
bitset<32> LO=0;
bitset<32> immc = 0;

bool instruction[8192];
bool data[8192];

const int opcodeLength = 6;
const int functcodeLength = 6;
const int rsLength = 5;
const int rtLength = 5;
const int rdLength = 5;
const int shamtCLength = 5;
const int immediateCLength = 26;
const int addressCLength = 26;
const int instructionLength = 32;
const int instructionStart = 64;

int inst_cnt = 0;
int data_cnt = 0;

// void processBinary(string s, bool *b[]);
void readIimage(string s);
void readDimage(string s);
void decodeIns(int i);
void operateRtypeIns(int i);
void operateItypeIns(int i);
void operateJtypeIns(int i);
int extractUnsignedIns(int start, int end, bool b[]);
int countSignedValue(bitset<32> b);
int countUnsignedValue(bitset<32> b);
int maxValue =0;

int main()
{	
	for(int i=0; i<32; i++){
		maxValue += 1>>i;
	}

	readIimage(iimageFile);
	readDimage(dimageFile);
	decodeIns(instructionAmount);
	// outputRegisterContent(registerSnapshot);
	// outputErrorMessage(errorMessageFile);
	return 0;
}

// void processBinary(string filename, bool *b[]){
// ifstream input(filename, ios::binary);
// 	// copies all data into buffer
// 	vector<char> buffer((istreambuf_iterator<char>(input)), 
// 	        (istreambuf_iterator<char>()));;

// 	cout << "read buffer\n";
// 	for(vector<char>::const_iterator i = buffer.begin(); i < buffer.end(); i++)
// 	{
// 	 	//becuz 1 char =8 bit
// 	 	char real_inst[2];
// 	 	real_inst[0] = (*i & 0xf0) >> 4;
// 	 	real_inst[1] = (*i & 0x0f);

// 	 	for(int j = 0; j < 2; j = j + 1)
// 	 	{
// 		 	switch(real_inst[j])
// 		 	{
// 		 		case 0x00:
// 		 			{
// 			 			*b[inst_cnt * 4] = false;
// 			 			*b[inst_cnt * 4 + 1] = false;
// 			 			*b[inst_cnt * 4 + 2] =false;
// 			 			*b[inst_cnt * 4 + 3] = false;
// 			 			// printf("%d ", real_inst[j]);
// 		 			}
// 		 			break;
// 		 		case 0x01:
// 		 			{
// 			 			*b[inst_cnt * 4] = false;
// 			 			*b[inst_cnt * 4 + 1] = false;
// 			 			*b[inst_cnt * 4 + 2] =false;
// 			 			*b[inst_cnt * 4 + 3] = true;
// 			 			// printf("%d ", real_inst[j]);

// 		 			}
// 		 			break;
// 		 		 case 0x02:
// 		 			{
// 			 			*b[inst_cnt * 4] = false;
// 			 			*b[inst_cnt * 4 + 1] = false;
// 			 			*b[inst_cnt * 4 + 2] =true;
// 			 			*b[inst_cnt * 4 + 3] = false;
// 			 			printf("%d ", real_inst[j]);	
// 		 			}
// 		 			break;
// 		 		 case 0x03:
// 		 			{
// 			 			*b[inst_cnt * 4] = false;
// 			 			*b[inst_cnt * 4 + 1] = false;
// 			 			*b[inst_cnt * 4 + 2] =true;
// 			 			*b[inst_cnt * 4 + 3] = true;	
// 			 			// printf("%d ", real_inst[j]);
// 		 			}
// 		 			break;
// 		 		 case 0x04:
// 		 			{
// 			 			*b[inst_cnt * 4] = false;
// 			 			*b[inst_cnt * 4 + 1] = true;
// 			 			*b[inst_cnt * 4 + 2] =false;
// 			 			*b[inst_cnt * 4 + 3] = false;
// 			 			// printf("%d ", real_inst[j]);
// 		 			}
// 		 			break;
// 		 		 case 0x05:
// 		 			{
// 			 			*b[inst_cnt * 4] = false;
// 			 			*b[inst_cnt * 4 + 1] = true;
// 			 			*b[inst_cnt * 4 + 2] =false;
// 			 			*b[inst_cnt * 4 + 3] = true;
// 			 			// printf("%d ", real_inst[j]);	
// 		 			}
// 		 			break;
// 		 		 case 0x06:
// 		 			{
// 			 			*b[inst_cnt * 4] = false;
// 			 			*b[inst_cnt * 4 + 1] = true;
// 			 			*b[inst_cnt * 4 + 2] =true;
// 			 			*b[inst_cnt * 4 + 3] = false;
// 			 			// printf("%d ", real_inst[j]);	
// 		 			}
// 		 			break;
// 		 		 case 0x07:
// 		 			{
// 			 			*b[inst_cnt * 4] = false;
// 			 			*b[inst_cnt * 4 + 1] = true;
// 			 			*b[inst_cnt * 4 + 2] =true;
// 			 			*b[inst_cnt * 4 + 3] = true;
// 			 			// printf("%d ", real_inst[j]);
// 		 			}
// 		 			break;
// 		 		 case 0x08:
// 		 			{
// 			 			*b[inst_cnt * 4] = true;
// 			 			*b[inst_cnt * 4 + 1] = false;
// 			 			*b[inst_cnt * 4 + 2] =false;
// 			 			*b[inst_cnt * 4 + 3] = false;
// 			 			// printf("%d ", real_inst[j]);	
// 		 			}
// 		 			break;
// 		 		 case 0x09:
// 		 			{
// 			 			*b[inst_cnt * 4] = true;
// 			 			*b[inst_cnt * 4 + 1] = false;
// 			 			*b[inst_cnt * 4 + 2] =false;
// 			 			*b[inst_cnt * 4 + 3] = true;
// 			 			// printf("%d ", real_inst[j]);	
// 		 			}
// 		 			break;
// 		 		 case 0x0A:
// 		 			{
// 			 			*b[inst_cnt * 4] = true;
// 			 			*b[inst_cnt * 4 + 1] = false;
// 			 			*b[inst_cnt * 4 + 2] =true;
// 			 			*b[inst_cnt * 4 + 3] = false;
// 			 			// printf("%d ", real_inst[j]);	
// 		 			}
// 		 			break;
// 		 		 case 0x0B:
// 		 			{
// 			 			*b[inst_cnt * 4] = true;
// 			 			*b[inst_cnt * 4 + 1] = false;
// 			 			*b[inst_cnt * 4 + 2] =true;
// 			 			*b[inst_cnt * 4 + 3] = true;
// 			 			// printf("%d ", real_inst[j]);	
// 		 			}
// 		 			break;
// 		 		 case 0x0C:
// 		 			{
// 			 			*b[inst_cnt * 4] = true;
// 			 			*b[inst_cnt * 4 + 1] = true;
// 			 			*b[inst_cnt * 4 + 2] =false;
// 			 			*b[inst_cnt * 4 + 3] = false;
// 			 			// printf("%d ", real_inst[j]);	
// 		 			}
// 		 			break;
// 		 		 case 0x0D:
// 		 			{
// 			 			*b[inst_cnt * 4] = true;
// 			 			*b[inst_cnt * 4 + 1] = true;
// 			 			*b[inst_cnt * 4 + 2] =false;
// 			 			*b[inst_cnt * 4 + 3] = true;
// 			 			// printf("%d ", real_inst[j]);	
// 		 			}
// 		 			break;
// 		 		 case 0x0E:
// 		 			{
// 			 			*b[inst_cnt * 4] = true;
// 			 			*b[inst_cnt * 4 + 1] = true;
// 			 			*b[inst_cnt * 4 + 2] =true;
// 			 			*b[inst_cnt * 4 + 3] = false;
// 			 			// printf("%d ", real_inst[j]);
// 		 			}
// 		 			break;
// 		 		 case 0x0F:
// 		 			{
// 			 			*b[inst_cnt * 4] = true;
// 			 			*b[inst_cnt * 4 + 1] = true;
// 			 			*b[inst_cnt * 4 + 2] =true;
// 			 			*b[inst_cnt * 4 + 3] = true;	
// 			 			printf("%d ", real_inst[j]);
// 		 			}
// 		 			break;
// 		 		default:
// 		 			// printf("WTF!!!");
// 		 		 	break;
// 		 	}
// 		 	inst_cnt = inst_cnt + 1;
// 	 	}
// 	 }
// }

void readIimage(string filename){
	ifstream input(filename, ios::binary);
	// copies all data into buffer
	vector<char> buffer((istreambuf_iterator<char>(input)), 
	        (istreambuf_iterator<char>()));;

	 cout << buffer.size() * sizeof(buffer) << endl;
	 ofstream fout("data.txt", ios::out | ios::binary);
	 fout.write((char*)&buffer[0], buffer.size() * sizeof(buffer));
	 fout.close();

	 cout << "read buffer\n";
	 for(vector<char>::const_iterator i = buffer.begin(); i < buffer.end(); i++)
	 {
	 	//becuz 1 char =8 bit
	 	char real_inst[2];
	 	real_inst[0] = (*i & 0xf0) >> 4;
	 	real_inst[1] = (*i & 0x0f);

	 	for(int j = 0; j < 2; j = j + 1)
	 	{
		 	switch(real_inst[j])
		 	{
		 		case 0x00:
		 			{
			 			instruction[inst_cnt * 4] = false;
			 			instruction[inst_cnt * 4 + 1] = false;
			 			instruction[inst_cnt * 4 + 2] =false;
			 			instruction[inst_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		case 0x01:
		 			{
			 			instruction[inst_cnt * 4] = false;
			 			instruction[inst_cnt * 4 + 1] = false;
			 			instruction[inst_cnt * 4 + 2] =false;
			 			instruction[inst_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);

		 			}
		 			break;
		 		 case 0x02:
		 			{
			 			instruction[inst_cnt * 4] = false;
			 			instruction[inst_cnt * 4 + 1] = false;
			 			instruction[inst_cnt * 4 + 2] =true;
			 			instruction[inst_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x03:
		 			{
			 			instruction[inst_cnt * 4] = false;
			 			instruction[inst_cnt * 4 + 1] = false;
			 			instruction[inst_cnt * 4 + 2] =true;
			 			instruction[inst_cnt * 4 + 3] = true;	
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		 case 0x04:
		 			{
			 			instruction[inst_cnt * 4] = false;
			 			instruction[inst_cnt * 4 + 1] = true;
			 			instruction[inst_cnt * 4 + 2] =false;
			 			instruction[inst_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		 case 0x05:
		 			{
			 			instruction[inst_cnt * 4] = false;
			 			instruction[inst_cnt * 4 + 1] = true;
			 			instruction[inst_cnt * 4 + 2] =false;
			 			instruction[inst_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x06:
		 			{
			 			instruction[inst_cnt * 4] = false;
			 			instruction[inst_cnt * 4 + 1] = true;
			 			instruction[inst_cnt * 4 + 2] =true;
			 			instruction[inst_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x07:
		 			{
			 			instruction[inst_cnt * 4] = false;
			 			instruction[inst_cnt * 4 + 1] = true;
			 			instruction[inst_cnt * 4 + 2] =true;
			 			instruction[inst_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		 case 0x08:
		 			{
			 			instruction[inst_cnt * 4] = true;
			 			instruction[inst_cnt * 4 + 1] = false;
			 			instruction[inst_cnt * 4 + 2] =false;
			 			instruction[inst_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x09:
		 			{
			 			instruction[inst_cnt * 4] = true;
			 			instruction[inst_cnt * 4 + 1] = false;
			 			instruction[inst_cnt * 4 + 2] =false;
			 			instruction[inst_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x0A:
		 			{
			 			instruction[inst_cnt * 4] = true;
			 			instruction[inst_cnt * 4 + 1] = false;
			 			instruction[inst_cnt * 4 + 2] =true;
			 			instruction[inst_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x0B:
		 			{
			 			instruction[inst_cnt * 4] = true;
			 			instruction[inst_cnt * 4 + 1] = false;
			 			instruction[inst_cnt * 4 + 2] =true;
			 			instruction[inst_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x0C:
		 			{
			 			instruction[inst_cnt * 4] = true;
			 			instruction[inst_cnt * 4 + 1] = true;
			 			instruction[inst_cnt * 4 + 2] =false;
			 			instruction[inst_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x0D:
		 			{
			 			instruction[inst_cnt * 4] = true;
			 			instruction[inst_cnt * 4 + 1] = true;
			 			instruction[inst_cnt * 4 + 2] =false;
			 			instruction[inst_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x0E:
		 			{
			 			instruction[inst_cnt * 4] = true;
			 			instruction[inst_cnt * 4 + 1] = true;
			 			instruction[inst_cnt * 4 + 2] =true;
			 			instruction[inst_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		 case 0x0F:
		 			{
			 			instruction[inst_cnt * 4] = true;
			 			instruction[inst_cnt * 4 + 1] = true;
			 			instruction[inst_cnt * 4 + 2] =true;
			 			instruction[inst_cnt * 4 + 3] = true;	
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		default:
		 			printf("WTF!!!");
		 		 	break;
		 	}
		 	inst_cnt = inst_cnt + 1;
	 	}
	 }
	// processBinary("iimage.bin", *instruction[]);
	// cout << "\n Inst cnt = " << inst_cnt << "\n";
	// cout << "\n pcCounter = " << pcCounter << "\n";

	// for(int i = 0; i < inst_cnt; i++){
	// 	cout <<	instruction[i * 4] << " ";
	// 	cout <<	instruction[i * 4 + 1] << " ";
	// 	cout << instruction[i * 4 + 2] << " ";
	// 	cout <<	instruction[i * 4 + 3] << "\n";	
	// }

	for(int i = 0; i < 64 ; i++){
		if(i<32){
			pcCounter += instruction[i] ? (1 << (31 - i)) : 0;
			//cout << "i = " << i << ", pcCountet = " << pcCounter << "\n";
		}
		else{
			instructionAmount += instruction[i] ? (1 << (63 - i)) : 0;
		}
	}

	 cout << "\n pcCounter' = " << pcCounter << "\n";
	 cout << "\n instructionAmount' = " << instructionAmount << "\n";
}

void readDimage(string dimageFile){

	ifstream input(dimageFile, ios::binary);
	// copies all data into buffer
	vector<char> buffer((istreambuf_iterator<char>(input)), 
	        (istreambuf_iterator<char>()));;
	for(vector<char>::const_iterator i = buffer.begin(); i < buffer.end(); i++)
	 {
	 		 	//becuz 1 char =8 bit
	 	char real_inst[2];
	 	real_inst[0] = (*i & 0xf0) >> 4;
	 	real_inst[1] = (*i & 0x0f);

	 	for(int j = 0; j < 2; j = j + 1)
	 	{
		 	switch(real_inst[j])
		 	{
		 		case 0x00:
		 			{
			 			data[data_cnt * 4] = false;
			 			data[data_cnt * 4 + 1] = false;
			 			data[data_cnt * 4 + 2] =false;
			 			data[data_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		case 0x01:
		 			{
			 			data[data_cnt * 4] = false;
			 			data[data_cnt * 4 + 1] = false;
			 			data[data_cnt * 4 + 2] =false;
			 			data[data_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);

		 			}
		 			break;
		 		 case 0x02:
		 			{
			 			data[data_cnt * 4] = false;
			 			data[data_cnt * 4 + 1] = false;
			 			data[data_cnt * 4 + 2] =true;
			 			data[data_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x03:
		 			{
			 			data[data_cnt * 4] = false;
			 			data[data_cnt * 4 + 1] = false;
			 			data[data_cnt * 4 + 2] =true;
			 			data[data_cnt * 4 + 3] = true;	
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		 case 0x04:
		 			{
			 			data[data_cnt * 4] = false;
			 			data[data_cnt * 4 + 1] = true;
			 			data[data_cnt * 4 + 2] =false;
			 			data[data_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		 case 0x05:
		 			{
			 			data[data_cnt * 4] = false;
			 			data[data_cnt * 4 + 1] = true;
			 			data[data_cnt * 4 + 2] =false;
			 			data[data_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x06:
		 			{
			 			data[data_cnt * 4] = false;
			 			data[data_cnt * 4 + 1] = true;
			 			data[data_cnt * 4 + 2] =true;
			 			data[data_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x07:
		 			{
			 			data[data_cnt * 4] = false;
			 			data[data_cnt * 4 + 1] = true;
			 			data[data_cnt * 4 + 2] =true;
			 			data[data_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		 case 0x08:
		 			{
			 			data[data_cnt * 4] = true;
			 			data[data_cnt * 4 + 1] = false;
			 			data[data_cnt * 4 + 2] =false;
			 			data[data_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x09:
		 			{
			 			data[data_cnt * 4] = true;
			 			data[data_cnt * 4 + 1] = false;
			 			data[data_cnt * 4 + 2] =false;
			 			data[data_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x0A:
		 			{
			 			data[data_cnt * 4] = true;
			 			data[data_cnt * 4 + 1] = false;
			 			data[data_cnt * 4 + 2] =true;
			 			data[data_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x0B:
		 			{
			 			data[data_cnt * 4] = true;
			 			data[data_cnt * 4 + 1] = false;
			 			data[data_cnt * 4 + 2] =true;
			 			data[data_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x0C:
		 			{
			 			data[data_cnt * 4] = true;
			 			data[data_cnt * 4 + 1] = true;
			 			data[data_cnt * 4 + 2] =false;
			 			data[data_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x0D:
		 			{
			 			data[data_cnt * 4] = true;
			 			data[data_cnt * 4 + 1] = true;
			 			data[data_cnt * 4 + 2] =false;
			 			data[data_cnt * 4 + 3] = true;
			 			printf("%d ", real_inst[j]);	
		 			}
		 			break;
		 		 case 0x0E:
		 			{
			 			data[data_cnt * 4] = true;
			 			data[data_cnt * 4 + 1] = true;
			 			data[data_cnt * 4 + 2] =true;
			 			data[data_cnt * 4 + 3] = false;
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		 case 0x0F:
		 			{
			 			data[data_cnt * 4] = true;
			 			data[data_cnt * 4 + 1] = true;
			 			data[data_cnt * 4 + 2] =true;
			 			data[data_cnt * 4 + 3] = true;	
			 			printf("%d ", real_inst[j]);
		 			}
		 			break;
		 		default:
		 			printf("WTF!!!");
		 		 	break;
		 	}
		 	data_cnt = data_cnt + 1;
	 	}
	 }
	 int a=0;
	 for(int i = 0; i < 64 ; i++){
		if(i<32){
			a += data[i] ? (1 << (31 - i)) : 0;			
		}
		else{
			dataSpace += data[i] ? (1 << (63 - i)) : 0;
		}
	}
	registers[29] = a;
	cout << " stack pointer = " << registers[29] << "\n";
	cout << " dataSpace = " << dataSpace << "\n";
}

void decodeIns(int instructionAmount){

	for(int i=2; i< (instructionAmount+2); i++){

		int opc = 0; 
		for(int j=0; j< opcodeLength; j++){
			opcode[j]= instruction[(i<<5)+j];
			opc += opcode[j] ? (1 << (opcodeLength - 1 - j)) : 0;
		}
		cout << "instrctuion"<< i << " opCode = " << opc << " --> ";
		if(opc==0x00){
			operateRtypeIns(i);
		}
		else if((opc==0x08) || (opc==0x09) || (opc==0x23) || (opc==0x21) || (opc==0x25) 
			|| (opc==0x20) || (opc==0x24) || (opc==0x2B) || (opc==0x29) || (opc==0x28)
			||(opc==0x0F) || (opc==0x0C) || (opc==0x0D)|| (opc==0x0E) || (opc==0x0A) 
			|| (opc==0x04) || (opc==0x05) || (opc==0x07)){
			operateItypeIns(i);
		}
		else if((opc==0x02)|| (opc==0x03)){
			operateJtypeIns(i);
		}
		else if (opc==0x3F){
			//Halt the program;
		}
		else{
			printf("Opcode Not Found\n");
		}		
		pcCounter +=4;
	}	
}

int extractUnsignedIns(int end, int start, bool b[]){

	int length = end - start;
	int cnt = length-1;
	int value = 0;
	for(int i= end; i>start; i--){
		value += b[i] ? (1<<(length-cnt-1)) : 0;
		cnt--;
	}
	return value;
}

int extractSignedIns(int end, int start, bool b[]){

	int length = end - start;
	int cnt = length-1;
	int value = 0;
	for(int i= end; i>start+1; i--){
		value += b[i] ? (1<<(length-cnt-1)) : 0;
		cnt--;
	}
	value = value - (b[start] ? (1<<(length-1)) : 0); 
	return value;
}

int countSignedValue(bitset<32> b){
	int value = 0;
	for(int i=0; i<32; i++){
		value+= b[i]<<i;
	}
	return value;
}

int countUnsignedValue(bitset<32> b){
	int value = 0;
	for(int i=0; i<31; i++){
		value+= b[i]<<i;
	}
	value = -(b[32]<<31)+value;
	return value;
}

void  operateRtypeIns(int i){

// int cnt1 = 5; //function code counter
	// int cnt2 = 4; //shamt c code counter
	// int cnt3 = 4; //rd code counter
	// int cnt4 = 4; //rt code counter
	// int cnt5 = 4; //rs code counter
	int functc = extractUnsignedIns((i<<5)+31, (i<<5)+25, instruction);
	int shamtc = extractUnsignedIns((i<<5)+25, (i<<5)+20, instruction);
	int rdc = extractUnsignedIns((i<<5)+20, (i<<5)+15, instruction);
	int rtc = extractUnsignedIns((i<<5)+15, (i<<5)+10, instruction);
	int rsc = extractUnsignedIns((i<<5)+5, (i<<5)+5, instruction);
	cout << "the first r-type ins's rs is" << rsc;
	// //record function code
	// for(int j=(i<<5)+31; j>(i<<5)+25; j--){
	// 	functCode[cnt1] = instruction[j];
		
	// 	functc += functCode[cnt1] ? (1<<(functcodeLength-cnt1-1)) : 0;
	// 	cnt1++;
	// }
	// //record shamt C
	// for(int j=(i<<5)+25; j>(i<<5)+20; j--){
	// 	shamtC[cnt2] = instruction[j];
	// 	shamtc += shamtC[cnt2] ? (1<<(shamtCLength-cnt2-1)) : 0;
	// 	cnt2++;
	// }
	// //record rd
	// for(int j=(i<<5)+20; j>(i<<5)+15; j--){
	// 	rd[cnt3] = instruction[j];		
	// 	rdc += rd[cnt3] ? (1<<(rdLength-cnt3-1)) : 0;
	// 	cnt3++;
	// }
	// //record rt
	// for(int j=(i<<5)+15; j>(i<<5)+10; j--){
	// 	rt[cnt4] = instruction[j];		
	// 	rtc += rt[cnt4] ? (1<<(rtLength-cnt4-1)) : 0;
	// 	cnt4++;
	// }
	// //record rs
	// for(int j=(i<<5)+10; j>(i<<5)+5; j--){
	// 	rsCode[cnt5] = instruction[j];		
	// 	rsc += rs[cnt5] ? (1<<(rsLength-cnt5-1)) : 0;
	// 	cnt5++;
	// }


	switch(functc){

		case 0x20://add 
		{	
			if((countSignedValue(registers[rtc])+countSignedValue(registers[rsc])) < (maxValue+1)){			
				registers[rdc]= countSignedValue(registers[rtc])+countSignedValue(registers[rsc]);
			}
			else{
				//TODO
				printf("overflow exception!");
			}
			
        }
        break;
        case 0x21://addu (rt could be unsigned)
        {
        	if((countUnsignedValue(registers[rtc])+countUnsignedValue(registers[rsc])) < (maxValue+1)){
				registers[rdc]= countUnsignedValue(registers[rtc])+countUnsignedValue(registers[rsc]);
       		}
       		else{
       			registers[rdc]= countUnsignedValue(registers[rtc])+countUnsignedValue(registers[rsc])-(1<<32);
       		}        		
        }
        break;
        case 0x22://sub
        {
        	registers[rdc]= countSignedValue(registers[rtc])+ countSignedValue(registers[rsc]);
        }
        break;
        case 0x24://and
        {
        	registers[rdc]= registers[rtc] & registers[rsc];
        }
        break;
        case 0x25://or
        {
        	registers[rdc]= registers[rtc]|registers[rsc];
        }
        break;
        case 0x26://xor
        {
        	registers[rdc]= registers[rtc]^registers[rsc];
        }
        break;
        case 0x27://nor
        {
        	registers[rdc]= ~(registers[rtc]|registers[rsc]);
        }
        break;
        case 0x28://nand
        {
        	registers[rdc]= ~(registers[rtc]&registers[rsc]);
        }
        break;
        case 0x2A://slt
        {
        	if(countSignedValue(registers[rsc])<countSignedValue(registers[rtc])){
        		registers[rdc]=1;
        	} 
        	else{
        		registers[rdc]=0;
        	}
        }
        break;
        case 0x00://sll
        {
        	registers[rdc] = registers[rtc] << shamtc;
        }
        break;
        case 0x02://srl
        {
        	registers[rdc] = registers[rtc] >> shamtc;	
        }
        break;
        case 0x03://sra (with sign bit shifted in)
        {
        	if(registers[rtc][31]!=1){
        		registers[rdc] = registers[rtc] >> shamtc;	
        	}
        	else{
        		registers[rdc] = registers[rtc] >> shamtc;
        		for(int i=31; i>(31-shamtc); i--){
        			registers[rdc][i]=1;
        		}
        	}        	
        }
        break;
        case 0x08://jr
        {
        	pcCounter = countSignedValue(registers[rsc]);
        	//TODO can pcCOunter be negative?
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
        }
        break;
        case 0x10://mfhi
        {
        	HI = registers[rdc];
        }
        break;
        case 0x12://mflo
        {
        	LO = registers[rdc];
        }
        default:
        cout << "illegal instruction found at 0x" << address;
        //TODO halt;
        break;
    }

}
	

void operateItypeIns(int i){

	int unsignedImmc = extractUnsignedIns((i<<5)+31, (i<<5)+15, instruction);
	int signedImmc = extractSignedIns((i<<5)+31, (i<<5)+15, instruction);
	int rtc = extractUnsignedIns((i<<5)+15, (i<<5)+10, instruction);
	int rsc = extractUnsignedIns((i<<5)+10, (i<<5)+5, instruction);
	int opc = extractUnsignedIns((i<<5)+5, (i<<5)-1, instruction);

	for(int i=31; i>15; i++){
		immc[31-i] =instruction[(i<<5)+i];
	} 
	for(int i=16; i<32; i++){
		immc[i]=0;
	}

	cout << "the first I-type ins's rs is" << rsc;

      switch (opc)  
      {          
         case 0x08://I-type instructions, addi 
         {
         	if((countSignedValue(registers[rsc])+signedImmc) < (maxValue+1)){			
				registers[rtc]=countSignedValue(registers[rsc])+signedImmc;
			}
			else{
				//TODO
				printf("overflow exception!");
			}
         }
         break;
         case 0x09://I-type instructions, addiu 
         { 
         	if((countSignedValue(registers[rsc])+unsignedImmc) < (maxValue+1)){			
				registers[rtc]=countSignedValue(registers[rsc])+unsignedImmc;
			}
			else{
       			registers[rtc]= countSignedValue(registers[rtc])+unsignedImmc-(1<<32);
			}         	
         }
         break;
         //超級不確定範圍開始
         case 0x23://I-type instructions, lw
         {
         	
         }
         break;
         case 0x21://I-type instructions, lh
         {

         }
         break; 
         case 0x25://I-type instructions, lhu
         {

         }
         break;
         case 0x20://I-type instructions, lb 
         {

         }
         break;
         case 0x24://I-type instructions, lbu 
         {

         }
         break;
         case 0x2B://I-type instructions, sw
         {

         }
         break;
         case 0x29://I-type instructions, sh 
         {

         }
         break;
         case 0x28://I-type instructions, sb
         {

         }
         break;
         //超級不確定範圍結束
         case 0x0F://I-type instructions, lui 
         {
         	registers[rtc]= signedImmc << 16;
         }
         break;
         case 0x0C://I-type instructions, andi 
         {
         	registers[rtc]= registers[rsc] & immc;
         }
         break;
         case 0x0D://I-type instructions, ori 
         {
         	registers[rtc]= registers[rsc] | immc;
         }
         break;
         case 0x0E://I-type instructions, nori 
         {
         	registers[rtc]= ~(registers[rsc] | immc);
         }
         break;          
         case 0x0A://I-type instructions, slti 
         {
        	if(countSignedValue(registers[rsc])<signedImmc){
        		registers[rtc]=1;
        	} 
        	else{
        		registers[rtc]=0;
        	}
         }
         break;
         case 0x04://I-type instructions, beq 
         {
        	if(countSignedValue(registers[rsc])==countSignedValue(registers[rtc])){
        		pcCounter = pcCounter + 4 + 4*signedImmc;
        	} 
         }
         break;
         case 0x05://I-type instructions, bne
         {
        	if(countSignedValue(registers[rsc])!=countSignedValue(registers[rtc])){
        		pcCounter = pcCounter + 4 + 4*signedImmc;
        	} 
         }
         break;
         case 0x07://I-type instructions, bgtz
         {
        	if(countSignedValue(registers[rsc])>0){
        		pcCounter = pcCounter + 4 + 4*signedImmc;
        	} 
         }
         break;
         default:  
            cout << "illegal instruction found at 0x" + address;
            //TODO halt;
            break;
      }

}

void operateJtypeIns(int i){

}

void outputRegisterContent(){

}

void outputErrorMessage(){

}
