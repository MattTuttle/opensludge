#ifdef WIN32
#include <windows.h>

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

#include "Compiler.hpp"
#include "WINTERFA.H"
#include "MOREIO.H"
#include "MessBox.h"
#include "wincomp.h"
#include "Project.hpp"
#include "settings.h"
#include "SPLITTER.HPP"
#include "Interface.h"
#include "launch.h"

enum parseMode {PM_NORMAL, PM_QUOTE, PM_COMMENT, PM_FILENAME};

bool keepImages, keepStrings, verboseMode;


#include "dialogueBoxInOut.h"

char *fileList[1000];
int fileListNum=0;

char *loadedFile = NULL;

HWND mainWin=NULL, compWin = NULL;
HINSTANCE inst;
HMENU myMenu;

char * loader;


char * searchString = NULL;

/* Messaging macros */
#define MESS(id,m,w,l) SendDlgItemMessage(mainWin,id,m,(WPARAM)w,(LPARAM)l)
#define MESSP(id,m,w,l) SendDlgItemMessage(hDlg,id,m,(WPARAM)w,(LPARAM)l)

void setChanged (bool newVal);

void fixExtension (char * buff, char * ext);
BOOL APIENTRY dialogComp(HWND h, UINT m, WPARAM w, LPARAM l);
void updateTitle ();
void setLoadedFile (char * t);

