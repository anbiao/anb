#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*
保留字：
const		int		float		char		void		main
while		scanf	printf	return	if			else
switch		case		default
>		<		>=		<=		==		!=
+		-		*			/			(			)
{		}		[			]			;			:
,		=		"			'			标识符	整数
浮点数		.			_
{=BIGLSY, }=BIGRSY, [=MIDLSY, ]=MIDRSY, ==EVALUATESY, "=STRINGSY, '=CHARACTERSY
*/

enum SYM {
	CASESY, CHARSY, CONSTSY, DEFAULTSY, ELSESY, FLOATSY,
	IFSY, INTSY, MAINSY, PRINTFSY, RETURNSY, SCANFSY,
	SWITCHSY, VOIDSY, WHILESY,
	BIGSY, LESSSY, BIGESY, LESSESY, EQUALSY, UNEQUALSY,
	PLUSSY, MINUSSY, MULTSY, DIVSY, LSY, RSY,
	BIGLSY, BIGRSY, MIDLSY, MIDRSY, SEMISY, COLONSY,
	COMMASY, EVALUATESY, STRINGSY, CHARACTERSY,
	IDSY, INTNUMSY, FLOATNUMSY, DOTSY, UNDERLINESY,
	INTEGERSY
}symbol;

char now_C,//读取源程序时，当前的字符。
Token[100],//读取源程序时，所识别的一个符号串。
name[100],//准备存入符号表中的标识符名字。
kind[20],//准备存入符号表中的标识符种类。
type[20];//准备存入符号表中的标识符类型。

FILE *f_in;//源程序文件指针。

int flag_error,//是否出现词法错误。
flag_step_const,//全局常量声明是否结束。
flag_step_var,//全局变量声明是否结束。
flag_step_function,//全局函数声明是否结束。
line,//行数。
level,//层级。
array_max,//数组上限。
P_op_stack,//运算数栈栈顶的“指针”。
P_run_stack,//静态运行栈栈顶的“指针”。
P_d_run_stack,//动态运行栈栈顶的“指针”。
P_label_array,//label_array栈顶的“指针”。
search_addr,//查表查到同名标识符时，符号表中标识符代表的值在run_stack中的地址。
P_CODE_TOP,//生成P-CODE时，P-CODE集合的指令数量，即下一条指令将要存入集合数组的位置。
P_string,//存入string_array的位置
switch_jump_end,//switch语句跳转至最后的标签。
void_or_not,//有无返回值函数的标志，1为无返回值函数，0为有返回值函数。
var_distance,//函数中变量相对于函数起始地址的偏移量。
UseVarInDorS,//使用的标识符存在于静态/动态运行栈的标志。
express_type,//表达式值的类型，0为字符型，1为整型，2为实型。
id_use_flag,//代表当前标识符的状态是什么，0为定义，1为使用。
fun_flag,//主要为了判断当前需要用动态运行栈还是静态运行栈。
main_flag;//

float op_stack[100],//存放运算数的栈。
static_run_stack[1000];//“静态运行栈”

//动态运行栈结构
float dynamic_run_stack[1000][1000];

/*
更改：

#0 关于运行栈的更改：
static_run_stack(float型数组)；dynamic_run_stack(float型二维数组)
所有涉及原run_stack的指令都需要更改，并添加一个参数，0代表存入static_run_stack，1代表存入dynamic_run_stack
dynamic只存函数值，函数内部变量值，其余全部存入static

#1 scanf的更改：
将原来的两个参数改为一个参数，现在读入的值先存到op_stack，再转存至运行栈，因此在生成scanf指令的时候，由原来的单条指令变为两条指令

#2 函数调用的更改：（之后写）

#3 增添指针P_d_run_stack，动态运行栈的栈顶指针，将原来的P_run_stack变为静态运行栈的栈顶指针。

#4 符号表中增添一项D_or_S，代表该标识符存放在静态/动态运行栈。

#5 增加UseVarInDorS，使用的标识符存在于静态/动态运行栈的标志。
*/

int label_array[100];//存储标签。

char string_array[100][1000];

char CODE_NAME[40][4] = {
"LDN",//加载数值1：将数值X存入op_stack栈顶中
"PSN",//加载数值2：将X中的数值存入op_stack的栈顶，X为run_stack的地址，Y为0时为静态运行栈，为1时为动态运行栈
"TKN",//提取数值：将op_stack的栈顶中的数值提取至X中，X为run_stack的地址，Y为0时为静态运行栈，为1时为动态运行栈
"STA",//存入数组值：将op_stack栈顶中的值，存入run_stack中，地址为次栈顶，X为0时为静态运行栈，为1时为动态运行栈
"ADD",//加法运算：对op_stack栈顶两个元素进行加法运算，结果存至栈顶
"SUB",//减法运算：op_stack次栈顶元素减栈顶元素，结果存至栈顶
"MUT",//乘法运算：对op_stack栈顶两个元素进行乘法运算，结果存至栈顶
"DIV",//除法运算：op_stack次栈顶元素除以栈顶元素，结果存至栈顶
"BIG",//大于判断：若op_stack次栈顶元素大于栈顶元素，退栈并栈顶置一
"LES",//小于判断：若op_stack次栈顶元素小于栈顶元素，退栈并栈顶置一
"BOE",//大于等于：若op_stack次栈顶元素大于等于栈顶元素，退栈并栈顶置一
"LOE",//小于等于：若op_stack次栈顶元素小于等于栈顶元素，退栈并栈顶置一
"EQL",//等于判断：若op_stack次栈顶元素等于栈顶元素，退栈并栈顶置一
"UEQ",//不等于判断：若op_stack次栈顶元素不等于栈顶元素，退栈并栈顶置一
"MIN",//变负操作：将op_stack栈顶元素变负，存入栈顶
"LAB",//设置标签：记录当前指令位置，存入label_array栈顶，栈顶+1
"JMP",//无条件跳转：跳转至label_array中flag_jump和X相同的指令
"ZJP",//条件为零跳转：op_stack栈顶元素为0跳转至label_array中flag_jump和X相同的指令
"SCF",//输入：格式化输入至op_stack栈顶，X代表类型，0-char, 1-int, 2-float
"PRS",//输出：输出string_array[X]字符串的值
"PRE",//输出栈顶表达式的值
"RTN",//有返回值返回：返回值在op_stack栈顶，直接用
"FED",//函数结束标志：返回调用函数位置，动态运行栈退一个
"FUS",//函数调用标志：调用函数位置，分配动态运行栈空间
"MBG",//主函数开始位置：
"JTM",//跳转至主函数开始位置：
"ATO",//取数组元素的值：从run_stack中取值，地址为op_stack栈顶的值，取值后存入栈顶，X为0时为静态运行栈，为1时为动态运行栈
"JAM",//判断是否超过数组上届：X为数组上界，与op_stack栈顶相比，若大于则继续执行，小于等于则停止执行。
"DRS",//分配动态运行栈空间以便进入函数时使用
"STP",//传函数参数
"SWT"
};
/*
*
*************************************指令助记符与编号对应表*******************************************
*
*0		LDN	X		加载数值1：将数值X存入op_stack栈顶中
*1		PSN	X	Y	加载数值2：将X中的数值存入op_stack的栈顶，X为run_stack的地址，Y为0时为静态运行栈，为1时为动态运行栈
*2		TKN	X	Y	提取数值：将op_stack的栈顶中的数值提取至X中，X为run_stack的地址，Y为0时为静态运行栈，为1时为动态运行栈
*3		STA	X		存入数组值：将op_stack栈顶中的值，存入run_stack中，地址为次栈顶，X为0时为静态运行栈，为1时为动态运行栈
*4		ADD			加法运算：对op_stack栈顶两个元素进行加法运算，结果存至栈顶
*5		SUB			减法运算：op_stack次栈顶元素减栈顶元素，结果存至栈顶
*6		MUT			乘法运算：对op_stack栈顶两个元素进行乘法运算，结果存至栈顶
*7		DIV			除法运算：op_stack次栈顶元素除以栈顶元素，结果存至栈顶
*8		BIG			大于判断：若op_stack次栈顶元素大于栈顶元素，退栈并栈顶置一
*9		LES			小于判断：若op_stack次栈顶元素小于栈顶元素，退栈并栈顶置一
*10		BOE			大于等于：若op_stack次栈顶元素大于等于栈顶元素，退栈并栈顶置一
*11		LOE			小于等于：若op_stack次栈顶元素小于等于栈顶元素，退栈并栈顶置一
*12		EQL			等于判断：若op_stack次栈顶元素等于栈顶元素，退栈并栈顶置一
*13		UEQ			不等于判断：若op_stack次栈顶元素不等于栈顶元素，退栈并栈顶置一
*14		MIN			变负操作：将op_stack栈顶元素变负，存入栈顶
*15		LAB			设置标签：记录当前指令位置，存入label_array栈顶，栈顶+1
*16		JMP	X		无条件跳转：跳转至label_array中flag_jump和X相同的指令
*17		ZJP			条件为零跳转：op_stack栈顶元素为0跳转至label_array中flag_jump和X相同的指令
*18		SCF	X		输入：格式化输入至op_stack栈顶，X代表类型，0-char, 1-int, 2-float
*19		PRS	X		输出：输出string_array[X]字符串的值
*20		PRE			输出：输出表达式的值
*21		RTN			有返回值返回：返回值在op_stack栈顶，直接用
*22		FED			函数结束标志：返回调用函数位置，动态运行栈退一个
*23		FUS			函数调用标志：调用函数位置，分配动态运行栈空间
*24		MBG			主函数开始位置：
*25		JTM			跳转至主函数开始位置：
*26		ATO	X		取数组元素的值：从run_stack中取值，地址为op_stack栈顶的值，取值后存入栈顶，X为0时为静态运行栈，为1时为动态运行栈
*27		JAM	X		判断是否超过数组上届：X为数组上界，与op_stack栈顶相比，若大于则继续执行，小于等于则停止执行。
*28		DRS			分配动态运行栈空间以便进入函数时使用
*
*******************************************************************************************************
*/

