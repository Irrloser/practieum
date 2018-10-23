#include "practicum.h"

unsigned long ul;

int main()
{
	COORD size = { SCR_COL, SCR_ROW };              /*窗口缓冲区大小*/

	gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出设备句柄*/
	gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* 获取标准输入设备句柄*/

	SetConsoleTitle(gp_sys_name);                 /*设置窗口标题*/
	SetConsoleScreenBufferSize(gh_std_out, size); /*设置窗口缓冲区大小80*25*/

	LoadData();                   /*数据加载*/
	InitInterface();          /*界面初始化*/
	RunSys(&gp_head);             /*系统功能模块的选择及运行*/
	CloseSys(gp_head);            /*退出系统*/

	return 0;
}

/**
* 函数名称: LoadData
* 函数功能: 将代码表和三类基础数据从数据文件载入到内存缓冲区和十字链表中.
* 输入参数: 无
* 输出参数: 无
* 返 回 值: BOOL类型, 功能函数中除了函数ExitSys的返回值可以为FALSE外,
*           其他函数的返回值必须为TRUE.
*
* 调用说明: 为了能够以统一的方式调用各功能函数, 将这些功能函数的原型设为
*           一致, 即无参数且返回值为BOOL. 返回值为FALSE时, 结束程序运行.
*/
BOOL LoadData()
{
	int Re = 0;

	Re = CreatList(&gp_head);
	gc_sys_state |= Re;
	gc_sys_state &= ~(4 + 8 + 16 - Re);
	if (gc_sys_state < (1 | 2 | 4 | 8 | 16))
	{  /*数据加载提示信息*/
		printf("\n系统基础数据不完整!\n");
		printf("\n按任意键继续...\n");
		getch();
	}

	return TRUE;
}

/**
* 函数名称: LoadCode
* 函数功能: 将代码表从数据文件载入到内存缓冲区, 并进行排序和去除空格.
* 输入参数: FileName 存放代码表的数据文件名.
* 输出参数: pBuffer 指向内存缓冲区的指针变量的地址.
* 返 回 值: 存放代码表的内存缓冲区大小(以字节为单位).
*
* 调用说明:
*/
int LoadCode(char *FileName, char **pBuffer)
{
	char *pTemp, *pStr1, *pStr2;
	int handle;
	int BufferLen, len, loc1, loc2, i;
	long filelen;

	if ((handle = open(FileName, O_RDONLY | O_TEXT)) == -1) /*如果以只读方式打开失败 */
	{
		handle = open(FileName, O_CREAT | O_TEXT, S_IREAD); /*以创建方式打开*/
	}
	filelen = filelength(handle);      /*数据文件的长度*/
	pTemp = (char *)calloc(filelen + 1, sizeof(char)); /*申请同样大小的动态存储区*/
	BufferLen = read(handle, pTemp, filelen); /*将数据文件的内容全部读入到内存*/
	close(handle);

	*(pTemp + BufferLen) = '\0'; /*在动态存储区尾存一个空字符，作为字符串结束标志*/
	BufferLen++;

	for (i = 0; i<BufferLen; i++) /*将动态存储区中的所有换行符替换成空字符*/
	{
		if (*(pTemp + i) == '\n')
		{
			*(pTemp + i) = '\0';
		}
	}

	/*再申请一块同样大小的动态存储区，用于存放排序后的代码串*/
	*pBuffer = (char *)calloc(BufferLen, sizeof(char));
	loc2 = 0;
	pStr1 = pTemp;
	len = strlen(pStr1);

	while (BufferLen > len + 1) /*选择法排序*/
	{
		loc1 = len + 1;
		while (BufferLen > loc1) /*每趟找到序列中最小代码串，首地址存入pStr1*/
		{
			pStr2 = pTemp + loc1;
			if (strcmp(pStr1, pStr2) > 0)
			{
				pStr1 = pStr2;
			}
			loc1 += strlen(pStr2) + 1;
		}
		len = strlen(pStr1);  /*这一趟所找到的最小代码串长度*/

							  /*如果不是空串，则进行复制，loc2是下一个最小代码串存放地址的偏移量*/
		if (len > 0)
		{
			strcpy(*pBuffer + loc2, pStr1);
			loc2 += len + 1;  /*已复制的代码串所占存储空间大小*/
		}

		/*将最小代码串从序列中删除掉*/
		for (i = 0; i<BufferLen - (pStr1 - pTemp) - (len + 1); i++)
		{
			*(pStr1 + i) = *(pStr1 + i + len + 1);
		}

		BufferLen -= len + 1; /*下一趟排序所处理序列的长度*/
		pStr1 = pTemp;  /*假定序列的第一个代码串为最小代码串*/
		len = strlen(pStr1);
	} /*序列中只剩下一个代码串时，排序结束*/

	  /*复制最后这个代码串*/
	len = strlen(pStr1);
	strcpy(*pBuffer + loc2, pStr1);

	/*修改动态存储区大小，使其正好放下排序后代码串*/
	loc2 += len + 1;
	*pBuffer = (char *)realloc(*pBuffer, loc2);
	free(pTemp);  /*释放最先申请的动态存储区*/

	return loc2;  /*返回存放代码串的内存缓冲区实际大小*/
}

/**
* 函数名称: CreatList
* 函数功能: 从数据文件读取基础数据, 并存放到所创建的十字链表中.
* 输入参数: 无
* 输出参数: phead 主链头指针的地址, 用来返回所创建的十字链.
* 返 回 值: int型数值, 表示链表创建的情况.
*           0  空链, 无数据
*           4  已加载学院信息数据，无团队基本信息和课题信息数据
*           12 已加载学院信息和团队基本信息数据，无课题信息数据
*           28 三类基础数据都已加载
*
* 调用说明:
*/
int CreatList(SCHOOL_NODE **phead)
{
	SCHOOL_NODE *hd = NULL, *pSchNode, tmp1;
	TEAM_NODE *pTeamNode, tmp2;
	SUBJECT_NODE *pSubjectNode, tmp3;
	FILE *pFile;
	int find;
	int re = 0;

	if ((pFile = fopen(gp_school_info_filename, "rb")) == NULL)
	{
		printf("学院信息数据文件打开失败!\n");
		return re;
	}
	printf("学院信息数据文件打开成功!\n");

	/*从数据文件中读学院信息数据，存入以后进先出方式建立的主链中*/
	while (fread(&tmp1, sizeof(SCHOOL_NODE), 1, pFile) == 1)
	{
		pSchNode = (SCHOOL_NODE *)malloc(sizeof(SCHOOL_NODE));
		*pSchNode = tmp1;
		pSchNode->Thead = NULL;
		pSchNode->next = hd;
		hd = pSchNode;
	}
	fclose(pFile);
	if (hd == NULL)
	{
		printf("学院信息数据文件加载失败!\n");
		return re;
	}
	printf("学院信息数据文件加载成功!\n");
	*phead = hd;
	re += 4;

	if ((pFile = fopen(gp_team_info_filename, "rb")) == NULL)
	{
		printf("团队基本信息数据文件打开失败!\n");
		return re;
	}
	printf("团队基本信息数据文件打开成功!\n");
	re += 8;

	/*从数据文件中读取团队基本信息数据，存入主链对应结点的团队基本信息支链中*/
	while (fread(&tmp2, sizeof(TEAM_NODE), 1, pFile) == 1)
	{
		/*创建结点，存放从数据文件中读出的团队基本信息*/
		pTeamNode = (TEAM_NODE *)malloc(sizeof(TEAM_NODE));
		*pTeamNode = tmp2;
		pTeamNode->Shead = NULL;

		/*在主链上查找该团队所住学院对应的主链结点*/
		pSchNode = hd;
		while (pSchNode != NULL
			&& strcmp(pSchNode->name, pTeamNode->school) != 0)
		{
			pSchNode = pSchNode->next;
		}
		if (pSchNode != NULL) /*如果找到，则将结点以后进先出方式插入团队信息支链*/
		{
			pTeamNode->next = pSchNode->Thead;
			pSchNode->Thead = pTeamNode;
		}
		else  /*如果未找到，则释放所创建结点的内存空间*/
		{
			free(pTeamNode);
		}
	}
	fclose(pFile);

	if ((pFile = fopen(gp_subject_info_filename, "rb")) == NULL)
	{
		printf("住宿课题信息数据文件打开失败!\n");
		return re;
	}
	printf("住宿课题信息数据文件打开成功!\n");
	re += 16;

	/*从数据文件中读取团队课题信息数据，存入团队基本信息支链对应结点的课题支链中*/
	while (fread(&tmp3, sizeof(SUBJECT_NODE), 1, pFile) == 1)
	{
		/*创建结点，存放从数据文件中读出的团队课题信息*/
		pSubjectNode = (SUBJECT_NODE *)malloc(sizeof(SUBJECT_NODE));
		*pSubjectNode = tmp3;

		/*查找团队信息支链上对应团队信息结点*/
		pSchNode = hd;
		find = 0;
		while (pSchNode != NULL && find == 0)
		{
			pTeamNode = pSchNode->Thead;
			while (pTeamNode != NULL && find == 0)
			{
				if (strcmp(pTeamNode->name, pSubjectNode->team) == 0)
				{
					find = 1;
					break;
				}
				pTeamNode = pTeamNode->next;
			}
			pSchNode = pSchNode->next;
		}
		if (find)  /*如果找到，则将结点以后进先出方式插入团队课题信息支链中*/
		{
			pSubjectNode->next = pTeamNode->Shead;
			pTeamNode->Shead = pSubjectNode;
		}
		else /*如果未找到，则释放所创建结点的内存空间*/
		{
			free(pSubjectNode);
		}
	}
	fclose(pFile);

	return re;
}

/**
* 函数名称: InitInterface
* 函数功能: 初始化界面.
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void InitInterface()
{
	WORD att = FOREGROUND_INTENSITY
| FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;/*白色前景和绿色背景*/

	SetConsoleTextAttribute(gh_std_out, att);  /*设置控制台屏幕缓冲区字符属性*/

	ClearScreen();  /* 清屏*/

					/*创建弹出窗口信息堆栈，将初始化后的屏幕窗口当作第一层弹出窗口*/
	gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*屏幕字符属性*/
	gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
	gp_top_layer->LayerNo = 0;      /*弹出窗口的层号为0*/
	gp_top_layer->rcArea.Left = 0;  /*弹出窗口的区域为整个屏幕窗口*/
	gp_top_layer->rcArea.Top = 0;
	gp_top_layer->rcArea.Right = SCR_COL - 1;
	gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
	gp_top_layer->pContent = NULL;
	gp_top_layer->pScrAtt = gp_scr_att;
	gp_top_layer->next = NULL;

	ShowMenu();     /*显示菜单栏*/
	ShowState();    /*显示状态栏*/

	return;
}

/**
* 函数名称: ClearScreen
* 函数功能: 清除屏幕信息.
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void ClearScreen(void)
{
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	COORD home = { 0, 0 };
	unsigned long size;

	GetConsoleScreenBufferInfo(gh_std_out, &bInfo);/*取屏幕缓冲区信息*/
	size = bInfo.dwSize.X * bInfo.dwSize.Y; /*计算屏幕缓冲区字符单元数*/

											/*将屏幕缓冲区所有单元的字符属性设置为当前屏幕缓冲区字符属性*/
	FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

	/*将屏幕缓冲区所有单元填充为空格字符*/
	FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

	return;
}

