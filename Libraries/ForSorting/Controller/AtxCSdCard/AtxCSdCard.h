/*
* AtxCSdCard.h
*
* Created: 04/06/2018 17:48:58
* Author: info
*/


#ifndef __ATXCSDCARD_H__
#define __ATXCSDCARD_H__

#include "SdFat.h"
#include "AtxCSdCardBase.h"
#include "Oled.h"
#include "stk500.h"
#include "Atx.h"
#include "AtxCard.h"
#include "AtxCSettings.h"
#include "AtxCEditor.h"
#include "MidiMsgListStream.h"
#include "MidiMsg.h"
#include "MidiMsgList.h"
//#include "AtxCSequencer.h"
//#include "AtxCHardware.h"
#include <ctype.h>

#define HEXLINELENGTH 50

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

class AtxCSdCard
{
	//variables
	public:
	// readMultiByte() parameters
	enum MultiByte : uint8_t
	{
		MB_LONG = 4,   ///< readMultibyte() parameter specifying expected 4 byte value
		MB_TRYTE = 3,  ///< readMultibyte() parameter specifying expected 3 byte value
		MB_WORD = 2,   ///< readMultibyte() parameter specifying expected 2 byte value
		MB_BYTE = 1   ///< readMultibyte() parameter specifying expected 1 byte value
	};
	enum SdFunction : uint8_t
	{
		SD_LOADPATCH = 0,
		SD_SAVEPATCH,
		SD_LOADMIDI,
		//SD_SAVEMIDI,
		//SD_LOADMIDI,
		//SD_SAVESEQ,
		//SD_MERGESEQ,
		//SD_LOADSEQ,
		//SD_LOADFX,
		//SD_SAVESETS,
		//SD_LOADSETS,
		SD_LOADHEX,
		SD_MAX
	};
	static const uint8_t FILE_NAME_SIZE = 13;
	static const uint8_t FILE_TITLE_SIZE = 9;
	static const uint8_t FILE_EXT_SIZE = 4;
	static const uint8_t FILE_PATH_SIZE = 32; // "/Filepath/Filepath/Filename.ext" + \0
	protected:
	private:
	enum NameEditMode : uint8_t
	{
		NE_OFF = 0,
		NE_FILE,
		NE_FOLDER
	};

	AtxCSdCardBase* base_ = NULL;
	//AtxCard* card_[atx::MAX_CARDS];  //pointers to the cards
	static const uint8_t PIN_SD_CS = ATN;
	static const uint8_t MAX_DIR_ENTRIES = 64;
	static const uint32_t FUNCTION_WRITE_MODE_C = 0b00000010;
	static const char* SETTINGS_FILENAME;
	static const char* SD_FAIL_MSG;
	static const char* SD_OK_MSG;
	static const uint8_t HDR_SIZE = 4;         ///< SMF marker length
	static const char* MTHD;
	static const char* MTRK;
	static const uint8_t MEOT[4];
	
	char* newFileTitle_;
	char* fileExtFilt_;
	char* newFolderTitle_;
	char* dirPath_;
	char* filePath_;
	