//单条指令结构。
struct CODE
{
	char Code_Name[4];//助记符名称。
	int Code_Num;//助记符编号。
	float OP_1;//第一个操作数。
	float OP_2;//第二个操作数。
	int flag_jump;//跳转标识。
}*PCODE[100000];//PCODE：生成P-CODE指令集合，结构指针数组。
  

//符号表结构，包含标识符简单信息。
struct Table
{
	char name[100];//标识符名称。
	char kind[20];//标识符种类（变量，函数，常量，数组）。
	char type[20];//标识符类型（int, char, float）。
	int level;//标识符层级。
	int addr;//标识符地址。
	int array_max;//数组上限。
	int para[100];//函数参数地址。
	int para_type[100];//函数参数类型。
	int func_begin_addr;//函数地址。
	int para_num;//函数参数个数。
	int D_or_S;//标识符存需要放在静态/动态运行栈的标志。
	struct Table *next;
	struct Table *pre;
}*myTable, *level_end, *func_struct;
/*
myTable: 当前符号表栈顶。
level_end: 记录标识符每一层最后一个节点的数组。
*/

int express(), term(), factor(), function_use(), condition(), insert(int n), search(char name[]);
void const_description(), var_description(), compound_statement(), sentence_list(), function_description(), main_function(), program(), sentence();

//查表操作 。
int search(char name[]) {
	struct Table *look;
	char func_kind[] = { "return-function" };
	char nfunc_kind[] = { "noreturn-function" };
	look = myTable;
	while (1){
		if (strcmp(look->name, name) == 0) {
			if (id_use_flag == 1) {
				search_addr = look->addr;
				UseVarInDorS = look->D_or_S;
				func_struct = look;
				return 1;
			}
			else {
				if (look->level == level) {
					search_addr = look->addr;
					UseVarInDorS = look->D_or_S;
					func_struct = look;
					return 1;
				}
				else return 0;
			}
		}
		else {
			if (look->pre != NULL) look = look->pre;
			else break;
		}
	}
	return 0;
}

//填表操作。（n为0时，调用静态运行栈；为1时，调用动态运行栈）
int insert(int n) {
	char type_list[3][6] = { "int","float","char" };
	char array_kind[] = { "array" };
	struct Table *temp_Table;
	temp_Table = (struct Table *)malloc(sizeof(struct Table));
	strcpy(temp_Table->name, name);
	strcpy(temp_Table->kind, kind);
	strcpy(temp_Table->type, type);
	temp_Table->level = level;
	if (n == 0) {
		if (myTable != NULL) {
			if (!search(temp_Table->name)) {
				temp_Table->addr = P_run_stack;
				if (strcmp(kind, array_kind) == 0) {
					P_run_stack += array_max - 1;
					temp_Table->array_max = array_max;
				}
				P_run_stack++;
				array_max = 0;
				temp_Table->D_or_S = 0;
				temp_Table->pre = myTable;
				myTable->next = temp_Table;
				myTable = temp_Table;
			}
			else {
				printf("%s is redefined!\n", temp_Table->name);
				return 0;
			}
		}
		else {
			temp_Table->addr = P_run_stack;
			if (strcmp(kind, array_kind) == 0) {
				P_run_stack += array_max - 1;
				temp_Table->array_max = array_max;
			}
			P_run_stack++;
			array_max = 0;
			temp_Table->D_or_S = 0;
			myTable = temp_Table;
			myTable->pre = NULL;
		}
	}
	else {
		if (myTable != NULL) {
			if (!search(temp_Table->name)) {
				temp_Table->addr = var_distance;
				if (strcmp(kind, array_kind) == 0) {
					 var_distance += array_max - 1;
					temp_Table->array_max = array_max;
				}
				var_distance++;
				array_max = 0;
				temp_Table->D_or_S = 1;
				temp_Table->pre = myTable;
				myTable->next = temp_Table;
				myTable = temp_Table;
			}
			else {
				printf("%s is redefined!\n", temp_Table->name);
				return 0;
			}
		}
		else {
			temp_Table->addr = 0;
			var_distance++;
			array_max = 0;
			temp_Table->D_or_S = 1;
			myTable = temp_Table;
			myTable->pre = NULL;
		}
	}
	//printf("%s:\td: %d\tf: %f\tc: %c\n",myTable->name,(int)run_stack[P_run_stack-1], run_stack[P_run_stack-1], (char)run_stack[P_run_stack-1] );
	return 1;
}

//进入下一层次。
void lvlup() {
	level=1;
	level_end = myTable;
}

//退回至上一层次。
void lvlback() {
	level = 0;
	myTable = level_end;
}

//用二分法检查是否为保留字。
int isReserve() {
	char res[15][10] = { "case","char","const","default","else","float","if","int","main","printf","return","scanf","switch","void","while" };
	//res和SYM中的保留字对应序号一致。
	int min, max, i;
	min = 0;
	max = 15;
	i = (min + max) / 2;
	strlwr(Token);
	while (min <= max) {
		if (strcmp(Token, res[i]) == 0)
			return i;
		else if (strcmp(Token, res[i]) > 0) {
			min = i + 1;
			i = (min + max) / 2;
		}
		else if (strcmp(Token, res[i]) < 0) {
			max = i - 1;
			i = (min + max) / 2;
		}
	}
	return -1;
}

//获取一个字符，并将字符补充到Token末尾。
void catToken() {
	char a[2];
	a[0] = now_C;
	a[1] = '\0';
	strcat(Token, a);
}

//文件指针回退一个字符。
void retract() {
	fseek(f_in, -1L, 1);
}

//清楚当前Token中的所有字符。
void clearToken() {
	int i, len;
	len = strlen(Token);
	for (i = len - 1; i >= 0; i--) {
		Token[i] = '\0';
	}
}

//错误处理（词法）。
void error() {
	flag_error = 1;
	printf("%c is illegal characters.\n", now_C);
}

//错误处理（语法）。
void grammer_error(int x) {
	switch (x) {
	case 0: {
		printf("语法错误：第%d行丢失 ' = '。\n", line);
	}break;
	case 1: {
		printf("语法错误：第%d行丢失整数。\n", line);
	}break;
	case 2: {
		printf("语法错误：第%d行丢失标识符。\n", line);
	}break;
	case 3: {
		printf("语法错误：第%d行丢失 ' ; ' 。\n", line);
	}break;
	case 4: {
		printf("语法错误：第%d行丢失实数。\n", line);
	}break;
	case 5: {
		printf("语法错误：第%d行丢失字符。\n", line);
	}break;
	case 6: {
		printf("语法错误：第%d行丢失类型标识符。\n", line);
	}break;
	case 7: {
		printf("语法错误：第%d行丢失 ' ( ' 。\n", line);
	}break;
	case 8: {
		printf("语法错误：第%d行丢失 ' ) ' 。\n", line);
	}break;
	case 9: {
		printf("语法错误：第%d行丢失 ' { ' 。\n", line);
	}break;
	case 10: {
		printf("语法错误：第%d行丢失 ' } ' 。\n", line);
	}break;
	case 11: {
		printf("语法错误：第%d行，常量声明应在程序开始处。\n", line);
	}break;
	case 12: {
		printf("语法错误：第%d行，变量声明应在函数声明前，常量声明后。\n", line);
	}break;
	case 13: {
		printf("语法错误：第%d行，出现不符合预期的符号。%s\n", line, Token);
	}break;
	case 14: {
		printf("语法错误：第%d行，函数声明应在主函数前，变量声明后。\n", line);
	}break;
	case 15: {
		printf("语法错误：第%d行丢失 ' ] ' 。\n", line);
	}break;
	case 16: {
		printf("语法错误：第%d行丢失 ' , ' 。\n", line);
	}break;
	case 17: {
		printf("语法错误：第%d行丢失表达式。\n", line);
	}break;
	case 18: {
		printf("语法错误：第%d行丢失因子。\n", line);
	}break;
	case 19: {
		printf("语法错误：第%d行丢失项。\n", line);
	}break;
	case 20: {
		printf("语法错误：第%d行丢失函数调用。\n", line);
	}break;
	case 21: {
		printf("语法错误：第%d行丢失条件。\n", line);
	}break;
	case 22: {
		printf("语法错误：第%d行丢失case。\n", line);
	}break;
	case 23: {
		printf("语法错误：第%d行丢失 ' : ' 。\n", line);
	}break;
	case 24: {
		printf("语法错误：第%d行丢失可枚举常量。\n", line);
	}break;
	}
}

//判断是否为字母。
int isLetter() {
	if (now_C >= 'A'&&now_C <= 'Z' || now_C >= 'a'&&now_C <= 'z' || now_C == '_')
		return 1;
	else return 0;
}

//判断是否为数字。
int isDigit() {
	if (now_C >= '0'&&now_C <= '9')
		return 1;
	else return 0;
}

//判断是否为无符号整数。
int isIntNum() {
	if (isDigit()) {
		catToken();
		if (now_C == '0') {
			now_C = fgetc(f_in);
			if (isDigit()) {
				retract();
				return 0;
			}//开头为0的数字整数，不合法，报错。
			else {
				retract();
				return 1;
			}//数字0。
		}
		else {
			now_C = fgetc(f_in);
			while (isDigit()) {
				catToken();
				now_C = fgetc(f_in);
			}
			retract();
			return 1;
		}
	}//不带正负号的整数。
	else return 0;
}

