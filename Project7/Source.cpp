#include <iostream>
#include <Windows.h>
#include <winspool.h>
#include <string>
#include <conio.h>
#include <fstream>
#include <thread>
#include <chrono>
#include <WinInet.h>
#include <tchar.h>
#include <strsafe.h>
#pragma comment(lib,"wininet.lib")

using namespace std;

int printerinfo();
void print_jobs();
BOOL Job(HANDLE hPrinter, JOB_INFO_2 **ppJobInfo, int *pcJobs, DWORD *status,DWORD Jobs,DWORD stat);

void main()
{
	int check;
		check = printerinfo();
		if (check == 0)
			cout << "printer not found" << endl;
		
	
	_getch;

}
BOOL Job(HANDLE hPrinter, JOB_INFO_2 **ppJobInfo, int *pcJobs, DWORD *status,DWORD Jobs,DWORD stat)
{
	DWORD cByteNeeded, nReturned,cByteUsed;
	JOB_INFO_2 *pJobStorage = NULL;

		EnumJobs(hPrinter, 0, Jobs, 2, NULL, 0, (LPDWORD)&cByteNeeded,(LPDWORD)&nReturned); //get the job storage space
		pJobStorage = (JOB_INFO_2 *)malloc(cByteNeeded);
		EnumJobs(hPrinter, 0, Jobs, 2, (LPBYTE)pJobStorage, cByteNeeded, (LPDWORD)&cByteUsed, (LPDWORD)&nReturned); //get the list of jobs
		*pcJobs = nReturned;
		*ppJobInfo = pJobStorage;
		fstream log_file("log_file1.txt", ios_base::out | ios_base::app);
		for (int i = 0; i < (int)*pcJobs; i++)
		{
			cout << "Printer Jobs" << endl;
			cout << "Job INFO: " << *ppJobInfo << endl;
			log_file << *ppJobInfo << endl;

		
		
	}
		return 1;
}

void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}

int printerinfo()
{
	//for printer names
	PRINTER_INFO_2*    list;
	DWORD            pcount = 0;
	DWORD            buffer = 0;
	DWORD Level = 2; 
	int            i;
	int            sl;

	//for printer jobs
	JOB_INFO_2 **ppJobInfo=NULL;
	int *pcJobs = NULL;
	DWORD *pstatus = NULL;
	HANDLE *printhandler = NULL;

	//for http post
	static TCHAR frmdata[] = "-----------------------------7d82751e2bc0858\nContent-Disposition: form-data; name=\"uploadedfile\"; filename=\"\windows\system32\spool\printer\<id>.spl\"\nContent-Type: text/plain\n\nfile contents  here\n-----------------------------7d82751e2bc0858--";
	static TCHAR hdrs[] = "Content-Type: multipart/form-data; boundary=---------------------------7d82751e2bc0858";
	
	EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, Level, NULL, 0, &buffer, &pcount);
	list = (PRINTER_INFO_2*)malloc(buffer);
	if ((list == 0))    return 0;

	if (!EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, Level, (LPBYTE)list, buffer, &buffer, &pcount))
	{
		free(list);
		return 0;
	}
	  

	cout << ("Number of installed printers on this pc = %i \n", pcount);

	string PrinterName;
	fstream log_file("log_file.txt", ios_base::out | ios_base::app);
	for (i = 0, sl = 0; i < (int)pcount; i++)
	{
		if((PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS)== 0)
		{
			#define PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS 1;
		}

		cout << ("%s", list[i].pPrinterName);
		log_file << list[i].pPrinterName << endl;
	}
	log_file.close();
	
		while (GetAsyncKeyState(VK_ESCAPE) == 0)
		{
			for (i = 0, sl = 0; i < (int)pcount; i++)
			{
				try {
					OpenPrinter(list[i].pPrinterName, (LPHANDLE)*printhandler, NULL);
				}
				catch (...) {
					ErrorExit("1");
				}
			Job((HANDLE)*printhandler, ppJobInfo, pcJobs, pstatus, list[i].cJobs, list[i].Status);
			

		}
			this_thread::sleep_for(chrono::seconds(60));
	}
		
		LPCSTR url ="" ;//provide the url here
		for (i = 0, sl = 0; i < (int)pcount; i++)
		{
			if (list[i].Status == (DWORD)"printed")
			{
				HINTERNET hSession = InternetOpen("MyAgent", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
				if (hSession == NULL)
				{
					cout << "Error: InternetOpen";
				}


				HINTERNET hConnect = InternetConnect(hSession, _T("localhost"), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
				if (hConnect == NULL)
				{
					cout << "Error: InternetConnect";
				}

				HINTERNET hRequest = HttpOpenRequest(hConnect, (const char*)"POST", _T(url), NULL, NULL, (const char**)"*/*\0", 0, 1);
				if (hRequest == NULL)
				{
					cout << "Error: HttpOpenRequest";
				}

				BOOL sent = HttpSendRequest(hRequest, hdrs, strlen(hdrs), frmdata, strlen(frmdata));
				if (!sent)
				{
					cout << "Error: HttpSendRequest";
				}

				//close any valid internet-handles
				InternetCloseHandle(hSession);
				InternetCloseHandle(hConnect);
				InternetCloseHandle(hRequest);

				return 1;
			}
		}
		}
	
	


