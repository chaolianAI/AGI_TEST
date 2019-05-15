#pragma once
#include<vector>
using namespace std;

#define RETURN_TEMP(resultType) temp_##resultType##.push_back(result);\
                    return &temp_##resultType##[temp_##resultType##.size()-1];


#define createVerb(verbName) void* uni_##verbName##(vector<void*> args)
#define createSimpleTypeVerb_tribleArgs(verbName,arg1type,arg2type,arg3type) void* uni_##verbName##(vector<void*> args){\
                    return (void*)(##verbName##((##arg1type##)args[0], (##arg2type##)args[1]),(##arg3type##)args[2]));}
#define createSimpleTypeVerb_doubleArgs(verbName,arg1type,arg2type) void* uni_##verbName##(vector<void*> args){\
                    return (void*)(##verbName##((##arg1type##)args[0], (##arg2type##)args[1]));}
#define createSimpleTypeVerb_singleArg(verbName,argtype) void* uni_##verbName##(vector<void*> args){\
                    return (void*)(##verbName##((##argtype##)args[0]));}

typedef void* (*FunctionPointer)(vector<void*>);

#define PREDICATE true
#define VERB false
#define PROPOSITION true
#define COMMAND false

class Sentence
{
public:
	FunctionPointer verb;
	vector<void*> args;
	vector<string> const_string;
	vector<Sentence> subSentence;
	Sentence *parent;
	void* execute()
	{
		if (subSentence.size() == 0)
			return verb(args);
	}
	bool getType();
};
