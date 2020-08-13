#pragma once
#define socklen_t int
#include <iostream>
#include <filesystem>
#include <string>
#include <sstream>
#include <string.h>
#include <thread>
#include <ctime>
#include "wx/wx.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"
#include <wx/wfstream.h>
#include "wx/glcanvas.h"
#include <wx/filepicker.h>


class client
{
private:
	wxListBox* m_list1 = nullptr;
	wxButton* m_btn2 = nullptr;
	int mySocket;
	std::string str, filename;
public:
	client();
	~client();
	void Init(wxListBox* l);
	void EndProgram();
	void error_packet(char* buf);
	void start(std::string host, std::string status, wxTextCtrl* m_txt3, std::string s);
	void my_function_to_recieve(int mySocket, const char* userInput, const struct sockaddr_in* name, const char* mode);
	void my_function_to_send(int mySocket, const char* userInput, const struct sockaddr_in* name, const char* mode);
	void printList(std::string str);
	void setFilename(std::string fn);
	void setButton(wxButton* b);
};

