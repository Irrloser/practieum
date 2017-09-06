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

#define SCR_ROW 25
#define SCR_COL 80

//data struct
//Ժϵ
typedef struct SCHOOL {
	char name[20];				//Ժϵ����
	char person_in_charge[12];	//������
	char phone[15];				//��ϵ����
	struct TEAM* head;
	struct SCHOOL* next;
}school;
//�Ŷ�
typedef struct TEAM {
	char name[20];				//�Ŷ�����
	char person_in_charge[12];	//������
	int tea_amount;				//��������
	int ug_amount;				//�о�������
	char school[20];			//����Ժϵ
	struct TEAM *next;
	struct SUBJECT *head;
}team;
//����
typedef struct SUBJECT {
	char ID[15];				//��Ŀ���
	char type;					//range from 1 to 5;
	char begin_time[8];			//��ʼʱ��
	float fund;					//����
	char person_in_charge[12];	//������
	char team[30];				//�����Ŷ�
	struct SUBJECT *next;
}subject;

//��Ļ������Ϣ�������ṹ
typedef struct layer_node {
	char LayerNo;				// �������ڲ���
	SMALL_RECT rcArea;			//< ����������������
	CHAR_INFO *pContent;		//�������������ַ���Ԫԭ��Ϣ�洢������
	char *pScrAtt;				// �������������ַ���Ԫԭ����ֵ�洢������
	struct layer_node *next;	// ָ����һ����ָ��
} LAYER_NODE;
//��ǩ���ṹ

typedef struct labe1_bundle {
	char **ppLabel;        //< ��ǩ�ַ��������׵�ַ
	COORD *pLoc;           // ��ǩ��λ�����׵�ַ
	int num;               // ��ǩ����
} LABEL_BUNDLE;

//����
typedef struct hot_area {
	SMALL_RECT *pArea;     /**< ������λ�����׵�ַ*/
	char *pSort;           /**< �������(�������ı���ѡ���)�����׵�ַ*/
	char *pTag;            /**< ������������׵�ַ*/
	int num;               /**< ��������*/
} HOT_AREA;

//claimation
LAYER_NODE *top_layer = NULL;
school *sch_top = NULL;		//����ͷָ��

char *sys_name = "������Ŀ��Ϣ����ϵͳ";
char *Pschool_info = "school.dat";
char *Pteam_info = "team.dat";
char *Psubject_info = "subject.dat";
//todo �ļ�ָ��

//���˵�ѡ��
char* main_menu[] = {
	"����ά��(F1)",
	"���ݲ�ѯ(F2)",
	"����ͳ��(F3)",
	"����(F4)",
	"�ļ�(F5)"
};
//�����˵�ѡ��
char* sub_menu[] = {
	"[S]Ժϵ��Ϣά��",
	"[T]�Ŷ���Ϣά��",
	"[B]��Ŀ��Ϣά��",
	"",
	"[Z]��Ժϵ�����˲�ѯ",
	"[X]��Ժϵ���Ʋ�ѯ",
	"[C]���Ŷ����Ʋ�ѯ",
	"[V]����ʦ������ѯ",
	"[N]����Ŀ��Ų�ѯ",
	"[M]����Ŀ���Ʋ�ѯ",
	"",
	"[J]ʦ����",
	"[K]��Ŀ���",
	"[P]�ص��Ŷ�",
	"[Q]��Ŀ��ʦ��",
	"",
	"[A]����",
	"[E]�˳�"
	//�����ѯtodo
};
int sub_menu_count[] = { 4,7,5,2 };			//�������˵����Ӳ˵�����
int gi_sel_menu = 1;						//��ѡ�е����˵���� ��ʼֵ1
int gi_sel_sub_menu = 0;					//��ѡ�е��Ӳ˵����,��ʼֵ0
CHAR_INFO *gp_buff_menubar_info = NULL;     //��Ų˵�����Ļ���ַ���Ϣ�Ļ�����
CHAR_INFO *gp_buff_stateBar_info = NULL;    //���״̬����Ļ���ַ���Ϣ�Ļ�����

char * P_scr_att = NULL;
char *P_sch_code = NULL;					//���ѧԺ����Ļ�����
char *P_team_code = NULL;					//����ŶӴ���Ļ�����
char *P_subj_code = NULL;					//�����Ŀ����Ļ�����
char sys_state = '\0';						//����ϵͳ״̬���ַ�

unsigned short school_code_len = 0;			//ѧԺ���볤��
unsigned short team_code_len = 0;			//�ŶӴ��볤��
unsigned short subj_code_len = 0;			//��Ŀ���볤��

HANDLE std_out;								//��׼������
HANDLE std_in;								//��׼������

int LodaCode(char *filename, char **PPbuffer);//���ش����
int CreateList(school** PPhead);			 //��������
void InitInterface(void);					 //��ʼ������
void Clear(void);							 //����
void ShowMenu(void);						 //��ʾ�˵�
void PopMenu(int num);						 //��ʾ�����˵�
void PopPrompt(int num);					 //��ʾ��������
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE*, HOT_AREA*);
//����������Ϣά��

void PopOff(void);							 //�رն��㴰��
void DrawBox(SMALL_RECT *P_area);			 //���Ʊ߿�
void LocSubMenu(int num, SMALL_RECT* P_area);//�����˵���λ
void ShowState(void);						 //��ʾ״̬
void TagMainMenu(int num);					 //��Ǳ�ѡ�е���ѡ��
void TagSubMenu(int num);					 //��Ǳ�ѡ�е������˵�
int DealConInput(HOT_AREA *phot_area, int *pihot_num);
//����̨���봦��
void SetHotPoint(HOT_AREA *phot_area, int hot_num);
//��������
void RunSys(school **P_sch);				 //ϵͳ����ģ���ѡ��������
BOOL ExeFunction(int main_menu_num, int sub_menu_num);
//���ù���ģ��
void CloseSys(school *P_sch);				 //�ر�ϵͳ
int CreatList(school** pphead);				 //���������ʼ��



BOOL LoadData(void);
BOOL SaveData(void);
BOOL ExitSys(void);
BOOL HELP(void);
BOOL ABOUT(void);
BOOL MaintainSchool(void);					 //ά��Ժϵ��Ϣ
BOOL MaintainTeam(void);					 //ά��������Ϣ
BOOL MaintainSubject(void);					 //ά����Ŀ��Ϣ
BOOL QurrySchoolP(void);
BOOL QurrySchoolName(void);
BOOL QurryTeamName(void);
BOOL QurryProAcc(void);
BOOL QurrySubjNum(void);
BOOL QurrySubjName(void);
BOOL StatPro2Stu(void);
BOOL StatSubType(void);
BOOL StatSub2Prof(void);
BOOL StatMostFund(void);

