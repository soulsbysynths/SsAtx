/*
* AtxCSdCard.cpp
*
* Created: 04/06/2018 17:48:57
* Author: info
*/


#include "AtxCSdCard.h"

const char* AtxCSdCard::SETTINGS_FILENAME = "/FIRMWARE/SETTINGS.SET";
const char* AtxCSdCard::SD_OK_MSG =   "CARD  FOUND ";
const char* AtxCSdCard::SD_FAIL_MSG = "NO SD CARD  ";
const char* AtxCSdCard::MTHD = "MThd";
const char* AtxCSdCard::MTRK = "MTrk";
const uint8_t AtxCSdCard::MEOT[4] = {0x00, 0xFF, 0x2F, 0x00};
// default constructor
AtxCSdCard::AtxCSdCard(AtxCSdCardBase* base)//, AtxCard* cardPtrs[])
{
	construct(base);//,cardPtrs);
} //AtxCSdCard

// default destructor
AtxCSdCard::~AtxCSdCard()
{
	if(base_!=NULL)
	{
		delete base_;
	}
	free(newFileTitle_);
	free(fileExtFilt_);
	free(newFolderTitle_);
	free(dirPath_);
	free(filePath_);
} //~AtxCSdCard

void AtxCSdCard::construct(AtxCSdCardBase* base)//, AtxCard* cardPtrs[])
{
	base_ = base;
	//for (uint_fast8_t i=0;i<atx::MAX_CARDS;++i)
	//{
	//card_[i] = cardPtrs[i];
	//}
	dirPath_ = (char *)malloc(FILE_PATH_SIZE);
	filePath_ = (char *)malloc(FILE_PATH_SIZE);
	fileExtFilt_ = (char *)malloc(FILE_EXT_SIZE);
	newFileTitle_ = (char *)malloc(FILE_TITLE_SIZE);
	strncpy(newFileTitle_,"00_NEW_F",FILE_TITLE_SIZE);
	newFolderTitle_ = (char *)malloc(FILE_TITLE_SIZE);
	strncpy(newFolderTitle_,"NEW_FLDR",FILE_TITLE_SIZE);
}

void AtxCSdCard::initialize()
{
	#ifndef TESTER
	while (!SD_.begin(PIN_SD_CS))
	{
		base_->sdPrintFail(SD_FAIL_MSG);
	}
	base_->sdPrintFail(SD_OK_MSG);
	#endif
}

//void AtxCSdCard::terminate()
//{
//for (uint8_t i=0;i<atx::MAX_ZONES;++i)
//{
//for (uint8_t j=0;j<atx::MAX_SEQS;++j)
//{
//seqFile_[i][j].close();
//}
//}
//initialize();
//}

void AtxCSdCard::buildDirIndex()
{
	using namespace atx;
	uint8_t dirEntryShortcut[MAX_DIR_ENTRIES];
	File f, d;
	dirEntrys_ = 1;  //0 = root
	d = SD_.open(dirPath_);
	if (!d)
	{
		base_->sdPrintFail(SD_FAIL_MSG);
		return;
	}
	memset(dirEntryShortcut,UNUSED,MAX_DIR_ENTRIES);
	memset(dirShortcutEntry_,UNUSED,MAX_DIR_ENTRIES);
	uint16_t ind;
	char* ptrSortTable[MAX_DIR_ENTRIES];
	for (uint8_t i=0;i<MAX_DIR_ENTRIES;i++)
	{
		ptrSortTable[i] = (char *)malloc(FILE_TITLE_SIZE);
	}
	for (uint8_t i=0;i<FILE_TITLE_SIZE;++i)
	{
		ptrSortTable[0][i] = 0;
	}
	char* fn;
	fn = (char *)malloc(FILE_NAME_SIZE);
	uint8_t dirDepth = calcDirPathDepth();
	while(f.openNext(&d,O_RDONLY))
	{
		ind =  f.dirIndex();
		f.getSFN(fn, FILE_NAME_SIZE);
		for(uint8_t i=0;i<FILE_NAME_SIZE;++i)
		{
			*(fn + i) = toupper(*(fn + i));
		}
		int16_t c = strcmp(fileExtFilt_,getFileNameExtension(fn));
		if (((f.isDirectory() && !f.isSystem() && dirDepth<3) || (!f.isDirectory() && c==0)) && dirEntrys_<MAX_DIR_ENTRIES)
		{
			dirEntryTable_[dirEntrys_] = ind;
			//memcpy(ptrSortTable[dirEntrys_],&fn,8);
			strncpy(ptrSortTable[dirEntrys_],fn,FILE_TITLE_SIZE);
			ptrSortTable[dirEntrys_][8] = '\0';  //force terminator
			char* ePtr;
			int32_t sc = strtol(fn,&ePtr,10);
			if (sc<MAX_DIR_ENTRIES)
			{
				dirEntryShortcut[dirEntrys_] = sc;
			}
			dirEntrys_++;
		}
		f.close();
	}
	free(fn);
	d.close();
	// sort the array
	char* sTmp;
	uint16_t iTmp;
	for(uint8_t i=0;i<(dirEntrys_-1);i++)  //1 - leave top entry alone
	{
		for(uint8_t j=i+1;j<dirEntrys_;j++)
		{
			//if(*ptrSortTable[j-1]>*ptrSortTable[j])
			if(strcmp(ptrSortTable[i],ptrSortTable[j])>0)
			{
				sTmp = ptrSortTable[i];
				ptrSortTable[i] = ptrSortTable[j];
				ptrSortTable[j] = sTmp;
				iTmp = dirEntryTable_[i];
				dirEntryTable_[i] = dirEntryTable_[j];
				dirEntryTable_[j] = iTmp;
				iTmp = dirEntryShortcut[i];
				dirEntryShortcut[i] = dirEntryShortcut[j];
				dirEntryShortcut[j] = iTmp;
			}
		}
	}
	for (uint8_t i=0;i<MAX_DIR_ENTRIES;++i)
	{
		if (dirEntryShortcut[i]!=UNUSED)
		{
			dirShortcutEntry_[dirEntryShortcut[i]] = i;
		}
	}
	nextFreeShortcut_ = 0;

	while (nextFreeShortcut_<MAX_DIR_ENTRIES && dirShortcutEntry_[nextFreeShortcut_]!=UNUSED)
	{
		nextFreeShortcut_++;
	}
	for (uint8_t i=0;i<MAX_DIR_ENTRIES;i++)
	{
		free(ptrSortTable[i]);
	}
}