/*
static bool advancedLoadSaveSetup (HWND hDlg, dlgOperation operation)
{
	return (dialogueBoxTransferValue_Checkbox (hDlg, ID_STARTUP_INVISIBLE, 			& settings.startupInvisible,   operation) &&
			dialogueBoxTransferValue_Checkbox (hDlg, ID_STARTUP_DISPLAY_SLUDGE, 	& settings.startupShowLogo,    operation) &&
			dialogueBoxTransferValue_Checkbox (hDlg, ID_STARTUP_DISPLAY_LOADING,	& settings.startupShowLoading, operation));
}

LRESULT CALLBACK projectAdvancedFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	dialogueBoxHandler (hDlg, message, wParam, lParam, advancedLoadSaveSetup);
}
*/
LRESULT CALLBACK prefsBoxFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
//	static OPENFILENAME ofn;
//	static char path[MAX_PATH];
	switch (message) {
        case WM_INITDIALOG:
        {
        	SetDlgItemText (hDlg, ID_EDIT_NAME, settings.windowName);
        	SetDlgItemText (hDlg, ID_EDIT_OUTPUTFILE, settings.finalFile);

        	SetDlgItemInt (hDlg, ID_EDIT_WINWIDTH, settings.screenWidth, 0);
        	SetDlgItemInt (hDlg, ID_EDIT_WINHEIGHT, settings.screenHeight, 0);
        	SetDlgItemInt (hDlg, ID_EDIT_SPEED, settings.frameSpeed, 0);
        	SetDlgItemText (hDlg, ID_EDIT_QUITMESSAGE, settings.quitMessage);
        	SetDlgItemText (hDlg, ID_EDIT_CUSTOMICON, settings.customIcon);
        	SetDlgItemText (hDlg, ID_EDIT_CUSTOMLOGO, settings.customLogo);
        	SetDlgItemText (hDlg, ID_EDIT_DATAFOLDER, settings.runtimeDataFolder);
        	SetDlgItemText (hDlg, ID_EDIT_LANGUAGE, settings.originalLanguage);


        	if (settings.runFullScreen) CheckDlgButton (hDlg, ID_FULLSCREEN, BST_CHECKED);
        	if (settings.forceSilent) CheckDlgButton (hDlg, ID_MAKESILENT, BST_CHECKED);
        	if (settings.ditherImages) CheckDlgButton (hDlg, ID_DITHERIMAGES, BST_CHECKED);
			return (true);
		}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_EDIT_SPEED:
				if (HIWORD(wParam) == 1024) {
					bool success = true;
					unsigned int newFrame = GetDlgItemInt (hDlg, ID_EDIT_SPEED, (BOOL *) &success, 0);

					if (! success || newFrame > 1000 || newFrame < 1) {
			        	SetDlgItemText (hDlg, ID_EDIT_SPEED2, "???");
					} else {
			        	SetDlgItemInt (hDlg, ID_EDIT_SPEED2, 1000/newFrame, 0);
			        }
				}
				break;

				case IDOK:
				{
					char * newText;
					int le;

					bool success = true;

					unsigned int newHeight, newWidth = GetDlgItemInt (hDlg, ID_EDIT_WINWIDTH, (BOOL *) & success, 0);
					if (success) {
						newHeight = GetDlgItemInt (hDlg, ID_EDIT_WINHEIGHT, (BOOL *) & success, 0);
					}
					if (! success) {
						messageBox (er, "Screen width and height should be positive integer values.");
						return true;
					}

					unsigned int newFrame = GetDlgItemInt (hDlg, ID_EDIT_SPEED,(BOOL *) &success, 0);
					if (! success || newFrame > 250 || newFrame < 1) {
						messageBox (er, "Milliseconds per frame (ms/frame) should be an integer between 1 and 250 inclusive.");
						return true;
					}

					settings.frameSpeed = newFrame;
					settings.screenWidth = newWidth;
					settings.screenHeight = newHeight;

					// GET FINAL FILE
					le = GetWindowTextLength (GetDlgItem (hDlg, ID_EDIT_OUTPUTFILE)) + 1;
					newText = new char[le];
					if (newText) {
						GetWindowText (GetDlgItem (hDlg, ID_EDIT_OUTPUTFILE), newText, le);
						if (settings.finalFile) delete settings.finalFile;
						settings.finalFile = newText;
					} else {
						errorBox ("Out of memory...", "Can't update quit message.");
					}

					// GET WINDOW NAME
					le = GetWindowTextLength (GetDlgItem (hDlg, ID_EDIT_NAME)) + 1;
					newText = new char[le];
					if (newText) {
						GetWindowText (GetDlgItem (hDlg, ID_EDIT_NAME), newText, le);
						if (settings.windowName) delete settings.windowName;
						settings.windowName = newText;
					} else {
						errorBox ("Out of memory...", "Can't update window name.");
					}

					// GET QUIT MESSAGE
					le = GetWindowTextLength (GetDlgItem (hDlg, ID_EDIT_QUITMESSAGE)) + 1;
					newText = new char[le];
					if (newText) {
						GetWindowText (GetDlgItem (hDlg, ID_EDIT_QUITMESSAGE), newText, le);
						if (settings.quitMessage) delete settings.quitMessage;
						settings.quitMessage = newText;
					} else {
						errorBox ("Out of memory...", "Can't update quit message.");
					}

					// GET ICON FILENAME
					le = GetWindowTextLength (GetDlgItem (hDlg, ID_EDIT_CUSTOMICON)) + 1;
					newText = new char[le];
					if (newText) {
						GetWindowText (GetDlgItem (hDlg, ID_EDIT_CUSTOMICON), newText, le);
						if (settings.customIcon) delete settings.customIcon;
						settings.customIcon = newText;
					} else {
						errorBox ("Out of memory...", "Can't update custom icon filename.");
					}

					// GET LOGO FILENAME
					le = GetWindowTextLength (GetDlgItem (hDlg, ID_EDIT_CUSTOMLOGO)) + 1;
					newText = new char[le];
					if (newText) {
						GetWindowText (GetDlgItem (hDlg, ID_EDIT_CUSTOMLOGO), newText, le);
						if (settings.customLogo) delete settings.customLogo;
						settings.customLogo = newText;
					} else {
						errorBox ("Out of memory...", "Can't update custom logo filename.");
					}

					// GET DATA FOLDER NAME
					le = GetWindowTextLength (GetDlgItem (hDlg, ID_EDIT_DATAFOLDER)) + 1;
					newText = new char[le];
					if (newText) {
						GetWindowText (GetDlgItem (hDlg, ID_EDIT_DATAFOLDER), newText, le);
						if (settings.runtimeDataFolder) delete settings.runtimeDataFolder;
						settings.runtimeDataFolder = newText;
					} else {
						errorBox ("Out of memory...", "Can't update run-time data folder name.");
					}

					// GET LANGUAGE
					le = GetWindowTextLength (GetDlgItem (hDlg, ID_EDIT_LANGUAGE)) + 1;
					newText = new char[le];
					if (newText) {
						GetWindowText (GetDlgItem (hDlg, ID_EDIT_LANGUAGE), newText, le);
						if (settings.originalLanguage) delete settings.originalLanguage;
						settings.originalLanguage = newText;
					} else {
						errorBox ("Out of memory...", "Can't update language name.");
					}

					// GET MORE STUFF
					settings.runFullScreen = isChecked (hDlg, ID_FULLSCREEN);
					settings.forceSilent = isChecked (hDlg, ID_MAKESILENT);
					settings.ditherImages = isChecked (hDlg, ID_DITHERIMAGES);
//					setChanged (true);
				}

				case IDCANCEL:
				EndDialog(hDlg, true);
				return (true);

			}
			break;
	}

    return false;
}

