/*
* AtxCPrintDisp.cpp
*
* Created: 23/08/2021 21:18:00
* Author: info
*/


#include "AtxCPrintDisp.h"

OledBuffer* AtxCPrintDisp::buffer_ = NULL;
Oled* AtxCPrintDisp::xtrOled_ = NULL;
atx::Firmware AtxCPrintDisp::channelFirmware[atx::MAX_CARDS] = {atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL,atx::FW_DISABL};
//atx::AtxCcList AtxCPrintDisp::ccList_[atx::MAX_ZONES];

void AtxCPrintDisp::printDisp(Oled* oled, atx::AtxCcSet* ccSet, uint_fast8_t cc)
{
	using namespace atx;
	
	PrintDispInfo info;
	
	info.row  = ((ccSet->getCcInfoCtrlEnabled(cc)==true && ccSet->getCcInfoCtrlIsFunc(cc)==true) ? 2 : 1);
	info.bits = ccSet->getCcInfoPtr(cc)->bits;
	info.tag = ccSet->getCcInfoPtr(cc)->tag;
	info.value = ccSet->getCcValue(cc);
	info.scValue = ccSet->getCcValueScaled(cc);
	info.zone = ccSet->getCcInfoCtrlZone(cc);

	if (info.bits<15) //COME BACK - you need to make all wave sample params 16 bit!!!!
	{
		uint16_t mask = (1 << info.bits) - 1;
		info.scValue &= mask;  //safety
	}
	switch(ccSet->getCcInfoFmtNum(cc))
	{
		case FMT_INT:
		printFmtInt(oled, &info);
		break;
		case FMT_ONOFF:
		printFmtOnOff(oled, &info);
		break;
		case FMT_SIGNED:
		printFmtSigned(oled,&info);
		break;
		case FMT_6BITRANGE:
		printFmt6BitRange(oled,&info);
		break;
		case FMT_SIGNEDFLOAT:
		printFmtSignedFloat(oled,&info);
		break;
		case FMT_MIDINOTE:
		printFmtMidiNote(oled,&info);
		break;
		case FMT_DIV:
		printFmtDiv(oled,&info);
		break;
		case FMT_PPQN:
		printFmtPpqn(oled,&info);
		break;
		case FMT_INTPLAIN:
		printFmtIntPlain(oled,&info);
		break;
		case FMT_2BIT:
		printFmt2Bit(oled,&info);
		break;
		case FMT_BINARY:
		printFmtBinary(oled,&info);
		break;
		case FMT_OFFSETINT:
		printFmtOffsetInt(oled,&info);
		break;
		case FMT_FIRMWARE:
		printFmtFirmware(oled,&info);
		break;
		case FMT_CCLIST:
		printFmtCcList(oled,&info,ccSet);
		break;
		case FMT_TAG:
		printFmtTag(oled,&info);
		break;
		default:
		clearThermom(oled,info.row);
		clearInvert(oled,info.row);
		break;
	}

	switch(ccSet->getCcInfoFmtXtr(cc))
	{
		case XTR_WAVE:
		//printXtrWave(&info);
		//no do it after retun of sysex
		break;
		case XTR_ARP:
		//printXtrArp(&info);
		//no do it after retun of sysex
		break;
		case XTR_ENV:  //CARD ONLY
		printXtrEnv(&info,ccSet);
		break;
		case XTR_FILTER:  //ATM ONLY
		printXtrFilter(&info,ccSet);
		break;
		case XTR_SEQPOS:   //NOT USED? //this is for drum sequences...  could it also do seq sequences?

		break;
		case XTR_ZONELAYOUT:
		printXtrZoneLayout();
		break;
		case XTR_ODYICON:
		printXtrOdyIcon(&info,ccSet);
		break;
		default:
		if (info.row==2)  //this is so that it only clears the row if cc is for func
		{
			buffer_->clearRow();
		}
		break;
	}
}

void AtxCPrintDisp::convMidiNoteString(uint8_t note, char * str)
{
	uint8_t n = note % 12;
	strncpy(&str[0],S_FMT_NOTES[n],2);
	int8_t oct = note / 12;
	oct--;
	snprintf(&str[2],3,"%d",oct);
}

