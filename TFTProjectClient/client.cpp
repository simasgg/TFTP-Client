#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4819)
#include "client.h"
#define socklen_t int
using namespace std;

#define PORT 69
#define PACKET_SIZE 516

#define OP_ACK 4
#define OP_RRQ 1
#define OP_WRQ 2
#define OP_DATAPACKET 3
#define OP_ERROR 5


client::client()
{
	mySocket = -1;
}

client::~client()
{
	if(mySocket !=-1)
		closesocket(mySocket);
}

void client::Init(wxListBox* l) {
	m_list1 = l;
	WSADATA WinSockData;
	int iWsaStartup;
	iWsaStartup = WSAStartup(MAKEWORD(2, 2), &WinSockData);
	if (iWsaStartup != 0) {
		cout << "WSAStartUp Failed" << endl;
	}
}

void client::EndProgram() {
	int iWsaCleanup;
	iWsaCleanup = WSACleanup();
	if (iWsaCleanup == SOCKET_ERROR) {
		cout << "CleanUp Fun Failed & Error No-> " << WSAGetLastError() << endl;
	}
}

void client::printList(string str) {
	m_list1->AppendString(str);
	m_list1->SetSelection(m_list1->GetCount() - 1);
	wxYield();
}

void client::error_packet(char* buf) {
	int i = 4;
	printList("<<<Error>>> number: " + to_string(+buf[3]));
	while (buf[i] != 0) {
		cout << buf[i];
		i++;
	}

	//int index = buf[3] - '0';
	switch (stoi(to_string(+buf[3]))){
		case 0:
			printList("Could not read file");
			break;
		case 1: 
			printList("Could not open file");
			break;
		case 3:
			printList("Disk full or allocation exceeded");
			break;
		case 4:
			printList("Illegal TFTP operation");
			break;
		default:
			printList("Unknown problem occured");
	}
	printList("Server was forcibly closed");
	wxMilliSleep(3000);
}
//start: ipadr >>> send/recv >>> port >>> file
void client::start(string host, string status, wxTextCtrl* m_txt3, string s) {
	struct sockaddr_in server;
	struct hostent* by_name;
	str = s;
	//port
	wxString valueport = m_txt3->GetValue();
	string myport = string(valueport.mb_str(wxConvUTF8));

	//convert string to char array, ipaddress
	char* cstr = new char[host.size() + 1];
	strcpy(cstr, host.c_str());

	//filepath to char
	char* userInput = new char[str.size() + 1];
	strcpy(userInput, str.c_str());

	memset(&server, 0, sizeof(server));
	by_name = gethostbyname(cstr);
	server.sin_family = AF_INET;
	if (myport.empty())
		server.sin_port = htons(PORT);
	else
		server.sin_port = htons(stoi(myport));	//convert string to int
	memcpy(&server.sin_addr.S_un.S_addr, by_name->h_addr_list[0], 4);

	mySocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (mySocket == -1) {
		fprintf(stderr, "socket: errno = %i\n", errno);
	}

	//calling function
	if (status == "recieve")
		my_function_to_recieve(mySocket, userInput, &server, "octet");
	else if (status == "send")
		my_function_to_send(mySocket, userInput, &server, "octet");
	else
		printList("Error, enter one more time");
	closesocket(mySocket);
	m_btn2->Enable(true);

	delete[]cstr;
	delete[]userInput;
}

void client::setButton(wxButton* b) {
	m_btn2 = b;
}

void client::setFilename(string fn) {
	filename = fn;
}