void AtxCSdCard::setFileIndex(uint8_t index)
{
	fileIndex_ = index;
	if (fileIndex_==0) //back or root
	{
		if (strlen(dirPath_)==1)  //root
		{
			base_->sdPrintString("SD    Card",Oled::FONT_SDCARD);
		}
		else  //back
		{
			char * i = strrchr(dirPath_, '/');
			*(strrchr(dirPath_, '/')) = 0;
			char * p = strrchr(dirPath_, '/');
			base_->sdPrintString(p+1,Oled::FONT_BACK);
			strncpy(i, "/\0", 2);
		}
	}
	else
	{
		File f;
		File d = SD_.open(dirPath_);
		if(d)
		{
			f.open(&d,dirEntryTable_[fileIndex_],O_RDONLY);
			if (f)
			{
				char fn[FILE_NAME_SIZE] = {0};
				f.getSFN(fn, FILE_NAME_SIZE);
				strncpy(filePath_,dirPath_,FILE_PATH_SIZE);
				strcat(filePath_,fn);
				if (f.isDirectory())
				{
					strcat(filePath_,"/");
					base_->sdPrintString(fn,Oled::FONT_FOLDER);
				}
				else
				{
					base_->sdPrintString(fn);
				}
			}
			else
			{
				base_->sdPrintString(SD_FAIL_MSG);
			}
		}
		else
		{
			base_->sdPrintString(SD_FAIL_MSG);
		}
		f.close();
		d.close();
	}
}

uint8_t AtxCSdCard::calcDirPathDepth()
{
	uint8_t i = 0, cnt = 0;
	while(i<FILE_PATH_SIZE && *(dirPath_ + i)!='\0')
	{
		if (*(dirPath_ + i)=='/')
		{
			cnt++;
		}
		i++;
	}
	return cnt;
}

void AtxCSdCard::incUI(bool buttonState)
{
	if (nameEditMode_==NE_OFF)
	{
		if (fileIndex_<(dirEntrys_-1))
		{
			setFileIndex(fileIndex_+1);
		}
		else
		{
			if (getFunctionFileMode()==FILE_WRITE)
			{
				nameEditPos_ = 0;
				nameEditMode_ = NE_FILE;
				refreshNameEdit();
			}
		}
	}
	else
	{
		if (nameEditMode_==NE_FILE && nameEditPos_==9)
		{
			nameEditPos_ = 0;
			nameEditMode_ = NE_FOLDER;
			refreshNameEdit();
		}
		else if(nameEditPos_<9)
		{
			if (buttonState)
			{
				if (nameEditMode_==NE_FILE)
				{
					incNewFileName(true);
				}
				else
				{
					incNewFldName(true);
				}
				abortClickUi_ = true;
			}
			else
			{
				nameEditPos_++;
				if (nameEditPos_==8)
				{
					nameEditPos_++;
				}
				refreshNameEdit();
			}
		}
	}
	#ifdef DEBUGSD
	debugSd();
	#endif
}
void AtxCSdCard::decUI(bool buttonState)
{
	if (nameEditMode_==NE_OFF)
	{
		if (fileIndex_>0)
		{
			setFileIndex(fileIndex_-1);
		}
	}
	else if(nameEditPos_==0)
	{
		if (nameEditMode_==NE_FILE)
		{
			if (buttonState)
			{
				incNewFileName(false);
				abortClickUi_ = true;
			}
			else
			{
				nameEditMode_ = NE_OFF;
				base_->sdHighlightChar(0xFF);
				setFileIndex(fileIndex_);
			}
		}
		else
		{
			if (buttonState)
			{
				incNewFldName(false);
				abortClickUi_ = true;
			}
			else
			{
				nameEditMode_ = NE_FILE;
				nameEditPos_ = 9;
				refreshNameEdit();
			}

		}
	}
	else
	{
		if (buttonState)
		{
			if (nameEditMode_==NE_FILE)
			{
				incNewFileName(false);
			}
			else
			{
				incNewFldName(false);
			}
			abortClickUi_ = true;
		}
		else
		{
			nameEditPos_--;
			if (nameEditPos_==8)
			{
				nameEditPos_--;
			}
			refreshNameEdit();
		}
	}
	#ifdef DEBUGSD
	debugSd();
	#endif
}

void AtxCSdCard::midiUI(uint8_t midiNote)
{
	using namespace atx;
	if (nameEditMode_==NE_OFF)
	{
		if (midiNote==72)
		{
			clickUI();
		}
		else if(midiNote<64)
		{
			if (dirShortcutEntry_[midiNote]!=UNUSED)
			{
				setFileIndex(dirShortcutEntry_[midiNote]);
				clickUI();
			}
			//setFileIndex(0);
			//for (uint_fast8_t i = 0;i < (midiNote-36);++i)
			//{
			//incUI(false);
			//if (fileMode_==FILE_WRITE && nameEditMode_==NE_FILE)
			//{
			//decUI(false);
			//break;
			//}
			//}
		}
	}
	else
	{
		if (nameEditPos_==9)
		{
			if (midiNote==72)
			{
				clickUI();
			}
		}
		else if(nameEditPos_<2)
		{
			if (midiNote<MAX_DIR_ENTRIES)
			{
				uint8_t iSc = midiNote % 100;
				char sSc[2];
				if (iSc<10)
				{
					sSc[0] = '0';
					sSc[1] = iSc + '0';
				}
				else
				{
					sSc[0] = (iSc/10) + '0';
					sSc[1] = (iSc%10) + '0';
				}
				switch (nameEditMode_)
				{
					case NE_FOLDER:
					{
						memcpy(newFolderTitle_,&sSc,2);
					}
					break;
					case NE_FILE:
					{
						memcpy(newFileTitle_,&sSc,2);
					}
					break;
				}
				nameEditPos_ = 2;
				refreshNameEdit();
			}
		}
		else
		{
			char c = 0;
			if (midiNote>=36 && midiNote<=61)
			{
				c = midiNote + 29;
			}
			else if(midiNote>=62 && midiNote<=71)
			{
				c = midiNote - 14;
			}
			else if(midiNote==72)
			{
				c = ' ';
			}
			if (c>0)
			{
				switch (nameEditMode_)
				{
					case NE_FOLDER:
					{
						newFolderTitle_[nameEditPos_] = c;
					}
					break;
					case NE_FILE:
					{
						newFileTitle_[nameEditPos_] = c;
					}
					break;
				}
				incUI(false);
			}
		}
	}
	#ifdef DEBUGSD
	debugSd();
	#endif
}

