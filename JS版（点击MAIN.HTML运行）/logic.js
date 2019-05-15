//不需要用到IntRef
/*
function IntRef(value){this.value=value;}
IntRef.prototype.getValue=function(){return this.value;}
IntRef.prototype.setValue=function(value){this.value=value;}
*/
//Object.prototype.get_name=function(){return name;}

function ArrayPointer(arrName,index)
{
	this.whichArray=arrName;
	this.index=index;
	this.mark_type="ArrayPointer";
}
ArrayPointer.prototype.get=function(){
	var temp=eval(this.whichArray+"["+this.index+"]");
	if(temp.mark_type=="ArrayPointer")//又是指针
	{
		var inner=temp.get();//再做一次解引用
		if( typeof(inner)=="number" )
			return temp;//必须把整数包裹起来
		else return temp.get();
	}
	else
		return temp;
}
function getValue_arrayPointer(ptr)
{
	var what=what.get();
	if(typeof(what)=="number"||typeof(what)=="string") return what;
	else return what.getValue();//若还是指针，将会递归
}

ArrayPointer.prototype.getValue=function(){
	var what=this.get();
	if(typeof(what)=="number"||typeof(what)=="string") return what;
	else return what.getValue();//若还是指针，将会递归
}
ArrayPointer.prototype.setValue=function(value){
	var target=eval(this.whichArray+"["+this.index+"]");
	console.log("target="+target);
	console.log("target.mark_type="+target.mark_type);
	if(target.mark_type=="ArrayPointer") target.setValue(value);
	else if(target.ptr!=undefined&&target.ptr.mark_type=="ArrayPointer") target.ptr.setValue(value);
	else eval(this.whichArray+"["+this.index+"]="+value);//基本型
}
ArrayPointer.prototype.get_name=function(){
	var what=this.get();
	if(typeof(what)=="number") return "";
	else if(what.mark_type=="ArrayPointer")
		return what.get_name();
	else return what.name;
}

function JSON2ArrayPointer(json)
{
	return new ArrayPointer(json.whichArray,json.index);
}

function binary2Byte(bin)//二进制字符串转为整数（单字节）
{
	return parseInt(bin,2);
}

function byte2Binary(byteVar)//(单字节)整数转为二进制字符串
{
	return byteVar.toString(2);
}

function binary2Short(bin)//二进制字符串转为整数（双字节）
{
	return parseInt(bin,2);
}

function short2Binary(shortVar)//双字节整数转二进制字符串
{
	return shortVar.toString(2);
}

function fill(original,len) //长度不够，高位添0
{
	var result="";
	for(var i=1;i<=len-original.length;i++)
		result+="0";
	result+=original+"";
	return result;
}

function Register(name,maxLen)
{
	this.name=name;
	this.maxLen=maxLen;
	this.value=0;
}
function JSON2Register(jsonVar)
{
	var reg=new Register(jsonVar.name,jsonVar.maxLen);
	reg.value=jsonVar.value;
}
Register.prototype.getString= function(){
	return fill(byte2Binary(this.value), this.maxLen);
}
Register.prototype.getValue=function(){return this.value;}
Register.prototype.setValue=function(value){
	var temp = 255 >> (8-this.maxLen);
	console.log("this.maxLen="+this.maxLen+",255>>maxLen="+temp);
	console.log("value="+value);
	console.log("value&temp="+(value&temp));
	this.value = (value&temp);
	console.log("this.value="+this.value);
}
Register.prototype.compareToZero=function(){
	if ( new Number(this.value) == 0) return 0;
	var flag = new Number(this.value) >> (this.maxLen - 1);
	if (flag == 1) return -1;
	else return 1;
}
Register.prototype.getMaxLen=function(){return this.maxLen;}
Register.prototype.toString=function(){return this.name;}
Register.prototype.get_name=function(){return this.name;}

function Register16(name,high,low)
{
	this.name=name;
	this.high=high;
	this.low=low;
}
function JSON2Register(jsonVar)
{
	var reg=new Register16(jsonVar.name,A,Q);
}

Register16.prototype.getValue=function(){
	var result=0;
	result += ( this.high.value << (this.low.maxLen) );
	result += this.low.value;
	return result;
}