void upperString (char * s) {
	while (* s) {
		(* s) = toupper (* s);
		s ++;
	}
}

LRESULT CALLBACK resourcesBoxFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
        case WM_INITDIALOG:
        {
			int i, n = MESS (ID_FILELIST, LB_GETCOUNT, 0, 0);
       		char * tx;

			gotoSourceDirectory ();

        	if (searchString) {
	        	SetDlgItemText (hDlg, IDOK, "Edit");
				//SetWindowText (hDlg, "Search results");
				if (! programSettings.searchSensitive) upperString (searchString);
        		for (i = 0; i < n; i ++) {
					tx = getFileFromBox (i);
					if (tx) {
						bool gotcha = false;
						char * entireFile = grabWholeFile (tx);

						if (entireFile) {
							if (! programSettings.searchSensitive) upperString (entireFile);

							// Look for searchString...
							for (int i = 0; entireFile[i]; i ++) {
								int j;
								for (j = 0; searchString[j]; j ++) {
									if (entireFile[i + j] != searchString[j])
										break;
								}
								if (! searchString[j]) {
									gotcha = true;
									break;
								}
							}

							// If we found it, add the name of the file
							if (gotcha) {
		    	    			if (LB_ERR == SendDlgItemMessage (hDlg, ID_FILELIST, LB_FINDSTRINGEXACT, (WPARAM) -1, (LPARAM) tx)) {
									SendDlgItemMessage (hDlg, ID_FILELIST, LB_ADDSTRING, (WPARAM) 0, (LPARAM) tx);
								}
							}
							delete entireFile;
						}
						delete tx;
					}
				}
				delete searchString;
				searchString = NULL;
        	} else {
				FILE * fp;
				char t, lastOne;
				parseMode pM;
				char buffer[256];
				int numBuff=0;

				for (i = 0; i < n; i ++) {
					if (SendDlgItemMessage (mainWin, ID_FILELIST, LB_GETSEL, (WPARAM) i, (LPARAM) 0)) {
						tx = getFileFromBox (i);
						char * extension = tx + strlen(tx) - 4;
//						errorBox ("Extension", extension);
						if (strlen (tx) > 4 && strcmp (extension, ".slu") == 0) {
							fp = fopen (tx, "rb");
							if (fp) {
//								errorBox ("Searching through", tx);
								pM = PM_NORMAL;
								t = ' ';
								for (;;) {
									lastOne = t;
									t = fgetc (fp);
									if (feof (fp)) break;
									switch (pM) {
										case PM_NORMAL:
										if (t == '\'') {
											pM = PM_FILENAME;
											numBuff = 0;
										}
										if (t == '\"') pM = PM_QUOTE;
										if (t == '#') pM = PM_COMMENT;
										break;

										case PM_COMMENT:
										if (t == '\n') pM = PM_NORMAL;
										break;

										case PM_QUOTE:
										if (t == '\"' && lastOne != '\\') pM = PM_NORMAL;
										break;

										case PM_FILENAME:
										if (t == '\'' && lastOne != '\\') {
											buffer[numBuff] = 0;
											pM = PM_NORMAL;
//											errorBox ("Just found resource file", buffer);
											if (LB_ERR == SendDlgItemMessage (hDlg, ID_FILELIST, LB_FINDSTRINGEXACT, (WPARAM) -1, (LPARAM) buffer)) {
												SendDlgItemMessage (hDlg, ID_FILELIST, LB_ADDSTRING, (WPARAM) 0, (LPARAM) buffer);
											}
										} else {
											buffer[numBuff++] = t;
											if (numBuff == 250) {
												buffer[numBuff++] = 0;
												errorBox ("Resource filename too long!", buffer);
												numBuff = 0;
											}
										}
										break;
									}
								}
								fclose (fp);
							} else {
								errorBox ("Can't open and hunt through", tx);
							}
						}
						else
						{
//							errorBox ("Skipping because not a .slu file", tx);
						}
						delete tx;
					}
				}
			}
			return (true);
		}

		case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDOK:
			int n = SendDlgItemMessage (hDlg, ID_FILELIST, LB_GETCOUNT, (WPARAM) 0, (LPARAM) 0);
			for (int i = 0; i < n; i ++) {
				if (SendDlgItemMessage (hDlg, ID_FILELIST, LB_GETSEL, (WPARAM) i, (LPARAM) 0)) {
					char * me = new char[SendDlgItemMessage (hDlg, ID_FILELIST, LB_GETTEXTLEN, (WPARAM) i, (LPARAM) 0) + 1];
					SendDlgItemMessage (hDlg, ID_FILELIST, LB_GETTEXT, (WPARAM) i, (LPARAM) me);
//					editFile (me, hDlg);
					delete me;
				}
			}
			return true;

			case IDCANCEL:
			EndDialog(hDlg, true);
			return true;
		}
		break;
	}

    return false;
}

