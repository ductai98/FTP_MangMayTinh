// stdafx.cpp : source file that includes just the standard includes
// ConsoleApplication1.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
// TODO: reference any additional headers you need in STDAFX.H
// and not in this file



// Ham lay gia tri lenh dieu khien nhap tu ban phim
// Khong co tham so truyen vao
// Tra ve mot chuoi lenh
string getCommand()
{
	string get;
	cout << "Nhap lenh dieu khien: ";
	cin >> get;
	cout << endl;
	return get;
}



// Ham chuyen tu chuoi nhan duoc tu Server sau khi gui lenh PASV sang Port lang nghe cua Server
// Tham so truyen vao: chuoi kieu char
// Gia tri Port lang nghe cua Server
int getPORT(char* command)
{
	int n = strlen(command);
	char s1[5];
	char s2[5];
	for (int i = n; i > 0; i--)
	{
		if ((command[i] >= '0'&&command[i] < '10') && command[i - 1] == ',')
		{
			s1[0] = command[i];
			s1[1] = command[i + 1];
			s1[2] = command[i + 2];
			s2[0] = command[i - 4];
			s2[1] = command[i - 3];
			s2[2] = command[i - 2];
			break;
		}
	}

	int a = atoi(s1);
	int b = atoi(s2);
	int p = b * 256 + a;
	return p;
}



// Ham lay danh sach cac file va folder voi duong dan hien tai tren Server theo che do ACTIVE
// Tham so truyen vao: Control Socket, Data Socket, Data port, IP cua Server
// Tra ve mot chuoi chua danh sach cac file va folder
char* getListActive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port)
{

	// ACTIVE MODE
	char buf[BUFSIZ + 1];
	unsigned int p1, p2;
	h_port = h_port + 1;
	p1 = h_port / 256;
	p2 = h_port % 256;

	// Gui Port lang nghe ket noi den Server
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "PORT 127,0,0,1,%d,%d\r\n", p1, p2);
	controlSock.Send(buf, strlen(buf), 0);

	// Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	// Gui lenh LIST den Server
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "LIST\r\n");
	controlSock.Send(buf, strlen(buf), 0);

	//Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	// Truyen PORT va IP cho DataSocket lang nghe
	dataSock.Create(h_port, SOCK_STREAM);

	// Lang nghe ket noi tu Server
	if (dataSock.Listen() == FALSE)
	{
		cout << "Khong the lang nghe ket noi tu Server!" << endl;
		dataSock.Close();
		return 0;
	}
	
	// Tao socket duy tri ket noi
	CSocket DataConnector;
	if (dataSock.Accept(DataConnector) == FALSE)
	{
		cout << "Khong the chap nhan ket noi tu Server!" << endl;
		dataSock.Close();
		return 0;
	}
	
	// Nhan danh sach cac file
	memset(buf, 0, sizeof buf);
	DataConnector.Receive(buf, BUFSIZ, 0);
	puts(buf);
	
	char retBuf[BUFSIZ + 1];
	strcpy_s(retBuf, buf);
	
	//Nhan lenh tra ve
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);
	
	cout << "Nhan danh sach cac file thanh cong!" << endl << endl;
	
	// Dong ket noi
	dataSock.Close();
	DataConnector.Close();

	return retBuf;
}



// Ham lay danh sach cac file va folder voi duong dan hien tai tren Server theo che do PASSIVE
// Tham so truyen vao: Control Socket, Data Socket, Data port
// Tra ve mot chuoi chua danh sach cac file va folder
char* getListPassive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port)
{
	// PASSIVE MODE

	// Gui lenh PASV
	char buf[BUFSIZ + 1];
	memset(buf, 0, sizeof buf);
	strcpy_s(buf, "PASV\r\n");
	controlSock.Send(buf, strlen(buf), 0);

	// Nhan port tra ve tu server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	// Chuyen sang PORT kieu int
	int ServerPort = getPORT(buf);
	cout << "Server Port: " << ServerPort << endl;

	// Gui lenh LIST
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "LIST\r\n");
	controlSock.Send(buf, strlen(buf), 0);

	//Tao ket noi den server thong qua Port da nhan
	h_port = h_port + 1;
	dataSock.Create(h_port, SOCK_STREAM, _T("127.0.0.1"));
	if (dataSock.Connect(_T("127.0.0.1"), ServerPort) == FALSE)
	{
		cout << "Khong the ket noi de nhan du lieu " << endl;
		dataSock.Close();
		return 0;
	}

	// Nhan du lieu
	memset(buf, 0, sizeof buf);
	dataSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	char retBuf[BUFSIZ + 1];
	strcpy_s(retBuf, buf);

	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	cout << "In danh sach file thanh cong!" << endl << endl;

	// Dong ket noi
	dataSock.Close();

	return retBuf;
}