Register16.prototype.setValue=function(value){
	var highValue = value >> (this.low.maxLen);
	var lowValue = value - (highValue << (this.low.maxLen));
	this.high.setValue(highValue);
	this.low.setValue(lowValue);
}

Register16.prototype.getMaxLen=function(){
	 return new Number(this.high.maxLen) + new Number(this.low.maxLen);
}

Register16.prototype.compareToZero=function(){
	var value=new Number(this.getValue());
	if (value == 0) return 0;
	var flag = value >> (this.getMaxLen() - 1);
	if (flag == 1) return -1;
	else return 1;
}
Register16.prototype.toString=function(){return this.name;}
Register16.prototype.get_name=function(){return this.name;}

const REG8=0;
const REG16=1;
const INTEGER=2;
const IMM=3;

function Anything(name)
{
	this.name=name;
	this.value=0;
	this.type= -1;//类型
	this.ptr=null;
}
Anything.prototype.get=function(){return this;}

function createIMM(name,value)//创建立即数对象
{
	var imm=new Anything(name);
	imm.value=value;
	imm.type=IMM;
	return imm;
}

function createObject(name,type,ptr)
{
	var obj=new Anything(name);
	obj.type=type;
	obj.ptr=ptr;
	return obj;
}

Anything.prototype.getValue=function(){
	if(this.type==IMM) return this.value;
	else return this.ptr.getValue();
}

Anything.prototype.setValue=function(value){
	if(this.type==IMM) this.value=value;
	else this.ptr.setValue(value);
}

Anything.prototype.equals=function(other){
	if(this.type!=other.type) return false;
	else if(this.type==IMM)
		return this.value==other.value;
	else return this.ptr==other.ptr;
}

Anything.prototype.get_name=function(){return this.name;}
function getValue_anything(anything)
{
	if(anything.type==IMM) return anything.value;
	else return getValue_arrayPointer(anything.ptr);
}

function toString_anything(anything)
{
	var str="名称："+anything.name+"，值："+//anything.value
	//getValue_anything(anything)
	anything.getValue()
	+"，类型：";
	if(anything.type==REG8||anything.type==REG16) str+="寄存器";
	else if(anything.type==IMM) str+="立即数";
	else if(anything.type==INTEGER) str+="整数";
	return str;
}


function JSON2Anything(json)
{
	var result=new Anything(json.name);
	result.value=json.value;
	result.type= json.type;
	if(json.ptr!=null)
		result.ptr=JSON2ArrayPointer(json.ptr);
	return result;
}

var C=createIMM("计数器",1);//定义计数器☆
var numOfBit=5;
//A、Q寄存器默认长度为5
var A=new Register("A",5);
var Q=new Register("Q",5);
var AQ=new Register16("AQ",A,Q);
var A_obj=createObject("A",REG8,A);
var Q_obj=createObject("Q",REG8,Q);
var AQ_obj=createObject("AQ",REG16,AQ);

function getName(obj){return obj.name;}

function expand(reg)//扩充一位
{
	reg.maxLen++;
	return true;
}

function left(reg)
{
	reg.setValue(reg.getValue()<<1);
	return true;
}

function right(reg)
{
	reg.setValue(reg.getValue() >> 1);
	return true;
}

function write1(reg) //加1
{
	reg.setValue( reg.getValue()+ 1);
	return true;
}

function add(target,value)
{
	target.setValue(target.getValue()+value.getValue());
	console.log("调试：执行完后AQ="+AQ.getValue());
	return true;
}

function positive(arg)
{
	return arg.compareToZero()==1;
}

function negative(arg)
{
	return arg.compareToZero()==-1;
}

function notNegative(arg)
{
	return !negative(arg);
}

function firstTime()
{
	return C.getValue() == 1;
}

function notFirstTime()
{
	return C.getValue()>1;
}

function endWith0(arg)
{
	return arg.getValue() % 2 == 0;
}

function endWith1(arg)
{
	return arg.getValue() % 2 == 1;
}

var methods=new Array();
function 负数(args)
{
	return negative(args[0].ptr);
}
methods.push(负数);

