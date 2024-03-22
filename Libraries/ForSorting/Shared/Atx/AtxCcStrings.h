/*
* AtxParamsProgmem.h
*
* Created: 18/02/2020
*  Author: info
*/


#ifndef ATXPARAMSPROGMEM_H_
#define ATXPARAMSPROGMEM_H_
//#include <stddef.h>

namespace atx
{

	//ATM
	static const char S_FINE_TUNE[] = "Fine Tune";
	static const char S_TUNE_SPREAD[] = "Tune Spread";
	static const char S_FILTER_FREQ[] = "Filter Freq";
	static const char S_FILTER_RES[] = "Filter Res";
	static const char S_AMP_BIAS[] = "Amp Bias";
	static const char S_DISTORTION[] = "Distortion";
	static const char S_PITCH_ENV[] = "Pitch Env";
	static const char S_PULSE_W_ENV[] = "Pulse W Env";
	static const char S_FILTER_ENV[] = "Filter Env";
	static const char S_PHASE_ENV[] = "Phase Env";
	static const char S_AMP_ENV[] = "Amp Env";
	static const char S_WARP[] = "Warp";
	static const char S_PITCH_LFO[] = "Pitch LFO";
	static const char S_PULSE_W_LFO[] = "Pulse W LFO";
	static const char S_FILTER_LFO[] = "Filter LFO";
	static const char S_PHASE_LFO[] = "Phase LFO";
	static const char S_AMP_LFO[] = "Amp LFO";
	static const char S_LEGATO[] = "Legato";
	static const char S_WAVE[] = "Wave";
	static const char S_FORM[] = "form";
	static const char S_BANK[] = "Bank";
	static const char S_COARSE[] = "Coarse";
	static const char S_TUNE[] = "Tune";
	static const char S_OCTAVE[] = "Octave";
	static const char S_QUANT[] = "Quant";
	static const char S_SCALE[] = "Scale";
	static const char S_PORTA[] = "Porta";
	static const char S_MENTO[] = "mento";
	static const char S_FILTER[] = "Filter";
	static const char S_TYPE[] = "Type";
	static const char S_NORM[] = "Norm";
	static const char S_CRUSH[] = "Crush";
	static const char S_WAVECR[] = "WaveCr";
	static const char S_PREFLT[] = "PreFlt";
	static const char S_AUDIO[] = "Audio";
	static const char S_RES[] = "Res";
	static const char S_SAMPLE[] = "Sample";
	static const char S_SOURCE[] = "Source";
	static const char S_CONT[] = "Cont";
	static const char S_MIN_SP[] = "Min Sp";
	static const char S_LENGTH[] = "Length";
	static const char S_A_ENV[] = "A Env";
	static const char S_A[] = "A";
	static const char S_D[] = "D";
	static const char S_S[] = "S";
	static const char S_R[] = "R";
	static const char S_EXPINV[] = "ExpInv";
	static const char S_M_ENV[] = "M Env";
	static const char S_LFO[] = "LFO";
	static const char S_DIV[] = "Div";
	static const char S_INVERT[] = "Invert";
	static const char S_ARP[] = "Arp";
	static const char S_PPONG[] = "PPong";


	static const char S_OSC_1[] = "Osc 1";
	static const char S_FMA[] = "FM-A";
	static const char S_FMB[] = "FM-B";
	static const char S_OSC_2[] = "Osc 2";
	static const char S_PULSEW[] = "PulseW";
	static const char S_PWM[] = "PWM";
	//static const char S_FX[] = "FX";
	static const char S_LEVEL[] = "Level";
	//static const char S_FILTER[] = "Filter";
	static const char S_ADSR[] = "ADSR";
	//static const char S_A[] = "A";
	//static const char S_D[] = "D";
	//static const char S_S[] = "S";
	//static const char S_R[] = "R";
	//static const char S_LOAD[] = "Load";
	//static const char S_PATCH[] = "Patch";
	static const char S_FMA_S[] = "FM-A S";
	static const char S_FMB_S[] = "FM-B S";
	static const char S_SYNC[] = "Sync";
	static const char S_PWM_S[] = "PWM S";
	//static const char S_SOURCE[] = "Source";
	static const char S_AR[] = "AR";
	//static const char S_SAVE[] = "Save";
	static const char S_KEYTRK[] = "KeyTrk";
	static const char S_AMP[] = "Amp";
	static const char S_OSC_1_FREQ[] = "Osc 1 Freq";
	static const char S_OSC_2_FREQ[] = "Osc 2 Freq";
	static const char S_LFO_FREQ[] = "LFO Freq";
	//static const char S_FILTER_FREQ[] = "Filter Freq";
	//static const char S_FILTER_RES[] = "Filter Res";
	static const char S_HPF_FREQ[] = "HPF Freq";
	static const char S_PORTAMENTO[] = "Portamento";
	static const char S_MONO[] = "Mono";
	static const char S_FILT_TYPE[] = "Filt Type";
	//static const char S_LEGATO[] = "Legato";
	static const char S_LFO_RETRIG[] = "LFO Retrig";