LRESULT CALLBACK searchBoxFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
        case WM_INITDIALOG:
       	if (programSettings.searchSensitive) CheckDlgButton (hDlg, ID_CASESENSITIVE, BST_CHECKED);
		return (true);

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
				int le = GetWindowTextLength (GetDlgItem (hDlg, ID_TEXT_EDITOR)) + 1;
				if (le == 1) {
					messageBox ("Find", "Please enter something to look for... What am I, a mindreader?");
					return 1;
				}
				searchString = new char[le];
				programSettings.searchSensitive = isChecked (hDlg, ID_CASESENSITIVE);

				//putRegSetting ("searchSensitive", programSettings.searchSensitive);

				if (searchString) {
					GetWindowText (GetDlgItem (hDlg, ID_TEXT_EDITOR), searchString, le);
					DialogBox(inst, MAKEINTRESOURCE(999), mainWin, (DLGPROC)resourcesBoxFunc);
				} else {
					messageBox ("Find", "No memory for specified string.");
				}
				// No break; here

				case IDCANCEL:
				EndDialog(hDlg, true);
				return (true);
			}
			break;
	}

    return false;
}

HBITMAP hbitmap = NULL;

void drawLogo (HWND hDlg, int x, int y) {
	PAINTSTRUCT ps;
	HDC hDestDC = BeginPaint (hDlg, &ps);

	if (hbitmap) {
		HDC hSrcDC = CreateCompatibleDC (NULL);
		if (hSrcDC) {
			if (SelectObject (hSrcDC, hbitmap)) {
				BITMAP bmp;          		    // structure for bitmap info
				if (GetObject (hbitmap, sizeof (BITMAP), & bmp)) {
					BitBlt (hDestDC, x, y, bmp.bmWidth, bmp.bmHeight, hSrcDC, 0, 0, SRCCOPY);
				}
			}
			DeleteDC (hSrcDC);
		}
	}

	EndPaint (hDlg, &ps);
}

LRESULT CALLBACK aboutBoxFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
        case WM_INITDIALOG:
      	return true;

        case WM_PAINT:
		drawLogo (hDlg, 149, 12);
		return false;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
				gotoSite (hDlg, "http://www.adventuredevelopers.com/forum/index.php?board=14.0");
				break;

				case IDCANCEL:
				EndDialog(hDlg, true);
				return (true);
			}
			break;
	}

    return false;
}



