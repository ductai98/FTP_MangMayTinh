// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS         // remove support for MFC controls in dialog

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
#include <fstream>
#include <string>
#include <afxsock.h>
#include <direct.h>
#include <atlstr.h>
// TODO: reference additional headers your program requires here

using namespace std;

string getCommand();
int getPORT(char* command);
void  getDirectoryServer(CSocket &controlSock);
char* getListActive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port);
char* getListPassive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port);
int getFileActive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port, char* h_filename);
int getFilePassive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port, char* h_filename);
int putFileActive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port, char* h_filename);
int putFilePassive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port, char* h_filename);
int changeDirClient(char* h_dirName);
void getCurrentDirClient();
int changeDirServer(CSocket &controlSock, char* h_dirName);
int makeDirectoryServer(CSocket &controlSock, char* h_filename);
int removeFileServer(CSocket &controlSock, char* h_filename);
int removeEmptyDirectoryServer(CSocket &controlSock, char* h_dirName);
int removeAllEmptyDirectoryServer(CSocket &controlSock1, CSocket &dataSock1, unsigned int &h_port1);
int disconnectServer(CSocket &controlSock);