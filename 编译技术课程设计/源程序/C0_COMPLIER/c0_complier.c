#include<stdio.h>
#include<string.h>
#include<stdlib.h>

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
	IDSY, INTNUMSY, FLOATNUMSY, DOTSY, UNDERLINESY,
	INTEGERSY
}symbol;

char now_C,//��ȡԴ����ʱ����ǰ���ַ���
Token[100],//��ȡԴ����ʱ����ʶ���һ�����Ŵ���
name[100],//׼��������ű��еı�ʶ�����֡�
kind[20],//׼��������ű��еı�ʶ�����ࡣ
type[20];//׼��������ű��еı�ʶ�����͡�

FILE *f_in;//Դ�����ļ�ָ�롣

int flag_error,//�Ƿ���ִʷ�����
flag_step_const,//ȫ�ֳ��������Ƿ������
flag_step_var,//ȫ�ֱ��������Ƿ������
flag_step_function,//ȫ�ֺ��������Ƿ������
line,//������
level,//�㼶��
array_max,//�������ޡ�
P_op_stack,//������ջջ���ġ�ָ�롱��
P_run_stack,//��̬����ջջ���ġ�ָ�롱��
P_d_run_stack,//��̬����ջջ���ġ�ָ�롱��
P_label_array,//label_arrayջ���ġ�ָ�롱��
search_addr,//���鵽ͬ����ʶ��ʱ�����ű��б�ʶ�������ֵ��run_stack�еĵ�ַ��
P_CODE_TOP,//����P-CODEʱ��P-CODE���ϵ�ָ������������һ��ָ�Ҫ���뼯�������λ�á�
P_string,//����string_array��λ��
switch_jump_end,//switch�����ת�����ı�ǩ��
void_or_not,//���޷���ֵ�����ı�־��1Ϊ�޷���ֵ������0Ϊ�з���ֵ������
var_distance,//�����б�������ں�����ʼ��ַ��ƫ������
UseVarInDorS,//ʹ�õı�ʶ�������ھ�̬/��̬����ջ�ı�־��
express_type,//���ʽֵ�����ͣ�0Ϊ�ַ��ͣ�1Ϊ���ͣ�2Ϊʵ�͡�
id_use_flag,//����ǰ��ʶ����״̬��ʲô��0Ϊ���壬1Ϊʹ�á�
fun_flag,//��ҪΪ���жϵ�ǰ��Ҫ�ö�̬����ջ���Ǿ�̬����ջ��
main_flag;//

float op_stack[100],//�����������ջ��
static_run_stack[1000];//����̬����ջ��

//��̬����ջ�ṹ
float dynamic_run_stack[1000][1000];

/*
���ģ�

#0 ��������ջ�ĸ��ģ�
static_run_stack(float������)��dynamic_run_stack(float�Ͷ�ά����)
�����漰ԭrun_stack��ָ���Ҫ���ģ������һ��������0�������static_run_stack��1�������dynamic_run_stack
dynamicֻ�溯��ֵ�������ڲ�����ֵ������ȫ������static

#1 scanf�ĸ��ģ�
��ԭ��������������Ϊһ�����������ڶ����ֵ�ȴ浽op_stack����ת��������ջ�����������scanfָ���ʱ����ԭ���ĵ���ָ���Ϊ����ָ��

#2 �������õĸ��ģ���֮��д��

#3 ����ָ��P_d_run_stack����̬����ջ��ջ��ָ�룬��ԭ����P_run_stack��Ϊ��̬����ջ��ջ��ָ�롣

#4 ���ű�������һ��D_or_S������ñ�ʶ������ھ�̬/��̬����ջ��

#5 ����UseVarInDorS��ʹ�õı�ʶ�������ھ�̬/��̬����ջ�ı�־��
*/

int label_array[100];//�洢��ǩ��

char string_array[100][1000];

