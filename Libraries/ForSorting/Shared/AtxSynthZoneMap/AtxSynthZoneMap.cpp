/* 
* AtxSynthZoneMap.cpp
*
* Created: 08/09/2021 19:30:36
* Author: info
*/


#include "AtxSynthZoneMap.h"

namespace atx
{
	uint8_t AtxSynthZoneMap::synthZone[atx::MAX_SYNTHCARDS] = {UNUSED,UNUSED,UNUSED,UNUSED,UNUSED,UNUSED,UNUSED,UNUSED};
	CardType AtxSynthZoneMap::synthCardType[atx::MAX_SYNTHCARDS] = {CT_DISCON,CT_DISCON,CT_DISCON,CT_DISCON,CT_DISCON,CT_DISCON,CT_DISCON,CT_DISCON};
	bool AtxSynthZoneMap::zoneEnabled[atx::MAX_ZONES] = {false};
	uint8_t AtxSynthZoneMap::zoneMasterSynth[atx::MAX_ZONES] = {0};
	uint8_t AtxSynthZoneMap::zoneSynths[atx::MAX_ZONES] = {0};
	CardType AtxSynthZoneMap::zoneCardType[atx::MAX_ZONES] = {CT_DISCON,CT_DISCON,CT_DISCON,CT_DISCON,CT_DISCON,CT_DISCON,CT_DISCON,CT_DISCON};
	
	// default constructor
	AtxSynthZoneMap::AtxSynthZoneMap()
	{
	} //AtxSynthZoneMap

	// default destructor
	AtxSynthZoneMap::~AtxSynthZoneMap()
	{
	} //~AtxSynthZoneMap

	void AtxSynthZoneMap::updateMap(AtxCcSet* ccSet)
	{
		uint8_t sCnt = 0;
		for (uint_fast8_t s=0;s<MAX_SYNTHCARDS;++s)
		{
			synthZone[s] = UNUSED;
		}
		for (uint_fast8_t z=0;z<MAX_ZONES;++z)
		{
			zoneEnabled[z] = false;
			zoneMasterSynth[z] = 0;
			zoneSynths[z] = 0;
 			bool enabled = ccSet->getCcValueScaledFromZone1Cc(SET_ZONEENABLE_Z1,z);
			uint_fast8_t masterSynth = ccSet->getCcValueScaledFromZone1Cc(SET_MASTER_Z1,z);
			uint_fast8_t synths = ccSet->getCcValueScaledFromZone1Cc(SET_SYNTHS_Z1,z);
			zoneCardType[z] = synthCardType[masterSynth];
			if (enabled)
			{
				sCnt = 0;
				for(uint_fast8_t s=masterSynth;s<=(masterSynth+synths);++s)
				{
					if(synthCardType[s]!=CT_DISCON && synthCardType[s]==zoneCardType[z] && synthZone[s]==UNUSED)  //all synths in zone must be connected and of same card type
					{
						synthZone[s] = z;
						zoneEnabled[z] = true;
						zoneMasterSynth[z] = masterSynth;
						zoneSynths[z] = sCnt;
						sCnt++;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}
