#include "practicum.h"

unsigned long ul;

int main()
{
	COORD size = { SCR_COL, SCR_ROW };              /*���ڻ�������С*/

	gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼����豸���*/
	gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* ��ȡ��׼�����豸���*/

	SetConsoleTitle(gp_sys_name);                 /*���ô��ڱ���*/
	SetConsoleScreenBufferSize(gh_std_out, size); /*���ô��ڻ�������С80*25*/

	LoadData();                   /*���ݼ���*/
	InitInterface();          /*�����ʼ��*/
	RunSys(&gp_head);             /*ϵͳ����ģ���ѡ������*/
	CloseSys(gp_head);            /*�˳�ϵͳ*/

	return 0;
}

/**
* ��������: LoadData
* ��������: ������������������ݴ������ļ����뵽�ڴ滺������ʮ��������.
* �������: ��
* �������: ��
* �� �� ֵ: BOOL����, ���ܺ����г��˺���ExitSys�ķ���ֵ����ΪFALSE��,
*           ���������ķ���ֵ����ΪTRUE.
*
* ����˵��: Ϊ���ܹ���ͳһ�ķ�ʽ���ø����ܺ���, ����Щ���ܺ�����ԭ����Ϊ
*           һ��, ���޲����ҷ���ֵΪBOOL. ����ֵΪFALSEʱ, ������������.
*/
BOOL LoadData()
{
	int Re = 0;

	Re = CreatList(&gp_head);
	gc_sys_state |= Re;
	gc_sys_state &= ~(4 + 8 + 16 - Re);
	if (gc_sys_state < (1 | 2 | 4 | 8 | 16))
	{  /*���ݼ�����ʾ��Ϣ*/
		printf("\nϵͳ�������ݲ�����!\n");
		printf("\n�����������...\n");
		getch();
	}

	return TRUE;
}

/**
* ��������: LoadCode
* ��������: �������������ļ����뵽�ڴ滺����, �����������ȥ���ո�.
* �������: FileName ��Ŵ����������ļ���.
* �������: pBuffer ָ���ڴ滺������ָ������ĵ�ַ.
* �� �� ֵ: ��Ŵ������ڴ滺������С(���ֽ�Ϊ��λ).
*
* ����˵��:
*/
int LoadCode(char *FileName, char **pBuffer)
{
	char *pTemp, *pStr1, *pStr2;
	int handle;
	int BufferLen, len, loc1, loc2, i;
	long filelen;

	if ((handle = open(FileName, O_RDONLY | O_TEXT)) == -1) /*�����ֻ����ʽ��ʧ�� */
	{
		handle = open(FileName, O_CREAT | O_TEXT, S_IREAD); /*�Դ�����ʽ��*/
	}
	filelen = filelength(handle);      /*�����ļ��ĳ���*/
	pTemp = (char *)calloc(filelen + 1, sizeof(char)); /*����ͬ����С�Ķ�̬�洢��*/
	BufferLen = read(handle, pTemp, filelen); /*�������ļ�������ȫ�����뵽�ڴ�*/
	close(handle);

	*(pTemp + BufferLen) = '\0'; /*�ڶ�̬�洢��β��һ�����ַ�����Ϊ�ַ���������־*/
	BufferLen++;

	for (i = 0; i<BufferLen; i++) /*����̬�洢���е����л��з��滻�ɿ��ַ�*/
	{
		if (*(pTemp + i) == '\n')
		{
			*(pTemp + i) = '\0';
		}
	}

	/*������һ��ͬ����С�Ķ�̬�洢�������ڴ�������Ĵ��봮*/
	*pBuffer = (char *)calloc(BufferLen, sizeof(char));
	loc2 = 0;
	pStr1 = pTemp;
	len = strlen(pStr1);

	while (BufferLen > len + 1) /*ѡ������*/
	{
		loc1 = len + 1;
		while (BufferLen > loc1) /*ÿ���ҵ���������С���봮���׵�ַ����pStr1*/
		{
			pStr2 = pTemp + loc1;
			if (strcmp(pStr1, pStr2) > 0)
			{
				pStr1 = pStr2;
			}
			loc1 += strlen(pStr2) + 1;
		}
		len = strlen(pStr1);  /*��һ�����ҵ�����С���봮����*/

							  /*������ǿմ�������и��ƣ�loc2����һ����С���봮��ŵ�ַ��ƫ����*/
		if (len > 0)
		{
			strcpy(*pBuffer + loc2, pStr1);
			loc2 += len + 1;  /*�Ѹ��ƵĴ��봮��ռ�洢�ռ��С*/
		}

		/*����С���봮��������ɾ����*/
		for (i = 0; i<BufferLen - (pStr1 - pTemp) - (len + 1); i++)
		{
			*(pStr1 + i) = *(pStr1 + i + len + 1);
		}

		BufferLen -= len + 1; /*��һ���������������еĳ���*/
		pStr1 = pTemp;  /*�ٶ����еĵ�һ�����봮Ϊ��С���봮*/
		len = strlen(pStr1);
	} /*������ֻʣ��һ�����봮ʱ���������*/

	  /*�������������봮*/
	len = strlen(pStr1);
	strcpy(*pBuffer + loc2, pStr1);

	/*�޸Ķ�̬�洢����С��ʹ�����÷����������봮*/
	loc2 += len + 1;
	*pBuffer = (char *)realloc(*pBuffer, loc2);
	free(pTemp);  /*�ͷ���������Ķ�̬�洢��*/

	return loc2;  /*���ش�Ŵ��봮���ڴ滺����ʵ�ʴ�С*/
}

/**
* ��������: CreatList
* ��������: �������ļ���ȡ��������, ����ŵ���������ʮ��������.
* �������: ��
* �������: phead ����ͷָ��ĵ�ַ, ����������������ʮ����.
* �� �� ֵ: int����ֵ, ��ʾ�����������.
*           0  ����, ������
*           4  �Ѽ���ѧԺ��Ϣ���ݣ����Ŷӻ�����Ϣ�Ϳ�����Ϣ����
*           12 �Ѽ���ѧԺ��Ϣ���Ŷӻ�����Ϣ���ݣ��޿�����Ϣ����
*           28 ����������ݶ��Ѽ���
*
* ����˵��:
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
		printf("ѧԺ��Ϣ�����ļ���ʧ��!\n");
		return re;
	}
	printf("ѧԺ��Ϣ�����ļ��򿪳ɹ�!\n");

	/*�������ļ��ж�ѧԺ��Ϣ���ݣ������Ժ���ȳ���ʽ������������*/
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
		printf("ѧԺ��Ϣ�����ļ�����ʧ��!\n");
		return re;
	}
	printf("ѧԺ��Ϣ�����ļ����سɹ�!\n");
	*phead = hd;
	re += 4;

	if ((pFile = fopen(gp_team_info_filename, "rb")) == NULL)
	{
		printf("�Ŷӻ�����Ϣ�����ļ���ʧ��!\n");
		return re;
	}
	printf("�Ŷӻ�����Ϣ�����ļ��򿪳ɹ�!\n");
	re += 8;

	/*�������ļ��ж�ȡ�Ŷӻ�����Ϣ���ݣ�����������Ӧ�����Ŷӻ�����Ϣ֧����*/
	while (fread(&tmp2, sizeof(TEAM_NODE), 1, pFile) == 1)
	{
		/*������㣬��Ŵ������ļ��ж������Ŷӻ�����Ϣ*/
		pTeamNode = (TEAM_NODE *)malloc(sizeof(TEAM_NODE));
		*pTeamNode = tmp2;
		pTeamNode->Shead = NULL;

		/*�������ϲ��Ҹ��Ŷ���סѧԺ��Ӧ���������*/
		pSchNode = hd;
		while (pSchNode != NULL
			&& strcmp(pSchNode->name, pTeamNode->school) != 0)
		{
			pSchNode = pSchNode->next;
		}
		if (pSchNode != NULL) /*����ҵ����򽫽���Ժ���ȳ���ʽ�����Ŷ���Ϣ֧��*/
		{
			pTeamNode->next = pSchNode->Thead;
			pSchNode->Thead = pTeamNode;
		}
		else  /*���δ�ҵ������ͷ������������ڴ�ռ�*/
		{
			free(pTeamNode);
		}
	}
	fclose(pFile);

	if ((pFile = fopen(gp_subject_info_filename, "rb")) == NULL)
	{
		printf("ס�޿�����Ϣ�����ļ���ʧ��!\n");
		return re;
	}
	printf("ס�޿�����Ϣ�����ļ��򿪳ɹ�!\n");
	re += 16;

	/*�������ļ��ж�ȡ�Ŷӿ�����Ϣ���ݣ������Ŷӻ�����Ϣ֧����Ӧ���Ŀ���֧����*/
	while (fread(&tmp3, sizeof(SUBJECT_NODE), 1, pFile) == 1)
	{
		/*������㣬��Ŵ������ļ��ж������Ŷӿ�����Ϣ*/
		pSubjectNode = (SUBJECT_NODE *)malloc(sizeof(SUBJECT_NODE));
		*pSubjectNode = tmp3;

		/*�����Ŷ���Ϣ֧���϶�Ӧ�Ŷ���Ϣ���*/
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
		if (find)  /*����ҵ����򽫽���Ժ���ȳ���ʽ�����Ŷӿ�����Ϣ֧����*/
		{
			pSubjectNode->next = pTeamNode->Shead;
			pTeamNode->Shead = pSubjectNode;
		}
		else /*���δ�ҵ������ͷ������������ڴ�ռ�*/
		{
			free(pSubjectNode);
		}
	}
	fclose(pFile);

	return re;
}