//获取字符并判断字符类型以及后续的一些列操作。
void getsym() {
	flag_error = 0;
	clearToken();
	now_C = fgetc(f_in);
	while (now_C == ' ' || now_C == '\n' || now_C == '\t') {
		if (now_C == '\n') {
			line++;
			//printf("Line: %d\n", line);
		}
		now_C = fgetc(f_in);
	}
	//判断是否为标识符或保留字。
	if (isLetter()) {
		while (isLetter() || isDigit()) {
			catToken();
			now_C = fgetc(f_in);
		}
		retract();
		int resultValue = isReserve();
		if (resultValue == -1) {
			symbol = IDSY;
			strlwr(Token);
		}
		else symbol = resultValue;
	}
	//判断是否为大于或大于等于。
	else if (now_C == '>') {
		catToken();
		now_C = fgetc(f_in);
		if (now_C == '=') {
			catToken();
			symbol = BIGESY;
		}//大于等于号
		else {
			retract();
			symbol = BIGSY;
		}//大于号
	}
	//判断是否为小于或小于等于。
	else if (now_C == '<') {
		catToken();
		now_C = fgetc(f_in);
		if (now_C == '=') {
			catToken();
			symbol = LESSESY;
		}//小于等于号
		else {
			retract();
			symbol = LESSSY;
		}//小于号
	}
	//判断是否为不等。
	else if (now_C == '!') {
		catToken();
		now_C = fgetc(f_in);
		if (now_C == '=') {
			catToken();
			symbol = UNEQUALSY;
		}
		else {
			retract();
			error();
		}
	}
	//判断是否为等号或赋值符号。
	else if (now_C == '=') {
		catToken();
		now_C = fgetc(f_in);
		if (now_C == '=') {
			catToken();
			symbol = EQUALSY;
		}
		else {
			symbol = EVALUATESY;
			retract();
		}
	}
	//判断是否为逗号。
	else if (now_C == ',') {
		catToken();
		symbol = COMMASY;
	}
	//判断是否为分号。
	else if (now_C == ';') {
		catToken();
		symbol = SEMISY;
	}
	//判断是否为字符。
	else if (now_C == 39) {
		now_C = fgetc(f_in);
		if (isDigit() || isLetter() || now_C == '+' || now_C == '-' || now_C == '*' || now_C == '/') {
			catToken();
			now_C = fgetc(f_in);
			if (now_C == 39) {
				symbol = CHARACTERSY;
			}
			else error();
		}
		else {
			error();
			retract();
		}
	}
	//判断是否为字符串。
	else if (now_C == 34) {
		now_C = fgetc(f_in);
		while (now_C == 32 || now_C == 33 || now_C >= 35 && now_C <= 126) {
			catToken();
			now_C = fgetc(f_in);
		}
		if (now_C == 34) {
			symbol = STRINGSY;
		}
		else {
			retract();
			error();
		}
	}
	//判断是否为整数或实数。
	else if (isIntNum()) {
		now_C = fgetc(f_in);
		if (now_C == '.') {
			catToken();
			now_C = fgetc(f_in);
			while (isDigit()) {
				catToken();
				now_C = fgetc(f_in);
			}
			retract();
			symbol = FLOATNUMSY;
		}//实数。
		else {
			retract();
			symbol = INTNUMSY;
		}//整数。
	}
	//判断是否为(
	else if (now_C == '(') {
		catToken();
		symbol = LSY;
	}
	//判断是否为)
	else if (now_C == ')') {
		catToken();
		symbol = RSY;
	}
	//判断是否为{
	else if (now_C == '{') {
		catToken();
		symbol = BIGLSY;
	}
	//判断是否为}
	else if (now_C == '}') {
		catToken();
		symbol = BIGRSY;
	}
	//判断是否为[
	else if (now_C == '[') {
		catToken();
		symbol = MIDLSY;
	}
	//判断是否为]
	else if (now_C == ']') {
		catToken();
		symbol = MIDRSY;
	}
	//判断是否为:
	else if (now_C == ':') {
		catToken();
		symbol = COLONSY;
	}
	else if (now_C == '*') {
		catToken();
		symbol = MULTSY;
	}
	else if (now_C == '+') {
	catToken();
	now_C = fgetc(f_in);
	if (isIntNum()) {
		now_C = fgetc(f_in);
		if (now_C == '.') {
			catToken();
			now_C = fgetc(f_in);
			while (isDigit()) {
				catToken();
				now_C = fgetc(f_in);
			}
			retract();
			symbol = FLOATNUMSY;
		}//实数。
		else {
			retract();
			symbol = INTEGERSY;
		}//带正号的整数。
	}
	else {
		retract();
		symbol = PLUSSY;
	}
	}
	else if (now_C == '-') {
		catToken();
		now_C = fgetc(f_in);
		if (isIntNum()) {
			now_C = fgetc(f_in);
			if (now_C == '.') {
				catToken();
				now_C = fgetc(f_in);
				while (isDigit()) {
					catToken();
					now_C = fgetc(f_in);
				}
				retract();
				symbol = FLOATNUMSY;
			}//实数。
			else {
				retract();
				symbol = INTEGERSY;
			}//带负号的整数。
		}
		else {
			retract();
			symbol = MINUSSY;
		}
	}
	else if (now_C == '/') {
		catToken();
		symbol = DIVSY;
	}
	else error();
}

//表达式分析。（跳出函数时，token为下个符号串）
int express() {
	int judge, minus;
	express_type = minus = 0;
	if (symbol == PLUSSY || symbol == MINUSSY) {
		if (express_type < 1) {
			express_type = 1;
		}
		//如果是负号，先将op_stack栈顶元素变负，再存入栈顶。
		if (symbol == MINUSSY) {
			minus = 1;
			getsym();
		}
		else {
			getsym();
			minus = 0;
		}
	}
	if (!term()) {
		grammer_error(19);
		return 0;
	}
	if (minus) {
		PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
		strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[14]);
		PCODE[P_CODE_TOP]->Code_Num = 14;
		printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
		P_CODE_TOP++;
	}
	while (symbol == PLUSSY || symbol == MINUSSY) {
		if (express_type < 1) {
			express_type = 1;
		}
		if (symbol == PLUSSY) {
			judge = 1;
		}
		else {
			judge = 0;
		}
		getsym();
		if (!term()) {
			grammer_error(19);
			return 0;
		}
		PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
		if (judge) {
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[4]);
			PCODE[P_CODE_TOP]->Code_Num = 4;
		}
		else {
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[5]);
			PCODE[P_CODE_TOP]->Code_Num = 5;
		}
		printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
		P_CODE_TOP++;
	}
	return 1;
}

//项分析。（跳出函数时，token为下个符号串）
int term() {
	int judge;
	if (!factor())return 0;
	while (symbol == MULTSY || symbol == DIVSY) {
		if (express_type < 1) {
			express_type = 1;
		}
		//乘法产生乘法指令，除法产生除法指令。
		if (symbol == MULTSY) {
			judge = 1;
		}
		else {
			judge = 0;
		}
		getsym();
		if (!factor()) {
			grammer_error(18);
			return 0;
		}
		PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
		if (judge) {
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[6]);
			PCODE[P_CODE_TOP]->Code_Num = 6;
		}
		else {
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[7]);
			PCODE[P_CODE_TOP]->Code_Num = 7;
		}
		printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
		P_CODE_TOP++;
	}
	return 1;
}

