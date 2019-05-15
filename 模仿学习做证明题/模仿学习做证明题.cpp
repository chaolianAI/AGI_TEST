// 模仿学习做证明题.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<vector>
#include<set>
#include<string>
#include<iostream>
#include<sstream>
#include<time.h>
using namespace std;

typedef bool(*funcp)();//指向bool函数的指针 
char alphabet1 = ' ', alphabet2 = ' ';
bool LastExcuted = false;//是否执行了“等号右边除以二”这一操作
int LastRemainder = 0;//等号右边除以二后，得到的余数
bool finished = false;

void init()
{
	alphabet1 =alphabet2= ' ';
	LastExcuted = false;
	LastRemainder = 0;
	finished = false;
}

string int2str(int i)
{
	stringstream ss;
	string str;
	ss << i;
	ss >> str;
	return str;
}

class item //运算项
{
public:
	int coef=1;//系数
	bool var1=false;
	bool var2 = false;
	item(){}
	item(int coef)
	{
		this->coef = coef;
	}
	item(int coef, bool v1, bool v2)
	{
		this->coef = coef;
		var1 = v1; var2 = v2;
	}
	string tostring()
	{
		string result;
		if (var1 || var2)
		{
			if (coef > 1) result += int2str(coef);
		}
		else
		{
			if(coef!=0) result += int2str(coef);
		}

		if (var1) result += alphabet1;
		if (var2) result += alphabet2;
		return result;
	}
	void display()
	{
		cout << this->tostring();
	}
	item div2()
	{
		item result(coef / 2, var1, var2);
		return result;
	}
};
class expression //表达式
{
public:
	vector<item> items; //表达式所包含的运算项，每一项之间用加号连接
	expression() {}
	expression(item it) { this->items.push_back(it); }
	expression(item it1, item it2)
	{
		this->items.push_back(it1);
		this->items.push_back(it2);
	}
	bool available() //是否有效
	{
		return (items.size()>0);//一个不包含运算项的表达式是无效的
	}
	void add(item it) //添加运算项
	{
		this->items.push_back(it);
	}
	string tostring()
	{
		if (items.size() == 0) return "";
		else
		{
			string result;
			for (int i = 0; i < items.size(); i++)
			{
				result += items[i].tostring();//输出当前运算项内容
				if (i != items.size() - 1)//不是最后一项
				{
					if (items[i+1].coef || items[i+1].var1 || items[i+1].var2)//如果下一项不为零
						result += "+";//则在当前号后面添个加号
				}
			}
			return result;
		}
	}
	void display()
	{
		cout << this->tostring();
	}
	int mod2()
	{
		int remainder = 0;//余数
		if (items.size() == 0) return 0;
		for (int i = 0; i < items.size(); i++)
		{
			remainder += items[i].coef % 2;
		}
		return remainder;
	}
	expression div2()
	{
		if (items.size() == 0) return *this;
		else
		{
			expression result = *this;
			for (int i = 0; i < result.items.size(); i++)
			{
				result.items[i].coef /= 2;
			}
			return result;
		}
	}
};

item ItemMul(item it1, item it2) //乘法
{
	item result;
	result.coef = it1.coef*it2.coef;
	result.var1 = it1.var1 || it2.var1;
	result.var2 = it1.var2 || it2.var2;
	return result;
}

expression ExprMul(expression ex1, expression ex2) //乘法
{
	expression result;
	for (int i = 0; i < ex1.items.size(); i++)
		for (int j = 0; j < ex2.items.size(); j++)
			result.items.push_back(ItemMul(ex1.items[i], ex2.items[j]));
	return result;
}

class formula //
{
public:
	bool odd[3] = { false,false,false };
	expression exprs[3];
	formula(){}
	formula(bool odd0, bool odd1, bool odd2)
	{
		odd[0] = odd0; odd[1] = odd1; odd[2] = odd2;
	}
	bool equals(formula f)
	{
		return f.odd[0] == this->odd[0] && f.odd[1] == this->odd[1] && f.odd[2] == this->odd[2];
	}
	void display()
	{
		cout << (odd[0] ? "奇数" : "偶数") << "*" << (odd[1] ? "奇数" : "偶数") <<
			"=" << (odd[2] ? "奇数" : "偶数");
	}
};

