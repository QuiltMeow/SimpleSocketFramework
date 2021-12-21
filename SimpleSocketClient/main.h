#pragma once

#include <iostream>
#include <sstream>
#include <filesystem>
#include <mutex>
#include <sys/stat.h>
#include <pthread.h>

#include "Session.h"
#include "PacketLittleEndianWriter.h"
#include "PacketHandler.h"
#include "Connector.h"
#include "WindowsCompatible.h"

#define CONNECT_IP_ADDRESS "127.0.0.1"
#define CONNECT_PORT 8923
#define PAUSE cout << "請按任意鍵繼續 ... "; getchar();

using namespace std;

mutex response;
void waitResponse();

int parseInt(string);
LPVOID uiThread(LPVOID lpParamter);
bool isFileExist(string);
string getFileName(string);