//因子分析。（跳出函数时，token为下个符号串）
int factor() {
	char factor_type[3][10]= { "int","float","char" };
	struct Table *temp_func_struct;
	temp_func_struct = (struct Table*)malloc(sizeof(struct Table));
	//标识符情况。
	if (symbol == IDSY) {
	//产生一条PSN指令，将该变量的值放至op_stack栈顶。变量，数组，有返回值函数三者OP_2需独立计算，指令中的其他参数相同。
		if (!search(Token)) {
			printf("%s is undefined!\n", Token);
		}
		temp_func_struct = func_struct;
		now_C = fgetc(f_in);
		if (strcmp(temp_func_struct->type, factor_type[2]) == 0 && express_type <= 0) {
			express_type = 0;
		}//字符型。
		else if (strcmp(temp_func_struct->type, factor_type[0]) == 0 && express_type <= 1) {
			express_type = 1;
		}//整型。
		else if (strcmp(temp_func_struct->type, factor_type[1]) == 0 && express_type <= 2) {
			express_type = 2;
		}//实型。
		//数组。
		if (now_C == '[') {
			//先生成LDN指令，将基地址放入op_stack。
			PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[0]);
			PCODE[P_CODE_TOP]->Code_Num = 0;
			PCODE[P_CODE_TOP]->OP_1 = (float)search_addr;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
			getsym();
			if (!express()) {
				grammer_error(17);
				return 0;
			}
			//再生成ADD，计算要取值的数组元素地址。
			PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[4]);
			PCODE[P_CODE_TOP]->Code_Num = 4;
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			//判断是否超出数组上届。
			PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[27]);
			PCODE[P_CODE_TOP]->Code_Num = 27;
			PCODE[P_CODE_TOP]->OP_1 = (float)temp_func_struct->array_max + temp_func_struct->addr - 1;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
			if (symbol != MIDRSY) {
				grammer_error(15);
				return 0;
			}
			PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[26]);
			PCODE[P_CODE_TOP]->Code_Num = 26;
			PCODE[P_CODE_TOP]->OP_1 = (float)temp_func_struct->D_or_S;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
			now_C = fgetc(f_in);
			switch (now_C)
			{
			case '+':symbol = PLUSSY; break;
			case '-':symbol = MINUSSY; break;
			case '*':symbol = MULTSY; break;
			case '/':symbol = DIVSY; break;
			case';':symbol = SEMISY; break;
			default: {
				retract();
				getsym();
			}
			}
		}
		//有返回值函数调用。
		else if (now_C == '(') {
			if (!function_use()) {
				grammer_error(20);
				return 0;
			}
			//记录调用函数位置+2
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[23]);
			PCODE[P_CODE_TOP]->Code_Num = 23;
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			//设置跳转指令，跳至函数指令位置。
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[16]);
			PCODE[P_CODE_TOP]->Code_Num = 16;
			PCODE[P_CODE_TOP]->flag_jump = temp_func_struct->func_begin_addr;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
		}
		//变量。
		else {
			retract();
			//直接生成PSN指令。
			PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[1]);
			PCODE[P_CODE_TOP]->Code_Num = 1;
			PCODE[P_CODE_TOP]->OP_1 = (float)search_addr;
			PCODE[P_CODE_TOP]->OP_2 = (float)temp_func_struct->D_or_S;
			printf("%d:\t%s %d %d\n", P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1, (int)PCODE[P_CODE_TOP]->OP_2);
			P_CODE_TOP++;
			now_C = fgetc(f_in);
			switch (now_C)
			{
			case '+':symbol = PLUSSY; break;
			case '-':symbol = MINUSSY; break;
			case '*':symbol = MULTSY; break;
			case '/':symbol = DIVSY; break;
			case';':symbol = SEMISY; break;
			default: {
				retract();
				getsym();
			}
			}
		}
		return 1;
	}
	//整数实数字符情况。
	else if (symbol == INTNUMSY || symbol == FLOATNUMSY || symbol == CHARACTERSY||symbol==INTEGERSY) {
		//放栈顶（上面各种情况也相同，读完一项将值放入栈顶。）
		//产生一条LDN指令。
		if (symbol == INTNUMSY && express_type == 0)
			express_type = 1;
		else if (symbol == FLOATNUMSY && express_type < 2)
			express_type = 2;
		else if (symbol == CHARACTERSY && express_type == 0)
			express_type = 0;
		else if (symbol == INTEGERSY && express_type == 0)
			express_type = 1;
		PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
		strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[0]);
		PCODE[P_CODE_TOP]->Code_Num = 0;
		if (symbol == CHARACTERSY) PCODE[P_CODE_TOP]->OP_1 = (float)Token[0];
		else PCODE[P_CODE_TOP]->OP_1 = (float)atof(Token);
		printf("%d:\t%s %f\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->OP_1);
		P_CODE_TOP++;
		now_C = fgetc(f_in);
		switch (now_C)
		{
		case '+':symbol = PLUSSY; break;
		case '-':symbol = MINUSSY; break;
		case '*':symbol = MULTSY; break;
		case '/':symbol = DIVSY; break;
		case';':symbol = SEMISY; break;
		default: {
			retract();
			getsym();
		}
		}
		return 1;
	}
	//（表达式）情况。
	else if (symbol == LSY) {
		getsym();
		if (!express()) {
			grammer_error(17);
			return 0;
		}
		if (symbol != RSY) {
			grammer_error(8);
			return 0;
		}
		getsym();
		if (express_type == 0)express_type = 1;
		return 1;
	}
	//
	else return 0;
}

//有/无返回值函数调用。（跳出函数时，token为下个符号串）
//调用的时候，跳转至该函数指令。
int function_use() {
	struct Table *temp_func_struct;
	temp_func_struct = (struct Table*)malloc(sizeof(struct Table));
	int i = 0;
	temp_func_struct = func_struct;
	getsym();
	if (symbol != RSY) {
		if (express()) {
			if (express_type != temp_func_struct->para_type[i]) {
				printf("语法错误：第%d行，函数调用时实参与形参类型应匹配。\n", line);
			}
			//将每个读到的参数传到相应位置。
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[29]);
			PCODE[P_CODE_TOP]->Code_Num = 29;
			PCODE[P_CODE_TOP]->OP_1 = (float)temp_func_struct->para[i];
			i++;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++; 
			while (symbol == COMMASY) {
				getsym();
				if (!express()) {
					grammer_error(17);
					return 0;
				}
				if (express_type != temp_func_struct->para_type[i]) {
					printf("语法错误：第%d行，函数调用时实参与形参类型应匹配。\n", line);
				}
				//将每个读到的参数传到相应位置。
				PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[29]);
				PCODE[P_CODE_TOP]->Code_Num = 29;
				PCODE[P_CODE_TOP]->OP_1 = (float)temp_func_struct->para[i];
				i++;
				printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
				P_CODE_TOP++; 
			}
			if (temp_func_struct->para_num != i - 1) {
				printf("语法错误：第%d行，实参与形参个数不匹配。\n", line);
			}
		}
		PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
		strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[28]);
		PCODE[P_CODE_TOP]->Code_Num = 28;
		printf("%d:\t%s\n", P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
		P_CODE_TOP++;
		if (symbol != RSY) {
			grammer_error(8);
			return 0;
		}
	}
	now_C = fgetc(f_in);
	switch (now_C)
	{
	case '+':symbol = PLUSSY; break;
	case '-':symbol = MINUSSY; break;
	case '*':symbol = MULTSY; break;
	case '/':symbol = DIVSY; break;
	case';':symbol = SEMISY; break;
	default: {
		retract();
		getsym();
	}
	}
	return 1;
}

//常量说明分析。（整体都算声明语句）
void const_description() {
	strcpy(kind, Token);
	getsym();
	//判断是否为整型常量声明。
	if (symbol == INTSY) {
		strcpy(type, Token);
		do {
			getsym();
			if (symbol == IDSY) {
				strcpy(name, Token);
				getsym();
				if (symbol == EVALUATESY)getsym();//判断是否为等号。
				else {
					grammer_error(0);
					break;
				}
				if (symbol == INTNUMSY || symbol == INTEGERSY) static_run_stack[P_run_stack] = (float)atoi(Token);//判断是否为整数。
				else grammer_error(1);
			}//判断是否为标识符（整型情况）。
			else {
				grammer_error(2);
				break;
			}
			insert(0);
			getsym();
		} while (symbol == COMMASY);
		if (symbol == SEMISY) return;
		else grammer_error(3);
	}
	//判断是否为实型常量声明。
	else if (symbol == FLOATSY) {
		strcpy(type, Token);
		do {
			getsym();
			if (symbol == IDSY) {
				strcpy(name, Token);
				getsym();
				if (symbol == EVALUATESY)getsym();//判断是否为等号。
				else {
					grammer_error(0);
					break;
				}
				if (symbol == FLOATNUMSY) static_run_stack[P_run_stack] = (float)atof(Token);//判断是否为整数。
				else grammer_error(4);
			}//判断是否为标识符（实型情况）。
			else {
				grammer_error(2);
				break;
			}
			insert(0);
			getsym();
		} while (symbol == COMMASY);
		if (symbol == SEMISY) return;
		else grammer_error(3);
	}
	//判断是否为字符型常量声明。
	else if (symbol == CHARSY) {
		strcpy(type, Token);
		do {
			getsym();
			if (symbol == IDSY) {
				strcpy(name, Token);
				getsym();
				if (symbol == EVALUATESY)getsym();//判断是否为等号。
				else {
					grammer_error(0);
					break;
				}
				if (symbol == CHARACTERSY) {
					static_run_stack[P_run_stack] = (float)Token[0];
					//printf("This is a \"const descritption - char\" statement.\n");
				}//判断是否为整数。
				else grammer_error(5);
			}//判断是否为标识符（整型情况）。
			else {
				grammer_error(2);
				break;
			}
			insert(0);
			getsym();
		} while (symbol == COMMASY);
		if (symbol == SEMISY) return;
		else grammer_error(3);
	}
	else grammer_error(6);
}

//变量说明分析。（整体都算声明语句）
void var_description() {
	char array_kind[] = { "array" };
	char var_kind[] = { "var" };
	//判断是否为逗号，即由一般变量声明进入。
	if (symbol == COMMASY) {
		do
		{
			getsym();
			if (symbol == IDSY) {
				strcpy(name, Token);
				getsym();
			}
			else {
				grammer_error(2);
				return;
			}
			//判断是否为数组。
			if (symbol == MIDLSY) {
				strcpy(kind, array_kind);
				getsym();
				if (symbol == INTNUMSY) {
					array_max = atoi(Token);
					getsym();
					if (symbol == MIDRSY) {
						if (fun_flag == 0)
							insert(0);
						else insert(1);
						//printf("This is a \"array variable description\" statement.\n");
						getsym();
					}
					else grammer_error(15);
				}
				else grammer_error(1);
			}
			else {
				strcpy(kind, var_kind);
				if (fun_flag == 0)
					insert(0);
				else insert(1);
				//printf("This is a \"variable description\" statement.\n");
			}
		} while (symbol == COMMASY);
	}
	//判断是否为 ' [ ' ，即由数组变量声明进入。
	else if (symbol == MIDLSY) {
		strcpy(kind, array_kind);
		getsym();
		if (symbol == INTNUMSY) {
			array_max = atoi(Token);
			getsym();
		}
		else {
			grammer_error(1);
			return;
		}
		if (symbol != MIDRSY) {
			grammer_error(15);
			return;
		}
		if (fun_flag == 0)
			insert(0);
		else insert(1);
		getsym();
		if (symbol != COMMASY) {
			if (symbol == SEMISY)return;
			else grammer_error(16);
		}
		do
		{
			getsym();
			if (symbol == IDSY) {
				strcpy(name, Token);
				getsym();
			}
			else {
				grammer_error(2);
				return;
			}
			//判断是否为数组。
			if (symbol == MIDLSY) {
				getsym();
				if (symbol == INTNUMSY) {
					array_max = atoi(Token);
					getsym();
					if (symbol == MIDRSY) {
						strcpy(kind, array_kind);
						if (fun_flag == 0)
							insert(0);
						else insert(1);
						//printf("This is a \"array variable description\" statement.\n");
						getsym();
					}
					else grammer_error(15);
				}
				else grammer_error(1);
			}
			else {
				strcpy(kind, var_kind);
				if (fun_flag == 0)
					insert(0);
				else insert(1);
				//printf("This is a \"variable description\" statement.\n");
				getsym();
			}
		} while (symbol == COMMASY);
	}
	//以上两者皆不是。
	else {
		grammer_error(13);
		return;
	}
	//判断声明结束后是否为分号。
	if (symbol != SEMISY) {
		grammer_error(3);
	}
	return;
}

