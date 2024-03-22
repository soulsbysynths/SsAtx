/* ATX_H_ */

/*
* Atx.h
*
* Created: 21/07/2018 21:08:48
*  Author: info
*/


#ifndef ATX_H_
#define ATX_H_
#include <stdint.h>

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
#ifndef bitToggle
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))
#endif

namespace atx
{
	static const uint8_t MAX_CARDS = 16;
	static const uint8_t MAX_CARDS_MASK = MAX_CARDS-1;
	static const uint8_t MAX_SYNTHCARDS = 8;
	static const uint8_t MAX_POLYCARDS = 8;
	static const uint8_t MAX_ZONES = 8;
	static const uint8_t MAX_ZONES_MASK = MAX_ZONES-1;
	static const uint8_t MAX_VOICES = 8;
	static const uint8_t MAX_TRACKS = 6;
	static const uint8_t MAX_SEQS_BS = 1;
	static const uint8_t MAX_SEQS = 1 << MAX_SEQS_BS;
	static const uint8_t MAX_CHANNELS = 16;
	static const uint8_t MAX_CCS = 120;
	static const uint8_t MIDI_DATA_BITS = 7;
	static const uint8_t UNUSED = 0xFF;

	static const uint8_t CLKS_PER_QUARTER_NOTE = 24;
	enum PlayState : uint8_t
	{
		PS_STOP = 0,
		PS_PLAYCUE,
		PS_PLAY,
		PS_RECCUE,
		PS_REC
	};
	enum FollowEditMode : uint8_t
	{
		FEM_OFF = 0,
		FEM_EDITSYNTH_SET_FXZONE,
		FEM_EDITFXCARD_SET_FXZONE,
		FEM_EDITBOTH_SET_FXZONE	
	};
	enum FollowCardMode : uint8_t
	{
		FCM_OFF = 0,
		FCM_FXCARD_SET_EDITSYNTH,
		FCM_FXCARD_SET_EDITSYNANDFXZONE,
		FCM_FXCARD_SET_EDITFXCARD	
	};
	#if defined(ARDUINO_SAMD_ZERO)
	static const uint8_t CARD_FLT = 8;
	static const uint8_t CARD_MIX = 9;
	static const uint8_t CARD_KEY = 10;
	#endif
}

static const uint8_t I_PPQN[4] = {1,4,16,24};

#endif /* ATX_H_ */