function 正数(args)
{
	return positive(args[0].ptr);
}
methods.push(正数);

function 非负(args)
{
	return !负数(args);
}
methods.push(非负);

function 首次运行(args)
{
	return firstTime();
}
methods.push(首次运行);

function 不是首次运行(args)
{
	return !firstTime();
}
methods.push(不是首次运行);

function 扩展1位(args)
{
	return expand(args[0].ptr);
}
methods.push(扩展1位);

function 加(args)
{
	var target = args[0];
	var value = args[1];	//console.log("目标="+target.name+"，类型="+target.type);
	console.log("目标："+target);
	if (target.type == REG8)
		return add(target.ptr, value);
	else if (target.type == REG16)
		return add(target.ptr, value);
}
methods.push(加);

function 赋值(args)
{
	args[0].setValue(args[1].getValue());
	console.log("args[1].getValue()="+args[1].getValue());
	console.log("args[0].getValue()="+args[0].getValue());
	return true;
}
methods.push(赋值);

function 左移(args)
{
	if (args[0].type == REG8) left(args[0].ptr);
	else if (args[0].type == REG16) left(args[0].ptr);
	else return false;

	return true;
}
methods.push(左移);

function 右移(args)
{
	if(args[0].type==REG8) right(args[0].ptr);
	else if(args[0].type==REG16) right(args[0].ptr);
	else return false;

	return true;
}
methods.push(右移);

function 加1(args)
{
	if (args[0].type == REG8) write1(args[0].ptr);
	else if (args[0].type == REG16) write1(args[0].ptr);
	else return false;

	return true;
}
methods.push(加1);

function 以0结尾(args)
{
	return endWith0(args[0]);
}
methods.push(以0结尾);

function 以1结尾(args)
{
	return endWith1(args[0]);
}
methods.push(以1结尾);

function getMethodByName(name)
{
	for (var i = 0; i < methods.length; i++)
		if (methods[i].name == name)
			return methods[i];
	console.log("无法在methods中找到名为"+name+"的函数");
	return null;
}

const PREDICATE=0;
const VERB=1;

function getMethodType(m)
{
	if( typeof(m)=="function" )
	{
		if(m==扩展1位||m==加||m==赋值||m==加1||m==左移||m==右移)
			return VERB;
		else return PREDICATE;
	}
	else if( typeof(m)=="string" )
	{
		if(m=="扩展1位"||m=="加"||m=="赋值"||m=="加1"||m=="左移"||m=="右移")
			return VERB;
		else return PREDICATE;
	}
}

function getNameByPointer_shortOnly(p)
{
	if(p.whichArray=="userArgValues")
		return userArgs[p.index].name;
	if (p.whichArray=="userTempValues")
		return userTemps[p.index].name;
	if (p.whichArray=="answerValues")
		return answers[p.index].name;
}

function Sentence(verbOrPredicate,nouns)
{
	this.verbOrPredicate=verbOrPredicate;
	this.nouns=nouns;
}

function JSON2Sentence(json)
{
	var temp_nouns=new Array();
	for(let nounInJson of json.nouns)
		temp_nouns.push(JSON2Anything(nounInJson));
	return new Sentence(json.verbOrPredicate,temp_nouns);
}

function createSentence(str)
{
	var str_arr=str.split("|");
	var methodName=str_arr[0];
	var method = getMethodByName(methodName);
	var args_arr=new Array();//Anything型数组
	if (str_arr.length > 1)
	{
		for (var i = 1; i < str_arr.length; i++)
			args_arr.push(getObjectByName(str_arr[i]));
	}
	var sentence = new Sentence(method, args_arr);
	return sentence;
}