//复合语句分析。
void compound_statement() {
	char array_kind[] = { "array" };
	char var_kind[] = { "var" };
	//判断复合语句中是否存在常量声明。
	while (symbol == CONSTSY) {
		const_description();
		getsym();
	}
	//判断复合语句中是否存在变量声明。
	while (symbol == INTSY || symbol == FLOATSY || symbol == CHARSY) {
		strcpy(type, Token);
		getsym();
		if (symbol == IDSY) {
			strcpy(name, Token);
			getsym();
			if (symbol == COMMASY) {
				strcpy(kind, var_kind);
				if (fun_flag == 0)insert(0);
				else insert(1);
				//printf("This is a \"variable description\" statement.\n");
				var_description();
				getsym();
			}
			else if (symbol == MIDLSY) {
				strcpy(kind, array_kind);
				getsym();
				if (symbol == INTNUMSY) {
					array_max = atoi(Token);
					getsym();
					if (symbol == MIDRSY) {
						if (fun_flag == 0)insert(0);
						else insert(1);
						//printf("This is a \"array variable description\" statement.\n");
						getsym();
					}
					else grammer_error(15);
				}
				else grammer_error(1);
				if (symbol == COMMASY) {
					var_description();
					getsym();
				}
				else if (symbol == SEMISY) {
					getsym();
				}
			}
			else if (symbol == SEMISY) {
				strcpy(kind, var_kind);
				if (fun_flag == 0)insert(0);
				else insert(1);
				getsym();
				//printf("This is a \"variable description\" statement.\n");
			}
		}
		else grammer_error(2);
	}
	//判断复合语句中的语句列。
	id_use_flag = 1;
	sentence_list();
	id_use_flag = 0;
}

//if，while条件分析。
int condition() {
	int judge, pre_type;
	pre_type = 0;
	if (express()) {
		if (symbol == LESSSY || symbol == BIGSY || symbol == BIGESY || symbol == LESSESY || symbol == EQUALSY || symbol == UNEQUALSY) {
			if (express_type == 0)printf("语法错误：第%d行，条件语句关系比较时不能包含字符。\n", line);
			else pre_type = express_type;
			//根据symbol产生指令。
			switch (symbol)
			{
			case BIGSY: {
				judge = 0;
			}
				break;
			case LESSSY: {
				judge = 1;
			}
				break;
			case BIGESY: {
				judge = 2;
			}
				break;
			case LESSESY: {
				judge = 3;
			}
				break;
			case EQUALSY: {
				judge = 4;
			}
				break;
			case UNEQUALSY: {
				judge = 5;
			}
				break;
			}
			getsym();
			if (!express()) {
				grammer_error(17);
				return 0;
			}
			if (express_type == 0)printf("语法错误：第%d行，条件语句关系比较时不能包含字符。\n", line);
			else if (pre_type != 0) {
				if (pre_type != express_type)
					printf("语法错误：第%d行，条件语句关系比较要求类型完全匹配。\n", line);
			}
			if (symbol != RSY) {
				grammer_error(8);
				return 0;
			}
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			switch (judge)
			{
			case 0: {
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[8]);
				PCODE[P_CODE_TOP]->Code_Num = 8;
			}
						break;
			case 1: {
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[9]);
				PCODE[P_CODE_TOP]->Code_Num = 9;
			}
						 break;
			case 2: {
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[10]);
				PCODE[P_CODE_TOP]->Code_Num = 10;
			}
						 break;
			case 3: {
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[11]);
				PCODE[P_CODE_TOP]->Code_Num = 11;
			}
						  break;
			case 4: {
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[12]);
				PCODE[P_CODE_TOP]->Code_Num = 12;
			}
						  break;
			case 5: {
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[13]);
				PCODE[P_CODE_TOP]->Code_Num = 13;
			}
							break;
			}
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			return 1;
		}
		else {
			if (express_type != 1) {
				printf("语法错误：第%d行，if ‘(’＜条件＞‘)’和while ‘(’＜条件＞‘)’里边，如果<条件>是单个表达式，则必须是整型。\n", line);
			}
			else return 1;
		}
	}
	else {
		grammer_error(17);
		return 0;
	}
}

//语句列分析。
void sentence_list() {
	while (symbol==SEMISY||symbol == IFSY || symbol == WHILESY || symbol == SWITCHSY || symbol == SCANFSY || symbol == PRINTFSY || symbol == RETURNSY || symbol == LSY || symbol == IDSY || symbol == BIGLSY) {
		sentence();
	}
	return;
}

