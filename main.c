
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
	LodaData();
	InitInterface();
	RunSys(&sch_top);
	CloseSys(sch_top);
	return 0;
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
	SetConsoleCursorInfo(std_out, &lpCur);

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
	COORD endpos = { 0,1 };
	SetConsoleCursorPosition(std_out, endpos);//光标设于第二行

											  //将菜单设为热区,热区编号为菜单编号,热区类型为0(按钮型)
	i = 0;
	do
	{
		PosB = PosA + strlen(main_menu[i]);//定位第i+1个菜单项的起始位置
		for (j = PosA; j < PosB; j++)
			P_scr_att[j]  |=(i + 1) << 2;  //设置菜单项所在字符单元的属性值
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
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
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

BOOL LodaData()
{
	int re = 0;
	if (P_sch_code != NULL)
		free(P_sch_code);
	school_code_len = LodaCode(Pschool_info, &P_sch_code);
	if (school_code_len < 3)
	{
		printf("学院代码表加载失败\n");
		sys_state &= 0xfe;
	}
	else
	{
		printf("学院代码表加载成功\n");
		sys_state |= 1;
	}
	if (P_team_code != NULL)
		free(P_team_code);
	team_code_len = LodaCode(Pteam_info, &P_team_code);
	if (team_code_len < 4)
	{
		printf("团队代码表加载失败\n");
		sys_state &= ~2;
	}else 
	{
		printf("学生类别代码表加载成功\n");
		sys_state |= 2;
	}
	if (P_subj_code != NULL)
		free(P_subj_code);
	subj_code_len = LodaCode(Psubject_info, &P_subj_code);
	//todo
	//if(team_code_len)
	re = CreatList(&sch_top);
	sys_state |= re;
	sys_state |= ~(4 + 8 + 16 - re);
	if (sys_state < (1 | 2 | 4 | 8 | 16))
	{
		//数据加载提示信息
		printf("\n系统基础数据不完整\n");
		printf("\n按任意键继续...\n");
		getch();
	}
	return TRUE;



}
int  LodaCode(char *filename, char **PPbuffer)
{
	char *pTemp, *str1, *str2;
	int handle;
	int BufferLen, len, loc1, loc2, i;
	long filelen;
	if ((handle = open(filename, O_RDONLY | O_TEXT)) == -1)//如果以只读方式打开失败
		handle = open(filename, O_CREAT | O_TEXT, S_IREAD);//以创建方式打开
	filelen = _filelength(handle);//数据文件的长度
	pTemp = (char *)calloc(filelen + 1, sizeof(char));//申请同样大小的动态储存
	BufferLen = read(handle, pTemp, filelen);//将数据文件内容全部读入到内存
	_close(handle);

	*(pTemp + BufferLen) = '\0';//在动态储存去末尾存一个/0作为字符串结束标志
	BufferLen++;

	//将动态储存区中所有换行符替换为空字符
	for (i = 0; i < BufferLen; i++)
		if (*(pTemp + 1) == '\n')
			*(pTemp + i) = '\0';

	//再申请一块同样大小的储存区用于存放排序后代码串
	*PPbuffer = (char*)calloc(BufferLen, sizeof(char));
	loc2 = 0;
	str1 = pTemp;
	len = strlen(str1);


	//选择排序
	while (BufferLen > len + 1)
	{
		loc1 = len + 1;
		while (BufferLen > loc1)//每次找到序列中最小的代码串，首地址存入str1
		{
			str2 = pTemp + loc1;
			if (strcmp(str1, str2) > 0)
				str1 = str2;
			loc1 += strlen(str2) + 1;
		}
		len = strlen(str1);//本次找到的最小代码串长度

		if (len > 0)//如果不是空串，进行复制，loc2是下个最小代码串存放地址的偏移量
		{
			strcpy(*PPbuffer + loc2, str1);
			loc2 += len + 1;//已复制的代码串所占存储空间大小
		}
		//将最小代码串从序列中删除掉
		for (i = 0; i < BufferLen - (str1 - pTemp) - (len + 1); i++)
			*(str1 + i) = *(str1 + i + len + 1);
		BufferLen -= len + 1;
		str1 = pTemp;
		len = strlen(str1);
	}//序列中只剩下一个代码串时排序结束
	len = strlen(str1);
	//复制最后的代码串
	strcpy(*PPbuffer + loc2,str1);

	//修改动态储存区大小使其正好放下排序后的代码串
	loc2 = len + 1;
	*PPbuffer = (char *)realloc(*PPbuffer, loc2);
	free(pTemp);
	return loc2;//返回存放代码串的内存缓存区实际大小




}
int CreatList(school **phead)
{
	school *hd = NULL, *p_sch, tmp1;
	team *p_team, tmp2;
	subject *p_sub,tmp3;
	FILE *pfile;
	int find, re = 0;

	if ((pfile = fopen(Pschool_info, "rb")) == NULL)
	{
		printf("学院信息数据文件打开失败！\n");
		return re;
	}
	printf("学院信息数据文件打开成功！\n");

	//从数据文件中读取学院信息数据，存入以后进先出的方式建立在主链中
	while (fread(&tmp1, sizeof(school), 1, pfile) == 1)
	{
		p_sch = (school*)malloc(sizeof(school));
		*p_sch = tmp1;
		p_sch->head = NULL;
		p_sch->next = hd;
		hd = p_sch;

	}
	fclose(pfile);
	if(hd==NULL)
	{
		printf("学院信息数据文件加载失败\n");
		return re;
	}
	printf("学院信息数据文件加载成功\n");
	re += 8;

	//从数据文件中读取团队基本信息，存入主链对应节点的学生基本信息支链中
	while (fread(&tmp2, sizeof(team), 1, pfile) == 1)
	{
		//创建节点，存放从数据文件中读入的团队基本信息
		p_team = (team*)malloc(sizeof(team));
		*p_team = tmp2;
		p_team->head = NULL;

		//在主链上查找团队所在学院的学院节点
		p_sch = hd;
		while (p_sch != NULL&&strcmp(p_sch->name, p_team->school) != 0)
			p_sch = p_sch->next;
		if (p_sch != NULL)//如果找到，则将节点以后进先出的方式插入团队信息支链
		{
			p_team->next = p_sch->head;
			p_sch->head = p_team;
		}
		else
			free(p_team);
		if ((pfile = fopen(Psubject_info, "rb")) == NULL)
		{
			printf("项目信息数据文件打开失败\n");
			return re;
		}
		printf("项目信息数据文件打开成功\n");
		re += 16;
		//从数据文件中读取项目信息数据，存入团队基本信息支链对应项目节点
		while (fread(&tmp3, sizeof(subject), 1, pfile) == 1)
		{
			//创建节点存放项目信息
			p_sub = (subject*)malloc(sizeof(subject));
			*p_sub = tmp3;
			
			//查找课题信息支链上对应团队信息节点
			p_sch= hd;
			find = 0;
			while (p_sch != NULL&&find == 0)
			{
				p_team = p_sch->head;
				while (p_team != 0 && find == 0)
				{
					if(strcmp(p_team->name,p_sub->team)==0)
					{
						find = 1;
						break;
					}
					p_team = p_team->next;
				}
				p_sch = p_sch->next;
			}
			if (find)//如果找到后进先出
			{
				p_sub->next = p_team->head;
				p_team->head = p_sub;
			}else {
				free(p_sub);
			}
		}
	
	}
	fclose(pfile);
	return re;
}

void PopMenu(int num)
{
	LABEL_BUNDLE labels;
	HOT_AREA areas;
	SMALL_RECT rcPop;
	COORD pos;
	WORD att;
	char *pCh;
	int i, j, loc = 0;

	if (num != gi_sel_menu)//如果指定主菜单不是已选中菜单
	{
		if (top_layer->LayerNo != 0)//如果已经有子菜单弹出
		{
			PopOff();
			gi_sel_menu = 0;
		}
	}
	else if (top_layer->LayerNo != 0)//若已弹出子菜单,则返回
		return;

	gi_sel_menu = num;//将选中的主菜单项置为主菜单项
	TagMainMenu(gi_sel_menu);//在选中的主菜单上做标记
	LocSubMenu(gi_sel_menu, &rcPop);//计算弹出子菜单的区域位置,存放在rcpop中

	//计算该子菜单中的第一项在主菜单字符串数组中的下标
	for (i = 1; i < gi_sel_sub_menu; i++)
		loc += sub_menu_count[i - 1];
	//将改组子菜单项项名存入标签束结构变量
	labels.ppLabel = sub_menu + loc;//标签束第一个字符标签的地址
	labels.num = sub_menu_count[gi_sel_menu - 1];//标签束中标签字符串的个数*/
	COORD aLoc[labels.num];//定义一个坐标数组存放每个标签字符串输出位置的坐标
	for (i = 0; i < labels.num; i++)//确定标签字符串的输出位置存放在坐标数组中
	{
		aLoc[i].X = rcPop.Left + 2;
		aLoc[i].Y = rcPop.Top + i + 1;
	}
	labels.pLoc = aLoc;//使标签束结构变量labels的成员pLoc指向坐标数组的首元素
	//设置热区信息
	areas.num = labels.num;//热区的个数等于标签的个数即子菜单的个数
	SMALL_RECT aArea [areas.num];//定义数组所存放所有热区位置
	char aSort[areas.num];//定义数组存放所有热区对应类别
	char aTag[areas.num];//定义数组存放每个热区的编号

	for (i = 0; i < areas.num; i++)
	{
		aArea[i].Left = rcPop.Left + 2;
		aArea[i].Top = rcPop.Top + i + 1;
		aArea[i].Right = rcPop.Right - 2;
		aArea[i].Bottom = aArea->Top;
		aSort[i] = 0;//热区类别都为0(按钮型)
		aTag[i] = i + 1;//热区按顺序编号
	}
	areas.pArea = aArea;//使热区结构变量areas的成员pArea指向位置数组热区的首元素
	areas.pSort = aSort; //使热区结构变量areas的成员pSort指向类别数组热区的首元素
	areas.pTag = aTag;//使热区结构变量areas的成员pTag指向热区编号数组的首元素

	att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;//白底黑字
	PopUp(&rcPop, att, &labels, &areas);
	DrawBox(&rcPop);//给弹出窗口画边框
	pos.X = rcPop.Left + 2;
	for (pos.Y = rcPop.Top + 1; pos.Y < rcPop.Bottom; pos.Y++)
	{
		//此循环用来在空串子菜单项位置划线形成分隔并取消次菜单项的热区属性
		pCh = sub_menu[loc + pos.Y - rcPop.Top - 1];
		if (strlen(pCh) == 0)//空串
		{
			//draw line
			FillConsoleOutputCharacter(std_out, '-', rcPop.Right - rcPop.Left - 3, pos, &ul);
			for (j = rcPop.Left + 2; j < rcPop.Right - 1; j++)
				//取消改区域字符单元的热区属性
				P_scr_att[pos.Y*SCR_COL + j] &= 3;//保留两位
		}
	}
	//将子菜单项的功能设为白底红字
	pos.X = rcPop.Left + 3;
	att = FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
	for (pos.Y = rcPop.Top + 1; pos.Y < rcPop.Bottom; pos.Y++)
	{
		if (strlen(sub_menu[loc + pos.Y - rcPop.Top - 1]) == 0)
			continue;
		FillConsoleOutputAttribute(std_out, att, 1, pos, &ul);
	}
	return;
}
//在指定区域输出窗口信息同时设置热区将弹出窗口信息入栈
//pRc 弹出窗口数据存放地址
//patt 弹出窗口字符区域属性
void PopUp(SMALL_RECT* pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
	LAYER_NODE *nextLayer;
	COORD size;
	COORD pos = { 0,0 };
	char *pCh;
	int i, j, row;
	/*弹出窗口所在位置字符单元信息入栈*/
	size.X = pRc->Right - pRc->Left + 1;    /*弹出窗口的宽度*/
	size.Y = pRc->Bottom - pRc->Top + 1;    /*弹出窗口的高度*/
											/*申请存放弹出窗口相关信息的动态存储区*/
	nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
	nextLayer->next = top_layer;
	nextLayer->LayerNo = top_layer->LayerNo + 1;
	nextLayer->rcArea = *pRc;
	nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y * sizeof(CHAR_INFO));
	nextLayer->pScrAtt = (char *)malloc(size.X*size.Y * sizeof(char));
	pCh = nextLayer->pScrAtt;
	//将弹出窗口覆盖区域的字符信息保存,用于关闭弹出窗口时恢复原样
	ReadConsoleOutput(std_out, nextLayer->pContent, size, pos, pRc);
	for (i = pRc->Left; i <= pRc->Bottom; i++)
		for (j = pRc->Left; j <= pRc->Right; j++)
		{
			*pCh = P_scr_att[i*SCR_COL + j];
			pCh++;
		}//二重循环将所覆盖字符单元原先属性值存入动态储存区,便于恢复
	top_layer = nextLayer;//完成弹出窗口相关信息入栈操作
	//设置弹出窗口区域字符的新属性
	pos.X = pRc->Left;
	pos.Y = pRc->Top;
	for (i = pRc->Top; i <= pRc->Bottom; i++)
	{
		FillConsoleOutputAttribute(std_out, att, size.X, pos, &ul);
		pos.Y++;
	}
	//将标签束中的标签字符串在设定位置输出
	for (i = 0; i < pLabel->num; i++)
	{
		pCh = pLabel->ppLabel[i];
		if (strlen(pCh) == 0)
			WriteConsoleOutputCharacter(std_out, pCh, strlen(pCh), pLabel->pLoc[i], &ul);
	}
	//设置弹出窗口字符区域字符单元的新属性
	for (i = pRc->Top; i <= pRc->Right; i++)
		for (j = pRc->Top; j <= pRc->Right; j++)
			P_scr_att[i*SCR_COL + j] = top_layer->LayerNo;
	//二重循环设置单元的层号
	for (i = 0; i < pHotArea->num; i++)
	{	//设置热区中字符单元的热区类型和热区编号
		row = pHotArea->pArea[i].Top;
		for (j = pHotArea->pArea[i].Left; j <= pHotArea->pArea[i].Right; j++)
			P_scr_att[row*SCR_COL + j] |= (pHotArea->pSort[i] << 6) | (pHotArea->pTag[i] << 2);

	}
	return;
}