void AtxCSdCard::clickUI()
{
	switch (nameEditMode_)
	{
		case NE_OFF:
		{
			//strcat (dirPath_,"/");
			File f = SD_.open(filePath_);
			if (fileIndex_==0)
			{
				f.close();
				if (strlen(dirPath_)>1)
				{
					*(strrchr(dirPath_, '/')) = 0;  //end one
					*(strrchr(dirPath_, '/')+1) = 0;  //parent
					buildDirIndex();
					setFileIndex(0);
				}
			}
			else if (f.isDirectory())
			{
				f.close();
				strncpy(dirPath_,filePath_,FILE_PATH_SIZE);
				buildDirIndex();
				setFileIndex(0);
			}
			else if(f)
			{
				f.close();
				//*(strrchr(dirPath_, '/')) = 0;  // why would you want to chop off the end /?
				if(getFunctionFileMode() == FILE_WRITE)
				{
					SD_.remove(filePath_);
				}
				activateFunction();
			}
		}
		break;
		case NE_FOLDER:
		{
			if (nameEditPos_==9)
			{
				createFolder(newFolderTitle_);
			}
			else
			{
				if (abortClickUi_)
				{
					abortClickUi_ = false;
				}
				else
				{
					incNewFldName(true);
				}
			}
		}
		break;
		case NE_FILE:
		{
			if (nameEditPos_==9)
			{
				createFile(newFileTitle_,fileExtFilt_);
				activateFunction();
			}
			else
			{
				if (abortClickUi_)
				{
					abortClickUi_ = false;
				}
				else
				{
					incNewFileName(true);
				}
			}
		}
		break;
	}
	#ifdef DEBUGSD
	debugSd();
	#endif

}

void AtxCSdCard::createFile(char* fileTitle, char* fileExt)
{
	strncpy(filePath_,dirPath_,FILE_PATH_SIZE);
	strcat (filePath_,fileTitle);
	strcat (filePath_,".");
	strcat (filePath_,fileExt);
	if (SD_.exists(filePath_))
	{
		SD_.remove(filePath_);
	}
	File f = SD_.open(filePath_,FILE_WRITE);
	f.close();
	buildDirIndex();
}

void AtxCSdCard::createFolder(char * fldName)
{
	strcat (dirPath_,fldName);
	strcat (dirPath_,"/");
	if (!SD_.exists(dirPath_))
	{
		SD_.mkdir(dirPath_);
	}
	buildDirIndex();
	setFileIndex(0);
}

void AtxCSdCard::incNewFileName(bool way)
{
	if (way)
	{
		newFileTitle_[nameEditPos_]++;
		if (newFileTitle_[nameEditPos_]>95)
		{
			newFileTitle_[nameEditPos_] = 32;
		}
	}
	else
	{
		newFileTitle_[nameEditPos_]--;
		if (newFileTitle_[nameEditPos_]<32)
		{
			newFileTitle_[nameEditPos_] = 95;
		}
	}
	refreshNameEdit();
}

void AtxCSdCard::incNewFldName(bool way)
{
	if (way)
	{
		newFolderTitle_[nameEditPos_]++;
		if (newFolderTitle_[nameEditPos_]>95)
		{
			newFolderTitle_[nameEditPos_] = 32;
		}
		
	}
	else
	{
		newFolderTitle_[nameEditPos_]--;
		if (newFolderTitle_[nameEditPos_]<32)
		{
			newFolderTitle_[nameEditPos_] = 95;
		}
	}
	refreshNameEdit();
}

const char* AtxCSdCard::getFileNameExtension(const char * filename)
{
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";
	return dot + 1;
}


//uint8_t AtxCSdCard::calcFileNameLen(const char* filename)
//{
//const char* dot = strrchr(filename, '.');
//if (!dot || dot == filename) return 0;
//return (dot - filename);
//}

void AtxCSdCard::calcFilePathFileTitle(char* fileTitle, char* filePath)
{
	const char *s = strrchr(filePath, '/');
	const char* dot = strrchr(filePath, '.');
	if (!dot || !s) return;
	uint8_t l = dot - s - 1;
	if (l<FILE_TITLE_SIZE)
	{
		strncpy(fileTitle, s+1, l);
		*(fileTitle+l) = '\0';
	}
	else
	{
		strncpy(fileTitle, s+1, (FILE_TITLE_SIZE-1));
		*(fileTitle+FILE_TITLE_SIZE-1) = '\0';
	}
}

//void AtxCSdCard::createFolderFromEntryName()
//{
//char fld[9];
//copyEntryName(fld,getEntryNameTitleLen());
//createFolder(fld);
//}
//void AtxCSdCard::copyEntryName(char * dest, size_t len)
//{
//if (len>13)
//{
//len = 13;
//}
//for (uint_fast8_t i=0;i<len;++i)
//{
//dest[i] = fileName_[i];
//}
//}

void AtxCSdCard::refreshNameEdit()
{
	char s[11];
	char i;
	if (nameEditMode_==NE_FILE)
	{
		strncpy(s,newFileTitle_,FILE_TITLE_SIZE);
		i = Oled::FONT_NEWFILE;
		
	}
	else if(nameEditMode_==NE_FOLDER)
	{
		strncpy(s,newFolderTitle_,FILE_TITLE_SIZE);
		i = Oled::FONT_NEWFOLDER;
	}
	s[8] = ' ';
	s[9] = Oled::FONT_OK;
	s[10] = '\0';
	base_->sdHighlightChar(nameEditPos_);
	base_->sdPrintString(s,i);
}

//void AtxCSdCard::saveComplete()
//{
//COME BACK!!!!
//int8_t c;
//uint16_t ind = file_.dirIndex();
//file_.close();
//buildDirIndex();
//fileIndex_ = 0;
//for (uint16_t i=0;i<dirEntrys_;++i)
//{
//if (dirEntryTable_[i]==ind)
//{
//fileIndex_ = i;
//break;
//}
//}
//setFileIndex(fileIndex_);
////dir_.rewindDirectory();
////fileIndex_ = 0;
////do
////{
////file_.close();
////file_ = dir_.openNextFile();
////fileIndex_++;
////char entryName[ENTRY_NAME_SIZE];
////file_.getName(entryName,ENTRY_NAME_SIZE);
////c = strcmp(entryName,fileName_);
////
////} while (file_ && c!=0);
//if (file_)
//{
//base_->sdHighlightChar(0xFF);
//char fn[FILE_NAME_SIZE];
//file_.getName(fn,FILE_NAME_SIZE);
//base_->sdPrintString(fn);
//file_.close();
//nameEditMode_ = NE_OFF;
//nameEditPos_ = 0;
//}
//}