formula CurrentFormula;
item _2m(2,true,false);
item _2n(2, false, true);
item _1(1, false, false);
expression _EVEN0(_2m);
expression _EVEN1(_2n);
expression _ODD0(_2m,_1);
expression _ODD1(_2n, _1);

//题设条件判断
bool ProblemOdd0()//等号左边第一个变量是奇数
{
	return CurrentFormula.odd[0];
}
bool ProblemOdd1()//等号左边第二个变量是奇数
{
	return CurrentFormula.odd[1];
}
bool ProblemOdd2()
{
	return CurrentFormula.odd[2];
}
bool ProblemEven0()//等号左边第一个变量是偶数
{
	return !CurrentFormula.odd[0];
}
bool ProblemEven1()//等号左边第二个变量是偶数
{
	return !CurrentFormula.odd[1];
}
bool ProblemEven2()
{
	return !CurrentFormula.odd[2];
}

funcp ProblemOdd[3] = { ProblemOdd0,ProblemOdd1,ProblemOdd2 };
funcp ProblemEven[3] = { ProblemEven0,ProblemEven1,ProblemEven2 };
//运行状态条件判断
bool ResultRightOdd() //公式等号右边（运算结果）为奇数
{
	if (!CurrentFormula.exprs[2].available())
	{
		//cout << "假：公式等号右边（运算结果）为奇数\n";
		return false;
	}
	else
	{
		//if (CurrentFormula.exprs[2].mod2() == 1&&LastExcuted) cout << "真：";
		//else cout << "假：";
		//cout << "公式等号右边（运算结果）为奇数\n";
		return CurrentFormula.exprs[2].mod2() == 1 && LastExcuted;
	}
}
bool Remainder0()
{
	//cout << ((LastExcuted && (LastRemainder == 0)) ? "真" : "假") << "：余数为零\n";
	return LastExcuted&&(LastRemainder == 0);
}
bool Remainder1()
{
	//cout << ((LastExcuted && (LastRemainder == 1)) ? "真" : "假") << "：余数为一\n";
	return LastExcuted&&(LastRemainder == 1);
}

//分类执行函数
bool Odd0Represent()
{
	if (CurrentFormula.odd[0])
	{
		CurrentFormula.exprs[0] = _ODD0;
		cout << "左边第一项表示为" << CurrentFormula.exprs[0].tostring() << endl;
		return true;
	}
	else
	{
		cout << "错误：左边第一项不是奇数！\n";
		return false;
	}
}
bool Odd1Represent()
{
	if (CurrentFormula.odd[1])
	{
		CurrentFormula.exprs[1] = _ODD1;
		cout << "左边第二项表示为" << CurrentFormula.exprs[1].tostring() << endl;
		return true;
	}
	else
	{
		cout << "错误：左边第二项不是奇数！\n";
		return false;
	}
}
bool Even0Represent()
{
	if (!CurrentFormula.odd[0])
	{
		CurrentFormula.exprs[0] = _EVEN0;
		cout << "左边第一项表示为" << CurrentFormula.exprs[0].tostring() << endl;
		return true;
	}
	else
	{
		cout << "错误：左边第一项不是偶数！\n";
		return false;
	}
}
bool Even1Represent()
{
	if (!CurrentFormula.odd[1])
	{
		CurrentFormula.exprs[1] = _EVEN1;
		cout << "左边第二项表示为" << CurrentFormula.exprs[1].tostring() << endl;
		return true;
	}
	else
	{
		cout << "错误：左边第二项不是偶数！\n";
		return false;
	}
}