/**
* 函数名称: ShowMenu
* 函数功能: 在屏幕上显示主菜单, 并设置热区, 在主菜单第一项上置选中标记.
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void ShowMenu()
{
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	CONSOLE_CURSOR_INFO lpCur;
	COORD size;
	COORD pos = { 0, 0 };
	int i, j;
	int PosA = 2, PosB;
	char ch;

	GetConsoleScreenBufferInfo(gh_std_out, &bInfo);
	size.X = bInfo.dwSize.X;
	size.Y = 1;
	SetConsoleCursorPosition(gh_std_out, pos);
	for (i = 0; i < 5; i++) /*在窗口第一行第一列处输出主菜单项*/
	{
		printf("  %s  ", ga_main_menu[i]);
	}

	GetConsoleCursorInfo(gh_std_out, &lpCur);
	lpCur.bVisible = FALSE;
	SetConsoleCursorInfo(gh_std_out, &lpCur);  /*隐藏光标*/

											   /*申请动态存储区作为存放菜单条屏幕区字符信息的缓冲区*/
	gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
	SMALL_RECT rcMenu = { 0, 0, size.X - 1, 0 };

	/*将窗口第一行的内容读入到存放菜单条屏幕区字符信息的缓冲区中*/
	ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

	/*将这一行中英文字母置为红色，其他字符单元置为白底黑字*/
	for (i = 0; i<size.X; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
			| BACKGROUND_RED;
		ch = (char)((gp_buff_menubar_info + i)->Char.AsciiChar);
		if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		{
			(gp_buff_menubar_info + i)->Attributes |= FOREGROUND_RED;
		}
	}

	/*修改后的菜单条字符信息回写到窗口的第一行*/
	WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
	COORD endPos = { 0, 1 };
	SetConsoleCursorPosition(gh_std_out, endPos);  /*将光标位置设置在第2行第1列*/

												   /*将菜单项置为热区，热区编号为菜单项号，热区类型为0(按钮型)*/
	i = 0;
	do
	{
		PosB = PosA + strlen(ga_main_menu[i]);  /*定位第i+1号菜单项的起止位置*/
		for (j = PosA; j<PosB; j++)
		{
			gp_scr_att[j] |= (i + 1) << 2; /*设置菜单项所在字符单元的属性值*/
		}
		PosA = PosB + 4;
		i++;
	} while (i<5);

	TagMainMenu(gi_sel_menu);  /*在选中主菜单项上做标记，gi_sel_menu初值为1*/

	return;
}

/**
* 函数名称: ShowState
* 函数功能: 显示状态条.
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明: 状态条字符属性为白底黑字, 初始状态无状态信息.
*/
void ShowState()
{
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	COORD size;
	COORD pos = { 0, 0 };
	int i;

	GetConsoleScreenBufferInfo(gh_std_out, &bInfo);
	size.X = bInfo.dwSize.X;
	size.Y = 1;
	SMALL_RECT rcMenu = { 0, bInfo.dwSize.Y - 1, size.X - 1, bInfo.dwSize.Y - 1 };

	if (gp_buff_stateBar_info == NULL)
	{
		gp_buff_stateBar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
		ReadConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);
	}

	for (i = 0; i<size.X; i++)
	{
		(gp_buff_stateBar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
			| BACKGROUND_RED;
		/*
		ch = (char)((gp_buff_stateBar_info+i)->Char.AsciiChar);
		if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		{
		(gp_buff_stateBar_info+i)->Attributes |= FOREGROUND_RED;
		}
		*/
	}

	WriteConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);

	return;
}

/**
* 函数名称: TagMainMenu
* 函数功能: 在指定主菜单项上置选中标志.
* 输入参数: num 选中的主菜单项号
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void TagMainMenu(int num)
{
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	COORD size;
	COORD pos = { 0, 0 };
	int PosA = 2, PosB;
	char ch;
	int i;

	if (num == 0) /*num为0时，将会去除主菜单项选中标记*/
	{
		PosA = 0;
		PosB = 0;
	}
	else  /*否则，定位选中主菜单项的起止位置: PosA为起始位置, PosB为截止位置*/
	{
		for (i = 1; i<num; i++)
		{
			PosA += strlen(ga_main_menu[i - 1]) + 4;
		}
		PosB = PosA + strlen(ga_main_menu[num - 1]);
	}

	GetConsoleScreenBufferInfo(gh_std_out, &bInfo);
	size.X = bInfo.dwSize.X;
	size.Y = 1;

	/*去除选中菜单项前面的菜单项选中标记*/
	for (i = 0; i<PosA; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
			| BACKGROUND_RED;
		ch = (gp_buff_menubar_info + i)->Char.AsciiChar;
		if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		{
			(gp_buff_menubar_info + i)->Attributes |= FOREGROUND_RED;
		}
	}

	/*在选中菜单项上做标记，黑底白字*/
	for (i = PosA; i<PosB; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
			| FOREGROUND_RED;
	}

	/*去除选中菜单项后面的菜单项选中标记*/
	for (i = PosB; i<bInfo.dwSize.X; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
			| BACKGROUND_RED;
		ch = (char)((gp_buff_menubar_info + i)->Char.AsciiChar);
		if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		{
			(gp_buff_menubar_info + i)->Attributes |= FOREGROUND_RED;
		}
	}

	/*将做好标记的菜单条信息写到窗口第一行*/
	SMALL_RECT rcMenu = { 0, 0, size.X - 1, 0 };
	WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

	return;
}

/**
* 函数名称: CloseSys
* 函数功能: 关闭系统.
* 输入参数: hd 主链头指针
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void CloseSys(SCHOOL_NODE *hd)
{
	SCHOOL_NODE *pSchNode1 = hd, *pSchNode2;
	TEAM_NODE *pTeamNode1, *pTeamNode2;
	SUBJECT_NODE *pSubjectNode1, *pSubjectNode2;

	while (pSchNode1 != NULL) /*释放十字交叉链表的动态存储区*/
	{
		pSchNode2 = pSchNode1->next;
		pTeamNode1 = pSchNode1->Thead;
		while (pTeamNode1 != NULL) /*释放团队基本信息支链的动态存储区*/
		{
			pTeamNode2 = pTeamNode1->next;
			pSubjectNode1 = pTeamNode1->Shead;
			while (pSubjectNode1 != NULL) /*释放课题信息支链的动态存储区*/
			{
				pSubjectNode2 = pSubjectNode1->next;
				free(pSubjectNode1);
				pSubjectNode1 = pSubjectNode2;
			}
			free(pTeamNode1);
			pTeamNode1 = pTeamNode2;
		}
		free(pSchNode1);  /*释放主链结点的动态存储区*/
		pSchNode1 = pSchNode2;
	}

	ClearScreen();        /*清屏*/

						  /*释放存放菜单条、状态条、性别代码和团队类别代码等信息动态存储区*/
	free(gp_buff_menubar_info);
	free(gp_buff_stateBar_info);
	free(gp_sub_code);
	//free(gp_type_code);

	/*关闭标准输入和输出设备句柄*/
	CloseHandle(gh_std_out);
	CloseHandle(gh_std_in);

	/*将窗口标题栏置为运行结束*/
	SetConsoleTitle("运行结束");

	return;
}