void RunSys(school **P_sch)
{
	INPUT_RECORD inRec;
	DWORD res;
	COORD pos = { 0,0 };
	BOOL bRet = TRUE;
	int i, loc, num;
	int cNo, cAtt;//cn0:字符单元层号,cAtt单元属性
	char vkc, asc;// vkc:虚拟键代码,asc:字符的ASCII码值

	while (bRet)
	{
		//从控制台输入缓存中读一条记录
		ReadConsoleInput(std_in, &inRec, 1, &res);

		if (inRec.EventType == MOUSE_EVENT)
		{
			pos = inRec.Event.MouseEvent.dwMousePosition;//获取鼠标位置
			cNo = P_scr_att[pos.Y*SCR_COL + pos.X] & 3;//取该位置层号
			cAtt = P_scr_att[pos.Y*SCR_COL + pos.X] >> 2;//取该字符单元属性
			if (cNo == 0)//层号为0表名该位置未被子菜单覆盖
						 //cAtt>0表面该位置处于热区(主菜单项字符单元)
						 //cAtt!=gi_sek_menu表示该位置主菜单未被选中
						 //top_layer->layerNo>0表示有子菜单弹出
			{
				if (cAtt > 0 && cAtt != gi_sel_menu&&top_layer->LayerNo > 0)
				{
					PopOff();//关闭弹出的子菜单
					gi_sel_sub_menu = 0;//将选中的子菜单项号置为0
					PopMenu(cAtt);//弹出鼠标所在主菜单项的对应的子菜单
				}
			}
			else if (cAtt > 0)//鼠标所在位置弹出子菜单的菜单项字符单元
				TagSubMenu(cAtt);//在该子菜单项做选中标记
			if (inRec.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				if (cNo == 0)
					PopMenu(cAtt);
				else if (top_layer->LayerNo > 0)
				{
					PopOff();
					gi_sel_sub_menu = 0;
				}

			}
			else
			{//层号不为0表明该位置被弹出子菜单覆盖
				if (cAtt > 0)
				{
					PopOff();//关闭弹出子菜单
					gi_sel_sub_menu = 0;//将选中子菜单香号置零
										//执行对应功能函数:gi_sel_meny,catt子菜单项号
					bRet = ExeFunction(gi_sel_menu, cAtt);
				}
			}
		}
		else if (inRec.EventType == KEY_EVENT&&inRec.Event.KeyEvent.bKeyDown)
		{
			vkc = inRec.Event.KeyEvent.wVirtualKeyCode;//获取按键的虚拟键码
			asc = inRec.Event.KeyEvent.uChar.AsciiChar;//获取案件的asc码
													   //系统的快捷键处理
			if (vkc == 112)//f1
			{
				/*if (top_layer->LayerNo != 0)//如果没有子菜单
				{
				PopOff();//关闭子菜单
				gi_sel_sub_menu = 0;
				}*/
				PopMenu(1);
				//运行帮助主题功能函数
			}
			else if (vkc == 113)//f2
				PopMenu(2);
			else if (vkc == 114)//f3
				PopMenu(3);
			else if (vkc == 115)//f4
				PopMenu(4);
			else if (vkc == 116)
				PopMenu(5);
			else if (asc == 0)//其他控制键的处理
			{
				if (top_layer->LayerNo == 0)//如果未弹出子菜单
				{
					switch (vkc)//处理方向键(左,右,下),不响应其他
					{
					case 37://left
						gi_sel_menu--;
						if (gi_sel_menu == 0)//如果光标在第一个主选单上
							gi_sel_menu = 4;
						TagMainMenu(gi_sel_menu);
						break;
					case 39://right
						gi_sel_menu++;
						if (gi_sel_menu == 6)//如果选中第五个主选单
							gi_sel_menu = 1;
						TagMainMenu(gi_sel_menu);
						break;
					case 40://down
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
						break;
					}
				}
				else //已弹出子菜单时
				{
					for (loc = 0, i = 1; i < gi_sel_sub_menu; i++)
						loc += sub_menu_count[i - 1];//计算该子菜单的第一项在子菜单支付串数组中的位置
					switch (vkc)//方向键处理
					{
					case 37://left 
							//选中左侧的选项
						gi_sel_menu--;
						if (gi_sel_menu < 1)
							gi_sel_sub_menu = 5;
						TagMainMenu(gi_sel_menu);
						PopOff();
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
						break;
					case 38://up
						num = gi_sel_sub_menu - 1;
						if (num < 1)
							num = sub_menu_count[gi_sel_sub_menu - 1];
						if (strlen(sub_menu[loc + num - 1]) == 0)
							num--;
						TagSubMenu(num);
						break;
					case 39:
						gi_sel_menu++;
						if (gi_sel_menu > 5)
							gi_sel_menu = 1;
						TagMainMenu(gi_sel_menu);
						PopOff();
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
						break;
					case 40://down
						num = gi_sel_sub_menu + 1;
						if (num > sub_menu_count[gi_sel_sub_menu - 1])
							num = 1;
						if (strlen(sub_menu[loc + num - 1]) == 0)
							num++;
						TagSubMenu(num);
						break;
					}
				}
			}
			else if ((asc - vkc == 0) || (asc - vkc == 32))//按下普通键
			{
				if (top_layer->LayerNo == 0)//如果未弹出子菜单
					if (vkc == 13)//回车
					{
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
					}
			}
			else //已弹出子菜单时的键盘输入处理
			{
				if (vkc == 27)//esc
				{
					PopOff();
					gi_sel_sub_menu = 0;
				}
				else if (vkc == 13)//回车
				{
					num = gi_sel_sub_menu;
					PopOff();
					gi_sel_sub_menu = 0;
					bRet = ExeFunction(gi_sel_menu, num);
				}
				else {//其他普通键处理
					for (i = loc; i < loc + sub_menu_count[gi_sel_menu - 1]; i++)
						loc += sub_menu_count[i - 1];
					//依次与当前子菜单每一项的代表字符进行比较
					for (i = loc; i < loc + sub_menu_count[gi_sel_sub_menu - 1]; i++)
					{
						if (strlen(sub_menu[i])> 0 && vkc == sub_menu[i][1] && (vkc == (sub_menu[i][1] - 'A' + 'a')))
						{
							//如果匹配成功
							PopOff();
							gi_sel_menu = 0;
							bRet = ExeFunction(gi_sel_menu, i - loc + 1);
						}
					}
				}
			}
		}
		/*//如果按下左(右)alt键
		else if (inRec.Event.KeyEvent.dwControlKeyState&(LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
		{
		switch (vkc)
		{
		case 88://ALT+X 退出
		if (top_layer->LayerNo != 0)//不是当前界面
		{
		PopOff();
		gi_sel_sub_menu = 0;
		}
		bRet = ExeFunction(1, 4);
		break;
		case 70://ALT+F
		PopMenu(1);
		break;

		default:
		break;
		}
		}*/
	}
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

	if ((top_layer->next == NULL) || (top_layer->pContent == NULL))
	{   /*栈底存放的主界面屏幕信息，不用关闭*/
		return;
	}
	nextLayer = top_layer->next;
	/*恢复弹出窗口区域原外观*/
	size.X = top_layer->rcArea.Right - top_layer->rcArea.Left + 1;
	size.Y = top_layer->rcArea.Bottom - top_layer->rcArea.Top + 1;
	WriteConsoleOutput(std_out, top_layer->pContent, size, pos, &(top_layer->rcArea));
	/*恢复字符单元原属性*/
	pCh = top_layer->pScrAtt;
	for (i = top_layer->rcArea.Top; i <= top_layer->rcArea.Bottom; i++)
	{
		for (j = top_layer->rcArea.Left; j <= top_layer->rcArea.Right; j++)
		{
			P_scr_att[i*SCR_COL + j] = *pCh;
			pCh++;
		}
	}
	free(top_layer->pContent);    /*释放动态存储区*/
	free(top_layer->pScrAtt);
	free(top_layer);
	top_layer = nextLayer;
	gi_sel_sub_menu = 0;
	return;
}