Sentence.prototype.execute=function(){
	var convertedNouns=new Array();
	for(let noun of this.nouns)
	{
		if(noun.type==REG8||noun.type==REG16)
			convertedNouns.push(noun);
		else if(noun.type==IMM)//立即数
			convertedNouns.push(noun.getValue());
		else if(noun.type==INTEGER)//指向全局整数变量
			convertedNouns.push(noun);
	}
	console.log("调试：即将执行"+this.verbOrPredicate.name+" "+convertedNouns);
	return this.verbOrPredicate(convertedNouns);
}
Sentence.prototype.toString=function(){
	var result = this.verbOrPredicate.name;
	if (this.nouns.length>0)
	{
		result+="|";
		for(var i=0;i<this.nouns.length;i++)
			result+=this.nouns[i].name+( (i==this.nouns.length-1)?"":"|" );
	}
	return result;
}
Sentence.prototype.judge=function(){this.execute();}
Sentence.prototype.equals=function(other){
	var str_this=this.toString();
	var str_other=other.toString();
	return (str_this==str_other);
	/*
	if (this.verbOrPredicate != other.verbOrPredicate)
		return false;
	if (this.nouns.length != other.nouns.length)
		return false;
	for (var i = 0; i < this.nouns.length; i++)
		if (this.nouns[i].equals(other.nouns[i])==false)
			return false;
	return true;
	*/
}

function allTrue(conditions)
{
	for(var i=0;i<conditions.length;i++)
		if(conditions[i].execute()==false)
			return false;
	return true;
}

function IfThen(command)
{
	this.command=command;
	this.conditions=new Array();
}

IfThen.prototype.conditionsToString=function(){
	var str="";
	for(var i=0;i<this.conditions.length;i++)
	{
		str+=this.conditions[i].toString();
		if (i != this.conditions.length - 1)
			str += "&&";
	}
	return str;
}

IfThen.prototype.toString=function(){
	return "if("+this.conditionsToString()+") "+this.command.toString();
}

IfThen.prototype.execute=function(){
	for (var i = 0; i < this.conditions.length; i++)
		if (this.conditions[i].execute() == false)
			return false;
	return this.command.execute();
}

function MapOfIfThen(command,conditions)
{
	this.command=command;
	this.conditions=conditions;
}

IfThen.prototype.toMap=function(){
	return new MapOfIfThen(this.command.toString(),stringify_sentenceArray(this.conditions));
}
function map2IfThen(mapVar)
{
	var ifThen=new IfThen(createSentence(mapVar.command));//字符串转为Sentence
	ifThen.conditions=parse_sentenceArray(mapVar.conditions);
	//for(let proposition of json.conditions) ifThen.conditions.push(JSON2Sentence(proposition));
	return ifThen;
}

function intersect(a,b)
{
	var result=new Array();
	for (var i = 0; i < a.length; i++)
	{
		for (var j = 0; j < b.length; j++)
		{
			if (b[j].equals(a[i]))
			{
				result.push(a[i]);
				break;//退出查找
			}
		}
	}
	return result;
}

var allObjs=getAllObjs();//全局对象数组
var userArgs=getUserArgs();
var userArgValues=getUserArgValues();//整数型数组
var userTempValues=getUserTempValues();//整数型数组
var userTemps=getUserTempValues();
var answerValues=getAnswerValues();//整数型数组
var answers=getAnswers();

