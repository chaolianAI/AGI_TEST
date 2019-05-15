// 控制信号.cpp : 定义控制台应用程序的入口点。
//
//类型有：寄存器、内存变量、立即数、参数
//寄存器肯定要用指针，内存变量也要用指针（用list链表来模拟内存），两者统一为void*
//立即数直接用一个Object打包就可以了
//两个全局链表，一个只能存储short整数，另一个存储Object命名对象，该对象指向一个short整数

#include "stdafx.h"
#include<vector>
#include<string>
#include<iostream>
#include"strsplit.h"
#include"getIndex.h"
#include<list>

using namespace std;

#define SHORTARRAY vector<short>
#define LEFT_IS_MEMORY true
#define LEFT_IS_REG false

class Solution
{
public:
	SHORTARRAY steps;
	bool equals(Solution other)
	{
		if (this->steps.size() != other.steps.size()) return false;
		for (int i = 0; i < steps.size(); i++)
			if (this->steps[i] != other.steps[i]) return false;
		return true;
	}
	string toString();
	bool execute();
};

vector<short> memory;//内存，只存放整数，不存放名称
vector<short> enumResults;

class Register
{
public:
	string name="NULL";
	short value=0;
	Register(){}
	Register(string name) { this->name = name; }
	void toNull() { name = "NULL"; value = 0; }
	bool isNull() { return "NULL" == name; }
	string toString() 
	{
		string str_value;
		char temp[10] = {};
		_itoa(value, temp, 10);
		str_value = temp;
		return name + "=" + str_value;
	}
};

Register regPtr;

#define MAR_CODE 0
#define MDR_CODE 1
#define ALU_CODE 2
#define Y_CODE 3
#define Z_CODE 4
#define R1_CODE 5
#define R2_CODE 6
#define IR1_CODE 7
#define IR2_CODE 8
#define REGPTR_CODE 9





void addEnum()
{
	short lastVerbCode = enumResults[enumResults.size() - 1] / 100;
	short min = 100 * (lastVerbCode + 1);
	short max = min + 99;
	for (short i = min; i <= max; i++)
	{
		short target = i % 100 / 10;//十位：目标寄存器编号
		short source = i % 10;//个位：源寄存器编号
		if (target == source) continue;//目标寄存器不可以和源寄存器一致
		if (target == IR1_CODE || target==IR2_CODE) continue;//不能改动IR1、IR2立即数寄存器
		
		enumResults.push_back(i);//添加到枚举数组中
	}
}

Register MAR=*new Register("MAR");
Register MDR = *new Register("MDR");
Register ALU = *new Register("ALU");
Register Y = *new Register("Y");
Register Z = *new Register("Z");
Register R1 = *new Register("R1");
Register R2 = *new Register("R2");
Register IR1= *new Register("IR1");
Register IR2 = *new Register("IR2");
Register* registers[10] = { &MAR,&MDR,&ALU,&Y,&Z,&R1,&R2,&IR1,&IR2,&regPtr};
int getIndexOfRegister(string name)
{
	for (int i = 0; i<REGPTR_CODE; i++)
		if (registers[i]->name == name)
			return i;
	return -1;
}
int getIndexOfRegister(Register* reg)
{
	for (int i = 0; i <REGPTR_CODE; i++)
		if (registers[i]==reg)
			return i;
	return -1;
}

void clear()
{
	regPtr.toNull();
	IR1.value = 0;
	IR2.value = 0;
}

void refresh_passive()
{
	if ("NULL" == regPtr.name)
		regPtr.value = 0;
	else
		regPtr = *registers[getIndexOfRegister(regPtr.name)];
}

void refresh_active()
{
	if (regPtr.name != "NULL")
		*registers[getIndexOfRegister(regPtr.name)] = regPtr;
}

vector<Register> getRegisterArray() //返回数组Register[]
{
	vector<Register> returning;
	for (int i = 0; i < 10; i++)
		returning.push_back(*registers[i]);
	return returning;
}

void setRegisters(vector<Register> regs)
{
	//cout << "调试：设置所有寄存器\n";
	for (int i = 0; i < 10; i++)
		*registers[i] = regs[i];
	//cout << "调试：设置完毕\n";
}

