/* RGD SDBoot Installer */

#include "menu.h"
#include "errorhandler.h"
#include "errorcodes.h"
#include "tools.h"
#include "installer.h"

#define CURSOR "-> "

#define DOWN 0
#define UP 1

u8 menuPosition = 0;

void ClearScreen( void ) {
	printf("\x1b[6;0H");
	for(int i = 0; i < MenuStr_Count + 15; i++) {
		printf("\33[2K\r\n");
	}
	printf("\x1b[6;0H");
}

void EnterOption( void ) {
	ClearScreen();

	switch(menuPosition) {
		case MenuStr_InstallSDBoot:
			SDBootInstaller();
			break;
		case MenuStr_InstallNANDBoot:
			NANDBootInstaller();
			break;
		case MenuStr_InstallBoot2WAD:
			Boot2WADInstaller();
			break;
		case MenuStr_InstallBoot2Backup:
			Boot2BackupInstaller();
			break;
		case MenuStr_MakeBoot2Backup:
			Boot2BackupMake();
			break;
		case MenuStr_BootSysCheck:
			BootSysCheck();
			break;
		case MenuStr_RestoreNANDBackup:
			RestoreNAND();
			break;
/*		case MenuStr_Credits:
			printf("\n\nApp Developers:\n       \x1b[32mroot1024\x1b[37m\n      \x1b31mRedBees\x1b[37m\n\nPayload Developers:\n         nitr8\n\nTesters:\n		\x1b[31mDeadlyFoez\x1b[37m\n\nUsing libruntimeiospatch by Nanolx\n\nAnd Wack0 for making it possible!");
			break;*/
		case MenuStr_Exit:
			exit(0);
			break;
		case DebugMenuStr_EraseNANDFS:
			EraseNANDFS();
			break;
		default:
			printf("\n\nThis doesn't work yet.");
			WaitForPad();
			break;
	}
	ClearScreen();
	PrintMenu();
	PrintCursor();
}

void PrintCursor( void ) {
	u8 location = menuPosition + 8;
        printf("\x1b[%i;0H", location);
        printf("%s", CURSOR);
        printf("\x1b[2;0H");
} 

void Move(u8 direction) {
	if(direction == UP) {
		if(menuPosition > 0) {
			menuPosition--;
		} else {
			menuPosition = MenuStr_Count + (DebugMenuStr_Count - MenuStr_Count) * enableDebugMenu - 1;
		}
	}  else if (direction == DOWN) {
		if(menuPosition < MenuStr_Count + (DebugMenuStr_Count - MenuStr_Count) * enableDebugMenu - 1) {
			menuPosition++;
		} else {
			menuPosition = 0;
		}
	}

	PrintMenu();
	PrintCursor();
}

void PrintMenu( void ) {
	printf("\x1b[8;0H");
	for(int i = 0; i < MenuStr_Count; i++) {
		printf("   %s\n", menuStrings[i]);
	}
	if(enableDebugMenu)
		for(int i = 0; i < DebugMenuStr_Count - MenuStr_Count; i++) {
			printf("   DEBUG - %s\n", debugMenuStrings[i]);
		}
	printf("\x1b[4;0H");
}

u8 EnterMenu( bool enableDebug ) {
	enableDebugMenu = enableDebug;

	PrintMenu();	
	PrintCursor();

	while(1) {
		switch(WaitForPad()) {
			case RGDSDB_PAD_BUTTON_UP:
			case WPAD_BUTTON_UP:
				Move(UP);
				break;
			case PAD_BUTTON_DOWN:
			case WPAD_BUTTON_DOWN:
				Move(DOWN);
				break;
			case PAD_BUTTON_A:
			case WPAD_BUTTON_A:
				EnterOption();
				break;
			default:
				break;
		}
	}
}	
