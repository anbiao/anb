#include<stdio.h>
#include<string.h>

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
	IDSY, INTNUMSY, FLOATNUMSY, DOTSY, UNDERLINESY
}symbol;

char now_C, Token[100];
FILE *f_in, *f_out;
int flag_error;

//用二分法检查是否为保留字。
int isReserve() {
	char res[15][10] = { "case","char","const","default","else","float","if","int","main","printf","return","scanf","switch","void","while" };
	//res和SYM中的保留字对应序号一致。
	int min, max, i;
	min = 0;
	max = 15;
	i = (min + max) / 2;
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
//错误处理。
void error() {
	flag_error = 1;
	printf("%c is illegal characters.\n", now_C);
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
//判断是否为整数。
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
	while (now_C == ' ' || now_C == '\n' || now_C == '\t')
		now_C = fgetc(f_in);
	//判断是否为标识符或保留字。
	if (isLetter()) {
		while (isLetter() || isDigit()) {
			catToken();
			now_C = fgetc(f_in);
		}
		retract();
		int resultValue = isReserve();
		if (resultValue == -1)
			symbol = IDSY;
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
		symbol = PLUSSY;
	}
	else if (now_C == '-') {
		catToken();
		symbol = MINUSSY;
	}
	else if (now_C == '/') {
		catToken();
		symbol = DIVSY;
	}
	else error();
}

void main() {
	int i;
	char filename[20];
	gets(filename);
	f_in = fopen(filename, "r");
	f_out = fopen("output.txt", "w");
	now_C = fgetc(f_in);
	char exm[42][20] = {
		"CASESY","CHARSY","CONSTSY","DEFAULTSY","ELSESY","FLOATSY",
		"IFSY","INTSY","MAINSY","PRINTFSY","RETURNSY","SCANFSY",
		"SWITCHSY","VOIDSY","WHILESY",
		"BIGSY","LESSSY","BIGESY","LESSESY","EQUALSY","UNEQUALSY",
		"PLUSSY","MINUSSY","MULTSY","DIVSY","LSY","RSY",
		"BIGLSY","BIGRSY","MIDLSY","MIDRSY","SEMISY","COLONSY",
		"COMMASY","EVALUATESY","STRINGSY","CHARACTERSY",
		"IDSY","INTNUMSY","FLOATNUMSY","DOTSY","UNDERLINESY"
	};
	i = 0;
	while (now_C != EOF) {
		getsym();
		if (flag_error == 0) {
			fprintf(f_out, "%d\t%s\t\t%s\n", i, exm[symbol], Token);
		}
		now_C = fgetc(f_in);
		i++;
	}
	fclose(f_in);
	fclose(f_out);
}