#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// ********** search HWND ***************

/// target process struct
typedef struct{
	DWORD pid;
	HWND hwnd;
} targetInfo;

/**
 * \brief Callback for searching through windows.
 * \param target pointer to target process struct.
 * \retval FALSE stop enumeration
 */
BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM target) {
	DWORD windowPid = 0;
	targetInfo* info = (targetInfo*) target;
	
	// get pid for current window
	GetWindowThreadProcessId(hwnd, &windowPid);
	
	//printf("check, window pid: %u, search for: %u\n", (unsigned int)windowPid, (unsigned int)info->pid);
	
	if(windowPid == info->pid){
		// found desired window
		info->hwnd = hwnd;
		return FALSE;
	}
	
	// continue search
	return TRUE;
}

/**
 * \brief determine the window handle for the passed process id.
 * \retval 0 no window with matchin pid found.
 */
HWND searchWindow(DWORD pid){
	targetInfo info;
	info.pid = pid;
	info.hwnd = 0;

	EnumWindows(enumWindowCallback, (LPARAM)&info);
	
	return info.hwnd;
}

// *********** move window *********************
void moveit(HWND hwnd, int x, int y) {
	RECT rect;
	
	// get current position
	GetWindowRect(hwnd, &rect);
	
	// replace invalid positions
	if(x < 0) {
		x = rect.left;
	}
	if(y < 0) {
		y = rect.top;
	}
	
	// move to destination
	SetWindowPos(hwnd, HWND_TOP, x, y, 0,0, SWP_NOSIZE);
}


// ************ main ***********************

int main(int argc, char** argv) {
	char programPath[4096];
	int xCoord = -1;
	int yCoord = -1;
	int verbose = 0;
	int timeout = 5;
	int opt;
	
	if(argc == 1) {
		printf("Usage: %s <options> <gui program>\n", argv[0]);
		printf("-x <num>  x coordinate of the program window\n");
		printf("-y <num>  y coordinate of the program window\n");
		printf("-t <sec>  how many seconds to wait for the program to start\n");
		printf("-v        print debug information\n");
		exit(-1);
	}
	
	// parse command line
	while((opt = getopt (argc, argv, "vx:y:t:")) != -1) {
		switch(opt){
		case 'x':
			xCoord = atoi(optarg);
			break;
			
		case 'y':
			yCoord = atoi(optarg);
			break;
		
		case 't':
			timeout = atoi(optarg);
			break;
		
		case 'v':
			verbose = 1;
			break;
		
		default:
			printf("Invalid option: %c\n", opt);
			exit(-1);
		}
	}
	
	// fetch gui program
	if(optind < argc) {
		char** fileName = {NULL};
		
		// search PATH for the program
		SearchPath(NULL, argv[optind], NULL, sizeof(programPath), programPath, fileName);
		
		if(verbose){
			printf("start %s\n", programPath);
		}
	} else {
		printf("No program to start given\n");
		exit(-1);
	}
	
	// prepare info variables
	STARTUPINFO startInfo;
	PROCESS_INFORMATION processInfo;
	ZeroMemory( &startInfo, sizeof(startInfo) );
    startInfo.cb = sizeof(startInfo);
    ZeroMemory( &processInfo, sizeof(processInfo) );
	
	// start the program
	BOOL success = CreateProcess(programPath, NULL,
		NULL, NULL, // security attributes
		FALSE, 0, // no handle inherited, use standard priority
		NULL, NULL, // no specific environment and work dir
		&startInfo, &processInfo);
	
	if(!success){
		printf("Can not start program\n");
		exit(-1);
	}
	
	// search for window handle
	HWND hwnd = 0;
	int i = 0;
	for(i = 0; i < timeout; i++){
		hwnd = searchWindow(processInfo.dwProcessId);
		// check if window was found
		if(hwnd != 0){
			// success
			moveit(hwnd, xCoord, yCoord);
			break;
		}
		
		Sleep(1000);
	}
	
	if(verbose) {
		printf("search runs: %i, found: %i\n", i, hwnd != 0);
	}
	
	return 0;
}