//语句分析。
void sentence() {
	char const_kind[] = { "const" };
	char id_type[3][10] = { "int","float","char" };
	struct Table *temp_func_struct;
	int flag_if_jump, flag_while_begin, flag_while_jump, flag_case_jump, flag_func_jump, flag_if_end, flag_para;
	int i = 0;
	int j, h;
	int case_value, case_type;
	char case_condition[100][100];
	flag_para = 0;
	temp_func_struct= (struct Table*)malloc(sizeof(struct Table));
	switch (symbol)
	{
	case SEMISY: {
		//printf("This is a \"empty\" statement.\n");
		getsym();
	}break;//空语句。
	case IFSY: {
		flag_if_jump = P_CODE_TOP;
		getsym();
		if (symbol == LSY) {
			getsym();
			if (!condition()) {
				grammer_error(21);
			}
			//产生ZJP指令
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[17]);
			PCODE[P_CODE_TOP]->Code_Num = 17;
			PCODE[P_CODE_TOP]->flag_jump = flag_if_jump;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++; 
			getsym();
			sentence();
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[16]);
			PCODE[P_CODE_TOP]->Code_Num = 16;
			PCODE[P_CODE_TOP]->flag_jump = P_CODE_TOP;
			flag_if_end = P_CODE_TOP;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
			//产生标签
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[15]);
			PCODE[P_CODE_TOP]->Code_Num = 15;
			PCODE[P_CODE_TOP]->flag_jump = flag_if_jump;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
			if (symbol == ELSESY) {
				getsym();
				sentence();
			}
			//产生标签
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[15]);
			PCODE[P_CODE_TOP]->Code_Num = 15;
			PCODE[P_CODE_TOP]->flag_jump = flag_if_end;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
		}
		else {
			grammer_error(7);
			return;
		}
		//printf("This is a \"if\"statement.\n");
	}break;//条件语句。
	case WHILESY: {
		getsym();
		if (symbol == LSY) {
			flag_while_begin = P_CODE_TOP;
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[15]);
			PCODE[P_CODE_TOP]->Code_Num = 15;
			PCODE[P_CODE_TOP]->flag_jump = flag_while_begin;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
			getsym();
			if (!condition()) {
				grammer_error(21);
			}
			flag_while_jump = P_CODE_TOP;
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[17]);
			PCODE[P_CODE_TOP]->Code_Num = 17;
			PCODE[P_CODE_TOP]->flag_jump = flag_while_jump;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
			getsym();
		}
		else {
			grammer_error(7);
			return;
		}
		sentence();
		PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
		strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[16]);
		PCODE[P_CODE_TOP]->Code_Num = 16;
		PCODE[P_CODE_TOP]->flag_jump = flag_while_begin;
		printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
		P_CODE_TOP++;
		PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
		strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[15]);
		PCODE[P_CODE_TOP]->Code_Num = 15;
		PCODE[P_CODE_TOP]->flag_jump = flag_while_jump;
		printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
		P_CODE_TOP++; 
		//printf("This is a \"while\"statement.\n");
	}break;//循环语句。
	case SWITCHSY: {
		getsym();
		if (symbol != LSY) {
			grammer_error(7);
			return;
		}
		getsym();
		if (!express()) {
			grammer_error(17);
			return;
		}
		case_type = express_type;
		if (express_type == 2) {
			printf("语法错误：第%d行，switch语句条件不应为实型。\n", line);
		}
		if (symbol != RSY) {
			grammer_error(8);
			return;
		}
		getsym();
		if (symbol != BIGLSY) {
			grammer_error(9);
			return;
		}
		getsym();
		do
		{
			flag_case_jump = P_CODE_TOP;
			if (symbol != CASESY) {
				grammer_error(22);
				return;
			}
			getsym();
			if (symbol != INTNUMSY && symbol != CHARACTERSY&&symbol!=MINUSSY&&symbol!=PLUSSY&&symbol!=INTEGERSY) {
				grammer_error(24);
				return;
			}
			if ((case_type == 0 && symbol != CHARACTERSY) || (case_type == 1 &&( symbol != INTEGERSY && symbol != INTNUMSY))) {
				printf("语法错误：第%d行，switchcase语句条件类型应匹配。\n", line);
			}
			j = 0;
			if (symbol == INTNUMSY||symbol==INTEGERSY) {
				case_value = atoi(Token);
			}
			else if (symbol == CHARACTERSY) {
				case_value = Token[0];
			}
			while (j < i) {
				if (strcmp(case_condition[j],Token)!=0)j++;
				else {
					printf("line %d case condition is repeat!\n",line);
					break;
				}
			}//判断case条件是否重复。

			i++;
			//产生LDN指令，将枚举型常量的值放入op_stack栈顶
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[0]);
			PCODE[P_CODE_TOP]->Code_Num = 0;
			PCODE[P_CODE_TOP]->OP_1 = (float)case_value;
			printf("%d:\t%s %f\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
			//产生EQL指令，对op_stack栈顶和次栈顶两个元素进行比较。
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[30]);
			PCODE[P_CODE_TOP]->Code_Num = 30;
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			//产生ZJP指令，以上判断若为0，跳转。
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[17]);
			PCODE[P_CODE_TOP]->Code_Num = 17;
			PCODE[P_CODE_TOP]->flag_jump = flag_case_jump;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name,PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
			getsym();
			if (symbol != COLONSY) {
				grammer_error(23);
				return;
			}
			getsym();
			sentence();
			//产生跳转指令，执行完语句跳出switch
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[16]);
			PCODE[P_CODE_TOP]->Code_Num = 16;
			PCODE[P_CODE_TOP]->flag_jump = switch_jump_end;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
			//设置标签，case条件不成立跳至此。
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[15]);
			PCODE[P_CODE_TOP]->Code_Num = 15;
			PCODE[P_CODE_TOP]->flag_jump = flag_case_jump;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++; 
			//printf("This is a \"case\" statement.\n");
		} while (symbol != DEFAULTSY&&symbol!=BIGRSY);
		if (symbol == DEFAULTSY) {
			getsym();
			if (symbol != COLONSY) {
				grammer_error(23);
				return;
			}
			getsym();
			sentence();
			//printf("This is a \"default\" statement.\n");
		}
		else if (symbol != BIGRSY) {
			grammer_error(10);
			return;
		}
		PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
		strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[15]);
		PCODE[P_CODE_TOP]->Code_Num = 15;
		PCODE[P_CODE_TOP]->flag_jump = switch_jump_end;
		printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
		P_CODE_TOP++; 
		getsym();
		switch_jump_end--;
		//printf("This is a \"switch\" statement.\n");
	}break;//情况语句。
	case SCANFSY: {
		getsym();
		if(symbol!=LSY){
			grammer_error(7);
			return;
		}
		do
		{
			getsym();
			if(!search(Token))printf("%s is undefined!\n",Token);
			if (symbol != IDSY) {
				grammer_error(2);
				return;
			}
			//先生成SCF指令，将值读入op_stack。
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[18]);
			PCODE[P_CODE_TOP]->Code_Num = 18;
			if (strcmp(func_struct->type, id_type[0]) == 0) {
				PCODE[P_CODE_TOP]->OP_1 = 1;
			}
			else if (strcmp(func_struct->type, id_type[1]) == 0) {
				PCODE[P_CODE_TOP]->OP_1 = 2;
			}
			else if (strcmp(func_struct->type, id_type[2]) == 0) {
				PCODE[P_CODE_TOP]->OP_1 = 0;
			}
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
			//再生成TKN指令，存入运行栈。
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[2]);
			PCODE[P_CODE_TOP]->Code_Num = 2;
			PCODE[P_CODE_TOP]->OP_1 = func_struct->addr;
			PCODE[P_CODE_TOP]->OP_2 = func_struct->D_or_S;
			printf("%d:\t%s %d, %d\n", P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1, (int)PCODE[P_CODE_TOP]->OP_2);
			P_CODE_TOP++;
			getsym();
		} while (symbol == COMMASY);
		if (symbol != RSY) {
			grammer_error(8);
			return;
		}
		getsym();
		if (symbol != SEMISY) {
			grammer_error(3);
			return;
		}
		//printf("This is a \"scanf\" statement.\n");
	}break;//读语句。
	case PRINTFSY: {
		getsym();
		if (symbol != LSY) {
			grammer_error(7);
			return;
		}
		getsym();
		if (symbol == STRINGSY) {
			strcpy(string_array[P_string], Token);
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[19]);
			PCODE[P_CODE_TOP]->Code_Num = 19;
			PCODE[P_CODE_TOP]->OP_1 = (float)P_string;
			P_string++;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
			getsym();
			if (symbol == COMMASY) {
				getsym();
				if (!express()) {
					grammer_error(17);
					return;
				}
				PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[20]);
				PCODE[P_CODE_TOP]->Code_Num = 20;
				PCODE[P_CODE_TOP]->OP_1 = (float)express_type;
				printf("%d:\t%s %d\n", P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
				P_CODE_TOP++;
			}
		}
		else if (express()) {
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[20]);
			PCODE[P_CODE_TOP]->Code_Num = 20;
			PCODE[P_CODE_TOP]->OP_1 = (float)express_type;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
		}
		if (symbol != RSY) {
			grammer_error(8);
			return;
		}
		getsym();
		if (symbol != SEMISY) {
			grammer_error(3);
			return;
		}
		getsym();
		//printf("This is a \"printf\" statement.\n");
	}break;//写语句。
	case RETURNSY: {
		getsym();
		if (void_or_not) {
			if (main_flag) {
				getsym();
			}
			else {
				if (symbol != SEMISY) {
					printf("non-return function shouldn't have return value!\n");
				}
				else {
					PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
					strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[21]);
					PCODE[P_CODE_TOP]->Code_Num = 21;
					printf("%d:\t%s\n", P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
					P_CODE_TOP++;
					getsym();
				}
			}
		}
		else {
			if (symbol == SEMISY) {
				PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[21]);
				PCODE[P_CODE_TOP]->Code_Num = 21;
				printf("%d:\t%s\n", P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
				P_CODE_TOP++;
				getsym();
			}
			else {
						if (symbol != LSY) {
							grammer_error(7);
							return;
						}
						getsym();
						if (!express()) {
							grammer_error(17);
							return;
						}
						if (symbol != RSY) {
							grammer_error(8);
							return;
						}
						getsym();
						if (symbol != SEMISY) {
							grammer_error(3);
							return;
						}
						PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
						strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[21]);
						PCODE[P_CODE_TOP]->Code_Num = 21;
						printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
						P_CODE_TOP++;
						getsym();
					}
		}
		
		//printf("This is a \"return\" statement.\n");
	}break;//返回语句。
	case LSY: {
		getsym();
		if (!express()) {
			grammer_error(17);
			return;
		}
		if (symbol != RSY) {
			grammer_error(8);
			return;
		}
		getsym();
		//printf("This is a \"express\" statement.\n");
	}break;//表达式。
	case IDSY: {
		if (!search(Token))
			printf("%s is undefined!\n", Token);
		temp_func_struct = func_struct;
		getsym();
		if (strcmp(temp_func_struct->kind, const_kind) == 0) {
			printf("语法错误：第%d行，不能给常量赋值。\n", line);
		}
		if (symbol == EVALUATESY) {
			j = search_addr;
			h = UseVarInDorS;
			getsym();
			if (!express()) {
				grammer_error(17);
				return;
			}
			if ((strcmp(temp_func_struct->type, id_type[0])==0 && express_type != 1) || (strcmp(temp_func_struct->type, id_type[1]) == 0 && express_type != 2) || (strcmp(temp_func_struct->type, id_type[2]) == 0 && express_type != 0)) {
				printf("语法错误：第%d行赋值语句类型不匹配。\n", line);
			}
			if (symbol != SEMISY) {
				grammer_error(3);
				return;
			}
			//printf("This is a \"variable evaluate\" statement.\n");
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[2]);
			PCODE[P_CODE_TOP]->Code_Num = 2;
			PCODE[P_CODE_TOP]->OP_1 = (float)j;
			PCODE[P_CODE_TOP]->OP_2 = (float)h;
			printf("%d:\t%s %d,%d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1, (int)PCODE[P_CODE_TOP]->OP_2);
			P_CODE_TOP++;
			getsym();
		}
		else if (symbol == MIDLSY) {
			//LDN
			h = func_struct->D_or_S;
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[0]);
			PCODE[P_CODE_TOP]->Code_Num = 0;
			PCODE[P_CODE_TOP]->OP_1 = (float)search_addr;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
			getsym();
			if (!express()) {
				grammer_error(17);
				return;
			}
			if (symbol != MIDRSY) {
				grammer_error(15);
				return;
			}
			//ADD
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[4]);
			PCODE[P_CODE_TOP]->Code_Num = 4;
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			//判断是否超出数组上届。
			PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[27]);
			PCODE[P_CODE_TOP]->Code_Num = 27;
			PCODE[P_CODE_TOP]->OP_1 = (float)temp_func_struct->array_max + temp_func_struct->addr - 1;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
			getsym();
			if (symbol != EVALUATESY) {
				grammer_error(0);
				return;
			}
			getsym();
			if (!express()) {
				grammer_error(17);
				return;
			}
			//STA
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[3]);
			PCODE[P_CODE_TOP]->Code_Num = 3;
			PCODE[P_CODE_TOP]->OP_1 = h;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
			if (symbol != SEMISY) {
				grammer_error(3);
				return;
			}
			getsym();
			//printf("This is a \"array variable evaluate\" statement.\n");
		}
		else if (symbol == LSY) {
			flag_func_jump = P_CODE_TOP;
			getsym();
			//printf("%s:%d,%d\n", func_struct->name, func_struct->para[0], func_struct->para[1]);
			if (symbol!=RSY) {
				if (!express()) {
					printf("error\n");
					return;
				}
				if (express_type != temp_func_struct->para_type[i]) {
					printf("语法错误：第%d行，函数调用时实参与形参类型应匹配。\n", line);
					}
				//将每个读到的参数传到相应位置。
				PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[29]);
				PCODE[P_CODE_TOP]->Code_Num = 29;
				PCODE[P_CODE_TOP]->OP_1 = (float)temp_func_struct->para[i];
				i++;
				printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
				P_CODE_TOP++;
				while (symbol == COMMASY) {
					getsym();
					if (!express()) {
						grammer_error(17);
						return;
					}
					if (express_type != temp_func_struct->para_type[i]) {
						printf("语法错误：第%d行，函数调用时实参与形参类型应匹配。\n", line);
					}
					//将每个读到的参数传到相应位置。
					PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
					strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[29]);
					PCODE[P_CODE_TOP]->Code_Num = 29;
					PCODE[P_CODE_TOP]->OP_1 = (float)temp_func_struct->para[i];
					i++;
					printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
					P_CODE_TOP++; 
				}
				if (temp_func_struct->para_num != i - 1) {
					printf("语法错误：第%d行，实参与形参个数不匹配。\n", line);
				}
			}
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[28]);
			PCODE[P_CODE_TOP]->Code_Num = 28;
			printf("%d:\t%s\n", P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			if (symbol != RSY) {
				grammer_error(8);
				return;
			}
			getsym();
			if (symbol != SEMISY) {
				grammer_error(3);
				return;
			}
			//记录调用函数位置+2
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[23]);
			PCODE[P_CODE_TOP]->Code_Num = 23;
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			//设置跳转指令，跳至函数指令位置。
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[16]);
			PCODE[P_CODE_TOP]->Code_Num = 16;
			PCODE[P_CODE_TOP]->flag_jump = temp_func_struct->func_begin_addr;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
			//设置标签，调用函数执行完成之后返回该位置。
			getsym();
			//printf("This is a \"function use\" statement.\n");
		}
		else {
			grammer_error(13);
			return;
		}
	}break;//标识符。
	case BIGLSY: {
		getsym();
		sentence_list();
		if (symbol != BIGRSY) {
			grammer_error(10);
		}
		getsym();
	}break;//语句列。
	default:grammer_error(13);
		break;
	}
	return;
}

