#include<stdio.h>
#include<string.h>

/*
�����֣�
const		int		float		char		void		main
while		scanf	printf	return	if			else
switch		case		default
>		<		>=		<=		==		!=
+		-		*			/			(			)
{		}		[			]			;			:
,		=		"			'			��ʶ��	����
������		.			_
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

//�ö��ַ�����Ƿ�Ϊ�����֡�
int isReserve() {
	char res[15][10] = { "case","char","const","default","else","float","if","int","main","printf","return","scanf","switch","void","while" };
	//res��SYM�еı����ֶ�Ӧ���һ�¡�
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
//��ȡһ���ַ��������ַ����䵽Tokenĩβ��
void catToken() {
	char a[2];
	a[0] = now_C;
	a[1] = '\0';
	strcat(Token, a);
}
//�ļ�ָ�����һ���ַ���
void retract() {
	fseek(f_in, -1L, 1);
}
//�����ǰToken�е������ַ���
void clearToken() {
	int i, len;
	len = strlen(Token);
	for (i = len - 1; i >= 0; i--) {
		Token[i] = '\0';
	}
}
//������
void error() {
	flag_error = 1;
	printf("%c is illegal characters.\n", now_C);
}
//�ж��Ƿ�Ϊ��ĸ��
int isLetter() {
	if (now_C >= 'A'&&now_C <= 'Z' || now_C >= 'a'&&now_C <= 'z' || now_C == '_')
		return 1;
	else return 0;
}
//�ж��Ƿ�Ϊ���֡�
int isDigit() {
	if (now_C >= '0'&&now_C <= '9')
		return 1;
	else return 0;
}
//�ж��Ƿ�Ϊ������
int isIntNum() {
	if (isDigit()) {
		catToken();
		if (now_C == '0') {
			now_C = fgetc(f_in);
			if (isDigit()) {
				retract();
				return 0;
			}//��ͷΪ0���������������Ϸ�������
			else {
				retract();
				return 1;
			}//����0��
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
	}//���������ŵ�������
	else return 0;
}
//��ȡ�ַ����ж��ַ������Լ�������һЩ�в�����
void getsym() {
	flag_error = 0;
	clearToken();
	while (now_C == ' ' || now_C == '\n' || now_C == '\t')
		now_C = fgetc(f_in);
	//�ж��Ƿ�Ϊ��ʶ�������֡�
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
	//�ж��Ƿ�Ϊ���ڻ���ڵ��ڡ�
	else if (now_C == '>') {
		catToken();
		now_C = fgetc(f_in);
		if (now_C == '=') {
			catToken();
			symbol = BIGESY;
		}//���ڵ��ں�
		else {
			retract();
			symbol = BIGSY;
		}//���ں�
	}
	//�ж��Ƿ�ΪС�ڻ�С�ڵ��ڡ�
	else if (now_C == '<') {
		catToken();
		now_C = fgetc(f_in);
		if (now_C == '=') {
			catToken();
			symbol = LESSESY;
		}//С�ڵ��ں�
		else {
			retract();
			symbol = LESSSY;
		}//С�ں�
	}
	//�ж��Ƿ�Ϊ���ȡ�
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
	//�ж��Ƿ�Ϊ�ȺŻ�ֵ���š�
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
	//�ж��Ƿ�Ϊ���š�
	else if (now_C == ',') {
		catToken();
		symbol = COMMASY;
	}
	//�ж��Ƿ�Ϊ�ֺš�
	else if (now_C == ';') {
		catToken();
		symbol = SEMISY;
	}
	//�ж��Ƿ�Ϊ�ַ���
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
	//�ж��Ƿ�Ϊ�ַ�����
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
	//�ж��Ƿ�Ϊ������ʵ����
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
		}//ʵ����
		else {
			retract();
			symbol = INTNUMSY;
		}//������
	}
	//�ж��Ƿ�Ϊ(
	else if (now_C == '(') {
		catToken();
		symbol = LSY;
	}
	//�ж��Ƿ�Ϊ)
	else if (now_C == ')') {
		catToken();
		symbol = RSY;
	}
	//�ж��Ƿ�Ϊ{
	else if (now_C == '{') {
		catToken();
		symbol = BIGLSY;
	}
	//�ж��Ƿ�Ϊ}
	else if (now_C == '}') {
		catToken();
		symbol = BIGRSY;
	}
	//�ж��Ƿ�Ϊ[
	else if (now_C == '[') {
		catToken();
		symbol = MIDLSY;
	}
	//�ж��Ƿ�Ϊ]
	else if (now_C == ']') {
		catToken();
		symbol = MIDRSY;
	}
	//�ж��Ƿ�Ϊ:
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