void AtxCPrintDisp::clearInvert(Oled* oled, uint_fast8_t row)
{
	oled->clearInvertMapRow(row);
}

void AtxCPrintDisp::clearThermom(Oled* oled, uint_fast8_t row)
{
	oled->setThermometerRow(atx::UNUSED);
	oled->clearInvertMapRow(row);
}

midi::MidiSysexType AtxCPrintDisp::getSysexTypeFromXtr(atx::DispExtra xtr)
{
	using namespace atx;
	switch(xtr)
	{
		case XTR_WAVE:
		return midi::MSXT_WAVETABLE;
		case XTR_ARP:
		return midi::MSXT_ARPPATTERN;
		break;
		default:
		return midi::MSXT_UNUSED;
		break;
	}
}


//**************************** HERE'S THE FORMATS********************************
void AtxCPrintDisp::printFmtInt(Oled* oled, PrintDispInfo* info)
{
	using namespace atx;
	if(info->bits>7)
	{
		oled->printIntBuffer(0,info->row,info->value,false,true,0);  //just an idea.  keep as many nums 0-127 as poss
	}
	else
	{
		oled->printIntBuffer(0,info->row,info->scValue,false,true,0);
	}
	uint8_t end = ((uint16_t)info->value * oled->getWidth()) >> MIDI_DATA_BITS;
	oled->setInvertRange(info->row,0,end);
	//oled->setInvertMapLevelRow(0,info->row,info->scValue,info->bits);
	oled->setThermometerRow(info->row);
	oled->setThermometerLine(false);
}
void AtxCPrintDisp::printFmtOnOff(Oled* oled, PrintDispInfo* info)
{
	oled->printStringBuffer(0,info->row,S_OFFON[info->scValue],false,true);
	clearThermom(oled,info->row);
};
void AtxCPrintDisp::printFmtSigned(Oled* oled, PrintDispInfo* info)
{
	using namespace atx;
	uint16_t off = (1 << (info->bits - 1));
	int16_t o = (int16_t)info->scValue - off;
	if (o>0)
	{
		oled->printCharBuffer(0,info->row,'+',false);
		oled->printIntBuffer(1,info->row,o,false,true,0);
	}
	else
	{
		oled->printIntBuffer(0,info->row,o,false,true,0);
	}
	uint8_t start = oled->getWidth() >> 1;
	uint8_t end = ((uint16_t)info->value * oled->getWidth()) >> MIDI_DATA_BITS;
	oled->setInvertRange(info->row,start,end);
	//oled->setInvertMapLevelRow(0,info->row,info->scValue,info->bits);
	oled->setThermometerRow(info->row);
	oled->setThermometerLine(true);
};

void AtxCPrintDisp::printFmt6BitRange(Oled* oled, PrintDispInfo* info)
{
	using namespace atx;
	uint8_t ccRange[2] = {7-((info->scValue & 0x07)),((info->scValue >> 3) & 0x07)};
	uint8_t oledRange[2];
	uint8_t pos = 0;
	for(uint_fast8_t i=0;i<2;++i)
	{
		ccRange[i]  = ((uint16_t)ccRange[i] * 293) >> 4;  //scale up to full cc range.  see keybed code for this.
		if(ccRange[i]>=INT8_MAX) ccRange[i] = INT8_MAX;
		oledRange[i] = ((uint16_t)ccRange[i] * oled->getWidth()) >>  MIDI_DATA_BITS;
		oled->printIntBuffer(pos,info->row,ccRange[i],false,false,3);
		pos += 3;
		if(i==0)
		{
			oled->printCharBuffer(pos,info->row,'-',false);
			pos++;
		}
	}
	oled->printStringBuffer(pos,info->row,"",false,true);
	oled->setInvertRange(info->row,oledRange[0],oledRange[1]);
	oled->setThermometerRow(info->row);
	oled->setThermometerLine(false);
	
}

