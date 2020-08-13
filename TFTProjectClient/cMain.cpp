#pragma warning(disable:4819)
#include "cMain.h"

using namespace std;


wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_BUTTON(10001, OnButtonClickedSend)
	EVT_BUTTON(10002, OnButtonClickedRecieve)
	EVT_BUTTON(10003, chooseFile)
	EVT_BUTTON(10004, sendFile)
	EVT_BUTTON(10005, recieveFile)
	EVT_BUTTON(10006, exitProgram)
	EVT_BUTTON(10008, restartProgram)
	EVT_PAINT(cMain::render)
	EVT_DIRPICKER_CHANGED(10007, chooseFolder)
wxEND_EVENT_TABLE()


cMain::cMain() : wxFrame(nullptr, wxID_ANY, "TFTPClient", wxPoint(500, 300), wxSize(850, 600))
{
	SetBackgroundColour(*wxLIGHT_GREY);
	wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT, false);

	//ipaddress
	m_stat3 = new wxStaticText(this, wxID_ANY, "IP Address: ", wxPoint(50, 100));
	m_txt2 = new wxTextCtrl(this, wxID_ANY, "127.0.0.1", wxPoint(140, 100), wxSize(100, 20));
	m_stat3->SetFont(font);
	m_txt2->SetBackgroundColour(*wxLIGHT_GREY);
	m_txt2->SetStyle(0, -1, m_txt2->GetDefaultStyle());

	//port
	m_stat4 = new wxStaticText(this, wxID_ANY, "Port: ", wxPoint(50, 130));
	m_txt3 = new wxTextCtrl(this, wxID_ANY, "", wxPoint(140, 130), wxSize(50, 20));
	m_stat4->SetFont(font);
	m_txt3->SetBackgroundColour(*wxLIGHT_GREY);
	m_txt3->SetStyle(0, -1, m_txt3->GetDefaultStyle());

	//send
	m_send = new wxButton(this, 10001, "Send", wxPoint(105, 250), wxSize(90, 27));

	//recieve
	m_recieve = new wxButton(this, 10002, "Recieve", wxPoint(105, 400), wxSize(90, 27));

	//choose directory
	wxDirPickerCtrl* dirPickerCtrl = new wxDirPickerCtrl(this, 10007,
		"Default Folder", wxDirSelectorPromptStr,
		wxPoint(30, 500), wxSize(90, 30), wxDIRP_CHANGE_DIR);

	m_stat5 = new wxStaticText(this, wxID_ANY, "Default folder", wxPoint(30, 530), wxSize(90, 27));

	//quit
	m_quit = new wxButton(this, 10006, "Quit", wxPoint(700, 500), wxSize(90, 27));

	//restart
	m_restart = new wxButton(this, 10008, "Restart", wxPoint(600, 500), wxSize(90, 27));
	m_restart->Enable(false);

	//screen + info
	mainMenu();

	//Main TFTP code
	myClient = new client();
}

cMain::~cMain()
{
	delete myClient;
}

void cMain::render(wxPaintEvent& evt)
{
	wxPaintDC dc(this);
	dc.SetBrush(*wxLIGHT_GREY_BRUSH);
	//ipadd and port
	dc.DrawRoundedRectangle(30, 80, 250, 100, 10);
	dc.SetBackgroundMode(wxSOLID);
	dc.DrawText(wxT("Server"), 130, 70);
	//send
	dc.DrawRoundedRectangle(30, 230, 250, 100, 10);
	dc.SetBackgroundMode(wxSOLID);
	dc.DrawText(wxT("Upload"), 130, 220);
	//recieve
	dc.DrawRoundedRectangle(30, 380, 250, 100, 10);
	dc.SetBackgroundMode(wxSOLID);
	dc.DrawText(wxT("Download"), 125, 370); 
}

void cMain::chooseFolder(wxFileDirPickerEvent& evt)
{
	flag = true;
	if (m_stat5)
	{
		m_stat5->SetLabel(evt.GetPath());
		wxString value = evt.GetPath();
		filename = string(value.mb_str(wxConvUTF8));
	}
}