char CODE_NAME[40][4] = {
"LDN",//������ֵ1������ֵX����op_stackջ����
"PSN",//������ֵ2����X�е���ֵ����op_stack��ջ����XΪrun_stack�ĵ�ַ��YΪ0ʱΪ��̬����ջ��Ϊ1ʱΪ��̬����ջ
"TKN",//��ȡ��ֵ����op_stack��ջ���е���ֵ��ȡ��X�У�XΪrun_stack�ĵ�ַ��YΪ0ʱΪ��̬����ջ��Ϊ1ʱΪ��̬����ջ
"STA",//��������ֵ����op_stackջ���е�ֵ������run_stack�У���ַΪ��ջ����XΪ0ʱΪ��̬����ջ��Ϊ1ʱΪ��̬����ջ
"ADD",//�ӷ����㣺��op_stackջ������Ԫ�ؽ��мӷ����㣬�������ջ��
"SUB",//�������㣺op_stack��ջ��Ԫ�ؼ�ջ��Ԫ�أ��������ջ��
"MUT",//�˷����㣺��op_stackջ������Ԫ�ؽ��г˷����㣬�������ջ��
"DIV",//�������㣺op_stack��ջ��Ԫ�س���ջ��Ԫ�أ��������ջ��
"BIG",//�����жϣ���op_stack��ջ��Ԫ�ش���ջ��Ԫ�أ���ջ��ջ����һ
"LES",//С���жϣ���op_stack��ջ��Ԫ��С��ջ��Ԫ�أ���ջ��ջ����һ
"BOE",//���ڵ��ڣ���op_stack��ջ��Ԫ�ش��ڵ���ջ��Ԫ�أ���ջ��ջ����һ
"LOE",//С�ڵ��ڣ���op_stack��ջ��Ԫ��С�ڵ���ջ��Ԫ�أ���ջ��ջ����һ
"EQL",//�����жϣ���op_stack��ջ��Ԫ�ص���ջ��Ԫ�أ���ջ��ջ����һ
"UEQ",//�������жϣ���op_stack��ջ��Ԫ�ز�����ջ��Ԫ�أ���ջ��ջ����һ
"MIN",//�为��������op_stackջ��Ԫ�ر为������ջ��
"LAB",//���ñ�ǩ����¼��ǰָ��λ�ã�����label_arrayջ����ջ��+1
"JMP",//��������ת����ת��label_array��flag_jump��X��ͬ��ָ��
"ZJP",//����Ϊ����ת��op_stackջ��Ԫ��Ϊ0��ת��label_array��flag_jump��X��ͬ��ָ��
"SCF",//���룺��ʽ��������op_stackջ����X�������ͣ�0-char, 1-int, 2-float
"PRS",//��������string_array[X]�ַ�����ֵ
"PRE",//���ջ�����ʽ��ֵ
"RTN",//�з���ֵ���أ�����ֵ��op_stackջ����ֱ����
"FED",//����������־�����ص��ú���λ�ã���̬����ջ��һ��
"FUS",//�������ñ�־�����ú���λ�ã����䶯̬����ջ�ռ�
"MBG",//��������ʼλ�ã�
"JTM",//��ת����������ʼλ�ã�
"ATO",//ȡ����Ԫ�ص�ֵ����run_stack��ȡֵ����ַΪop_stackջ����ֵ��ȡֵ�����ջ����XΪ0ʱΪ��̬����ջ��Ϊ1ʱΪ��̬����ջ
"JAM",//�ж��Ƿ񳬹������Ͻ죺XΪ�����Ͻ磬��op_stackջ����ȣ������������ִ�У�С�ڵ�����ִֹͣ�С�
"DRS",//���䶯̬����ջ�ռ��Ա���뺯��ʱʹ��
"STP",//����������
"SWT"
};
/*
*
*************************************ָ�����Ƿ����Ŷ�Ӧ��*******************************************
*
*0		LDN	X		������ֵ1������ֵX����op_stackջ����
*1		PSN	X	Y	������ֵ2����X�е���ֵ����op_stack��ջ����XΪrun_stack�ĵ�ַ��YΪ0ʱΪ��̬����ջ��Ϊ1ʱΪ��̬����ջ
*2		TKN	X	Y	��ȡ��ֵ����op_stack��ջ���е���ֵ��ȡ��X�У�XΪrun_stack�ĵ�ַ��YΪ0ʱΪ��̬����ջ��Ϊ1ʱΪ��̬����ջ
*3		STA	X		��������ֵ����op_stackջ���е�ֵ������run_stack�У���ַΪ��ջ����XΪ0ʱΪ��̬����ջ��Ϊ1ʱΪ��̬����ջ
*4		ADD			�ӷ����㣺��op_stackջ������Ԫ�ؽ��мӷ����㣬�������ջ��
*5		SUB			�������㣺op_stack��ջ��Ԫ�ؼ�ջ��Ԫ�أ��������ջ��
*6		MUT			�˷����㣺��op_stackջ������Ԫ�ؽ��г˷����㣬�������ջ��
*7		DIV			�������㣺op_stack��ջ��Ԫ�س���ջ��Ԫ�أ��������ջ��
*8		BIG			�����жϣ���op_stack��ջ��Ԫ�ش���ջ��Ԫ�أ���ջ��ջ����һ
*9		LES			С���жϣ���op_stack��ջ��Ԫ��С��ջ��Ԫ�أ���ջ��ջ����һ
*10		BOE			���ڵ��ڣ���op_stack��ջ��Ԫ�ش��ڵ���ջ��Ԫ�أ���ջ��ջ����һ
*11		LOE			С�ڵ��ڣ���op_stack��ջ��Ԫ��С�ڵ���ջ��Ԫ�أ���ջ��ջ����һ
*12		EQL			�����жϣ���op_stack��ջ��Ԫ�ص���ջ��Ԫ�أ���ջ��ջ����һ
*13		UEQ			�������жϣ���op_stack��ջ��Ԫ�ز�����ջ��Ԫ�أ���ջ��ջ����һ
*14		MIN			�为��������op_stackջ��Ԫ�ر为������ջ��
*15		LAB			���ñ�ǩ����¼��ǰָ��λ�ã�����label_arrayջ����ջ��+1
*16		JMP	X		��������ת����ת��label_array��flag_jump��X��ͬ��ָ��
*17		ZJP			����Ϊ����ת��op_stackջ��Ԫ��Ϊ0��ת��label_array��flag_jump��X��ͬ��ָ��
*18		SCF	X		���룺��ʽ��������op_stackջ����X�������ͣ�0-char, 1-int, 2-float
*19		PRS	X		��������string_array[X]�ַ�����ֵ
*20		PRE			�����������ʽ��ֵ
*21		RTN			�з���ֵ���أ�����ֵ��op_stackջ����ֱ����
*22		FED			����������־�����ص��ú���λ�ã���̬����ջ��һ��
*23		FUS			�������ñ�־�����ú���λ�ã����䶯̬����ջ�ռ�
*24		MBG			��������ʼλ�ã�
*25		JTM			��ת����������ʼλ�ã�
*26		ATO	X		ȡ����Ԫ�ص�ֵ����run_stack��ȡֵ����ַΪop_stackջ����ֵ��ȡֵ�����ջ����XΪ0ʱΪ��̬����ջ��Ϊ1ʱΪ��̬����ջ
*27		JAM	X		�ж��Ƿ񳬹������Ͻ죺XΪ�����Ͻ磬��op_stackջ����ȣ������������ִ�У�С�ڵ�����ִֹͣ�С�
*28		DRS			���䶯̬����ջ�ռ��Ա���뺯��ʱʹ��
*
*******************************************************************************************************
*/

