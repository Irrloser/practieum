#include "Config.h"
#include <string>

using namespace std;

Config::Config(void)
{
}

Config::~Config(void)
{
}

const string Config::SERVERADDRESS = "127.0.0.1";	//������IP��ַ
const int Config::MAXCONNECTION = 5;				//���������5
const int Config::BUFFERLENGTH = 4096;				//��������С256�ֽ�
const int Config::PORT = 80;						//�������˿�5050
const u_long Config::BLOCKMODE = 1;					//SOCKETΪ������ģʽ
const string Config::url="C:\\Users\\KatowiZz\\Desktop\\ChatRoomServerAsyn\\Debug\\";