void cMain::chooseFile(wxCommandEvent& evt) {
	wxFileDialog openFileDialog(this, _("Open file"), "", "", "visi failai (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;
	wxFileInputStream input_stream(openFileDialog.GetPath());
	if (!input_stream.IsOk())
	{
		wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
		return;
	}
	wxString value = openFileDialog.GetPath();
	str = string(value.mb_str(wxConvUTF8));
	printList("Current file: " + str);
	m_btn2 = new wxButton(this, 10004, "Send", wxPoint(600, 80), wxSize(60, 30));
	evt.Skip();
}


void cMain::exitProgram(wxCommandEvent& evt) {
	Destroy();
}

void cMain::restartProgram(wxCommandEvent& evt) {
	//grazinam viska i pradine faze
	m_send->Enable(true);
	m_recieve->Enable(true);
	m_txt2->Enable(true);
	m_txt3->Enable(true);
	m_list1->Destroy();
	if (m_btn1 != nullptr) {
		m_btn1->Destroy();
		m_btn1 = nullptr;
	}
	if (m_txt4 != nullptr) {
		m_txt4->Destroy();
		m_txt4 = nullptr;
	}
	m_stat1->Destroy();
	m_btn2->Destroy();

	mainMenu();

	delete myClient;
	myClient = new client();
	m_restart->Enable(false);
}

void cMain::mainMenu() {
	wxFont font2(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_SLANT, wxFONTWEIGHT_BOLD, false, wxEmptyString, wxFONTENCODING_DEFAULT);
	m_stat2 = new wxStaticText(this, wxID_ANY, "TFTP Client", wxPoint(420, 100));
	m_stat2->SetFont(font2);
	wxFont font3(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_LIGHT, false);
	m_stat1 = new wxStaticText(this, wxID_ANY, "Steps:\n"
		">Enter server IP address\n"
		">Enter port number (optional)\n"
		">Press Send to upload file to the server\n"
		">Press Recieve to download file from the server\n\n"
		"Optional:\n"
		">Press Browse to choose location for downloaded files", wxPoint(370, 200));
	m_stat1->SetFont(font3);
}

void cMain::sendFile(wxCommandEvent& evt) {
	m_btn1->Enable(false);
	m_btn2->Enable(false);
	myClient->Init(m_list1);
	myClient->setFilename(filename);
	myClient->setButton(m_btn2);
	myClient->start(ipadd, "send", m_txt3, str);
	myClient->EndProgram();
	m_restart->Enable(true);
	evt.Skip();
}

void cMain::recieveFile(wxCommandEvent& evt) {
	m_btn2->Enable(false);
	wxString value = m_txt4->GetValue();
	str = string(value.mb_str(wxConvUTF8));

	//siusiu faila i pasirinkta folderi
	//jei nebuvo nurodytas folderis tai siunciame vietoje, kitu atveju siunciame nurodytu pathu
	if (flag == true) {
		filename = filename + "\\" + str;
		std::replace(str.begin(), str.end(), '\\', '/');
	}else
		filename = str;
	myClient->Init(m_list1);
	myClient->setFilename(filename);
	myClient->setButton(m_btn2);
	myClient->start(ipadd, "recieve", m_txt3, str);
	myClient->EndProgram();
	m_restart->Enable(true);
	evt.Skip();
}

void cMain::OnButtonClickedSend(wxCommandEvent& evt)
{
	//ipadd
	wxString value = m_txt2->GetValue();
	ipadd = string(value.mb_str(wxConvUTF8));

	m_stat2->Destroy();
	m_stat1->Destroy();
	m_send->Enable(false);
	m_recieve->Enable(false);
	m_txt2->Enable(false);
	m_txt3->Enable(false);

	wxFont font3(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT, false);
	m_stat1 = new wxStaticText(this, wxID_ANY, "Upload file: ", wxPoint(370, 85));
	m_btn1 = new wxButton(this, 10003, "Browse", wxPoint(500, 80), wxSize(60, 30));
	m_list1 = new wxListBox(this, wxID_ANY, wxPoint(350, 150), wxSize(400, 300));
	m_list1->SetBackgroundColour(*wxLIGHT_GREY);
	m_stat1->SetFont(font3);
	printList("***Client is ready***");
	evt.Skip();
}

void cMain::OnButtonClickedRecieve(wxCommandEvent& evt)
{
	wxString value = m_txt2->GetValue();
	ipadd = string(value.mb_str(wxConvUTF8));

	m_stat2->Destroy();
	m_stat1->Destroy();
	m_send->Enable(false);
	m_recieve->Enable(false);
	m_txt2->Enable(false);
	m_txt3->Enable(false);

	wxFont font3(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT, false);
	m_stat1 = new wxStaticText(this, wxID_ANY, "Enter file name: ", wxPoint(350, 85));
	m_txt4 = new wxTextCtrl(this, wxID_ANY, "", wxPoint(470, 80), wxSize(120, 25));
	m_btn2 = new wxButton(this, 10005, "Recieve", wxPoint(620, 80), wxSize(60, 25));

	m_list1 = new wxListBox(this, wxID_ANY, wxPoint(350, 150), wxSize(400, 300));
	m_list1->SetBackgroundColour(*wxLIGHT_GREY);
	m_stat1->SetFont(font3);
	printList("***Client is ready***");
	evt.Skip();
}


void cMain::printList(string str) {
	m_list1->AppendString(str);
	m_list1->SetSelection(m_list1->GetCount() - 1);
	wxYield();
}