class Equation
{
public:
	int left_index;
	short right_value;
	bool leftIsMemory;
	bool judge()
	{
		if (leftIsMemory==false)
			return registers[left_index]->value == right_value;
		else
			return memory[left_index] == right_value;
	}
	Equation(){}
	Equation(int left_index, short right_value, bool leftIsMemory)
	{
		this->left_index = left_index;
		this->right_value = right_value;
		this->leftIsMemory = leftIsMemory;
	}
	void toBeTrue()
	{
		if (leftIsMemory==false)//是寄存器
		{
			registers[left_index]->value = right_value;
			if (left_index == REGPTR_CODE) refresh_active();
			else refresh_passive();
		}
		else
			memory[left_index] = right_value;
	}
};

bool RM(Register* a, Register* b)//读内存Read Memory
{
	if (memory.size() - 1 < MAR.value)//内存数组下标越界
		return false;
	else
	{
		MDR.value = memory[MAR.value];//将地址为MAR的内存单元的数据放到暂存器MDR中
		return true;
	}
}

bool WM(Register* a, Register* b) //写内存
{
	if (memory.size() - 1 < MAR.value) //下标越界
		return false;
	else
	{
		memory[MAR.value] = MDR.value;//将暂存器MDR中的数据写到地址为MAR的内存单元内
		return true;
	}
}

bool ALU_plus(Register* a, Register* b) //ALU加法控制信号
{
	Z.value = ALU.value + Y.value;
	return true;
}

bool ALU_minus(Register* a, Register* b) //ALU减法控制信号
{
	if (ALU.value >= Y.value)
	{
		Z.value = ALU.value - Y.value; //强制规定被减数>=减数
		return true;
	}
	else
		return false;
}

bool mov(Register* a, Register* b)
{
	if (a->isNull() || b->isNull()) return false;//寄存器指针如果是空的，就返回【失败】

	a->value = b->value;//令寄存器a的值等于寄存器b的值

	if (a == &regPtr) refresh_active();//指针主动修改寄存器的值
	return true;
}


#define FUNCTION_NOARG_LENGTH 4
#define FUNCTION_NOARG_LASTINDEX 3
#define FUNCTION_DOUBLEARG_FIRSTINDEX 4
#define BASICFUNCTION_LENGTH 5
#define BASICFUNCTION_LASTINDEX 4
#define USERFUCTION_FIRSTINDEX BASICFUNCTION_LENGTH

typedef bool(*FunctionPointer)(Register*,Register*);//函数指针
FunctionPointer basicFunctions[BASICFUNCTION_LENGTH] = { &RM,&WM,&ALU_minus,&ALU_plus,&mov };
string names_basicFunctions[BASICFUNCTION_LENGTH] = { "RM","WM","PLUS","MINUS","MOV"};

string getNameOfBasicFunction(FunctionPointer fp)
{
	for (int i = 0; i <= FUNCTION_NOARG_LASTINDEX; i++)
		if (fp == basicFunctions[i])
			return names_basicFunctions[i];
}

int getIndexOfBasicFunction(string name)
{
	for (int i = 0; i <= BASICFUNCTION_LASTINDEX; i++)
		if (names_basicFunctions[i] == name)
			return i;
	return -1;
}

bool executeCode_basic(short code)
{
	if ((code/100) > BASICFUNCTION_LASTINDEX)
		cout << "调试：错误！这不是一个基本函数，code="<<code<<endl;
	short verb = code / 100;
	short target = code % 100 / 10;
	short source = code % 10;
	return basicFunctions[verb](registers[target], registers[source]);
}

class Function
{
public:
	string name;
	vector<short> sentences;
	bool targetIsMemory = false;
	Function() { }
	Function(string name) { this->name = name; }
	bool execute(Register* target, Register* source);
};
vector<Function> userFunctions;

