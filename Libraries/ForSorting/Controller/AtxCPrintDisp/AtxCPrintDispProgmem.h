/*
 * AtxCPrintDispProgmem.h
 *
 * Created: 24/08/2021 11:47:02
 *  Author: info
 */ 


#ifndef ATXCPRINTDISPPROGMEM_H_
#define ATXCPRINTDISPPROGMEM_H_

static const uint8_t I_ATMFILT_COORD[16][32] =
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{6,6,6,6,6,6,6,6,5,5,5,4,3,2,2,1,1,2,4,6,8,10,12,12,13,14,14,15,15,15,15,15},
	{15,15,15,15,15,15,14,14,14,13,12,11,10,9,7,4,2,1,1,1,2,3,4,4,5,5,6,6,6,6,6,6},
	{15,15,15,14,14,14,13,13,12,12,11,10,9,8,6,4,2,2,4,5,8,9,10,11,12,12,13,14,14,14,15,15},
	{3,3,3,3,3,3,3,3,3,3,3,3,4,4,5,6,9,13,9,6,5,4,4,3,3,3,3,3,3,3,3,3},
	{5,5,5,5,5,5,5,5,5,4,4,4,3,3,3,2,2,2,2,3,3,3,4,4,4,5,5,5,5,5,5,5},
	{9,9,9,9,9,9,9,9,8,8,8,7,6,5,4,3,2,2,3,3,5,6,6,7,8,8,9,9,9,9,9,9},
	{12,12,12,12,12,12,11,11,11,11,10,10,9,8,7,5,2,2,5,7,8,9,10,11,11,11,11,12,12,12,12,12},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6},
	{3,3,3,3,3,3,3,3,3,3,3,4,5,5,6,7,8,9,9,10,10,11,11,11,11,11,11,11,12,12,12,12},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{12,12,12,12,12,12,12,11,11,11,11,11,11,10,9,9,8,7,6,5,5,4,4,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,5,6,6,7,8,9,10,11,12,13,13,14,14,15,15,15},
	{15,15,15,15,15,14,14,13,13,12,12,11,10,9,8,7,6,5,5,4,4,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,6,6,7,7,8,9,10,11,12,13,14,14,15,15},
	{15,15,15,14,14,13,12,11,10,10,9,8,7,7,6,6,5,5,5,4,4,4,4,3,3,3,3,3,3,3,3,3}
};
static const uint8_t I_RES[4] = {16,32,64,128};
static const uint8_t I_AUDIO_MIN_LENGTH[8] = {2,8,32,64,96,112,120,126};  //COPIED FROM AtxAudioProgmem - I'm sure I'll regret copying this at some point!
static const uint8_t I_ODY_SINE[24] = {7,9,11,13,14,15,15,15,14,13,11,9,7,5,4,2,1,0,0,0,1,2,4,5};
static const char* const S_FILTERTYPE[16] = {"Bypass","LPF","HPF","BPF","Notch","Para 1","Para 2","Para 3","Lo Sh1","Lo Sh2","Hi Sh1","Hi Sh2","But LP","But HP","Bes LP","Bes HP"};
static const char* const S_FMT_NOTES[12] = {"C ","C#","D ","D#","E ","F ","F#","G ","G#","A ","A#","B "};
static const char* const S_FMT_QUANT[16] = {"Off","Chrom","Maj","Hm Min","Nt Min","W Tone","Dim","Penta","Hexa","Hepta","MajTri","MinTri","Dim7th","Aug5th","5th","Oct"};
static const char* const S_ODY_FILT[4] = {"BYPASS","SOFT", "HARSH1", "HARSH2"};
static const char* const S_SET_LSDATA[4] = {"Patches","Zone Setup","ZS + Patches","All Settings"};
static const char* const S_SET_VOICEASSIGN[4] = {"Buffered","Round Robin","Unison","---"};
static const char* const S_SET_CLICKTYPE[4] = {"Off","Count","Record","On"};
static const char* const S_SET_FOLLOWEDITMODE[4] = {"Off","Synth->Zone","FxCard->Zone","Both->Zone"};
static const char* const S_SET_FOLLOWCARDMODE[4] = {"Off","Zone->Synth","Zone->Sy+FxZ","Zone->FxCard"};		
static const char* const S_SET_LFOTRIGGERMODE[4] = {"Free Running","Free Running","Retrig Zone","Retrig Synth"};	
static const char* const S_OFFON[2] = {"OFF","ON"};
	
#endif /* ATXCPRINTDISPPROGMEM_H_ */