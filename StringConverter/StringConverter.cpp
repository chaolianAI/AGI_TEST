// StringConverter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include"reflect.h"
#include"strsplit.h"
#include<afx.h>
using namespace std;

template<typename T>
int getIndex(vector<T> arr, T obj) 
{
	if (arr.size() == 0) return -1;
	for (int i = 0; i < arr.size(); i++)
		if (obj == arr[i])
			return i;
	return -1;
}

template<typename T>
bool arrEquals(vector<T> &a, vector<T> &b)
{
	if (a.size() != b.size()) return false;
	for (int i = 0; i < a.size(); i++)
		if (a[i] != b[i]) return false;
	return true;
}

template<typename K, typename V>
vector<V> getValuesForKey(multimap<K, V> &_map, K key)
{
	vector<V> results;
	auto begin = _map.lower_bound(key);
	auto end = _map.upper_bound(key);
	for (auto it = begin; it != end; ++it)
		results.push_back(it->second);
	return results;
}

#define getCondition() second
#define getActionOrConsequence() first
#define getAction() first
#define getConsequene() first


typedef const char* CharArray;

vector<string> temp_string;
vector<CString> temp_CString;
vector<CharArray> temp_CharArray;
vector<int> temp_int;

void* argPointer = NULL;
#define SECONDARY &argPointer

vector<FunctionPointer> funcs;
multimap<FunctionPointer, FunctionPointer> relationships;
map<FunctionPointer, bool> funcTypes;
map<FunctionPointer, string> names;

FunctionPointer getFunction(string name)
{
	for (FunctionPointer f : funcs)
		if (names[f] == name)
			return f;
	return NULL;
}

typedef vector<FunctionPointer> ComplexFunction;
#define complexFunctionEquals arrEquals<FunctionPointer>

int count_complexFunctions = 0;
ComplexFunction* complexFunctions=new ComplexFunction[5];