bool Function::execute(Register* target, Register* source)
{
	if (target->isNull() || source->isNull()) return false;
	vector<short> bak_memory = ::memory;//备份内存
	vector<Register> bak_regs = ::getRegisterArray();//备份寄存器
	Register bak_regPtr = *registers[REGPTR_CODE];//备份寄存器指针

	if (targetIsMemory) //操作目标是内存单元
	{
		regPtr.toNull();//寄存器指针清空
		IR1.value = target->value;//左边当作立即数处理
	}
	else
	{
		regPtr = *target;//指针指向目标寄存器
		IR1.value = 0;//不需要立即数，就置零
	}

	IR2.value = source->value;//不管函数是否面向内存，右边永远当作立即数处理

	bool whole_succeed = true;//整个函数（所有语句）是否执行成功
	for (int i = 0; i < sentences.size(); i++)
	{
		bool current_succeed = true;//当前语句是否执行成功
		if (sentences[i] / 100 <= BASICFUNCTION_LASTINDEX)
			current_succeed = executeCode_basic(sentences[i]);
		else
		{
			Function f = userFunctions[sentences[i] / 100 - USERFUCTION_FIRSTINDEX];
			Register* _target = registers[sentences[i] % 100 / 10];
			Register* _source = registers[sentences[i] % 10];
			current_succeed = f.execute(_target, _source);
			if (_target == &regPtr) {
				/*
				cout << "目标是寄存器指针，来源是" << _source->name << "。";
				cout << "刷新前REGPTR=" << regPtr.name << "=" << regPtr.value
					<<"，指向的寄存器的值是"<<
					registers[getIndexOfRegister(regPtr.name)]->value<< endl;
				*/
				refresh_active();//主动刷新
				/*
				cout << "刷新前REGPTR=" << regPtr.name << "=" << regPtr.value
					<< "，指向的寄存器的值是" <<
					registers[getIndexOfRegister(regPtr.name)]->value << endl;
				*/
			}
		}
		if (current_succeed == false) {
			whole_succeed = false;
			break;//后面可以无视了
		}
	}

	//恢复原样
	if (targetIsMemory) //操作对象是内存
	{
		::setRegisters(bak_regs);//寄存器全部恢复原值，指针也是一样
	}
	else //操作对象是寄存器
	{
		::memory = bak_memory;//恢复内存
		for (int i = 0; i <= REGPTR_CODE-1; i++)
		{
			if (registers[i]->name == target->name) continue;
			else *registers[i] = bak_regs[i];
		}
		
		if (target == &regPtr) //操作对象正好是寄存器指针
			refresh_active();//主动刷新
		else
			refresh_passive();//被动

		//注意了，这个地方很危险！！！
		regPtr.name = bak_regPtr.name;//寄存器指针单独处理
		refresh_passive();

	}
	return whole_succeed;
}

vector<short> convertIntoBitArray(long longNumber,int radix,int size)
{
	//将longNumber转化为长度为size的radix进制数，长度不够则高位补零
	vector<short> result;
	long q = longNumber;
	long r = 0;
	while (q > 0)
	{
		r = q % radix;
		q = q / radix;
		result.push_back(r);
	}

	if (result.size() < size)
		for (int i = 1; i <= size - result.size(); i++)
			result.push_back(0); //高位补零

	vector<short> reversed;
	for (int a = (result.size() - 1); a >= 0; a--) //倒过来
	{
		if (a < 0) break;
		reversed.push_back(result[a]);
	}
	return reversed;
}

bool executeCode(short code)
{
	short verb = code / 100;//函数（动词）编号
	short target = code % 100 / 10; //目标寄存器编号
	short source = code % 10; //源寄存器编号
	if (verb <= BASICFUNCTION_LASTINDEX) return executeCode_basic(code);
	else 
	{
		bool succeed=
			userFunctions[verb - USERFUCTION_FIRSTINDEX].execute(registers[target], registers[source]);
		
		return succeed;
	}
}

bool Solution::execute()
{
	for (int i = 0; i < this->steps.size(); i++)
	{
		bool succeed=executeCode(this->steps[i]);
		if (succeed == false) return false;
	}
	return true;
}

bool executeByIndex(short index) //执行“枚举结果”数组中下标为index的命令
{
	return executeCode(enumResults[index]);
}

bool executeByIndexes(vector<short> indexes) //执行多条语句
{
	for (int i = 0; i < indexes.size(); i++)
	{
		bool succeed = executeByIndex(indexes[i]);
		if (succeed == false) return false;
	}
	return true;
}

