#pragma once
#pragma warning(disable:4819)
#pragma warning(disable:4996)
#include "client.h"
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

class cMain : public wxFrame
{
private:
	wxButton* m_btn1 = nullptr;
	wxButton* m_btn2 = nullptr;
	wxButton* m_send = nullptr;
	wxButton* m_recieve = nullptr;
	wxButton* m_quit = nullptr;
	wxButton* m_restart = nullptr;
	wxTextCtrl* m_txt1 = nullptr;
	wxTextCtrl* m_txt2 = nullptr;
	wxTextCtrl* m_txt3 = nullptr;
	wxTextCtrl* m_txt4 = nullptr;
	wxStaticText* m_stat1 = nullptr;
	wxStaticText* m_stat2 = nullptr;
	wxStaticText* m_stat3 = nullptr;
	wxStaticText* m_stat4 = nullptr;
	wxStaticText* m_stat5 = nullptr;
	wxListBox* m_list1 = nullptr;
	wxTextCtrl* m_textCtrl = nullptr;
	std::string ipadd, str, filename;
	wxPanel* panel;
	bool flag = false;
	client* myClient;
public:
	cMain();
	~cMain();
	void render(wxPaintEvent& evt);
	void OnButtonClickedSend(wxCommandEvent& evt);
	void OnButtonClickedRecieve(wxCommandEvent& evt);
	void chooseFile(wxCommandEvent& evt);
	void sendFile(wxCommandEvent& evt);
	void recieveFile(wxCommandEvent& evt);
	void exitProgram(wxCommandEvent& evt);
	void chooseFolder(wxFileDirPickerEvent& evt);
	void printList(std::string str);
	void restartProgram(wxCommandEvent& evt);
	void mainMenu();

	wxDECLARE_EVENT_TABLE();
};