void AtxCPrintDisp::printFmtSignedFloat(Oled* oled, PrintDispInfo* info)
{
	using namespace atx;
	float halfscale = (float)(1 << (info->bits-1));
	float f = (((float)info->scValue - halfscale) * info->tag) / halfscale;
	if (f>0)
	{
		oled->printCharBuffer(0,1,'+',false);
		oled->printFloatBuffer(1,1,f,false,true,1);
	}
	else
	{
		oled->printFloatBuffer(0,1,f,false,true,1);
	}
	uint8_t start = oled->getWidth() >> 1;
	uint8_t end = ((uint16_t)info->value * oled->getWidth()) >> MIDI_DATA_BITS;
	oled->setInvertRange(info->row,start,end);
	oled->setThermometerRow(info->row);
	oled->setThermometerLine(true);
};
void AtxCPrintDisp::printFmtMidiNote(Oled* oled, PrintDispInfo* info)
{
	char s[5];
	convMidiNoteString(info->scValue,s);
	oled->printStringBuffer(0,info->row,s,true,true);
	clearThermom(oled,info->row);
};
void AtxCPrintDisp::printFmtDiv(Oled* oled, PrintDispInfo* info)
{
	oled->printCharBuffer(0,info->row,info->scValue,true);
	oled->printStringBuffer(1,info->row,"",false,true);
	clearThermom(oled,info->row);
};
void AtxCPrintDisp::printFmtPpqn(Oled* oled, PrintDispInfo* info)
{
	oled->printIntBuffer(0,info->row,I_PPQN[info->scValue],true,true,0);
	clearThermom(oled,info->row);
};
void AtxCPrintDisp::printFmtIntPlain(Oled* oled, PrintDispInfo* info)
{
	oled->printIntBuffer(0,info->row,info->scValue,true,true,0);  //regular int
	clearThermom(oled,info->row);
};
void AtxCPrintDisp::printFmt2Bit(Oled* oled, PrintDispInfo* info)
{
	oled->clearRow(info->row);
	oled->printStringBuffer(0,info->row,S_OFFON[bitRead(info->scValue,0)],false,false);
	oled->printStringBuffer(3,info->row,S_OFFON[bitRead(info->scValue,1)],false,false);
	clearThermom(oled,info->row);
};
//prob delete now cc is 7 bit
void AtxCPrintDisp::printFmtBinary(Oled* oled, PrintDispInfo* info)
{
	oled->clearRow(info->row);
	for (uint_fast8_t i=0;i<info->bits;++i)
	{
		uint_fast8_t v = bitRead(info->scValue,i);
		oled->printIntBuffer(i,info->row,v,false,true,0);
	}
	clearThermom(oled,info->row);
}
void AtxCPrintDisp::printFmtOffsetInt(Oled* oled, PrintDispInfo* info)
{
	uint16_t v = info->scValue + (uint16_t)info->tag;
	oled->printIntBuffer(0,info->row,v,true,true,0);
	clearThermom(oled,info->row);
}
void AtxCPrintDisp::printFmtFirmware(Oled* oled, PrintDispInfo* info)
{
	using namespace atx;
	oled->printStringBuffer(0,info->row,AtxCcSet::getFirmwareNamePtr((Firmware)info->scValue,true),false,true);
	clearThermom(oled,info->row);
};