bool executeLongNumber(long longNumber, int radix,int size)
{
	vector<short> bitArray = convertIntoBitArray(longNumber,radix,size);
	//cout << "bitArray的长度" << bitArray.size() << endl;
	/*调试输出
	for (int i = 0; i < bitArray.size(); i++)
		cout << bitArray[i] << " , ";
	cout << endl;
	*/
	//return true;
	return executeByIndexes(bitArray);
}

Solution createSolutionByLongNumber(long longNumber, int radix,int size)
{
	vector<short> bitArray = convertIntoBitArray(longNumber, radix,size);
	Solution sol = *new Solution();
	for (int i = 0; i < bitArray.size(); i++)
		sol.steps.push_back(enumResults[bitArray[i]]);
	
	return sol;
}

int getIndexOfUserFunction(string name)
{
	for (int i = 0; i <userFunctions.size(); i++)
		if (userFunctions[i].name == name)
			return i;
	return -1;
}

string getNameOfFunction(short code)
{
	if (code <= 4)
		return names_basicFunctions[code];
	else
		return userFunctions[code - 5].name;
}

class Sentence
{
public:
	short verb;
	short target=0;
	short source=0;
	Sentence(){}
	Sentence(short code)
	{
		verb = code / 100;
		target = code % 100 / 10;
		source = code % 10;
	}
	Sentence(short verbCode, short targetCode, short sourceCode)
	{
		verb = verbCode;
		target = targetCode;
		source = sourceCode;
	}
	string toString()
	{
		if (verb <= FUNCTION_NOARG_LASTINDEX) return ::names_basicFunctions[verb];
		else {
			string leftName;
			if (target == REGPTR_CODE) leftName = "REGPTR";
			else leftName = registers[target]->name;
			string rightName = registers[source]->name;
			return getNameOfFunction(verb) + "|" + leftName + "|" + rightName;
		}
	}
	string toString_value()
	{
		if (verb <= FUNCTION_NOARG_LASTINDEX) return ::names_basicFunctions[verb];
		else {
			string leftName="";
			char temp[10] = {};
			if (target == IR1_CODE) {
				_itoa(IR1.value, temp, 10);
				leftName = temp;
			}
			else if (target == IR2_CODE) {
				_itoa(IR2.value, temp, 10);
				leftName = temp;
			}
			else {
				/*
				if (target == REGPTR_CODE)
					cout << "调试提示，左边是" << target << endl;
				if (registers[target]->isNull())
					cout << "调试提示：左边寄存器指针是空的" << endl;
				*/
				leftName = registers[target]->name;
			}

			string rightName="";
			char temp2[10] = {};
			if (source == IR1_CODE) {
				_itoa(IR1.value, temp, 10);
				rightName = temp;
			}
			else if (source == IR2_CODE) {
				_itoa(IR2.value, temp, 10);
				rightName = temp;
			}
			else rightName = registers[source]->name;

			return getNameOfFunction(verb) + "|" + leftName + "|" + rightName;
		}

	}
	bool execute()
	{
		if (verb <= 4)
		{
			bool succeed=basicFunctions[verb](registers[target], registers[source]);
			if (target == REGPTR_CODE) refresh_active();//主动改变
			else refresh_passive();//指针的值被动变更
		}
		else
		{
			//return false;
			return userFunctions[verb - 5].execute(registers[target], registers[source]);
		}
	}
};

string Solution::toString()
{
	string str = "";
	for (int i = 0; i < this->steps.size(); i++)
	{
		Sentence sentence = *new Sentence(steps[i]);
		string str_sentence = sentence.toString();
		str += str_sentence + "\n";
	}
	return str;
}