/**
* 函数名称: RunSys
* 函数功能: 运行系统, 在系统主界面下运行用户所选择的功能模块.
* 输入参数: 无
* 输出参数: phead 主链头指针的地址
* 返 回 值: 无
*
* 调用说明:
*/
void RunSys(SCHOOL_NODE **phead)
{
	INPUT_RECORD inRec;
	DWORD res;
	COORD pos = { 0, 0 };
	BOOL bRet = TRUE;
	int i, loc, num;
	int cNo, cAtt;      /*cNo:字符单元层号, cAtt:字符单元属性*/
	char vkc, asc;      /*vkc:虚拟键代码, asc:字符的ASCII码值*/

	while (bRet)
	{
		/*从控制台输入缓冲区中读一条记录*/
		ReadConsoleInput(gh_std_in, &inRec, 1, &res);

		if (inRec.EventType == MOUSE_EVENT) /*如果记录由鼠标事件产生*/
		{
			pos = inRec.Event.MouseEvent.dwMousePosition;  /*获取鼠标坐标位置*/
			cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*取该位置的层号*/
			cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*取该字符单元属性*/
			if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
			{
				/* cAtt > 0 表明该位置处于热区(主菜单项字符单元)
				* cAtt != gi_sel_menu 表明该位置的主菜单项未被选中
				* gp_top_layer->LayerNo > 0 表明当前有子菜单弹出
				*/
				if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
				{
					PopOff();            /*关闭弹出的子菜单*/
					gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
					PopMenu(cAtt);       /*弹出鼠标所在主菜单项对应的子菜单*/
				}
			}
			else if (cAtt > 0) /*鼠标所在位置为弹出子菜单的菜单项字符单元*/
			{
				TagSubMenu(cAtt); /*在该子菜单项上做选中标记*/
			}

			if (inRec.Event.MouseEvent.dwButtonState
				== FROM_LEFT_1ST_BUTTON_PRESSED) /*如果按下鼠标左边第一键*/
			{
				if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
				{
					if (cAtt > 0) /*如果该位置处于热区(主菜单项字符单元)*/
					{
						PopMenu(cAtt);   /*弹出鼠标所在主菜单项对应的子菜单*/
					}
					/*如果该位置不属于主菜单项字符单元，且有子菜单弹出*/
					else if (gp_top_layer->LayerNo > 0)
					{
						PopOff();            /*关闭弹出的子菜单*/
						gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
					}
				}
				else /*层号不为0，表明该位置被弹出子菜单覆盖*/
				{
					if (cAtt > 0) /*如果该位置处于热区(子菜单项字符单元)*/
					{
						PopOff(); /*关闭弹出的子菜单*/
						gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/

											 /*执行对应功能函数:gi_sel_menu主菜单项号,cAtt子菜单项号*/
						bRet = ExeFunction(gi_sel_menu, cAtt);
					}
				}
			}
			else if (inRec.Event.MouseEvent.dwButtonState
				== RIGHTMOST_BUTTON_PRESSED) /*如果按下鼠标右键*/
			{
				if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
				{
					PopOff();            /*关闭弹出的子菜单*/
					gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
				}
			}
		}
		else if (inRec.EventType == KEY_EVENT  /*如果记录由按键产生*/
			&& inRec.Event.KeyEvent.bKeyDown) /*且键被按下*/
		{
			vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*获取按键的虚拟键码*/
			asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*获取按键的ASC码*/

														/*系统快捷键的处理*/
			if (vkc == 112) /*如果按下F1键*/
			{
				if (gp_top_layer->LayerNo != 0) /*如果当前有子菜单弹出*/
				{
					PopOff();            /*关闭弹出的子菜单*/
					gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
				}
				bRet = ExeFunction(5, 1);  /*运行帮助主题功能函数*/
			}
			else if (inRec.Event.KeyEvent.dwControlKeyState
				& (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
			{ /*如果按下左或右Alt键*/
				switch (vkc)  /*判断组合键Alt+字母*/
				{
				case 88:  /*Alt+X 退出*/
					if (gp_top_layer->LayerNo != 0)
					{
						PopOff();
						gi_sel_sub_menu = 0;
					}
					bRet = ExeFunction(1, 4);
					break;
				case 70:  /*Alt+F*/
					PopMenu(1);
					break;
				case 77: /*Alt+M*/
					PopMenu(2);
					break;
				case 81: /*Alt+Q*/
					PopMenu(3);
					break;
				case 83: /*Alt+S*/
					PopMenu(4);
					break;
				case 72: /*Alt+H*/
					PopMenu(5);
					break;
				}
			}
			else if (asc == 0) /*其他控制键的处理*/
			{
				if (gp_top_layer->LayerNo == 0) /*如果未弹出子菜单*/
				{
					switch (vkc) /*处理方向键(左、右、下)，不响应其他控制键*/
					{
					case 37:
						gi_sel_menu--;
						if (gi_sel_menu == 0)
						{
							gi_sel_menu = 5;
						}
						TagMainMenu(gi_sel_menu);
						break;
					case 39:
						gi_sel_menu++;
						if (gi_sel_menu == 6)
						{
							gi_sel_menu = 1;
						}
						TagMainMenu(gi_sel_menu);
						break;
					case 40:
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
						break;
					}
				}
				else  /*已弹出子菜单时*/
				{
					for (loc = 0, i = 1; i<gi_sel_menu; i++)
					{
						loc += ga_sub_menu_count[i - 1];
					}  /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
					switch (vkc) /*方向键(左、右、上、下)的处理*/
					{
					case 37:
						gi_sel_menu--;
						if (gi_sel_menu < 1)
						{
							gi_sel_menu = 5;
						}
						TagMainMenu(gi_sel_menu);
						PopOff();
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
						break;
					case 38:
						num = gi_sel_sub_menu - 1;
						if (num < 1)
						{
							num = ga_sub_menu_count[gi_sel_menu - 1];
						}
						if (strlen(ga_sub_menu[loc + num - 1]) == 0)
						{
							num--;
						}
						TagSubMenu(num);
						break;
					case 39:
						gi_sel_menu++;
						if (gi_sel_menu > 5)
						{
							gi_sel_menu = 1;
						}
						TagMainMenu(gi_sel_menu);
						PopOff();
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
						break;
					case 40:
						num = gi_sel_sub_menu + 1;
						if (num > ga_sub_menu_count[gi_sel_menu - 1])
						{
							num = 1;
						}
						if (strlen(ga_sub_menu[loc + num - 1]) == 0)
						{
							num++;
						}
						TagSubMenu(num);
						break;
					}
				}
			}
			else if ((asc - vkc == 0) || (asc - vkc == 32)) {  /*按下普通键*/
				if (gp_top_layer->LayerNo == 0)  /*如果未弹出子菜单*/
				{
					switch (vkc)
					{
					case 70: /*f或F*/
						PopMenu(1);
						break;
					case 77: /*m或M*/
						PopMenu(2);
						break;
					case 81: /*q或Q*/
						PopMenu(3);
						break;
					case 83: /*s或S*/
						PopMenu(4);
						break;
					case 72: /*h或H*/
						PopMenu(5);
						break;
					case 13: /*回车*/
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
						break;
					}
				}
				else /*已弹出子菜单时的键盘输入处理*/
				{
					if (vkc == 27) /*如果按下ESC键*/
					{
						PopOff();
						gi_sel_sub_menu = 0;
					}
					else if (vkc == 13) /*如果按下回车键*/
					{
						num = gi_sel_sub_menu;
						PopOff();
						gi_sel_sub_menu = 0;
						bRet = ExeFunction(gi_sel_menu, num);
					}
					else /*其他普通键的处理*/
					{
						/*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
						for (loc = 0, i = 1; i<gi_sel_menu; i++)
						{
							loc += ga_sub_menu_count[i - 1];
						}

						/*依次与当前子菜单中每一项的代表字符进行比较*/
						for (i = loc; i<loc + ga_sub_menu_count[gi_sel_menu - 1]; i++)
						{
							if (strlen(ga_sub_menu[i])>0 && vkc == ga_sub_menu[i][1])
							{ /*如果匹配成功*/
								PopOff();
								gi_sel_sub_menu = 0;
								bRet = ExeFunction(gi_sel_menu, i - loc + 1);
							}
						}
					}
				}
			}
		}
	}
}


/**
* 函数名称: PopMenu
* 函数功能: 弹出指定主菜单项对应的子菜单.
* 输入参数: num 指定的主菜单项号
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void PopMenu(int num)
{
	LABEL_BUNDLE labels;
	HOT_AREA areas;
	SMALL_RECT rcPop;
	COORD pos;
	WORD att;
	char *pCh;
	int i, j, loc = 0;

	if (num != gi_sel_menu)       /*如果指定主菜单不是已选中菜单*/
	{
		if (gp_top_layer->LayerNo != 0) /*如果此前已有子菜单弹出*/
		{
			PopOff();
			gi_sel_sub_menu = 0;
		}
	}
	else if (gp_top_layer->LayerNo != 0) /*若已弹出该子菜单，则返回*/
	{
		return;
	}

	gi_sel_menu = num;    /*将选中主菜单项置为指定的主菜单项*/
	TagMainMenu(gi_sel_menu); /*在选中的主菜单项上做标记*/
	LocSubMenu(gi_sel_menu, &rcPop); /*计算弹出子菜单的区域位置, 存放在rcPop中*/

									 /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
	for (i = 1; i<gi_sel_menu; i++)
	{
		loc += ga_sub_menu_count[i - 1];
	}
	/*将该组子菜单项项名存入标签束结构变量*/
	labels.ppLabel = ga_sub_menu + loc;   /*标签束第一个标签字符串的地址*/
	labels.num = ga_sub_menu_count[gi_sel_menu - 1]; /*标签束中标签字符串的个数*/
	COORD aLoc[100];/*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
	for (i = 0; i<labels.num; i++) /*确定标签字符串的输出位置，存放在坐标数组中*/
	{
		aLoc[i].X = rcPop.Left + 2;
		aLoc[i].Y = rcPop.Top + i + 1;
	}
	labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
						/*设置热区信息*/
	areas.num = labels.num;       /*热区的个数，等于标签的个数，即子菜单的项数*/
	SMALL_RECT aArea[100];// [areas.num];                    /*定义数组存放所有热区位置*/
	char aSort[100];// [areas.num];                      /*定义数组存放所有热区对应类别*/
	char aTag[100];// [areas.num];                         /*定义数组存放每个热区的编号*/
	for (i = 0; i<areas.num; i++)
	{
		aArea[i].Left = rcPop.Left + 2;  /*热区定位*/
		aArea[i].Top = rcPop.Top + i + 1;
		aArea[i].Right = rcPop.Right - 2;
		aArea[i].Bottom = aArea[i].Top;
		aSort[i] = 0;       /*热区类别都为0(按钮型)*/
		aTag[i] = i + 1;           /*热区按顺序编号*/
	}
	areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
	areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
	areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

	att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
	PopUp(&rcPop, att, &labels, &areas);
	DrawBox(&rcPop);  /*给弹出窗口画边框*/
	pos.X = rcPop.Left + 2;
	for (pos.Y = rcPop.Top + 1; pos.Y<rcPop.Bottom; pos.Y++)
	{ /*此循环用来在空串子菜项位置画线形成分隔，并取消此菜单项的热区属性*/
		pCh = ga_sub_menu[loc + pos.Y - rcPop.Top - 1];
		if (strlen(pCh) == 0) /*串长为0，表明为空串*/
		{   /*首先画横线*/
			FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right - rcPop.Left - 3, pos, &ul);
			for (j = rcPop.Left + 2; j<rcPop.Right - 1; j++)
			{   /*取消该区域字符单元的热区属性*/
				gp_scr_att[pos.Y*SCR_COL + j] &= 3; /*按位与的结果保留了低两位*/
			}
		}

	}
	/*将子菜单项的功能键设为白底红字*/
	pos.X = rcPop.Left + 3;
	att = FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
	for (pos.Y = rcPop.Top + 1; pos.Y<rcPop.Bottom; pos.Y++)
	{
		if (strlen(ga_sub_menu[loc + pos.Y - rcPop.Top - 1]) == 0)
		{
			continue;  /*跳过空串*/
		}
		FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
	}
	return;
}

/**
* 函数名称: PopUp
* 函数功能: 在指定区域输出弹出窗口信息, 同时设置热区, 将弹出窗口位置信息入栈.
* 输入参数: pRc 弹出窗口位置数据存放的地址
*           att 弹出窗口区域字符属性
*           pLabel 弹出窗口中标签束信息存放的地址
pHotArea 弹出窗口中热区信息存放的地址
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
	LAYER_NODE *nextLayer;
	COORD size;
	COORD pos = { 0, 0 };
	char *pCh;
	int i, j, row;

	/*弹出窗口所在位置字符单元信息入栈*/
	size.X = pRc->Right - pRc->Left + 1;    /*弹出窗口的宽度*/
	size.Y = pRc->Bottom - pRc->Top + 1;    /*弹出窗口的高度*/
											/*申请存放弹出窗口相关信息的动态存储区*/
	nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
	nextLayer->next = gp_top_layer;
	nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
	nextLayer->rcArea = *pRc;
	nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y * sizeof(CHAR_INFO));
	nextLayer->pScrAtt = (char *)malloc(size.X*size.Y * sizeof(char));
	pCh = nextLayer->pScrAtt;
	/*将弹出窗口覆盖区域的字符信息保存，用于在关闭弹出窗口时恢复原样*/
	ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
	for (i = pRc->Top; i <= pRc->Bottom; i++)
	{   /*此二重循环将所覆盖字符单元的原先属性值存入动态存储区，便于以后恢复*/
		for (j = pRc->Left; j <= pRc->Right; j++)
		{
			*pCh = gp_scr_att[i*SCR_COL + j];
			pCh++;
		}
	}
	gp_top_layer = nextLayer;  /*完成弹出窗口相关信息入栈操作*/
							   /*设置弹出窗口区域字符的新属性*/
	pos.X = pRc->Left;
	pos.Y = pRc->Top;
	for (i = pRc->Top; i <= pRc->Bottom; i++)
	{
		FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
		pos.Y++;
	}
	/*将标签束中的标签字符串在设定的位置输出*/
	for (i = 0; i<pLabel->num; i++)
	{
		pCh = pLabel->ppLabel[i];
		if (strlen(pCh) != 0)
		{
			WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
				pLabel->pLoc[i], &ul);
		}
	}
	/*设置弹出窗口区域字符单元的新属性*/
	for (i = pRc->Top; i <= pRc->Bottom; i++)
	{   /*此二重循环设置字符单元的层号*/
		for (j = pRc->Left; j <= pRc->Right; j++)
		{
			gp_scr_att[i*SCR_COL + j] = gp_top_layer->LayerNo;
		}
	}

	for (i = 0; i<pHotArea->num; i++)
	{   /*此二重循环设置所有热区中字符单元的热区类型和热区编号*/
		row = pHotArea->pArea[i].Top;
		for (j = pHotArea->pArea[i].Left; j <= pHotArea->pArea[i].Right; j++)
		{
			gp_scr_att[row*SCR_COL + j] |= (pHotArea->pSort[i] << 6)
				| (pHotArea->pTag[i] << 2);
		}
	}
	return;
}

/**
* 函数名称: PopOff
* 函数功能: 关闭顶层弹出窗口, 恢复覆盖区域原外观和字符单元原属性.
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void PopOff(void)
{
	LAYER_NODE *nextLayer;
	COORD size;
	COORD pos = { 0, 0 };
	char *pCh;
	int i, j;

	if ((gp_top_layer->next == NULL) || (gp_top_layer->pContent == NULL))
	{   /*栈底存放的主界面屏幕信息，不用关闭*/
		return;
	}
	nextLayer = gp_top_layer->next;
	/*恢复弹出窗口区域原外观*/
	size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
	size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
	WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
	/*恢复字符单元原属性*/
	pCh = gp_top_layer->pScrAtt;
	for (i = gp_top_layer->rcArea.Top; i <= gp_top_layer->rcArea.Bottom; i++)
	{
		for (j = gp_top_layer->rcArea.Left; j <= gp_top_layer->rcArea.Right; j++)
		{
			gp_scr_att[i*SCR_COL + j] = *pCh;
			pCh++;
		}
	}
	free(gp_top_layer->pContent);    /*释放动态存储区*/
	free(gp_top_layer->pScrAtt);
	free(gp_top_layer);
	gp_top_layer = nextLayer;
	gi_sel_sub_menu = 0;
	return;
}