bool AtxCSdCard::openFileRead(File& f, char * filePath)
{
	//bool res = hexfile_.open(&dir_,dirEntryTable_[fileIndex_],O_RDONLY);
	f = SD_.open(filePath,FILE_READ);
	if (f)
	{
		return true;
	}
	else
	{
		return false;
	}
	//return res;
}

int16_t AtxCSdCard::readHexFilePage(File* f, stk500::AVRMEM* buf)
{
	int16_t len;
	uint16_t address;
	int16_t total_len = hexLeftover_;
	if (hexLeftover_ > 0)
	{
		memcpy(buf->buf,hexLeftoverBuf_,hexLeftover_);
	}
	// grab hexPageSize_ bytes or less (one page)
	int16_t i = 0;
	while (total_len < hexPageSize_)
	{
		len = readHexIntelLine(f, &address, &hexLineMemBuffer_[0]);
		if (len < 0)
		{
			if (i==0) total_len = len;
			break;
		}
		//    else
		//      total_len=total_len+len;
		if (i==0)// first record determines the page address
		{
			buf->pageaddress = address - hexLeftover_;
			hexLeftover_ = 0;
		}
		i++;
		//    memcpy((buf->buf)+(i*16), hexLineMemBuffer_, len);
		if ((total_len+len) > hexPageSize_)
		{
			hexLeftover_ = (total_len+len)-hexPageSize_;
			memcpy(buf->buf+total_len, hexLineMemBuffer_,hexPageSize_-total_len);
			int kk = 0;
			for (int k =(hexPageSize_-total_len);k<len;k++)
			hexLeftoverBuf_[kk++] = hexLineMemBuffer_[k];
			hexLeftover_ = kk;
			total_len = hexPageSize_;
		}
		else
		{
			memcpy((buf->buf)+total_len, hexLineMemBuffer_, len);
			total_len=total_len+len;
		}
	}
	
	buf->size = total_len;
	return total_len;
}

// read one line of intel hex from file. Return the number of databytes
// Since the arduino code is always sequential, ignore the address for now.
// If you want to burn bootloaders, etc. we'll have to modify to
// return the address.

// INTEL HEX FORMAT:
// :<8-bit record size><16bit address><8bit record type><data...><8bit checksum>
int16_t AtxCSdCard::readHexIntelLine(File* f, uint16_t* address, char* buf)
{
	char c;
	int16_t i = 0;
	while (true)
	{
		if (f->available())
		//if (file_.available())
		{
			c = f->read();
			//c = file_.read();
			//DEBUGP(c);
			// this should handle unix or ms-dos line endings.
			// break out when you reach either, then check
			// for lf in stream to discard
			if ((c == 0x0d)|| (c == 0x0a))
			{
				break;
			}
			else
			{
				hexLineBuffer_[i++] = c;
			}
		}
		else
		{
			return -1; //end of file
		}
	}
	hexLineBuffer_[i]= 0; // terminate the string
	//peek at the next byte and discard if line ending char.
	if (f->peek() == 0xa) f->read();
	//if (file_.peek() == 0xa) file_.read();
	int16_t len = hex2byte(&hexLineBuffer_[1]);
	*address = (hex2byte(&hexLineBuffer_[3]) <<8) | (hex2byte(&hexLineBuffer_[5]));
	int16_t j=0;
	for (int16_t i = 9; i < ((len*2)+9); i +=2)
	{
		buf[j] = hex2byte(&hexLineBuffer_[i]);
		j++;
	}
	return len;
}
uint8_t AtxCSdCard::hex2byte(char* code)
{
	uint8_t result =0;

	if ((code[0] >= '0') && (code[0] <='9'))
	{
		result = ((int16_t)code[0] - '0') << 4;
	}
	else if ((code[0] >='A') && (code[0] <= 'F'))
	{
		result = ((int16_t)code[0] - 'A'+10) << 4;
	}
	if ((code[1] >= '0') && (code[1] <='9'))
	{
		result |= ((int16_t)code[1] - '0');
	}
	else if((code[1] >='A') && (code[1] <= 'F'))
	{
		result |= ((int16_t)code[1] -'A'+10);
	}
	return result;
}

uint32_t AtxCSdCard::readFileVarLen(File* f, uint8_t& byteCount)
// read variable length parameter from input
{
	if (f==NULL)
	{
		return 0;
	}
	uint32_t value = 0;
	uint8_t c;
	do
	{
		c = f->read();
		byteCount++;
		value = (value << 7) + (c & 0x7f);
	}  while (c & 0x80);
	
	return(value);
}

void AtxCSdCard::writeFileVarLen(File* f, uint32_t data)
{
	uint8_t bytes[4] = {0};

	bytes[0] = (uint8_t)((data >> 21) & 0x7f);  // most significant 7 bits
	bytes[1] = (uint8_t)((data >> 14) & 0x7f);
	bytes[2] = (uint8_t)((data >> 7)  & 0x7f);
	bytes[3] = (uint8_t)((data)       & 0x7f);  // least significant 7 bits

	uint_fast8_t start = 0;
	while ((start<4) && (bytes[start] == 0))  start++;

	for (uint_fast8_t i=start;i<3;i++)
	{
		bytes[i] = bytes[i] | 0x80;
		f->write(bytes[i]);
	}
	f->write(bytes[3]);
}

uint32_t AtxCSdCard::readFileMultiByte(File* f, MultiByte nLen, uint8_t& byteCount)
// read fixed length parameter from input
{
	if (f==NULL)
	{
		return 0;
	}
	uint32_t  value = 0L;
	
	for (uint8_t i=0; i<(uint8_t)nLen; i++)
	{
		value = (value << 8) + f->read();
		byteCount++;
	}
	
	return(value);
}

void AtxCSdCard::writeFileMultiByte(File * f, uint32_t data, MultiByte nLen)
{
	if (f==NULL)
	{
		return;
	}
	uint8_t value;
	for (uint8_t i=0; i<(uint8_t)nLen; i++)
	{
		value = (data >> (((uint8_t)nLen - 1 - i) << 3)) & 0xFF; //value = byte, msb first
		f->write(value);
	}
}



