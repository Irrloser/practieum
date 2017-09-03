#include"Practicum.h"
unsigned long ul;//???
int main()
{
	COORD size = { SCR_COL,SCR_ROW };
	WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE;
	std_out = GetStdHandle(STD_OUTPUT_HANDLE);
	std_in = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleTitle(sys_name);
	SetConsoleScreenBufferSize(std_out, size);

	InitInterface();
}
void InitInterface(void)
{
	WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE;
	SetConsoleTextAttribute(std_out, att);//�����ַ�����

	Clear();

	//�����������ڶ�ջ
	P_scr_att = (char*)calloc(SCR_COL*SCR_ROW, sizeof(char));
	top_layer = (LAYER_NODE*)malloc(sizeof(LAYER_NODE));
	top_layer->LayerNo = 0;
	top_layer->rcArea.Left = 0;
	top_layer->rcArea.Top = 0;
	top_layer->rcArea.Right = SCR_COL - 1;
	top_layer->rcArea.Bottom = SCR_ROW - 1;
	top_layer->pContent = NULL;
	top_layer->pScrAtt = P_scr_att;
	top_layer->next = NULL;

	ShowMenu();
	ShowState();
}
void Clear()
{
	CONSOLE_SCREEN_BUFFER_INFO binfo;
	COORD home = { 0,0 };
	unsigned long size;

	GetConsoleScreenBufferInfo(std_out, &binfo);		//ȡ��Ļ��������Ϣ
	size = binfo.dwSize.X*binfo.dwSize.Y;				//������Ļ�������ַ���Ԫ��
	
	//����Ļ���������е�Ԫ�ַ�������ɵ�ǰ�������ַ�����
	FillConsoleOutputAttribute(std_out, binfo.wAttributes, size, home, &ul);

	//�����������е�Ԫ��ɿո��ַ�
	FillConsoleOutputCharacter(std_out, ' ', size, home, &ul);
	return;
}

void ShowMenu()
{
	CONSOLE_SCREEN_BUFFER_INFO binfo;
	CONSOLE_CURSOR_INFO lpCur;
	COORD size;
	COORD pos = { 0,0 };
	int i, j;
	int PosA = 2, PosB;
	char ch;
	GetConsoleScreenBufferInfo(std_out, &binfo);
	size.X = binfo.dwSize.X;
	size.Y = 1;
	SetConsoleCursorPosition(std_out, pos);
	for (i = 0; i < 5; i++)						//�ڴ��ڵ�һ��������˵�
		printf("\t%s\t", main_menu[i]);
	GetConsoleCursorInfo(std_out, &lpCur);
	lpCur.bVisible = FALSE;						//��겻�ɼ�
	SetConsoleCursorInfo(std_out,&lpCur);

	//���붯̬��������Ϊ��Ų˵���Ļ�ַ���Ϣ�Ļ�����
	gp_buff_menubar_info = (CHAR_INFO*)malloc(size.X*size.Y * sizeof(CHAR_INFO));
	SMALL_RECT rcMenu = { 0,0,size.X - 1,0 };

	//����һ����ɺ�ɫ,����δ�׵׺���
	for (i = 0; i < size.X; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
		ch = (char)((gp_buff_menubar_info + i)->Char.AsciiChar);
		if ((ch >= 'a'&& ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
			(gp_buff_menubar_info + i)->Attributes |= FOREGROUND_RED;
	}

	//�޸ĺ�Ĳ˵����ַ���д����һ��
	WriteConsoleOutput(std_out, gp_buff_menubar_info, size, pos, &rcMenu);
	COORD endpos= { 0,1 };
	SetConsoleCursorPosition(std_out, endpos);//������ڵڶ���

	//���˵���Ϊ����,�������Ϊ�˵����,��������Ϊ0(��ť��)
	i = 0;
	do 
	{
		PosB = PosA + strlen(main_menu[i]);//��λ��i+1���˵������ʼλ��
		for (j = PosA; j < PosB; j++)
			P_scr_att[j] | -(i + 1) << 2;  //���ò˵��������ַ���Ԫ������ֵ
		PosA = PosB + 4;
		i++;
	} while (i < 5);

	TagMainMenu(gi_sel_menu);
}
void ShowState()
{
	CONSOLE_SCREEN_BUFFER_INFO binfo;
	COORD size;
	COORD pos = { 0,0 };
	int i;

	GetConsoleScreenBufferInfo(std_out, &binfo);
	size.X = binfo.dwSize.X;
	size.Y = 1;
	SMALL_RECT rcMenu = { 0,binfo.dwSize.Y - 1,size.X - 1,binfo.dwSize.Y - 1 };
	if (gp_buff_stateBar_info == NULL)
	{
		gp_buff_stateBar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
		ReadConsoleOutput(std_out, gp_buff_stateBar_info, size, pos, &rcMenu);
	}

	for (i = 0; i < size.X; i++)
	{
		(gp_buff_stateBar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
			| BACKGROUND_RED;
	}
	WriteConsoleOutput(std_out, gp_buff_stateBar_info, size, pos, &rcMenu);
}
void TagMainMenu(int num)
{
	CONSOLE_SCREEN_BUFFER_INFO binfo;
	COORD size;
	COORD pos = { 0,0 };
	int PosA = 2, PosB;
	char ch;
	int i;
	if (num == 0)
		PosA = PosB = 0;			
	else {						//����λѡ�����˵���ֹλ��,POSAΪ��POSBΪֹ
		for (i = 1; i < num; i++)
			PosA += strlen(main_menu[i - 1]) + 4;
		PosB = PosA + strlen(main_menu[num - 1]);
	}
	GetConsoleScreenBufferInfo(std_out, &binfo);
	size.X = binfo.dwSize.X;
	size.Y = 1;

	//ȥ��ѡ�в˵��еı��
	for (i = 0; i < PosA; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
		ch = (gp_buff_menubar_info + i)->Char.AsciiChar;
		if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		{
			(gp_buff_menubar_info + i)->Attributes |= FOREGROUND_RED;
		}
	}

	
	//��ѡ�в˵���������ǣ��ڵװ���
	for (i = PosA; i<PosB; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
			| FOREGROUND_RED;
	}

	//ȥ��ѡ�в˵������Ĳ˵���ѡ�б��
	for (i = PosB; i<binfo.dwSize.X; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN| BACKGROUND_RED;
		ch = (char)((gp_buff_menubar_info + i)->Char.AsciiChar);
		if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		{
			(gp_buff_menubar_info + i)->Attributes |= FOREGROUND_RED;
		}
	}

	//�����ñ�ǵĲ˵�����Ϣд�����ڵ�һ��
	SMALL_RECT rcMenu = { 0, 0, size.X - 1, 0 };
	WriteConsoleOutput(std_out, gp_buff_menubar_info, size, pos, &rcMenu);

	
}