LRESULT CALLBACK sizeBoxFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
        case WM_INITDIALOG:
        {
        //	SetDlgItemInt (hDlg, ID_EDIT_WINWIDTH, getRegInt ("utilityWidth", 640), 0);
        //	SetDlgItemInt (hDlg, ID_EDIT_WINHEIGHT, getRegInt ("utilityHeight", 480), 0);
		//	if (! loadedFile) EnableWindow (GetDlgItem (hDlg, ID_USE_PROJECT_SIZE), false);
			return (true);
		}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_USE_PROJECT_SIZE:
    	    	SetDlgItemInt (hDlg, ID_EDIT_WINWIDTH, settings.screenWidth, 0);
	        	SetDlgItemInt (hDlg, ID_EDIT_WINHEIGHT, settings.screenHeight, 0);
				return (true);

				case IDOK:
				{
					bool success;
					unsigned int newHeight, newWidth = GetDlgItemInt (hDlg, ID_EDIT_WINWIDTH, (BOOL *) & success, 0);
					if (success) {
						newHeight = GetDlgItemInt (hDlg, ID_EDIT_WINHEIGHT, (BOOL *) & success, 0);
					}
					if (! success || newHeight < 100 || newWidth < 100) {
						messageBox (er, "Width and height should both be positive integer values bigger than 99");
						return true;
					}
					//if (! putRegInt ("utilityWidth", newWidth))		return true;
					//if (! putRegInt ("utilityHeight", newHeight))	return true;
				}

				case IDCANCEL:
				EndDialog(hDlg, true);
				return (true);
			}
			break;
	}

    return false;
}

void setTick (int what, bool yesNo) {
	CheckMenuItem (myMenu, what, yesNo ? MF_CHECKED : MF_UNCHECKED);
}