// Ham download mot file tu Server xuong Client theo che do ACTIVE
// Tham so truyen vao: Control Socket, Data Socket, Data port, ten file can download (ten file lay tu duong dan hien tai tren Server)
// Tra ve 1 neu thanh cong, 0 neu that bai
int getFileActive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port, char* h_filename)
{
	// ACTIVE MODE
	char buf[BUFSIZ + 1];
	unsigned int p1, p2;
	h_port = h_port + 1;
	p1 = h_port / 256;
	p2 = h_port % 256;

	// Gui Port lang nghe ket noi den Server
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "PORT 127,0,0,1,%d,%d\r\n", p1, p2);
	controlSock.Send(buf, strlen(buf), 0);

	// Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	// Gui lenh RETR den Server
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "RETR %s\r\n", h_filename);
	controlSock.Send(buf, strlen(buf), 0);

	// Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	if (strstr(buf, "150")==NULL)
	{
		cout << "Khong tim thay file tren Server!" << endl << endl;
		return 0;
	}
	
	// Truyen PORT va IP cho DataSocket lang nghe
	dataSock.Create(h_port, SOCK_STREAM, _T("127.0.0.1"));

	// Lang nghe ket noi tu Server
	if (dataSock.Listen() == FALSE)
	{
		cout << "Khong the lang nghe ket noi tu Server!" << endl << endl;
		dataSock.Close();
		return 0;
	}

	// Tao socket duy tri ket noi
	CSocket DataConnector;
	if (dataSock.Accept(DataConnector) == FALSE)
	{
		cout << "Khong the chap nhan ket noi tu Server!" << endl << endl;
		dataSock.Close();
		DataConnector.Close();
		return 0;
	}
	
	// Nhan du lieu file
	memset(buf, 0, sizeof buf);
	DataConnector.Receive(buf, BUFSIZ, 0);
	cout << "Noi dung file: " << buf << endl << endl;

	// Ghi ra file
	fstream f;
	f.open(h_filename, ios::out);
	f << buf;
	f.close();

	// Nhan lenh tra ve
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	cout << "Download file " << h_filename << " tu Server thanh cong" << endl << endl;
			
	// Dong ket noi
	dataSock.Close();
	DataConnector.Close();

	return 1;
}



// Ham download mot file tu Server xuong Client theo che do PASSIVE
// Tham so truyen vao: Control Socket, Data Socket, Data port, ten file can download (ten file lay tu duong dan hien tai tren Server)
// Tra ve 1 neu thanh cong, 0 neu that bai
int getFilePassive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port, char* h_filename)
{
	
	// PASSIVE MODE
	
	// Gui lenh PASV
	char buf[BUFSIZ + 1];
	memset(buf, 0, sizeof buf);
	strcpy_s(buf, "PASV\r\n");
	controlSock.Send(buf, strlen(buf), 0);

	// Nhan port tra ve tu server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	// Chuyen sang PORT kieu int
	unsigned int ServerPort = getPORT(buf);
	cout << "Server Port: " << ServerPort << endl << endl;

	// Gui lenh RETR
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "RETR %s\r\n", h_filename);
	controlSock.Send(buf, strlen(buf), 0);

	//Tao ket noi den server thong qua Port da nhan
	h_port = h_port + 1;
	dataSock.Create(h_port, SOCK_STREAM, _T("127.0.0.1"));
	if (dataSock.Connect(_T("127.0.0.1"), ServerPort) == FALSE)
	{
		cout << "Khong the ket noi de nhan du lieu!" << endl << endl;
		cout << "Download khong thanh cong!" << endl << endl;
		dataSock.Close();
		return 0;
	}

	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	cout << buf << endl << endl;

	if (strstr(buf, "150") == NULL)
	{
		cout << "Khong tim thay file tren Server" << endl << endl;
		dataSock.Close();
		return 0;
	}

	// Nhan du lieu file tra ve
	memset(buf, 0, sizeof buf);
	dataSock.Receive(buf, BUFSIZ, 0);
	cout << "Noi dung file: " << buf << endl << endl;

	// Ghi ra file
	fstream f;
	f.open(h_filename, ios::out);
	f << buf;
	f.close();
	
	// Nhan lenh tra ve
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	cout << buf << endl << endl;

	cout << "Download file " << h_filename << " tu Server thanh cong" << endl << endl;
	
	dataSock.Close();
	return 1;
}



