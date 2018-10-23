#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ctype.h>
#include <time.h>
#include<stdbool.h>

#define SCR_ROW 25             /*��Ļ����*/
#define SCR_COL 80             /*��Ļ����*/



//data struct

//Ժϵ
typedef struct school {

	char name[20];				//Ժϵ����
	char person_in_charge[12];	//������
	char phone[15];				//��ϵ����
	struct team* Thead;
	struct school* next;
}SCHOOL_NODE;

typedef struct sch_data {
	char name[20];
	int Sch_prof_total;
	int Stu_Amount;
	float Pro2Stu;
	int _973_num;
	int _863_num;
	int sch_pro_num;
	float total_fund;
	struct sch_data* next;
}SCH_DATA;

//�Ŷ�

typedef struct team {
	char name[20];				//�Ŷ�����
	char person_in_charge[12];	//������
	int tea_amount;				//��������
	int ug_amount;				//�о�������
	char school[20];			//����Ժϵ
	struct team *next;
	struct subject *Shead;
}TEAM_NODE;

typedef struct team_data {
	char name[30];
	int t_pro_amount;
	int t_tea_amount;
	float Pro2Prof;
	float t_total_fund;
	int NA_pro;
	struct team_data* next;
}TEAM_DATA;

//����

typedef struct subject {
	char ID[15];				//��Ŀ���
	char type;					//range from 1 to 5;
	char begin_time[8];			//��ʼʱ��
	float fund;					//����
	char person_in_charge[12];	//������
	char team[30];				//�����Ŷ�
	struct subject *next;
}SUBJECT_NODE;





/**
*��Ļ������Ϣ�������ṹ
*/

typedef struct layer_node {
	char LayerNo;            /**< �������ڲ���*/
	SMALL_RECT rcArea;       /**< ����������������*/
	CHAR_INFO *pContent;     /**< �������������ַ���Ԫԭ��Ϣ�洢������*/
	char *pScrAtt;           /**< �������������ַ���Ԫԭ����ֵ�洢������*/
	struct layer_node *next; /**< ָ����һ����ָ��*/
} LAYER_NODE;



/**
*��ǩ���ṹ
*/

typedef struct labe1_bundle {
	char **ppLabel;        /**< ��ǩ�ַ��������׵�ַ*/
	COORD *pLoc;           /**< ��ǩ��λ�����׵�ַ*/
	int num;               /**< ��ǩ����*/
} LABEL_BUNDLE;

/**

*�����ṹ

*/