void activateMenus (bool on) {
	EnableWindow (GetDlgItem (mainWin, ID_FILELIST), on);
	EnableWindow (GetDlgItem (mainWin, ID_PROJECT_COMPILE), on);
	EnableWindow (GetDlgItem (mainWin, ID_DISPLAY), on);
	EnableWindow (GetDlgItem (mainWin, ID_PROJECT_SAVE), on /*&& changed*/);
	EnableWindow (GetDlgItem (mainWin, ID_CONTENTS_ADD_FILE), on);
	EnableWindow (GetDlgItem (mainWin, ID_CONTENTS_FIND_IN_FILE), on);
	EnableWindow (GetDlgItem (mainWin, ID_CONTENTS_RESOURCES), on);
	EnableWindow (GetDlgItem (mainWin, ID_CONTENTS_EDIT_FILE), on);
	EnableWindow (GetDlgItem (mainWin, ID_CONTENTS_DELETE_FILE), on);
//	EnableWindow (GetDlgItem (mainWin, ID_EDIT_QUITMESSAGE), on);
//	EnableWindow (GetDlgItem (mainWin, ID_EDIT_WINWIDTH), on);
//	EnableWindow (GetDlgItem (mainWin, ID_EDIT_WINHEIGHT), on);

	EnableMenuItem (myMenu, ID_PROJECT_COMPILE, on ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem (myMenu, ID_PROJECT_SAVEAS, on ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem (myMenu, ID_PROJECT_CLOSE, on ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem (myMenu, ID_CONTENTS_ADD_FILE, on ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem (myMenu, ID_CONTENTS_FIND_IN_FILE, on ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem (myMenu, ID_CONTENTS_RESOURCES, on ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem (myMenu, ID_DISPLAY, on ? MF_ENABLED : MF_GRAYED);

	EnableMenuItem (myMenu, ID_PROJECT_SAVE, (on /*&& changed*/) ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem (myMenu, ID_PROJECT_REVERT, (on /*&& changed*/) ? MF_ENABLED : MF_GRAYED);
}

char * getFileFromBox (int i) {
	char * me = new char[MESS (ID_FILELIST, LB_GETTEXTLEN, i, 0) + 1];
	MESS (ID_FILELIST, LB_GETTEXT, i, me);
	return me;
}



bool checkOverwrite (char * fn) {
	FILE * fp = fopen (fn, "rb");
	if (fp) {
		fclose (fp);
		char * buff = joinStrings ("That file already exists! Are you sure you want to overwrite the following file?\n\n", fn);
		if (MessageBox (mainWin, buff, "Overwrite?", MB_TASKMODAL | MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND) == IDYES) {
			delete buff;
			return true;
		} else {
			delete buff;
			return false;
		}
	}
	return true;
}

void updateFileList()
{
	MESS(ID_FILELIST, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < fileListNum; i++)
    {
        int pos = (int) MESS(ID_FILELIST, LB_ADDSTRING, 0, fileList[i]);
        // Set the array index of the player as item data.
        // This enables us to retrieve the item from the array
        // even after the items are sorted by the list box.
        MESS(ID_FILELIST, LB_SETITEMDATA, pos, i);
        fprintf (stderr, "%d: %s\n", i, fileList[i]);
    }
}


BOOL APIENTRY dialogproc (HWND h, UINT m, WPARAM w, LPARAM l) {

	static OPENFILENAME ofn;
	static char path[MAX_PATH];

	switch (m) {
//		case WM_TIMER:
//			{
//				sprintf(text,"%.1f",BASS_GetCPU());
//				MESS(ID_CPU,WM_SETTEXT,0,text);
//			}
//			break;

		case WM_COMMAND:
		switch (LOWORD(w)) {
			case ID_SETTINGSBOX:
//			DialogBox(inst, MAKEINTRESOURCE(DLG_FILE_LOCATIONS), h, (DLGPROC)settingsBoxFunc);
			break;

			case ID_SIZEBOX:
			DialogBox(inst, MAKEINTRESOURCE(DLG_WINDOW_SIZES), h, (DLGPROC)sizeBoxFunc);
			break;

			case ID_DISPLAY:
			DialogBox(inst, MAKEINTRESOURCE(DLG_PROJECT_PROPERTIES), h, (DLGPROC)prefsBoxFunc);
			break;

			case ID_PROJECT_SAVE:
                saveProject (loadedFile, fileList, &fileListNum);
                updateTitle();

			break;

			case ID_PROJECT_CLOSE:		// My menu thingy
                closeProject (fileList, &fileListNum);
                updateFileList();
                delete loadedFile;
                loadedFile = NULL;
                updateTitle();
			break;

			case ID_PROJECT_COMPILE:
                if (compWin)
                    DestroyWindow (compWin);

                CreateDialog(inst, MAKEINTRESOURCE(DLG_COMPILE), mainWin, dialogComp);
			break;

			case IDCANCEL:				// X button
			case ID_EXIT:
			PostQuitMessage (0);
			break;

			case ID_LAUNCH_FLOOR:
			case ID_LAUNCH_Z:
			case ID_LAUNCH_ENGINE:
			case ID_LAUNCH_TRANS:
			case ID_LAUNCH_SPRITE:
			//launchOtherKitProgram (LOWORD(w), h);
			break;

			case ID_PROJECT_SAVEAS:
			{
				char file[MAX_PATH]="";
				messageBox ("Important! If you save this project into a different directory, the project manager and compiler will not be able to find the SLUDGE scripts (as all locations are relative to the location of the project file).", "To move an entire project to a new location, please use Windows to copy all the required files, or clear the list of files and add them again.");
				ofn.lpstrFilter="SLUDGE projects (*.SLP)\0*.slp\0\0";
				ofn.lpstrFile=file;
				ofn.lpstrTitle = "Save this SLUDGE project as...";
				ofn.Flags=OFN_HIDEREADONLY|OFN_EXPLORER;
				if (GetSaveFileName(&ofn)) {
					memcpy(path,file,ofn.nFileOffset);
					path[ofn.nFileOffset-1]=0;
					fixExtension (file, "slp");
					if (checkOverwrite (file)) {
						saveProject (file, fileList, &fileListNum);
                        setLoadedFile (file);
                        updateTitle();
					}
				}
			}
			break;

			case ID_PROJECT_NEW:
			{
				char file[MAX_PATH]="New.slp";
				ofn.lpstrFilter="SLUDGE projects (*.SLP)\0*.slp\0\0";
				ofn.lpstrTitle = "Create a new SLUDGE project...";
				ofn.lpstrFile=file;
				ofn.Flags=OFN_HIDEREADONLY|OFN_EXPLORER;
				if (GetSaveFileName(&ofn)) {
					memcpy(path,file,ofn.nFileOffset);
					path[ofn.nFileOffset-1]=0;
					fixExtension (file, "slp");
					if (checkOverwrite (file)) {
					    fileListNum = 0;
						doNewProject (file, fileList, &fileListNum);
                        setLoadedFile (file);
                        updateTitle();
					}
				}
				updateFileList();
			}
			break;

			case ID_CONTENTS_FIND_IN_FILE:
			DialogBox(inst, MAKEINTRESOURCE(666), h, (DLGPROC)searchBoxFunc);
			break;

			case ID_CONTENTS_RESOURCES:
			DialogBox(inst, MAKEINTRESOURCE(999), h, (DLGPROC)resourcesBoxFunc);
			break;

			case ID_ABOUT:
			DialogBox(inst, MAKEINTRESOURCE(1234), h, (DLGPROC)aboutBoxFunc);
			break;

			case ID_SLUDGE_HELP:
//			WinHelp (mainWin, helpFile, HELP_FINDER, 0);
			//editFile (helpFile, mainWin);
			break;

			case ID_CONTENTS_ADD_FILE:
			{
				char file[MAX_PATH]="";
				ofn.lpstrFilter="SLUDGE scripts (*.SLU)\0*.slu\0SLUDGE definition files (*.SLD)\0*.sld\0SLUDGE translation files (*.TRA)\0*.tra\0";
				ofn.lpstrFile=file;
				ofn.lpstrTitle = "Add a file to the current project...";
				ofn.Flags=OFN_HIDEREADONLY|OFN_EXPLORER;

				if (GetOpenFileName(&ofn)) {
					memcpy(path,file,ofn.nFileOffset);
					path[ofn.nFileOffset-1]=0;
					switch (ofn.nFilterIndex) {
						case 1:
						fixExtension (file, "slu");
						break;

						case 2:
						fixExtension (file, "sld");
						break;

						case 3:
						fixExtension (file, "tra");
						break;
					}
					FILE * tester = fopen (file, "rb");

					bool ok = true;
					if (tester) {
						fclose (tester);
					} else {
						char * q1 = joinStrings ("The file \"", file);
						char * q2 = joinStrings (q1, "\" doesn't exist at the moment. Do you want to create it now?");
						delete q1;
						ok = MessageBox (mainWin, q2, "Non-existant file!", MB_YESNO | MB_SETFOREGROUND) == IDYES;
						delete q2;

						if (ok) {
							tester = fopen (file, "wb");
							fclose (tester);
						}
					}

					if (ok) {
					    getSourceDirFromName(loadedFile);
						addFileToProject (file, sourceDirectory, fileList, &fileListNum);
						//setChanged (true);
                        updateFileList();
					}
				}
			}
			break;

			case ID_CONTENTS_DELETE_FILE:
			{
                int i, o, num = fileListNum;
                for (i=0, o=0; i<num; i++, o++) {
					if (! MESS (ID_FILELIST, LB_GETSEL, i, 0)) continue;
                    removeFileFromList (o, fileList, &fileListNum);
                    o--;
                }
				//setChanged (true);
				updateFileList();
			}
			break;

			case ID_CONTENTS_EDIT_FILE:
			{
				int i, n = MESS (ID_FILELIST, LB_GETCOUNT, 0, 0);

				for (i = 0; i < n; i ++) {
					if (MESS (ID_FILELIST, LB_GETSEL, i, 0)) {
						char * fn = getFileFromBox (i);
						editFile (fn, h);
						delete fn;
					}
				}
			}
			break;
/*
			case ID_KEEP_IMAGES:
			if (putRegSetting ("compilerKillImages", keepImages)) {
				keepImages = ! keepImages;
				setTick (ID_KEEP_IMAGES, keepImages);
			}
			break;

			case ID_KEEP_STRINGS:
			if (putRegSetting ("compilerWriteStrings", ! keepStrings)) {
				keepStrings = ! keepStrings;
				setTick (ID_KEEP_STRINGS, keepStrings);
			}
			break;

			case ID_VERBOSE:
			if (putRegSetting ("compilerVerbose", ! verboseMode)) {
				verboseMode = ! verboseMode;
				setTick (ID_VERBOSE, verboseMode);
			}
			break;
*/

			case ID_PROJECT_LOAD:
			{
				char file[MAX_PATH]="";
				ofn.lpstrFilter = "SLUDGE projects (*.SLP)\0*.slp\0\0";
				ofn.lpstrFile = file;
				ofn.lpstrTitle = "Load a SLUDGE project...";
				ofn.Flags=OFN_HIDEREADONLY|OFN_EXPLORER;
				if (GetOpenFileName(&ofn)) {
					memcpy(path,file,ofn.nFileOffset);
					path[ofn.nFileOffset-1]=0;
					loadProject (file, fileList, &fileListNum);
					updateFileList();
                    setLoadedFile (file);
                    updateTitle();
				}
			}
			break;
		}
		break;

		case WM_INITDIALOG:

		// Set up the menu
		myMenu = LoadMenu (inst, MAKEINTRESOURCE(1));
		SetMenu (h, myMenu);
//		verifyOtherKitPrograms ();

		mainWin=h;
		SetClassLong(h, GCL_HICON, (LONG) LoadIcon(inst, MAKEINTRESOURCE(3)));
		GetCurrentDirectory(MAX_PATH,path);
		memset(&ofn,0,sizeof(ofn));
		ofn.lStructSize=sizeof(ofn);
		ofn.hwndOwner=h;
		ofn.hInstance=inst;
		ofn.nMaxFile=MAX_PATH;
		ofn.lpstrInitialDir=path;
        noSettings ();

		keepImages = ! programSettings.compilerKillImages;
		setTick (ID_KEEP_IMAGES, keepImages);

		keepStrings = programSettings.compilerWriteStrings;
		setTick (ID_KEEP_STRINGS, keepStrings);

		verboseMode = programSettings.compilerVerbose;
		setTick (ID_VERBOSE, verboseMode);

		if (loader[0]) {
//			messageBox ("Trying to load...", loader);
//			loadProject (loader);
		} else {
//			updateTitle ();
		}
		return 1;

        case WM_PAINT:
		drawLogo (h, 301, 18);
		return false;

		case WM_DESTROY:
		//WinHelp (mainWin, helpFile, HELP_QUIT, 0);
		PostQuitMessage(0);
		return 1;
	}
	return 0;
}

const char * getTempDir () {
	char la[] = "%temp%";
	char buffer[500];
	char * returnVal = NULL;


	if (ExpandEnvironmentStrings (la, buffer, 499) == 0) {
		addComment (ERRORTYPE_SYSTEMERROR, "Can't expand string containing environment variable(s)", la, NULL);
		return NULL;
	}
	if (! buffer[0]) {
		addComment (ERRORTYPE_SYSTEMERROR, "No environment variable(s)", la, NULL);
		return NULL;
	}
	returnVal = joinStrings (buffer, "");

	return returnVal;
}


int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	inst=hInstance;
	loader = lpCmdLine;
	if (loader[0] == '\"') {
		stringArray * bits = splitString (loader + 1, '\"', ONCE);
		loader = joinStrings (bits -> string, "");
		while (destroyFirst (bits)) {;}
	}

	hbitmap = (HBITMAP) LoadImage (hInstance, MAKEINTRESOURCE (1235),
                                    IMAGE_BITMAP, 0, 0,
                                    LR_CREATEDIBSECTION);

	if (! CreateDialog(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, dialogproc)) {
		return 0;
	}

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0))
				break;
			if (! IsDialogMessage (mainWin, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else
			WaitMessage();
	}

	return 0;
}

#endif