// Ham upload mot file tu Client len Server theo che do ACTIVE
// Tham so truyen vao: Control Socket, Data Socket, Data port, ten file can upload (ten file lay tu duong dan hien tai duoi Client)
// Tra ve 1 neu thanh cong, 0 neu that bai
int putFileActive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port, char* h_filename)
{
	// ACTIVE MODE
	char buf[BUFSIZ + 1];
	unsigned int p1, p2;
	h_port = h_port + 1;
	p1 = h_port / 256;
	p2 = h_port % 256;
	
	// Gui Port lang nghe ket noi den Server
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "PORT 127,0,0,1,%d,%d\r\n", p1, p2);
	controlSock.Send(buf, strlen(buf), 0);

	// Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	// Gui lenh STOR den Server
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "STOR %s\r\n", h_filename);
	controlSock.Send(buf, strlen(buf), 0);

	//Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	//Doc file can upload
	memset(buf, 0, sizeof buf);
	fstream f;
	f.open(h_filename, ios::in);
	f.getline(buf, BUFSIZ);
	f.close();
	
	if (strlen(buf) == 0)
	{
		cout << "Khong the mo file!" << endl << endl;
		return 0;
	}

	// Truyen PORT va IP cho DataSocket lang nghe
	dataSock.Create(h_port, SOCK_STREAM, _T("127.0.0.1"));

	// Lang nghe ket noi tu Server
	if (dataSock.Listen() == FALSE)
	{
		cout << "Khong the lang nghe ket noi tu Server!" << endl << endl;
		dataSock.Close();
		return 0;
	}

	// Tao socket duy tri ket noi
	CSocket DataConnector;
	if (dataSock.Accept(DataConnector) == FALSE)
	{
		cout << "Khong the chap nhan ket noi tu Server!" << endl << endl;
		dataSock.Close();
		DataConnector.Close();
		return 0;
	}
	
	// Gui du lieu file len Server
	DataConnector.Send(buf, BUFSIZ, 0);
	cout << "Noi dung file: ";
	cout << buf << endl << endl;

	//Dong ket noi
	dataSock.Close();
	DataConnector.Close();

	//Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	cout << "UPLOAD file " << h_filename << " den Server thanh cong!" << endl << endl;
	
	return 1;
}



// Ham upload mot file tu Client len Server theo che do PASSIVE
// Tham so truyen vao: Control Socket, Data Socket, Data port, ten file can upload (ten file lay tu duong dan hien tai duoi Client)
// Tra ve 1 neu thanh cong, 0 neu that bai
int putFilePassive(CSocket &controlSock, CSocket &dataSock, unsigned int &h_port, char* h_filename)
{
	
	// PASSIVE MODE

	// Gui lenh PASV
	char buf[BUFSIZ + 1];
	memset(buf, 0, sizeof buf);
	strcpy_s(buf, "PASV\r\n");
	controlSock.Send(buf, strlen(buf), 0);

	// Nhan port tra ve tu server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	// Chuyen sang PORT kieu int
	unsigned int ServerPort = getPORT(buf);
	cout << "Server Port: " << ServerPort << endl << endl;

	// Gui lenh STOR
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "STOR %s\r\n", h_filename);
	controlSock.Send(buf, strlen(buf), 0);

	//Tao ket noi den server thong qua Port da nhan
	h_port = h_port + 1;
	dataSock.Create(h_port, SOCK_STREAM, _T("127.0.0.1"));
	if (dataSock.Connect(_T("127.0.0.1"), ServerPort) == FALSE)
	{
		cout << "Khong the ket noi de nhan du lieu!" << endl << endl;
		cout << "Upload khong thanh cong!" << endl << endl;
		dataSock.Close();
		return 0;
	}
	
	//Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	cout << buf << endl;

	//Doc file can upload
	memset(buf, 0, sizeof buf);
	fstream f;
	f.open(h_filename, ios::in);
	f.getline(buf, BUFSIZ);
	f.close();

	if (strlen(buf) == 0)
	{
		cout << "Khong the mo file!" << endl << endl;
		return 0;
	}

	// Gui du lieu file len Server
	dataSock.Send(buf, BUFSIZ, 0);
	cout << "Noi dung file: ";
	cout << buf << endl << endl;
	
	//Dong ket noi
	dataSock.Close();

	//Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	cout << buf << endl;

	cout << "UPLOAD file " << h_filename << " den Server thanh cong!" << endl << endl;

	return 1;
}



// Ham thay doi duong dan hien tai tren Client
// Tham so truyen vao: Ten duong dan moi
// Tra ve 1 neu thanh cong, 0 neu that bai
int changeDirClient(char* h_dirName)
{
	return _chdir(h_dirName) + 1;
}



// Ham in ra duong dan hien tai duoi Client
// Khong co tham so truyen vao
// Khong co gia tri tra ve
void getCurrentDirClient()
{
	char buffer[MAX_PATH + 1];
	_getcwd(buffer, MAX_PATH);
	cout << buffer << endl << endl;
}