/**
* ��������: InitInterface
* ��������: ��ʼ������.
* �������: ��
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
*/
void InitInterface()
{
	WORD att = FOREGROUND_INTENSITY
| FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;/*��ɫǰ������ɫ����*/

	SetConsoleTextAttribute(gh_std_out, att);  /*���ÿ���̨��Ļ�������ַ�����*/

	ClearScreen();  /* ����*/

					/*��������������Ϣ��ջ������ʼ�������Ļ���ڵ�����һ�㵯������*/
	gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*��Ļ�ַ�����*/
	gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
	gp_top_layer->LayerNo = 0;      /*�������ڵĲ��Ϊ0*/
	gp_top_layer->rcArea.Left = 0;  /*�������ڵ�����Ϊ������Ļ����*/
	gp_top_layer->rcArea.Top = 0;
	gp_top_layer->rcArea.Right = SCR_COL - 1;
	gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
	gp_top_layer->pContent = NULL;
	gp_top_layer->pScrAtt = gp_scr_att;
	gp_top_layer->next = NULL;

	ShowMenu();     /*��ʾ�˵���*/
	ShowState();    /*��ʾ״̬��*/

	return;
}

/**
* ��������: ClearScreen
* ��������: �����Ļ��Ϣ.
* �������: ��
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
*/
void ClearScreen(void)
{
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	COORD home = { 0, 0 };
	unsigned long size;

	GetConsoleScreenBufferInfo(gh_std_out, &bInfo);/*ȡ��Ļ��������Ϣ*/
	size = bInfo.dwSize.X * bInfo.dwSize.Y; /*������Ļ�������ַ���Ԫ��*/

											/*����Ļ���������е�Ԫ���ַ���������Ϊ��ǰ��Ļ�������ַ�����*/
	FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

	/*����Ļ���������е�Ԫ���Ϊ�ո��ַ�*/
	FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

	return;
}

/**
* ��������: ShowMenu
* ��������: ����Ļ����ʾ���˵�, ����������, �����˵���һ������ѡ�б��.
* �������: ��
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
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
	for (i = 0; i < 5; i++) /*�ڴ��ڵ�һ�е�һ�д�������˵���*/
	{
		printf("  %s  ", ga_main_menu[i]);
	}

	GetConsoleCursorInfo(gh_std_out, &lpCur);
	lpCur.bVisible = FALSE;
	SetConsoleCursorInfo(gh_std_out, &lpCur);  /*���ع��*/

											   /*���붯̬�洢����Ϊ��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
	gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
	SMALL_RECT rcMenu = { 0, 0, size.X - 1, 0 };

	/*�����ڵ�һ�е����ݶ��뵽��Ų˵�����Ļ���ַ���Ϣ�Ļ�������*/
	ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

	/*����һ����Ӣ����ĸ��Ϊ��ɫ�������ַ���Ԫ��Ϊ�׵׺���*/
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

	/*�޸ĺ�Ĳ˵����ַ���Ϣ��д�����ڵĵ�һ��*/
	WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
	COORD endPos = { 0, 1 };
	SetConsoleCursorPosition(gh_std_out, endPos);  /*�����λ�������ڵ�2�е�1��*/

												   /*���˵�����Ϊ�������������Ϊ�˵���ţ���������Ϊ0(��ť��)*/
	i = 0;
	do
	{
		PosB = PosA + strlen(ga_main_menu[i]);  /*��λ��i+1�Ų˵������ֹλ��*/
		for (j = PosA; j<PosB; j++)
		{
			gp_scr_att[j] |= (i + 1) << 2; /*���ò˵��������ַ���Ԫ������ֵ*/
		}
		PosA = PosB + 4;
		i++;
	} while (i<5);

	TagMainMenu(gi_sel_menu);  /*��ѡ�����˵���������ǣ�gi_sel_menu��ֵΪ1*/

	return;
}

