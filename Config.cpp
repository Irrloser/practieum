#include "Config.h"
#include <string>

using namespace std;

Config::Config(void)
{
}

Config::~Config(void)
{
}

const string Config::SERVERADDRESS = "127.0.0.1";	//服务器IP地址
const int Config::MAXCONNECTION = 5;				//最大连接数5
const int Config::BUFFERLENGTH = 4096;				//缓冲区大小256字节
const int Config::PORT = 80;						//服务器端口5050
const u_long Config::BLOCKMODE = 1;					//SOCKET为非阻塞模式
const string Config::url="C:\\Users\\KatowiZz\\Desktop\\ChatRoomServerAsyn\\Debug\\";