/**
* 函数名称: DrawBox
* 函数功能: 在指定区域画边框.
* 输入参数: pRc 存放区域位置信息的地址
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void DrawBox(SMALL_RECT *pRc)
{
	char chBox[] = { '+','-','|' };  /*画框用的字符*/
	COORD pos = { pRc->Left, pRc->Top };  /*定位在区域的左上角*/

	WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左上角*/
	for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
	{   /*此循环画上边框横线*/
		WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
	}
	pos.X = pRc->Right;
	WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右上角*/
	for (pos.Y = pRc->Top + 1; pos.Y < pRc->Bottom; pos.Y++)
	{   /*此循环画边框左边线和右边线*/
		pos.X = pRc->Left;
		WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
		pos.X = pRc->Right;
		WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
	}
	pos.X = pRc->Left;
	pos.Y = pRc->Bottom;
	WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左下角*/
	for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
	{   /*画下边框横线*/
		WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
	}
	pos.X = pRc->Right;
	WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右下角*/
	return;
}

/**
* 函数名称: TagSubMenu
* 函数功能: 在指定子菜单项上做选中标记.
* 输入参数: num 选中的子菜单项号
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void TagSubMenu(int num)
{
	SMALL_RECT rcPop;
	COORD pos;
	WORD att;
	int width;

	LocSubMenu(gi_sel_menu, &rcPop);  /*计算弹出子菜单的区域位置, 存放在rcPop中*/
	if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom - rcPop.Top - 1))
	{   /*如果子菜单项号越界，或该项子菜单已被选中，则返回*/
		return;
	}

	pos.X = rcPop.Left + 2;
	width = rcPop.Right - rcPop.Left - 3;
	if (gi_sel_sub_menu != 0) /*首先取消原选中子菜单项上的标记*/
	{
		pos.Y = rcPop.Top + gi_sel_sub_menu;
		att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
		FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
		pos.X += 1;
		att |= FOREGROUND_RED;/*白底红字*/
		FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
	}
	/*在制定子菜单项上做选中标记*/
	pos.X = rcPop.Left + 2;
	pos.Y = rcPop.Top + num;
	att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
	FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
	gi_sel_sub_menu = num;  /*修改选中子菜单项号*/
	return;
}

/**
* 函数名称: LocSubMenu
* 函数功能: 计算弹出子菜单区域左上角和右下角的位置.
* 输入参数: num 选中的主菜单项号
* 输出参数: rc 存放区域位置信息的地址
* 返 回 值: 无
*
* 调用说明:
*/
void LocSubMenu(int num, SMALL_RECT *rc)
{
	int i, len, loc = 0;

	rc->Top = 1; /*区域的上边定在第2行，行号为1*/
	rc->Left = 1;
	for (i = 1; i<num; i++)
	{   /*计算区域左边界位置, 同时计算第一个子菜单项在子菜单字符串数组中的位置*/
		rc->Left += strlen(ga_main_menu[i - 1]) + 4;
		loc += ga_sub_menu_count[i - 1];
	}
	rc->Right = strlen(ga_sub_menu[loc]);/*暂时存放第一个子菜单项字符串长度*/
	for (i = 1; i<ga_sub_menu_count[num - 1]; i++)
	{   /*查找最长子菜单字符串，将其长度存放在rc->Right*/
		len = strlen(ga_sub_menu[loc + i]);
		if (rc->Right < len)
		{
			rc->Right = len;
		}
	}
	rc->Right += rc->Left + 3;  /*计算区域的右边界*/
	rc->Bottom = rc->Top + ga_sub_menu_count[num - 1] + 1;/*计算区域下边的行号*/
	if (rc->Right >= SCR_COL)  /*右边界越界的处理*/
	{
		len = rc->Right - SCR_COL + 1;
		rc->Left -= len;
		rc->Right = SCR_COL - 1;
	}
	return;
}

/**
* 函数名称: DealInput
* 函数功能: 在弹出窗口区域设置热区, 等待并相应用户输入.
* 输入参数: pHotArea
*           piHot 焦点热区编号的存放地址, 即指向焦点热区编号的指针
* 输出参数: piHot 用鼠标单击、按回车或空格时返回当前热区编号
* 返 回 值:
*
* 调用说明:
*/
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
	INPUT_RECORD inRec;
	DWORD res;
	COORD pos = { 0, 0 };
	int num, arrow, iRet = 0;
	int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
	char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/

	SetHotPoint(pHotArea, *piHot);
	while (TRUE)
	{    /*循环*/
		ReadConsoleInput(gh_std_in, &inRec, 1, &res);
		if ((inRec.EventType == MOUSE_EVENT) &&
			(inRec.Event.MouseEvent.dwButtonState
				== FROM_LEFT_1ST_BUTTON_PRESSED))
		{
			pos = inRec.Event.MouseEvent.dwMousePosition;
			cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;
			cTag = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2) & 15;
			cSort = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 6) & 3;

			if ((cNo == gp_top_layer->LayerNo) && cTag > 0)
			{
				*piHot = cTag;
				SetHotPoint(pHotArea, *piHot);
				if (cSort == 0)
				{
					iRet = 13;
					break;
				}
			}
		}
		else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown)
		{
			vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
			asc = inRec.Event.KeyEvent.uChar.AsciiChar;;
			if (asc == 0)
			{
				arrow = 0;
				switch (vkc)
				{  /*方向键(左、上、右、下)的处理*/
				case 37: arrow = 1; break;
				case 38: arrow = 2; break;
				case 39: arrow = 3; break;
				case 40: arrow = 4; break;
				}
				if (arrow > 0)
				{
					num = *piHot;
					while (TRUE)
					{
						if (arrow < 3)
						{
							num--;
						}
						else
						{
							num++;
						}
						if ((num < 1) || (num > pHotArea->num) ||
							((arrow % 2) && (pHotArea->pArea[num - 1].Top
								== pHotArea->pArea[*piHot - 1].Top)) || ((!(arrow % 2))
									&& (pHotArea->pArea[num - 1].Top
										!= pHotArea->pArea[*piHot - 1].Top)))
						{
							break;
						}
					}
					if (num > 0 && num <= pHotArea->num)
					{
						*piHot = num;
						SetHotPoint(pHotArea, *piHot);
					}
				}
			}
			else if (vkc == 27)
			{  /*ESC键*/
				iRet = 27;
				break;
			}
			else if (vkc == 13 || vkc == 32)
			{  /*回车键或空格表示按下当前按钮*/
				iRet = 13;
				break;
			}
		}
	}
	return iRet;
}



void SetHotPoint(HOT_AREA *pHotArea, int iHot)
{
	CONSOLE_CURSOR_INFO lpCur;
	COORD pos = { 0, 0 };
	WORD att1, att2;
	int i, width;

	att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
	att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
	for (i = 0; i<pHotArea->num; i++)
	{  /*将按钮类热区置为白底黑字*/
		pos.X = pHotArea->pArea[i].Left;
		pos.Y = pHotArea->pArea[i].Top;
		width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
		if (pHotArea->pSort[i] == 0)
		{  /*热区是按钮类*/
			FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
		}
	}

	pos.X = pHotArea->pArea[iHot - 1].Left;
	pos.Y = pHotArea->pArea[iHot - 1].Top;
	width = pHotArea->pArea[iHot - 1].Right - pHotArea->pArea[iHot - 1].Left + 1;
	if (pHotArea->pSort[iHot - 1] == 0)
	{  /*被激活热区是按钮类*/
		FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
	}
	else if (pHotArea->pSort[iHot - 1] == 1)
	{  /*被激活热区是文本框类*/
		SetConsoleCursorPosition(gh_std_out, pos);
		GetConsoleCursorInfo(gh_std_out, &lpCur);
		lpCur.bVisible = TRUE;
		SetConsoleCursorInfo(gh_std_out, &lpCur);
	}
}

/**
* 函数名称: ExeFunction
* 函数功能: 执行由主菜单号和子菜单号确定的功能函数.
* 输入参数: m 主菜单项号
*           s 子菜单项号
* 输出参数: 无
* 返 回 值: BOOL类型, TRUE 或 FALSE
*
* 调用说明: 仅在执行函数ExitSys时, 才可能返回FALSE, 其他情况下总是返回TRUE
*/
BOOL ExeFunction(int m, int s)
{
	BOOL bRet = TRUE;
	/*函数指针数组，用来存放所有功能函数的入口地址*/
	BOOL(*pFunction[19])(void);// [ga_sub_menu_count[0] + ga_sub_menu_count[1] + ga_sub_menu_count[2] + ga_sub_menu_count[3] + ga_sub_menu_count[4]])(void);
	int i, loc;

	/*将功能函数入口地址存入与功能函数所在主菜单号和子菜单号对应下标的数组元素*/
	pFunction[0] = SaveData;
	pFunction[1] = ExitSys;
	pFunction[2] = MaintainSchInfo;
	pFunction[3] = MaintainTeamInfo;
	pFunction[4] = MaintainSubjectInfo;
	pFunction[5] = QurrySchoolP;
	pFunction[6] = QurrySchoolName;
	pFunction[7] = NULL;
	pFunction[8] = QurryTeamName;
	pFunction[9] = QurryProfAcc;
	pFunction[10] = NULL;
	pFunction[11] = QurrySubID;
	pFunction[12] = QurryTeamBelong;
	pFunction[13] = StatPro2Stu;
	pFunction[14] = StatSubjectAcc;
	pFunction[15] = StatESubject;
	pFunction[16] = StatSub2Pro;
	pFunction[17] = Other;
	pFunction[18] = About;


	for (i = 1, loc = 0; i<m; i++)  /*根据主菜单号和子菜单号计算对应下标*/
	{
		loc += ga_sub_menu_count[i - 1];
	}
	loc += s - 1;

	if (pFunction[loc] != NULL)
	{
		bRet = (*pFunction[loc])();  /*用函数指针调用所指向的功能函数*/
	}

	return bRet;
}
/**
* 函数名称: SaveData
* 函数功能: 保存数据
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 1
*
* 调用说明:调用SaveSysdata
*/
BOOL SaveData(void)
{
    system("cls");
	BOOL bRet = TRUE;
	char *plabel_name[] = { "主菜单项：文件",
		"子菜单项：数据保存",
		"确认"
	};

	ShowModule(plabel_name, 3);
	SaveSysData(gp_head);

	printf("保存成功!\n");

	return bRet;
}



/**
* 函数名称: ExitSys
* 函数功能: 推出系统
* 输入参数: 无
* 输出参数: 状态参量
* 返 回 值: 1
*
* 调用说明:DealInput	PopOff	PopUp
*/
BOOL ExitSys(void)
{
	LABEL_BUNDLE labels;
	HOT_AREA areas;
	BOOL bRet = TRUE;
	SMALL_RECT rcPop;
	COORD pos;
	WORD att;
	char *pCh[] = { "确认退出系统吗？", "确定    取消" };
	int iHot = 1;

	pos.X = strlen(pCh[0]) + 6;
	pos.Y = 7;
	rcPop.Left = (SCR_COL - pos.X) / 2;
	rcPop.Right = rcPop.Left + pos.X - 1;
	rcPop.Top = (SCR_ROW - pos.Y) / 2;
	rcPop.Bottom = rcPop.Top + pos.Y - 1;

	att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
	labels.num = 2;
	labels.ppLabel = pCh;
	COORD aLoc[] = { { rcPop.Left + 3, rcPop.Top + 2 },
	{ rcPop.Left + 5, rcPop.Top + 5 } };
	labels.pLoc = aLoc;

	areas.num = 2;
	SMALL_RECT aArea[] = { { rcPop.Left + 5, rcPop.Top + 5,
		rcPop.Left + 8, rcPop.Top + 5 },
		{ rcPop.Left + 13, rcPop.Top + 5,
		rcPop.Left + 16, rcPop.Top + 5 } };
	char aSort[] = { 0, 0 };
	char aTag[] = { 1, 2 };
	areas.pArea = aArea;
	areas.pSort = aSort;
	areas.pTag = aTag;
	PopUp(&rcPop, att, &labels, &areas);

	pos.X = rcPop.Left + 1;
	pos.Y = rcPop.Top + 4;
	FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right - rcPop.Left - 1, pos, &ul);

	if (DealInput(&areas, &iHot) == 13 && iHot == 1)
	{
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
	}
	PopOff();

	return bRet;
}

