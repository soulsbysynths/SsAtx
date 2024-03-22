/*
* AtxSynthZoneMap.h
*
* Created: 08/09/2021 19:30:37
* Author: info
*/


#ifndef __ATXSYNTHZONEMAP_H__
#define __ATXSYNTHZONEMAP_H__

#include "Atx.h"
#include "AtxCcSet.h"

namespace atx
{
	class AtxSynthZoneMap
	{
		//variables
		public:
		static uint8_t synthZone[atx::MAX_SYNTHCARDS];
		static CardType synthCardType[atx::MAX_SYNTHCARDS];
		static bool zoneEnabled[atx::MAX_ZONES];
		static uint8_t zoneMasterSynth[atx::MAX_ZONES];
		static uint8_t zoneSynths[atx::MAX_ZONES];
		static CardType zoneCardType[atx::MAX_ZONES];
		protected:
		private:

		//functions
		public:
		AtxSynthZoneMap();
		~AtxSynthZoneMap();
		AtxSynthZoneMap( const AtxSynthZoneMap &c );
		AtxSynthZoneMap& operator=( const AtxSynthZoneMap &c );
		static void updateMap(AtxCcSet* ccSet);
		protected:
		private:


	}; //AtxSynthZoneMap
}



#endif //__ATXSYNTHZONEMAP_H__
