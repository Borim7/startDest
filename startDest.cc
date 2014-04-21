#include <windows.h>

#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;

/// target process struct
typedef struct{
	DWORD pid;
	DWORD tid;
	HWND hwnd;
} targetInfo;


// ********** search Window ***************
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
 * Does not work for process with single instance option.
 * \retval 0 no window with matchin pid found.
 */
HWND searchWindow(DWORD pid){
	targetInfo info;
	info.pid = pid;
	info.hwnd = 0;

	EnumWindows(enumWindowCallback, (LPARAM)&info);
	
	return info.hwnd;
}

// ***************** scan window ********************
BOOL CALLBACK scanWindowCallback(HWND hwnd, LPARAM infoVector) {
	DWORD windowPid = 0;
	DWORD windowTid = 0;
	vector<targetInfo>* infoVec = (vector<targetInfo>*) infoVector;
	targetInfo info;
	
	// get pid and tid for current window
	windowTid = GetWindowThreadProcessId(hwnd, &windowPid);
	
	// add new value to vector
	info.pid = windowPid;
	info.tid = windowTid;
	info.hwnd = hwnd;
	infoVec->push_back(info);
	
	// continue iteration
	return TRUE;
}

/**
 * @brief add all currently existing windows to the info list.
 * @param infos the window info list.
 */
void fillCurrentWindows(vector<targetInfo>* infos){
	// walk over all existing windows
	EnumWindows(scanWindowCallback, (LPARAM)infos);
}


// ************ find new window *****************

typedef struct {
	vector<targetInfo>* knownWindows;
	HWND newWindow;
} NewWindowInfo;


BOOL CALLBACK newWindowCallback(HWND hwnd, LPARAM newInfoVector) {
	DWORD windowPid = 0;
	DWORD windowTid = 0;
	NewWindowInfo* newInfo = (NewWindowInfo*) newInfoVector;
	
	// get pid and tid for current window
	windowTid = GetWindowThreadProcessId(hwnd, &windowPid);
	
	// check if windows is already known
	for(targetInfo info : *(newInfo->knownWindows) ) {
		if( (info.pid == windowPid) && (info.tid == windowTid) ) {
			// window already known
			continue;
		}
		
		// else new window
		newInfo->newWindow = hwnd;
		// stop search
		return FALSE;
	}
	
	// continue search
	return TRUE;
}


/**
 * @brief Find the new window that is not in the window list.
 */
HWND findNewWindow(vector<targetInfo>* infos) {
	NewWindowInfo newInfo;
	newInfo.knownWindows = infos;
	newInfo.newWindow = 0;
	
	// walk over all existing windows
	EnumWindows(scanWindowCallback, (LPARAM)&newInfo);
	
	return newInfo.newWindow;
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
	
	vector<targetInfo> windowList;
	
	
	if(argc == 1) {
		cout << "Usage: " << argv[0] << " <options> <gui program>\n"
			<< "-x <num>  x coordinate of the program window\n"
			<< "-y <num>  y coordinate of the program window\n"
			<< "-t <sec>  how many seconds to wait for the program to start\n"
			<< "-v        print debug information" << endl;
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
			cout << "Invalid option: " << (char)opt << endl;
			exit(-1);
		}
	}
	
	// fetch gui program
	if(optind < argc) {
		char** fileName = {NULL};
		
		// search PATH for the program
		SearchPath(NULL, argv[optind], NULL, sizeof(programPath), programPath, fileName);
		
		if(verbose){
			cout << "start " << programPath << endl;
		}
	} else {
		cout << "No program to start given" << endl;
		exit(-1);
	}
	
	// scan all current windows and remember them
	fillCurrentWindows(&windowList);
	cout << "Found " << windowList.size() << " window threads" << endl;
	
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
		cout << "Can not start program" << endl;
		exit(-1);
	}
	
	// search for window handle
	HWND hwnd = 0;
	int i = 0;
	for(i = 0; i < timeout; i++){
		// simple search after process id does not work for single instance programs
		//hwnd = searchWindow(processInfo.dwProcessId);
		hwnd = findNewWindow(&windowList);
		// check if window was found
		if(hwnd != 0){
			// success
			moveit(hwnd, xCoord, yCoord);
			break;
		}
		
		Sleep(1000);
	}
	
	if(verbose) {
		cout << "search runs: " << i << ", found: " << (hwnd != 0) << endl;
	}
	
	return 0;
}