//通用执行函数（用户输入一段文字就执行，无需判断）
bool FormulaMultiply()
{
	if (CurrentFormula.exprs[0].available()&& CurrentFormula.exprs[1].available())
	{
		CurrentFormula.exprs[2] = ExprMul(CurrentFormula.exprs[0], CurrentFormula.exprs[1]);
		cout << "经计算，等号右边等于" << CurrentFormula.exprs[2].tostring() << endl;
		return true;
	}
	else
	{
		if (!CurrentFormula.exprs[0].available())
			cout << "你还没有用代数式表示左边第一项\n";
		if (!CurrentFormula.exprs[1].available())
			cout << "你还没有用代数式表示左边第二项\n";
		return false;
	}
}
bool Mod2()
{
	if (CurrentFormula.exprs[2].available())
	{
		int remainder = CurrentFormula.exprs[2].mod2();
		expression quotient = CurrentFormula.exprs[2].div2();//商
		cout << "等号右边除以2，商" << quotient.tostring() << "，余数" << remainder << endl;
		LastExcuted = true;
		LastRemainder = remainder;
		return true;
	}
	else
	{
		cout << "等号右边没有被表示成代数式！\n";
		return false;
	}
}
bool Remainder1Odd()//余1则为奇数
{
	if (LastExcuted)
	{
		if (LastRemainder == 1)
		{
			cout << "所以等号右边为奇数。\n";
			cout << "证明完毕\n";
			finished = true;
			return true;
		}
		else
		{
			cout << "错误：余数不是1。\n";
			return false;
		}
	}
	else
	{
		cout << "还没有进行过除法运算！\n";
		return false;
	}
}
bool Remainder0Even()//余1则为奇数
{
	if (LastExcuted)
	{
		if (LastRemainder == 0)
		{
			cout << "所以等号右边为偶数。\n";
			cout << "证明完毕\n";
			finished = true;
			return true;
		}
		else
		{
			cout << "错误：余数不是0。\n";
			return false;
		}
	}
	else
	{
		cout << "还没有进行过除法运算！\n";
		return false;
	}
}

//用于记录的变量，以下两个数组是下标平行关系
vector<funcp> commands;//用户输入的命令
vector<vector<funcp>> conditions;//每个指令的执行都是有一定前提条件的（前提条件不止一个）

vector<funcp> GetTrueConditions(int VarIndex) //返回结果为true的条件函数，参数：当前正在操作的变量的下标
{
	vector<funcp> returning;
	if (VarIndex >= 0 && VarIndex <= 2)
	{
		if (ProblemOdd[VarIndex]()) returning.push_back(ProblemOdd[VarIndex]);
		if (ProblemEven[VarIndex]()) returning.push_back(ProblemEven[VarIndex]);
	}
	if (ResultRightOdd()) returning.push_back(&ResultRightOdd);
	if (Remainder0()) returning.push_back(&Remainder0);
	if (Remainder1()) returning.push_back(&Remainder1);
	return returning;
}

bool ConditionsAllTrue(int index) //判断：所有必须的条件都符合（当前条件==原先条件）
{
	if (conditions[index].size() == 0)
		return true;
	else
	{
		for (int i = 0; i < conditions[index].size(); i++)
		{
			if (conditions[index][i]() == false)
			{
				//cout << conditions[index][i] << "不为真\n";
				return false;
			}
		}
	}
	return true;
}

int GetCommandIndex(funcp cmd) //获取命令下标
{
	if (commands.size() == 0) return -1;
	else
	{
		for (int i = 0; i < commands.size(); i++)
		{
			if (commands[i] == cmd) return i;
		}
	}
	return -1;
}

//记录命令
bool RecordCommand(int VarIndex,funcp cmd)
{
	bool exist = false;
	if (commands.size() > 0)
	{
		for (int i = 0; i < commands.size(); i++)
		{
			if (commands[i] == cmd)
			{
				exist = true; return false;
			}
		}
	}
	if (!exist)
	{
		commands.push_back(cmd);
		conditions.push_back(GetTrueConditions(VarIndex));
		return true;
	}
}