/**
* 函数名称: MainTainSchinfo
* 函数功能: 维护学院信息
* 输入参数: 无
* 输出参数: 状态参数
* 返 回 值: TRUE
*
* 调用说明:DelSchoolNode  InsertSchoolNode	ModifySchool
*/
BOOL MaintainSchInfo(void)
{
	BOOL bRet = TRUE;
	int  model;
	char *plabel_name[] = { "主菜单项：数据维护",
		"子菜单项：学院信息",
		"确认"
	};

	ShowModule(plabel_name, 3);
	system("cls");
	printf("请输入想进行的操作:\n");
	printf("(1)输入学院信息		(2)删除学院信息		（3）修改学院信息		(4)返回上一级\n");
	scanf("%d", &model);
	getchar();
	switch (model)
	{
	case 1:
		InsertSchoolNode();
		break;
	case 2:
		DelSchoolNode();
		break;
	case 3:
		ModifySchoolInfo();
		break;
	case 4:
		system("cls");
		return TRUE;
	default:
		printf("错误操作\n");
		break;
	}

	return bRet;
}

/**
* 函数名称: MainTainTeaminfo
* 函数功能: 维护团队信息
* 输入参数: 无
* 输出参数: 状态参数
* 返 回 值: TRUE
*
* 调用说明:DelTeamNode  InsertTeamNode	ModifyTeaminfo
*/
BOOL MaintainTeamInfo(void)
{
	BOOL bRet = TRUE;
	int  model;
	char *plabel_name[] = { "主菜单项：数据维护",
		"子菜单项：学院信息",
		"确认"
	};

	ShowModule(plabel_name, 3);
	system("cls");
	printf("请输入想进行的操作:\n");
	printf("(1)输入团队信息		(2)删除团队信息		（3）修改团队信息		(4)返回上一级\n");
	scanf("%d", &model);
	getchar();
	switch (model)
	{
	case 1:
		InsertTeamNode();
		break;
	case 2:
		DelTeamNode();
		break;
	case 3:
		ModifyTeamInfo();
		break;
	case 4:
		system("cls");
		return TRUE;
	default:
		printf("错误操作\n");
		break;
	}

	ShowModule(plabel_name, 3);

	return bRet;
}

/**
* 函数名称: MainTainSubinfo
* 函数功能: 维护项目信息
* 输入参数: 无
* 输出参数: 状态参数
* 返 回 值: TRUE
*
* 调用说明:DelSubNode  InsertSubNode	ModifySubNode
*/
BOOL MaintainSubjectInfo(void)
{
	BOOL bRet = TRUE;
	int  model;
	char *plabel_name[] = { "主菜单项：数据维护",
		"子菜单项：学院信息",
		"确认"
	};

	ShowModule(plabel_name, 3);
	system("cls");
	printf("请输入想进行的操作:\n");
	printf("(1)输入项目信息		(2)删除项目信息		（3）修改项目信息		(4)返回上一级\n");
	printf("请选择操作模式:");
	scanf("%d", &model);
	getchar();
	switch (model)
	{
	case 1:
		InsertProInfo();
		break;
	case 2:
		DelProInfo();
		break;
	case 3:
		ModifyProInfo_2();
		break;
	case 4:
		system("cls");
		return TRUE;
	default:
		printf("错误操作\n");
		break;
	}



	return bRet;
}






/**
* 函数名称: InsertProNode
* 函数功能: 在十字链表中插入一个课题信息结点.
* 输入参数: void
* 输出参数: 无
* 返 回 值: BOOL类型, TRUE表示插入成功, FALSE表示插入失败
*
* 调用说明:
*/
BOOL InsertProInfo(void)
{
	system("cls");
	SUBJECT_NODE* new_subject = (SUBJECT_NODE*)malloc(sizeof(SUBJECT_NODE));
	TEAM_NODE *temp;
	printf("请输入项目所属团队:");
	scanf("%s", new_subject->team);
	getchar();
	if ((temp = MatchTeamName(gp_head, new_subject->team)) != NULL)
	{
		new_subject->next = temp->Shead;
		temp->Shead = new_subject;
		FullFillProInfo(new_subject);
		printf("录入成功\n");
		return TRUE;
	}
	else
	{
		printf("\n未找到该团队.请重试.\n");
		return FALSE;
	}

}

/**
* 函数名称: DelSubjectNode
* 函数功能: 从十字链表中删除指定的课题信息结点.
* 输入参数: hd 主链头指针
*           Team_id 课题团队学号
*           date 课题日期
* 输出参数: 无
* 返 回 值: BOOL类型, TRUE表示删除成功, FALSE表示删除失败
*
* 调用说明: 根据学号和课题日期可以确定唯一的课题信息
*/
BOOL DelSubjectNode(void)
{
	system("cls");

	char ID[15];
	printf("请输入项目编号：");
	scanf("%s", ID);
	getchar();
	TEAM_NODE* tmp1 = SeekProID_2(gp_head, ID);
	SUBJECT_NODE* pS1, *pS2, *tmp2;
	if (tmp1 != NULL)
		tmp2 = SeekProID(gp_head, ID);
	if (tmp2 != NULL)
	{
		pS1 = NULL;
		pS2 = tmp1->Shead;
		while (pS2 != NULL&&pS2 != tmp2)
		{
			pS1 = pS2;
			pS2 = pS2->next;
		}
		if (pS2 == tmp2)
		{
			if (pS1 == NULL)
				tmp1->Shead = pS2->next;
			else
				pS1->next = pS2->next;
			printf("删除成功！\n");
			free(pS2);
			return TRUE;
		}
	}
	return TRUE;
}

/**
* 函数名称: SeekProID
* 函数功能: 在十字链表中寻找一个课题信息结点.
* 输入参数: 主链头指针
* 输出参数: 无
* 返 回 值: 课题所属团队指针
*
* 调用说明:
*/
TEAM_NODE* SeekProID_2(SCHOOL_NODE*hd, char *id)
{
	SCHOOL_NODE* pSchool = hd;
	TEAM_NODE * pTeam;
	SUBJECT_NODE* pSubject;
	while (pSchool != NULL)
	{
		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			pSubject = pTeam->Shead;
			while (pSubject != NULL)
			{
				if (strcmp(pSubject->ID, id) == 0)
					return pTeam;
				pSubject = pSubject->next;
			}
			pTeam = pTeam->next;
		}
		pSchool = pSchool->next;
	}
	return NULL;
}



/**
* 函数名称: SaveSysData
* 函数功能: 保存系统代码表和三类基础数据.
* 输入参数: hd 主链头结点指针
* 输出参数:
* 返 回 值: BOOL类型, 总是为TRUE
*
* 调用说明:
*/
BOOL SaveSysData(SCHOOL_NODE *hd)
{
	SCHOOL_NODE* pSchool;
	TEAM_NODE *pTeam;
	SUBJECT_NODE *pSubject;
	FILE *pfout;
	int handle;

	pfout = fopen(gp_school_info_filename, "wb");
	for (pSchool = hd; pSchool != NULL; pSchool = pSchool->next)
		fwrite(pSchool, sizeof(SCHOOL_NODE), 1, pfout);
	fclose(pfout);
	pfout = fopen(gp_team_info_filename, "wb");
	for (pSchool = hd; pSchool != NULL; pSchool = pSchool->next)
	{
		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			fwrite(pTeam, sizeof(TEAM_NODE), 1, pfout);
			pTeam = pTeam->next;
		}
	}
	fclose(pfout);

	pfout = fopen(gp_subject_info_filename, "wb");
	for (pSchool = hd; pSchool != NULL; pSchool = pSchool->next)
	{
		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			pSubject = pTeam->Shead;
			while (pSubject != NULL)
			{
				fwrite(pSubject, sizeof(SUBJECT_NODE), 1, pfout);
				pSubject = pSubject->next;
			}
			pTeam = pTeam->next;
		}
	}

	fclose(pfout);
	return TRUE;
}





BOOL ShowModule(char **pString, int n)
{
	LABEL_BUNDLE labels;
	HOT_AREA areas;
	BOOL bRet = TRUE;
	SMALL_RECT rcPop;
	COORD pos;
	WORD att;
	int iHot = 1;
	int i, maxlen, str_len;

	for (i = 0, maxlen = 0; i<n; i++) {
		str_len = strlen(pString[i]);
		if (maxlen < str_len) {
			maxlen = str_len;
		}
	}

	pos.X = maxlen + 6;
	pos.Y = n + 5;
	rcPop.Left = (SCR_COL - pos.X) / 2;
	rcPop.Right = rcPop.Left + pos.X - 1;
	rcPop.Top = (SCR_ROW - pos.Y) / 2;
	rcPop.Bottom = rcPop.Top + pos.Y - 1;

	att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
	labels.num = n;
	labels.ppLabel = pString;
	COORD aLoc[100];

	for (i = 0; i<n; i++) {
		aLoc[i].X = rcPop.Left + 3;
		aLoc[i].Y = rcPop.Top + 2 + i;

	}
	str_len = strlen(pString[n - 1]);
	aLoc[n - 1].X = rcPop.Left + 3 + (maxlen - str_len) / 2;
	aLoc[n - 1].Y = aLoc[n - 1].Y + 2;

	labels.pLoc = aLoc;

	areas.num = 1;
	SMALL_RECT aArea[] = { { aLoc[n - 1].X, aLoc[n - 1].Y,
		aLoc[n - 1].X + 3, aLoc[n - 1].Y } };

	char aSort[] = { 0 };
	char aTag[] = { 1 };

	areas.pArea = aArea;
	areas.pSort = aSort;
	areas.pTag = aTag;
	PopUp(&rcPop, att, &labels, &areas);

	pos.X = rcPop.Left + 1;
	pos.Y = rcPop.Top + 2 + n;
	FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right - rcPop.Left - 1, pos, &ul);

	DealInput(&areas, &iHot);
	PopOff();

	return bRet;

}

/**
* 函数名称: QurrySchoolP
* 函数功能: 查询学院负责人.
* 输入参数: woid
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
* 调用说明:SearchPIC
*/
BOOL QurrySchoolP(void) {
	system("cls");
	char person[12];
	SCHOOL_NODE* temp;
	printf("\n\n请输入负责人姓名:");
	scanf("%s", person);
	if ((temp = SearchPIC(gp_head, person)) != NULL)
	{
		printf("院系名称            负责人      联系电话       \n");
		printf("%-20s%-12s%-15s\n", temp->name, temp->person_in_charge, temp->phone);
	}
	else
		printf("\nNOT FOUND!\n");

	return TRUE;
}
/**
* 函数名称: SearchSchoolPIC
* 函数功能: 遍历链表查询学院负责人.
* 输入参数: 主链头指针	负责人姓名
* 输出参数: 所在院系指针
* 返 回 值: TRUE代表查询正常
*
* 调用说明:SearchPIC
*/
SCHOOL_NODE *SearchPIC(SCHOOL_NODE* hd, char*person)
{
	SCHOOL_NODE *pSchool = hd;
	while (pSchool != NULL)
	{
		if (strcmp(pSchool->person_in_charge, person) == 0)
			return pSchool;
		pSchool = pSchool->next;
	}
	return NULL;

}

/**
* 函数名称: QurryTeamName
* 函数功能: 查询团队名称
* 输入参数: void
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
* 调用说明:SearchPIC
*/

BOOL QurryTeamName(void)
{
    system("cls");
	char s1[] = "团队";
	char s2[] = "负责人";
	char s3[] = "所属学院";
	char s4[] = "教师总数";
	char s5[] = "学生总数";

	char name[20];
	printf("请输入团队名称:");
	scanf("%s", name);
	printf("%-30s%-12s%s	%s		%s\n", s1, s2, s4, s5, s3);
	if (!SeekTeamName(gp_head, name))
		printf("\nNOT FOUND!\n");



	return TRUE;
}/**
* 函数名称: QurryProfAcc
* 函数功能: 查询学院教师人数，输出大于该值的所有团队
* 输入参数: void
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
* 调用说明:
*/

