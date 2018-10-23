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

#define SCR_ROW 25             /*屏幕行数*/
#define SCR_COL 80             /*屏幕列数*/



//data struct

//院系
typedef struct school {

	char name[20];				//院系名称
	char person_in_charge[12];	//负责人
	char phone[15];				//联系号码
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

//团队

typedef struct team {
	char name[20];				//团队名称
	char person_in_charge[12];	//负责人
	int tea_amount;				//教授人数
	int ug_amount;				//研究生人数
	char school[20];			//所属院系
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

//课题

typedef struct subject {
	char ID[15];				//项目编号
	char type;					//range from 1 to 5;
	char begin_time[8];			//起始时间
	float fund;					//经费
	char person_in_charge[12];	//负责人
	char team[30];				//所属团队
	struct subject *next;
}SUBJECT_NODE;





/**
*屏幕窗口信息链结点结点结构
*/

typedef struct layer_node {
	char LayerNo;            /**< 弹出窗口层数*/
	SMALL_RECT rcArea;       /**< 弹出窗口区域坐标*/
	CHAR_INFO *pContent;     /**< 弹出窗口区域字符单元原信息存储缓冲区*/
	char *pScrAtt;           /**< 弹出窗口区域字符单元原属性值存储缓冲区*/
	struct layer_node *next; /**< 指向下一结点的指针*/
} LAYER_NODE;



/**
*标签束结构
*/

typedef struct labe1_bundle {
	char **ppLabel;        /**< 标签字符串数组首地址*/
	COORD *pLoc;           /**< 标签定位数组首地址*/
	int num;               /**< 标签个数*/
} LABEL_BUNDLE;

/**

*热区结构

*/

typedef struct hot_area {
	SMALL_RECT *pArea;     /**< 热区定位数组首地址*/
	char *pSort;           /**< 热区类别(按键、文本框、选项框)数组首地址*/
	char *pTag;            /**< 热区序号数组首地址*/
	int num;               /**< 热区个数*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;               /*弹出窗口信息链链头*/
SCHOOL_NODE*gp_head = NULL;                     /*主链头指针*/


char *gp_sys_name = "科研信息管理系统";    /*系统名称*/
char *gp_team_info_filename = "team.dat";        /*学生基本信息数据文件*/
char *gp_subject_info_filename = "subject.dat";  /*住宿缴费信息数据文件*/
char *gp_school_info_filename = "school.dat";      /*宿舍楼信息数据文件*/

char *ga_main_menu[] = { "文件(F)",             /*系统主菜单名*/
"数据维护(M)",
"数据查询(Q)",
"数据统计(S)",
"帮助(H)"
};



char *ga_sub_menu[] = { "[S] 数据保存",          /*系统子菜单名*/
"[X] 退出    Alt+X",
"[S] 院系信息",
"[T] 团队信息",
"[P]科研项目",
"[D] 院系负责人",
"[P]院系名称 ",
"",
"[C] 团队名称",
"[S] 教师人数",
"",
"[I]项目编号",
"[B]所属团队",
"[P] 学生教师比",
"[C] 科研项目数",
"[I] 优秀团队",
"[T] 项目教师比",
"[C] 学院信息概览",
"[A] 关于..."
};

int ga_sub_menu_count[] = { 2, 3, 8, 5,1 };  /*各主菜单项下子菜单的个数*/
int gi_sel_menu = 1;                        /*被选中的主菜单项号,初始为1*/
int gi_sel_sub_menu = 0;                    /*被选中的子菜单项号,初始为0,表示未选中*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*存放菜单条屏幕区字符信息的缓冲区*/

CHAR_INFO *gp_buff_stateBar_info = NULL;    /*存放状态条屏幕区字符信息的缓冲区*/
char *gp_scr_att = NULL;    /*存放屏幕上字符单元属性值的缓冲区*/
char *gp_sub_code = NULL;   /*存放课题代码表的数据缓冲区*/
char gc_sys_state = '\0';   /*用来保存系统状态的字符*/
unsigned long gul_sub_code_len = 0;    /*课题代码表长度*/

HANDLE gh_std_out;          /*标准输出设备句柄*/
HANDLE gh_std_in;           /*标准输入设备句柄*/



int LoadCode(char *filename, char **ppbuffer);  /*代码表加载*/

int CreatList(SCHOOL_NODE **pphead);              /*数据链表初始化*/
void InitInterface(void);                 /*系统界面初始化*/
void ClearScreen(void);                         /*清屏*/
void ShowMenu(void);                            /*显示菜单栏*/
void PopMenu(int num);                          /*显示下拉菜单*/
void PopPrompt(int num);                        /*显示弹出窗口*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*弹出窗口屏幕信息维护*/
void PopOff(void);                              /*关闭顶层弹出窗口*/
void DrawBox(SMALL_RECT *parea);                /*绘制边框*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*主菜单下拉菜单定位*/
void ShowState(void);                           /*显示状态栏*/
void TagMainMenu(int num);                      /*标记被选中的主菜单项*/
void TagSubMenu(int num);                       /*标记被选中的子菜单项*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*设置热区*/
void RunSys(SCHOOL_NODE **pphd);                  /*系统功能模块的选择和运行*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*功能模块的调用*/
void CloseSys(SCHOOL_NODE *phd);                  /*退出系统*/
BOOL ShowModule(char **pString, int n);

BOOL LoadData(void);           /*数据加载*/
BOOL SaveData(void);           /*保存数据*/
BOOL BackupData(void);         /*备份数据*/
BOOL RestoreData(void);        /*恢复数据*/
BOOL ExitSys(void);            /*退出系统*/


BOOL MaintainSchInfo(void);
BOOL MaintainTeamInfo(void);    /*维护学生基本信息*/
BOOL MaintainSubjectInfo(void); /*维护学生缴费信息*/


BOOL QurrySchoolP(void);//查询学校负责人
BOOL QurrySchoolName(void);//查询学院名
BOOL QurryTeamName(void);//查询团队名
BOOL QurryProfAcc(void);//查询教师数量
BOOL QurrySubID(void);//查询项目编号
BOOL QurryTeamBelong(void);//查询所属团队


BOOL StatPro2Stu(void);//统计教师比学生
BOOL StatSubjectAcc(void);//统计学院项目数
BOOL StatESubject(void);//统计优秀项目
BOOL StatSub2Pro(void);//统计项目比教师
BOOL Other(void);//显示学院信息



BOOL About(void);//其他


SCHOOL_NODE *SearchPIC(SCHOOL_NODE*, char*);//查找学院负责人
BOOL SeekSchoolName(SCHOOL_NODE*, char*);//查找学院名
BOOL SeekTeamName(SCHOOL_NODE*, char*);//查找团队信息
SUBJECT_NODE*  SeekProID(SCHOOL_NODE*, char*);//查找项目编号
TEAM_NODE* MatchProID_1(SCHOOL_NODE*hd, char* id);//匹配项目编号第一步
SUBJECT_NODE* MatchProID_2(SCHOOL_NODE*hd, char* id);//匹配项目编号第二步
SCHOOL_NODE * MatchTeamNode_Step_1(SCHOOL_NODE * hd, char * team);//匹配团队节点第一步
TEAM_NODE * MatchTeamNode_Step_2(SCHOOL_NODE * hd, char * team);//匹配团队节点第二步
SCHOOL_NODE *MatchSchoolName(SCHOOL_NODE*hd, char * school);//匹配学院名
TEAM_NODE *MatchTeamName(SCHOOL_NODE *, char*);//匹配团队名



void FullFillSchInfo(SCHOOL_NODE*);//填写学院信息
void FullFillTeamInfo(TEAM_NODE*);//填写团队信息
void FullFillProInfo(SUBJECT_NODE*);//填写项目信息


BOOL DelProInfo(void);//删除项目信息
BOOL DelTeamNode(void);//删除团队节点
BOOL DelSchoolNode(void);//删除学院节点


BOOL InsertProInfo(void);//插入项目节点
BOOL InsertTeamNode(void);//插入团队节点
BOOL ModifyProInfo_2(void);//修改项目信息
BOOL ModifyTeamInfo(void);//修改学院信息;

void SortSchoolInfo_1(SCH_DATA* hd);//教师比学生排序
void SortSchoolInfo_2(SCH_DATA* hd);//项目数排序
void SortTeamInfo_1(TEAM_DATA*);//优秀项目数排序
void SortTeamInfo_2(TEAM_DATA*);//项目比老师排序


void StatSchInfo(SCHOOL_NODE * hd);
void statistic_1(SCHOOL_NODE * hd);
void statistic_2(SCHOOL_NODE * hd);
void statistic_3(SCHOOL_NODE * hd);
void statistic_4(SCHOOL_NODE * hd);

TEAM_NODE* SeekProID_2(SCHOOL_NODE*hd, char *id);
BOOL SaveSysData(SCHOOL_NODE*);