	static const char S_BD_VOLUME[] = "BD Volume";
	static const char S_SNARE_VOLUME[] = "Snare Volume";
	static const char S_CLAP_VOLUME[] = "Clap Volume";
	static const char S_TOM_VOLUME[] = "Tom Volume";
	static const char S_CL_HH_VOLUME[] = "Cl HH Volume";
	static const char S_OP_HH_VOLUME[] = "Op HH Volume";
	static const char S_CBELL_VOLUME[] = "CBell Volume";
	static const char S_RIMSH_VOLUME[] = "RimSh Volume";
	static const char S_FREQ_LFO[] = "Freq LFO";
	static const char S_SAMPLE_FREQ[] = "Sample Freq";
	static const char S_BD[] = "BD";
	static const char S_DELAY[] = "Delay";
	static const char S_PITCH[] = "Pitch";
	//static const char S_LENGTH[] = "Length";
	static const char S_SNARE[] = "Snare";
	static const char S_CLAP[] = "Clap";
	static const char S_TOM[] = "Tom";
	static const char S_CL_HH[] = "Cl HH";
	static const char S_OP_HH[] = "Op HH";
	static const char S_CBELL[] = "CBell";
	static const char S_RIMSH[] = "RimSh";
	//static const char S_LFO[] = "LFO";
	//static const char S_WAVE[] = "Wave";



	//static const char S_WAVE[] = "Wave";
	//static const char S_A[] = "A";
	static const char S_B[] = "B";
	static const char S_C[] = "C";
	//static const char S_D[] = "D";
	static const char S_CYCLER[] = "Cycler";
	//static const char S_DIV[] = "Div";
	//static const char S_A_ENV[] = "A Env";
	//static const char S_S[] = "S";
	//static const char S_R[] = "R";
	static const char S_PT_LFO[] = "Pt LFO";
	static const char S_PH_LFO[] = "Ph LFO";
	//static const char S_EXTINV[] = "ExtInv";
	//static const char S_ARP[] = "Arp";
	//static const char S_TYPE[] = "Type";
	//static const char S_EXTPP[] = "ExtPP";
	static const char S_TIE[] = "Tie";
	static const char S_ENABLE[] = "Enable";
	static const char S_GATE[] = "Gate";
	//static const char S_EXPINV[] = "ExpInv";
	//static const char S_MONO[] = "Mono";
	static const char S_PHONIC[] = "phonic";
	//static const char S_LOAD[] = "Load";
	//static const char S_PATCH[] = "Patch";
	//static const char S_SAVE[] = "Save";
	static const char S_DETUNE[] = "Detune";
	//static const char S_PITCH_LFO[] = "Pitch LFO";
	//static const char S_PITCH_ENV[] = "Pitch Env";
	static const char S_PH_SINE_FQ[] = "Ph Sine Fq";
	static const char S_PHASER_DEPTH[] = "Phaser Depth";
	static const char S_PH_SINE_WT[] = "Ph Sine<->WT";
	static const char S_BYPASS[] = "Bypass";
	static const char S_CLIP[] = "Clip";
	static const char S_EXTINV[] = "DELETE";
	static const char S_EXTPP[] = "DELETE";