typedef struct hot_area {
	SMALL_RECT *pArea;     /**< ������λ�����׵�ַ*/
	char *pSort;           /**< �������(�������ı���ѡ���)�����׵�ַ*/
	char *pTag;            /**< ������������׵�ַ*/
	int num;               /**< ��������*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;               /*����������Ϣ����ͷ*/
SCHOOL_NODE*gp_head = NULL;                     /*����ͷָ��*/


char *gp_sys_name = "������Ϣ����ϵͳ";    /*ϵͳ����*/
char *gp_team_info_filename = "team.dat";        /*ѧ��������Ϣ�����ļ�*/
char *gp_subject_info_filename = "subject.dat";  /*ס�޽ɷ���Ϣ�����ļ�*/
char *gp_school_info_filename = "school.dat";      /*����¥��Ϣ�����ļ�*/

char *ga_main_menu[] = { "�ļ�(F)",             /*ϵͳ���˵���*/
"����ά��(M)",
"���ݲ�ѯ(Q)",
"����ͳ��(S)",
"����(H)"
};



char *ga_sub_menu[] = { "[S] ���ݱ���",          /*ϵͳ�Ӳ˵���*/
"[X] �˳�    Alt+X",
"[S] Ժϵ��Ϣ",
"[T] �Ŷ���Ϣ",
"[P]������Ŀ",
"[D] Ժϵ������",
"[P]Ժϵ���� ",
"",
"[C] �Ŷ�����",
"[S] ��ʦ����",
"",
"[I]��Ŀ���",
"[B]�����Ŷ�",
"[P] ѧ����ʦ��",
"[C] ������Ŀ��",
"[I] �����Ŷ�",
"[T] ��Ŀ��ʦ��",
"[C] ѧԺ��Ϣ����",
"[A] ����..."
};

int ga_sub_menu_count[] = { 2, 3, 8, 5,1 };  /*�����˵������Ӳ˵��ĸ���*/
int gi_sel_menu = 1;                        /*��ѡ�е����˵����,��ʼΪ1*/
int gi_sel_sub_menu = 0;                    /*��ѡ�е��Ӳ˵����,��ʼΪ0,��ʾδѡ��*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/

CHAR_INFO *gp_buff_stateBar_info = NULL;    /*���״̬����Ļ���ַ���Ϣ�Ļ�����*/
char *gp_scr_att = NULL;    /*�����Ļ���ַ���Ԫ����ֵ�Ļ�����*/
char *gp_sub_code = NULL;   /*��ſ�����������ݻ�����*/
char gc_sys_state = '\0';   /*��������ϵͳ״̬���ַ�*/
unsigned long gul_sub_code_len = 0;    /*����������*/

HANDLE gh_std_out;          /*��׼����豸���*/
HANDLE gh_std_in;           /*��׼�����豸���*/



int LoadCode(char *filename, char **ppbuffer);  /*��������*/

int CreatList(SCHOOL_NODE **pphead);              /*���������ʼ��*/
void InitInterface(void);                 /*ϵͳ�����ʼ��*/
void ClearScreen(void);                         /*����*/
void ShowMenu(void);                            /*��ʾ�˵���*/
void PopMenu(int num);                          /*��ʾ�����˵�*/
void PopPrompt(int num);                        /*��ʾ��������*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*����������Ļ��Ϣά��*/
void PopOff(void);                              /*�رն��㵯������*/
void DrawBox(SMALL_RECT *parea);                /*���Ʊ߿�*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*���˵������˵���λ*/
void ShowState(void);                           /*��ʾ״̬��*/
void TagMainMenu(int num);                      /*��Ǳ�ѡ�е����˵���*/
void TagSubMenu(int num);                       /*��Ǳ�ѡ�е��Ӳ˵���*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*��������*/
void RunSys(SCHOOL_NODE **pphd);                  /*ϵͳ����ģ���ѡ�������*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*����ģ��ĵ���*/
void CloseSys(SCHOOL_NODE *phd);                  /*�˳�ϵͳ*/
BOOL ShowModule(char **pString, int n);

BOOL LoadData(void);           /*���ݼ���*/
BOOL SaveData(void);           /*��������*/
BOOL BackupData(void);         /*��������*/
BOOL RestoreData(void);        /*�ָ�����*/
BOOL ExitSys(void);            /*�˳�ϵͳ*/


BOOL MaintainSchInfo(void);
BOOL MaintainTeamInfo(void);    /*ά��ѧ��������Ϣ*/
BOOL MaintainSubjectInfo(void); /*ά��ѧ���ɷ���Ϣ*/


BOOL QurrySchoolP(void);//��ѯѧУ������
BOOL QurrySchoolName(void);//��ѯѧԺ��
BOOL QurryTeamName(void);//��ѯ�Ŷ���
BOOL QurryProfAcc(void);//��ѯ��ʦ����
BOOL QurrySubID(void);//��ѯ��Ŀ���
BOOL QurryTeamBelong(void);//��ѯ�����Ŷ�


BOOL StatPro2Stu(void);//ͳ�ƽ�ʦ��ѧ��
BOOL StatSubjectAcc(void);//ͳ��ѧԺ��Ŀ��
BOOL StatESubject(void);//ͳ��������Ŀ
BOOL StatSub2Pro(void);//ͳ����Ŀ�Ƚ�ʦ
BOOL Other(void);//��ʾѧԺ��Ϣ



BOOL About(void);//����


SCHOOL_NODE *SearchPIC(SCHOOL_NODE*, char*);//����ѧԺ������
BOOL SeekSchoolName(SCHOOL_NODE*, char*);//����ѧԺ��
BOOL SeekTeamName(SCHOOL_NODE*, char*);//�����Ŷ���Ϣ
SUBJECT_NODE*  SeekProID(SCHOOL_NODE*, char*);//������Ŀ���
TEAM_NODE* MatchProID_1(SCHOOL_NODE*hd, char* id);//ƥ����Ŀ��ŵ�һ��
SUBJECT_NODE* MatchProID_2(SCHOOL_NODE*hd, char* id);//ƥ����Ŀ��ŵڶ���
SCHOOL_NODE * MatchTeamNode_Step_1(SCHOOL_NODE * hd, char * team);//ƥ���Ŷӽڵ��һ��
TEAM_NODE * MatchTeamNode_Step_2(SCHOOL_NODE * hd, char * team);//ƥ���Ŷӽڵ�ڶ���
SCHOOL_NODE *MatchSchoolName(SCHOOL_NODE*hd, char * school);//ƥ��ѧԺ��
TEAM_NODE *MatchTeamName(SCHOOL_NODE *, char*);//ƥ���Ŷ���



void FullFillSchInfo(SCHOOL_NODE*);//��дѧԺ��Ϣ
void FullFillTeamInfo(TEAM_NODE*);//��д�Ŷ���Ϣ
void FullFillProInfo(SUBJECT_NODE*);//��д��Ŀ��Ϣ


BOOL DelProInfo(void);//ɾ����Ŀ��Ϣ
BOOL DelTeamNode(void);//ɾ���Ŷӽڵ�
BOOL DelSchoolNode(void);//ɾ��ѧԺ�ڵ�


BOOL InsertProInfo(void);//������Ŀ�ڵ�
BOOL InsertTeamNode(void);//�����Ŷӽڵ�
BOOL ModifyProInfo_2(void);//�޸���Ŀ��Ϣ
BOOL ModifyTeamInfo(void);//�޸�ѧԺ��Ϣ;

void SortSchoolInfo_1(SCH_DATA* hd);//��ʦ��ѧ������
void SortSchoolInfo_2(SCH_DATA* hd);//��Ŀ������
void SortTeamInfo_1(TEAM_DATA*);//������Ŀ������
void SortTeamInfo_2(TEAM_DATA*);//��Ŀ����ʦ����


void StatSchInfo(SCHOOL_NODE * hd);
void statistic_1(SCHOOL_NODE * hd);
void statistic_2(SCHOOL_NODE * hd);
void statistic_3(SCHOOL_NODE * hd);
void statistic_4(SCHOOL_NODE * hd);

TEAM_NODE* SeekProID_2(SCHOOL_NODE*hd, char *id);
BOOL SaveSysData(SCHOOL_NODE*);