	uint8_t nextFreeShortcut_ = 0;
	SdFat SD_;
	File seqFile_[atx::MAX_TRACKS][atx::MAX_SEQS];
	uint8_t fileIndex_ = 0;
	uint16_t dirEntryTable_[MAX_DIR_ENTRIES] = {0};
	uint8_t dirShortcutEntry_[MAX_DIR_ENTRIES] = {0};
	uint8_t dirEntrys_= 0;
	uint8_t nameEditPos_ = 0;
	NameEditMode nameEditMode_ = NE_OFF;
	bool abortClickUi_ = false;
	int16_t hexLeftover_ = 0;
	uint8_t hexLeftoverBuf_[16];
	char hexLineBuffer_[HEXLINELENGTH];
	char hexLineMemBuffer_[16];
	uint8_t hexPageSize_ = 128;
	SdFunction sdFunction_ = SD_SAVEPATCH; //function rot enc on
	//functions
	public:
	static AtxCSdCard& getInstance()
	{
		static AtxCSdCard instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	void construct(AtxCSdCardBase* base);//, AtxCard* cardPtrs[]);
	void initialize();
	//void terminate();
	void setFunction(SdFunction newFunction);
	SdFunction getFunction(){return sdFunction_;}
	void activateFunction();
	//const char* getFilePath(){return filePath_;}
	//uint8_t calcFileNameLen(const char * filename);
	uint8_t calcStatusBar(File* f);
	void incUI(bool buttonState);
	void decUI(bool buttonState);
	void clickUI();
	void midiUI(uint8_t midiNote);
	bool openFileRead(File& f, char* filePath);
	//bool deleteSettings();
	int16_t readHexFilePage(File* f, stk500::AVRMEM* buf);
	void setHexPageSize(uint8_t newSize){hexPageSize_ = newSize;}
	//void createFolderFromEntryName();

	void saveMidiMsgList(midi::MidiMsgList* mml);
	void loadMidiMsgList(midi::MidiMsgList* mml);
	void initMidiStream(uint_fast8_t track, uint_fast8_t seq, midi::MidiMsgListStream& mmListStream);
	void seekStartMidiStream(uint_fast8_t track, uint_fast8_t seq){seqFile_[track][seq].seek(22);}
	void readMidiStream(uint_fast8_t track, uint_fast8_t seq, midi::MidiMsgListStream& mmListStream);
	
	protected:
	private:
	void incNewFileName(bool way);
	void incNewFldName(bool way);
	uint8_t hex2byte(char* code);
	int16_t readHexIntelLine(File* f, uint16_t* address, char* buf);
	void buildDirIndex();
	void processFile();
	void openSeqStream(char* filePath, uint8_t zoneNum);
	void debugSd();
	uint16_t getFunctionFileMode(){return bitRead(FUNCTION_WRITE_MODE_C,sdFunction_) ? FILE_WRITE : FILE_READ;}
	uint8_t calcDirPathDepth();
	void remapMidiToSequence(midi::MidiMsgList& midiList, midi::MidiMsgList& mmList);
	
	void writeFileMultiByte(File* f, uint32_t data, MultiByte nLen);
	uint32_t readFileMultiByte(File* f, MultiByte nLen, uint8_t& byteCount);
	uint32_t readFileMultiByte(File* f, MultiByte nLen){uint8_t dummy; return readFileMultiByte(f,nLen,dummy);}
	void writeFileVarLen(File* f, uint32_t data);
	uint32_t readFileVarLen(File* f, uint8_t& byteCount);
	uint32_t readFileVarLen(File* f){uint8_t dummy; return readFileVarLen(f,dummy);}
	
	inline uint8_t readFileSingleByte(File* f, uint8_t& byteCount)
	{
		byteCount++;
		return f->read();
	}
	uint8_t readFileSingleByte(File* f){uint8_t dummy; return readFileSingleByte(f,dummy);}
	void setFileIndex(uint8_t index);
	void createFolder(char* fldName);
	void createFile(char* fileTitle, char* fileExt);
	void refreshNameEdit();
	void setFileExtFilt(const char* newExt){strncpy(fileExtFilt_,newExt,FILE_EXT_SIZE);*(fileExtFilt_ + 3) = '\0';}
	const char* getFileNameExtension(const char* filename);
	void calcFilePathFileTitle(char* fileTitle, char* filePath);



	//void loadPatch(char* filePath);
	

	//void loadSeq(char* filePath, AtxSeqMsgListStream& amls);
	//void loadSeq(AtxSeqMsgListStream& amls){loadSeq(filePath_,amls);}
	void saveSeq(char* filePath, midi::MidiMsgList& mmList);
	void saveSeq(midi::MidiMsgList& mmList){saveSeq(filePath_,mmList);}
	void createSeq(uint8_t zone, uint8_t seq, midi::MidiMsgList& mmList);
	void loadSeq(char* filePath, midi::MidiMsgList& mmList);

	//void seekSeq(uint8_t zone, uint8_t seq, uint16_t index){seqFile_[zone][seq].seekSet((index * sizeof(midi::MidiMsg)) + 9);}  //9 byte header in seq file   - I don't think that'll be that easy any more....
	//void readSeq(uint8_t zone, uint8_t seq, midi::MidiMsgListStream& mmListStream);
	void loadFx(char* filePath);
	void loadFx(){loadFx(filePath_);}
	bool loadSettings();
	void saveSettings(uint16_t* settings);


	//bool isNumeric(char c){(c>=48 && c<=57) ? return true : return false;}
	AtxCSdCard() {}
	AtxCSdCard(AtxCSdCardBase* base);//, AtxCard* cardPtrs[]);
	~AtxCSdCard();
	AtxCSdCard( const AtxCSdCard &c );
	AtxCSdCard& operator=( const AtxCSdCard &c );
}; //AtxCSdCard

#endif //__ATXCSDCARD_H__