	//AUDIO IN
	static const char S_MUTE[] = "Mute";
	static const char S_INPUT[] = "Input";
	static const char S_QUALITY[] = "Quality";
	static const char S_INPUT_FILTER[] = "Input Filter";
	//static const char S_VOLUME[] = "Volume";
	//static const char S_PITCH[] = "Pitch";
	//static const char S_LOAD[] = "Load";
	//static const char S_PATCH[] = "Patch";
	//static const char S_SAVE[] = "Save";

	
	//BEZIER
	//static const char S_COARSE[] = "Coarse";
	//static const char S_TUNE[] = "Tune";
	//static const char S_NORM[] = "Norm";
	static const char S_SCAN[] = "Scan";
	static const char S_BWRDS[] = "Bwrds";
	//static const char S_M_ENV[] = "M Env";
	//static const char S_A[] = "A";
	//static const char S_D[] = "D";
	//static const char S_S[] = "S";
	//static const char S_R[] = "R";
	//static const char S_EXPINV[] = "ExpInv";
	//static const char S_A_ENV[] = "A Env";
	//static const char S_LFO[] = "LFO";
	//static const char S_WAVE[] = "Wave";
	//static const char S_DIV[] = "Div";
	//static const char S_ARP[] = "Arp";
	//static const char S_TYPE[] = "Type";
	//static const char S_PORTA[] = "Porta";
	//static const char S_MENTO[] = "mento";
	//static const char S_LOAD[] = "Load";
	//static const char S_PATCH[] = "Patch";
	//static const char S_OCTAVE[] = "Octave";
	static const char S_PITCH_SPREAD[] = "Pitch Spread";
	static const char S_POINT_A_X[] = "Point A X";
	//static const char S_CRUSH[] = "Crush";
	//static const char S_WAVECR[] = "WaveCr";
	//static const char S_PREFLT[] = "PreFlt";
	//static const char S_AUDIO[] = "Audio";
	//static const char S_RES[] = "Res";
	static const char S_POINT_A_Y[] = "Point A Y";
	static const char S_POINT_B_X[] = "Point B X";
	static const char S_POINT_B_Y[] = "Point B Y";
	//static const char S_INVERT[] = "Invert";
	//static const char S_PPONG[] = "PPong";
	//static const char S_QUANT[] = "Quant";
	//static const char S_SCALE[] = "Scale";
	//static const char S_SAVE[] = "Save";
	static const char S_POINT_C_X[] = "Point C X";
	static const char S_POINT_C_Y[] = "Point C Y";
	static const char S_A_X_ENV[] = "A X Env";
	static const char S_A_Y_ENV[] = "A Y Env";
	//static const char S_AMP_LFO[] = "Amp LFO";
	static const char S_B_X_ENV[] = "B X Env";
	static const char S_B_Y_ENV[] = "B Y Env";
	//static const char S_FINE_TUNE[] = "Fine Tune";
	static const char S_C_X_ENV[] = "C X Env";
	//static const char S_PITCH_ENV[] = "Pitch Env";
	//static const char S_PITCH_LFO[] = "Pitch LFO";
	static const char S_C_Y_ENV[] = "C Y Env";
	static const char S_A_X_LFO[] = "A X LFO";
	static const char S_A_Y_LFO[] = "A Y LFO";
	static const char S_B_X_LFO[] = "B X LFO";
	//static const char S_AMP_ENV[] = "Amp Env";
	static const char S_B_Y_LFO[] = "B Y LFO";
	static const char S_C_X_LFO[] = "C X LFO";
	//static const char S_AMP_BIAS[] = "Amp Bias";
	static const char S_C_Y_LFO[] = "C Y LFO";

	//SETTINGS
	static const char S_ZONE_ENABLE[] = "Zone Enable";
	static const char S_MASTER_SYNTH[] = "Master Synth";
	static const char S_SYNTHS[] = "Synths";
	static const char S_LOWER_NOTE[] = "Lower Note";
	static const char S_UPPER_NOTE[] = "Upper Note";
	static const char S_VOICE_ASSIGN[] = "Voice Assign";
	static const char S_LFO_MODE[] = "LFO Mode";
	static const char S_FIRM[] = "Firm";
	static const char S_WARE[] = "ware";
	static const char S_BPM[] = "BPM";
	static const char S_CLICK_ON[] = "Click On";
	static const char S_CLICK_ZONE[] = "Click Zone";
	static const char S_EDIT_FXCARD[] = "Edit->FxCard";
	static const char S_FXCARD_EDIT[] = "FxCard->Edit";
	static const char S_CTRLS_EDIT[] = "Ctrls->Edit";

	//KEYBED
	static const char S_X_AXIS_RANGE[] = "X Axis Range";
	static const char S_UP_RANGE[] = "Up Range";
	static const char S_DOWN_RANGE[] = "Down Range";
	static const char S_ATOUCH_RANGE[] = "ATouch Range";
	static const char S_X_AXIS_CC[] = "X Axis CC";
	static const char S_UP_CC[] = "Up CC";
	static const char S_DOWN_CC[] = "Down CC";
	static const char S_ATOUCH_CC[] = "ATouch CC";
	static const char S_X_AXIS_CHAN[] = "X Axis Chan";
	static const char S_UP_CHAN[] = "Up Chan";
	static const char S_DOWN_CHAN[] = "Down Chan";
	static const char S_AFTOUCH_CHAN[] = "AfTouch Chan";
	//static const char S_OCTAVE[] = "Octave";
	static const char S_VELOCITY_AMT[] = "Velocity Amt";

	//FILTER
	//static const char S_FILTER_FREQ[] = "Filter Freq";
	//static const char S_FILTER_RES[] = "Filter Res";
	static const char S_KEYBD_TRKING[] = "Keybd Trking";
	//static const char S_FILTER_ENV[] = "Filter Env";
	static const char S_ATTACK[] = "Attack";
	static const char S_DECAY[] = "Decay";
	static const char S_SUSTAIN[] = "Sustain";
	static const char S_RELEASE[] = "Release";
	static const char S_FILT_LFO[] = "Filt LFO";
	//static const char S_LFO_DIVISION[] = "LFO Division";
	//static const char S_INVERT[] = "Invert";
	static const char S_MODE[] = "Mode";
	static const char S_LHSB[] = "LHSB";
	static const char S_ENV[] = "Env";
	//static const char S_EXPINV[] = "ExpInv";
	//static const char S_LFO[] = "LFO";
	//static const char S_WAVE[] = "Wave";