Sentence createSentence(string str)
{
	vector<string> strArr=split(str, '|');

	if (strArr.size() == 3)
	{
		//cout << "切分后：" + strArr[0] + "  " + strArr[1] + "  " + strArr[2] + "\n";
		int verbCode = getIndexOfBasicFunction(strArr[0]);
		if (verbCode == -1) verbCode = getIndexOfUserFunction(strArr[0]) + USERFUCTION_FIRSTINDEX;
		int target = getIndexOfRegister(strArr[1]);
		int source = getIndexOfRegister(strArr[2]);
		//cout << "target=" << target << "  source=" << source << endl;

		if (target == -1) {
			IR1.value = atoi((char*)(strArr[1].c_str()));
			regPtr.toNull(); 
		}
		else {
			IR1.value = 0;
			regPtr = *registers[target];
		}

		if (source == -1) {
			//cout << "调试：IR2将被赋值为" << atoi((char*)(strArr[2].c_str())) << endl;
			IR2.value = atoi((char*)(strArr[2].c_str()));//数字
			//cout << "调试：if(source==1)分支执行完毕\n";
		}
		else {
			IR2.value = registers[source]->value;
		}

		short leftCode, rightCode;
		if (regPtr.isNull()) leftCode = IR1_CODE;
		else leftCode = getIndexOfRegister(regPtr.name);
		rightCode = IR2_CODE;

		//cout << "即将new一个Sentence，报告verbCode、leftCode、rightCode分别是" <<
		//	verbCode << " " << leftCode << " " << rightCode << endl;;
		Sentence sentence = *new Sentence(verbCode,leftCode,rightCode);
		return sentence;
	}
	else //if(strArr.size()==1)
	{
		short verbCode = getIndexOfBasicFunction(strArr[0]);
		Sentence sentence = *new Sentence(verbCode, 0, 0);
		return sentence;
	}
}

string getNameOfRegister(Register* reg)
{
	if (NULL==reg) return "NULL";
	else return reg->name;
}

void showAllRegisters()
{
	for (int i = 0; i < REGPTR_CODE; i++)
		cout << registers[i]->toString() << "，";
	cout << "REGPTR=" + regPtr.toString() << endl;
}

void showMemory()
{
	for (int i = 0; i < memory.size(); i++)
	{
		cout << "[" << i << "]=" << memory[i];
		if (i != memory.size() - 1) cout << "，";
	}
	cout << endl;
}

void modifyRegister()
{
	string name;
	cout << "请输入要修改的寄存器的名称：";
	cin >> name;
	cout << "将" + name << "赋值为：";
	short value;
	cin >> value;
	registers[getIndexOfRegister(name)]->value = value;
	showAllRegisters();
}

void modifyMemory()
{
	int address;
	cout << "请输入内存地址：";
	cin >> address;
	cout << "将地址为" << address << "的内存单元的数据修改为：";
	short value;
	cin >> value;
	memory[address] = value;
	showMemory();
}

void insertDataIntoMemory()
{
	cout << "将在地址为" << memory.size() << "的内存单元插入数据：";
	short value;
	cin >> value;
	memory.push_back(value);
	showMemory();
}

vector<Solution> enumAndTry(int len,int percent,Equation goal) //枚举&试错
{
	long min = pow(enumResults.size(), len - 1);
	long max = pow(enumResults.size(), len) - 1;
	long total = max - min+1;
	long total_actual;
	if (percent == 100) total_actual = total;
	else total_actual= ((double)percent) / 100 * ((double)total);
	long max_actual = min + total_actual - 1;

	SHORTARRAY bak_mem = ::memory;
	vector<Register> bak_regs = ::getRegisterArray();
	cout << "初始状态:\n";
	::showAllRegisters(); ::showMemory();
	//string what; cin >> what;

	cout << "min=" << min <<"，";
	cout << "max_actual=" << max_actual <<"，";
	cout << "总共要尝试" << max_actual - min << "种方案\n";

	vector<Solution> solutions;
	for (long i = min; i <= max_actual; i++)
	{
		Solution currentSolution;
		executeLongNumber(i, enumResults.size(), len);

		//cout << "调试：现在MDR=" << MDR.value << endl;
		if (goal.judge()==true) //符合要求
		{
			vector<short> bitArray = convertIntoBitArray(i, enumResults.size(), len);
			
			for (int j = 0; j < bitArray.size(); j++)
			{
				//cout << "下标" << bitArray[j] << "对应的数码";cout<<enumResults[bitArray[j]] << "  ";
				currentSolution.steps.push_back(enumResults[bitArray[j]]);
			}

			memory = bak_mem;
			setRegisters(bak_regs);
			solutions.push_back(currentSolution);
		}
		else
		{
			//寄存器和变量全部恢复原值
			memory = bak_mem;
			setRegisters(bak_regs);
		}
		
	}
	return solutions;
}