bool Var0Represent()//让用户输入一个代数式来表示左边第一项（奇偶性不确定）
{
	string input;
	while (true)
	{
		cout << "输入一个代数式来表示左边第一项（格式如：2X 或 2X+1）：";
		cin >> input;
		if (input.length() != 2 && input.length() != 4)
		{
			cout << "格式错误！\n";
		}
		else
		{
			alphabet1 = input[1];
			if (input.length() == 2) //偶数
			{
				if (Even0Represent() == false)//此时程序会显示错误消息：左边第一项不是偶数
					continue;//让用户回去再输入
				else
				{
					//记录之
					RecordCommand(0, &Even0Represent);
				}
			}
			else //奇数
			{
				if (Odd0Represent() == false)
					continue;
				else
				{
					//记录之
					RecordCommand(0, &Odd0Represent);
				}
			}
			break;
		}
	}
	return true;
}
bool Var1Represent()//让用户输入一个代数式来表示左边第二项（奇偶性不确定）
{
	string input;
	while (true)
	{
		cout << "输入一个代数式来表示左边第二项（格式如：2X 或 2X+1）：";
		cin >> input;
		if (input.length() != 2 && input.length() != 4)
		{
			cout << "格式错误！\n";
		}
		else
		{
			alphabet2 = input[1];
			if (input.length() == 2)
			{
				if (Even1Represent() == false) continue;
				else
				{
					//记录之
					RecordCommand(1, &Even1Represent);
				}
			}
			else
			{
				if (Odd1Represent() == false) continue;
				else
				{
					RecordCommand(1, &Odd1Represent);
				}
			}
			break;
		}
	}
	return true;
}

//命令解释，在执行命令前先记录
bool interpret(string command)
{
	funcp cmd;//要执行的命令
	bool cmdSucceed = false;//命令是否执行成功
	int VarIndex = -1;
	if (command == "用代数式表示左边第一项") cmd=&Var0Represent;
	else if (command == "用代数式表示左边第二项") cmd = &Var1Represent;
	else if (command == "相乘") cmd = &FormulaMultiply;
	else if (command == "等号右边除以二") cmd = &Mod2;
	else if (command == "余一则为奇数") cmd = &Remainder1Odd;
	else if (command == "余零则为偶数") cmd = &Remainder0Even;
	else cmd = 0;


	if (cmd)//如果指向bool型函数的指针不为零
	{
		bool HasRecorded = false;
		if(GetCommandIndex(cmd)==-1 && //如果这个函数没有被记录过
			cmd != &Var0Represent&&cmd != Var1Represent)
			HasRecorded=RecordCommand(-1, cmd);//先记录

		cmdSucceed = cmd();//再执行函数

		if (cmdSucceed==false&& //如果执行失败
			HasRecorded)//并且刚才记录了
		{
			commands.pop_back();//就把刚才记录过的命令“吐”出来
			conditions.pop_back();
		}
	}
	return cmdSucceed;
}


int main()
{
	formula formula1(true, true, true);//公式1：奇数X奇数=奇数
	formula formula2(false, false, false);//公式2：偶数X偶数=偶数
	formula formula3(true, false, false);//公式3：奇数X偶数=偶数，要求计算机自己证明
	CurrentFormula = formula1;
	
start_prove:
	system("cls");
	cout << "现在开始证明公式：";
	CurrentFormula.display(); cout << "\n";
	while (finished==false)
	{
		string input;
		cin >> input;
		interpret(input);
	}
	if (CurrentFormula.equals(formula1))
	{
		CurrentFormula = formula2;
		init();
		system("pause");
		goto start_prove;
	}

	system("pause"); init(); system("cls");
	//自动证明公式3
	char RandomAlphabet1=alphabet1 = rand() % 24 + 'a';
	char RandomAlphabet2 = alphabet2 = alphabet1 + 1;
	CurrentFormula = formula3;
	cout << "以下证明过程包含一些不必要的操作，稍后程序会对过程进行简化\n";
	cout << "现在开始证明公式：";
	CurrentFormula.display(); cout << "\n";
	set<int> AlreadyExcuted;//已经执行过的命令所对应的下标
	vector<int> procedure;//证明过程
	while (!finished)
	{
		for (int i = 0; i < commands.size(); i++)
		{
			set<int>::iterator it = AlreadyExcuted.find(i);//防止重复执行
			if (ConditionsAllTrue(i)&&it==AlreadyExcuted.end())
			{
				if (commands[i]()) //如果执行成功
				{
					AlreadyExcuted.insert(i);//记录下标，避免重复执行
					procedure.push_back(i);
				}
			}
		}
		//system("pause");
	}
	system("pause");
	system("cls"); cout << "现在是简化版本：\n";
	init();
	alphabet1 = RandomAlphabet1;
	alphabet2 = RandomAlphabet2;
	for (int i = 0; i < procedure.size(); i++)
		commands[procedure[i]]();
	system("pause");
	return 0;
}