function getAllObjs()
{
	if(localStorage["allObjs"]==undefined){
		var results=new Array();
		console.log("if分支 results.push(A_obj): "+A_obj);
		results.push(A_obj);
		results.push(Q_obj);
		results.push(AQ_obj);
		return results;
	}
	else
	{
		var results=new Array();
		results.push(A_obj);
	    console.log("results.push(A_obj): "+A_obj);
		results.push(Q_obj);
		results.push(AQ_obj);
		var arrFromJSON=JSON.parse(localStorage["allObjs"]);
		console.log("localStorage['allObjs']="+localStorage["allObjs"])
		A.maxLen=arrFromJSON[0].ptr.maxLen;
		Q.maxLen=arrFromJSON[1].ptr.maxLen;
		for(var i=3;i<arrFromJSON.length;i++)
		{
			var elementFromJSON=arrFromJSON[i];
			results.push(JSON2Anything(elementFromJSON));
		}
		return results;
	}
	//return JSON.parse(localStorage["allObjs"]);//返回数组 error!
}
function setAllObjs(arr)
{
	localStorage["allObjs"]=JSON.stringify(arr);
}
function getUserArgs()
{
	if(localStorage["userArgs"]==undefined) return new Array();
	else{
		var results=new Array();
		var arrFromJSON=JSON.parse(localStorage["userArgs"]);
		for(let element of arrFromJSON)
			results.push(JSON2Anything(element));
		return results;
	}
	//return JSON.parse(localStorage["userArgs"]);//返回数组  应该调用JSON2Anything啊！！！
}
function setUserArgs(arr)
{
	localStorage["userArgs"]=JSON.stringify(arr);
}
function getUserArgValues()
{
	if(localStorage["userArgValues"]==undefined) return new Array();
	return JSON.parse(localStorage["userArgValues"]);//返回数组
}
function setUserArgValues(arr)
{
	localStorage["userArgValues"]=JSON.stringify(arr);
}
function getUserTempValues()
{
	if(localStorage["userTempValues"]==undefined) return new Array();
	return JSON.parse(localStorage["userTempValues"]);//返回数组
}
function setUserTempValues(arr)
{
	localStorage["userTempValues"]=JSON.stringify(arr);
}
function getUserTemps()
{
	if(localStorage["userTemps"]==undefined) return new Array();
	else
	{
		var results=new Array();
		var arrFromJSON=JSON.parse(localStorage["userTemps"]);
		for(let element of arrFromJSON)
			results.push(JSON2Anything(element));
		return results;
	}
	//return JSON.parse(localStorage["userTemps"]);错的！！
}
function setUserTemps(arr)
{
	localStorage["userTemps"]=JSON.stringify(arr);
}
function getAnswers()//这个地方要改一下啊！！！
{
	if(localStorage["answers"]==undefined) return new Array();
	else
	{
		var results=new Array();
		var arrFromJSON=JSON.parse(localStorage["answers"]);
		for(let element of arrFromJSON)
			results.push(JSON2Anything(element));
		return results;
	}
	//return JSON.parse(localStorage["answers"]);//不能直接这样Parse的！
}
function setAnswers(arr)
{
	localStorage["answers"]=JSON.stringify(arr);
}
function getAnswerValues()
{
	if(localStorage["answerValues"]==undefined) return new Array();
	return JSON.parse(localStorage["answerValues"]);
}
function setAnswerValues(arr)
{
	localStorage["answerValues"]=JSON.stringify(arr);
}

function getIfThens()
{
	if(localStorage["ifThens"]==undefined||localStorage["ifThens"]=="") return new Array();
	else
	{
		var results=new Array();
		var arr=JSON.parse(localStorage["ifThens"]);
		console.log("arr="+arr);
		for(let inner of arr)
		{
			var map=new MapOfIfThen(inner.command,inner.conditions);
			var ifThen=map2IfThen(map);
			results.push(ifThen);
		}
		return results;
	}
}

function setIfThens(arr)
{
	if(arr.length==0) localStorage["ifThens"]="";
	else
	{
		var results=new Array();
		for(let ifThen of arr)
			results.push(ifThen.toMap());
		localStorage["ifThens"]=JSON.stringify(results);
	}
}

function getObjectByName(name)
{
	for (var i = 0; i < allObjs.length; i++)
		if (allObjs[i].name == name)
			return allObjs[i];
	return null;
}

function restore(argValues)
{
	var allObjs=getAllObjs();
	var userArgValues=getUserArgValues();
	for (var i = 0; i < allObjs.length; i++)
		allObjs[i].setValue(0);
	for (var i = 0; i < argValues.length; i++)
		userArgValues[i] = argValues[i];
}

function insert_userArg(name,value)
{
	userArgValues.push(value);
	userArgs.push(createObject(name,INTEGER,new ArrayPointer("userArgValues",userArgValues.length-1)));
	allObjs.push( createObject(name,INTEGER,new ArrayPointer("userArgs",userArgs.length-1)));
}

function insert_userTemp(name,value)
{
	userTempValues.push(value);
	userTemps.push(createObject(name,INTEGER,new ArrayPointer("userTempValues",userTempValues.length-1)));
	allObjs.push( createObject(name,INTEGER,new ArrayPointer("userTemps",userTemps.length-1)) );
}