void DrawBox(SMALL_RECT *pRc)
{
	char chBox[] = { '+','-','|' };  /*画框用的字符*/
	COORD pos = { pRc->Left, pRc->Top };  /*定位在区域的左上角*/

	WriteConsoleOutputCharacter(std_out, &chBox[0], 1, pos, &ul);/*画边框左上角*/
	for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
	{   /*此循环画上边框横线*/
		WriteConsoleOutputCharacter(std_out, &chBox[1], 1, pos, &ul);
	}
	pos.X = pRc->Right;
	WriteConsoleOutputCharacter(std_out, &chBox[0], 1, pos, &ul);/*画边框右上角*/
	for (pos.Y = pRc->Top + 1; pos.Y < pRc->Bottom; pos.Y++)
	{   /*此循环画边框左边线和右边线*/
		pos.X = pRc->Left;
		WriteConsoleOutputCharacter(std_out, &chBox[2], 1, pos, &ul);
		pos.X = pRc->Right;
		WriteConsoleOutputCharacter(std_out, &chBox[2], 1, pos, &ul);
	}
	pos.X = pRc->Left;
	pos.Y = pRc->Bottom;
	WriteConsoleOutputCharacter(std_out, &chBox[0], 1, pos, &ul);/*画边框左下角*/
	for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
	{   /*画下边框横线*/
		WriteConsoleOutputCharacter(std_out, &chBox[1], 1, pos, &ul);
	}
	pos.X = pRc->Right;
	WriteConsoleOutputCharacter(std_out, &chBox[0], 1, pos, &ul);/*画边框右下角*/
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
		FillConsoleOutputAttribute(std_out, att, width, pos, &ul);
		pos.X += 1;
		att |= FOREGROUND_RED;/*白底红字*/
		FillConsoleOutputAttribute(std_out, att, 1, pos, &ul);
	}
	/*在制定子菜单项上做选中标记*/
	pos.X = rcPop.Left + 2;
	pos.Y = rcPop.Top + num;
	att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
	FillConsoleOutputAttribute(std_out, att, width, pos, &ul);
	gi_sel_sub_menu = num;  /*修改选中子菜单项号*/
	return;
}
/*
**
* 函数名称: LocSubMenu
* 函数功能 : 计算弹出子菜单区域左上角和右下角的位置.
* 输入参数 : num 选中的主菜单项号
* 输出参数 : rc 存放区域位置信息的地址
* 返 回 值 : 无
*
* 调用说明 :
	*/