//有/无返回值函数定义。（开头算声明，注意形参）
void function_description() {
	struct Table *temp_func;
	int i = 0;
	int now_para_type;
	temp_func = (struct Table *)malloc(sizeof(struct Table));
	temp_func = myTable;
	char var_kind[] = { "var" };
	lvlup();
	getsym();
	//判断形参。
	if (symbol == INTSY || symbol == FLOATSY || symbol == CHARSY) {
		if (symbol == INTSY) {
			now_para_type = 1;
		}
		else if (symbol == FLOATSY) {
			now_para_type = 2;
		}
		else if (symbol == CHARSY) {
			now_para_type = 0;
		}
		strcpy(type, Token);
		getsym();
		if (symbol == IDSY) {
			strcpy(name, Token);
			strcpy(kind, var_kind);
			insert(1);
			search(Token);
			temp_func->para[i] = search_addr;
			temp_func->para_type[i] = now_para_type;
			getsym();
			while (symbol ==COMMASY) {
				i++;
				getsym();
				if (symbol != INTSY && symbol != FLOATSY && symbol != CHARSY) {
					grammer_error(6);
					return;
				}
				if (symbol == INTSY) {
					now_para_type = 1;
				}
				else if (symbol == FLOATSY) {
					now_para_type = 2;
				}
				else if (symbol == CHARSY) {
					now_para_type = 0;
				}
				strcpy(type, Token);
				getsym();
				if (symbol != IDSY) {
					grammer_error(2);
					return;
				}
				strcpy(name, Token);
				insert(1);
				search(Token);
				temp_func->para[i] = search_addr;
				temp_func->para_type[i] = now_para_type;
				getsym();
			}
			temp_func->para_num = i;
		}
	}
	if (symbol != RSY) {
		grammer_error(8);
		return;
	}
	getsym();
	if (symbol != BIGLSY) {
		grammer_error(8);
		return;
	}
	getsym();
	temp_func->func_begin_addr = P_CODE_TOP;
	PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
	strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[15]);
	PCODE[P_CODE_TOP]->Code_Num = 15;
	PCODE[P_CODE_TOP]->flag_jump = P_CODE_TOP;
	printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
	P_CODE_TOP++;
	compound_statement();
	if (symbol != BIGRSY) {
		grammer_error(10);
		return;
	}
	//返回函数调用位置
	PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
	strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[22]);
	PCODE[P_CODE_TOP]->Code_Num = 22;
	printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
	P_CODE_TOP++;
	lvlback();
	//printf("This is a \"function description\" statement.\n");
}

//主函数分析。
void main_function() {
	main_flag = 1;
	lvlup();
	getsym();
	if (symbol != LSY) {
		grammer_error(7);
		return;
	}
	getsym();
	if (symbol != RSY) {
		grammer_error(8);
		return;
	}
	getsym();
	if (symbol != BIGLSY) {
		grammer_error(9);
		return;
	}
	getsym();
	compound_statement();
	if (symbol != BIGRSY) {
		grammer_error(10);
		return;
	}
	now_C = fgetc(f_in);
	now_C = fgetc(f_in);
	//printf("This is a main function.\n");
}

//判断是否为程序中的一部分(常量说明，变量说明，有/无返回值函数，主函数)。
void program() {
	char var_kind[] = { "var" };
	char func_kind[] = { "return-function" };
	char nfunc_kind[] = { "noreturn-function" };
	char void_type[] = { "type" };
	//先将symbol存入符号表。
	//判断是否为常量说明。
	if (symbol == CONSTSY) {
		if (flag_step_const != 1)
			grammer_error(11);
		const_description();
		return;
	}
	//判断是否为变量说明或有返回值函数定义。
	else if (symbol == INTSY || symbol == FLOATSY || symbol == CHARSY) {
		strcpy(type, Token);
		flag_step_const = 0;
		getsym();
		if (symbol == IDSY) {
			strcpy(name, Token);
			getsym();//先将名称存入符号表。在根据下面symbol的类型做相应操作。
			switch (symbol)
			{
			case COMMASY: {
				if (flag_step_var != 1)
					grammer_error(12);
				strcpy(kind, var_kind);
				insert(0);
				//printf("This is a \"variable description\" statement.\n");
				var_description();
			}break;//多个变量声明（非数组）。
			case MIDLSY: {
				if (flag_step_var != 1)
					grammer_error(12);
				var_description();
			}break;//多个变量声明（数组）。
			case SEMISY: {
				if (flag_step_var != 1)
					grammer_error(12);
				strcpy(kind, var_kind);
				insert(0);
				//printf("This is a \"variable description\" statement.\n");
			}break;//单个变量声明。
			case LSY: {
				if (flag_step_function != 1)
					grammer_error(14);
				flag_step_var = 0;
				fun_flag = 1;
				void_or_not = 0;
				strcpy(kind, func_kind);
				var_distance = 0;
				insert(1);
				function_description();
			}break;//有返回值函数定义。
			default:grammer_error(13);
				break;
			}
		}
	}
	//判断是否为无返回值函数定义或主函数。
	else if (symbol == VOIDSY) {
		void_or_not = 1;
		strcpy(type, void_type);
		flag_step_const = flag_step_var = 0;
		getsym();
		//主函数。
		if (symbol == MAINSY) {
			fun_flag = 0;
			flag_step_function = 0;
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[24]);
			PCODE[P_CODE_TOP]->Code_Num = 24;
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			main_function();
		}
		//无返回值函数定义。（存入符号表）
		else if (symbol == IDSY) {
			var_distance = 0;
			fun_flag = 1;
			strcpy(name, Token);
			if (flag_step_function != 1)
				grammer_error(14);
			getsym();
			if (symbol == LSY) {
				strcpy(kind, nfunc_kind);
				insert(0);
				function_description();
			}
			else {
				grammer_error(7);
				return;
			}
		}
	}
	//不是以上任何一种情况
	else grammer_error(13);
}

