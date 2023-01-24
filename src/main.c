/* RGDBoot */

#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <gctypes.h>

#include "errorhandler.h"
#include "errorcodes.h"
#include "seeprom.h"
#include "boot2.h"

#define RGDSDB_VER_MAJOR	0
#define RGDSDB_VER_MINOR	3

#define AHBPROT_DISABLED (*(vu32*)0xcd800064 == 0xFFFFFFFF)

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int WaitForPad() {
	int wpadButtons = 0;
	while(1) {
		WPAD_ScanPads();
		for(int i = 0; i < 4; i++) {
				wpadButtons += WPAD_ButtonsDown(i);
		}
		if (wpadButtons) break;
		VIDEO_WaitVSync();
	}
	return wpadButtons;
}

int main(int argc, char **argv) {

	VIDEO_Init();
	WPAD_Init();
	PAD_Init();
	rmode = VIDEO_GetPreferredMode(NULL);
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ); //	console_init(xfb, 396, 124, 305, 212, 305*VI_DISPLAY_PIX_SZ);
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	u32 wpadButtons = 0;
	u32 padButtons = 0;
	
	printf("\x1b[2;0H");
	printf("RGD SDBoot Installer v%u.%u - by \x1b[32mroot1024\x1b[37m, \x1b[31mRedBees\x1b[37m, \x1b[31mDeadlyFoez\x1b[37m, \x1b[31mLarsenv\x1b[37m \n raregamingdump.ca", RGDSDB_VER_MAJOR, RGDSDB_VER_MINOR);
	fatInitDefault();

	if(!AHBPROT_DISABLED) { 
		ThrowError(errorStrings[ErrStr_NeedPerms]);
	}
	printf("\nPress any controller button to clear the boot2 version.");
	WaitForPad(wpadButtons, padButtons);
	clearVersion();
	
	int choice = 0;
	s32 ret = 0;
	printf("\nSuccess!\nPress the A button to install SDboot from /boot2/sdboot.bin, or the B button to install nandboot from /boot2/nandboot.bin.\n");

choice:
	choice = WaitForPad(wpadButtons);
	if(choice & WPAD_BUTTON_A) {
		ret = installRAWboot2("/boot2/sdboot.bin");
		goto out;
	}
	if(choice & WPAD_BUTTON_B) {
		ret = installRAWboot2("/boot2/nandboot.bin");
		goto out;
	}
	else { goto choice; }
	
out:
	switch(ret){
		case 0:
			printf("SUCCESS!\n"); break;
		case -4:
			printf("Error: cannot install boot2 using an emulator\n"); break;
		case -1022:
			printf("Error: content SHA1 does not match the TMD hash\n"); break;
		case -1031:
			printf("Error: cannot downgrade boot2\n"); break;
		default:
			printf("Unknown error: %d\n", ret); break;
	}


	printf("\x1b[37m\n\nPress any controller button to exit.");
	while(1) {

		WPAD_ScanPads();
		PAD_ScanPads();
		for(int i = 0; i < 4; i++) {
			wpadButtons += WPAD_ButtonsDown(i);
			padButtons += PAD_ButtonsDown(i);
		}
		if (wpadButtons || padButtons) exit(0);

		VIDEO_WaitVSync();
	}

	return 0;
}