//����ָ��ṹ��
struct CODE
{
	char Code_Name[4];//���Ƿ����ơ�
	int Code_Num;//���Ƿ���š�
	float OP_1;//��һ����������
	float OP_2;//�ڶ�����������
	int flag_jump;//��ת��ʶ��
}*PCODE[100000];//PCODE������P-CODEָ��ϣ��ṹָ�����顣
  

//���ű�ṹ��������ʶ������Ϣ��
struct Table
{
	char name[100];//��ʶ�����ơ�
	char kind[20];//��ʶ�����ࣨ���������������������飩��
	char type[20];//��ʶ�����ͣ�int, char, float����
	int level;//��ʶ���㼶��
	int addr;//��ʶ����ַ��
	int array_max;//�������ޡ�
	int para[100];//����������ַ��
	int para_type[100];//�����������͡�
	int func_begin_addr;//������ַ��
	int para_num;//��������������
	int D_or_S;//��ʶ������Ҫ���ھ�̬/��̬����ջ�ı�־��
	struct Table *next;
	struct Table *pre;
}*myTable, *level_end, *func_struct;
/*
myTable: ��ǰ���ű�ջ����
level_end: ��¼��ʶ��ÿһ�����һ���ڵ�����顣
*/

int express(), term(), factor(), function_use(), condition(), insert(int n), search(char name[]);
void const_description(), var_description(), compound_statement(), sentence_list(), function_description(), main_function(), program(), sentence();

//������ ��
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