//PCODE解释执行程序。
void execute() {
	int stop,//程序停止运行信号
		exe_pcode,//当前执行指令地址。
		exe_pcode_no,//当前执行指令名称。
		op_num,//当前操作数。
		P_function_use_stack;//function_use_stack的指针。
	int i;
	int function_use_stack[100];
	char lab_judge[] = { "LAB" };
	char mbg_judge[] = { "MBG" };
	char fed_judge[] = { "FED" };
	char scanf_c;
	P_function_use_stack = exe_pcode = stop = 0;
	while (exe_pcode<P_CODE_TOP) {
		//获取指令序号
		exe_pcode_no = PCODE[exe_pcode]->Code_Num;
/*
注意：
1.P_op_stack指向位置默认为空。
*/
		switch (exe_pcode_no)
		{
		case 0: {
			op_stack[P_op_stack]=PCODE[exe_pcode]->OP_1;
			P_op_stack++;
			exe_pcode++;
		}break;//LDN
		case 1: {
			if (PCODE[exe_pcode]->OP_2 == 0)
				op_stack[P_op_stack] = static_run_stack[(int)PCODE[exe_pcode]->OP_1];
			else
				op_stack[P_op_stack] = dynamic_run_stack[P_d_run_stack][(int)PCODE[exe_pcode]->OP_1];
			P_op_stack++;
			exe_pcode++;
		}break;//PSN
		case 2: {
			if (PCODE[exe_pcode]->OP_2 == 0)
				static_run_stack[(int)PCODE[exe_pcode]->OP_1] = op_stack[P_op_stack - 1];
			else
				dynamic_run_stack[P_d_run_stack][(int)PCODE[exe_pcode]->OP_1] = op_stack[P_op_stack - 1];
			op_stack[P_op_stack - 1] = 0;
			P_op_stack--;
			exe_pcode++;
		}break;//TKN
		case 3: {
			if(PCODE[exe_pcode]->OP_1 == 0)
				static_run_stack[(int)op_stack[P_op_stack - 2]] = op_stack[P_op_stack - 1];
			else
				dynamic_run_stack[P_d_run_stack][(int)op_stack[P_op_stack - 2]] = op_stack[P_op_stack - 1];
			op_stack[P_op_stack - 1] = 0;
			P_op_stack = P_op_stack - 2;
			exe_pcode++;
		}break;//STA
		case 4: {
			op_stack[P_op_stack - 2] = op_stack[P_op_stack - 2] + op_stack[P_op_stack - 1];
			op_stack[P_op_stack - 1] = 0;
			P_op_stack--;
			exe_pcode++;
		}break;//ADD
		case 5: {
			op_stack[P_op_stack - 2] = op_stack[P_op_stack - 2] - op_stack[P_op_stack - 1];
			op_stack[P_op_stack - 1] = 0;
			P_op_stack--;
			exe_pcode++;
		}break;//SUB
		case 6: {
			op_stack[P_op_stack - 2] = op_stack[P_op_stack - 2] * op_stack[P_op_stack - 1];
			op_stack[P_op_stack - 1] = 0;
			P_op_stack--;
			exe_pcode++;
		}break;//MUT
		case 7: {
			op_stack[P_op_stack - 2] = op_stack[P_op_stack - 2] / op_stack[P_op_stack - 1];
			op_stack[P_op_stack - 1] = 0;
			P_op_stack--;
			exe_pcode++;
		}break;//DIV
		case 8: {
			if (op_stack[P_op_stack - 2] > op_stack[P_op_stack - 1]) {
				op_stack[P_op_stack - 2] = 1;
				op_stack[P_op_stack - 1] = 0;
				P_op_stack--;
			}
			else {
				op_stack[P_op_stack - 2] = 0;
				op_stack[P_op_stack - 1] = 0;
				P_op_stack--;
			}
			exe_pcode++;
		}break;//BIG
		case 9: {
			if (op_stack[P_op_stack - 2] < op_stack[P_op_stack - 1]) {
				op_stack[P_op_stack - 2] = 1;
				op_stack[P_op_stack - 1] = 0;
				P_op_stack--;
			}
			else {
				op_stack[P_op_stack - 2] = 0;
				op_stack[P_op_stack - 1] = 0;
				P_op_stack--;
			}
			exe_pcode++;
		}break;//LES
		case 10: {
			if (op_stack[P_op_stack - 2] >= op_stack[P_op_stack - 1]) {
				op_stack[P_op_stack - 2] = 1;
				op_stack[P_op_stack - 1] = 0;
				P_op_stack--;
			}
			else {
				op_stack[P_op_stack - 2] = 0;
				op_stack[P_op_stack - 1] = 0;
				P_op_stack--;
			}
			exe_pcode++;
		}break;//BOE
		case 11: {
			if (op_stack[P_op_stack - 2] <= op_stack[P_op_stack - 1]) {
				op_stack[P_op_stack - 2] = 1;
				op_stack[P_op_stack - 1] = 0;
				P_op_stack--;
			}
			else {
				op_stack[P_op_stack - 2] = 0;
				op_stack[P_op_stack - 1] = 0;
				P_op_stack--;
			}
			exe_pcode++;
		}break;//LOE
		case 12: {
			if (op_stack[P_op_stack - 2] == op_stack[P_op_stack - 1]) {
				op_stack[P_op_stack - 2] = 1;
				op_stack[P_op_stack - 1] = 0;
			}
			else {
				op_stack[P_op_stack - 2] = 0;
				op_stack[P_op_stack - 1] = 0;
			}
			P_op_stack--;
			exe_pcode++;
		}break;//EQL
		case 13: {
			if (op_stack[P_op_stack - 2] != op_stack[P_op_stack - 1]) {
				op_stack[P_op_stack - 2] = 1;
				op_stack[P_op_stack - 1] = 0;
				P_op_stack--;
			}
			else {
				op_stack[P_op_stack - 2] = 0;
				op_stack[P_op_stack - 1] = 0;
				P_op_stack--;
			}
			exe_pcode++;
		}break;//UEQ
		case 14: {
			op_stack[P_op_stack - 1] = 0 - op_stack[P_op_stack - 1];
			exe_pcode++;
		}break;//MIN
		case 15: {
			exe_pcode++;
		}break;//LAB
		case 16: {
			i = 0;
			while (i<P_CODE_TOP){
				if (strcmp(lab_judge, PCODE[i]->Code_Name) == 0 && (PCODE[i]->flag_jump == PCODE[exe_pcode]->flag_jump)) {
					exe_pcode = i;
					break;
				}
				else i++;
			}
			if (i == P_CODE_TOP)
				printf("jump nowhere!\n");
			exe_pcode++;
		}break;//JMP
		case 17: {
			if ((int)op_stack[P_op_stack - 1] == 0) {
				i = 0;
				while (i < P_CODE_TOP) {
					if (strcmp(lab_judge, PCODE[i]->Code_Name) == 0 && (PCODE[i]->flag_jump == PCODE[exe_pcode]->flag_jump)) {
						exe_pcode = i;
						break;
					}
					else i++;
				}
				if (i == P_CODE_TOP)
					printf("jump nowhere!\n");
			}
			exe_pcode++;
			op_stack[P_op_stack - 1] = 0;
			P_op_stack--;
		}break;//ZJP
		case 18: {
			if (PCODE[exe_pcode]->OP_1 != 0) {
				scanf("%f", &op_stack[P_op_stack]);
			}
			else {
				scanf_c=getchar();
				op_stack[P_op_stack] = (float)scanf_c;
			}
			P_op_stack++;
			exe_pcode++;
		}break;//SCF 
		case 19: {
			printf("%s\n", string_array[(int)PCODE[exe_pcode]->OP_1]);
			exe_pcode++;
		}break;//PRS
		case 20: {
			if ((int)PCODE[exe_pcode]->OP_1 == 0) {
				printf("%c\n", (char)op_stack[P_op_stack - 1]);
			}
			else if ((int)PCODE[exe_pcode]->OP_1 == 1) {
				printf("%d\n", (int)op_stack[P_op_stack - 1]);
			}
			else if ((int)PCODE[exe_pcode]->OP_1 == 2) {
				printf("%f\n", op_stack[P_op_stack - 1]);
			}
			op_stack[P_op_stack - 1] = 0;
			P_op_stack--;
			exe_pcode++;
		}break;//PRE
		case 21: {
			i = exe_pcode;
			while (i < P_CODE_TOP) {
				if (PCODE[i]->Code_Num==22) {
					exe_pcode = i;
					break;
				}
				else i++;
			}
			if (i == P_CODE_TOP)
				printf("return nowhere!\n");
		}break;//RTN
		case 22: {
			exe_pcode = function_use_stack[P_function_use_stack - 1];
			P_d_run_stack--;
			P_function_use_stack--;
		}break;//FED
		case 23: {
			function_use_stack[P_function_use_stack] = exe_pcode + 2;
			P_function_use_stack++;
			exe_pcode++;
		}break;//FUS
		case 24: {
			
		}break;//MBG
		case 25: {
			i = 0;
			while (i<P_CODE_TOP) {
				if (strcmp(mbg_judge, PCODE[i]->Code_Name) == 0) {
					exe_pcode = i;
					break;
				}
				else i++;
			}
			if (i == P_CODE_TOP)printf("no main function!\n");
			exe_pcode++;
		}break;//JTM
		case 26: {
			op_num = op_stack[P_op_stack - 1];
			if(PCODE[exe_pcode]->OP_1==1)
				op_stack[P_op_stack - 1] = dynamic_run_stack[P_d_run_stack][op_num];
			else
				op_stack[P_op_stack - 1] = static_run_stack[op_num];
			exe_pcode++;
		}break;//ATO
		case 27: {
			if (PCODE[exe_pcode]->OP_1 < op_stack[P_op_stack - 1]) {
				printf("over array max!\n");
				stop = 1;
			}
			else exe_pcode++;
		}break;//JAM
		case 28: {
			P_d_run_stack++;
			exe_pcode++;
		}break;//DRS
		case 29: {
			dynamic_run_stack[P_d_run_stack + 1][(int)PCODE[exe_pcode]->OP_1] = op_stack[P_op_stack - 1];
			op_stack[P_op_stack - 1] = 0;
			P_op_stack--;
			exe_pcode++;
		}break;//STP
		case 30: {
			if (op_stack[P_op_stack - 2] == op_stack[P_op_stack - 1]) {
				op_stack[P_op_stack - 1] = 1;
			}
			else {
				op_stack[P_op_stack - 1] = 0;
			}
			exe_pcode++;
		}break;//SWT
		default:printf("doesn't has this code!\n");
			break;
		}
		if (stop)break;
	}
}

void main() {
	char filename[100];
	printf("please input file name:\n");
	gets(filename);
	struct CODE *Temp_Code;
	Temp_Code = (struct CODE*)malloc(sizeof(struct CODE));
	f_in = fopen(filename, "r");

	//init
	now_C = ' ';
	flag_step_const = flag_step_var = flag_step_function = 1;
	var_distance = 0;
	main_flag = id_use_flag = P_label_array = P_string = line = level = array_max = P_op_stack = P_run_stack = P_CODE_TOP = fun_flag = 0;
	P_d_run_stack = switch_jump_end = -1;
	myTable = NULL;
	printf("*----------PCODE----------*\n");
	strcpy(Temp_Code->Code_Name, CODE_NAME[25]);
	Temp_Code->Code_Num = 25;
	PCODE[P_CODE_TOP] = Temp_Code;
	printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
	P_CODE_TOP++;

	//编译并生成P-CODE
	while (now_C != EOF) {
		getsym();
		program();
	}

	//P-CODE执行
	printf("\n*---------EXECUTE---------*\n");
	execute();

	fclose(f_in);
}
