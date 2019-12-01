// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ConsoleApplication1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;



int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			// Khoi tao thu vien Socket
			if (AfxSocketInit() == FALSE)
			{
				cout << "Khong the khoi tao Socket Libraray";
				return FALSE;
			}

			// Tao socket gui/nhan lenh
			string ipServer;
			unsigned int port;
			unsigned int p1, p2;
			cout << "Nhap port: ";
			cin >> port;
			cout << endl;

			// Tao socket gui/nhan lenh
			CSocket ClientSocket;
			ClientSocket.Create(port);

			// Tao socket gui/nhan du lieu
			CSocket DataSocket;

			// Ket noi den Server
			if (ClientSocket.Connect(_T("127.0.0.1"), 21) != 0)
			{
				cout << "Ket noi toi Server thanh cong !!!" << endl << endl;
			}
			else
				return FALSE;


			char buf[BUFSIZ + 1];
			int tmpres;
			printf("Connection established, waiting for welcome message...\n");
			memset(buf, 0, sizeof buf);
			ClientSocket.Receive(buf, BUFSIZ, 0);
			puts(buf);

			while (1)
			{
				//Send Username
				char info[50];
				printf("Name (%s): ", "127.0.0.1");
				memset(buf, 0, sizeof buf);
				cin >> info;
				
				if (strstr(info,"0")!=NULL)
				{
					return nRetCode;
				}

				sprintf_s(buf, "USER %s\r\n", info);
				tmpres = ClientSocket.Send(buf, strlen(buf), 0);

				memset(buf, 0, sizeof buf);
				tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
				puts(buf);

				//Send Password
				memset(info, 0, sizeof info);
				printf("Password: ");
				memset(buf, 0, sizeof buf);
				cin >> info;
				cout << endl;
				sprintf_s(buf, "PASS %s\r\n", info);
				tmpres = ClientSocket.Send(buf, strlen(buf), 0);

				memset(buf, 0, sizeof buf);
				tmpres = ClientSocket.Receive(buf, BUFSIZ, 0);
				puts(buf);
				if (strstr(buf, "230")==NULL)
				{
					cout << "Sai tai khoan hoac mat khau!" << endl << endl;
				}
				else
				{
					break;
				}
			}

			while (1)
			{
				string cmd;
				cmd = getCommand();
				int i;

				//Download file tu server
				if (cmd == "get")
				{
					char filename[50];
					cout << "Nhap server filename: ";
					cin >> filename;
					cout << endl;
					cout << "Nhap che do truyen/nhan du lieu: 1 - ACTIVE\t2 - PASSIVE" << endl << endl;
					cin >> i;
					if (i == 1)
					{
						cout << "Truyen du lieu theo che do ACTIVE!" << endl << endl;
						getFileActive(ClientSocket, DataSocket, port, filename);
					}
					else
					{
						cout << "Truyen du lieu theo che do PASSIVE!" << endl << endl;
						getFilePassive(ClientSocket, DataSocket, port, filename);
					}
				}

				// Upload file tu Server
				if (cmd == "put")
				{
					char filename[50];
					cout << "Nhap server filename: ";
					cin >> filename;
					cout << endl;
					cout << "Nhap che do truyen/nhan du lieu: 1 - ACTIVE\t2 - PASSIVE" << endl;
					cin >> i;

					if (i == 1)
					{
						cout << "Truyen du lieu theo che do ACTIVE!" << endl << endl;
						putFileActive(ClientSocket, DataSocket, port, filename);
					}
					else
					{
						cout << "Truyen du lieu theo che do PASSIVE!" << endl << endl;
						putFilePassive(ClientSocket, DataSocket, port, filename);
					}
				}

				
				// In ra danh sach file tren duong dan hien tai
				if (cmd == "ls")
				{
					cout << "Nhap che do truyen/nhan du lieu: 1 - ACTIVE\t2 - PASSIVE" << endl;
					cin >> i;
					if (i == 1)
					{
						cout << "Truyen du lieu theo che do ACTIVE!" << endl << endl;
						getListActive(ClientSocket, DataSocket, port);

					}
					else
					{
						cout << "Truyen du lieu theo che do PASSIVE!" << endl << endl;
						getListPassive(ClientSocket, DataSocket, port);
					}
				}

		
				//Lay duong dan hien tai duoi Client
				if (cmd == "lpwd")
				{
					getCurrentDirClient();
				}


				//Thay doi duong dan duoi Client
				if (cmd == "lcd")
				{
					char dirName[MAX_PATH];
					cout << "Nhap duong dan di chuyen duoi Client: ";
					cin >> dirName;
					cout << endl;

					if (changeDirClient(dirName) == 1)
					{
						cout << "Thay doi duong dan thanh cong!" << endl << endl;
					}
					else
					{
						cout << "Thay doi duong dan that bai!" << endl << endl;
					}
				}


				// Lay duong dan hien tai tren Server
				if (cmd == "pwd")
				{
					getDirectoryServer(ClientSocket);
				}

				//Thay doi duong dan tren Server 
				if (cmd == "cd")
				{
					char dirName[MAX_PATH];
					cout << "Nhap duong dan can di chuyen tren Server: ";
					cin >> dirName;
					cout << endl;
					if (changeDirServer(ClientSocket, dirName) == 1)
					{
						cout << "Thay doi duong dan thanh cong!" << endl << endl;
					}
					else
					{
						cout << "Thay doi duong dan that bai!" << endl << endl;
					}

				}
				
				
				if (cmd == "mkdir")
				{
					char name[50];
					cout << "Nhap ten thu muc can tao: ";
					cin >> name;
					cout << endl;
					if (makeDirectoryServer(ClientSocket, name))
					{
						cout << "Tao thu muc thanh cong!" << endl << endl;
					}
					else
					{
						cout << "Tao thu muc that bai!" << endl << endl;
					}
				}


				if (cmd == "rmdir")
				{
					char name[50];
					cout << "Nhap ten thu muc rong can xoa: ";
					cin >> name;
					cout << endl;
					if (removeEmptyDirectoryServer(ClientSocket, name) == 1)
					{
						cout << "Xoa thu muc thanh cong!" << endl << endl;
					}
					else
					{
						cout << "Xoa thu muc khong thanh cong!" << endl << endl;
					}
				}
				
				if (cmd == "rmdir/all")
				{
					int dem = removeAllEmptyDirectoryServer(ClientSocket, DataSocket, port);
					cout << "Da xoa " << dem << " thu muc rong tren duong dan hien tai!" << endl << endl;
				}


				if (cmd == "delete")
				{
					char name[50];
					cout << "Nhap ten file can xoa: ";
					cin >> name;
					cout << endl;
					if (removeFileServer(ClientSocket,name)==1)
					{
						cout << "Xoa file thanh cong!" << endl << endl;
					}
					else
					{
						cout << "Xoa file that bai!" << endl << endl;
					}
				}

				if (cmd == "quit" || cmd == "exit")
				{
					if (disconnectServer(ClientSocket) == 1)
					{
						cout << "Dong ket noi thanh cong!" << endl << endl;
						ClientSocket.Close();
						break;
					}
					else
					{
						cout << "Dong ket noi that bai!" << endl << endl;
					}
				}

				if (cmd == "0")
				{
					break;
				}
			}

			ClientSocket.Close();
			cin.get();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