#define declareUniversalComplexFunction(id) void* uni_complexFunction##id##(vector<void*> args){void* returning = NULL;\
                   for (FunctionPointer f : complexFunctions[##id##])returning = f(args);return returning; }

declareUniversalComplexFunction(0);
declareUniversalComplexFunction(1);
declareUniversalComplexFunction(2);
declareUniversalComplexFunction(3);
declareUniversalComplexFunction(4);

FunctionPointer uni_complexFunctions[5] = { &uni_complexFunction0, &uni_complexFunction1, &uni_complexFunction2, &uni_complexFunction3, &uni_complexFunction4 };

string toString_route(vector<FunctionPointer> route)
{
	string str = "";
	for (int i = 0; i < route.size(); i++)
	{
		str += names[route[i]]; 
		if (i != route.size() - 1) str += "——>";
	}
	return str;
}

vector<FunctionPointer> removePredicates(vector<FunctionPointer> original)
{
	vector<FunctionPointer> result;
	for (FunctionPointer f : original)
		if (funcTypes[f] == VERB)
			result.push_back(f);
	return result;
}

vector<vector<FunctionPointer>> getAllValidSequences(vector<FunctionPointer> route)
{
	vector<vector<FunctionPointer>> sequences;
	for (int i = 1; i < route.size(); i++)
	{
		sequences.push_back(*new vector<FunctionPointer>(route.begin(), route.begin() + i + 1));
	}
	return sequences;
}

vector<vector<FunctionPointer>> getAllValidSequences_backwards(vector<FunctionPointer> originalRoute)
{
	vector<FunctionPointer> route;
	for (int i = originalRoute.size() - 1; i >= 0; i--)
		route.push_back(originalRoute[i]);

	vector<vector<FunctionPointer>>  temp=getAllValidSequences(route);
	vector<vector<FunctionPointer>> sequences;
	for (vector<FunctionPointer> one : temp)
	{
		vector<FunctionPointer> reversed;
		for (int i = one.size() - 1; i >= 0; i--)
			reversed.push_back(one[i]);
		sequences.push_back(reversed);
	}
	return sequences;
}

vector<pair<FunctionPointer,ComplexFunction>> detectRelationships_conditionAndFunction(vector<FunctionPointer> route)
{
	vector<pair<FunctionPointer, ComplexFunction>> returning;
	for (int i = 0; i < route.size()-1; i++)
	{
		FunctionPointer f = route[i];
		if (funcTypes[f] == VERB) continue;//前项必须是谓词
		vector<FunctionPointer> rest = *new vector<FunctionPointer>(route.begin() + i + 1, route.end());
		vector<ComplexFunction> seqs=getAllValidSequences(rest);
		vector<ComplexFunction> sifted;//筛选后的
		for (ComplexFunction seq : seqs)
		{
			ComplexFunction seq_predicatesRemoved = removePredicates(seq);
			if (seq_predicatesRemoved.size() < 2) continue;//长度为0、1，没意义
			bool exist = false;
			for (ComplexFunction one_sifted : sifted)
			{
				if (arrEquals(one_sifted, seq_predicatesRemoved))
				{
					exist = true;
					break;
				}
			}
			if (!exist)
				sifted.push_back(seq_predicatesRemoved);
		}
		for (ComplexFunction one_sifted : sifted)
			returning.push_back(*new pair<FunctionPointer, ComplexFunction>(route[i], one_sifted));
	}
	return returning;
}

vector<pair<ComplexFunction,FunctionPointer>> detectRelationships_functionAndEffect(vector<FunctionPointer> route)
{
	vector<pair<ComplexFunction, FunctionPointer>> returning;
	for (int i = 2; i < route.size(); i++)
	{
		FunctionPointer f = route[i];
		if (funcTypes[f] == VERB) continue;//后项必须是谓词
		vector<FunctionPointer> rest = *new vector<FunctionPointer>(route.begin(), route.begin()+i);//注意区间左开右闭
		vector<ComplexFunction> seqs = getAllValidSequences_backwards(rest);
		vector<ComplexFunction> sifted;//筛选后的
		for (ComplexFunction seq : seqs)
		{
			ComplexFunction seq_predicatesRemoved = removePredicates(seq);
			if (seq_predicatesRemoved.size() < 2) continue;//长度为0、1，没意义
			bool exist = false;
			for (ComplexFunction one_sifted : sifted)
			{
				if (arrEquals(one_sifted, seq_predicatesRemoved))
				{
					exist = true;
					break;
				}
			}
			if (!exist)
				sifted.push_back(seq_predicatesRemoved);
		}
		for (ComplexFunction one_sifted : sifted)
			returning.push_back(*new pair<ComplexFunction, FunctionPointer>(one_sifted,route[i]));
	}
	return returning;
}

bool exist_complexFunction(ComplexFunction complexFunction)
{
	for (int i = 0; i < count_complexFunctions; i++)
		if (complexFunctionEquals(complexFunctions[i], complexFunction))
			return true;
	return false;
}

vector<ComplexFunction> extractComplexFunctions(vector<pair<FunctionPointer, ComplexFunction>> pairs_conditionAndFunction,
	vector<pair<ComplexFunction, FunctionPointer>> pairs_functionAndEffect)
{
	vector<ComplexFunction> results;
	for (auto it = pairs_conditionAndFunction.begin(); it != pairs_conditionAndFunction.end(); it++)
		results.push_back((*it).second);
	for (auto it = pairs_functionAndEffect.begin(); it != pairs_functionAndEffect.end(); it++)
		results.push_back((*it).first);
	return results;
}


void updateRelationships(vector<pair<FunctionPointer, ComplexFunction>> pairs_conditionAndFunction,
	vector<pair<ComplexFunction, FunctionPointer>> pairs_functionAndEffect)
{
	for (auto thisPair : pairs_conditionAndFunction)
	{
		for (int i = 0; i < count_complexFunctions; i++)
		{
			ComplexFunction storedFunction = complexFunctions[i];
			if (complexFunctionEquals(storedFunction, thisPair.second))
			{
				relationships.insert(*new pair<FunctionPointer, FunctionPointer>(uni_complexFunctions[i], thisPair.first));//复杂函数<——条件
				break;
			}
		}
	}

	for (auto thisPair : pairs_functionAndEffect)
	{
		for (int i = 0; i < count_complexFunctions; i++)
		{
			ComplexFunction storedFunction = complexFunctions[i];
			if (complexFunctionEquals(storedFunction, thisPair.first))
			{
				relationships.insert(*new pair<FunctionPointer, FunctionPointer>(thisPair.second,uni_complexFunctions[i]));//影响<——复杂函数
				break;
			}
		}
	}
}

void saveFunction(string name, ComplexFunction steps)
{
	complexFunctions[count_complexFunctions] = steps;
	funcs.push_back(uni_complexFunctions[count_complexFunctions]);
	names.insert(*new pair<FunctionPointer, string>(uni_complexFunctions[count_complexFunctions], name));
	funcTypes.insert(*new pair<FunctionPointer, bool>(uni_complexFunctions[count_complexFunctions], VERB));
	count_complexFunctions++;
}

void IO_saveFunction(ComplexFunction complexFunction)
{
	cout << "函数内容：" << toString_route(complexFunction) << endl;
	cout << "请为此函数取名：";
	string name;
	cin >> name;
	if (name != "XX")
		saveFunction(name, complexFunction);
}

void IO_saveFunctions(vector<ComplexFunction> complexFunctions)
{
	for (ComplexFunction func : complexFunctions)
	{
		if (exist_complexFunction(func) == false)
			IO_saveFunction(func);
	}
}

void* executeUni(FunctionPointer func, void* arg)
{
	vector<void*> args;
	args.push_back(arg);
	return func(args);
}

vector<FunctionPointer> getTruePredicates(void* arg)
{
	vector<FunctionPointer> predicates;
	for (FunctionPointer func : funcs)
	{
		if (funcTypes[func] == VERB) continue;
		if (executeUni(func, arg))
			predicates.push_back(func);
	}
	return predicates;
}

bool executeAndRecordRelationships(FunctionPointer verb, void *arg)
{
	vector<FunctionPointer> conditions = getTruePredicates(arg);
	if (executeUni(verb, arg))
	{
		if (funcTypes[verb] == VERB)
		{
			for (FunctionPointer condition : conditions)
				relationships.insert(*new pair<FunctionPointer, FunctionPointer>(verb, condition));

			vector<FunctionPointer> effects = getTruePredicates(arg);
			for (FunctionPointer effect : effects)
				if (getIndex<FunctionPointer>(conditions, effect) == -1)
					relationships.insert(*new pair<FunctionPointer, FunctionPointer>(effect, verb));
		}

		return true;
	}
	else
		return false;
}

int showMsgBox(CString content)
{
	return MessageBox(NULL, content, L"这是你要求系统显示的信息", 1);
}

bool isStd(void* ptr)
{
	if (ptr == SECONDARY) return isStd(*SECONDARY);
	if (temp_string.size() == 0) return false;
	else
	{
		void* head = &(temp_string[0]);
		void *tail = &(temp_string[temp_string.size() - 1]);
		return head <= ptr && tail >= ptr;
	}
}
createSimpleTypeVerb_singleArg(isStd, void*);

bool isCString(void* ptr)
{
	if (ptr == SECONDARY) return isCString(  *SECONDARY );
	if (temp_CString.size() == 0) return false;
	else
	{
		void* head = &(temp_CString[0]);
		void *tail = &(temp_CString[temp_CString.size()-1]);
		return head <= ptr && tail >= ptr;
	}
}
createSimpleTypeVerb_singleArg(isCString, void*);

bool isCharArray(void* ptr)
{
	if (ptr == SECONDARY) return isCharArray(*SECONDARY);
	if (temp_CharArray.size() == 0) return false;
	else
	{
		void* head = &(temp_CharArray[0]);
		void *tail = &(temp_CharArray[temp_CharArray.size() - 1]);
		return head <= ptr && tail >= ptr;
	}
}
createSimpleTypeVerb_singleArg(isCharArray, void*);

bool isInteger(void* ptr)
{
	if (ptr == SECONDARY) return isInteger(*SECONDARY);
	if (temp_int.size() == 0) return false;
	else
	{
		void* head = &(temp_int[0]);
		void *tail = &(temp_int[temp_int.size() - 1]);
		return head <= ptr && tail >= ptr;
	}
}
createSimpleTypeVerb_singleArg(isInteger, void*);

void* uni_showMsgBox(vector<void*> args)
{
	if (args[0] == SECONDARY)
	{
		vector<void*> temp;
		temp.push_back( *SECONDARY );
		return uni_showMsgBox(temp);
	}

	argPointer = args[0];
	if (isCString(args[0]))
	{
		showMsgBox(*(CString*)args[0]);
		return (void*)true;
	}
	else
		return (void*)false;
}

CharArray std2CharArray(string str)
{
	CharArray charArray = str.c_str();
	return charArray;
}

CString CharArray2CString(CharArray charArray)
{
	return *new CString(charArray);
}

CString int2CString(int integer)
{
	CString str;
	str.Format(_T("%d"), integer);
	return str;
}

void* uni_std2CharArray(vector<void*> args)
{
	if (isStd(args[0]) == false) return NULL;
	if (args[0] == SECONDARY)
	{
		args[0] = *SECONDARY;
		return uni_std2CharArray(args);
	}
	CharArray result = ((string*)args[0])->c_str();
	temp_CharArray.push_back(result);
	argPointer = &temp_CharArray[temp_CharArray.size() - 1];
	return argPointer;
}

void* uni_CharArray2CString(vector<void*> args)
{
	if (isCharArray(args[0]) == false) return NULL;
	if (args[0] == SECONDARY)
	{
		args[0] = *SECONDARY;
		return uni_CharArray2CString(args);
	}
	CString result = CharArray2CString(*(CharArray*)args[0]);
	temp_CString.push_back(result);
	argPointer = &temp_CString[temp_CString.size() - 1];
	return argPointer;
}

void* uni_int2CString(vector<void*> args)
{
	if (isInteger(args[0]) == false) return NULL;
	if (args[0] == SECONDARY)
	{
		args[0] = *SECONDARY;
		return uni_int2CString(args);
	}
	CString result = int2CString(*(int*)args[0]);
	temp_CString.push_back(result);
	argPointer = &temp_CString[temp_CString.size() - 1];
	return argPointer;
}

bool findCluesBackwards(FunctionPointer currentGoal,void* arg,vector<FunctionPointer> *route)
{
	//if (funcTypes[currentGoal] == VERB) 
		route->push_back(currentGoal);

	if (executeUni(currentGoal, arg))
	{
		if (funcTypes[currentGoal] == PREDICATE)
			cout << "判断：" << names[currentGoal] << "？" << "真" << endl;
		else
			cout << "尝试：" << names[currentGoal] << "，" << "成功" << endl;
		return true;
	}
	else
	{
		if (funcTypes[currentGoal] == PREDICATE)
			cout << "判断：" << names[currentGoal] << "？" << "假" << endl;
		else
			cout << "尝试：" << names[currentGoal] << "，" << "失败" << endl;
	}

	vector<FunctionPointer> conditions = getValuesForKey<FunctionPointer, FunctionPointer>(relationships,currentGoal);
	if (conditions.size() == 0) return false;

	for (FunctionPointer condition:conditions)
	{
		bool returning = executeUni(condition, arg);
		
		if (returning == true)
		{
			if (funcTypes[condition] == PREDICATE)
				cout << "判断：" << names[condition] << "？" << (returning ? "真" : "假") << endl;
			else
				cout << "尝试：" << names[condition] << "，" << (returning ? "成功" : "失败") << endl;

			route->push_back(condition);
			return true;
		}
		else
		{
			vector<FunctionPointer> innerRoute = *route;
			bool returning= findCluesBackwards(condition, arg,&innerRoute);
			if (returning == true)
			{
				*route = innerRoute;
				return true;
			}
		}
	}

	return false;
}

void initFunctions()
{
	funcs.push_back(&uni_isCharArray);
	funcs.push_back(&uni_isCString);
	funcs.push_back(&uni_isInteger);
	funcs.push_back(&uni_isStd);
	funcs.push_back(&uni_showMsgBox);
	funcs.push_back(&uni_std2CharArray);
	funcs.push_back(&uni_CharArray2CString);
	funcs.push_back(&uni_int2CString);
	
	funcTypes.insert(*new pair<FunctionPointer, bool>(&uni_isCharArray, PREDICATE));
	funcTypes.insert(*new pair<FunctionPointer, bool>(&uni_isCString, PREDICATE));
	funcTypes.insert(*new pair<FunctionPointer, bool>(&uni_isInteger, PREDICATE));
	funcTypes.insert(*new pair<FunctionPointer, bool>(&uni_isStd, PREDICATE));
	funcTypes.insert(*new pair<FunctionPointer, bool>(&uni_showMsgBox, VERB));
	funcTypes.insert(*new pair<FunctionPointer, bool>(&uni_std2CharArray, VERB));
	funcTypes.insert(*new pair<FunctionPointer, bool>(&uni_CharArray2CString, VERB));
	funcTypes.insert(*new pair<FunctionPointer, bool>(&uni_int2CString, VERB));
	names.insert(*new pair<FunctionPointer, string>(&uni_isCharArray, "是字符数组"));
	names.insert(*new pair<FunctionPointer, string>(&uni_isCString, "是CString型字符串"));
	names.insert(*new pair<FunctionPointer, string>(&uni_isInteger, "是整数"));
	names.insert(*new pair<FunctionPointer, string>(&uni_isStd, "是标准字符串"));
	names.insert(*new pair<FunctionPointer, string>(&uni_showMsgBox, "弹出对话框"));
	names.insert(*new pair<FunctionPointer, string>(&uni_std2CharArray, "标准字符串转为字符数组"));
	names.insert(*new pair<FunctionPointer, string>(&uni_CharArray2CString, "字符数组转为CString"));
	names.insert(*new pair<FunctionPointer, string>(&uni_int2CString, "整数转为CString"));
}

void initRules()
{
	relationships.insert(*new pair<FunctionPointer, FunctionPointer>(&uni_showMsgBox, &uni_isCString));
	relationships.insert(*new pair<FunctionPointer, FunctionPointer>(&uni_isCString, &uni_CharArray2CString));
	relationships.insert(*new pair<FunctionPointer, FunctionPointer>(&uni_isCString, &uni_int2CString));//干扰项
	relationships.insert(*new pair<FunctionPointer, FunctionPointer>(&uni_CharArray2CString, &uni_isCharArray));
	relationships.insert(*new pair<FunctionPointer, FunctionPointer>(&uni_isCharArray, &uni_std2CharArray));
	relationships.insert(*new pair<FunctionPointer, FunctionPointer>(&uni_std2CharArray, &uni_isStd));

	/*
	vector<FunctionPointer> values = getValuesForKey<FunctionPointer, FunctionPointer>(relationships,&uni_isCString);
	for (FunctionPointer it:values)
		cout << names[it] << endl;
	*/
}

//元操作是动词，我这里规定每个元操作函数必须有返回值，返回0（NULL）表示执行失败，返回非零值表示成功。
//谓词仅仅用于判定参数的属性，不会对参数对象造成任何影响。如果传入的参数对象具有某属性，则返回true，否则返回false。
//所有的动词、谓词均只接受一个参数。

//本实验中涉及的“关系”只有两种：
//一种是【执行的动作】与【造成的影响】之间的联系，前者是一个由动词和参数构成的命令，后者是由谓词和参数构成的命题。
//例如【将字符数组转为CString】是执行的动作，【使得成为CString型字符串】是造成的影响，
//另外一种关系是【执行的动作】与【执行此动作所需要满足的前提条件】之间的联系。
//例如【将字符数组转为CString】，首先你传入的参数变量必须得是字符数组，如果是其他对象，则程序会将该对象强制转型成字符数组导致异常，
//这里【参数是字符数组】就是执行命令的前提条件，它也是一个由谓词和参数构成的命题。
//上述这些关联，都不是由程序员预先编写好的，需要通过程序自己学习得到。


//面向目标模式
//在常规模式下，执行一个命令，如果失败了，则系统就会直接在屏幕上输出“失败”。
//而在面向目标模式下，如果执行命令失败了，系统会思考：如果要使这个命令执行成功，需要满足什么条件？然后它会想方设法使得前提条件成立。
//例如给定一个标准型字符串，让系统在对话框里显示这个字符串，由于参数类型不是CString型，执行失败。
//系统想方设法将参数类型转化为CString型，它了解到调用【将字符数组转为CString】可以达到这一效果，
//于是它调用了该函数，结果由于参数不是字符数组，命令执行失败了。
//然后系统又想法设法让参数变成字符数组，它发现【将标准字符串转为字符数组】可以满足这一需求，该命令执行成功。
//于是系统记录下全过程：将标准字符串转为字符数组—→将字符数组转为CString—→弹出对话框
//逐个执行这些命令，最终成功地在对话框里输出了原标准字符串的内容。

//记录负经验（失败经验）
//我在元操作的函数体内写入了异常处理代码，例如【将字符数组转为CString】，如果传入的指针参数如果不是指向字符数组的，
//则直接返回NULL，不会执行后面的代码。但如果将这一句异常处理代码注释掉呢？很显然整个程序会直接崩溃、终止运行。
//我们可以引入一个负经验记录机制，随时随刻地在数据库里面（不要记录在内存里面，因为程序崩溃以后内存的数据就没了）
//记录系统执行的命令C，以及执行命令之前参数的状态P。如果程序崩溃异常退出，则下次运行程序的时候，读取数据库里的运行数据。
//并记录下这条负经验——在参数满足状态P的时候执行命令C，会导致程序崩溃。下次再执行命令C的时候，系统会检查参数是否符合状态P，
//如果满足，则直接返回NULL，避免强制转型导致程序崩溃。经多次训练以后，程序就会习得一套异常处理机制。




string toString_Relationship(pair<FunctionPointer, FunctionPointer> relationship)
{
	return names[relationship.first] + " <---- " + names[relationship.second];
}

void showAllRelationships()
{
	for (FunctionPointer func : funcs)
	{
		vector<FunctionPointer> conditions=
			getValuesForKey<FunctionPointer,FunctionPointer>(relationships, func);
		for (FunctionPointer condition : conditions)
			cout << names[func] << " <---- " << names[condition] << endl;
	}
}

#define STD 1
#define CHARARRAY 2
#define CSTRING 3
#define INT 4

void IO_processInputCmd(string cmd, int type,bool simplyExecute)
{
	string funcName = split(cmd, '|')[0];
	string argString = split(cmd, '|')[1];
	FunctionPointer func = getFunction(funcName);
	switch (type)
	{
	case STD:
	{
		temp_string.push_back(argString); 
		argPointer = &temp_string[temp_string.size()-1]; 
		break; 
	}
	case CHARARRAY:
	{
		temp_CharArray.push_back(argString.c_str()); 
		argPointer = &temp_CharArray[temp_CharArray.size() - 1];
		break; 
	}
	case CSTRING:
	{
		temp_CString.push_back(*new CString(argString.c_str()));
		argPointer = &temp_CString[temp_CString.size() - 1];
		break;
	}
	case INT:
	{
		temp_int.push_back(atoi(argString.c_str()));
		argPointer = &temp_int[temp_int.size() - 1];
		break;
	}
	default:break;
	}

	if (simplyExecute)
	{
		bool returning=executeAndRecordRelationships(func, SECONDARY);
		if (funcTypes[func] == VERB) cout << (returning?"成功\n":"失败\n");
		else cout << (returning?"真\n":"假\n");
	}
	else
	{
		FunctionPointer initialStatus = getTruePredicates(SECONDARY)[0];
		vector<FunctionPointer> route;//用于存储路径
		findCluesBackwards(func, SECONDARY, &route);
		route.push_back(initialStatus);//把初始状态放到最后
		auto backup = route;//备份倒序路径，用于逆序转置
		route.clear();//先把原数组清空
		for (int i = backup.size() - 1; i >= 0; i--)//逆序转置
			route.push_back(backup[i]);

		auto relationships_conditionAndFunction = detectRelationships_conditionAndFunction(route);
		auto relationships_functionAndEffect = detectRelationships_functionAndEffect(route);

		auto appearedComplexFunctions=extractComplexFunctions(relationships_conditionAndFunction, relationships_functionAndEffect);

		IO_saveFunctions(appearedComplexFunctions);

		updateRelationships(relationships_conditionAndFunction, relationships_functionAndEffect);
	}
}

void IO_inputCmdAndArgType(bool simplyExecute)
{
	string cmd;
	cout << "请输入命令：";
	cin >> cmd;
	int argType;
	cout << "请输入参数类型（1：标准字符串；2：字符数组；3.CString型字符串；4.整数）\n";
	cin >> argType;
	IO_processInputCmd(cmd, argType, simplyExecute);
}

void showComplexFunctions()
{
	for (int i = 0; i < count_complexFunctions; i++)
		cout << (i + 1) << ". " << names[uni_complexFunctions[i]] << endl << toString_route(complexFunctions[i]) << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	initFunctions();
	while (true)
	{
		string taskName;
		cin >> taskName;
		if ("执行命令" == taskName)
			IO_inputCmdAndArgType(true);
		else if ("实现目标" == taskName)
			IO_inputCmdAndArgType(false);
		else if ("查看因果关系" == taskName)
			showAllRelationships();
		else if ("查看复杂函数" == taskName)
			showComplexFunctions();
	}
	
	string any;
	
	//initRules();


	temp_string.push_back("澳大利亚"); temp_string.push_back("新加坡"); temp_string.push_back("中国"); temp_string.push_back("德国");
	temp_CharArray.push_back("晚上"); temp_CharArray.push_back("下午"); temp_CharArray.push_back("早上");
	temp_CString.push_back(L"Chemistry"); temp_CString.push_back(L"Math"); temp_CString.push_back(L"History");

	/*
	vector<string> part = *new vector<string>(temp_string.begin() + 1, temp_string.begin() + 3);
	for (string str : part)
		cout << str << "  ";
	cout << endl;
	*/

	argPointer = &temp_string[1];//“新加坡”

	executeAndRecordRelationships(&uni_std2CharArray, SECONDARY);
	showAllRelationships();
	//cin >> any;
	executeAndRecordRelationships(&uni_CharArray2CString, SECONDARY);
	showAllRelationships();
	//cin >> any;
	executeAndRecordRelationships(&uni_showMsgBox, SECONDARY);
	showAllRelationships();
	//cin >> any;
	temp_int.push_back(347);
	argPointer = &temp_int[0];
	executeAndRecordRelationships(&uni_int2CString, SECONDARY);
	showAllRelationships();
	//cin >> any;

	/*
	cout<<executeUni(&uni_showMsgBox, SECONDARY)<<endl;
	cout << executeUni(&uni_std2CharArray, SECONDARY) << endl;
	cout << executeUni(&uni_CharArray2CString, SECONDARY) << endl;
	cout << executeUni(&uni_showMsgBox, SECONDARY) << endl;
	*/

	argPointer = &temp_string[2];
	FunctionPointer initialStatus = getTruePredicates(SECONDARY)[0];
	vector<FunctionPointer> route;
	cout << executeUni(&uni_isCString, SECONDARY) << endl;
	cout << findCluesBackwards(&uni_isCString, SECONDARY, &route) << endl;
	cout << executeUni(&uni_isCString, SECONDARY) << endl;
	route.push_back(initialStatus);

	if (isStd(SECONDARY)) cout << "标准\n";
	else if (isCString(SECONDARY)) cout << "CString\n";
	else if (isCharArray(SECONDARY)) cout << "字符数组\n";
	else if (isInteger(SECONDARY)) cout << "整数\n";
	else cout << "其他\n";


	for (int i = route.size() - 1; i >= 0; i--)
	{
		executeUni(route[i], SECONDARY);
	}
	
	auto backup = route;
	route.clear();
	for (int i = backup.size() - 1; i >= 0; i--)
	{
		route.push_back(backup[i]);
	}

	//route = *new vector<FunctionPointer>(route.begin(), route.end());
	for (int i =0; i<route.size(); i++)
	{
		cout << names[route[i]];
		if (i != route.size()-1) cout << "——>";
	}

	cout << endl;

	if (isStd(SECONDARY)) cout << "标准\n";
	else if (isCString(SECONDARY)) cout << "CString\n";
	else if (isCharArray(SECONDARY)) cout << "字符数组\n";
	else if (isInteger(SECONDARY)) cout << "整数\n";
	else cout << "其他\n";


	for (pair<FunctionPointer, ComplexFunction> thisPair : detectRelationships_conditionAndFunction(route))
	{
		cout << "（" << toString_route(thisPair.second) << "）" ;
		cout << "<——" << names[thisPair.first] << endl;
	}

	//用户交互流程
	//1.显示系统发现的所有因果关系
	//2.显示系统提取出来的函数
	//3.让用户【逐个】给这些函数命名，如果重复就不命名了
	//4.重新建立并显示因果关系

	return 0;
}