uint8_t AtxCSdCard::calcStatusBar(File * f)
{
	if (f==NULL)
	{
		return 0;
	}
	if (f->size()==0)
	{
		return 0;
	}
	else
	{
		uint32_t sb = (f->position() << 8) / f->size();
		if (sb > 255)
		{
			return 255;
		}
		else
		{
			return (uint8_t)sb;
		}
	}
}

void AtxCSdCard::setFunction(SdFunction newFunction)
{
	uint16_t m = 0;
	bool initUI = true;
	setFileExtFilt("MID");
	switch(newFunction)
	{
		case SD_LOADPATCH:
		case SD_SAVEPATCH:
		//DO NOTHING THIS MUST BE DONE PRIOR TO CALLING THIS FUNC
		break;
		//case SD_LOADMIDI:
		//case SD_SAVEMIDI:
		//
		//break;
		//
		//case SD_LOADSETS:
		//case SD_SAVESETS:
		////setFileExtFilt("SET");  //tricky, coz could be patch
		////initUI = false;
		//break;
		//case SD_LOADSEQ:
		//case SD_SAVESEQ:
		//case SD_MERGESEQ:
		////setFileExtFilt("XSQ");
		//break;
		//case SD_LOADFX:
		//setFileExtFilt("HEX");
		//break;
		//case SD_LOADHEX:
		//setFileExtFilt("HEX");
		//initUI =false;
		//break;
	}
	strncpy(dirPath_, "/",FILE_PATH_SIZE);
	strncpy(filePath_, "/",FILE_PATH_SIZE);
	buildDirIndex();
	if(initUI)
	{
		if (nameEditMode_==NE_OFF)
		{
			setFileIndex(0);
		}
		else
		{
			refreshNameEdit();
		}
	}
	#ifdef DEBUGSD
	debugSd();
	#endif
	sdFunction_ = newFunction;
	base_->sdFunctionChanged(newFunction);
}

void AtxCSdCard::activateFunction()
{
	switch(sdFunction_)
	{
		//case AtxCSdCard::SD_LOADPATCH:
		//loadPatch(destination_);
		//break;
		//case AtxCSdCard::SD_SAVEPATCH:
		//savePatch(destination_);
		//break;
		
		
		//case AtxCSdCard::SD_LOADSETS:
		//loadSettings();
		//break;
		//
		//case AtxCSdCard::SD_LOADMIDI:
		//convertMidi();
		//break;
		//case AtxCSdCard::SD_LOADSEQ:
		////initSeq(sequencer_.getEditZone(),sequencer_.getEditSeq(),sequencer_.getSequence());
		////sequencer_.refreshDisplayElement(AtxCSequencer::RDE_NAME);
		////printFreeMem();
		//break;
		//case AtxCSdCard::SD_LOADFX:
		////clkTimer_.enable(false);
		//loadFx();
		////sequencerBpmChanged(sequencer_.getBpm());
		//break;
		//
		//case AtxCSdCard::SD_SAVESETS:
		////sdCard_.saveSettings(settings_.getCcValuePtr());
		//break;
		//case AtxCSdCard::SD_MERGESEQ:
		////sequencer_.mergeSeq();
		////sdCard_.saveSeq(sequencer_.getBuffer());
		////sequencer_.clearBuffer();
		////sdCard_.initSeq(sequencer_.getEditZone(),sequencer_.getEditSeq(),sequencer_.getSequence());
		////sequencer_.refreshDisplayElement(AtxCSequencer::RDE_NAME);
		//break;
		//case AtxCSdCard::SD_SAVESEQ:
		////sdCard_.saveSeq(sequencer_.getBuffer());
		////sequencer_.clearBuffer();
		////sdCard_.initSeq(sequencer_.getEditZone(),sequencer_.getEditSeq(),sequencer_.getSequence());
		////sequencer_.refreshDisplayElement(AtxCSequencer::RDE_NAME);
		//
		//break;
		//case AtxCSdCard::SD_SAVEMIDI:
		//break;
	}
	base_->sdFunctionActivated(sdFunction_);
}

void AtxCSdCard::saveMidiMsgList(midi::MidiMsgList* mml)
{
	using namespace midi;
	if (SD_.exists(filePath_))
	{
		SD_.remove(filePath_);
	}
	File f = SD_.open(filePath_,FILE_WRITE);
	//0:MThd
	f.write(MTHD,HDR_SIZE);
	//1:Header Length
	writeFileMultiByte(&f,6,MB_LONG);
	//2:Format
	writeFileMultiByte(&f,0,MB_WORD);
	//3:Tracks
	writeFileMultiByte(&f,1,MB_WORD);
	//4:PPQN
	writeFileMultiByte(&f,24,MB_WORD);
	//5:MTrk
	f.write(MTRK,HDR_SIZE);
	//6:Length
	writeFileMultiByte(&f,mml->getSize(),MB_LONG);
	uint_fast8_t runningStatus = 0;
	for(uint32_t i=0;i<mml->getCount();++i)
	{
		uint32_t delta;
		if(i==0)
		{
			delta = mml->getEvent(i).getClk();
		}
		else
		{
			delta = mml->getEvent(i).getClk()-mml->getEvent(i-1).getClk();
		}
		writeFileVarLen(&f,delta);
		if(mml->getEvent(i).getStatus()!=runningStatus)  //deal with running status
		{
			f.write(mml->getEvent(i).getStatus());
			runningStatus = mml->getEvent(i).getStatus();
		}
		if(mml->getEvent(i).getMsgSize()>1)
		{
			f.write(mml->getEvent(i).getData1());
			if(mml->getEvent(i).getMsgSize()>2)
			{
				f.write(mml->getEvent(i).getData2());
			}
		}
	}
	//End of track meta message
	f.write(MEOT,HDR_SIZE);
	f.close();
}