function insert_answer(name,value)
{
	answerValues.push(value);
	answers.push(createObject(name,INTEGER,new ArrayPointer("answerValues",answerValues.length-1)));
	allObjs.push( createObject(name,INTEGER,new ArrayPointer("answers",answers.length-1)) );
}

function stringify_sentenceArray(arr)
{
	if(arr.length==0) return "";
	else return arr.join(";");
}
function parse_sentenceArray(strVar)
{
	if(strVar=="") return new Array();
	else
	{
		var result=new Array();
		var splited=strVar.split(";");
		//console.log("strVar="+strVar+"   Splited="+splited);
		for(let one of splited)
			result.push(createSentence(one));
		return result;
	}
}

function getPreCommands()
{
	if(localStorage["pre_commands"]==undefined) return new Array();
	else{
		return parse_sentenceArray(localStorage["pre_commands"]);
	}
}
function setPreCommands(arr)
{
	localStorage["pre_commands"]=stringify_sentenceArray(arr);
}

function getMiddleCommands()
{
	if(localStorage["middle_commands"]==undefined) return new Array();
	else{
		return parse_sentenceArray(localStorage["middle_commands"]);
	}
}
function setMiddleCommands(arr)
{
	localStorage["middle_commands"]=stringify_sentenceArray(arr);
}

function getEndCommands()
{
	if(localStorage["end_commands"]==undefined) return new Array();
	else{
		return parse_sentenceArray(localStorage["end_commands"]);
	}
}
function setEndCommands(arr)
{
	localStorage["end_commands"]=stringify_sentenceArray(arr);
}

var middle_commands=getMiddleCommands();//中间命令
var ifThens=getIfThens();

function getNameByPointer_shortOnly(p){return p.name;}

var pre_commands=getPreCommands();
var end_commands=getEndCommands();
var propositions=new Array();
init_createPropositions();

function createNewArgs(names)
{
	for(let name of names) insert_userArg(name,0);//初值为0
}

function createNewTemps(names)
{
	for(let name of names) insert_userTemp(name,0);
}

function createNewAnswers(names)
{
	for(let name of names) insert_answer(name,0);
}

Function.prototype.type=function(){
	return getMethodType(this);
}

function init_createPropositions() //初始化，枚举所有状态命题
{
	for (var i = 0; i < methods.length; i++)
	{
		if (getMethodType(methods[i]) == VERB) continue;
		if (methods[i].name == "首次运行" || methods[i].name == "不是首次运行")
		{
			propositions.push(new Sentence(methods[i], new Array()));
			continue;
		}

		for (var j = 0; j < allObjs.length; j++)
		{
			var isArg = false;
			for (var k = 0; k <userArgs.length; k++)
				if (userArgs[k].name == allObjs[j].get_name()) { isArg = true; break; }
			var isAnswer = false;
			for (var k = 0; k < answers.length; k++)
				if (answers[k].name == allObjs[j].get_name()) { isAnswer = true; break; }

			if (isArg||isAnswer) continue;
			var nouns=new Array();
			nouns.push(allObjs[j].get());
			propositions.push(new Sentence(methods[i], nouns));
		}
	}
}

function selectTrueStatements()
{
	var results=new Array();
	for(var i=0;i<propositions.length;i++)
		if(propositions[i].execute()==true)
			results.push(propositions[i]);
	return results;
}

function findIfThenByCommand(command)
{
	for(var i=0;i<middle_commands.length;i++)
		if(ifThens[i].command.equals(command))
			return ifThens[i];
	return null;
}

function insertNewCommand(command)//重复返回false，插入成功返回true
{
	for(var i=0;i<middle_commands.length;i++)
		if(command.equals(middle_commands[i]))
			return false;
	
	middle_commands.push(command);
	ifThens.push(new IfThen(middle_commands[middle_commands.length-1]));
	return true;
}

function createConnections(command)
{
	var isNew = insertNewCommand(command);
	var ifThen = null;
	if (isNew) ifThen = ifThens[middle_commands.length - 1];
	else ifThen = findIfThenByCommand(command);
	
	if (isNew) ifThen.conditions = selectTrueStatements();
	else ifThen.conditions = intersect(ifThen.conditions, selectTrueStatements());

	return isNew;
}