BOOL QurryProfAcc(void)
{
	char s1[] = "学院";
	char s2[] = "负责人";
	char s3[] = "所属学院";
	char s4[] = "教师总数";
	char s5[] = "学生总数";
	system("cls");
	int num, count = 0;
	printf("教师人数:");
	scanf("%d", &num);
	printf("%-30s%-12s%s	%s%s\n", s1, s2, s4, s5, s3);
	SCHOOL_NODE*pSchool;
	TEAM_NODE *pTeam;
	pSchool = gp_head;
	while (pSchool != NULL)
	{
		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			if (pTeam->tea_amount >= num)
			{
				printf("%-30s%-12s	%d	%d	%-20s\n", pTeam->name, pTeam->person_in_charge, pTeam->tea_amount, pTeam->ug_amount, pTeam->school);
				count++;
			}
			pTeam = pTeam->next;
		}
		pSchool = pSchool->next;
	}
	if (count == 0)
		printf("\n没有找到符合要求的团队信息!\n");
	return TRUE;
}
/**
* 函数名称: QurrySubID
* 函数功能: 查询项目编号.
* 输入参数: void
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
* 调用说明:
*/
BOOL QurrySubID(void)
{
	system("cls");
	char ID[15];
	SUBJECT_NODE *temp;
	printf("请输入项目编号:");
	scanf("%s", ID);
	char s1[] = "项目编号";
	char s2[] = "项目类别";
	char s3[] = "资金";
	char s4[] = "负责人;";
	char s5[] = "所属团队";
	char s6[] = "起始时间";
	printf("%-15s%s	%-8s%-8s %-12s%-30s\n",s1,s2,s6,s3,s4,s5);
	if ((temp = SeekProID(gp_head, ID)) != NULL)
		printf("%-15s%c	%-8s  %.2f  %-12s%-30s\n", temp->ID, temp->type, temp->begin_time, temp->fund, temp->person_in_charge, temp->team);
	else printf("\nNot Found!\n");
	return TRUE;
}
/**
* 函数名称: QurryTeamBelonging
* 函数功能: 查询团队所有项目信息
* 输入参数: void
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
*/
BOOL QurryTeamBelong(void)
{
    system("cls");
	printf("输入团队名：");
	char team[30];
	scanf("%s", team);
	getchar();
	SUBJECT_NODE *pSubject;
	TEAM_NODE* pTeam = MatchTeamName(gp_head, team);
	if (pTeam != NULL)
	{
		pSubject = pTeam->Shead;
		printf("项目编号       项目类别     起始时间		 资金		负责人      所属团队\n");
		while (pSubject != NULL)
		{
			printf("%-15s%c		%-8s	%f	%-12s%-30s\n", pSubject->ID, pSubject->type, pSubject->begin_time, pSubject->fund, pSubject->person_in_charge, pSubject->team);
			pSubject = pSubject->next;
		}

	}
	return TRUE;
}

BOOL StatPro2Stu(void)
{
	system("cls");
	statistic_1(gp_head);


	return TRUE;
}

BOOL StatSubjectAcc(void)
{
	system("cls");
	statistic_2(gp_head);
	return TRUE;
}

BOOL StatESubject(void)
{
	system("cls");
	statistic_3(gp_head);
	return TRUE;
}

BOOL StatSub2Pro(void)
{
	system("cls");
	statistic_4(gp_head);
	return TRUE;
}

BOOL Other(void)
{
	system("cls");
	StatSchInfo(gp_head);

	return TRUE;
}

BOOL About(void)
{
	printf("\n\n\n Developed By XXX\n	If you find any problem,contact me on XXXX\n\n");
	return TRUE;
}
BOOL QurrySchoolName(void)
{
	system("cls");
	char name[30];
	printf("\n请输入院系名称:");
	scanf("%s", name);
	printf("院系名称            负责人      联系电话       \n");
	if (!SeekSchoolName(gp_head, name))
		printf("\nNOT FOUND!\n");
	return TRUE;
}
BOOL SeekSchoolName(SCHOOL_NODE *hd, char* name)
{
	BOOL signal = FALSE;
	SCHOOL_NODE *pSchool = hd;
	while (pSchool != NULL)
	{
		if (strstr(pSchool->name, name) != NULL)
		{
			signal = TRUE;
			printf("%-20s%-12s%-15s\n", pSchool->name, pSchool->person_in_charge, pSchool->phone);
		}
		pSchool = pSchool->next;
	}
	return signal;
}
BOOL SeekTeamName(SCHOOL_NODE* hd, char *name)
{
	BOOL signal = FALSE;
	SCHOOL_NODE *pSchool = hd;
	TEAM_NODE *pTeam;
	while (pSchool != NULL)
	{
		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			if (strstr(pTeam->name, name) != NULL)
			{
				signal = TRUE;
				printf("%-30s%-12s	%d	%d		%-20s\n", pTeam->name, pTeam->person_in_charge, pTeam->tea_amount, pTeam->ug_amount, pTeam->school);
			}
			pTeam = pTeam->next;
		}
		pSchool = pSchool->next;
	}
	return signal;
}
/**
* 函数名称: SeekProId
* 函数功能: 遍历列表查询项目编号
* 输入参数: 主链头指针 项目编号
* 输出参数: 所在项目指针
* 返 回 值:
*
* 调用说明:
*/
SUBJECT_NODE* SeekProID(SCHOOL_NODE *hd, char *ID)
{
	SCHOOL_NODE*pSchool;
	TEAM_NODE *pTeam;
	SUBJECT_NODE* pSubject;

	pSchool = hd;
	while (pSchool != NULL)
	{
		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			pSubject = pTeam->Shead;
			while (pSubject != NULL)
			{
				if (strcmp(ID, pSubject->ID) == 0)
					return pSubject;
				pSubject = pSubject->next;
			}
			pTeam = pTeam->next;
		}
		pSchool = pSchool->next;
	}
	return NULL;
}
/**
* 函数名称: MatchTeamName
* 函数功能: 匹配团队信息负责人.
* 输入参数: 主链头指针
* 输出参数: 所在团队指针
* 返 回 值:

* 调用说明:
*/
TEAM_NODE *MatchTeamName(SCHOOL_NODE*hd, char* name)
{
	SCHOOL_NODE* pSchool = hd;
	TEAM_NODE *pTeam;
	while (pSchool != NULL)
	{
		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			if (strcmp(name, pTeam->name) == 0)
				return pTeam;
			pTeam = pTeam->next;
		}
		pSchool = pSchool->next;
	}
	return NULL;

}
/**
* 函数名称: FullfillProInfo
* 函数功能: 填写项目信息
* 输入参数: 对于项目指针
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:SearchPIC
*/
void FullFillProInfo(SUBJECT_NODE *sub)
{
	printf("请输入项目编号;");
	scanf("%s", sub->ID);
	getchar();
	printf("请输入项目类别:");
	scanf("%c", &(sub->type));
	getchar();
	printf("请输入起始时间:");
	scanf("%s", sub->begin_time);
	getchar();
	printf("请输入项目经费:");
	scanf("%f", &(sub->fund));
	getchar();
	printf("请输入负责人:");
	scanf("%s", sub->person_in_charge);
	getchar();
}
/**
* 函数名称: DelProInfo
* 函数功能: 删除课题信息
* 输入参数: BOOL
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
* 调用说明:SearchproID
*/
BOOL DelProInfo(void)
{
	system("cls");
	char ID[15];
	printf("请输入项目编号:");
	scanf("%s", ID);

	getchar();

	SUBJECT_NODE *p1, *p2, *tmp1;
	TEAM_NODE*tmp2 = MatchProID_1(gp_head, ID);

	if (tmp2 != NULL)
		tmp1 = MatchProID_2(gp_head, ID);
	else {
		printf("删除失败！\n");
		return TRUE;
	}

	if (tmp1 != NULL)
	{
		p1 = NULL;
		p2 = tmp2->Shead;
		while (p2 != tmp1&&p2 != NULL)
		{
			p1 = p2;
			p2 = p2->next;
		}
		if (p2 == tmp1)
		{
			if (p1 == NULL)
				tmp2->Shead = p2->next;
			else
				p1->next = p2->next;
			free(p2);
			printf("删除成功！\n");
			return TRUE;
		}
	}
	else printf("删除失败！\n");
	return TRUE;
}


TEAM_NODE* MatchProID_1(SCHOOL_NODE*hd, char* id)
{
	SCHOOL_NODE* pSchool = hd;
	TEAM_NODE *pTeam;
	SUBJECT_NODE *pSubject;
	while (pSchool != NULL)
	{
		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			pSubject = pTeam->Shead;
			while (pSubject != NULL)
			{
				if (strcmp(id, pSubject->ID) == 0)
					return pTeam;
				pSubject = pSubject->next;
			}
			pTeam = pTeam->next;
		}
		pSchool = pSchool->next;
	}
	return NULL;

}
BOOL ModifyProInfo_2(void)
{
	system("cls");
	char id[15];
	printf("请输入项目编号:");
	scanf("%s", id);
	getchar();
	SUBJECT_NODE *temp = MatchProID_2(gp_head, id);
	if (temp != NULL)
	{
		FullFillProInfo(temp);
		printf("修改成功!\n");

		return TRUE;
	}

	else printf("\n未找到该项目,请重试!");
	return FALSE;

}


/*
**
* 函数名称: InsertTeamNode
* 函数功能: 后进先出插入团队节点
* 输入参数: 无
* 输出参数: 无
* 返 回 值: TRUE代表正常
*
*/
SUBJECT_NODE* MatchProID_2(SCHOOL_NODE*hd, char* id)
{
	SCHOOL_NODE* pSchool = hd;
	TEAM_NODE *pTeam;
	SUBJECT_NODE *pSubject;
	//遍历链表查找所需信息
	while (pSchool != NULL)
	{

		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			pSubject = pTeam->Shead;
			while (pSubject != NULL)
			{
				if (strcmp(id, pSubject->ID) == 0)
					return pSubject;
				pSubject = pSubject->next;
			}
			pTeam = pTeam->next;
		}
		pSchool = pSchool->next;
	}
	return NULL;

}

/*
**
* 函数名称: InsertTeamNode
* 函数功能: 后进先出插入团队节点
* 输入参数: 无
* 输出参数: 无
* 返 回 值: TRUE代表正常
*
*/
BOOL InsertTeamNode(void)
{
	system("cls");
	printf("请输入所属院系名称:");
	TEAM_NODE*new_team = (TEAM_NODE*)malloc(sizeof(TEAM_NODE));
	new_team->Shead = NULL;
	scanf("%s", new_team->school);
	getchar();
	SCHOOL_NODE *temp = MatchSchoolName(gp_head, new_team->school);
	if (temp != NULL)
	{
		new_team->next = temp->Thead;
		temp->Thead = new_team;
		FullFillTeamInfo(new_team);
		return TRUE;
	}
	else
		printf("\n未找到该院系,请重试.\n");
	return FALSE;
}