void AtxCPrintDisp::printFmtCcList(Oled* oled, PrintDispInfo* info, atx::AtxCcSet* ccSet)
{
	using namespace atx;
	uint8_t channelCc = info->tag;
	Firmware fw = channelFirmware[ccSet->getCcValueScaled(channelCc)];
	CardType ct = AtxCcSet::getFirmwareCardType(fw);
	const char* s[2] = {AtxCcSet::getCcInfoPtr(fw,info->scValue)->label1,AtxCcSet::getCcInfoPtr(fw,info->scValue)->label2};  
	uint8_t len[2] = {strlen(s[0]),strlen(s[1])};
	uint8_t pos = 0;
	if(ct!=CT_SYNTH && ct!=CT_YM && info->zone<MAX_ZONES)  //oof nasty
	{
		oled->printCharBuffer(0,info->row,'Z',false);
		oled->printIntBuffer(1,info->row,AtxCcSet::getCcInfoCtrlZone(fw,info->scValue)+1,false,false,0);
		oled->printCharBuffer(2,info->row,' ',false);
		pos = 3;
	}
	oled->printStringBuffer(pos,info->row,s[0],false,false);
	pos += len[0];
	if((12-pos)>len[1])
	{
		oled->printCharBuffer(pos,info->row,' ',false);
		pos++;
	}
	oled->printStringBuffer(pos,info->row,s[1],false,true);
	pos += len[1];
	if(pos>12)
	{
		oled->printCharBuffer(11,info->row,'.',false);
	}
	clearThermom(oled,info->row);
}
void AtxCPrintDisp::printFmtTag(Oled* oled, PrintDispInfo* info)
{
	using namespace atx;
	switch(info->tag)
	{
		case TAG_KEY_CHAN:
		if(info->scValue<MAX_ZONES)
		{
			oled->printStringBuffer(0,info->row,S_ZONE,false,true);
			oled->printIntBuffer(4,info->row,info->scValue+1,false,false,0);
		}
		else
		{
			oled->printStringBuffer(0,info->row,S_CARD,false,true);
			oled->printIntBuffer(4,info->row,info->scValue+1,false,false,2);
		}
		oled->printStringBuffer(6,info->row,AtxCcSet::getFirmwareNamePtr(channelFirmware[info->scValue],true),false,true);
		break;
		case TAG_ATM_FILT:
		oled->printStringBuffer(0,info->row,S_FILTERTYPE[info->scValue],false,true);
		break;
		case TAG_ATM_RES:
		oled->printIntBuffer(0,info->row,I_RES[info->scValue],true,true,0);
		break;
		case TAG_ATM_SMPLEN:
		oled->printIntBuffer(0,info->row,I_AUDIO_MIN_LENGTH[info->scValue],true,true,0);
		break;
		case TAG_ATM_QUANTSCALE:
		oled->printStringBuffer(0,info->row,S_FMT_QUANT[info->scValue],false,true);
		break;
		case TAG_ODY_FILT:
		oled->printStringBuffer(0,info->row,S_ODY_FILT[info->scValue],false,true);
		break;
		case TAG_SET_VOICEASSIGN:
		oled->printStringBuffer(0,info->row,S_SET_VOICEASSIGN[info->scValue],false,true);
		break;
		case TAG_SET_CLICKTYPE:
		oled->printStringBuffer(0,info->row,S_SET_CLICKTYPE[info->scValue],false,true);
		break;
		case TAG_SET_FOLLOWEDITMODE:
		oled->printStringBuffer(0,info->row,S_SET_FOLLOWEDITMODE[info->scValue],false,true);
		break;
		case TAG_SET_FOLLOWCARDMODE:
		oled->printStringBuffer(0,info->row,S_SET_FOLLOWCARDMODE[info->scValue],false,true);
		break;
		case TAG_SET_LFOMODE:
		oled->printStringBuffer(0,info->row,S_SET_LFOTRIGGERMODE[info->scValue],false,true);
		break;
	}
	clearThermom(oled,info->row);
}

