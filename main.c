
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
	SetConsoleCursorInfo(std_out, &lpCur);

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
	COORD endpos = { 0,1 };
	SetConsoleCursorPosition(std_out, endpos);//������ڵڶ���

											  //���˵���Ϊ����,�������Ϊ�˵����,��������Ϊ0(��ť��)
	i = 0;
	do
	{
		PosB = PosA + strlen(main_menu[i]);//��λ��i+1���˵������ʼλ��
		for (j = PosA; j < PosB; j++)
			P_scr_att[j]  |=(i + 1) << 2;  //���ò˵��������ַ���Ԫ������ֵ
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
		(gp_buff_menubar_info + i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
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

BOOL LodaData()
{
	int re = 0;
	if (P_sch_code != NULL)
		free(P_sch_code);
	school_code_len = LodaCode(Pschool_info, &P_sch_code);
	if (school_code_len < 3)
	{
		printf("ѧԺ��������ʧ��\n");
		sys_state &= 0xfe;
	}
	else
	{
		printf("ѧԺ�������سɹ�\n");
		sys_state |= 1;
	}
	if (P_team_code != NULL)
		free(P_team_code);
	team_code_len = LodaCode(Pteam_info, &P_team_code);
	if (team_code_len < 4)
	{
		printf("�ŶӴ�������ʧ��\n");
		sys_state &= ~2;
	}else 
	{
		printf("ѧ�����������سɹ�\n");
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
		//���ݼ�����ʾ��Ϣ
		printf("\nϵͳ�������ݲ�����\n");
		printf("\n�����������...\n");
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
	if ((handle = open(filename, O_RDONLY | O_TEXT)) == -1)//�����ֻ����ʽ��ʧ��
		handle = open(filename, O_CREAT | O_TEXT, S_IREAD);//�Դ�����ʽ��
	filelen = _filelength(handle);//�����ļ��ĳ���
	pTemp = (char *)calloc(filelen + 1, sizeof(char));//����ͬ����С�Ķ�̬����
	BufferLen = read(handle, pTemp, filelen);//�������ļ�����ȫ�����뵽�ڴ�
	_close(handle);

	*(pTemp + BufferLen) = '\0';//�ڶ�̬����ȥĩβ��һ��/0��Ϊ�ַ���������־
	BufferLen++;

	//����̬�����������л��з��滻Ϊ���ַ�
	for (i = 0; i < BufferLen; i++)
		if (*(pTemp + 1) == '\n')
			*(pTemp + i) = '\0';

	//������һ��ͬ����С�Ĵ��������ڴ���������봮
	*PPbuffer = (char*)calloc(BufferLen, sizeof(char));
	loc2 = 0;
	str1 = pTemp;
	len = strlen(str1);


	//ѡ������
	while (BufferLen > len + 1)
	{
		loc1 = len + 1;
		while (BufferLen > loc1)//ÿ���ҵ���������С�Ĵ��봮���׵�ַ����str1
		{
			str2 = pTemp + loc1;
			if (strcmp(str1, str2) > 0)
				str1 = str2;
			loc1 += strlen(str2) + 1;
		}
		len = strlen(str1);//�����ҵ�����С���봮����

		if (len > 0)//������ǿմ������и��ƣ�loc2���¸���С���봮��ŵ�ַ��ƫ����
		{
			strcpy(*PPbuffer + loc2, str1);
			loc2 += len + 1;//�Ѹ��ƵĴ��봮��ռ�洢�ռ��С
		}
		//����С���봮��������ɾ����
		for (i = 0; i < BufferLen - (str1 - pTemp) - (len + 1); i++)
			*(str1 + i) = *(str1 + i + len + 1);
		BufferLen -= len + 1;
		str1 = pTemp;
		len = strlen(str1);
	}//������ֻʣ��һ�����봮ʱ�������
	len = strlen(str1);
	//�������Ĵ��봮
	strcpy(*PPbuffer + loc2,str1);

	//�޸Ķ�̬��������Сʹ�����÷��������Ĵ��봮
	loc2 = len + 1;
	*PPbuffer = (char *)realloc(*PPbuffer, loc2);
	free(pTemp);
	return loc2;//���ش�Ŵ��봮���ڴ滺����ʵ�ʴ�С




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
		printf("ѧԺ��Ϣ�����ļ���ʧ�ܣ�\n");
		return re;
	}
	printf("ѧԺ��Ϣ�����ļ��򿪳ɹ���\n");

	//�������ļ��ж�ȡѧԺ��Ϣ���ݣ������Ժ���ȳ��ķ�ʽ������������
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
		printf("ѧԺ��Ϣ�����ļ�����ʧ��\n");
		return re;
	}
	printf("ѧԺ��Ϣ�����ļ����سɹ�\n");
	re += 8;

	//�������ļ��ж�ȡ�Ŷӻ�����Ϣ������������Ӧ�ڵ��ѧ��������Ϣ֧����
	while (fread(&tmp2, sizeof(team), 1, pfile) == 1)
	{
		//�����ڵ㣬��Ŵ������ļ��ж�����Ŷӻ�����Ϣ
		p_team = (team*)malloc(sizeof(team));
		*p_team = tmp2;
		p_team->head = NULL;

		//�������ϲ����Ŷ�����ѧԺ��ѧԺ�ڵ�
		p_sch = hd;
		while (p_sch != NULL&&strcmp(p_sch->name, p_team->school) != 0)
			p_sch = p_sch->next;
		if (p_sch != NULL)//����ҵ����򽫽ڵ��Ժ���ȳ��ķ�ʽ�����Ŷ���Ϣ֧��
		{
			p_team->next = p_sch->head;
			p_sch->head = p_team;
		}
		else
			free(p_team);
		if ((pfile = fopen(Psubject_info, "rb")) == NULL)
		{
			printf("��Ŀ��Ϣ�����ļ���ʧ��\n");
			return re;
		}
		printf("��Ŀ��Ϣ�����ļ��򿪳ɹ�\n");
		re += 16;
		//�������ļ��ж�ȡ��Ŀ��Ϣ���ݣ������Ŷӻ�����Ϣ֧����Ӧ��Ŀ�ڵ�
		while (fread(&tmp3, sizeof(subject), 1, pfile) == 1)
		{
			//�����ڵ�����Ŀ��Ϣ
			p_sub = (subject*)malloc(sizeof(subject));
			*p_sub = tmp3;
			
			//���ҿ�����Ϣ֧���϶�Ӧ�Ŷ���Ϣ�ڵ�
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
			if (find)//����ҵ�����ȳ�
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

	if (num != gi_sel_menu)//���ָ�����˵�������ѡ�в˵�
	{
		if (top_layer->LayerNo != 0)//����Ѿ����Ӳ˵�����
		{
			PopOff();
			gi_sel_menu = 0;
		}
	}
	else if (top_layer->LayerNo != 0)//���ѵ����Ӳ˵�,�򷵻�
		return;

	gi_sel_menu = num;//��ѡ�е����˵�����Ϊ���˵���
	TagMainMenu(gi_sel_menu);//��ѡ�е����˵��������
	LocSubMenu(gi_sel_menu, &rcPop);//���㵯���Ӳ˵�������λ��,�����rcpop��

	//������Ӳ˵��еĵ�һ�������˵��ַ��������е��±�
	for (i = 1; i < gi_sel_sub_menu; i++)
		loc += sub_menu_count[i - 1];
	//�������Ӳ˵������������ǩ���ṹ����
	labels.ppLabel = sub_menu + loc;//��ǩ����һ���ַ���ǩ�ĵ�ַ
	labels.num = sub_menu_count[gi_sel_menu - 1];//��ǩ���б�ǩ�ַ����ĸ���*/
	COORD aLoc[labels.num];//����һ������������ÿ����ǩ�ַ������λ�õ�����
	for (i = 0; i < labels.num; i++)//ȷ����ǩ�ַ��������λ�ô��������������
	{
		aLoc[i].X = rcPop.Left + 2;
		aLoc[i].Y = rcPop.Top + i + 1;
	}
	labels.pLoc = aLoc;//ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��
	//����������Ϣ
	areas.num = labels.num;//�����ĸ������ڱ�ǩ�ĸ������Ӳ˵��ĸ���
	SMALL_RECT aArea [areas.num];//���������������������λ��
	char aSort[areas.num];//��������������������Ӧ���
	char aTag[areas.num];//����������ÿ�������ı��

	for (i = 0; i < areas.num; i++)
	{
		aArea[i].Left = rcPop.Left + 2;
		aArea[i].Top = rcPop.Top + i + 1;
		aArea[i].Right = rcPop.Right - 2;
		aArea[i].Bottom = aArea->Top;
		aSort[i] = 0;//�������Ϊ0(��ť��)
		aTag[i] = i + 1;//������˳����
	}
	areas.pArea = aArea;//ʹ�����ṹ����areas�ĳ�ԱpAreaָ��λ��������������Ԫ��
	areas.pSort = aSort; //ʹ�����ṹ����areas�ĳ�ԱpSortָ�����������������Ԫ��
	areas.pTag = aTag;//ʹ�����ṹ����areas�ĳ�ԱpTagָ����������������Ԫ��

	att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;//�׵׺���
	PopUp(&rcPop, att, &labels, &areas);
	DrawBox(&rcPop);//���������ڻ��߿�
	pos.X = rcPop.Left + 2;
	for (pos.Y = rcPop.Top + 1; pos.Y < rcPop.Bottom; pos.Y++)
	{
		//��ѭ�������ڿմ��Ӳ˵���λ�û����γɷָ���ȡ���β˵������������
		pCh = sub_menu[loc + pos.Y - rcPop.Top - 1];
		if (strlen(pCh) == 0)//�մ�
		{
			//draw line
			FillConsoleOutputCharacter(std_out, '-', rcPop.Right - rcPop.Left - 3, pos, &ul);
			for (j = rcPop.Left + 2; j < rcPop.Right - 1; j++)
				//ȡ���������ַ���Ԫ����������
				P_scr_att[pos.Y*SCR_COL + j] &= 3;//������λ
		}
	}
	//���Ӳ˵���Ĺ�����Ϊ�׵׺���
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
//��ָ���������������Ϣͬʱ��������������������Ϣ��ջ
//pRc �����������ݴ�ŵ�ַ
//patt ���������ַ���������
void PopUp(SMALL_RECT* pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
	LAYER_NODE *nextLayer;
	COORD size;
	COORD pos = { 0,0 };
	char *pCh;
	int i, j, row;
	/*������������λ���ַ���Ԫ��Ϣ��ջ*/
	size.X = pRc->Right - pRc->Left + 1;    /*�������ڵĿ��*/
	size.Y = pRc->Bottom - pRc->Top + 1;    /*�������ڵĸ߶�*/
											/*�����ŵ������������Ϣ�Ķ�̬�洢��*/
	nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
	nextLayer->next = top_layer;
	nextLayer->LayerNo = top_layer->LayerNo + 1;
	nextLayer->rcArea = *pRc;
	nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y * sizeof(CHAR_INFO));
	nextLayer->pScrAtt = (char *)malloc(size.X*size.Y * sizeof(char));
	pCh = nextLayer->pScrAtt;
	//���������ڸ���������ַ���Ϣ����,���ڹرյ�������ʱ�ָ�ԭ��
	ReadConsoleOutput(std_out, nextLayer->pContent, size, pos, pRc);
	for (i = pRc->Left; i <= pRc->Bottom; i++)
		for (j = pRc->Left; j <= pRc->Right; j++)
		{
			*pCh = P_scr_att[i*SCR_COL + j];
			pCh++;
		}//����ѭ�����������ַ���Ԫԭ������ֵ���붯̬������,���ڻָ�
	top_layer = nextLayer;//��ɵ������������Ϣ��ջ����
	//���õ������������ַ���������
	pos.X = pRc->Left;
	pos.Y = pRc->Top;
	for (i = pRc->Top; i <= pRc->Bottom; i++)
	{
		FillConsoleOutputAttribute(std_out, att, size.X, pos, &ul);
		pos.Y++;
	}
	//����ǩ���еı�ǩ�ַ������趨λ�����
	for (i = 0; i < pLabel->num; i++)
	{
		pCh = pLabel->ppLabel[i];
		if (strlen(pCh) == 0)
			WriteConsoleOutputCharacter(std_out, pCh, strlen(pCh), pLabel->pLoc[i], &ul);
	}
	//���õ��������ַ������ַ���Ԫ��������
	for (i = pRc->Top; i <= pRc->Right; i++)
		for (j = pRc->Top; j <= pRc->Right; j++)
			P_scr_att[i*SCR_COL + j] = top_layer->LayerNo;
	//����ѭ�����õ�Ԫ�Ĳ��
	for (i = 0; i < pHotArea->num; i++)
	{	//�����������ַ���Ԫ���������ͺ��������
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
	int cNo, cAtt;//cn0:�ַ���Ԫ���,cAtt��Ԫ����
	char vkc, asc;// vkc:���������,asc:�ַ���ASCII��ֵ

	while (bRet)
	{
		//�ӿ���̨���뻺���ж�һ����¼
		ReadConsoleInput(std_in, &inRec, 1, &res);

		if (inRec.EventType == MOUSE_EVENT)
		{
			pos = inRec.Event.MouseEvent.dwMousePosition;//��ȡ���λ��
			cNo = P_scr_att[pos.Y*SCR_COL + pos.X] & 3;//ȡ��λ�ò��
			cAtt = P_scr_att[pos.Y*SCR_COL + pos.X] >> 2;//ȡ���ַ���Ԫ����
			if (cNo == 0)//���Ϊ0������λ��δ���Ӳ˵�����
						 //cAtt>0�����λ�ô�������(���˵����ַ���Ԫ)
						 //cAtt!=gi_sek_menu��ʾ��λ�����˵�δ��ѡ��
						 //top_layer->layerNo>0��ʾ���Ӳ˵�����
			{
				if (cAtt > 0 && cAtt != gi_sel_menu&&top_layer->LayerNo > 0)
				{
					PopOff();//�رյ������Ӳ˵�
					gi_sel_sub_menu = 0;//��ѡ�е��Ӳ˵������Ϊ0
					PopMenu(cAtt);//��������������˵���Ķ�Ӧ���Ӳ˵�
				}
			}
			else if (cAtt > 0)//�������λ�õ����Ӳ˵��Ĳ˵����ַ���Ԫ
				TagSubMenu(cAtt);//�ڸ��Ӳ˵�����ѡ�б��
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
			{//��Ų�Ϊ0������λ�ñ������Ӳ˵�����
				if (cAtt > 0)
				{
					PopOff();//�رյ����Ӳ˵�
					gi_sel_sub_menu = 0;//��ѡ���Ӳ˵��������
										//ִ�ж�Ӧ���ܺ���:gi_sel_meny,catt�Ӳ˵����
					bRet = ExeFunction(gi_sel_menu, cAtt);
				}
			}
		}
		else if (inRec.EventType == KEY_EVENT&&inRec.Event.KeyEvent.bKeyDown)
		{
			vkc = inRec.Event.KeyEvent.wVirtualKeyCode;//��ȡ�������������
			asc = inRec.Event.KeyEvent.uChar.AsciiChar;//��ȡ������asc��
													   //ϵͳ�Ŀ�ݼ�����
			if (vkc == 112)//f1
			{
				/*if (top_layer->LayerNo != 0)//���û���Ӳ˵�
				{
				PopOff();//�ر��Ӳ˵�
				gi_sel_sub_menu = 0;
				}*/
				PopMenu(1);
				//���а������⹦�ܺ���
			}
			else if (vkc == 113)//f2
				PopMenu(2);
			else if (vkc == 114)//f3
				PopMenu(3);
			else if (vkc == 115)//f4
				PopMenu(4);
			else if (vkc == 116)
				PopMenu(5);
			else if (asc == 0)//�������Ƽ��Ĵ���
			{
				if (top_layer->LayerNo == 0)//���δ�����Ӳ˵�
				{
					switch (vkc)//�������(��,��,��),����Ӧ����
					{
					case 37://left
						gi_sel_menu--;
						if (gi_sel_menu == 0)//�������ڵ�һ����ѡ����
							gi_sel_menu = 4;
						TagMainMenu(gi_sel_menu);
						break;
					case 39://right
						gi_sel_menu++;
						if (gi_sel_menu == 6)//���ѡ�е������ѡ��
							gi_sel_menu = 1;
						TagMainMenu(gi_sel_menu);
						break;
					case 40://down
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
						break;
					}
				}
				else //�ѵ����Ӳ˵�ʱ
				{
					for (loc = 0, i = 1; i < gi_sel_sub_menu; i++)
						loc += sub_menu_count[i - 1];//������Ӳ˵��ĵ�һ�����Ӳ˵�֧���������е�λ��
					switch (vkc)//���������
					{
					case 37://left 
							//ѡ������ѡ��
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
			else if ((asc - vkc == 0) || (asc - vkc == 32))//������ͨ��
			{
				if (top_layer->LayerNo == 0)//���δ�����Ӳ˵�
					if (vkc == 13)//�س�
					{
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
					}
			}
			else //�ѵ����Ӳ˵�ʱ�ļ������봦��
			{
				if (vkc == 27)//esc
				{
					PopOff();
					gi_sel_sub_menu = 0;
				}
				else if (vkc == 13)//�س�
				{
					num = gi_sel_sub_menu;
					PopOff();
					gi_sel_sub_menu = 0;
					bRet = ExeFunction(gi_sel_menu, num);
				}
				else {//������ͨ������
					for (i = loc; i < loc + sub_menu_count[gi_sel_menu - 1]; i++)
						loc += sub_menu_count[i - 1];
					//�����뵱ǰ�Ӳ˵�ÿһ��Ĵ����ַ����бȽ�
					for (i = loc; i < loc + sub_menu_count[gi_sel_sub_menu - 1]; i++)
					{
						if (strlen(sub_menu[i])> 0 && vkc == sub_menu[i][1] && (vkc == (sub_menu[i][1] - 'A' + 'a')))
						{
							//���ƥ��ɹ�
							PopOff();
							gi_sel_menu = 0;
							bRet = ExeFunction(gi_sel_menu, i - loc + 1);
						}
					}
				}
			}
		}
		/*//���������(��)alt��
		else if (inRec.Event.KeyEvent.dwControlKeyState&(LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
		{
		switch (vkc)
		{
		case 88://ALT+X �˳�
		if (top_layer->LayerNo != 0)//���ǵ�ǰ����
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
* ��������: PopOff
* ��������: �رն��㵯������, �ָ���������ԭ��ۺ��ַ���Ԫԭ����.
* �������: ��
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
*/
void PopOff(void)
{
	LAYER_NODE *nextLayer;
	COORD size;
	COORD pos = { 0, 0 };
	char *pCh;
	int i, j;

	if ((top_layer->next == NULL) || (top_layer->pContent == NULL))
	{   /*ջ�״�ŵ���������Ļ��Ϣ�����ùر�*/
		return;
	}
	nextLayer = top_layer->next;
	/*�ָ�������������ԭ���*/
	size.X = top_layer->rcArea.Right - top_layer->rcArea.Left + 1;
	size.Y = top_layer->rcArea.Bottom - top_layer->rcArea.Top + 1;
	WriteConsoleOutput(std_out, top_layer->pContent, size, pos, &(top_layer->rcArea));
	/*�ָ��ַ���Ԫԭ����*/
	pCh = top_layer->pScrAtt;
	for (i = top_layer->rcArea.Top; i <= top_layer->rcArea.Bottom; i++)
	{
		for (j = top_layer->rcArea.Left; j <= top_layer->rcArea.Right; j++)
		{
			P_scr_att[i*SCR_COL + j] = *pCh;
			pCh++;
		}
	}
	free(top_layer->pContent);    /*�ͷŶ�̬�洢��*/
	free(top_layer->pScrAtt);
	free(top_layer);
	top_layer = nextLayer;
	gi_sel_sub_menu = 0;
	return;
}

void DrawBox(SMALL_RECT *pRc)
{
	char chBox[] = { '+','-','|' };  /*�����õ��ַ�*/
	COORD pos = { pRc->Left, pRc->Top };  /*��λ����������Ͻ�*/

	WriteConsoleOutputCharacter(std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
	for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
	{   /*��ѭ�����ϱ߿����*/
		WriteConsoleOutputCharacter(std_out, &chBox[1], 1, pos, &ul);
	}
	pos.X = pRc->Right;
	WriteConsoleOutputCharacter(std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
	for (pos.Y = pRc->Top + 1; pos.Y < pRc->Bottom; pos.Y++)
	{   /*��ѭ�����߿�����ߺ��ұ���*/
		pos.X = pRc->Left;
		WriteConsoleOutputCharacter(std_out, &chBox[2], 1, pos, &ul);
		pos.X = pRc->Right;
		WriteConsoleOutputCharacter(std_out, &chBox[2], 1, pos, &ul);
	}
	pos.X = pRc->Left;
	pos.Y = pRc->Bottom;
	WriteConsoleOutputCharacter(std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
	for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
	{   /*���±߿����*/
		WriteConsoleOutputCharacter(std_out, &chBox[1], 1, pos, &ul);
	}
	pos.X = pRc->Right;
	WriteConsoleOutputCharacter(std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
	return;
}
/**
* ��������: TagSubMenu
* ��������: ��ָ���Ӳ˵�������ѡ�б��.
* �������: num ѡ�е��Ӳ˵����
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
*/
void TagSubMenu(int num)
{
	SMALL_RECT rcPop;
	COORD pos;
	WORD att;
	int width;

	LocSubMenu(gi_sel_menu, &rcPop);  /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
	if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom - rcPop.Top - 1))
	{   /*����Ӳ˵����Խ�磬������Ӳ˵��ѱ�ѡ�У��򷵻�*/
		return;
	}

	pos.X = rcPop.Left + 2;
	width = rcPop.Right - rcPop.Left - 3;
	if (gi_sel_sub_menu != 0) /*����ȡ��ԭѡ���Ӳ˵����ϵı��*/
	{
		pos.Y = rcPop.Top + gi_sel_sub_menu;
		att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
		FillConsoleOutputAttribute(std_out, att, width, pos, &ul);
		pos.X += 1;
		att |= FOREGROUND_RED;/*�׵׺���*/
		FillConsoleOutputAttribute(std_out, att, 1, pos, &ul);
	}
	/*���ƶ��Ӳ˵�������ѡ�б��*/
	pos.X = rcPop.Left + 2;
	pos.Y = rcPop.Top + num;
	att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
	FillConsoleOutputAttribute(std_out, att, width, pos, &ul);
	gi_sel_sub_menu = num;  /*�޸�ѡ���Ӳ˵����*/
	return;
}
/*
**
* ��������: LocSubMenu
* �������� : ���㵯���Ӳ˵��������ϽǺ����½ǵ�λ��.
* ������� : num ѡ�е����˵����
* ������� : rc �������λ����Ϣ�ĵ�ַ
* �� �� ֵ : ��
*
* ����˵�� :
	*/
void LocSubMenu(int num, SMALL_RECT *rc)
{
	int i, len, loc = 0;

	rc->Top = 1; /*������ϱ߶��ڵ�2�У��к�Ϊ1*/
	rc->Left = 1;
	for (i = 1; i<num; i++)
	{   /*����������߽�λ��, ͬʱ�����һ���Ӳ˵������Ӳ˵��ַ��������е�λ��*/
		rc->Left += strlen(main_menu[i - 1]) + 4;
		loc += sub_menu_count[i - 1];
	}
	rc->Right = strlen(sub_menu[loc]);/*��ʱ��ŵ�һ���Ӳ˵����ַ�������*/
	for (i = 1; i<sub_menu_count[num - 1]; i++)
	{   /*������Ӳ˵��ַ��������䳤�ȴ����rc->Right*/
		len = strlen(sub_menu[loc + i]);
		if (rc->Right < len)
		{
			rc->Right = len;
		}
	}
	rc->Right += rc->Left + 3;  /*����������ұ߽�*/
	rc->Bottom = rc->Top + sub_menu_count[num - 1] + 1;/*���������±ߵ��к�*/
	if (rc->Right >= SCR_COL)  /*�ұ߽�Խ��Ĵ���*/
	{
		len = rc->Right - SCR_COL + 1;
		rc->Left -= len;
		rc->Right = SCR_COL - 1;
	}
	return;
}
/**
	*��������: DealInput
	* �������� : �ڵ�������������������, �ȴ�����Ӧ�û�����.
	* ������� : pHotArea
	*           piHot ����������ŵĴ�ŵ�ַ, ��ָ�򽹵�������ŵ�ָ��
	* ������� : piHot ����굥�������س���ո�ʱ���ص�ǰ�������
	* �� �� ֵ :
	* ����˵�� :
*/
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
	INPUT_RECORD inRec;
	DWORD res;
	COORD pos = { 0,0 };
	int num, arrow, iRet = 0;
	int cNo, cTag, cSort;//cno ���,cTag�������,cSort:��������
	char vkc, asc;//vkc:���������,asc;�ַ���ascii��ֵ

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
					//�����
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
			else if (vkc == VK_SPACE || vkc == 13)//�ո��س�
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
	att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED; //�ڵװ���
	att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;//�ڵװ���
	for (i = 0; i < pHotArea->num; i++)
	{
		pos.X = pHotArea->pArea[i].Left;
		pos.Y = pHotArea->pArea[i].Top;
		width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
		if (pHotArea->pSort[i] == 0)
			//�����ǰ�ť��
			FillConsoleOutputAttribute(std_out, att2, width, pos, &ul);
	}
	pos.X = pHotArea->pArea[iHot - 1].Left;
	pos.Y = pHotArea->pArea[iHot - 1].Top;
	width = pHotArea->pArea[iHot - 1].Right - pHotArea->pArea[iHot - 1].Left + 1;
	if (pHotArea->pSort[iHot - 1] == 0)
		FillConsoleOutputAttribute(std_out, att1, width, pos, &ul);
	else if (pHotArea->pSort[iHot] == 1)//��������ı�������
	{
		SetConsoleCursorPosition(std_out, pos);
		SetConsoleCursorInfo(std_out, &lpCur);
		lpCur.bVisible = TRUE;
		SetConsoleCursorInfo(std_out, &lpCur);
	}
}
/**
* ��������: ExeFunction
* ��������: ִ�������˵��ź��Ӳ˵���ȷ���Ĺ��ܺ���.
* �������: m ���˵����
*           s �Ӳ˵����
* �������: ��
* �� �� ֵ: BOOL����, TRUE �� FALSE
*
* ����˵��: ����ִ�к���ExitSysʱ, �ſ��ܷ���FALSE, ������������Ƿ���TRUE
*/
BOOL ExeFunction(int m, int n)
{
	BOOL bRet = TRUE;
	int i, loc;
	//����ָ������ ����������й��ܺ��������;
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
	for (i = 1, loc = 0; i < m; i++)//�������˵��ź��Ӳ˵��ż�������±�
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

	att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
	char *pCh[] = { "ȷ���˳�ϵͳ��", "ȷ��    ȡ��" };
	int iHot = 1;

	pos.X = strlen(pCh[0]) + 6;
	pos.Y = 7;
	rcPop.Left = (SCR_COL - pos.X) / 2;
	rcPop.Right = rcPop.Left + pos.X - 1;
	rcPop.Top = (SCR_ROW - pos.Y) / 2;
	rcPop.Bottom = rcPop.Top + pos.Y - 1;

	att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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

	while (pSchool != NULL)//�Ƚ��ȳ�
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

	SetConsoleTitle("���н���");


}