void AtxCSdCard::loadMidiMsgList(midi::MidiMsgList* mml)
{
	using namespace atx;
	using namespace midi;
	
	char h[HDR_SIZE]; // Header characters + nul
	File f = SD_.open(filePath_,FILE_READ);

	//0:MThd
	f.read(h,HDR_SIZE);
	if (strncmp(h, MTHD,HDR_SIZE) != 0)
	{
		base_->sdPrintFail("MIDerr");
		f.close();
		return;
	}
	//1:Header Length
	uint32_t len = readFileMultiByte(&f, MB_LONG);
	if (len != 6)   // must be 6 for this header
	{
		base_->sdPrintFail("HDRerr");
		f.close();
		return;
	}
	//2:Format
	uint16_t i = readFileMultiByte(&f, MB_WORD);
	if (i != 0)// && (dat16 != 1))
	{
		base_->sdPrintFail("FMTerr");
		f.close();
		return;
	}
	//3:Tracks
	i = readFileMultiByte(&f, MB_WORD);  //don't do anything with this one
	//4::PPQN
	i = readFileMultiByte(&f, MB_WORD);
	// read ticks per quarter note
	if (i & 0x8000) // top bit set is SMTE format
	{
		uint16_t framespersecond = (i >> 8) & 0x00ff;
		uint16_t resolution      = i & 0x00ff;

		switch (framespersecond)
		{
			case 232:  framespersecond = 24; break;
			case 231:  framespersecond = 25; break;
			case 227:  framespersecond = 29; break;
			case 226:  framespersecond = 30; break;
			default:
			base_->sdPrintFail("TPQerr");
			f.close();
			return;
			break;
		}
		i = framespersecond * resolution;
	}
	mml->setClksPerQuarterNote(i);
	//5:MTrk
	//strncpy(hdr,"MTrk",HDR_SIZE);
	f.read(h,HDR_SIZE);
	if (strncmp(h, MTRK,HDR_SIZE) != 0)
	{
		base_->sdPrintFail("MIDerr");
		f.close();
		return;
	}
	//6:Length
	len = readFileMultiByte(&f,MB_LONG);
	mml->clear();  //first import byte here, so may as well clear old midi list down
	//mml.setSize(len);
	uint32_t clkCount = 0;
	while (1)
	{
		MidiSeqMsg mmsg;
		uint32_t delta = readFileVarLen(&f);
		clkCount += delta;
		mmsg.setClk(clkCount);
		uint8_t status = f.read();
		switch(status)
		{
			case 0x80 ... 0xef:	// MIDI message
			{
				mmsg.setStatus(status);
				mmsg.setData1(f.read());
				if (MidiMsg::getMsgSize(status)==3)
				{
					mmsg.setData2(f.read());
				}
				mml->append(mmsg);
			}
			break;
			case 0x00 ... 0x7f:	// MIDI run on message
			{
				mmsg.setStatus(mml->getLastEvent().getStatus());
				mmsg.setData1(status);
				if (mmsg.getMsgSize()==3)
				{
					mmsg.setData2(f.read());
				}
				mml->append(mmsg);
			}
			break;
			// ---------------------------- SYSEX
			case 0xf0:  // sysex_event = 0xF0 + <len:1> + <data_bytes> + 0xF7
			case 0xf7:  // sysex_event = 0xF7 + <len:1> + <data_bytes> + 0xF7
			{
				uint8_t len = readFileVarLen(&f);
				for (uint_fast8_t i=0;i<len;++i)
				{
					f.read();
				}
				//don't do anything with sysex
			}
			break;
			case 0xff:  // meta_event = 0xFF + <meta_type:1> + <length:v> + <event_data_bytes>
			{
				uint8_t mType = f.read();
				uint8_t len = readFileVarLen(&f);
				switch(mType)
				{
					case 0x2f:  // End of track
					//char fn[FILE_NAME_SIZE];
					//f.getSFN(fn);
					//f.close();
					//AtxCSequencer::getInstance().remapMidiToSequence(fn);
					//remapMidiToSequence(mml, mmList);
					//saveSeq(savePath, mmList);
					f.close();
					return;
					break;
					default:
					for (uint_fast8_t i=0;i<len;++i)
					{
						f.read();
					}
					//don't do anything with meta
					break;
				}
			}
			break;
		}
	}
}

void AtxCSdCard::initMidiStream(uint_fast8_t track, uint_fast8_t seq, midi::MidiMsgListStream& mmls)
{
	using namespace atx;
	using namespace midi;
	
	mmls.fileOpen = false;
	char ft[FILE_TITLE_SIZE];
	calcFilePathFileTitle(ft,filePath_);
	mmls.setName(ft);
	mmls.looping = false;
	
	File* f = &seqFile_[track][seq];
	f->close();
	char h[HDR_SIZE]; // Header characters + nul
	seqFile_[track][seq] = SD_.open(filePath_,FILE_READ);

	//0:MThd
	f->read(h,HDR_SIZE);
	if (strncmp(h, MTHD,HDR_SIZE) != 0)
	{
		base_->sdPrintFail("MIDerr");
		f->close();
		return;
	}
	//1:Header Length
	uint32_t len = readFileMultiByte(f, MB_LONG);
	if (len != 6)   // must be 6 for this header
	{
		base_->sdPrintFail("HDRerr");
		f->close();
		return;
	}
	//2:Format
	uint16_t i = readFileMultiByte(f, MB_WORD);
	if (i != 0)// && (dat16 != 1))
	{
		base_->sdPrintFail("FMTerr");
		f->close();
		return;
	}
	//3:Tracks
	i = readFileMultiByte(f, MB_WORD);  //don't do anything with this one
	//4::PPQN
	i = readFileMultiByte(f, MB_WORD);
	// read ticks per quarter note
	if (i & 0x8000) // top bit set is SMTE format
	{
		uint16_t framespersecond = (i >> 8) & 0x00ff;
		uint16_t resolution      = i & 0x00ff;

		switch (framespersecond)
		{
			case 232:  framespersecond = 24; break;
			case 231:  framespersecond = 25; break;
			case 227:  framespersecond = 29; break;
			case 226:  framespersecond = 30; break;
			default:
			base_->sdPrintFail("TPQerr");
			f->close();
			return;
			break;
		}
		i = framespersecond * resolution;
	}
	mmls.setClksPerQuarterNote(i);
	//5:MTrk
	//strncpy(hdr,"MTrk",HDR_SIZE);
	f->read(h,HDR_SIZE);
	if (strncmp(h, MTRK,HDR_SIZE) != 0)
	{
		base_->sdPrintFail("MIDerr");
		f->close();
		return;
	}
	//6:Length
	len = readFileMultiByte(f,MB_LONG);
	mmls.setLength(len);

	mmls.fileOpen = true;
}