//**************************** HERE'S THE XTRAS********************************
//prob move to own function
void AtxCPrintDisp::printXtrWave(midi::SysexMsg* msg)
{
	buffer_->clearBuffer();
	for(uint_fast8_t x=0;x<24;++x)
	{
		uint8_t y = (127-msg->getData(x+2)) >> 3;  //15 - (uint8_t)(((int8_t)v + 128) / 16);
		if(x==0)
		{
			buffer_->drawPixelBuffer(x<<1,y,true);
		}
		else
		{
			buffer_->drawLineBuffer(x<<1,y,true);
		}
	}
	buffer_->setReadyToPrint(true);
}
//prob move to own function
void AtxCPrintDisp::printXtrArp(midi::SysexMsg* msg)
{
	using namespace midi;
	buffer_->clearBuffer();
	uint_fast8_t ymin = 255, ymax = 0;
	for (uint_fast8_t i=0;i<16;++i)
	{
		if(msg->getData(i+2)<ymin) ymin = msg->getData(i+2);
		if(msg->getData(i+2)>ymax) ymax = msg->getData(i+2);
	}
	for(uint_fast8_t i=0;i<16;++i)
	{
		uint8_t x = ((uint32_t)i * 48) >> 4;
		uint8_t y = (msg->getData(i+2) - ymin) << 4 / (ymax - ymin);
		buffer_->drawPixelBuffer(x,y,true);
		buffer_->drawLineBuffer(x+2,y,true);
	}
	buffer_->setReadyToPrint(true);
}
void AtxCPrintDisp::printXtrEnv(PrintDispInfo* info, atx::AtxCcSet* ccSet)
{
	buffer_->clearBuffer();
	uint8_t cc = info->tag;
	for(uint_fast8_t i=0;i<5;++i)  //this works providing ADSRinvexp are in order
	{
		uint_fast8_t nextCC = ccSet->getCcInfoPtr(cc)->tag;
		if(nextCC<cc)
		{
			cc = nextCC;
			break;
		}
		else
		{
			cc = nextCC;
		}
	}
	
	uint_fast8_t x[5], y[5];
	x[0] = 0; y[0] = 15;
	x[1] = (ccSet->getCcValueScaled(cc) * 12) >> 4;
	y[1] = 0;
	cc = ccSet->getCcInfoPtr(cc)->tag;
	x[2] = x[1] + ((ccSet->getCcValueScaled(cc) * 12) >> 4);
	cc = ccSet->getCcInfoPtr(cc)->tag;
	y[2] = 15 - ccSet->getCcValueScaled(cc);
	cc = ccSet->getCcInfoPtr(cc)->tag;
	x[3] = 47 - ((ccSet->getCcValueScaled(cc) * 12) >> 4);
	y[3] = y[2];
	x[4] = 47;
	y[4] = 15;
	cc = ccSet->getCcInfoPtr(cc)->tag;
	uint8_t arcType[2] = {2,1};
	if (bitRead(ccSet->getCcValueScaled(cc),1))
	{
		for(uint_fast8_t i=0;i<2;++i)
		{
			arcType[i] = 3 - arcType[i];
		}
		for (uint_fast8_t i=0;i<5;++i)
		{
			y[i] = 15 - y[i];
		}
	}
	if(bitRead(ccSet->getCcValueScaled(cc),0))
	{
		buffer_->drawPixelBuffer(x[0],y[0],true);
		buffer_->drawArcBuffer(x[1],y[1],arcType[0],true);
		buffer_->drawArcBuffer(x[2],y[2],arcType[1],true);
		buffer_->drawLineBuffer(x[3],y[3],true);
		buffer_->drawArcBuffer(x[4],y[4],arcType[1],true);
	}
	else
	{
		buffer_->drawPixelBuffer(x[0],y[0],true);
		for (uint_fast8_t i=1;i<5;++i)
		{
			buffer_->drawLineBuffer(x[i],y[i],true);
		}
	}

	buffer_->setReadyToPrint(true);
}
void AtxCPrintDisp::printXtrFilter(PrintDispInfo* info, atx::AtxCcSet* ccSet)
{
	uint_fast8_t x = 0;
	
	buffer_->clearBuffer();
	buffer_->drawPixelBuffer(x,I_ATMFILT_COORD[ccSet->getCcValueScaled(atx::ATM_FILTTYPE)][0]);  //can't grab it from info, has to work for filt norm cc too
	for (uint_fast16_t i=1;i<32;++i)
	{
		x = (i * 48) >> 5;
		buffer_->drawLineBuffer(x,I_ATMFILT_COORD[ccSet->getCcValueScaled(atx::ATM_FILTTYPE)][i]);
	}
	buffer_->setReadyToPrint(true);
}
void AtxCPrintDisp::printXtrZoneLayout()
{
	//oled->clearRow(3);
	using namespace atx;
	for(uint_fast8_t i=0;i<MAX_SYNTHCARDS;++i)
	{
		if (AtxSynthZoneMap::synthCardType[i]!=CT_DISCON)
		{
			xtrOled_->printDigit6x8Buffer(i,0,i+1);
			if (AtxSynthZoneMap::synthZone[i]==UNUSED)
			{
				xtrOled_->printDigit6x8Buffer(i,1,Oled::FONT_6x8_CLEAR);
			}
			else
			{
				xtrOled_->printDigit6x8Buffer(i,1,AtxSynthZoneMap::synthZone[i]+11);
			}
		}
		else
		{
			xtrOled_->printDigit6x8Buffer(i,0,Oled::FONT_6x8_CLEAR);
		}
	}
}
void AtxCPrintDisp::printXtrOdyIcon(PrintDispInfo* info, atx::AtxCcSet* ccSet)
{
	using namespace atx;
	uint_fast8_t icon;
	if (info->tag<0x80)
	{
		icon = info->tag + info->scValue;
	}
	else
	{
		uint8_t mastCc = info->tag & 0x7F;
		icon = ccSet->getCcInfoPtr(mastCc)->tag + ccSet->getCcValueScaled(mastCc);  //this is mad! revisit.
	}
	buffer_->clearBuffer();
	bool buff = true;
	switch (icon)
	{
		case ODI_ADSR1:
		case ODI_ADSR2:
		case ODI_ADSR3:
		buffer_->drawPixelBuffer(0,15);
		buffer_->drawLineBuffer(8,15);
		buffer_->drawLineBuffer(16,0);
		buffer_->drawLineBuffer(24,8);
		buffer_->drawLineBuffer(32,8);
		buffer_->drawLineBuffer(40,15);
		buffer_->drawLineBuffer(47,15);
		break;
		case ODI_AR:
		buffer_->drawPixelBuffer(0,15);
		buffer_->drawLineBuffer(8,15);
		buffer_->drawLineBuffer(16,0);
		buffer_->drawLineBuffer(32,0);
		buffer_->drawLineBuffer(40,15);
		buffer_->drawLineBuffer(47,15);
		break;
		case ODI_NOISE:
		xtrOled_->printStringBuffer(0,info->row,"Noise",false,true);
		buff = false;
		break;
		case ODI_RMOD:
		xtrOled_->printStringBuffer(0,info->row,"R Mod",false,true);
		buff = false;
		break;
		case ODI_SAW:
		buffer_->drawPixelBuffer(0,15);
		buffer_->drawLineBuffer(16,0);
		buffer_->drawLineBuffer(16,15);
		buffer_->drawLineBuffer(32,0);
		buffer_->drawLineBuffer(32,15);
		buffer_->drawLineBuffer(47,0);
		buffer_->drawLineBuffer(47,15);
		break;
		case ODI_SH1:
		case ODI_SH2:
		xtrOled_->printStringBuffer(0,info->row," S/H",false,true);
		buff = false;
		break;
		case ODI_SIN1:
		case ODI_SIN2:
		case ODI_SIN3:
		buffer_->drawPixelBuffer(0,I_ODY_SINE[0],false,OledBuffer::SEX_OFF);
		for (uint_fast8_t i=1;i<48;++i)
		{
			buffer_->drawLineBuffer(i,(15-I_ODY_SINE[(i%24)]));
		}
		break;
		case ODI_SQ1:
		case ODI_SQ2:
		buffer_->drawPixelBuffer(0,15);
		buffer_->drawLineBuffer(6,15);
		buffer_->drawLineBuffer(6,0);
		buffer_->drawLineBuffer(18,0);
		buffer_->drawLineBuffer(18,15);
		buffer_->drawLineBuffer(30,15);
		buffer_->drawLineBuffer(30,0);
		buffer_->drawLineBuffer(42,0);
		buffer_->drawLineBuffer(42,15);
		buffer_->drawLineBuffer(47,15);
		break;
	}
	if (buff)
	{
		buffer_->setReadyToPrint(true);
	}
}