void LocSubMenu(int num, SMALL_RECT *rc)
{
	int i, len, loc = 0;

	rc->Top = 1; /*区域的上边定在第2行，行号为1*/
	rc->Left = 1;
	for (i = 1; i<num; i++)
	{   /*计算区域左边界位置, 同时计算第一个子菜单项在子菜单字符串数组中的位置*/
		rc->Left += strlen(main_menu[i - 1]) + 4;
		loc += sub_menu_count[i - 1];
	}
	rc->Right = strlen(sub_menu[loc]);/*暂时存放第一个子菜单项字符串长度*/
	for (i = 1; i<sub_menu_count[num - 1]; i++)
	{   /*查找最长子菜单字符串，将其长度存放在rc->Right*/
		len = strlen(sub_menu[loc + i]);
		if (rc->Right < len)
		{
			rc->Right = len;
		}
	}
	rc->Right += rc->Left + 3;  /*计算区域的右边界*/
	rc->Bottom = rc->Top + sub_menu_count[num - 1] + 1;/*计算区域下边的行号*/
	if (rc->Right >= SCR_COL)  /*右边界越界的处理*/
	{
		len = rc->Right - SCR_COL + 1;
		rc->Left -= len;
		rc->Right = SCR_COL - 1;
	}
	return;
}
/**
	*函数名称: DealInput
	* 函数功能 : 在弹出窗口区域设置热区, 等待并相应用户输入.
	* 输入参数 : pHotArea
	*           piHot 焦点热区编号的存放地址, 即指向焦点热区编号的指针
	* 输出参数 : piHot 用鼠标单击、按回车或空格时返回当前热区编号
	* 返 回 值 :
	* 调用说明 :
*/
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
	INPUT_RECORD inRec;
	DWORD res;
	COORD pos = { 0,0 };
	int num, arrow, iRet = 0;
	int cNo, cTag, cSort;//cno 层号,cTag热区编号,cSort:热区类型
	char vkc, asc;//vkc:虚拟键代码,asc;字符的ascii码值

	SetHotPoint(pHotArea, *piHot);
	while (TRUE)
	{
		//loop
		ReadConsoleInput(std_in, &inRec, 1, &res);
		if ((inRec.EventType == MOUSE_EVENT) && (inRec.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED))
		{
			pos = inRec.Event.MouseEvent.dwMousePosition;
			cNo = P_scr_att[pos.Y*SCR_COL + pos.X] & 3;
			cTag = (P_scr_att[pos.Y*SCR_COL + pos.X] >> 2) & 0xf;
			cSort = (P_scr_att[pos.Y*SCR_COL + pos.X] >> 6) & 3;

			if ((cNo == top_layer->LayerNo) && cTag > 0)
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
		else if (inRec.EventType == KEY_EVENT&&inRec.Event.KeyEvent.bKeyDown)
		{
			vkc == inRec.Event.KeyEvent.wVirtualKeyCode;
			asc = inRec.Event.KeyEvent.uChar.AsciiChar;
			if (asc == 0)
			{
				arrow = 0;
				switch (vkc)
				{
					//方向键
				case VK_LEFT://left
					arrow = 1;
					break;
				case VK_UP://up
					arrow = 2;
					break;
				case VK_RIGHT:
					arrow = 3;
					break;
				case VK_DOWN:
					arrow = 4;
					break;
				}
				if (arrow > 0)
				{
					num = *piHot;
					while (TRUE)
					{
						num = *piHot;
						while (TRUE)

						{
							if (arrow < 3)
								num--;
							else
								num++;
							if ((num < 1) || (num > pHotArea->num) || ((arrow % 2) && (pHotArea->pArea[num - 1].Top == pHotArea->pArea[*piHot - 1].Top)) || ((!(arrow % 2)) && (pHotArea->pArea[num - 1].Top != pHotArea->pArea[*piHot - 1].Top)))
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
			else if (vkc == VK_ESCAPE)
			{
				iRet = 27;
				break;
			}
			else if (vkc == VK_SPACE || vkc == 13)//空格或回车
			{
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
	COORD pos = { 0,0 };
	int i,width;
	WORD att1, att2;
	att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED; //黑底白字
	att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;//黑底白字
	for (i = 0; i < pHotArea->num; i++)
	{
		pos.X = pHotArea->pArea[i].Left;
		pos.Y = pHotArea->pArea[i].Top;
		width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
		if (pHotArea->pSort[i] == 0)
			//热区是按钮类
			FillConsoleOutputAttribute(std_out, att2, width, pos, &ul);
	}
	pos.X = pHotArea->pArea[iHot - 1].Left;
	pos.Y = pHotArea->pArea[iHot - 1].Top;
	width = pHotArea->pArea[iHot - 1].Right - pHotArea->pArea[iHot - 1].Left + 1;
	if (pHotArea->pSort[iHot - 1] == 0)
		FillConsoleOutputAttribute(std_out, att1, width, pos, &ul);
	else if (pHotArea->pSort[iHot] == 1)//被激活的文本框类型
	{
		SetConsoleCursorPosition(std_out, pos);
		SetConsoleCursorInfo(std_out, &lpCur);
		lpCur.bVisible = TRUE;
		SetConsoleCursorInfo(std_out, &lpCur);
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
BOOL ExeFunction(int m, int n)
{
	BOOL bRet = TRUE;
	int i, loc;
	//函数指针数组 用来存放所有功能函数的入口;
	BOOL(*pFunction[18])(void);// todo[sub_menu_count[0] + sub_menu_count[1] + sub_menu_count[2] + sub_menu_count[3] + sub_menu_count[4]])(void);
	pFunction[0] = MaintainSchool;
	pFunction[1] = MaintainTeam;
	pFunction[2] = MaintainSubject;
	pFunction[3] = NULL;
	pFunction[4] = QurrySchoolP;
	pFunction[5] = QurrySchoolName;
	pFunction[6] = QurryTeamName;
	pFunction[7] = QurryProAcc;
	pFunction[8] = QurrySubjNum;
	pFunction[9] = QurrySubjName;
	pFunction[10] = NULL;
	pFunction[11] = StatPro2Stu;
	pFunction[12] = StatSubType;
	pFunction[13] = StatMostFund;
	pFunction[14] = StatSub2Prof;
	pFunction[15] = NULL;
	pFunction[16] = ABOUT;
	pFunction[17] = ExitSys;
	for (i = 1, loc = 0; i < m; i++)//根据主菜单号和子菜单号计算对于下标
		loc += sub_menu_count[i - 1];
	loc += n - 1;
	if (pFunction[loc] != NULL)
		bRet = (*pFunction[loc])();
	return bRet;
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
	COORD aLoc[n];

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
	FillConsoleOutputCharacter(std_out, '-', rcPop.Right - rcPop.Left - 1, pos, &ul);

	DealInput(&areas, &iHot);
	PopOff();

	return bRet;

}
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
	FillConsoleOutputCharacter(std_out, '-', rcPop.Right - rcPop.Left - 1, pos, &ul);

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
void CloseSys(school *P_sch)
{
	school *pSchool = P_sch, *pSchool_2;
	team *pTeam_1, *pTeam_2;
	subject *pSubject_1, *pSubject_2;

	while (pSchool != NULL)//先进先出
	{
		pSchool_2 = pSchool->next;
		pTeam_1 = pSchool->head;
		while (pTeam_1 != NULL)
		{
			pTeam_2 = pTeam_1->next;
			pSubject_1 = pTeam_1->head;
			while (pSubject_1 != NULL)
			{
				pSubject_2 = pSubject_1->next;
				free(pSubject_1);
				pSubject_1 = pSubject_2;
			}
			free(pTeam_1);
			pTeam_1 = pTeam_2;
		}
		free(pSchool);
		pSchool = pSchool_2;
	}
	Clear();
	free(gp_buff_menubar_info);
	free(gp_buff_stateBar_info);
	free(P_sch_code);
	free(P_team_code);
	free(P_subj_code);

	CloseHandle(std_out);
	CloseHandle(std_in);

	SetConsoleTitle("运行结束");


}