/*
**
* 函数名称: MatchSchoolName
* 函数功能: 查找对应的团队节点返回该队所在学院节点
* 输入参数: 主链头指针 团队名
* 输出参数: pTeam
* 返 回 值: 找到返回对应学院节点，未找到返回NULL
*
*/
SCHOOL_NODE *MatchSchoolName(SCHOOL_NODE*hd, char * school)
{
	SCHOOL_NODE* pSchool = hd;
	while (pSchool != NULL)
	{
		if (strcmp(pSchool->name, school) == 0)
			return pSchool;
		pSchool = pSchool->next;
	}
	return NULL;
}
/**
* 函数名称: FullFillTeamInfo
* 函数功能: 查询学院负责人.
* 输入参数: 对应团队指针
* 输出参数: 无
* 返 回 值: 无
*
* 调用说明:
*/
void FullFillTeamInfo(TEAM_NODE* team)
{
	printf("请输入团队名称:");
	scanf("%s", team->name);
	getchar();
	printf("请输入负责人姓名:");
	scanf("%s", team->person_in_charge);
	getchar();
	printf("请输入教师人数:");
	scanf("%d", &(team->tea_amount));
	getchar();
	printf("请输入研究生人数：");
	scanf("%d", &(team->ug_amount));
	getchar();
}
/**
* 函数名称: 删除团队信息
* 函数功能: 删除团队节点.
* 输入参数: void
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
* 调用说明:MacthTeamNode_step_1,2
*/
BOOL DelTeamNode(void)
{
	system("cls");
	char team[30];
	TEAM_NODE* pT1, *pT2, *tmp2;
	printf("请输入团队名称:");
	scanf("%s", team);
	getchar();
	SCHOOL_NODE *tmp1 = MatchTeamNode_Step_1(gp_head, team);
	if (tmp1 != NULL)
		tmp2 = MatchTeamNode_Step_2(tmp1, team);
	else
	{
		printf("删除失败！\n");
		return TRUE;
	}
	if (tmp2 != NULL)
	{
		pT1 = NULL;
		pT2 = tmp1->Thead;
		while (pT2 != NULL&&pT2 != tmp2)
		{
			pT1 = pT2;
			pT2 = pT2->next;
		}
		if (pT2 == tmp2)
		{
			if (pT1 == NULL)
				tmp1->Thead = pT2->next;
			else pT1->next = pT2->next;
			free(tmp2);
			printf("删除成功\n");
			return TRUE;
		}
	}
	else printf("\n未找到该团队\n");
	return FALSE;
}

/*
**
* 函数名称: MatchTeamNode_Step_1
* 函数功能: 查找对应的团队节点返回该团队所在学院节点
* 输入参数: 主链头指针 团队名
* 输出参数: pTeam
* 返 回 值: 找到返回该团队所在学院节点，未找到返回NULL
*
*/
SCHOOL_NODE *MatchTeamNode_Step_1(SCHOOL_NODE* hd, char * team)
{
	SCHOOL_NODE *pSchool = hd;
	TEAM_NODE *pTeam;
	while (pSchool != NULL)
	{
		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			if (strcmp(pTeam->name, team) == 0)
				return pSchool;
			pTeam = pTeam->next;
		}
		pSchool = pSchool->next;
	}
	return NULL;
}
/*
**
* 函数名称: MatchTeamNode_Step_2
* 函数功能: 查找对应的团队节点返回该团队节点
* 输入参数: 主链头指针 团队名
* 输出参数: pTeam
* 返 回 值: 找到返回对应节点，未找到返回NULL
*
*/
TEAM_NODE *MatchTeamNode_Step_2(SCHOOL_NODE* hd, char *team)
{
	TEAM_NODE* pTeam = hd->Thead;
	while (pTeam != NULL)
	{
		if (strcmp(pTeam->name, team) == 0)
			return pTeam;
		pTeam = pTeam->next;
	}
	printf("未找到该团队\n");
	return NULL;
}
/**
* 函数名称: ModifyTeamInfo
* 函数功能: 查询学院负责人.
* 输入参数: void
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
* 调用说明:FullFillTeamInfo
*/
BOOL ModifyTeamInfo(void)
{
	system("cls");
	char team[30];
	SCHOOL_NODE *pSchool = gp_head;
	TEAM_NODE *pTeam;
	printf("请输入团队名称:");
	scanf("%s", team);
	getchar();

	while (pSchool != NULL)
	{
		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			if (strcmp(pTeam->name, team) == 0)
			{
				FullFillTeamInfo(pTeam);
				return TRUE;
			}
			pTeam = pTeam->next;
		}
		pSchool = pSchool->next;
	}
	printf("\n未找到团队\n");
	return FALSE;
}
/**
* 函数名称: InsertSchoolNode
* 函数功能: 后进先出插入学院节点
* 输入参数: void
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
* 调用说明:FullFillSchInfo
*/
BOOL InsertSchoolNode(void)
{
	system("cls");
	SCHOOL_NODE * nSch = (SCHOOL_NODE*)malloc(sizeof(SCHOOL_NODE));
	nSch->Thead = NULL;
	FullFillSchInfo(nSch);
	nSch->next = gp_head;
	gp_head = nSch;
	printf("输入成功！\n");
	return TRUE;

}

/**
* 函数名称: FullFillSchInfo
* 函数功能: 填写学院信息
* 输入参数: void
* 输出参数: 无
* 返 回 值: 无
*
*/

void FullFillSchInfo(SCHOOL_NODE * new_sch)
{
	printf("\n请输入院系名:");
	scanf("%s", new_sch->name);
	getchar();
	printf("请输入负责人姓名:");
	scanf("%s", new_sch->person_in_charge);
	getchar();
	printf("请输入联系电话:");
	scanf("%s", new_sch->phone);
	getchar();
}

/**
* 函数名称: DelSchoolNode
* 函数功能: 删除学院节点
* 输入参数: BOOL
* 输出参数: 状态参量
* 返 回 值: TRUE代表统计正常
*
*/
BOOL DelSchoolNode(void)
{
	char name[20];
	printf("请输入学院名称：");
	scanf("%s", name);
	getchar();
	SCHOOL_NODE* temp = MatchSchoolName(gp_head, name), *p1, *p2;
	if (temp != NULL)
	{
		p1 = NULL;
		p2 = gp_head;
		while (p2 != NULL&&p2 != temp)
		{
			p1 = p2;
			p2 = p2->next;
		}
		if (p2 == temp)
		{
			if (p1 == NULL)
				gp_head = p2->next;
			else
				p1->next = p2->next;
		}
		free(p2);
		printf("删除成功");
		return  TRUE;
	}
	printf("删除失败!\n");
	return FALSE;
}

/**
* 函数名称: SortSchoolInfo_2
* 函数功能: 按学院师生比对学院信息链表进行排序
* 输入参数: void
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
*/
void SortSchoolInfo_1(SCH_DATA* hd)
{
	SCH_DATA *prior, *after, *current, *temp, *p;
	prior = hd;
	if (prior == NULL)
		return;
	temp = (SCH_DATA*)malloc(sizeof(SCH_DATA));
	while (prior->next != NULL)
	{
		current = prior;
		after = prior->next;
		while (after != NULL)
		{
			if (current->Pro2Stu <after->Pro2Stu)
				current = after;
			after = after->next;
		}
		if (current != prior)
		{
			*temp = *prior;
			*prior = *current;
			prior->next = temp->next;
			temp->next = current->next;
			*current = *temp;
		}
		prior = prior->next;
	}
	char str1[] = "学院";
	char str2[] = "教师总数";
	char str3[] = "学生总数";
	char str4[] = "教师比学生";
	printf("%s	       %s		%s	%s\n", str1, str2, str3, str4);
	for (p = hd; p != NULL; p = p->next)
		{
		    if(p->Stu_Amount==0)
                continue;
		    printf("%s		%d		%d		%f\n", p->name, p->Sch_prof_total, p->Stu_Amount, p->Pro2Stu);
        }
	free(temp);
	return;
}
/**
* 函数名称: SortSchoolInfo_2
* 函数功能: 按学院项目数对学院信息链表进行排序
* 输入参数: void
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
*/
void SortSchoolInfo_2(SCH_DATA* hd)
{
	SCH_DATA *prior, *after, *current, *temp;
	prior = hd;
	if (prior == NULL)
		return;
	temp = (SCH_DATA*)malloc(sizeof(SCH_DATA));
	while (prior->next != NULL)
	{
		current = prior;
		after = prior->next;
		while (after != NULL)
		{
			if (current->sch_pro_num <after->sch_pro_num)
				current = after;
			after = after->next;
		}
		if (current != prior)
		{
			*temp = *prior;
			*prior = *current;
			prior->next = temp->next;
			temp->next = current->next;
			*current = *temp;
		}
		prior = prior->next;
	}
	SCH_DATA *p = hd;
	char s1[] = "学院";
	char s2[] = "973项目数";
	char s3[] = "863项目数";
	char s4[] = "总资金";
	printf("%-20s%-10s%-10s%s\n", s1, s2, s3, s4);
	for (p = hd; p != NULL; p = p->next)
		{
		    if(p->total_fund==0) continue;
            printf("%-20s%-10d%-10d%-12.2f\n", p->name, p->_973_num, p->_863_num, p->total_fund);
        }
}
void SortTeamInfo_1(TEAM_DATA* hd)
{
	TEAM_DATA *prior, *after, *current, *temp;
	prior = hd;
	if (prior == NULL)
		return;
	temp = (TEAM_DATA*)malloc(sizeof(TEAM_DATA));
	while (prior->next != NULL)
	{
		current = prior;
		after = prior->next;
		while (after != NULL)
		{
			if (current->t_pro_amount < after->t_pro_amount)
				current = after;
			after = after->next;
		}
		if (current != prior)
		{
			*temp = *prior;
			*prior = *current;
			prior->next = temp->next;
			temp->next = current->next;
			*current = *temp;
		}
		prior = prior->next;
	}
	int i;
	TEAM_DATA* p;
	char s1[] = "团队名";
	char s2[] = "国家自然基金项目";
	char s3[] = "团队总基金";
	printf("%-30s%-s%s\n", s1, s2, s3);
	for (i = 0, p = hd; i < 10 && p != NULL; i++, p = p->next)
		printf("%-30s%-8d		%-12.2f\n", p->name, p->NA_pro, p->t_total_fund);
}


/**
* 函数名称: SortTeamInfo_2
* 函数功能: 按团队重点项目数对团队信息链表进行排序
* 输入参数: voud
* 输出参数: 状态参量
* 返 回 值: TRUE代表排序正常
*
*/
void SortTeamInfo_2(TEAM_DATA* hd)
{
	TEAM_DATA *prior, *after, *current, *temp;
	prior = hd;
	if (prior == NULL)
		return;
	temp = (TEAM_DATA*)malloc(sizeof(TEAM_DATA));
	while (prior->next != NULL)
	{
		current = prior;
		after = prior->next;
		while (after != NULL)
		{
			if (current->Pro2Prof< after->Pro2Prof)
				current = after;
			after = after->next;
		}
		if (current != prior)
		{
			*temp = *prior;
			*prior = *current;
			prior->next = temp->next;
			temp->next = current->next;
			*current = *temp;
		}
		prior = prior->next;
	}
	int i;
	TEAM_DATA *p;
	char s1[] = "团队名称";
	char s2[] = "项目总数";
	char s3[] = "教师总数";
	char s4[] = "项目比教师";
	printf("%-30s%-8s%-8s%-12s\n", s1, s2, s3, s4);
	for (p = hd, i = 0; i < 5 && p != NULL; p = p->next, i++)
		printf("%-30s%-8d%-8d%-12f\n", p->name, p->t_pro_amount, p->t_tea_amount, p->Pro2Prof);
	return;
}
void StatSchInfo(SCHOOL_NODE* hd)
{
	SCHOOL_NODE *p = hd;
	TEAM_NODE* pTeam;
	SUBJECT_NODE *pSubject;
	char s1[] = "学院";
	char s2[] = "负责人";
	char s3[] = "联系电话";
	printf("%-20s%-12s%-15s\n", s1, s2, s3);
	for (p = hd; p != NULL; p = p->next)
	{
		printf("\n");
		printf("%-20s%-12s%-15s\n", p->name, p->person_in_charge, p->phone);
		pTeam = p->Thead;
		while (p != NULL)
		{
			printf("%-30s%-12s%-15s\n", p->name, p->person_in_charge, p->phone);
			pSubject = pTeam->Shead;
			while (pSubject != NULL)
			{
				printf("%-10s%-12s%-10.2f\n", pSubject->ID, pSubject->person_in_charge, pSubject->fund);
				pSubject = pSubject->next;
			}
			pTeam = pTeam->next;
		}
	}
}