vector<Solution> eliminate(vector<Solution> oldOnes,Equation goal)
{
	cout << "开始淘汰原方案...提醒：初始状态如下所示：\n";
	showMemory(); showAllRegisters();
	vector<Solution> intersection;
	for (int i = 0; i < oldOnes.size(); i++)
	{
		SHORTARRAY bak_memory = ::memory;
		vector<Register> bak_regs = ::getRegisterArray();

		oldOnes[i].execute();
		if (goal.judge() == true) intersection.push_back(oldOnes[i]);

		memory = bak_memory;
		setRegisters(bak_regs);
	}
	return intersection;
}

void IO_selfProgram()
{
	bool targetIsMemory = false;
	cout << "操作的目标是内存（M）还是寄存器（R）？ ";
	string inp_target;cin >> inp_target;
	if (inp_target == "M") targetIsMemory = true;

	Equation goal;

	if (targetIsMemory)
	{
		cout << "请输入内存地址和目标值，以空格分隔：";
		int address; int value;
		cin >> address >> value;
		goal = *new Equation(address, value, LEFT_IS_MEMORY);
		regPtr.toNull();//不需要寄存器指针
		IR1.value = address; 
		//IR2.value = value;
	}
	else
	{
		cout << "请输入寄存器名和目标值，以空格分隔：";
		string nameOfReg; int value;
		cin >> nameOfReg >> value;
		goal = *new Equation(::getIndexOfRegister(nameOfReg), value, LEFT_IS_REG);
		regPtr = *registers[::getIndexOfRegister(nameOfReg)];
		IR1.value = 0;
		//IR2.value = value;
	}

	int numOfSteps;
	cout << "请设置步骤数："; cin >> numOfSteps;

	vector<Solution> solutions=::enumAndTry(numOfSteps, 100, goal);
	if (solutions.size() == 0) cout << "没有找到方案\n";
	else
	{
		while (solutions.size()>0)
		{
			cout << "计算机经过试错，找到了这些方案：\n";
			for (int i = 0; i < solutions.size(); i++)
			{
				cout << "编号" << i << "：\n";
				cout << solutions[i].toString() << endl;
			}
			cout << "是否保存方案？（Y/N）:";
			string YorN; cin >> YorN;
			if (YorN == "Y")
			{
				cout << "请输入方案编号：";
				int indexOfSolution; cin >> indexOfSolution;
				Solution chosenSolution = solutions[indexOfSolution];
				cout << "请为该方案（函数）取个名字：";
				string fName; cin >> fName;
				Function f = *new Function(fName);
				f.sentences = chosenSolution.steps;
				f.targetIsMemory = targetIsMemory;
				userFunctions.push_back(f);
				::addEnum();
				return;
			}
			else
			{
				while (true)
				{
					cout << "为了进一步筛选方案，请您调整寄存器的值或修改内存变量。\n";
					cout << "修改内存请输入1，插入数据请输入2，修改寄存器请输入3，";
					cout << "查看内存请输入4，查看寄存器请输入5，完毕请输入6" << endl;
					int choice=-1;
					cin >> choice;
					if (choice == 1) modifyMemory();
					else if (choice == 2) insertDataIntoMemory();
					else if (choice == 3) modifyRegister();
					else if (choice == 4) showMemory();
					else if (choice == 5) showAllRegisters();
					else if (choice == 6) break;
				}

				//开始设定目标
				if (targetIsMemory)
				{
					cout << "请输入内存地址和目标值，以空格分隔：";
					int address; int value;
					cin >> address >> value;
					goal = *new Equation(address, value, LEFT_IS_MEMORY);
					regPtr.toNull();//不需要寄存器指针
					IR1.value = address; 
					//IR2.value = value;
				}
				else
				{
					cout << "请输入寄存器名和目标值，以空格分隔：";
					string nameOfReg; int value;
					cin >> nameOfReg >> value;
					goal = *new Equation(::getIndexOfRegister(nameOfReg), value, LEFT_IS_REG);
					regPtr = *registers[::getIndexOfRegister(nameOfReg)];
					//IR2.value = value;
				}

				//完毕开始筛选
				solutions = ::eliminate(solutions, goal);
				//转向循环的开头处，让用户自己选择方案
				if (solutions.size() == 0) cout << "没有合适的方案\n";
			}
		}
	}
}