//����������nΪ0ʱ�����þ�̬����ջ��Ϊ1ʱ�����ö�̬����ջ��
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

//������һ��Ρ�
void lvlup() {
	level=1;
	level_end = myTable;
}

//�˻�����һ��Ρ�
void lvlback() {
	level = 0;
	myTable = level_end;
}

//�ö��ַ�����Ƿ�Ϊ�����֡�
int isReserve() {
	char res[15][10] = { "case","char","const","default","else","float","if","int","main","printf","return","scanf","switch","void","while" };
	//res��SYM�еı����ֶ�Ӧ���һ�¡�
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

//�������ʷ�����
void error() {
	flag_error = 1;
	printf("%c is illegal characters.\n", now_C);
}

//�������﷨����
void grammer_error(int x) {
	switch (x) {
	case 0: {
		printf("�﷨���󣺵�%d�ж�ʧ ' = '��\n", line);
	}break;
	case 1: {
		printf("�﷨���󣺵�%d�ж�ʧ������\n", line);
	}break;
	case 2: {
		printf("�﷨���󣺵�%d�ж�ʧ��ʶ����\n", line);
	}break;
	case 3: {
		printf("�﷨���󣺵�%d�ж�ʧ ' ; ' ��\n", line);
	}break;
	case 4: {
		printf("�﷨���󣺵�%d�ж�ʧʵ����\n", line);
	}break;
	case 5: {
		printf("�﷨���󣺵�%d�ж�ʧ�ַ���\n", line);
	}break;
	case 6: {
		printf("�﷨���󣺵�%d�ж�ʧ���ͱ�ʶ����\n", line);
	}break;
	case 7: {
		printf("�﷨���󣺵�%d�ж�ʧ ' ( ' ��\n", line);
	}break;
	case 8: {
		printf("�﷨���󣺵�%d�ж�ʧ ' ) ' ��\n", line);
	}break;
	case 9: {
		printf("�﷨���󣺵�%d�ж�ʧ ' { ' ��\n", line);
	}break;
	case 10: {
		printf("�﷨���󣺵�%d�ж�ʧ ' } ' ��\n", line);
	}break;
	case 11: {
		printf("�﷨���󣺵�%d�У���������Ӧ�ڳ���ʼ����\n", line);
	}break;
	case 12: {
		printf("�﷨���󣺵�%d�У���������Ӧ�ں�������ǰ������������\n", line);
	}break;
	case 13: {
		printf("�﷨���󣺵�%d�У����ֲ�����Ԥ�ڵķ��š�%s\n", line, Token);
	}break;
	case 14: {
		printf("�﷨���󣺵�%d�У���������Ӧ��������ǰ������������\n", line);
	}break;
	case 15: {
		printf("�﷨���󣺵�%d�ж�ʧ ' ] ' ��\n", line);
	}break;
	case 16: {
		printf("�﷨���󣺵�%d�ж�ʧ ' , ' ��\n", line);
	}break;
	case 17: {
		printf("�﷨���󣺵�%d�ж�ʧ���ʽ��\n", line);
	}break;
	case 18: {
		printf("�﷨���󣺵�%d�ж�ʧ���ӡ�\n", line);
	}break;
	case 19: {
		printf("�﷨���󣺵�%d�ж�ʧ�\n", line);
	}break;
	case 20: {
		printf("�﷨���󣺵�%d�ж�ʧ�������á�\n", line);
	}break;
	case 21: {
		printf("�﷨���󣺵�%d�ж�ʧ������\n", line);
	}break;
	case 22: {
		printf("�﷨���󣺵�%d�ж�ʧcase��\n", line);
	}break;
	case 23: {
		printf("�﷨���󣺵�%d�ж�ʧ ' : ' ��\n", line);
	}break;
	case 24: {
		printf("�﷨���󣺵�%d�ж�ʧ��ö�ٳ�����\n", line);
	}break;
	}
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

//�ж��Ƿ�Ϊ�޷���������
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
	now_C = fgetc(f_in);
	while (now_C == ' ' || now_C == '\n' || now_C == '\t') {
		if (now_C == '\n') {
			line++;
			//printf("Line: %d\n", line);
		}
		now_C = fgetc(f_in);
	}
	//�ж��Ƿ�Ϊ��ʶ�������֡�
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
		}//ʵ����
		else {
			retract();
			symbol = INTEGERSY;
		}//�����ŵ�������
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
			}//ʵ����
			else {
				retract();
				symbol = INTEGERSY;
			}//�����ŵ�������
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