// Ham thay doi duong dan hien tai tren Server
// Tham so truyen vao: Control Socket, ten duong dan moi
// Tra ve 1 neu thanh cong, 0 neu that bai
int changeDirServer(CSocket &controlSock, char* h_dirName)
{
	// Gui lenh CWD lenh Server
	char buf[BUFSIZ + 1];
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "CWD %s\r\n", h_dirName);
	controlSock.Send(buf, strlen(buf), 0);

	//Nhan du lieu tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);
	
	if (strstr(buf, "250") == NULL)
	{
		return 0;
	}

	return 1;
}



// Ham lay duong dan hien tai tren Server
// Tham so truyen vao: Control Socket
// Khong co gia tri tra ve
void  getDirectoryServer(CSocket &controlSock)
{
	//Gui lenh PWD
	char buf[BUFSIZ + 1];
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "PWD\r\n");
	controlSock.Send(buf, strlen(buf), 0);

	//Nhan du lieu tra ve
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);
}



// Ham tao thu muc tren Server voi duong dan hien tai
// Tham so truyen vao: Control Socket, ten thu muc moi
// Tra ve 1 neu thanh cong, 0 neu that bai
int makeDirectoryServer(CSocket &controlSock, char* h_filename)
{
	// Gui lenh MKD lenh Server
	char buf[BUFSIZ + 1];
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "MKD %s\r\n", h_filename);
	controlSock.Send(buf, strlen(buf), 0);

	//Nhan du lieu tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	if (strstr(buf, "257") == NULL)
	{
		return 0;
	}

	return 1;
}



// Ham xoa thu muc rong tren Server voi duong dan hien tai
// Tham so truyen vao: Control Socket, Data Socket, ten thu muc can xoa
// Tra ve 1 neu thanh cong, 0 neu that bai
int removeEmptyDirectoryServer(CSocket &controlSock, char* h_dirName)
{
	// Gui lenh RMD len Server
	char buf[BUFSIZ + 1];
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "RMD %s\r\n", h_dirName);
	controlSock.Send(buf, strlen(buf), 0);

	//Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	if (strstr(buf, "250") == NULL)
	{
		return 0;
	}
	return 1;
}



// Ham xoa tat ca cac Folder rong trong duong dan hien tai
// Tham so truyen vao: Control Socket, Data Socket, Data Port
// Tra ve so Folder rong da xoa
int removeAllEmptyDirectoryServer(CSocket &controlSock1, CSocket &dataSock1, unsigned int &h_port1)
{
	
	char buf[BUFSIZ+1];
	strcpy_s(buf, getListActive(controlSock1, dataSock1,h_port1));
	string str = buf;
	string stemp;
	int dem = 0; //Bien dem so luong thu muc rong
	int len = strlen(buf);
	for (int i = 0; i < len; i++)
	{
		if ((buf[i] == '0') && (buf[i - 1] == ' ') && (buf[i + 1] == ' '))
		{
			int t = str.find('\n', i) - 1;
			stemp = str.substr(i + 15, t - (i + 15));
			cout << "Thu muc dang xet: ";
			cout << stemp << endl << endl;
			if (removeEmptyDirectoryServer(controlSock1, (char*)stemp.c_str()) == 1)
			{
				cout << "Da xoa thu muc rong: " << stemp << endl << endl;
				dem++;
			}
			else
			{
				cout << "Thu muc khong rong!" << endl << endl;
			}
		}
	}
	if (dem == 0)
	{
		cout << "Khong tim thay thu muc rong tren duong dan hien tai!" << endl << endl;
	}
	return dem;
}



// Ham xoa file tren Server voi duong dan hien tai
// Tham so truyen vao: Control Socket, ten file can xoa
// Tra ve 1 neu xoa thanh cong, 0 neu that bai
int removeFileServer(CSocket &controlSock, char* h_filename)
{

	// Gui lenh DELE len Server
	char buf[BUFSIZ + 1];
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "DELE %s\r\n", h_filename);
	controlSock.Send(buf, strlen(buf), 0);

	//Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);
	
	if (strstr(buf, "250") == NULL)
	{
		return 0;
	}

	return 1;
}



// Ham dong ket noi toi FTP Server
// Tham so truyen vao: Control Socket
// Tra ve 1 neu thanh cong, 0 neu that bai
int disconnectServer(CSocket &controlSock)
{
	// Gui lenh QUIT len Server
	char buf[BUFSIZ + 1];
	memset(buf, 0, sizeof buf);
	sprintf_s(buf, "QUIT\r\n");
	controlSock.Send(buf, strlen(buf), 0);

	//Nhan lenh tra ve tu Server
	memset(buf, 0, sizeof buf);
	controlSock.Receive(buf, BUFSIZ, 0);
	puts(buf);

	if (strstr(buf, "221") == NULL)
	{
		return 0;
	}
	return 1;
}