function executeSuitableCommand()//返回值是命令
{
	for (var i = 0; i < middleCommands.length; i++)
	{
		if (allTrue(ifThens[i].conditions))
		{
			ifThens[i].command.execute();
			return ifThens[i].command;
		}
	}
	return null;
}

function clear()
{
	C.setValue(1);//循环计数器设置为1
	A.setValue(0);//寄存器清零
	Q.setValue(0);//同上

	for (var i = 0; i < allObjs.length; i++)
		allObjs[i].setValue(0);
}

function executeAll()
{
	var displayContent="";
	for (var i = 0; i < pre_commands.length; i++)
	{
		displayContent+=pre_commands[i].toString()+"\n";
		pre_commands[i].execute();
	}

	for (var i = 0; i < middleCommands.length; i++)
	{
		var command = executeSuitableCommand();
		if (command != null)
			displayContent+=command.toString()+"\n";
	}

	for (var i = 0; i < end_commands.length; i++)
	{
		displayContent+=end_commands[i].toString()+"\n";
		end_commands[i].execute();
	}
	return displayContent;
}

function allValues_toString()
{
	var result="";
	for (var i = 0; i < allObjs.length; i++)
	{
		result += allObjs[i].name;
		result+="=";
		
		var currentObj=null;
		//if(allObjs[i].mark_type=="ArrayPointer")
		//	currentObj=allObjs[i].get();
		//else
			currentObj=allObjs[i];
		
		if (currentObj.type == INTEGER) //allObjs里是不可能有立即数的
		{
			var js_getInnerArrayPointer=currentObj.ptr.whichArray+"["+currentObj.ptr.index+"]";//名字形如XXXXArg
			var innerArrayPointer=eval(js_getInnerArrayPointer);
			var whichArr=innerArrayPointer.ptr.whichArray;
			var index=innerArrayPointer.ptr.index;
			console.log(whichArr+"["+index+"]");
			result += short2Binary( eval(whichArr+"["+index+"]") );
			//console.log(currentObj.ptr.whichArray+"["+currentObj.ptr.index+"].value");
			//short2Binary(currentObj.getValue());
		}
		else
		{
			var maxLen=0;
			if (currentObj.type == REG8)
				maxLen=currentObj.ptr.maxLen;
			else if(currentObj.type==REG16)
				maxLen = AQ.getMaxLen();
			
			if(currentObj.type==REG8) result+=fill(short2Binary(currentObj.ptr.value), maxLen);
			else if(currentObj.type==REG16) result+=fill(short2Binary(AQ.getValue()),maxLen);
			
			//result += fill(short2Binary(currentObj.getValue()), maxLen);这个地方出错！
		}
		result += "    ";
	}
	result+="  C="+C.value+"\n";
	return result;
}

function alreadyInsertRegister(regName,index)
{
	if(allObjs.length-1<index) return false;
	else return allObjs[index].name==regName;
}

function Solution_execute()
{
	C.setValue(0);
	for(var i=0;i<pre_commands.length;i++)
	{
		pre_commands[i].execute();
		document.write(pre_commands[i].toString()+"<br>");
		document.write(allValues_toString()+"<br>");
	}
	document.write("<br>");
	for(var c=1;c<=Q.maxLen;c++)
	{
		C.setValue(c);
		for (var i = 0; i < ifThens.length; i++)
	    {
		    var succeed = ifThens[i].execute();
		    if (succeed)
		    {
			    document.write(ifThens[i].command.toString()+"<br>");
			    document.write(allValues_toString()+"<br>");
		    }
	    }
	}
	document.write("<br>");
	for(var i=0;i<end_commands.length;i++)
	{
		end_commands[i].execute();
		document.write(end_commands[i].toString()+"<br>");
		document.write(allValues_toString()+"<br>");
	}
}

function reOrderIfThens(indexes)
{
	var temp=new Array();
	for(let index of indexes)
	{
		temp.push(ifThens[index]);
	}
	for(var i=0;i<ifThens.length;i++)
		ifThens[i]=temp[i];
	setIfThens(ifThens);
}