//���ʽ����������������ʱ��tokenΪ�¸����Ŵ���
int express() {
	int judge, minus;
	express_type = minus = 0;
	if (symbol == PLUSSY || symbol == MINUSSY) {
		if (express_type < 1) {
			express_type = 1;
		}
		//����Ǹ��ţ��Ƚ�op_stackջ��Ԫ�ر为���ٴ���ջ����
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

//�����������������ʱ��tokenΪ�¸����Ŵ���
int term() {
	int judge;
	if (!factor())return 0;
	while (symbol == MULTSY || symbol == DIVSY) {
		if (express_type < 1) {
			express_type = 1;
		}
		//�˷������˷�ָ�������������ָ�
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

//���ӷ���������������ʱ��tokenΪ�¸����Ŵ���
int factor() {
	char factor_type[3][10]= { "int","float","char" };
	struct Table *temp_func_struct;
	temp_func_struct = (struct Table*)malloc(sizeof(struct Table));
	//��ʶ�������
	if (symbol == IDSY) {
	//����һ��PSNָ����ñ�����ֵ����op_stackջ�������������飬�з���ֵ��������OP_2��������㣬ָ���е�����������ͬ��
		if (!search(Token)) {
			printf("%s is undefined!\n", Token);
		}
		temp_func_struct = func_struct;
		now_C = fgetc(f_in);
		if (strcmp(temp_func_struct->type, factor_type[2]) == 0 && express_type <= 0) {
			express_type = 0;
		}//�ַ��͡�
		else if (strcmp(temp_func_struct->type, factor_type[0]) == 0 && express_type <= 1) {
			express_type = 1;
		}//���͡�
		else if (strcmp(temp_func_struct->type, factor_type[1]) == 0 && express_type <= 2) {
			express_type = 2;
		}//ʵ�͡�
		//���顣
		if (now_C == '[') {
			//������LDNָ�������ַ����op_stack��
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
			//������ADD������Ҫȡֵ������Ԫ�ص�ַ��
			PCODE[P_CODE_TOP] = (struct CODE*)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[4]);
			PCODE[P_CODE_TOP]->Code_Num = 4;
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			//�ж��Ƿ񳬳������Ͻ졣
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
		//�з���ֵ�������á�
		else if (now_C == '(') {
			if (!function_use()) {
				grammer_error(20);
				return 0;
			}
			//��¼���ú���λ��+2
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[23]);
			PCODE[P_CODE_TOP]->Code_Num = 23;
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			//������תָ���������ָ��λ�á�
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[16]);
			PCODE[P_CODE_TOP]->Code_Num = 16;
			PCODE[P_CODE_TOP]->flag_jump = temp_func_struct->func_begin_addr;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
		}
		//������
		else {
			retract();
			//ֱ������PSNָ�
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
	//����ʵ���ַ������
	else if (symbol == INTNUMSY || symbol == FLOATNUMSY || symbol == CHARACTERSY||symbol==INTEGERSY) {
		//��ջ��������������Ҳ��ͬ������һ�ֵ����ջ������
		//����һ��LDNָ�
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
	//�����ʽ�������
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

//��/�޷���ֵ�������á�����������ʱ��tokenΪ�¸����Ŵ���
//���õ�ʱ����ת���ú���ָ�
int function_use() {
	struct Table *temp_func_struct;
	temp_func_struct = (struct Table*)malloc(sizeof(struct Table));
	int i = 0;
	temp_func_struct = func_struct;
	getsym();
	if (symbol != RSY) {
		if (express()) {
			if (express_type != temp_func_struct->para_type[i]) {
				printf("�﷨���󣺵�%d�У���������ʱʵ�����β�����Ӧƥ�䡣\n", line);
			}
			//��ÿ�������Ĳ���������Ӧλ�á�
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
					printf("�﷨���󣺵�%d�У���������ʱʵ�����β�����Ӧƥ�䡣\n", line);
				}
				//��ÿ�������Ĳ���������Ӧλ�á�
				PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
				strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[29]);
				PCODE[P_CODE_TOP]->Code_Num = 29;
				PCODE[P_CODE_TOP]->OP_1 = (float)temp_func_struct->para[i];
				i++;
				printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, (int)PCODE[P_CODE_TOP]->OP_1);
				P_CODE_TOP++; 
			}
			if (temp_func_struct->para_num != i - 1) {
				printf("�﷨���󣺵�%d�У�ʵ�����βθ�����ƥ�䡣\n", line);
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

//����˵�������������嶼��������䣩
void const_description() {
	strcpy(kind, Token);
	getsym();
	//�ж��Ƿ�Ϊ���ͳ���������
	if (symbol == INTSY) {
		strcpy(type, Token);
		do {
			getsym();
			if (symbol == IDSY) {
				strcpy(name, Token);
				getsym();
				if (symbol == EVALUATESY)getsym();//�ж��Ƿ�Ϊ�Ⱥš�
				else {
					grammer_error(0);
					break;
				}
				if (symbol == INTNUMSY || symbol == INTEGERSY) static_run_stack[P_run_stack] = (float)atoi(Token);//�ж��Ƿ�Ϊ������
				else grammer_error(1);
			}//�ж��Ƿ�Ϊ��ʶ���������������
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
	//�ж��Ƿ�Ϊʵ�ͳ���������
	else if (symbol == FLOATSY) {
		strcpy(type, Token);
		do {
			getsym();
			if (symbol == IDSY) {
				strcpy(name, Token);
				getsym();
				if (symbol == EVALUATESY)getsym();//�ж��Ƿ�Ϊ�Ⱥš�
				else {
					grammer_error(0);
					break;
				}
				if (symbol == FLOATNUMSY) static_run_stack[P_run_stack] = (float)atof(Token);//�ж��Ƿ�Ϊ������
				else grammer_error(4);
			}//�ж��Ƿ�Ϊ��ʶ����ʵ���������
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
	//�ж��Ƿ�Ϊ�ַ��ͳ���������
	else if (symbol == CHARSY) {
		strcpy(type, Token);
		do {
			getsym();
			if (symbol == IDSY) {
				strcpy(name, Token);
				getsym();
				if (symbol == EVALUATESY)getsym();//�ж��Ƿ�Ϊ�Ⱥš�
				else {
					grammer_error(0);
					break;
				}
				if (symbol == CHARACTERSY) {
					static_run_stack[P_run_stack] = (float)Token[0];
					//printf("This is a \"const descritption - char\" statement.\n");
				}//�ж��Ƿ�Ϊ������
				else grammer_error(5);
			}//�ж��Ƿ�Ϊ��ʶ���������������
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

//����˵�������������嶼��������䣩
void var_description() {
	char array_kind[] = { "array" };
	char var_kind[] = { "var" };
	//�ж��Ƿ�Ϊ���ţ�����һ������������롣
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
			//�ж��Ƿ�Ϊ���顣
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
	//�ж��Ƿ�Ϊ ' [ ' ��������������������롣
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
			//�ж��Ƿ�Ϊ���顣
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
	//�������߽Բ��ǡ�
	else {
		grammer_error(13);
		return;
	}
	//�ж������������Ƿ�Ϊ�ֺš�
	if (symbol != SEMISY) {
		grammer_error(3);
	}
	return;
}

//������������
void compound_statement() {
	char array_kind[] = { "array" };
	char var_kind[] = { "var" };
	//�жϸ���������Ƿ���ڳ���������
	while (symbol == CONSTSY) {
		const_description();
		getsym();
	}
	//�жϸ���������Ƿ���ڱ���������
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
	//�жϸ�������е�����С�
	id_use_flag = 1;
	sentence_list();
	id_use_flag = 0;
}

//if��while����������
int condition() {
	int judge, pre_type;
	pre_type = 0;
	if (express()) {
		if (symbol == LESSSY || symbol == BIGSY || symbol == BIGESY || symbol == LESSESY || symbol == EQUALSY || symbol == UNEQUALSY) {
			if (express_type == 0)printf("�﷨���󣺵�%d�У���������ϵ�Ƚ�ʱ���ܰ����ַ���\n", line);
			else pre_type = express_type;
			//����symbol����ָ�
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
			if (express_type == 0)printf("�﷨���󣺵�%d�У���������ϵ�Ƚ�ʱ���ܰ����ַ���\n", line);
			else if (pre_type != 0) {
				if (pre_type != express_type)
					printf("�﷨���󣺵�%d�У���������ϵ�Ƚ�Ҫ��������ȫƥ�䡣\n", line);
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
				printf("�﷨���󣺵�%d�У�if ��(������������)����while ��(������������)����ߣ����<����>�ǵ������ʽ������������͡�\n", line);
			}
			else return 1;
		}
	}
	else {
		grammer_error(17);
		return 0;
	}
}

//����з�����
void sentence_list() {
	while (symbol==SEMISY||symbol == IFSY || symbol == WHILESY || symbol == SWITCHSY || symbol == SCANFSY || symbol == PRINTFSY || symbol == RETURNSY || symbol == LSY || symbol == IDSY || symbol == BIGLSY) {
		sentence();
	}
	return;
}

//��������
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
	}break;//����䡣
	case IFSY: {
		flag_if_jump = P_CODE_TOP;
		getsym();
		if (symbol == LSY) {
			getsym();
			if (!condition()) {
				grammer_error(21);
			}
			//����ZJPָ��
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
			//������ǩ
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
			//������ǩ
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
	}break;//������䡣
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
	}break;//ѭ����䡣
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
			printf("�﷨���󣺵�%d�У�switch���������ӦΪʵ�͡�\n", line);
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
				printf("�﷨���󣺵�%d�У�switchcase�����������Ӧƥ�䡣\n", line);
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
			}//�ж�case�����Ƿ��ظ���

			i++;
			//����LDNָ���ö���ͳ�����ֵ����op_stackջ��
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[0]);
			PCODE[P_CODE_TOP]->Code_Num = 0;
			PCODE[P_CODE_TOP]->OP_1 = (float)case_value;
			printf("%d:\t%s %f\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->OP_1);
			P_CODE_TOP++;
			//����EQLָ���op_stackջ���ʹ�ջ������Ԫ�ؽ��бȽϡ�
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[30]);
			PCODE[P_CODE_TOP]->Code_Num = 30;
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			//����ZJPָ������ж���Ϊ0����ת��
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
			//������תָ�ִ�����������switch
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[16]);
			PCODE[P_CODE_TOP]->Code_Num = 16;
			PCODE[P_CODE_TOP]->flag_jump = switch_jump_end;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
			//���ñ�ǩ��case���������������ˡ�
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
	}break;//�����䡣
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
			//������SCFָ���ֵ����op_stack��
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
			//������TKNָ���������ջ��
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
	}break;//����䡣
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
	}break;//д��䡣
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
	}break;//������䡣
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
	}break;//���ʽ��
	case IDSY: {
		if (!search(Token))
			printf("%s is undefined!\n", Token);
		temp_func_struct = func_struct;
		getsym();
		if (strcmp(temp_func_struct->kind, const_kind) == 0) {
			printf("�﷨���󣺵�%d�У����ܸ�������ֵ��\n", line);
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
				printf("�﷨���󣺵�%d�и�ֵ������Ͳ�ƥ�䡣\n", line);
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
			//�ж��Ƿ񳬳������Ͻ졣
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
					printf("�﷨���󣺵�%d�У���������ʱʵ�����β�����Ӧƥ�䡣\n", line);
					}
				//��ÿ�������Ĳ���������Ӧλ�á�
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
						printf("�﷨���󣺵�%d�У���������ʱʵ�����β�����Ӧƥ�䡣\n", line);
					}
					//��ÿ�������Ĳ���������Ӧλ�á�
					PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
					strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[29]);
					PCODE[P_CODE_TOP]->Code_Num = 29;
					PCODE[P_CODE_TOP]->OP_1 = (float)temp_func_struct->para[i];
					i++;
					printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
					P_CODE_TOP++; 
				}
				if (temp_func_struct->para_num != i - 1) {
					printf("�﷨���󣺵�%d�У�ʵ�����βθ�����ƥ�䡣\n", line);
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
			//��¼���ú���λ��+2
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[23]);
			PCODE[P_CODE_TOP]->Code_Num = 23;
			printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
			P_CODE_TOP++;
			//������תָ���������ָ��λ�á�
			PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
			strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[16]);
			PCODE[P_CODE_TOP]->Code_Num = 16;
			PCODE[P_CODE_TOP]->flag_jump = temp_func_struct->func_begin_addr;
			printf("%d:\t%s %d\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name, PCODE[P_CODE_TOP]->flag_jump);
			P_CODE_TOP++;
			//���ñ�ǩ�����ú���ִ�����֮�󷵻ظ�λ�á�
			getsym();
			//printf("This is a \"function use\" statement.\n");
		}
		else {
			grammer_error(13);
			return;
		}
	}break;//��ʶ����
	case BIGLSY: {
		getsym();
		sentence_list();
		if (symbol != BIGRSY) {
			grammer_error(10);
		}
		getsym();
	}break;//����С�
	default:grammer_error(13);
		break;
	}
	return;
}