/**
* ��������: ShowState
* ��������: ��ʾ״̬��.
* �������: ��
* �������: ��
* �� �� ֵ: ��
*
* ����˵��: ״̬���ַ�����Ϊ�׵׺���, ��ʼ״̬��״̬��Ϣ.
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
* ��������: TagMainMenu
* ��������: ��ָ�����˵�������ѡ�б�־.
* �������: num ѡ�е����˵����
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
*/
void TagMainMenu(int num)
{
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	COORD size;
	COORD pos = { 0, 0 };
	int PosA = 2, PosB;
	char ch;
	int i;

	if (num == 0) /*numΪ0ʱ������ȥ�����˵���ѡ�б��*/
	{
		PosA = 0;
		PosB = 0;
	}
	else  /*���򣬶�λѡ�����˵������ֹλ��: PosAΪ��ʼλ��, PosBΪ��ֹλ��*/
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

	/*ȥ��ѡ�в˵���ǰ��Ĳ˵���ѡ�б��*/
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

	/*��ѡ�в˵���������ǣ��ڵװ���*/
	for (i = PosA; i<PosB; i++)
	{
		(gp_buff_menubar_info + i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
			| FOREGROUND_RED;
	}

	/*ȥ��ѡ�в˵������Ĳ˵���ѡ�б��*/
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

	/*�����ñ�ǵĲ˵�����Ϣд�����ڵ�һ��*/
	SMALL_RECT rcMenu = { 0, 0, size.X - 1, 0 };
	WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

	return;
}

/**
* ��������: CloseSys
* ��������: �ر�ϵͳ.
* �������: hd ����ͷָ��
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
*/
void CloseSys(SCHOOL_NODE *hd)
{
	SCHOOL_NODE *pSchNode1 = hd, *pSchNode2;
	TEAM_NODE *pTeamNode1, *pTeamNode2;
	SUBJECT_NODE *pSubjectNode1, *pSubjectNode2;

	while (pSchNode1 != NULL) /*�ͷ�ʮ�ֽ�������Ķ�̬�洢��*/
	{
		pSchNode2 = pSchNode1->next;
		pTeamNode1 = pSchNode1->Thead;
		while (pTeamNode1 != NULL) /*�ͷ��Ŷӻ�����Ϣ֧���Ķ�̬�洢��*/
		{
			pTeamNode2 = pTeamNode1->next;
			pSubjectNode1 = pTeamNode1->Shead;
			while (pSubjectNode1 != NULL) /*�ͷſ�����Ϣ֧���Ķ�̬�洢��*/
			{
				pSubjectNode2 = pSubjectNode1->next;
				free(pSubjectNode1);
				pSubjectNode1 = pSubjectNode2;
			}
			free(pTeamNode1);
			pTeamNode1 = pTeamNode2;
		}
		free(pSchNode1);  /*�ͷ��������Ķ�̬�洢��*/
		pSchNode1 = pSchNode2;
	}

	ClearScreen();        /*����*/

						  /*�ͷŴ�Ų˵�����״̬�����Ա������Ŷ����������Ϣ��̬�洢��*/
	free(gp_buff_menubar_info);
	free(gp_buff_stateBar_info);
	free(gp_sub_code);
	//free(gp_type_code);

	/*�رձ�׼���������豸���*/
	CloseHandle(gh_std_out);
	CloseHandle(gh_std_in);

	/*�����ڱ�������Ϊ���н���*/
	SetConsoleTitle("���н���");

	return;
}

/**
* ��������: RunSys
* ��������: ����ϵͳ, ��ϵͳ�������������û���ѡ��Ĺ���ģ��.
* �������: ��
* �������: phead ����ͷָ��ĵ�ַ
* �� �� ֵ: ��
*
* ����˵��:
*/
void RunSys(SCHOOL_NODE **phead)
{
	INPUT_RECORD inRec;
	DWORD res;
	COORD pos = { 0, 0 };
	BOOL bRet = TRUE;
	int i, loc, num;
	int cNo, cAtt;      /*cNo:�ַ���Ԫ���, cAtt:�ַ���Ԫ����*/
	char vkc, asc;      /*vkc:���������, asc:�ַ���ASCII��ֵ*/

	while (bRet)
	{
		/*�ӿ���̨���뻺�����ж�һ����¼*/
		ReadConsoleInput(gh_std_in, &inRec, 1, &res);

		if (inRec.EventType == MOUSE_EVENT) /*�����¼������¼�����*/
		{
			pos = inRec.Event.MouseEvent.dwMousePosition;  /*��ȡ�������λ��*/
			cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*ȡ��λ�õĲ��*/
			cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*ȡ���ַ���Ԫ����*/
			if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
			{
				/* cAtt > 0 ������λ�ô�������(���˵����ַ���Ԫ)
				* cAtt != gi_sel_menu ������λ�õ����˵���δ��ѡ��
				* gp_top_layer->LayerNo > 0 ������ǰ���Ӳ˵�����
				*/
				if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
				{
					PopOff();            /*�رյ������Ӳ˵�*/
					gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
					PopMenu(cAtt);       /*��������������˵����Ӧ���Ӳ˵�*/
				}
			}
			else if (cAtt > 0) /*�������λ��Ϊ�����Ӳ˵��Ĳ˵����ַ���Ԫ*/
			{
				TagSubMenu(cAtt); /*�ڸ��Ӳ˵�������ѡ�б��*/
			}

			if (inRec.Event.MouseEvent.dwButtonState
				== FROM_LEFT_1ST_BUTTON_PRESSED) /*������������ߵ�һ��*/
			{
				if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
				{
					if (cAtt > 0) /*�����λ�ô�������(���˵����ַ���Ԫ)*/
					{
						PopMenu(cAtt);   /*��������������˵����Ӧ���Ӳ˵�*/
					}
					/*�����λ�ò��������˵����ַ���Ԫ�������Ӳ˵�����*/
					else if (gp_top_layer->LayerNo > 0)
					{
						PopOff();            /*�رյ������Ӳ˵�*/
						gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
					}
				}
				else /*��Ų�Ϊ0��������λ�ñ������Ӳ˵�����*/
				{
					if (cAtt > 0) /*�����λ�ô�������(�Ӳ˵����ַ���Ԫ)*/
					{
						PopOff(); /*�رյ������Ӳ˵�*/
						gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/

											 /*ִ�ж�Ӧ���ܺ���:gi_sel_menu���˵����,cAtt�Ӳ˵����*/
						bRet = ExeFunction(gi_sel_menu, cAtt);
					}
				}
			}
			else if (inRec.Event.MouseEvent.dwButtonState
				== RIGHTMOST_BUTTON_PRESSED) /*�����������Ҽ�*/
			{
				if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
				{
					PopOff();            /*�رյ������Ӳ˵�*/
					gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
				}
			}
		}
		else if (inRec.EventType == KEY_EVENT  /*�����¼�ɰ�������*/
			&& inRec.Event.KeyEvent.bKeyDown) /*�Ҽ�������*/
		{
			vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*��ȡ�������������*/
			asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*��ȡ������ASC��*/

														/*ϵͳ��ݼ��Ĵ���*/
			if (vkc == 112) /*�������F1��*/
			{
				if (gp_top_layer->LayerNo != 0) /*�����ǰ���Ӳ˵�����*/
				{
					PopOff();            /*�رյ������Ӳ˵�*/
					gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
				}
				bRet = ExeFunction(5, 1);  /*���а������⹦�ܺ���*/
			}
			else if (inRec.Event.KeyEvent.dwControlKeyState
				& (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
			{ /*������������Alt��*/
				switch (vkc)  /*�ж���ϼ�Alt+��ĸ*/
				{
				case 88:  /*Alt+X �˳�*/
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
			else if (asc == 0) /*�������Ƽ��Ĵ���*/
			{
				if (gp_top_layer->LayerNo == 0) /*���δ�����Ӳ˵�*/
				{
					switch (vkc) /*�������(���ҡ���)������Ӧ�������Ƽ�*/
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
				else  /*�ѵ����Ӳ˵�ʱ*/
				{
					for (loc = 0, i = 1; i<gi_sel_menu; i++)
					{
						loc += ga_sub_menu_count[i - 1];
					}  /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
					switch (vkc) /*�����(���ҡ��ϡ���)�Ĵ���*/
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
			else if ((asc - vkc == 0) || (asc - vkc == 32)) {  /*������ͨ��*/
				if (gp_top_layer->LayerNo == 0)  /*���δ�����Ӳ˵�*/
				{
					switch (vkc)
					{
					case 70: /*f��F*/
						PopMenu(1);
						break;
					case 77: /*m��M*/
						PopMenu(2);
						break;
					case 81: /*q��Q*/
						PopMenu(3);
						break;
					case 83: /*s��S*/
						PopMenu(4);
						break;
					case 72: /*h��H*/
						PopMenu(5);
						break;
					case 13: /*�س�*/
						PopMenu(gi_sel_menu);
						TagSubMenu(1);
						break;
					}
				}
				else /*�ѵ����Ӳ˵�ʱ�ļ������봦��*/
				{
					if (vkc == 27) /*�������ESC��*/
					{
						PopOff();
						gi_sel_sub_menu = 0;
					}
					else if (vkc == 13) /*������»س���*/
					{
						num = gi_sel_sub_menu;
						PopOff();
						gi_sel_sub_menu = 0;
						bRet = ExeFunction(gi_sel_menu, num);
					}
					else /*������ͨ���Ĵ���*/
					{
						/*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
						for (loc = 0, i = 1; i<gi_sel_menu; i++)
						{
							loc += ga_sub_menu_count[i - 1];
						}

						/*�����뵱ǰ�Ӳ˵���ÿһ��Ĵ����ַ����бȽ�*/
						for (i = loc; i<loc + ga_sub_menu_count[gi_sel_menu - 1]; i++)
						{
							if (strlen(ga_sub_menu[i])>0 && vkc == ga_sub_menu[i][1])
							{ /*���ƥ��ɹ�*/
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
* ��������: PopMenu
* ��������: ����ָ�����˵����Ӧ���Ӳ˵�.
* �������: num ָ�������˵����
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
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

	if (num != gi_sel_menu)       /*���ָ�����˵�������ѡ�в˵�*/
	{
		if (gp_top_layer->LayerNo != 0) /*�����ǰ�����Ӳ˵�����*/
		{
			PopOff();
			gi_sel_sub_menu = 0;
		}
	}
	else if (gp_top_layer->LayerNo != 0) /*���ѵ������Ӳ˵����򷵻�*/
	{
		return;
	}

	gi_sel_menu = num;    /*��ѡ�����˵�����Ϊָ�������˵���*/
	TagMainMenu(gi_sel_menu); /*��ѡ�е����˵����������*/
	LocSubMenu(gi_sel_menu, &rcPop); /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

									 /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
	for (i = 1; i<gi_sel_menu; i++)
	{
		loc += ga_sub_menu_count[i - 1];
	}
	/*�������Ӳ˵������������ǩ���ṹ����*/
	labels.ppLabel = ga_sub_menu + loc;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
	labels.num = ga_sub_menu_count[gi_sel_menu - 1]; /*��ǩ���б�ǩ�ַ����ĸ���*/
	COORD aLoc[100];/*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
	for (i = 0; i<labels.num; i++) /*ȷ����ǩ�ַ��������λ�ã����������������*/
	{
		aLoc[i].X = rcPop.Left + 2;
		aLoc[i].Y = rcPop.Top + i + 1;
	}
	labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
						/*����������Ϣ*/
	areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ��������Ӳ˵�������*/
	SMALL_RECT aArea[100];// [areas.num];                    /*������������������λ��*/
	char aSort[100];// [areas.num];                      /*��������������������Ӧ���*/
	char aTag[100];// [areas.num];                         /*����������ÿ�������ı��*/
	for (i = 0; i<areas.num; i++)
	{
		aArea[i].Left = rcPop.Left + 2;  /*������λ*/
		aArea[i].Top = rcPop.Top + i + 1;
		aArea[i].Right = rcPop.Right - 2;
		aArea[i].Bottom = aArea[i].Top;
		aSort[i] = 0;       /*�������Ϊ0(��ť��)*/
		aTag[i] = i + 1;           /*������˳����*/
	}
	areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
	areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
	areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

	att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
	PopUp(&rcPop, att, &labels, &areas);
	DrawBox(&rcPop);  /*���������ڻ��߿�*/
	pos.X = rcPop.Left + 2;
	for (pos.Y = rcPop.Top + 1; pos.Y<rcPop.Bottom; pos.Y++)
	{ /*��ѭ�������ڿմ��Ӳ���λ�û����γɷָ�����ȡ���˲˵������������*/
		pCh = ga_sub_menu[loc + pos.Y - rcPop.Top - 1];
		if (strlen(pCh) == 0) /*����Ϊ0������Ϊ�մ�*/
		{   /*���Ȼ�����*/
			FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right - rcPop.Left - 3, pos, &ul);
			for (j = rcPop.Left + 2; j<rcPop.Right - 1; j++)
			{   /*ȡ���������ַ���Ԫ����������*/
				gp_scr_att[pos.Y*SCR_COL + j] &= 3; /*��λ��Ľ�������˵���λ*/
			}
		}

	}
	/*���Ӳ˵���Ĺ��ܼ���Ϊ�׵׺���*/
	pos.X = rcPop.Left + 3;
	att = FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
	for (pos.Y = rcPop.Top + 1; pos.Y<rcPop.Bottom; pos.Y++)
	{
		if (strlen(ga_sub_menu[loc + pos.Y - rcPop.Top - 1]) == 0)
		{
			continue;  /*�����մ�*/
		}
		FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
	}
	return;
}

/**
* ��������: PopUp
* ��������: ��ָ�������������������Ϣ, ͬʱ��������, ����������λ����Ϣ��ջ.
* �������: pRc ��������λ�����ݴ�ŵĵ�ַ
*           att �������������ַ�����
*           pLabel ���������б�ǩ����Ϣ��ŵĵ�ַ
pHotArea ����������������Ϣ��ŵĵ�ַ
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
*/
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
	LAYER_NODE *nextLayer;
	COORD size;
	COORD pos = { 0, 0 };
	char *pCh;
	int i, j, row;

	/*������������λ���ַ���Ԫ��Ϣ��ջ*/
	size.X = pRc->Right - pRc->Left + 1;    /*�������ڵĿ��*/
	size.Y = pRc->Bottom - pRc->Top + 1;    /*�������ڵĸ߶�*/
											/*�����ŵ������������Ϣ�Ķ�̬�洢��*/
	nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
	nextLayer->next = gp_top_layer;
	nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
	nextLayer->rcArea = *pRc;
	nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y * sizeof(CHAR_INFO));
	nextLayer->pScrAtt = (char *)malloc(size.X*size.Y * sizeof(char));
	pCh = nextLayer->pScrAtt;
	/*���������ڸ���������ַ���Ϣ���棬�����ڹرյ�������ʱ�ָ�ԭ��*/
	ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
	for (i = pRc->Top; i <= pRc->Bottom; i++)
	{   /*�˶���ѭ�����������ַ���Ԫ��ԭ������ֵ���붯̬�洢���������Ժ�ָ�*/
		for (j = pRc->Left; j <= pRc->Right; j++)
		{
			*pCh = gp_scr_att[i*SCR_COL + j];
			pCh++;
		}
	}
	gp_top_layer = nextLayer;  /*��ɵ������������Ϣ��ջ����*/
							   /*���õ������������ַ���������*/
	pos.X = pRc->Left;
	pos.Y = pRc->Top;
	for (i = pRc->Top; i <= pRc->Bottom; i++)
	{
		FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
		pos.Y++;
	}
	/*����ǩ���еı�ǩ�ַ������趨��λ�����*/
	for (i = 0; i<pLabel->num; i++)
	{
		pCh = pLabel->ppLabel[i];
		if (strlen(pCh) != 0)
		{
			WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
				pLabel->pLoc[i], &ul);
		}
	}
	/*���õ������������ַ���Ԫ��������*/
	for (i = pRc->Top; i <= pRc->Bottom; i++)
	{   /*�˶���ѭ�������ַ���Ԫ�Ĳ��*/
		for (j = pRc->Left; j <= pRc->Right; j++)
		{
			gp_scr_att[i*SCR_COL + j] = gp_top_layer->LayerNo;
		}
	}

	for (i = 0; i<pHotArea->num; i++)
	{   /*�˶���ѭ�����������������ַ���Ԫ���������ͺ��������*/
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

	if ((gp_top_layer->next == NULL) || (gp_top_layer->pContent == NULL))
	{   /*ջ�״�ŵ���������Ļ��Ϣ�����ùر�*/
		return;
	}
	nextLayer = gp_top_layer->next;
	/*�ָ�������������ԭ���*/
	size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
	size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
	WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
	/*�ָ��ַ���Ԫԭ����*/
	pCh = gp_top_layer->pScrAtt;
	for (i = gp_top_layer->rcArea.Top; i <= gp_top_layer->rcArea.Bottom; i++)
	{
		for (j = gp_top_layer->rcArea.Left; j <= gp_top_layer->rcArea.Right; j++)
		{
			gp_scr_att[i*SCR_COL + j] = *pCh;
			pCh++;
		}
	}
	free(gp_top_layer->pContent);    /*�ͷŶ�̬�洢��*/
	free(gp_top_layer->pScrAtt);
	free(gp_top_layer);
	gp_top_layer = nextLayer;
	gi_sel_sub_menu = 0;
	return;
}

/**
* ��������: DrawBox
* ��������: ��ָ�����򻭱߿�.
* �������: pRc �������λ����Ϣ�ĵ�ַ
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
*/
void DrawBox(SMALL_RECT *pRc)
{
	char chBox[] = { '+','-','|' };  /*�����õ��ַ�*/
	COORD pos = { pRc->Left, pRc->Top };  /*��λ����������Ͻ�*/

	WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
	for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
	{   /*��ѭ�����ϱ߿����*/
		WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
	}
	pos.X = pRc->Right;
	WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
	for (pos.Y = pRc->Top + 1; pos.Y < pRc->Bottom; pos.Y++)
	{   /*��ѭ�����߿�����ߺ��ұ���*/
		pos.X = pRc->Left;
		WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
		pos.X = pRc->Right;
		WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
	}
	pos.X = pRc->Left;
	pos.Y = pRc->Bottom;
	WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
	for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
	{   /*���±߿����*/
		WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
	}
	pos.X = pRc->Right;
	WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
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
		FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
		pos.X += 1;
		att |= FOREGROUND_RED;/*�׵׺���*/
		FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
	}
	/*���ƶ��Ӳ˵�������ѡ�б��*/
	pos.X = rcPop.Left + 2;
	pos.Y = rcPop.Top + num;
	att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
	FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
	gi_sel_sub_menu = num;  /*�޸�ѡ���Ӳ˵����*/
	return;
}

/**
* ��������: LocSubMenu
* ��������: ���㵯���Ӳ˵��������ϽǺ����½ǵ�λ��.
* �������: num ѡ�е����˵����
* �������: rc �������λ����Ϣ�ĵ�ַ
* �� �� ֵ: ��
*
* ����˵��:
*/
void LocSubMenu(int num, SMALL_RECT *rc)
{
	int i, len, loc = 0;

	rc->Top = 1; /*������ϱ߶��ڵ�2�У��к�Ϊ1*/
	rc->Left = 1;
	for (i = 1; i<num; i++)
	{   /*����������߽�λ��, ͬʱ�����һ���Ӳ˵������Ӳ˵��ַ��������е�λ��*/
		rc->Left += strlen(ga_main_menu[i - 1]) + 4;
		loc += ga_sub_menu_count[i - 1];
	}
	rc->Right = strlen(ga_sub_menu[loc]);/*��ʱ��ŵ�һ���Ӳ˵����ַ�������*/
	for (i = 1; i<ga_sub_menu_count[num - 1]; i++)
	{   /*������Ӳ˵��ַ��������䳤�ȴ����rc->Right*/
		len = strlen(ga_sub_menu[loc + i]);
		if (rc->Right < len)
		{
			rc->Right = len;
		}
	}
	rc->Right += rc->Left + 3;  /*����������ұ߽�*/
	rc->Bottom = rc->Top + ga_sub_menu_count[num - 1] + 1;/*���������±ߵ��к�*/
	if (rc->Right >= SCR_COL)  /*�ұ߽�Խ��Ĵ���*/
	{
		len = rc->Right - SCR_COL + 1;
		rc->Left -= len;
		rc->Right = SCR_COL - 1;
	}
	return;
}

/**
* ��������: DealInput
* ��������: �ڵ�������������������, �ȴ�����Ӧ�û�����.
* �������: pHotArea
*           piHot ����������ŵĴ�ŵ�ַ, ��ָ�򽹵�������ŵ�ָ��
* �������: piHot ����굥�������س���ո�ʱ���ص�ǰ�������
* �� �� ֵ:
*
* ����˵��:
*/
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
	INPUT_RECORD inRec;
	DWORD res;
	COORD pos = { 0, 0 };
	int num, arrow, iRet = 0;
	int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
	char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/

	SetHotPoint(pHotArea, *piHot);
	while (TRUE)
	{    /*ѭ��*/
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
				{  /*�����(���ϡ��ҡ���)�Ĵ���*/
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
			{  /*ESC��*/
				iRet = 27;
				break;
			}
			else if (vkc == 13 || vkc == 32)
			{  /*�س�����ո��ʾ���µ�ǰ��ť*/
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

	att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
	att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
	for (i = 0; i<pHotArea->num; i++)
	{  /*����ť��������Ϊ�׵׺���*/
		pos.X = pHotArea->pArea[i].Left;
		pos.Y = pHotArea->pArea[i].Top;
		width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
		if (pHotArea->pSort[i] == 0)
		{  /*�����ǰ�ť��*/
			FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
		}
	}

	pos.X = pHotArea->pArea[iHot - 1].Left;
	pos.Y = pHotArea->pArea[iHot - 1].Top;
	width = pHotArea->pArea[iHot - 1].Right - pHotArea->pArea[iHot - 1].Left + 1;
	if (pHotArea->pSort[iHot - 1] == 0)
	{  /*�����������ǰ�ť��*/
		FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
	}
	else if (pHotArea->pSort[iHot - 1] == 1)
	{  /*�������������ı�����*/
		SetConsoleCursorPosition(gh_std_out, pos);
		GetConsoleCursorInfo(gh_std_out, &lpCur);
		lpCur.bVisible = TRUE;
		SetConsoleCursorInfo(gh_std_out, &lpCur);
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
BOOL ExeFunction(int m, int s)
{
	BOOL bRet = TRUE;
	/*����ָ�����飬����������й��ܺ�������ڵ�ַ*/
	BOOL(*pFunction[19])(void);// [ga_sub_menu_count[0] + ga_sub_menu_count[1] + ga_sub_menu_count[2] + ga_sub_menu_count[3] + ga_sub_menu_count[4]])(void);
	int i, loc;

	/*�����ܺ�����ڵ�ַ�����빦�ܺ����������˵��ź��Ӳ˵��Ŷ�Ӧ�±������Ԫ��*/
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


	for (i = 1, loc = 0; i<m; i++)  /*�������˵��ź��Ӳ˵��ż����Ӧ�±�*/
	{
		loc += ga_sub_menu_count[i - 1];
	}
	loc += s - 1;

	if (pFunction[loc] != NULL)
	{
		bRet = (*pFunction[loc])();  /*�ú���ָ�������ָ��Ĺ��ܺ���*/
	}

	return bRet;
}
/**
* ��������: SaveData
* ��������: ��������
* �������: ��
* �������: ��
* �� �� ֵ: 1
*
* ����˵��:����SaveSysdata
*/
BOOL SaveData(void)
{
    system("cls");
	BOOL bRet = TRUE;
	char *plabel_name[] = { "���˵���ļ�",
		"�Ӳ˵�����ݱ���",
		"ȷ��"
	};

	ShowModule(plabel_name, 3);
	SaveSysData(gp_head);

	printf("����ɹ�!\n");

	return bRet;
}



/**
* ��������: ExitSys
* ��������: �Ƴ�ϵͳ
* �������: ��
* �������: ״̬����
* �� �� ֵ: 1
*
* ����˵��:DealInput	PopOff	PopUp
*/
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
* ��������: MainTainSchinfo
* ��������: ά��ѧԺ��Ϣ
* �������: ��
* �������: ״̬����
* �� �� ֵ: TRUE
*
* ����˵��:DelSchoolNode  InsertSchoolNode	ModifySchool
*/
BOOL MaintainSchInfo(void)
{
	BOOL bRet = TRUE;
	int  model;
	char *plabel_name[] = { "���˵������ά��",
		"�Ӳ˵��ѧԺ��Ϣ",
		"ȷ��"
	};

	ShowModule(plabel_name, 3);
	system("cls");
	printf("����������еĲ���:\n");
	printf("(1)����ѧԺ��Ϣ		(2)ɾ��ѧԺ��Ϣ		��3���޸�ѧԺ��Ϣ		(4)������һ��\n");
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
		printf("�������\n");
		break;
	}

	return bRet;
}

/**
* ��������: MainTainTeaminfo
* ��������: ά���Ŷ���Ϣ
* �������: ��
* �������: ״̬����
* �� �� ֵ: TRUE
*
* ����˵��:DelTeamNode  InsertTeamNode	ModifyTeaminfo
*/
BOOL MaintainTeamInfo(void)
{
	BOOL bRet = TRUE;
	int  model;
	char *plabel_name[] = { "���˵������ά��",
		"�Ӳ˵��ѧԺ��Ϣ",
		"ȷ��"
	};

	ShowModule(plabel_name, 3);
	system("cls");
	printf("����������еĲ���:\n");
	printf("(1)�����Ŷ���Ϣ		(2)ɾ���Ŷ���Ϣ		��3���޸��Ŷ���Ϣ		(4)������һ��\n");
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
		printf("�������\n");
		break;
	}

	ShowModule(plabel_name, 3);

	return bRet;
}

/**
* ��������: MainTainSubinfo
* ��������: ά����Ŀ��Ϣ
* �������: ��
* �������: ״̬����
* �� �� ֵ: TRUE
*
* ����˵��:DelSubNode  InsertSubNode	ModifySubNode
*/
BOOL MaintainSubjectInfo(void)
{
	BOOL bRet = TRUE;
	int  model;
	char *plabel_name[] = { "���˵������ά��",
		"�Ӳ˵��ѧԺ��Ϣ",
		"ȷ��"
	};

	ShowModule(plabel_name, 3);
	system("cls");
	printf("����������еĲ���:\n");
	printf("(1)������Ŀ��Ϣ		(2)ɾ����Ŀ��Ϣ		��3���޸���Ŀ��Ϣ		(4)������һ��\n");
	printf("��ѡ�����ģʽ:");
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
		printf("�������\n");
		break;
	}



	return bRet;
}






/**
* ��������: InsertProNode
* ��������: ��ʮ�������в���һ��������Ϣ���.
* �������: void
* �������: ��
* �� �� ֵ: BOOL����, TRUE��ʾ����ɹ�, FALSE��ʾ����ʧ��
*
* ����˵��:
*/
BOOL InsertProInfo(void)
{
	system("cls");
	SUBJECT_NODE* new_subject = (SUBJECT_NODE*)malloc(sizeof(SUBJECT_NODE));
	TEAM_NODE *temp;
	printf("��������Ŀ�����Ŷ�:");
	scanf("%s", new_subject->team);
	getchar();
	if ((temp = MatchTeamName(gp_head, new_subject->team)) != NULL)
	{
		new_subject->next = temp->Shead;
		temp->Shead = new_subject;
		FullFillProInfo(new_subject);
		printf("¼��ɹ�\n");
		return TRUE;
	}
	else
	{
		printf("\nδ�ҵ����Ŷ�.������.\n");
		return FALSE;
	}

}

/**
* ��������: DelSubjectNode
* ��������: ��ʮ��������ɾ��ָ���Ŀ�����Ϣ���.
* �������: hd ����ͷָ��
*           Team_id �����Ŷ�ѧ��
*           date ��������
* �������: ��
* �� �� ֵ: BOOL����, TRUE��ʾɾ���ɹ�, FALSE��ʾɾ��ʧ��
*
* ����˵��: ����ѧ�źͿ������ڿ���ȷ��Ψһ�Ŀ�����Ϣ
*/
BOOL DelSubjectNode(void)
{
	system("cls");

	char ID[15];
	printf("��������Ŀ��ţ�");
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
			printf("ɾ���ɹ���\n");
			free(pS2);
			return TRUE;
		}
	}
	return TRUE;
}

/**
* ��������: SeekProID
* ��������: ��ʮ��������Ѱ��һ��������Ϣ���.
* �������: ����ͷָ��
* �������: ��
* �� �� ֵ: ���������Ŷ�ָ��
*
* ����˵��:
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
* ��������: SaveSysData
* ��������: ����ϵͳ�����������������.
* �������: hd ����ͷ���ָ��
* �������:
* �� �� ֵ: BOOL����, ����ΪTRUE
*
* ����˵��:
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

	att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
* ��������: QurrySchoolP
* ��������: ��ѯѧԺ������.
* �������: woid
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
*
* ����˵��:SearchPIC
*/
BOOL QurrySchoolP(void) {
	system("cls");
	char person[12];
	SCHOOL_NODE* temp;
	printf("\n\n�����븺��������:");
	scanf("%s", person);
	if ((temp = SearchPIC(gp_head, person)) != NULL)
	{
		printf("Ժϵ����            ������      ��ϵ�绰       \n");
		printf("%-20s%-12s%-15s\n", temp->name, temp->person_in_charge, temp->phone);
	}
	else
		printf("\nNOT FOUND!\n");

	return TRUE;
}
/**
* ��������: SearchSchoolPIC
* ��������: ���������ѯѧԺ������.
* �������: ����ͷָ��	����������
* �������: ����Ժϵָ��
* �� �� ֵ: TRUE�����ѯ����
*
* ����˵��:SearchPIC
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
* ��������: QurryTeamName
* ��������: ��ѯ�Ŷ�����
* �������: void
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
*
* ����˵��:SearchPIC
*/

BOOL QurryTeamName(void)
{
    system("cls");
	char s1[] = "�Ŷ�";
	char s2[] = "������";
	char s3[] = "����ѧԺ";
	char s4[] = "��ʦ����";
	char s5[] = "ѧ������";

	char name[20];
	printf("�������Ŷ�����:");
	scanf("%s", name);
	printf("%-30s%-12s%s	%s		%s\n", s1, s2, s4, s5, s3);
	if (!SeekTeamName(gp_head, name))
		printf("\nNOT FOUND!\n");



	return TRUE;
}/**
* ��������: QurryProfAcc
* ��������: ��ѯѧԺ��ʦ������������ڸ�ֵ�������Ŷ�
* �������: void
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
*
* ����˵��:
*/

BOOL QurryProfAcc(void)
{
	char s1[] = "ѧԺ";
	char s2[] = "������";
	char s3[] = "����ѧԺ";
	char s4[] = "��ʦ����";
	char s5[] = "ѧ������";
	system("cls");
	int num, count = 0;
	printf("��ʦ����:");
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
		printf("\nû���ҵ�����Ҫ����Ŷ���Ϣ!\n");
	return TRUE;
}
/**
* ��������: QurrySubID
* ��������: ��ѯ��Ŀ���.
* �������: void
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
*
* ����˵��:
*/
BOOL QurrySubID(void)
{
	system("cls");
	char ID[15];
	SUBJECT_NODE *temp;
	printf("��������Ŀ���:");
	scanf("%s", ID);
	char s1[] = "��Ŀ���";
	char s2[] = "��Ŀ���";
	char s3[] = "�ʽ�";
	char s4[] = "������;";
	char s5[] = "�����Ŷ�";
	char s6[] = "��ʼʱ��";
	printf("%-15s%s	%-8s%-8s %-12s%-30s\n",s1,s2,s6,s3,s4,s5);
	if ((temp = SeekProID(gp_head, ID)) != NULL)
		printf("%-15s%c	%-8s  %.2f  %-12s%-30s\n", temp->ID, temp->type, temp->begin_time, temp->fund, temp->person_in_charge, temp->team);
	else printf("\nNot Found!\n");
	return TRUE;
}
/**
* ��������: QurryTeamBelonging
* ��������: ��ѯ�Ŷ�������Ŀ��Ϣ
* �������: void
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
*
*/
BOOL QurryTeamBelong(void)
{
    system("cls");
	printf("�����Ŷ�����");
	char team[30];
	scanf("%s", team);
	getchar();
	SUBJECT_NODE *pSubject;
	TEAM_NODE* pTeam = MatchTeamName(gp_head, team);
	if (pTeam != NULL)
	{
		pSubject = pTeam->Shead;
		printf("��Ŀ���       ��Ŀ���     ��ʼʱ��		 �ʽ�		������      �����Ŷ�\n");
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
	printf("\n������Ժϵ����:");
	scanf("%s", name);
	printf("Ժϵ����            ������      ��ϵ�绰       \n");
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
* ��������: SeekProId
* ��������: �����б��ѯ��Ŀ���
* �������: ����ͷָ�� ��Ŀ���
* �������: ������Ŀָ��
* �� �� ֵ:
*
* ����˵��:
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
* ��������: MatchTeamName
* ��������: ƥ���Ŷ���Ϣ������.
* �������: ����ͷָ��
* �������: �����Ŷ�ָ��
* �� �� ֵ:

* ����˵��:
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
* ��������: FullfillProInfo
* ��������: ��д��Ŀ��Ϣ
* �������: ������Ŀָ��
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:SearchPIC
*/
void FullFillProInfo(SUBJECT_NODE *sub)
{
	printf("��������Ŀ���;");
	scanf("%s", sub->ID);
	getchar();
	printf("��������Ŀ���:");
	scanf("%c", &(sub->type));
	getchar();
	printf("��������ʼʱ��:");
	scanf("%s", sub->begin_time);
	getchar();
	printf("��������Ŀ����:");
	scanf("%f", &(sub->fund));
	getchar();
	printf("�����븺����:");
	scanf("%s", sub->person_in_charge);
	getchar();
}
/**
* ��������: DelProInfo
* ��������: ɾ��������Ϣ
* �������: BOOL
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
*
* ����˵��:SearchproID
*/
BOOL DelProInfo(void)
{
	system("cls");
	char ID[15];
	printf("��������Ŀ���:");
	scanf("%s", ID);

	getchar();

	SUBJECT_NODE *p1, *p2, *tmp1;
	TEAM_NODE*tmp2 = MatchProID_1(gp_head, ID);

	if (tmp2 != NULL)
		tmp1 = MatchProID_2(gp_head, ID);
	else {
		printf("ɾ��ʧ�ܣ�\n");
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
			printf("ɾ���ɹ���\n");
			return TRUE;
		}
	}
	else printf("ɾ��ʧ�ܣ�\n");
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
	printf("��������Ŀ���:");
	scanf("%s", id);
	getchar();
	SUBJECT_NODE *temp = MatchProID_2(gp_head, id);
	if (temp != NULL)
	{
		FullFillProInfo(temp);
		printf("�޸ĳɹ�!\n");

		return TRUE;
	}

	else printf("\nδ�ҵ�����Ŀ,������!");
	return FALSE;

}


/*
**
* ��������: InsertTeamNode
* ��������: ����ȳ������Ŷӽڵ�
* �������: ��
* �������: ��
* �� �� ֵ: TRUE��������
*
*/
SUBJECT_NODE* MatchProID_2(SCHOOL_NODE*hd, char* id)
{
	SCHOOL_NODE* pSchool = hd;
	TEAM_NODE *pTeam;
	SUBJECT_NODE *pSubject;
	//�����������������Ϣ
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
* ��������: InsertTeamNode
* ��������: ����ȳ������Ŷӽڵ�
* �������: ��
* �������: ��
* �� �� ֵ: TRUE��������
*
*/
BOOL InsertTeamNode(void)
{
	system("cls");
	printf("����������Ժϵ����:");
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
		printf("\nδ�ҵ���Ժϵ,������.\n");
	return FALSE;
}

/*
**
* ��������: MatchSchoolName
* ��������: ���Ҷ�Ӧ���Ŷӽڵ㷵�ظö�����ѧԺ�ڵ�
* �������: ����ͷָ�� �Ŷ���
* �������: pTeam
* �� �� ֵ: �ҵ����ض�ӦѧԺ�ڵ㣬δ�ҵ�����NULL
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
* ��������: FullFillTeamInfo
* ��������: ��ѯѧԺ������.
* �������: ��Ӧ�Ŷ�ָ��
* �������: ��
* �� �� ֵ: ��
*
* ����˵��:
*/
void FullFillTeamInfo(TEAM_NODE* team)
{
	printf("�������Ŷ�����:");
	scanf("%s", team->name);
	getchar();
	printf("�����븺��������:");
	scanf("%s", team->person_in_charge);
	getchar();
	printf("�������ʦ����:");
	scanf("%d", &(team->tea_amount));
	getchar();
	printf("�������о���������");
	scanf("%d", &(team->ug_amount));
	getchar();
}
/**
* ��������: ɾ���Ŷ���Ϣ
* ��������: ɾ���Ŷӽڵ�.
* �������: void
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
*
* ����˵��:MacthTeamNode_step_1,2
*/
BOOL DelTeamNode(void)
{
	system("cls");
	char team[30];
	TEAM_NODE* pT1, *pT2, *tmp2;
	printf("�������Ŷ�����:");
	scanf("%s", team);
	getchar();
	SCHOOL_NODE *tmp1 = MatchTeamNode_Step_1(gp_head, team);
	if (tmp1 != NULL)
		tmp2 = MatchTeamNode_Step_2(tmp1, team);
	else
	{
		printf("ɾ��ʧ�ܣ�\n");
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
			printf("ɾ���ɹ�\n");
			return TRUE;
		}
	}
	else printf("\nδ�ҵ����Ŷ�\n");
	return FALSE;
}

/*
**
* ��������: MatchTeamNode_Step_1
* ��������: ���Ҷ�Ӧ���Ŷӽڵ㷵�ظ��Ŷ�����ѧԺ�ڵ�
* �������: ����ͷָ�� �Ŷ���
* �������: pTeam
* �� �� ֵ: �ҵ����ظ��Ŷ�����ѧԺ�ڵ㣬δ�ҵ�����NULL
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
* ��������: MatchTeamNode_Step_2
* ��������: ���Ҷ�Ӧ���Ŷӽڵ㷵�ظ��Ŷӽڵ�
* �������: ����ͷָ�� �Ŷ���
* �������: pTeam
* �� �� ֵ: �ҵ����ض�Ӧ�ڵ㣬δ�ҵ�����NULL
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
	printf("δ�ҵ����Ŷ�\n");
	return NULL;
}
/**
* ��������: ModifyTeamInfo
* ��������: ��ѯѧԺ������.
* �������: void
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
*
* ����˵��:FullFillTeamInfo
*/
BOOL ModifyTeamInfo(void)
{
	system("cls");
	char team[30];
	SCHOOL_NODE *pSchool = gp_head;
	TEAM_NODE *pTeam;
	printf("�������Ŷ�����:");
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
	printf("\nδ�ҵ��Ŷ�\n");
	return FALSE;
}
/**
* ��������: InsertSchoolNode
* ��������: ����ȳ�����ѧԺ�ڵ�
* �������: void
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
*
* ����˵��:FullFillSchInfo
*/
BOOL InsertSchoolNode(void)
{
	system("cls");
	SCHOOL_NODE * nSch = (SCHOOL_NODE*)malloc(sizeof(SCHOOL_NODE));
	nSch->Thead = NULL;
	FullFillSchInfo(nSch);
	nSch->next = gp_head;
	gp_head = nSch;
	printf("����ɹ���\n");
	return TRUE;

}

/**
* ��������: FullFillSchInfo
* ��������: ��дѧԺ��Ϣ
* �������: void
* �������: ��
* �� �� ֵ: ��
*
*/

void FullFillSchInfo(SCHOOL_NODE * new_sch)
{
	printf("\n������Ժϵ��:");
	scanf("%s", new_sch->name);
	getchar();
	printf("�����븺��������:");
	scanf("%s", new_sch->person_in_charge);
	getchar();
	printf("��������ϵ�绰:");
	scanf("%s", new_sch->phone);
	getchar();
}

/**
* ��������: DelSchoolNode
* ��������: ɾ��ѧԺ�ڵ�
* �������: BOOL
* �������: ״̬����
* �� �� ֵ: TRUE����ͳ������
*
*/
BOOL DelSchoolNode(void)
{
	char name[20];
	printf("������ѧԺ���ƣ�");
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
		printf("ɾ���ɹ�");
		return  TRUE;
	}
	printf("ɾ��ʧ��!\n");
	return FALSE;
}

/**
* ��������: SortSchoolInfo_2
* ��������: ��ѧԺʦ���ȶ�ѧԺ��Ϣ�����������
* �������: void
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
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
	char str1[] = "ѧԺ";
	char str2[] = "��ʦ����";
	char str3[] = "ѧ������";
	char str4[] = "��ʦ��ѧ��";
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
* ��������: SortSchoolInfo_2
* ��������: ��ѧԺ��Ŀ����ѧԺ��Ϣ�����������
* �������: void
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
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
	char s1[] = "ѧԺ";
	char s2[] = "973��Ŀ��";
	char s3[] = "863��Ŀ��";
	char s4[] = "���ʽ�";
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
	char s1[] = "�Ŷ���";
	char s2[] = "������Ȼ������Ŀ";
	char s3[] = "�Ŷ��ܻ���";
	printf("%-30s%-s%s\n", s1, s2, s3);
	for (i = 0, p = hd; i < 10 && p != NULL; i++, p = p->next)
		printf("%-30s%-8d		%-12.2f\n", p->name, p->NA_pro, p->t_total_fund);
}


/**
* ��������: SortTeamInfo_2
* ��������: ���Ŷ��ص���Ŀ�����Ŷ���Ϣ�����������
* �������: voud
* �������: ״̬����
* �� �� ֵ: TRUE������������
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
	char s1[] = "�Ŷ�����";
	char s2[] = "��Ŀ����";
	char s3[] = "��ʦ����";
	char s4[] = "��Ŀ�Ƚ�ʦ";
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
	char s1[] = "ѧԺ";
	char s2[] = "������";
	char s3[] = "��ϵ�绰";
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
* ��������: statistic_1
* ��������: ͳ��������ݰ�ѧԺʦ���ȶ�ѧԺ��Ϣ�����������
* �������: BOOL
* �������: ״̬����
* �� �� ֵ: TRUE����ͳ������
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
		//��ʼ���µ�ѧУ��Ϣ�ڵ�
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

			//��ʼ���µ��Ŷ���Ϣ�ڵ�
			strcpy(tdata_hd->name, pTeam->name);
			tdata_hd->t_tea_amount = pTeam->tea_amount;
			tdata_hd->t_pro_amount = 0;
			tdata_hd->t_total_fund = 0;
			tdata_hd->NA_pro = 0;
			//����ѧԺ��Ϣ
			sdata_hd->Stu_Amount += pTeam->ug_amount;
			sdata_hd->Sch_prof_total += pTeam->tea_amount;

			pPro = pTeam->Shead;
			while (pPro != NULL)
			{
				//�����������
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
* ��������: statistic_2
* ��������: ͳ��������ݰ�ѧԺ��Ŀ����ѧԺ��Ϣ�����������
* �������: BOOL
* �������: ״̬����
* �� �� ֵ: TRUE����ͳ������
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
		//��ʼ���µ�ѧУ��Ϣ�ڵ�
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

			//��ʼ���µ��Ŷ���Ϣ�ڵ�
			strcpy(tdata_hd->name, pTeam->name);
			tdata_hd->t_tea_amount = pTeam->tea_amount;
			tdata_hd->t_pro_amount = 0;
			tdata_hd->t_total_fund = 0;
			tdata_hd->NA_pro = 0;
			//����ѧԺ��Ϣ
			sdata_hd->Stu_Amount += pTeam->ug_amount;
			sdata_hd->Sch_prof_total += pTeam->tea_amount;

			pPro = pTeam->Shead;
			while (pPro != NULL)
			{
				//�����������
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
* ��������: statistic_3
* ��������: ͳ���������,���Ŷ��ص���Ŀ�����Ŷ���Ϣ�����������
* �������: BOOL
* �������: ״̬����
* �� �� ֵ: TRUE����ͳ������
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
		//��ʼ���µ�ѧУ��Ϣ�ڵ�
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

			//��ʼ���µ��Ŷ���Ϣ�ڵ�
			strcpy(tdata_hd->name, pTeam->name);
			tdata_hd->t_tea_amount = pTeam->tea_amount;
			tdata_hd->t_pro_amount = 0;
			tdata_hd->t_total_fund = 0;
			tdata_hd->NA_pro = 0;
			//����ѧԺ��Ϣ
			sdata_hd->Stu_Amount += pTeam->ug_amount;
			sdata_hd->Sch_prof_total += pTeam->tea_amount;

			pPro = pTeam->Shead;
			while (pPro != NULL)
			{
				//�����������
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
* ��������: statistic_1
* ��������: ͳ��������ݰ�ѧԺʦ���ȶ�ѧԺ��Ϣ�����������
* �������: BOOL
* �������: ״̬����
* �� �� ֵ: TRUE����ͳ������
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
		//��ʼ���µ�ѧУ��Ϣ�ڵ�
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

			//��ʼ���µ��Ŷ���Ϣ�ڵ�
			strcpy(tdata_hd->name, pTeam->name);
			tdata_hd->t_tea_amount = pTeam->tea_amount;
			tdata_hd->t_pro_amount = 0;
			tdata_hd->t_total_fund = 0;
			tdata_hd->NA_pro = 0;
			//����ѧԺ��Ϣ
			sdata_hd->Stu_Amount += pTeam->ug_amount;
			sdata_hd->Sch_prof_total += pTeam->tea_amount;

			pPro = pTeam->Shead;
			while (pPro != NULL)
			{
				//�����������
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
* ��������: ModifySchoolInfo
* ��������: �޸�ѧԺ��Ϣ
* �������: BOOL
* �������: ״̬����
* �� �� ֵ: TRUE�����ѯ����
*
*/
BOOL ModifySchoolInfo()
{
	char name[20];
	printf("������ѧԺ����");
	scanf("%s", name);
	getchar();
	SCHOOL_NODE* temp = MatchSchoolName(gp_head, name);
	if (temp != NULL)
	{
		FullFillSchInfo(temp);
		printf("�޸ĳɹ���");
		return TRUE;
	}
	else printf("δ�ҵ����Ŷӣ�\n");
	return FALSE;
}