void client::my_function_to_recieve(int mySocket, const char* userInput, const struct sockaddr_in* name, const char* mode) {
	double t1, t2;
	struct sockaddr_in inc_name;
	FILE* pFile = NULL;
	char  buf[PACKET_SIZE];
	int pos = 0, block = 0, byte_count = 0;

	//name of file to char
	char* filenamechar = new char[filename.size() + 1];
	strcpy(filenamechar, filename.c_str());

	//2 bytes - Opcode; string - filename; 1 byte - 0; string - mode; 1 byte
	int by_name_length = 2 + strlen(userInput) + 1 + strlen(mode) + 1;
	buf[pos] = 0;
	pos++;
	buf[pos] = OP_RRQ;
	pos++;
	for (size_t  i = 0; i < strlen(userInput); i++) {
		buf[pos] = userInput[i];
		pos++;
	}
	buf[pos] = 0;
	pos++;

	for (size_t  i = 0; i < strlen(mode); i++) {
		buf[pos] = mode[i];
		pos++;
	}
	buf[pos] = 0;

	//---------------RRQ (Read Request)
	int bytes_sent = sendto(mySocket, buf, by_name_length, 0, (const struct sockaddr*)name, sizeof(struct sockaddr_in));
	if (bytes_sent == -1) {
		fprintf(stderr, "sendto: errno = %i\n", errno);
		printList("sendto: errno = " + errno);
	}

	t1 = clock();
	while (true) {
		int size = sizeof(struct sockaddr_in);
		//---------------DATA block (512 Bytes)
		int bytes_recv = recvfrom(mySocket, buf, PACKET_SIZE, 0, (struct sockaddr*)&inc_name, &size);
		if (bytes_recv == -1) {
			fprintf(stderr, "recvfrom: errno = %i\n", errno);
			printList("recvfrom: errno = " + errno);
		}

		if (buf[1] == OP_ERROR) {
			error_packet(buf);
			closesocket(mySocket);
			return;
		}
		if (!pFile) {
			if ((pFile = fopen(filenamechar, "wb")) == NULL)
			{
				printList("Could not open file");
				break;
			}
		}

		//1 block = 512 bytes
		int block_number = ((unsigned short)buf[2] << 8) | (unsigned short)buf[3];
		if ((bytes_recv - 4 > 0) && (block_number > block)) {
			byte_count += bytes_recv - 4;
			block++;
			fwrite(&buf[4], 1, bytes_recv - 4, pFile);
			if (ferror(pFile)) {
				printf("Error writing data\n");
				printList("Error writing data");
				break;
			}
		}

		buf[0] = 0;
		buf[1] = OP_ACK;
		//---------------ACK block
		int send_count = sendto(mySocket, buf, 4, 0, (const struct sockaddr*)&inc_name, sizeof(struct sockaddr_in));
		if (send_count == -1) {
			fprintf(stderr, "sendto: errno = %i\n", errno);
			printList("sendto: errno = " + errno);
		}

		printList(to_string(byte_count) + " bytes (" + to_string(block) + " blocks) received");
		fflush(stdout);

		if (bytes_recv - 4 != 512) {
			break;
		}
	}
	t2 = clock();
	printList("***Transfered " + to_string(byte_count) + " bytes in " + to_string((t2 - t1) / CLK_TCK) + " sec***");
	if (byte_count != 0)
		printList(str + " was successfully downloaded from the server");
	else
		printList(str + " failed to download");
	fclose(pFile);
	delete[]filenamechar;
}

