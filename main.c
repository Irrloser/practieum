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
	SetConsoleTextAttribute(std_out, att);//设置字符属性

	Clear();

	//创建弹出窗口堆栈
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

	GetConsoleScreenBufferInfo(std_out, &binfo);		//取屏幕缓冲区信息
	size = binfo.dwSize.X*binfo.dwSize.Y;				//计算屏幕缓冲区字符单元数
	
	//将屏幕缓冲区所有单元字符属性设成当前缓冲区字符属性
	FillConsoleOutputAttribute(std_out, binfo.wAttributes, size, home, &ul);

	//将缓冲区所有单元射成空格字符
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
	for (i = 0; i < 5; i++)						//在窗口第一行输出主菜单
		printf("\t%s\t", main_menu[i]);
	GetConsoleCursorInfo(std_out, &lpCur);
	lpCur.bVisible = FALSE;						//光标不可见
	SetConsoleCursorInfo(std_out,&lpCur);

	//申请动态储存区作为存放菜单屏幕字符信息的缓冲区
	gp_buff_menubar_info = (CHAR_INFO*)malloc(size.X*size.Y * sizeof(CHAR_INFO));
	SMALL_RECT rcMenu = { 0,0,size.X - 1,0 };

	//将第一行设成红色,其他未白底黑字
	for (i = 0; i < size.X; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
		ch = (char)((gp_buff_menubar_info + i)->Char.AsciiChar);
		if ((ch >= 'a'&& ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
			(gp_buff_menubar_info + i)->Attributes |= FOREGROUND_RED;
	}

	//修改后的菜单条字符回写到第一行
	WriteConsoleOutput(std_out, gp_buff_menubar_info, size, pos, &rcMenu);
	COORD endpos= { 0,1 };
	SetConsoleCursorPosition(std_out, endpos);//光标设于第二行

	//将菜单设为热区,热区编号为菜单编号,热区类型为0(按钮型)
	i = 0;
	do 
	{
		PosB = PosA + strlen(main_menu[i]);//定位第i+1个菜单项的起始位置
		for (j = PosA; j < PosB; j++)
			P_scr_att[j] | -(i + 1) << 2;  //设置菜单项所在字符单元的属性值
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
	else {						//否则定位选择主菜单起止位置,POSA为起POSB为止
		for (i = 1; i < num; i++)
			PosA += strlen(main_menu[i - 1]) + 4;
		PosB = PosA + strlen(main_menu[num - 1]);
	}
	GetConsoleScreenBufferInfo(std_out, &binfo);
	size.X = binfo.dwSize.X;
	size.Y = 1;

	//去除选中菜单中的标记
	for (i = 0; i < PosA; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
		ch = (gp_buff_menubar_info + i)->Char.AsciiChar;
		if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		{
			(gp_buff_menubar_info + i)->Attributes |= FOREGROUND_RED;
		}
	}

	
	//在选中菜单项上做标记，黑底白字
	for (i = PosA; i<PosB; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
			| FOREGROUND_RED;
	}

	//去除选中菜单项后面的菜单项选中标记
	for (i = PosB; i<binfo.dwSize.X; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN| BACKGROUND_RED;
		ch = (char)((gp_buff_menubar_info + i)->Char.AsciiChar);
		if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		{
			(gp_buff_menubar_info + i)->Attributes |= FOREGROUND_RED;
		}
	}

	//将做好标记的菜单条信息写到窗口第一行
	SMALL_RECT rcMenu = { 0, 0, size.X - 1, 0 };
	WriteConsoleOutput(std_out, gp_buff_menubar_info, size, pos, &rcMenu);

	
}

