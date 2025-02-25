/* RGD SDBoot Installer */

#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <gctypes.h>

#include "errorhandler.h"
#include "errorcodes.h"
#include "seeprom.h"
#include "boot2.h"
#include "tools.h"
#include "prodinfo.h"
#include "runtimeiospatch.h"
#include "menu.h"
#include "flash.h"
#include "version.h"

#define AHBPROT_DISABLED (*(vu32*)0xcd800064 == 0xFFFFFFFF)

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int main(int argc, char **argv) {
	Fix_ES_ImportBoot();
	Enable_DevFlash();

	VIDEO_Init();
	WPAD_Init();
	PAD_Init();
	rmode = VIDEO_GetPreferredMode(NULL);
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	printf("\x1b[2;0H");
	
	if(IsDolphin()) {
		ThrowError(errorStrings[ErrStr_InDolphin]);
	} else if(IsWiiU()) {
		ThrowError(errorStrings[ErrStr_InCafe]);
	}
	if(!fatInitDefault()){
		ThrowError(errorStrings[ErrStr_SDCard]);
	}
	if(!AHBPROT_DISABLED) { 
		ThrowError(errorStrings[ErrStr_NeedPerms]);
	}
	if(NANDFlashInit() < 0){
		ThrowError(errorStrings[ErrStr_DevFlashErr]);
	}
	
	bool enableDebug = false;
	if(argc == 2)
		enableDebug = (strcmp(argv[1], "debug") == 0) ? true : false;

	printf("RGD SDBoot Installer build %s - by \x1b[32mroot1024\x1b[37m, \x1b[31mRedBees\x1b[37m, \x1b[31mDeadlyFoez\x1b[37m\nraregamingdump.ca", buildNumber);
	printf("\nCurrent boot2 version: %i", GetBoot2Version());
	printf("\tCurrent IOS version: IOS%i v%i\n\n", IOS_GetVersion(), IOS_GetRevision());
	
	printf("\nWARNING: PLEASE READ THIS CAREFULLY!\n\n");
	printf("THIS IS BETA SOFTWARE. AS SUCH, IT CARRIES A HIGH RISK OF BRICKING THE\nCONSOLE.\n\n");
	printf("THIS TOOL DIRECTLY WRITES TO BOOT1 AND BOOT2.\n\n");
	printf("IT'S ADVISED YOU USE THIS TOOL ONLY IF YOU HAVE AN EXTERNAL NAND FLASHER.\n\n");
	printf("BY CONTINUING, YOU ACCEPT THAT YOU USE THIS PROGRAM AT YOUR OWN RISK.\n\n");
	printf("THE AUTHORS CANNOT BE HELD RESPONSIBLE TO ANY DAMAGE THIS TOOL MAY CAUSE.\n\n");
	printf("IF YOU DON'T AGREE TO THESE TERMS, PLEASE QUIT THIS PROGRAM IMMEDIATELY.\n\n\n");

	printf("Press (+/A) to continue, or (-/Y) to quit the program and return to HBC.\n");

	while(1) {
		switch(WaitForPad()) {
			//case RGDSDB_PAD_BUTTON_PLUS:
			case WPAD_BUTTON_PLUS:
			case PAD_BUTTON_A:
				ClearScreen();
				EnterMenu(enableDebug);
				return 0;
			//case RGDSDB_PAD_BUTTON_MINUS:
			case WPAD_BUTTON_MINUS:
			case PAD_BUTTON_B:
				return 1;
		}
	}

	return 0; // NOT REACHED HERE
}