void client::my_function_to_send(int mySocket, const char* userInput, const struct sockaddr_in* name, const char* mode) {
	double t1, t2;
	struct sockaddr_in inc_name;
	FILE* pFile = NULL;
	FILE* tempFile = NULL;
	char  buf[PACKET_SIZE];
	bool flag = false;
	int pos = 0, block = 0, byte_count = 0, my_size = 0;
	unsigned char i = 0, j = 0;

	string test(userInput);
	//convert path to filename
	filesystem::path my_path(test);
	string path_string = my_path.filename().string();
	printList(path_string);

	char* file_name = new char[path_string.size() + 1];
	strcpy(file_name, path_string.c_str());

	//2 bytes - Opcode; string - filename; 1 byte - 0; string - mode; 1 byte
	int by_name_length = 2 + strlen(file_name) + 1 + strlen(mode) + 1;
	buf[pos] = 0;
	pos++;
	buf[pos] = OP_WRQ;
	pos++;
	for (size_t  i = 0; i < strlen(file_name); i++) {
		buf[pos] = file_name[i];
		pos++;
	}
	buf[pos] = 0;
	pos++;

	for (size_t  i = 0; i < strlen(mode); i++) {
		buf[pos] = mode[i];
		pos++;
	}
	buf[pos] = 0;

	//---------------WRQ (Read Request)
	int bytes_sent = sendto(mySocket, buf, by_name_length, 0, (const struct sockaddr*)name, sizeof(struct sockaddr_in));
	if (bytes_sent == -1) {
		fprintf(stderr, "sendtoWRQ: errno = %i\n", errno);
		printList("sendtoWRQ: errno = " + errno);
	}
	int ss = sizeof(struct sockaddr_in);

	//--------------ACK block
	int bytes_recv = recvfrom(mySocket, buf, 4, 0, (struct sockaddr*)&inc_name, &ss);
	if (bytes_recv == -1) {
		fprintf(stderr, "recvfromACK: errno = %i\n", errno);
		printList("recvfromACK: errno = " + errno);
		return;
	}

	t1 = clock();
	while (true) {
		if (!pFile) {
			if ((pFile = fopen(userInput, "rb")) == NULL)
			{
				fprintf(stderr, "Could not open file\n");
				printList("Could not open file");
				break;
			}
		}
		if (buf[1] == OP_ERROR) {
			error_packet(buf);
			closesocket(mySocket);
			return;
		}
		if (flag == false) {
			fseek(pFile, 0, SEEK_END);
			//susiskaiciuoju failo dydi baitais
			my_size = ftell(pFile);
			flag = true;
			fseek(pFile, 0, SEEK_SET);
		}
		//1 block = 512 bytes
		int block_number = ((unsigned short)buf[2] << 8) | (unsigned short)buf[3];
		int to_send = PACKET_SIZE;

		if ((to_send - 4 > 0)) {
			if (my_size < 516)
				to_send = my_size + 4;
			my_size = my_size - 512;
			byte_count += to_send - 4;
			block++;
			fread(&buf[4], 1, to_send - 4, pFile);
			if (ferror(pFile)) {
				printf("Error writing data\n");
				printList("Error writing data");
				break;
			}
		}
		//Opcode 3=DATA
		buf[0] = 0;
		buf[1] = OP_DATAPACKET;
		//skaiciuoju block number kuri paskui siunciu kartu su DATA packetu
		i++;
		buf[2] = (unsigned short)j;
		buf[3] = (unsigned short)i;
		if (buf[3] == -1) {
			j++;
		}

		//DATA block (512 Bytes)
		int send_count = sendto(mySocket, buf, to_send, 0, (const struct sockaddr*)&inc_name, sizeof(struct sockaddr_in));
		if (send_count == -1) {
			fprintf(stderr, "sendtoDATA: errno = %i\n", errno);
			printList("sendtoDATA: errno = " + errno);
			break;
		}

		//--------------ACK block
		bytes_recv = recvfrom(mySocket, buf, 4, 0, (struct sockaddr*)&inc_name, &ss);
		if (bytes_recv == -1) {
			fprintf(stderr, "recvfromACK: errno = %i\n", errno);
			printList("recvfromACK: errno = " + errno);
			cout << (unsigned short)buf[3] << endl;
			break;
		}
		printList(to_string(byte_count) + " bytes (" + to_string(block) + " blocks) sent");
		if (send_count - 4 != 512) {
			break;
		}

		if (buf[1] == OP_ERROR) {
			error_packet(buf);
			closesocket(mySocket);
			return;
		}
	}
	t2 = clock();
	printList("***Transfered " + to_string(byte_count) + " bytes in " + to_string((t2 - t1) / CLK_TCK) + " sec***");
	if (byte_count != 0)
		printList(path_string + " was successfully uploaded to the server");
	else
		printList(path_string + " failed to upload");
	delete[]file_name;
	fclose(pFile);
}