	//MIXER
	//static const char S_AMP_BIAS[] = "Amp Bias";
	static const char S_MIXCUE[] = "MixCue";
	//static const char S_AMP_ENV[] = "Amp Env";
	//static const char S_ATTACK[] = "Attack";
	//static const char S_DECAY[] = "Decay";
	//static const char S_SUSTAIN[] = "Sustain";
	//static const char S_RELEASE[] = "Release";
	//static const char S_AMP_LFO[] = "Amp LFO";
	static const char S_LFO_DIVISION[] = "LFO Division";
	//static const char S_INVERT[] = "Invert";
	static const char S_PAN_FX[] = "Pan/FX";
	static const char S_LRSFX[] = "LRSFX";
	//static const char S_ENV[] = "Env";
	//static const char S_EXPINV[] = "ExpInv";
	//static const char S_LFO[] = "LFO";
	//static const char S_WAVE[] = "Wave";
	static const char S_FX[] = "FX";
	static const char S_PATCH[] = "Patch";
	static const char S_FX_PARAM_1[] = "FX Param 1";
	static const char S_FX_PARAM_2[] = "FX Param 2";
	static const char S_FX_PARAM_3[] = "FX Param 3";
	static const char S_FX_SEND[] = "FX Send";
	static const char S_FX_LFO[] = "FX LFO";
	static const char S_123S[] = "123S";
	static const char S_FX_LFO_ZONE[] = "FX LFO Zone";


	//YM
	static const char S_SQUARE[] = "Square";
	static const char S_1[] = "1";
	static const char S_2[] = "2";
	static const char S_3[] = "3";
	static const char S_BUZZ[] = "Buzz";
	static const char S_OSC[] = "Osc";
	static const char S_FINE_TUNE_1[] = "Fine Tune 1";
	static const char S_FINE_TUNE_2[] = "Fine Tune 2";
	static const char S_FINE_TUNE_3[] = "Fine Tune 3";
	// static const char S_COARSE[] = "Coarse";
	static const char S_TUNE_1[] = "Tune 1";
	static const char S_TUNE_2[] = "Tune 2";
	static const char S_TUNE_3[] = "Tune 3";
	//static const char S_OCTAVE[] = "Octave";
	// static const char S_A_ENV[] = "A Env";
	//static const char S_A[] = "A";
	//static const char S_D[] = "D";
	//static const char S_S[] = "S";
	//static const char S_R[] = "R";
	//static const char S_EXPINV[] = "ExpInv";
	//static const char S_M_ENV[] = "M Env";
	static const char S_FINE_TUNE_BZ[] = "Fine Tune Bz";
	static const char S_TUNEBZ[] = "TuneBz";
	static const char S_NOISE1[] = "Noise1";
	// static const char S_LENGTH[] = "Length";
	static const char S_NOISE2[] = "Noise2";
	static const char S_NOISE3[] = "Noise3";
	// static const char S_LFO[] = "LFO";
	//static const char S_WAVE[] = "Wave";
	// static const char S_DIV[] = "Div";
	// static const char S_INVERT[] = "Invert";
	// static const char S_ARP[] = "Arp";
	//static const char S_TYPE[] = "Type";
	// static const char S_PPONG[] = "PPong";
	static const char S_PITCH_ENV_1[] = "Pitch Env 1";
	static const char S_PITCH_ENV_2[] = "Pitch Env 2";
	static const char S_PITCH_ENV_3[] = "Pitch Env 3";
	static const char S_PITCH_ENV_BZ[] = "Pitch Env Bz";
	static const char S_NOISE_FREQ[] = "Noise Freq";
	static const char S_NOISE_ENV[] = "Noise Env";
	static const char S_NOISE_LFO[] = "Noise LFO";
	static const char S_PITCH_LFO_1[] = "Pitch LFO 1";
	static const char S_PITCH_LFO_2[] = "Pitch LFO 2";
	static const char S_PITCH_LFO_3[] = "Pitch LFO 3";
	static const char S_PITCH_LFO_BZ[] = "Pitch LFO Bz";
	// static const char S_AMP_BIAS[] = "Amp Bias";
	// static const char S_AMP_ENV[] = "Amp Env";
	// static const char S_AMP_LFO[] = "Amp LFO";
	// static const char S_UNISON[] = "Mono";
	// static const char S_LOAD[] = "Load";
	//static const char S_PATCH[] = "Patch";
	// static const char S_SAVE[] = "Save";
}



#endif /* ATXCARDPROGMEM_H_ */