void AtxCSdCard::readMidiStream(uint_fast8_t track, uint_fast8_t seq, midi::MidiMsgListStream& mmListStream)
{
	using namespace midi;
	File* f = &seqFile_[track][seq];
	MidiSeqMsg mmsg;
	bool eot = false;
	while(mmListStream.availableForStore() && eot==false && mmListStream.isEndOfFile()==false)
	{
		bool addMsg = false;
		uint8_t byteCount = 0;
		uint32_t delta = readFileVarLen(f,byteCount);
		mmListStream.incClks(delta);
		mmsg.setClk(mmListStream.getClks());
		uint_fast8_t status = readFileSingleByte(f,byteCount);
		switch(status)
		{
			case 0x80 ... 0xef:	// MIDI message
			{
				mmsg.setStatus(status);
				mmListStream.runningStatus = status;
				mmsg.setData1(readFileSingleByte(f,byteCount));
				if (MidiMsg::getMsgSize(status)==3)
				{
					mmsg.setData2(readFileSingleByte(f,byteCount));
				}
				addMsg = true;
			}
			break;
			case 0x00 ... 0x7f:	// MIDI run on message
			{
				mmsg.setStatus(mmListStream.runningStatus);
				mmsg.setData1(status);
				if (mmsg.getMsgSize()==3)
				{
					mmsg.setData2(readFileSingleByte(f,byteCount));
				}
				addMsg = true;
			}
			break;
			// ---------------------------- SYSEX
			case 0xf0:  // sysex_event = 0xF0 + <len:1> + <data_bytes> + 0xF7
			case 0xf7:  // sysex_event = 0xF7 + <len:1> + <data_bytes> + 0xF7
			{
				uint8_t len = readFileVarLen(f,byteCount);
				for (uint_fast8_t i=0;i<len;++i)
				{
					readFileSingleByte(f,byteCount);
				}
				//don't do anything with sysex
			}
			break;
			case 0xff:  // meta_event = 0xFF + <meta_type:1> + <length:v> + <event_data_bytes>
			{
				mmsg.setStatus(status);
				uint8_t mType = readFileSingleByte(f,byteCount);
				mmsg.setData1(mType);
				uint8_t len = readFileVarLen(f,byteCount);
				//don't do anything with meta
				for (uint_fast8_t i=0;i<len;++i)
				{
					readFileSingleByte(f,byteCount);
				}
				switch(mType)
				{
					case 0x2f:  // End of track
					eot = true;
					addMsg = true;
					break;
					default:
					break;
				}
			}
			break;
		}
		if(addMsg)
		{
			mmListStream.writeMsg(&mmsg);
		}
		mmListStream.incPos(byteCount);
	}
	
}

//ALL THIS NEEDS COMPLETELY REWRITING
void AtxCSdCard::remapMidiToSequence(midi::MidiMsgList& midiList, midi::MidiMsgList& mmList)
{
	//using namespace atx;
	//using namespace midi;
	//uint8_t ppqnRatio = midiList.getClksPerQuarterNote() / atx::CLKS_PER_QUARTER_NOTE;
	//mmList.clear();
	//uint32_t ts = 0;
	//for (uint_fast16_t i=0;i<midiList.getCount();++i)
	//{
		//uint8_t cmd,ch,n,v;
		//uint32_t d;
		//MidiSeqMsg m;
		//cmd = midiList.getEvent(i).getStatus() & 0xF0;
		////ch = midiList.getEvent(i).getStatus() & 0x0F;
		//n = midiList.getEvent(i).getData1();
		//v = midiList.getEvent(i).getData2();
		//if (cmd==MCMD_NOTEON && v==0)
		//{
			//cmd = MCMD_NOTEOFF;
		//}
		//d = midiList.getEvent(i).getDeltaTime();
		//ts += d;
		//switch (cmd)
		//{
			//case MCMD_NOTEON:
			//{
				//m.setCommand(MCMD_NOTEON);
				//m.setData1(n);
				//m.setData2(v);
				////m.setClkStamp(ts / ppqnRatio);
				//m.setDeltaTime(d);
				//mmList.append(m);
			//}
			//break;
			//case MCMD_NOTEOFF:
			//{
				//m.setCommand(MCMD_NOTEON);
				////m.setMsgByte(1,(MidiMsg::MCMD_NOTEOFF | editZone_));  //no point setting that here
				//m.setData1(n);
				////m.setData2(ts / ppqnRatio);
				//m.setDeltaTime(d);
				//mmList.append(m);
			//}
			//break;
		//}
	//}
	
}

//void AtxCSdCard::loadPatch(char* filePath, uint8_t cardNum)
//{
////File f = SD_.open(filePath,FILE_READ);
////for(uint16_t i=0;i<AtxCard::MAX_CCS;++i) //card_[cardNum]->getCcs();++i)
////{
////uint16_t v;
////f.read((void*)&v,2);
////if(i==0)
////{
////if (v!=card_[cardNum]->getCcValue(0))
////{
////base_->sdPrintString("WRONG HW/FW ");
////f.close();
////return;
////}
////}
////if (card_[cardNum]->connected && (card_[cardNum]->synth==0 || cardNum>=atx::MAX_SYNTHCARDS))
////{
//////AtxCEditor::getInstance().setCcVal(cardNum,i,v);
////base_->sdLoadPatch(cardNum,i,v);
////}
////}
////f.close();
//}

//void AtxCSdCard::savePatch(char* filePath, uint8_t cardNum)
//{
////File f = SD_.open(filePath,FILE_WRITE);
////for(uint16_t i=0;i<AtxCard::MAX_CCS;++i)
////{
////uint16_t v;
////v =  card_[cardNum]->getCcValue(i);
////uint8_t* a = (uint8_t*)&v;
////f.write(a,2);
////}
////f.close();
//}

//bool AtxCSdCard::deleteSettings()
//{
//if (SD_.exists(SETTINGS_FILENAME))
//{
//SD_.remove(SETTINGS_FILENAME);
//}
//}

bool AtxCSdCard::loadSettings()
{
	//if (SD_.exists(SETTINGS_FILENAME))
	//{
	//File f;
	//f = SD_.open(SETTINGS_FILENAME,FILE_READ);
	//for(uint16_t i=0;i<AtxCSettings::SETTINGS_MAXPARAMS;++i)
	//{
	//uint16_t v;
	//f.read((void*)&v,2);
	//if(i==0)
	//{
	//if (v!=AtxCSettings::SETTINGS_FWHW)
	//{
	//base_->sdPrintString("WRONG HW/FW ");
	//f.close();
	//return false;
	//}
	//
	//}
	////AtxCSettings::getInstance().setCcValue(i,v);
	//base_->sdLoadSetting(i,v);
	//}
	//f.close();
	//return true;
	//}
	//else
	//{
	//return false;
	//}
}

