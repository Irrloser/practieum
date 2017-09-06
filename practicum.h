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
//院系
typedef struct SCHOOL {
	char name[20];				//院系名称
	char person_in_charge[12];	//负责人
	char phone[15];				//联系号码
	struct TEAM* head;
	struct SCHOOL* next;
}school;
//团队
typedef struct TEAM {
	char name[20];				//团队名称
	char person_in_charge[12];	//负责人
	int tea_amount;				//教授人数
	int ug_amount;				//研究生人数
	char school[20];			//所属院系
	struct TEAM *next;
	struct SUBJECT *head;
}team;
//课题
typedef struct SUBJECT {
	char ID[15];				//项目编号
	char type;					//range from 1 to 5;
	char begin_time[8];			//起始时间
	float fund;					//经费
	char person_in_charge[12];	//负责人
	char team[30];				//所属团队
	struct SUBJECT *next;
}subject;

//屏幕窗口信息链结点结点结构
typedef struct layer_node {
	char LayerNo;				// 弹出窗口层数
	SMALL_RECT rcArea;			//< 弹出窗口区域坐标
	CHAR_INFO *pContent;		//弹出窗口区域字符单元原信息存储缓冲区
	char *pScrAtt;				// 弹出窗口区域字符单元原属性值存储缓冲区
	struct layer_node *next;	// 指向下一结点的指针
} LAYER_NODE;
//标签束结构

typedef struct labe1_bundle {
	char **ppLabel;        //< 标签字符串数组首地址
	COORD *pLoc;           // 标签定位数组首地址
	int num;               // 标签个数
} LABEL_BUNDLE;

//热区
typedef struct hot_area {
	SMALL_RECT *pArea;     /**< 热区定位数组首地址*/
	char *pSort;           /**< 热区类别(按键、文本框、选项框)数组首地址*/
	char *pTag;            /**< 热区序号数组首地址*/
	int num;               /**< 热区个数*/
} HOT_AREA;

//claimation
LAYER_NODE *top_layer = NULL;
school *sch_top = NULL;		//主链头指针

char *sys_name = "科研项目信息管理系统";
char *Pschool_info = "school.dat";
char *Pteam_info = "team.dat";
char *Psubject_info = "subject.dat";
//todo 文件指针

//主菜单选项
char* main_menu[] = {
	"数据维护(F1)",
	"数据查询(F2)",
	"数据统计(F3)",
	"帮助(F4)",
	"文件(F5)"
};
//下拉菜单选项
char* sub_menu[] = {
	"[S]院系信息维护",
	"[T]团队信息维护",
	"[B]项目信息维护",
	"",
	"[Z]按院系负责人查询",
	"[X]按院系名称查询",
	"[C]按团队名称查询",
	"[V]按教师人数查询",
	"[N]按项目编号查询",
	"[M]按项目名称查询",
	"",
	"[J]师生比",
	"[K]项目类别",
	"[P]重点团队",
	"[Q]项目老师比",
	"",
	"[A]关于",
	"[E]退出"
	//额外查询todo
};
int sub_menu_count[] = { 4,7,5,2 };			//各下拉菜单下子菜单个数
int gi_sel_menu = 1;						//被选中的主菜单项号 初始值1
int gi_sel_sub_menu = 0;					//被选中的子菜单编号,初始值0
CHAR_INFO *gp_buff_menubar_info = NULL;     //存放菜单条屏幕区字符信息的缓冲区
CHAR_INFO *gp_buff_stateBar_info = NULL;    //存放状态条屏幕区字符信息的缓冲区

char * P_scr_att = NULL;
char *P_sch_code = NULL;					//存放学院代码的缓存区
char *P_team_code = NULL;					//存放团队代码的缓冲区
char *P_subj_code = NULL;					//存放项目代码的缓冲区
char sys_state = '\0';						//保存系统状态的字符

unsigned short school_code_len = 0;			//学院代码长度
unsigned short team_code_len = 0;			//团队代码长度
unsigned short subj_code_len = 0;			//项目代码长度

HANDLE std_out;								//标准输出句柄
HANDLE std_in;								//标准输入句柄

int LodaCode(char *filename, char **PPbuffer);//加载代码表
int CreateList(school** PPhead);			 //创建链表
void InitInterface(void);					 //初始化界面
void Clear(void);							 //清屏
void ShowMenu(void);						 //显示菜单
void PopMenu(int num);						 //显示下拉菜单
void PopPrompt(int num);					 //显示弹出窗口
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE*, HOT_AREA*);
//弹出窗口信息维护

void PopOff(void);							 //关闭顶层窗口
void DrawBox(SMALL_RECT *P_area);			 //绘制边框
void LocSubMenu(int num, SMALL_RECT* P_area);//下拉菜单定位
void ShowState(void);						 //显示状态
void TagMainMenu(int num);					 //标记被选中的主选单
void TagSubMenu(int num);					 //标记被选中的下拉菜单
int DealConInput(HOT_AREA *phot_area, int *pihot_num);
//控制台输入处理
void SetHotPoint(HOT_AREA *phot_area, int hot_num);
//设置热区
void RunSys(school **P_sch);				 //系统功能模块的选择与运行
BOOL ExeFunction(int main_menu_num, int sub_menu_num);
//调用功能模块
void CloseSys(school *P_sch);				 //关闭系统
int CreatList(school** pphead);				 //数据链表初始化



BOOL LoadData(void);
BOOL SaveData(void);
BOOL ExitSys(void);
BOOL HELP(void);
BOOL ABOUT(void);
BOOL MaintainSchool(void);					 //维护院系信息
BOOL MaintainTeam(void);					 //维护团体信息
BOOL MaintainSubject(void);					 //维护项目信息
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

