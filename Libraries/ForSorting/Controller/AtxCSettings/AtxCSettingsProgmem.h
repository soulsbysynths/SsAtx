///*
//* AtxCSettingsProgmem.h
//*
//* Created: 15/01/2019 21:16:27
//*  Author: info
//*/
//
//
//#ifndef ATXCSETTINGSPROGMEM_H_
//#define ATXCSETTINGSPROGMEM_H_
//#include <stddef.h>
//#include "Atx.h"
//#include "AtxCcInfo.h"
//
//static const char* const S_SETTINGS_TOP[16] = {"Zone"  ,"MIDI" ,"Sequen","Synth", "Load/","Hard" ,"","","","","","","","","",""};
//static const char* const S_SETTINGS_BOT[16] = {"Setup" ,"Setup","cer"   ,"Setup", "Save" ,"ware" ,"","","","","","","","","",""};
//static const uint8_t SETTINGS_PERZONE[16] =	  {1       ,1      ,1       ,2      ,0       ,0      ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 };
//
//using namespace atx;
//
//static const CcInfo SET_PARAM_INFO[232] =
//{
	//{
		//"","",16,0,0 // 0 SET_HELLO
	//},
	//{
		//S_TX_MPE,S_ZONES,0,0,0 // 1 SET_TX_MPE
	//},
	//{
		//S_MIDI,S_CTRL,1,2,0 // 2 SET_SEQ_MIDICTRL
	//},
	//{
		//S_BPM,"",8,12,30 // 3 SET_SEQ_BPM
	//},
	//{
		//S_LOAD,"",6,96,9 // 4 SET_LS_LOAD
	//},
	//{
		//S_SAVE,"",6,96,8 // 5 SET_LS_SAVE
	//},
	//{
		//S_ZONE,"",3,12,1 // 6 SET_LS_ZONE
	//},
	//{
		//S_DATA,"",2,15,7 // 7 SET_LS_DATA
	//},
	//{
		//S_USB_HOST,"",1,2,0 // 8 SET_HW_USBHOST
	//},
	//{
		//S_EXT_CLK,"",1,2,0 // 9 SET_SEQ_EXTCLK
	//},
	//{
		//S_EXT_ENABLE,"",1,146,0 // 10 SET_HW_EXTENABLE
	//},
	//{
		//S_EXT_MASTER,"",3,156,1 // 11 SET_HW_EXTMASTER
	//},
	//{
		//S_EXT_CARDS,"",3,156,1 // 12 SET_HW_EXTCARDS
	//},
	//{
		//"","",0,0,0 // 13 SET_UNUSED_13
	//},
	//{
		//"","",0,0,0 // 14 SET_UNUSED_14
	//},
	//{
		//"","",0,0,0 // 15 SET_UNUSED_15
	//},
	//{
		//"","",0,0,0 // 16 SET_UNUSED_16
	//},
	//{
		//"","",0,0,0 // 17 SET_UNUSED_17
	//},
	//{
		//"","",0,0,0 // 18 SET_UNUSED_18
	//},
	//{
		//"","",0,0,0 // 19 SET_UNUSED_19
	//},
	//{
		//"","",0,0,0 // 20 SET_UNUSED_20
	//},
	//{
		//"","",0,0,0 // 21 SET_UNUSED_21
	//},
	//{
		//"","",0,0,0 // 22 SET_UNUSED_22
	//},
	//{
		//"","",0,0,0 // 23 SET_UNUSED_23
	//},
	//{
		//"","",0,0,0 // 24 SET_UNUSED_24
	//},
	//{
		//"","",0,0,0 // 25 SET_UNUSED_25
	//},
	//{
		//"","",0,0,0 // 26 SET_UNUSED_26
	//},
	//{
		//"","",0,0,0 // 27 SET_UNUSED_27
	//},
	//{
		//"","",0,0,0 // 28 SET_UNUSED_28
	//},
	//{
		//"","",0,0,0 // 29 SET_UNUSED_29
	//},
	//{
		//"","",0,0,0 // 30 SET_UNUSED_30
	//},
	//{
		//"","",0,0,0 // 31 SET_UNUSED_31
	//},
	//{
		//"","",0,0,0 // 32 SET_UNUSED_32
	//},
	//{
		//"","",0,0,0 // 33 SET_UNUSED_33
	//},
	//{
		//"","",0,0,0 // 34 SET_UNUSED_34
	//},
	//{
		//"","",0,0,0 // 35 SET_UNUSED_35
	//},
	//{
		//"","",0,0,0 // 36 SET_UNUSED_36
	//},
	//{
		//"","",0,0,0 // 37 SET_UNUSED_37
	//},
	//{
		//"","",0,0,0 // 38 SET_UNUSED_38
	//},
	//{
		//"","",0,0,0 // 39 SET_UNUSED_39
	//},
	//{
		//"","",0,0,0 // 40 SET_UNUSED_40
	//},
	//{
		//"","",0,0,0 // 41 SET_UNUSED_41
	//},
	//{
		//"","",0,0,0 // 42 SET_UNUSED_42
	//},
	//{
		//"","",0,0,0 // 43 SET_UNUSED_43
	//},
	//{
		//"","",0,0,0 // 44 SET_UNUSED_44
	//},
	//{
		//"","",0,0,0 // 45 SET_UNUSED_45
	//},
	//{
		//"","",0,0,0 // 46 SET_UNUSED_46
	//},
	//{
		//"","",0,0,0 // 47 SET_UNUSED_47
	//},
	//{
		//"","",0,0,0 // 48 SET_UNUSED_48
	//},
	//{
		//"","",0,0,0 // 49 SET_UNUSED_49
	//},
	//{
		//"","",0,0,0 // 50 SET_UNUSED_50
	//},
	//{
		//"","",0,0,0 // 51 SET_UNUSED_51
	//},
	//{
		//"","",0,0,0 // 52 SET_UNUSED_52
	//},
	//{
		//"","",0,0,0 // 53 SET_UNUSED_53
	//},
	//{
		//"","",0,0,0 // 54 SET_UNUSED_54
	//},
	//{
		//"","",0,0,0 // 55 SET_UNUSED_55
	//},
	//{
		//"","",0,0,0 // 56 SET_UNUSED_56
	//},
	//{
		//"","",0,0,0 // 57 SET_UNUSED_57
	//},
	//{
		//"","",0,0,0 // 58 SET_UNUSED_58
	//},
	//{
		//"","",0,0,0 // 59 SET_UNUSED_59
	//},
	//{
		//"","",0,0,0 // 60 SET_UNUSED_60
	//},
	//{
		//"","",0,0,0 // 61 SET_UNUSED_61
	//},
	//{
		//"","",0,0,0 // 62 SET_UNUSED_62
	//},
	//{
		//"","",0,0,0 // 63 SET_UNUSED_63
	//},
	//{
		//"","",0,0,0 // 64 SET_UNUSED_64
	//},
	//{
		//"","",0,0,0 // 65 SET_UNUSED_65
	//},
	//{
		//"","",0,0,0 // 66 SET_UNUSED_66
	//},
	//{
		//"","",0,0,0 // 67 SET_UNUSED_67
	//},
	//{
		//"","",0,0,0 // 68 SET_UNUSED_68
	//},
	//{
		//"","",0,0,0 // 69 SET_UNUSED_69
	//},
	//{
		//"","",0,0,0 // 70 SET_UNUSED_70
	//},
	//{
		//"","",0,0,0 // 71 SET_UNUSED_71
	//},
	//{
		//"","",0,0,0 // 72 SET_UNUSED_72
	//},
	//{
		//"","",0,0,0 // 73 SET_UNUSED_73
	//},
	//{
		//"","",0,0,0 // 74 SET_UNUSED_74
	//},
	//{
		//"","",0,0,0 // 75 SET_UNUSED_75
	//},
	//{
		//"","",0,0,0 // 76 SET_UNUSED_76
	//},
	//{
		//"","",0,0,0 // 77 SET_UNUSED_77
	//},
	//{
		//"","",0,0,0 // 78 SET_UNUSED_78
	//},
	//{
		//"","",0,0,0 // 79 SET_UNUSED_79
	//},
	//{
		//"","",0,0,0 // 80 SET_UNUSED_80
	//},
	//{
		//"","",0,0,0 // 81 SET_UNUSED_81
	//},
	//{
		//"","",0,0,0 // 82 SET_UNUSED_82
	//},
	//{
		//"","",0,0,0 // 83 SET_UNUSED_83
	//},
	//{
		//"","",0,0,0 // 84 SET_UNUSED_84
	//},
	//{
		//"","",0,0,0 // 85 SET_UNUSED_85
	//},
	//{
		//"","",0,0,0 // 86 SET_UNUSED_86
	//},
	//{
		//"","",0,0,0 // 87 SET_UNUSED_87
	//},
	//{
		//"","",0,0,0 // 88 SET_UNUSED_88
	//},
	//{
		//"","",0,0,0 // 89 SET_UNUSED_89
	//},
	//{
		//"","",0,0,0 // 90 SET_UNUSED_90
	//},
	//{
		//"","",0,0,0 // 91 SET_UNUSED_91
	//},
	//{
		//"","",0,0,0 // 92 SET_UNUSED_92
	//},
	//{
		//"","",0,0,0 // 93 SET_UNUSED_93
	//},
	//{
		//"","",0,0,0 // 94 SET_UNUSED_94
	//},
	//{
		//"","",0,0,0 // 95 SET_UNUSED_95
	//},
	//{
		//"","",0,0,0 // 96 SET_UNUSED_96
	//},
	//{
		//"","",0,0,0 // 97 SET_UNUSED_97
	//},
	//{
		//"","",0,0,0 // 98 SET_UNUSED_98
	//},
	//{
		//"","",0,0,0 // 99 SET_UNUSED_99
	//},
	//{
		//"","",0,0,0 // 100 SET_UNUSED_100
	//},
	//{
		//"","",0,0,0 // 101 SET_UNUSED_101
	//},
	//{
		//"","",0,0,0 // 102 SET_UNUSED_102
	//},
	//{
		//"","",0,0,0 // 103 SET_UNUSED_103
	//},
	//{
		//"","",0,0,0 // 104 SET_UNUSED_104
	//},
	//{
		//"","",0,0,0 // 105 SET_UNUSED_105
	//},
	//{
		//"","",0,0,0 // 106 SET_UNUSED_106
	//},
	//{
		//"","",0,0,0 // 107 SET_UNUSED_107
	//},
	//{
		//"","",0,0,0 // 108 SET_UNUSED_108
	//},
	//{
		//"","",0,0,0 // 109 SET_UNUSED_109
	//},
	//{
		//"","",0,0,0 // 110 SET_UNUSED_110
	//},
	//{
		//"","",0,0,0 // 111 SET_UNUSED_111
	//},
	//{
		//"","",0,0,0 // 112 SET_UNUSED_112
	//},
	//{
		//"","",0,0,0 // 113 SET_UNUSED_113
	//},
	//{
		//"","",0,0,0 // 114 SET_UNUSED_114
	//},
	//{
		//"","",0,0,0 // 115 SET_UNUSED_115
	//},
	//{
		//"","",0,0,0 // 116 SET_UNUSED_116
	//},
	//{
		//"","",0,0,0 // 117 SET_UNUSED_117
	//},
	//{
		//"","",0,0,0 // 118 SET_UNUSED_118
	//},
	//{
		//"","",0,0,0 // 119 SET_UNUSED_119
	//},
	//{
		//"","",0,0,0 // 120 SET_UNUSED_120
	//},
	//{
		//"","",0,0,0 // 121 SET_UNUSED_121
	//},
	//{
		//"","",0,0,0 // 122 SET_UNUSED_122
	//},
	//{
		//"","",0,0,0 // 123 SET_UNUSED_123
	//},
	//{
		//"","",0,0,0 // 124 SET_UNUSED_124
	//},
	//{
		//"","",0,0,0 // 125 SET_UNUSED_125
	//},
	//{
		//"","",0,0,0 // 126 SET_UNUSED_126
	//},
	//{
		//"","",0,0,0 // 127 SET_UNUSED_127
	//},
	//{
		//S_FIRM,S_WARE,4,157,0 // 128 SET_FW_Z1
	//},
	//{
		//S_FIRM,S_WARE,4,157,0 // 129 SET_FW_Z2
	//},
	//{
		//S_FIRM,S_WARE,4,157,0 // 130 SET_FW_Z3
	//},
	//{
		//S_FIRM,S_WARE,4,157,0 // 131 SET_FW_Z4
	//},
	//{
		//S_FIRM,S_WARE,4,157,0 // 132 SET_FW_Z5
	//},
	//{
		//S_FIRM,S_WARE,4,157,0 // 133 SET_FW_Z6
	//},
	//{
		//S_FIRM,S_WARE,4,157,0 // 134 SET_FW_Z7
	//},
	//{
		//S_FIRM,S_WARE,4,157,0 // 135 SET_FW_Z8
	//},
	//{
		//S_ZONE_ENABLE,"",1,146,0 // 136 SET_ZONEENABLE_Z1
	//},
	//{
		//S_ZONE_ENABLE,"",1,146,0 // 137 SET_ZONEENABLE_Z2
	//},
	//{
		//S_ZONE_ENABLE,"",1,146,0 // 138 SET_ZONEENABLE_Z3
	//},
	//{
		//S_ZONE_ENABLE,"",1,146,0 // 139 SET_ZONEENABLE_Z4
	//},
	//{
		//S_ZONE_ENABLE,"",1,146,0 // 140 SET_ZONEENABLE_Z5
	//},
	//{
		//S_ZONE_ENABLE,"",1,146,0 // 141 SET_ZONEENABLE_Z6
	//},
	//{
		//S_ZONE_ENABLE,"",1,146,0 // 142 SET_ZONEENABLE_Z7
	//},
	//{
		//S_ZONE_ENABLE,"",1,146,0 // 143 SET_ZONEENABLE_Z8
	//},
	//{
		//S_MASTER_CARD,"",3,156,1 // 144 SET_MASTER_Z1
	//},
	//{
		//S_MASTER_CARD,"",3,156,1 // 145 SET_MASTER_Z2
	//},
	//{
		//S_MASTER_CARD,"",3,156,1 // 146 SET_MASTER_Z3
	//},
	//{
		//S_MASTER_CARD,"",3,156,1 // 147 SET_MASTER_Z4
	//},
	//{
		//S_MASTER_CARD,"",3,156,1 // 148 SET_MASTER_Z5
	//},
	//{
		//S_MASTER_CARD,"",3,156,1 // 149 SET_MASTER_Z6
	//},
	//{
		//S_MASTER_CARD,"",3,156,1 // 150 SET_MASTER_Z7
	//},
	//{
		//S_MASTER_CARD,"",3,156,1 // 151 SET_MASTER_Z8
	//},
	//{
		//S_CARDS,"",3,156,1 // 152 SET_CARDS_Z1
	//},
	//{
		//S_CARDS,"",3,156,1 // 153 SET_CARDS_Z2
	//},
	//{
		//S_CARDS,"",3,156,1 // 154 SET_CARDS_Z3
	//},
	//{
		//S_CARDS,"",3,156,1 // 155 SET_CARDS_Z4
	//},
	//{
		//S_CARDS,"",3,156,1 // 156 SET_CARDS_Z5
	//},
	//{
		//S_CARDS,"",3,156,1 // 157 SET_CARDS_Z6
	//},
	//{
		//S_CARDS,"",3,156,1 // 158 SET_CARDS_Z7
	//},
	//{
		//S_CARDS,"",3,156,1 // 159 SET_CARDS_Z8
	//},
	//{
		//S_LOWER_NOTE,"",7,6,0 // 160 SET_LOWERNOTE_Z1
	//},
	//{
		//S_LOWER_NOTE,"",7,6,0 // 161 SET_LOWERNOTE_Z2
	//},
	//{
		//S_LOWER_NOTE,"",7,6,0 // 162 SET_LOWERNOTE_Z3
	//},
	//{
		//S_LOWER_NOTE,"",7,6,0 // 163 SET_LOWERNOTE_Z4
	//},
	//{
		//S_LOWER_NOTE,"",7,6,0 // 164 SET_LOWERNOTE_Z5
	//},
	//{
		//S_LOWER_NOTE,"",7,6,0 // 165 SET_LOWERNOTE_Z6
	//},
	//{
		//S_LOWER_NOTE,"",7,6,0 // 166 SET_LOWERNOTE_Z7
	//},
	//{
		//S_LOWER_NOTE,"",7,6,0 // 167 SET_LOWERNOTE_Z8
	//},
	//{
		//S_UPPER_NOTE,"",7,6,0 // 168 SET_UPPERNOTE_Z1
	//},
	//{
		//S_UPPER_NOTE,"",7,6,0 // 169 SET_UPPERNOTE_Z2
	//},
	//{
		//S_UPPER_NOTE,"",7,6,0 // 170 SET_UPPERNOTE_Z3
	//},
	//{
		//S_UPPER_NOTE,"",7,6,0 // 171 SET_UPPERNOTE_Z4
	//},
	//{
		//S_UPPER_NOTE,"",7,6,0 // 172 SET_UPPERNOTE_Z5
	//},
	//{
		//S_UPPER_NOTE,"",7,6,0 // 173 SET_UPPERNOTE_Z6
	//},
	//{
		//S_UPPER_NOTE,"",7,6,0 // 174 SET_UPPERNOTE_Z7
	//},
	//{
		//S_UPPER_NOTE,"",7,6,0 // 175 SET_UPPERNOTE_Z8
	//},
	//{
		//S_MST_PCH_BEND,"",7,3,0 // 176 SET_MSTPCHBND_Z1
	//},
	//{
		//S_MST_PCH_BEND,"",7,3,0 // 177 SET_MSTPCHBND_Z2
	//},
	//{
		//S_MST_PCH_BEND,"",7,3,0 // 178 SET_MSTPCHBND_Z3
	//},
	//{
		//S_MST_PCH_BEND,"",7,3,0 // 179 SET_MSTPCHBND_Z4
	//},
	//{
		//S_MST_PCH_BEND,"",7,3,0 // 180 SET_MSTPCHBND_Z5
	//},
	//{
		//S_MST_PCH_BEND,"",7,3,0 // 181 SET_MSTPCHBND_Z6
	//},
	//{
		//S_MST_PCH_BEND,"",7,3,0 // 182 SET_MSTPCHBND_Z7
	//},
	//{
		//S_MST_PCH_BEND,"",7,3,0 // 183 SET_MSTPCHBND_Z8
	//},
	//{
		//S_P_N_PCH_BEND,"",7,3,0 // 184 SET_PNPCHBND_Z1
	//},
	//{
		//S_P_N_PCH_BEND,"",7,3,0 // 185 SET_PNPCHBND_Z2
	//},
	//{
		//S_P_N_PCH_BEND,"",7,3,0 // 186 SET_PNPCHBND_Z3
	//},
	//{
		//S_P_N_PCH_BEND,"",7,3,0 // 187 SET_PNPCHBND_Z4
	//},
	//{
		//S_P_N_PCH_BEND,"",7,3,0 // 188 SET_PNPCHBND_Z5
	//},
	//{
		//S_P_N_PCH_BEND,"",7,3,0 // 189 SET_PNPCHBND_Z6
	//},
	//{
		//S_P_N_PCH_BEND,"",7,3,0 // 190 SET_PNPCHBND_Z7
	//},
	//{
		//S_P_N_PCH_BEND,"",7,3,0 // 191 SET_PNPCHBND_Z8
	//},
	//{
		//S_TRIGGER_ZONE,"",3,12,1 // 192 SET_TRIGGERZONE_Z1
	//},
	//{
		//S_TRIGGER_ZONE,"",3,12,1 // 193 SET_TRIGGERZONE_Z2
	//},
	//{
		//S_TRIGGER_ZONE,"",3,12,1 // 194 SET_TRIGGERZONE_Z3
	//},
	//{
		//S_TRIGGER_ZONE,"",3,12,1 // 195 SET_TRIGGERZONE_Z4
	//},
	//{
		//S_TRIGGER_ZONE,"",3,12,1 // 196 SET_TRIGGERZONE_Z5
	//},
	//{
		//S_TRIGGER_ZONE,"",3,12,1 // 197 SET_TRIGGERZONE_Z6
	//},
	//{
		//S_TRIGGER_ZONE,"",3,12,1 // 198 SET_TRIGGERZONE_Z7
	//},
	//{
		//S_TRIGGER_ZONE,"",3,12,1 // 199 SET_TRIGGERZONE_Z8
	//},
	//{
		//S_MPE_MIDI,"",1,2,0 // 200 SET_MIDIMPE_1
	//},
	//{
		//S_MPE_MIDI,"",1,2,0 // 201 SET_MIDIMPE_2
	//},
	//{
		//S_MPE_MIDI,"",1,2,0 // 202 SET_MIDIMPE_3
	//},
	//{
		//S_MPE_MIDI,"",1,2,0 // 203 SET_MIDIMPE_4
	//},
	//{
		//S_MPE_MIDI,"",1,2,0 // 204 SET_MIDIMPE_5
	//},
	//{
		//S_MPE_MIDI,"",1,2,0 // 205 SET_MIDIMPE_6
	//},
	//{
		//S_MPE_MIDI,"",1,2,0 // 206 SET_MIDIMPE_7
	//},
	//{
		//S_MPE_MIDI,"",1,2,0 // 207 SET_MIDIMPE_8
	//},
	//{
		//S_MIDI_CHANNEL,"",4,12,1 // 208 SET_MIDICHAN_Z1
	//},
	//{
		//S_MIDI_CHANNEL,"",4,12,1 // 209 SET_MIDICHAN_Z2
	//},
	//{
		//S_MIDI_CHANNEL,"",4,12,1 // 210 SET_MIDICHAN_Z3
	//},
	//{
		//S_MIDI_CHANNEL,"",4,12,1 // 211 SET_MIDICHAN_Z4
	//},
	//{
		//S_MIDI_CHANNEL,"",4,12,1 // 212 SET_MIDICHAN_Z5
	//},
	//{
		//S_MIDI_CHANNEL,"",4,12,1 // 213 SET_MIDICHAN_Z6
	//},
	//{
		//S_MIDI_CHANNEL,"",4,12,1 // 214 SET_MIDICHAN_Z7
	//},
	//{
		//S_MIDI_CHANNEL,"",4,12,1 // 215 SET_MIDICHAN_Z8
	//},
	//{
		//S_EXT_CV_PARAM,"",7,14,0 // 216 SET_SYNTHEXTCVPRM_S1
	//},
	//{
		//S_EXT_CV_PARAM,"",7,14,0 // 217 SET_SYNTHEXTCVPRM_S2
	//},
	//{
		//S_EXT_CV_PARAM,"",7,14,0 // 218 SET_SYNTHEXTCVPRM_S3
	//},
	//{
		//S_EXT_CV_PARAM,"",7,14,0 // 219 SET_SYNTHEXTCVPRM_S4
	//},
	//{
		//S_EXT_CV_PARAM,"",7,14,0 // 220 SET_SYNTHEXTCVPRM_S5
	//},
	//{
		//S_EXT_CV_PARAM,"",7,14,0 // 221 SET_SYNTHEXTCVPRM_S6
	//},
	//{
		//S_EXT_CV_PARAM,"",7,14,0 // 222 SET_SYNTHEXTCVPRM_S7
	//},
	//{
		//S_EXT_CV_PARAM,"",7,14,0 // 223 SET_SYNTHEXTCVPRM_S8
	//},
	//{
		//S_VOICE_ASSIGN,"",2,15,16 // 224 SET_VOICEASSIGN_Z1
	//},
	//{
		//S_VOICE_ASSIGN,"",2,15,16 // 225 SET_VOICEASSIGN_Z2
	//},
	//{
		//S_VOICE_ASSIGN,"",2,15,16 // 226 SET_VOICEASSIGN_Z3
	//},
	//{
		//S_VOICE_ASSIGN,"",2,15,16 // 227 SET_VOICEASSIGN_Z4
	//},
	//{
		//S_VOICE_ASSIGN,"",2,15,16 // 228 SET_VOICEASSIGN_Z5
	//},
	//{
		//S_VOICE_ASSIGN,"",2,15,16 // 229 SET_VOICEASSIGN_Z6
	//},
	//{
		//S_VOICE_ASSIGN,"",2,15,16 // 230 SET_VOICEASSIGN_Z7
	//},
	//{
		//S_VOICE_ASSIGN,"",2,15,16 // 231 SET_VOICEASSIGN_Z8
	//}
//};
//
//#endif /* ATXCSETTINGSPROGMEM_H_ */