/*
* AtxCProgmem.h
*
* Created: 18/01/2018 15:39:14
*  Author: info
*/


#ifndef ATXCPROGMEM_H_
#define ATXCPROGMEM_H_

static const char* const S_GLOBAL_BANK_NAME[] = {" EDIT ", "SETUP " , " SEQ  " , "SDCARD"};
static const char S_ZONE[] = "Zone";  //In AtxCCstrings.h
static const char S_CARD[] = "Card";
static const char* const S_INTEXT[2] = {"INT","EXT"};
	
static const char* const S_SEQMODE_TOP[] = {"Over","Seq"   ,"Merge","Record","Edit" ,"","","","","","","","","","",""};
static const char* const S_SEQMODE_BOT[] = {"view","Length","Seq"  ,"Seq"   ,"Seq"  ,"","","","","","","","","","",""};
static const char* const S_SEQ_EVENTLIST_CTRL[] = {"Length","Channel","Command","Data1","Data2","","Index"};
static const char* const S_SEQ_ADDTRANSCTRL[] = {"Parameter","Div Ratio","Amplitude","Bias","Start","End"};
static const char* const S_SEQ_MSGMODE[] = {"Off","Param","Beats","Notes"};

static const char* const S_SD_FUNC_TOP[] = {"Load" ,"Save" ,"Load","","","","","","","","","","","","",""};
static const char* const S_SD_FUNC_BOT[] = {"Patch","Patch","MIDI","","","","","","","","","","","","",""};

//static const char* const S_ATXMSG_HDR[] = {"Blank","Parameter","Cmmn2","Cmmn3","SysEx St","Cmmn1","SysEx E2","SysEx E3","Note Off","Note On","Poly Key","Controller","Prog Ch","Ch Press","Pitch Bend","Realtime"};
static const char* const S_I2C_ERR = "I2C  X er:"	;


#endif /* ATXCPROGMEM_H_ */