//��/�޷���ֵ�������塣����ͷ��������ע���βΣ�
void function_description() {
	struct Table *temp_func;
	int i = 0;
	int now_para_type;
	temp_func = (struct Table *)malloc(sizeof(struct Table));
	temp_func = myTable;
	char var_kind[] = { "var" };
	lvlup();
	getsym();
	//�ж��βΡ�
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
	//���غ�������λ��
	PCODE[P_CODE_TOP] = (struct CODE *)malloc(sizeof(struct CODE));
	strcpy(PCODE[P_CODE_TOP]->Code_Name, CODE_NAME[22]);
	PCODE[P_CODE_TOP]->Code_Num = 22;
	printf("%d:\t%s\n",P_CODE_TOP, PCODE[P_CODE_TOP]->Code_Name);
	P_CODE_TOP++;
	lvlback();
	//printf("This is a \"function description\" statement.\n");
}

//������������
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

//�ж��Ƿ�Ϊ�����е�һ����(����˵��������˵������/�޷���ֵ������������)��
void program() {
	char var_kind[] = { "var" };
	char func_kind[] = { "return-function" };
	char nfunc_kind[] = { "noreturn-function" };
	char void_type[] = { "type" };
	//�Ƚ�symbol������ű�
	//�ж��Ƿ�Ϊ����˵����
	if (symbol == CONSTSY) {
		if (flag_step_const != 1)
			grammer_error(11);
		const_description();
		return;
	}
	//�ж��Ƿ�Ϊ����˵�����з���ֵ�������塣
	else if (symbol == INTSY || symbol == FLOATSY || symbol == CHARSY) {
		strcpy(type, Token);
		flag_step_const = 0;
		getsym();
		if (symbol == IDSY) {
			strcpy(name, Token);
			getsym();//�Ƚ����ƴ�����ű��ڸ�������symbol����������Ӧ������
			switch (symbol)
			{
			case COMMASY: {
				if (flag_step_var != 1)
					grammer_error(12);
				strcpy(kind, var_kind);
				insert(0);
				//printf("This is a \"variable description\" statement.\n");
				var_description();
			}break;//������������������飩��
			case MIDLSY: {
				if (flag_step_var != 1)
					grammer_error(12);
				var_description();
			}break;//����������������飩��
			case SEMISY: {
				if (flag_step_var != 1)
					grammer_error(12);
				strcpy(kind, var_kind);
				insert(0);
				//printf("This is a \"variable description\" statement.\n");
			}break;//��������������
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
			}break;//�з���ֵ�������塣
			default:grammer_error(13);
				break;
			}
		}
	}
	//�ж��Ƿ�Ϊ�޷���ֵ�����������������
	else if (symbol == VOIDSY) {
		void_or_not = 1;
		strcpy(type, void_type);
		flag_step_const = flag_step_var = 0;
		getsym();
		//��������
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
		//�޷���ֵ�������塣��������ű�
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
	//���������κ�һ�����
	else grammer_error(13);
}

//PCODE����ִ�г���
void execute() {
	int stop,//����ֹͣ�����ź�
		exe_pcode,//��ǰִ��ָ���ַ��
		exe_pcode_no,//��ǰִ��ָ�����ơ�
		op_num,//��ǰ��������
		P_function_use_stack;//function_use_stack��ָ�롣
	int i;
	int function_use_stack[100];
	char lab_judge[] = { "LAB" };
	char mbg_judge[] = { "MBG" };
	char fed_judge[] = { "FED" };
	char scanf_c;
	P_function_use_stack = exe_pcode = stop = 0;
	while (exe_pcode<P_CODE_TOP) {
		//��ȡָ�����
		exe_pcode_no = PCODE[exe_pcode]->Code_Num;
/*
ע�⣺
1.P_op_stackָ��λ��Ĭ��Ϊ�ա�
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

	//���벢����P-CODE
	while (now_C != EOF) {
		getsym();
		program();
	}

	//P-CODEִ��
	printf("\n*---------EXECUTE---------*\n");
	execute();

	fclose(f_in);
}