void AtxCSdCard::saveSettings(uint16_t* settings)
{
	//deleteSettings();
	//File f = SD_.open(SETTINGS_FILENAME,FILE_WRITE);
	//for (uint16_t i=0;i<AtxCSettings::SETTINGS_MAXPARAMS;++i)
	//{
	//uint16_t v = *(settings+i);
	//uint8_t* a = (uint8_t*)&v;
	//f.write(a,2);
	//base_->sdRefreshStatusBar(calcStatusBar(&f));
	//}
	//f.close();
}

//void AtxCSdCard::loadSeq(char* filePath, AtxSeqMsgList& mmList)
//{
//File f = SD_.open(filePath,FILE_READ);
//size_t count = readFileMultiByte(&f,MB_LONG);
//mmList.clear();
//char ft[FILE_TITLE_SIZE];
//calcFilePathFileTitle(ft,filePath);
//mmList.setName(ft);
//mmList.setLength(readFileMultiByte(&f,MB_WORD));
//mmList.looping = (bool)f.read();;
//mmList.setClksPerBeat(f.read());
//mmList.setBeatsPerBar(f.read());
//for (size_t i=0;i<count;++i)
//{
//AtxSeqMsg m;
//f.read(&m,sizeof(AtxSeqMsg));
//mmList.append(m);
//base_->sdRefreshStatusBar(calcStatusBar(&f));
//}
//f.close();
//}

//void AtxCSdCard::loadSeq(char* filePath, AtxSeqMsgListStream& mmLists)
//{
//mmLists.setFilePath(filePath);
//}


//void AtxCSdCard::readSeq(uint8_t zone, uint8_t seq, midi::MidiMsgListStream& mmLists)
//{
//using namespace midi;
//uint8_t m[sizeof(MidiMsg)];
//seqFile_[zone][seq].read(&m,sizeof(MidiMsg));
//memcpy(mmLists.getMsgPtr(),&m,sizeof(MidiMsg));
//}

void AtxCSdCard::loadSeq(char* filePath, midi::MidiMsgList& mmList)
{
	//COME BACK!
	//File f = SD_.open(filePath,FILE_READ);
	//if (f)
	//{
	//calcFilePathFileTitle(mmList.getNamePtr(),filePath);
	//mmList.setCount(readFileMultiByte(&f,MB_LONG));
	//mmList.setLength(readFileMultiByte(&f,MB_WORD));
	//mmList.looping = (bool)f.read();;
	//mmList.setClksPerBeat(f.read());
	//mmList.setBeatsPerBar(f.read());
	//mmList.setIndex(0);
	//for (size_t i=0;i<mmList.getCount();++i)
	//{
	//AtxSeqMsg m;
	//f.read(&m,sizeof(AtxSeqMsg));
	//mmList.append(m);
	//base_->sdRefreshStatusBar(calcStatusBar(&f));
	//};
	//f.close();
	//}
}

void AtxCSdCard::createSeq(uint8_t zone, uint8_t seq, midi::MidiMsgList& mmList)
{
	using namespace midi;
	char fn[FILE_NAME_SIZE];
	if (seqFile_[zone][seq])
	{
		seqFile_[zone][seq].getName(fn,FILE_NAME_SIZE);
		seqFile_[zone][seq].close();
		strncpy(filePath_,dirPath_,FILE_PATH_SIZE);
		strcat (filePath_,fn);
	}
	else
	{
		createFile("NewSeq","XSQ");
	}
	saveSeq(filePath_,mmList);
}

void AtxCSdCard::saveSeq(char* filePath, midi::MidiMsgList& mmList)
{
	//COME BACK!!!
	//File f = SD_.open(filePath,FILE_WRITE);
	//size_t count = mmList.getCount();
	//writeFileMultiByte(&f,count,MB_LONG);
	//writeFileMultiByte(&f,mmList.getLength(),MB_WORD);
	//f.write((uint8_t)mmList.looping);
	//f.write(mmList.getClksPerBeat());
	//f.write(mmList.getBeatsPerBar());
	//for (size_t i=0;i<count;++i)
	//{
	//uint8_t m[sizeof(AtxSeqMsg)];
	//memcpy(&m,mmList.getMsgPtr(i),sizeof(AtxSeqMsg));
	//f.write(m,sizeof(AtxSeqMsg));
	//base_->sdRefreshStatusBar(calcStatusBar(&f));
	//};
	//f.close();
}

void AtxCSdCard::loadFx(char* filePath)
{
	//if (card_[atx::CARD_MIX]->connected==false)
	//{
	//base_->sdPrintString("NO MIX      ");
	//}
	//else
	//{
	////AtxCHardware::getInstance().txWireHex();
	//}
}

void AtxCSdCard::debugSd()
{
	//char c = 0;
	//Oled * o[6];
	//for (uint8_t i=0;i<6;++i)
	//{
	//o[i] = AtxCHardware::getInstance().getOledPtr(i);
	//o[i]->clearDisplay();
	//o[i]->setThermometerRow(atx::UNUSED);
	//}
	//
	//o[1]->printStringBuffer(0,0,&dirPath_[0],false,false);
	//o[1]->printStringBuffer(0,1,&dirPath_[12],false,false);
	//o[3]->printStringBuffer(0,0,&filePath_[0],false,false);
	//o[3]->printStringBuffer(0,1,&filePath_[12],false,false);
	//o[4]->printIntBuffer(0,0,fileIndex_,false,false,2);
	//o[4]->printCharBuffer(3,0,'/',false);
	//o[4]->printIntBuffer(4,0,dirEntrys_,false,true,2);
	//o[4]->printStringBuffer(0,1,&newFileTitle_[0],false,true);
	//o[4]->printCharBuffer(8,1,'.',false);
	//o[4]->printStringBuffer(9,1,&fileExtFilt_[0],false,true);
	//o[5]->printStringBuffer(0,0,"Dir:",false,false);
	//o[5]->printCharBuffer(4,0,c,false);
	//o[5]->printCharBuffer(5,0,':',false);
	//o[5]->printCharBuffer(6,0,c,false);
	//o[5]->printStringBuffer(7,0,"",false,true);
	//if (getFunctionFileMode()==FILE_WRITE)
	//{
	//c = 'W';
	//}
	//else
	//{
	//c = 'R';
	//}
	//o[5]->printCharBuffer(8,1,c,false);
	//o[5]->printStringBuffer(9,1,"   ",false,false);
}