#include "kernel.h"
#include "../libc/string.h"
#include "../cpu/soundManager.h"
#include "../cpu/timer.h"
#include "../drivers/sound.h"
#include "../drivers/time.h"
#include "../fs/hdd.h"
#include "../fs/hddw.h"
#include "../libc/stdio.h"
#include <stdint.h>
#include "../libc/mem.h"

int arg = 0; //Is an argument being taken?
int argt = 0; //Which Command Is taking the argument?

void execute_command(char *input) {
	char *testy;
	kprint("\n");
  if (strcmp(input, "shutdown") == 0) {
		shutdown();
  } else if (strcmp(input, "panic") == 0) {
		panic();
  } else if (strcmp(input, "fmem") == 0) {
	  	free(*testy, 0x1000);
  } else if (strcmp(input, "free") == 0) {
        char temp[25];
		int_to_ascii(memoryRemaining, temp);
		kprint("Memory available: ");
		kprint(temp);
		kprint(" bytes\n");
		int_to_ascii(usedMem, temp);
		kprint("Memory used: ");
		kprint(temp);
		kprint(" bytes");
  } else if (strcmp(input, "help") == 0) {
		kprint("Commands: scan, testDrive, fmem, help, shutdown, panic, print, clear, bgtask, bgoff, time, read, drives, select, testMem, free\n");
	} else if (strcmp(input, "clear") == 0){
		clear_screen();
	} else if (match("print", input) == -2) {
		kprint("Not enough args!");
	} else if ((match(input, "print") + 2) == 6) {
		kprint(afterSpace(input));
	} else if (match("tone", input) == -2) {
		kprint("Not enough args!");
	} else if ((match(input, "tone") + 1) == 4) {
		char test[20];
		int_to_ascii(atoi(afterSpace(input)), test);
		kprint(test);
		p_tone(atoi(afterSpace(input)), 100);
	} else if (strcmp(input, "bgtask") == 0) {
		kprint("Background task started!");
		task = 1;
	} else if (strcmp(input, "bgoff") == 0) {
		kprint("Background task stopped!");
		task = 0;
	} else if (strcmp(input, "testMem") == 0) {
		uint32_t pAddr;
		for (int c = 0; c < 10000; c++) {
			testy = (char *)kmalloc(0x1000);
			strcpy(testy, "ok this is a test\0");
			kprint(testy);
			char temp[25];
			int_to_ascii(memoryRemaining, temp);
			kprint("\nMemory Remaining: ");
			kprint(temp);
			kprint(" bytes\n");
		}
		free(testy, 0x1000);
	} else if (strcmp(input, "testMemLess") == 0) {
		uint32_t pAddr;
		for (int c = 0; c < 1; c++) {
			testy = (char *)kmalloc(0x1000);
			strcpy(testy, "ok this is a test\0");
			kprint(testy);
			char temp[25];
			int_to_ascii(memoryRemaining, temp);
			kprint("\nMemory Remaining: ");
			kprint(temp);
			kprint(" bytes\n");
		}
		free(testy, 0x1000);
	} else if (strcmp(input, "time") == 0) {
		read_rtc();
		kprint_int(year);
		kprint("/");
		kprint_int(month);
		kprint("/");
		kprint_int(day);
		if (hour - 5 < 0) {
			kprint(" ");
			kprint_int(24 + (hour - 5));
		} else if(hour - 5 <= 10) {
			kprint(" 0");
			kprint_int(hour - 5);
		} else {
			kprint(" ");
			kprint_int(hour - 5);
		}
		if (minute > 9) {
			kprint(":");
			kprint_int(minute);
		} else {
			kprint(":0");
			kprint_int(minute);
		}
		if (second > 9) {
			kprint(":");
			kprint_int(second);
		} else {
			kprint(":0");
			kprint_int(second);
		}
	} else if (strcmp(input, "scan") == 0) {
		drive_scan();
	} else if (match("testDrive", input) == -2) {
		kprint("Not enough args!");
	} else if ((match(input, "testDrive") + 1) == 9) {
		writeIn[0] = 0x1111;
		writeFromBuffer(atoi(afterSpace(input)));
		kprint("Writing 0x1111 to sector ");
		kprint_int(atoi(afterSpace(input)));
		kprint("\n");
	} else if (match("read", input) == -2) {
		kprint("Not enough args!");
	} else if ((match(input, "read") + 1) == 4) {
		read_disk(atoi(afterSpace(input)));
		//kprint(atoi(afterSpace(input)));
	} else if (match("select", input) == -2) {
		kprint("Not enough args!");
	} else if ((match(input, "select") + 1) == 6) {
		uint8_t driveToSet = atoi(afterSpace(input));
		if (driveToSet == 1) {
			if (mp == 0) {
				ata_drive = MASTER_DRIVE;
				ata_controler = PRIMARY_IDE;
				nodrives = 0;
			} else if (mp48 == 0) {
				ata_drive = MASTER_DRIVE_PIO48;
				ata_controler = PRIMARY_IDE;
				ata_pio = 1;
				nodrives = 0;
			} else {
				kprint("That drive is offline!\n");
			}
		} else if (driveToSet == 2) {
			if (ms == 0) {
				ata_drive = SLAVE_DRIVE;
				ata_controler = PRIMARY_IDE;
				nodrives = 0;
			} else if (ms48 == 0) {
				ata_drive = SLAVE_DRIVE_PIO48;
				ata_controler = PRIMARY_IDE;
				ata_pio = 1;
				nodrives = 0;
			} else {
				kprint("That drive is offline!\n");
			}
		} else if (driveToSet == 3) {
			if (sp == 0) {
				ata_drive = MASTER_DRIVE;
				ata_controler = SECONDARY_IDE;
				nodrives = 0;
			} else if (sp48 == 0) {
				ata_drive = MASTER_DRIVE_PIO48;
				ata_controler = SECONDARY_IDE;
				ata_pio = 1;
				nodrives = 0;
			} else {
				kprint("That drive is offline!\n");
			}
		} else if (driveToSet == 4) {
			if (ss == 0) {
				ata_drive = SLAVE_DRIVE;
				ata_controler = SECONDARY_IDE;
				ata_pio = 0;
				nodrives = 0;
			} else if (ss48 == 0) {
				ata_drive = SLAVE_DRIVE_PIO48;
				ata_controler = SECONDARY_IDE;
				ata_pio = 1;
				nodrives = 0;
			} else {
				kprint("That drive is offline!\n");
			}
		} else {
			kprint("Not a valid drive!\n");
		}
		//kprint(atoi(afterSpace(input)));
	} else if (match("copy", input) == -2) {
		kprint("Not enough args!");
	} else if ((match(input, "copy") + 1) == 4) {
		copy_sector(0, atoi(afterSpace(input)));
	} else if (match("clearS", input) == -2) {
		kprint("Not enough args!");
	} else if ((match(input, "clearS") + 1) == 6) {
		clear_sector(atoi(afterSpace(input)));
	} else if (strcmp("drives", input) == 0) {
		if (mp == 0 || mp48 == 0) {
			kprint("Primary IDE, Master Drive (Drive 1): Online\n");
		} else {
			kprint("Primary IDE, Master Drive (Drive 1): Offline\n");
		}
		if (ms == 0 || ms48 == 0) {
			kprint("Primary IDE, Slave Drive (Drive 2): Online\n");
		} else {
			kprint("Primary IDE, Slave Drive (Drive 2): Offline\n");
		}
		if (sp == 0 || sp48 == 0) {
			kprint("Secondary IDE, Master Drive (Drive 3): Online\n");
		} else {
			kprint("Secondary IDE, Master Drive (Drive 3): Offline\n");
		}
		if (ss == 0 || ss48 == 0) {
			kprint("Secondary IDE, Slave Drive (Drive 4): Online\n");
		} else {
			kprint("Secondary IDE, Slave Drive (Drive 4): Offline\n");
		}
	} else {
		kprint("Unknown command: ");
		kprint(input);
		p_tone(100, 5);
	}
	kprint("\n");
	kprint("drip@DripOS> ");
}

void p_tone(uint32_t soundin, int len) {
	play(soundin);
	lSnd = tick;
	pSnd = len;
}

void read_disk(uint32_t sector) {
	uint32_t sectornum;
	uint16_t nom;
	sectornum = 0;
	char str1[32];
	char str2[32];
	kprint ("\nSector ");
	kprint_int(sector);
	kprint(" contents:\n\n");
 
	//! read sector from disk
	read(sector);
	for (int l = 0; l<256; l++) {
		//hex_to_ascii(sector[l] & 0xff, str1);
		//hex_to_ascii((sector[l] >> 8), str2);
		//kprint(str2);
		//kprint(" ");
		//kprint(str1);
		//kprint(" ");
		hex_to_ascii(readOut[l], str2);
		kprint(str2);
		kprint(" ");
		for (int i = 0; i<32; i++) {
			str1[i] = 0;
			str2[i] = 0;
		}
	}
	clear_ata_buffer();
	//kprint_int(sizeof(ata_buffer));
}