void IO_interpreter()
{
	cout << "您已进入解释器模式，输入EXIT可退出。\n";
	system("color 1e");
	string input;
	while (input != "EXIT")
	{
		cin >> input;
		if (input == "修改内存") modifyMemory();
		else if (input == "修改寄存器") modifyRegister();
		else if (input == "EXIT") {
			system("color 0f");
			cout << "您已退出解释器模式\n";
			return;
		}
		else
		{
			Sentence sentence = createSentence(input);
			sentence.execute();
			
			showMemory(); showAllRegisters();
		}
	}
}

void IO_main() //主要命令处理
{
	while (true)
	{
		string cmdInput;
		cin >> cmdInput;
		if (cmdInput == "查看内存") showMemory();
		else if (cmdInput == "查看寄存器") showAllRegisters();
		else if (cmdInput == "修改内存") modifyMemory();
		else if (cmdInput == "插入数据") insertDataIntoMemory();
		else if (cmdInput == "修改寄存器") modifyRegister();
		else if (cmdInput == "解释器模式") IO_interpreter();
		else if (cmdInput == "自编程") IO_selfProgram();
		else if (cmdInput == "退出") break;
	}
}

int main()
{
	//functions.push_back(*new Function());
	memory.push_back(133);//[0]
	memory.push_back(2041);//[1]
	memory.push_back(2007);//[2]
	memory.push_back(1900);//[3]
	memory.push_back(3);//[4]
	enumResults.push_back(0);
	enumResults.push_back(100);
	enumResults.push_back(200);
	enumResults.push_back(300);
	::addEnum();
	
	IO_main();
	
	//int size = enumResults.size();
	/*
	int min = enumResults.size();
	int max = pow(enumResults.size(), 3) - 1;
	cout << "从" << min << "到" << max << endl;
	for (int i = min; i <= max; i++)
	{
		SHORTARRAY bitArray = convertIntoBitArray(i, enumResults.size(),3);
		for (int j = 0; j < bitArray.size(); j++)
			cout << bitArray[j] << "  ";
		cout << endl;
		cout << createSolutionByLongNumber(i, enumResults.size(),3).toString() << endl;
	}
	*/
	//IR2.value = 1;

	/*
	R1.value = 37; IR2.value = 1;
	regPtr = R1;

	vector<Solution> 间接寻址 = ::enumAndTry(3,19 ,*new Equation(R1_CODE,2041,LEFT_IS_REG));

	regPtr = R2;
	间接寻址 = ::eliminate(间接寻址, *new Equation(R2_CODE, 2041, LEFT_IS_REG));

	if (间接寻址.size() != 0)
	{
		cout << "总共有" << 间接寻址.size() << "个方案\n";
		for (int x = 0; x < 间接寻址.size(); x++)
		{
			cout << x << "\n" + 间接寻址[x].toString() << endl;
		}
	}
	else
		cout << "没有合适方案\n";

	Function f_间接寻址 = *new Function("传值_右引用");
	f_间接寻址.targetIsMemory = false;
	f_间接寻址.sentences = 间接寻址[0].steps;
	userFunctions.push_back(f_间接寻址);
	::addEnum();


	string anyStr; cin >> anyStr;

	R1.value = 0; regPtr = R2;
	IR2.value = 4;
	showMemory();
	showAllRegisters();
	vector<Solution> 二级间接寻址 = ::enumAndTry(2, 100, 
		*new Equation(R2_CODE, 1900, LEFT_IS_REG));
	if (二级间接寻址.size() == 0) cout << "没有可行方案\n";
	else
	{
		for (int i = 0; i < 二级间接寻址.size(); i++)
		{
			cout << i << ":\n";
			cout << 二级间接寻址[i].toString() << endl;
		}
	}

	cin >> anyStr;
	/*
	Sentence 间接寻址语句 = createSentence("传值_右引用|R1|2");
	间接寻址语句.execute();
	showAllRegisters(); showMemory();
	*/
	/*
	R1.value = 0; regPtr = R1;
	IR2.value = 4;
	showMemory();
	showAllRegisters();

	Solution 二级指针寻址 = *new Solution();
	二级指针寻址.steps.push_back(500 + MAR_CODE * 10 + IR2_CODE);
	二级指针寻址.steps.push_back(500 + REGPTR_CODE * 10 + MAR_CODE);
	
	Function f_二级指针 = *new Function("二级指针寻址");
	f_二级指针.targetIsMemory = false;
	f_二级指针.sentences = 二级指针寻址.steps;
	f_二级指针.execute(&R2, &IR2);

	

	cout << "执行完毕以后\n";
	showAllRegisters();

	//bool 第一句话是否成功=executeCode(二级指针寻址.steps[0]);
	//cout << "第一句话是否成功=" << 第一句话是否成功 << endl;
	//showAllRegisters();
	//IR2.value = MAR.value;
	//userFunctions[0].execute(registers[REGPTR_CODE], &MAR);
	//bool 第二句话是否成功 = executeCode(二级指针寻址.steps[1]);
	//cout << "第二句话是否成功=" << 第二句话是否成功 << endl;
	//showAllRegisters();

	//二级指针寻址.execute();
	//cout << "执行了\n" + 二级指针寻址.toString() << endl;
	//showMemory(); showAllRegisters();
	/*
	vector<Solution> 二级指针寻址 = ::enumAndTry(2, 100, 
		*new Equation(R1_CODE, 1900, LEFT_IS_REG));
	for (int i = 0; i < 二级指针寻址.size(); i++)
	{
		cout << "方案" << i << endl;
		cout << 二级指针寻址[i].toString() << endl;
	}
	*/

	/*
	cin >> anyStr;

	R1.value = 34;
	if (registers[REGPTR_CODE]->isNull())
		cout << "第一句话创建之前，左寄存器是空值\n";
	
	Sentence testSentence=createSentence("MOV|R1|23");
	cout <<
		testSentence.toString() +
		"  " + testSentence.toString_value() + " "
		+ IR1.toString() + " " + IR2.toString() + " " + R1.toString()
		<< "，REGPTR=" + regPtr.toString() << endl;

	testSentence.target = REGPTR_CODE;
	if (NULL==registers[testSentence.target])
		cout << "在这之前，左寄存器是空值\n";
	cout <<
		testSentence.toString() +
		"  " + testSentence.toString_value() +" "
		+IR1.toString()+" "+IR2.toString()+" "+R1.toString()
		<<"，REGPTR=" + regPtr.toString() << endl;

	string any; cin >> any;

	testSentence.execute();
	cout << "执行了下\n";
	cout <<
		testSentence.toString() +
		"  " + testSentence.toString_value() + " "
		+ IR1.toString() + " " + IR2.toString() + " " + R1.toString()
		<< "，REGPTR=" + regPtr.toString() << endl;

	cin >> any;

	cout << "等下我要改变regPtr1的名字" << endl;
	regPtr.name = "MDR";
	cout << "等下我要刷新了！\n";
	refresh_passive();//被动
	cout <<
		testSentence.toString() +
		"  " + testSentence.toString_value() +
		"，IR1=" << IR1.value << "，IR2=" << IR2.value <<
		"，REGPTR=" + regPtr.toString() +"，"
		<<registers[getIndexOfRegister(regPtr.name)]->toString()
		<< endl;
	cin >> any;
	testSentence.execute();
	cout << "修改后的语句，执行一遍:\n";
	cout <<
		testSentence.toString() +
		"  " + testSentence.toString_value() +
		"，IR1=" << IR1.value << "，IR2=" << IR2.value <<
		"，REGPTR=" + regPtr.toString() + "，"
		<< registers[getIndexOfRegister(regPtr.name)]->toString()
		<< endl;
	
	cout << "按任意键进入命令行...";
	cin >> any;

	clear();
	showAllRegisters();
	while (true)
	{
		string input;
		cin >> input;
		if (input == "EXIT") break;
		Sentence userSentence = createSentence(input);
		cout << "您刚才输入了：" + userSentence.toString() + "  " + userSentence.toString_value() + "\n";
		userSentence.execute();
		showAllRegisters();
		clear();
	}
	*/
	return 0;
}

//传值_左地址：5,R1
//IR1=5，IR2=34
//IR1→MAR  5
//IR2→MDR  34
//WM

//传值_右地址：R1（可换成任意寄存器）,7
//IR1=0,IR2=7  REGPTR=R1
//IR2→MAR
//WM
//MDR→REGPTR （指针主动修改原寄存器的值）