/**
* 函数名称: statistic_1
* 函数功能: 统计相关数据按学院师生比对学院信息链表进行排序
* 输入参数: BOOL
* 输出参数: 状态参量
* 返 回 值: TRUE代表统计正常
*
*/
void statistic_1(SCHOOL_NODE*hd)
{

	SCH_DATA *sdata_hd = NULL;

	TEAM_DATA *tdata_hd = NULL;


	SCHOOL_NODE *pSchool = hd;
	TEAM_NODE* pTeam;
	SUBJECT_NODE* pPro;
	while (pSchool != NULL)
	{
		SCH_DATA *tmp1 = (SCH_DATA*)malloc(sizeof(SCH_DATA));
		tmp1->next = sdata_hd;
		sdata_hd = tmp1;
		//初始化新的学校信息节点
		strcpy(sdata_hd->name, pSchool->name);
		sdata_hd->Sch_prof_total = 0;
		sdata_hd->Stu_Amount = 0;
		sdata_hd->_863_num = 0;
		sdata_hd->_973_num = 0;
		sdata_hd->total_fund = 0;
		sdata_hd->sch_pro_num = 0;

		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			TEAM_DATA* tmp2 = (TEAM_DATA*)malloc(sizeof(TEAM_DATA));
			tmp2->next = tdata_hd;
			tdata_hd = tmp2;

			//初始化新的团队信息节点
			strcpy(tdata_hd->name, pTeam->name);
			tdata_hd->t_tea_amount = pTeam->tea_amount;
			tdata_hd->t_pro_amount = 0;
			tdata_hd->t_total_fund = 0;
			tdata_hd->NA_pro = 0;
			//计算学院信息
			sdata_hd->Stu_Amount += pTeam->ug_amount;
			sdata_hd->Sch_prof_total += pTeam->tea_amount;

			pPro = pTeam->Shead;
			while (pPro != NULL)
			{
				//计算相关数据
				sdata_hd->total_fund += pPro->fund;
				tdata_hd->t_total_fund += pPro->fund;
				if (pPro->type == '1')
					sdata_hd->_973_num++;
				if (pPro->type == '3')
					sdata_hd->_863_num++;
				if (pPro->type == '2')
					tdata_hd->NA_pro++;
				sdata_hd->sch_pro_num++;
				tdata_hd->t_pro_amount++;

				pPro = pPro->next;
			}
			tdata_hd->Pro2Prof = ((float)tdata_hd->t_pro_amount) / ((float)tdata_hd->t_tea_amount);
			pTeam = pTeam->next;
		}
		sdata_hd->Pro2Stu = ((float)sdata_hd->Sch_prof_total) / ((float)sdata_hd->Stu_Amount);
		pSchool = pSchool->next;
	}
	SortSchoolInfo_1(sdata_hd);
}

/**
* 函数名称: statistic_2
* 函数功能: 统计相关数据按学院项目数对学院信息链表进行排序
* 输入参数: BOOL
* 输出参数: 状态参量
* 返 回 值: TRUE代表统计正常
*
*/
void statistic_2(SCHOOL_NODE*hd)
{
	SCH_DATA *sdata_hd = NULL;

	TEAM_DATA *tdata_hd = NULL;


	SCHOOL_NODE *pSchool = hd;
	TEAM_NODE* pTeam;
	SUBJECT_NODE* pPro;
	while (pSchool != NULL)
	{
		SCH_DATA *tmp1 = (SCH_DATA*)malloc(sizeof(SCH_DATA));
		tmp1->next = sdata_hd;
		sdata_hd = tmp1;
		//初始化新的学校信息节点
		strcpy(sdata_hd->name, pSchool->name);
		sdata_hd->Sch_prof_total = 0;
		sdata_hd->Stu_Amount = 0;
		sdata_hd->_863_num = 0;
		sdata_hd->_973_num = 0;
		sdata_hd->total_fund = 0;
		sdata_hd->sch_pro_num = 0;

		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			TEAM_DATA* tmp2 = (TEAM_DATA*)malloc(sizeof(TEAM_DATA));
			tmp2->next = tdata_hd;
			tdata_hd = tmp2;

			//初始化新的团队信息节点
			strcpy(tdata_hd->name, pTeam->name);
			tdata_hd->t_tea_amount = pTeam->tea_amount;
			tdata_hd->t_pro_amount = 0;
			tdata_hd->t_total_fund = 0;
			tdata_hd->NA_pro = 0;
			//计算学院信息
			sdata_hd->Stu_Amount += pTeam->ug_amount;
			sdata_hd->Sch_prof_total += pTeam->tea_amount;

			pPro = pTeam->Shead;
			while (pPro != NULL)
			{
				//计算相关数据
				sdata_hd->total_fund += pPro->fund;
				tdata_hd->t_total_fund += pPro->fund;
				if (pPro->type == '1')
					sdata_hd->_973_num++;
				if (pPro->type == '3')
					sdata_hd->_863_num++;
				if (pPro->type == '2')
					tdata_hd->NA_pro++;
				sdata_hd->sch_pro_num++;
				tdata_hd->t_pro_amount++;

				pPro = pPro->next;
			}
			tdata_hd->Pro2Prof = ((float)tdata_hd->t_pro_amount) / ((float)tdata_hd->t_tea_amount);
			pTeam = pTeam->next;
		}
		sdata_hd->Pro2Stu = ((float)sdata_hd->Sch_prof_total) / ((float)sdata_hd->Stu_Amount);
		pSchool = pSchool->next;
	}
	SortSchoolInfo_2(sdata_hd);
}

/**
* 函数名称: statistic_3
* 函数功能: 统计相关数据,按团队重点项目数对团队信息链表进行排序
* 输入参数: BOOL
* 输出参数: 状态参量
* 返 回 值: TRUE代表统计正常
*
*/
void statistic_3(SCHOOL_NODE*hd)
{
	SCH_DATA *sdata_hd = NULL;

	TEAM_DATA *tdata_hd = NULL;


	SCHOOL_NODE *pSchool = hd;
	TEAM_NODE* pTeam;
	SUBJECT_NODE* pPro;
	while (pSchool != NULL)
	{
		SCH_DATA *tmp1 = (SCH_DATA*)malloc(sizeof(SCH_DATA));
		tmp1->next = sdata_hd;
		sdata_hd = tmp1;
		//初始化新的学校信息节点
		strcpy(sdata_hd->name, pSchool->name);
		sdata_hd->Sch_prof_total = 0;
		sdata_hd->Stu_Amount = 0;
		sdata_hd->_863_num = 0;
		sdata_hd->_973_num = 0;
		sdata_hd->total_fund = 0;
		sdata_hd->sch_pro_num = 0;

		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			TEAM_DATA* tmp2 = (TEAM_DATA*)malloc(sizeof(TEAM_DATA));
			tmp2->next = tdata_hd;
			tdata_hd = tmp2;

			//初始化新的团队信息节点
			strcpy(tdata_hd->name, pTeam->name);
			tdata_hd->t_tea_amount = pTeam->tea_amount;
			tdata_hd->t_pro_amount = 0;
			tdata_hd->t_total_fund = 0;
			tdata_hd->NA_pro = 0;
			//计算学院信息
			sdata_hd->Stu_Amount += pTeam->ug_amount;
			sdata_hd->Sch_prof_total += pTeam->tea_amount;

			pPro = pTeam->Shead;
			while (pPro != NULL)
			{
				//计算相关数据
				sdata_hd->total_fund += pPro->fund;
				tdata_hd->t_total_fund += pPro->fund;
				if (pPro->type == '1')
					sdata_hd->_973_num++;
				if (pPro->type == '3')
					sdata_hd->_863_num++;
				if (pPro->type == '2')
					tdata_hd->NA_pro++;
				sdata_hd->sch_pro_num++;
				tdata_hd->t_pro_amount++;

				pPro = pPro->next;
			}
			tdata_hd->Pro2Prof = ((float)tdata_hd->t_pro_amount) / ((float)tdata_hd->t_tea_amount);
			pTeam = pTeam->next;
		}

		sdata_hd->Pro2Stu = ((float)sdata_hd->Sch_prof_total) / ((float)sdata_hd->Stu_Amount);
		pSchool = pSchool->next;
	}
	SortTeamInfo_1(tdata_hd);
}


/**
* 函数名称: statistic_1
* 函数功能: 统计相关数据按学院师生比对学院信息链表进行排序
* 输入参数: BOOL
* 输出参数: 状态参量
* 返 回 值: TRUE代表统计正常
*
*/
void statistic_4(SCHOOL_NODE*hd)
{
	SCH_DATA *sdata_hd = NULL;

	TEAM_DATA *tdata_hd = NULL;


	SCHOOL_NODE *pSchool = hd;
	TEAM_NODE* pTeam;
	SUBJECT_NODE* pPro;
	while (pSchool != NULL)
	{
		SCH_DATA *tmp1 = (SCH_DATA*)malloc(sizeof(SCH_DATA));
		tmp1->next = sdata_hd;
		sdata_hd = tmp1;
		//初始化新的学校信息节点
		strcpy(sdata_hd->name, pSchool->name);
		sdata_hd->Sch_prof_total = 0;
		sdata_hd->Stu_Amount = 0;
		sdata_hd->_863_num = 0;
		sdata_hd->_973_num = 0;
		sdata_hd->total_fund = 0;
		sdata_hd->sch_pro_num = 0;

		pTeam = pSchool->Thead;
		while (pTeam != NULL)
		{
			TEAM_DATA* tmp2 = (TEAM_DATA*)malloc(sizeof(TEAM_DATA));
			tmp2->next = tdata_hd;
			tdata_hd = tmp2;

			//初始化新的团队信息节点
			strcpy(tdata_hd->name, pTeam->name);
			tdata_hd->t_tea_amount = pTeam->tea_amount;
			tdata_hd->t_pro_amount = 0;
			tdata_hd->t_total_fund = 0;
			tdata_hd->NA_pro = 0;
			//计算学院信息
			sdata_hd->Stu_Amount += pTeam->ug_amount;
			sdata_hd->Sch_prof_total += pTeam->tea_amount;

			pPro = pTeam->Shead;
			while (pPro != NULL)
			{
				//计算相关数据
				sdata_hd->total_fund += pPro->fund;
				tdata_hd->t_total_fund += pPro->fund;
				if (pPro->type == '1')
					sdata_hd->_973_num++;
				if (pPro->type == '3')
					sdata_hd->_863_num++;
				if (pPro->type == '2')
					tdata_hd->NA_pro++;
				sdata_hd->sch_pro_num++;
				tdata_hd->t_pro_amount++;

				pPro = pPro->next;
			}
			tdata_hd->Pro2Prof = ((float)tdata_hd->t_pro_amount) / ((float)tdata_hd->t_tea_amount);
			pTeam = pTeam->next;
		}

		sdata_hd->Pro2Stu = ((float)sdata_hd->Sch_prof_total) / ((float)sdata_hd->Stu_Amount);
		pSchool = pSchool->next;
	}
	SortTeamInfo_2(tdata_hd);
}

/**
* 函数名称: ModifySchoolInfo
* 函数功能: 修改学院信息
* 输入参数: BOOL
* 输出参数: 状态参量
* 返 回 值: TRUE代表查询正常
*
*/
BOOL ModifySchoolInfo()
{
	char name[20];
	printf("请输入学院名：");
	scanf("%s", name);
	getchar();
	SCHOOL_NODE* temp = MatchSchoolName(gp_head, name);
	if (temp != NULL)
	{
		FullFillSchInfo(temp);
		printf("修改成功！");
		return TRUE;
	}
	else printf("未找到该团队！\n");
	return FALSE;
}
