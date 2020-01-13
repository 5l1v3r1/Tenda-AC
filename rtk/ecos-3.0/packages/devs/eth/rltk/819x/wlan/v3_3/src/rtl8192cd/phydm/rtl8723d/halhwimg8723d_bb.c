/****************************************************************************** 
* 
* Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved. 
* 
* This program is free software; you can redistribute it and/or modify it 
* under the terms of version 2 of the GNU General Public License as 
* published by the Free Software Foundation. 
* 
* This program is distributed in the hope that it will be useful, but WITHOUT 
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
* more details. 
* 
* You should have received a copy of the GNU General Public License along with 
* this program; if not, write to the Free Software Foundation, Inc., 
* 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA 
* 
* 
******************************************************************************/

#include "Mp_Precomp.h"
#include "../phydm_precomp.h"

#if (RTL8723D_SUPPORT == 1)
static BOOLEAN
CheckPositive(
    IN  PDM_ODM_T     pDM_Odm,
    IN  const u4Byte  Condition1,
    IN  const u4Byte  Condition2
    )
{
    u1Byte    _BoardType = ((pDM_Odm->BoardType & BIT4) >> 4) << 0 | // _GLNA
                           ((pDM_Odm->BoardType & BIT3) >> 3) << 1 | // _GPA 
                           ((pDM_Odm->BoardType & BIT7) >> 7) << 2 | // _ALNA
                           ((pDM_Odm->BoardType & BIT6) >> 6) << 3 | // _APA 
                           ((pDM_Odm->BoardType & BIT2) >> 2) << 4;  // _BT  

	u4Byte 	  cond1   = Condition1, cond2 = Condition2;
	u4Byte    driver1 = pDM_Odm->CutVersion       << 24 |  
		                pDM_Odm->SupportPlatform  << 16 | 
		                pDM_Odm->PackageType      << 12 | 
		                pDM_Odm->SupportInterface << 8  |
		                _BoardType;

	u4Byte    driver2 = pDM_Odm->TypeGLNA <<  0 |  
		                pDM_Odm->TypeGPA  <<  8 | 
		                pDM_Odm->TypeALNA << 16 | 
		                pDM_Odm->TypeAPA  << 24; 

    ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
                ("===> [8812A] CheckPositive (cond1, cond2) = (0x%X 0x%X)\n", cond1, cond2));
    ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
                ("===> [8812A] CheckPositive (driver1, driver2) = (0x%X 0x%X)\n", driver1, driver2));

    ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
                ("	(Platform, Interface) = (0x%X, 0x%X)\n", pDM_Odm->SupportPlatform, pDM_Odm->SupportInterface));
    ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
                ("	(Board, Package) = (0x%X, 0x%X)\n", pDM_Odm->BoardType, pDM_Odm->PackageType));


    // We don't care [31:28] and [23:20]
	cond1 &= 0x0FF0FFFF; 
	driver1 &= 0x0FF0FFFF; 

    if ((cond1 & driver1) == cond1) 
    {
        u4Byte bitMask = 0;
        if ((cond1 & 0xFF) == 0) // BoardType is DONTCARE
            return TRUE;

        if ((cond1 & BIT0) != 0) //GLNA
            bitMask |= 0xFF;
        if ((cond1 & BIT1) != 0) //GPA
            bitMask |= 0xFF00;
        if ((cond1 & BIT2) != 0) //ALNA
            bitMask |= 0xFF0000;
        if ((cond1 & BIT3) != 0) //APA
            bitMask |= 0xFF000000;

        if (((cond2 & driver2) & bitMask) == (cond2 & bitMask)) // BoardType of each RF path is matched
            return TRUE;
        else
            return FALSE;
    }
    else 
    {
        return FALSE;
    }
     
}

static BOOLEAN
CheckNegative(
    IN  PDM_ODM_T     pDM_Odm,
    IN  const u4Byte  Condition1,
    IN  const u4Byte  Condition2
    )
{
    return TRUE;
}

/******************************************************************************
*                           AGC_TAB.TXT
******************************************************************************/

u4Byte Array_MP_8723D_AGC_TAB[] = { 
	0x80000400,0x00000000,0x40000000,0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFB020001,
		0xC78, 0xFB030001,
		0xC78, 0xFB040001,
		0xC78, 0xFB050001,
		0xC78, 0xFB060001,
		0xC78, 0xFB070001,
		0xC78, 0xFA080001,
		0xC78, 0xF9090001,
		0xC78, 0xF80A0001,
		0xC78, 0xF70B0001,
		0xC78, 0xF60C0001,
		0xC78, 0xF50D0001,
		0xC78, 0xF40E0001,
		0xC78, 0xF30F0001,
		0xC78, 0xF2100001,
		0xC78, 0xF1110001,
		0xC78, 0xF0120001,
		0xC78, 0xEF130001,
		0xC78, 0xEE140001,
		0xC78, 0xED150001,
		0xC78, 0xEC160001,
		0xC78, 0xEB170001,
		0xC78, 0xEA180001,
		0xC78, 0xE9190001,
		0xC78, 0xC81A0001,
		0xC78, 0xC71B0001,
		0xC78, 0xC61C0001,
		0xC78, 0x071D0001,
		0xC78, 0x061E0001,
		0xC78, 0x051F0001,
		0xC78, 0x04200001,
		0xC78, 0x03210001,
		0xC78, 0xAA220001,
		0xC78, 0xA9230001,
		0xC78, 0xA8240001,
		0xC78, 0xA7250001,
		0xC78, 0xA6260001,
		0xC78, 0x85270001,
		0xC78, 0x84280001,
		0xC78, 0x83290001,
		0xC78, 0x252A0001,
		0xC78, 0x242B0001,
		0xC78, 0x232C0001,
		0xC78, 0x222D0001,
		0xC78, 0x672E0001,
		0xC78, 0x662F0001,
		0xC78, 0x65300001,
		0xC78, 0x64310001,
		0xC78, 0x63320001,
		0xC78, 0x62330001,
		0xC78, 0x61340001,
		0xC78, 0x45350001,
		0xC78, 0x44360001,
		0xC78, 0x43370001,
		0xC78, 0x42380001,
		0xC78, 0x41390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0x90010003,0x00000000,0x40000000,0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFA020001,
		0xC78, 0xF9030001,
		0xC78, 0xF8040001,
		0xC78, 0xF7050001,
		0xC78, 0xF6060001,
		0xC78, 0xF5070001,
		0xC78, 0xF4080001,
		0xC78, 0xF3090001,
		0xC78, 0xF20A0001,
		0xC78, 0xF10B0001,
		0xC78, 0xF00C0001,
		0xC78, 0xEF0D0001,
		0xC78, 0xEE0E0001,
		0xC78, 0xED0F0001,
		0xC78, 0xEC100001,
		0xC78, 0xEB110001,
		0xC78, 0xEA120001,
		0xC78, 0xE9130001,
		0xC78, 0xE8140001,
		0xC78, 0xE7150001,
		0xC78, 0xE6160001,
		0xC78, 0xE5170001,
		0xC78, 0xE4180001,
		0xC78, 0xE3190001,
		0xC78, 0xE21A0001,
		0xC78, 0xE11B0001,
		0xC78, 0x8A1C0001,
		0xC78, 0x891D0001,
		0xC78, 0x881E0001,
		0xC78, 0x871F0001,
		0xC78, 0x86200001,
		0xC78, 0x85210001,
		0xC78, 0x84220001,
		0xC78, 0x83230001,
		0xC78, 0x82240001,
		0xC78, 0x6A250001,
		0xC78, 0x69260001,
		0xC78, 0x68270001,
		0xC78, 0x67280001,
		0xC78, 0x66290001,
		0xC78, 0x652A0001,
		0xC78, 0x642B0001,
		0xC78, 0x632C0001,
		0xC78, 0x622D0001,
		0xC78, 0x612E0001,
		0xC78, 0x602F0001,
		0xC78, 0x47300001,
		0xC78, 0x46310001,
		0xC78, 0x45320001,
		0xC78, 0x44330001,
		0xC78, 0x43340001,
		0xC78, 0x42350001,
		0xC78, 0x41360001,
		0xC78, 0x40370001,
		0xC78, 0x40380001,
		0xC78, 0x40390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0x90010002,0x00000000,0x40000000,0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFB020001,
		0xC78, 0xFB030001,
		0xC78, 0xFB040001,
		0xC78, 0xFB050001,
		0xC78, 0xFA060001,
		0xC78, 0xF9070001,
		0xC78, 0xF8080001,
		0xC78, 0xF7090001,
		0xC78, 0xF60A0001,
		0xC78, 0xF50B0001,
		0xC78, 0xF40C0001,
		0xC78, 0xF30D0001,
		0xC78, 0xF20E0001,
		0xC78, 0xF10F0001,
		0xC78, 0xF0100001,
		0xC78, 0xEF110001,
		0xC78, 0xEE120001,
		0xC78, 0xED130001,
		0xC78, 0xEC140001,
		0xC78, 0xEB150001,
		0xC78, 0xEA160001,
		0xC78, 0xE9170001,
		0xC78, 0xE8180001,
		0xC78, 0xE7190001,
		0xC78, 0xC81A0001,
		0xC78, 0xC71B0001,
		0xC78, 0xC61C0001,
		0xC78, 0x071D0001,
		0xC78, 0x061E0001,
		0xC78, 0x051F0001,
		0xC78, 0x04200001,
		0xC78, 0x03210001,
		0xC78, 0xAA220001,
		0xC78, 0xA9230001,
		0xC78, 0xA8240001,
		0xC78, 0xA7250001,
		0xC78, 0xA6260001,
		0xC78, 0x85270001,
		0xC78, 0x84280001,
		0xC78, 0x83290001,
		0xC78, 0x252A0001,
		0xC78, 0x242B0001,
		0xC78, 0x232C0001,
		0xC78, 0x222D0001,
		0xC78, 0x672E0001,
		0xC78, 0x662F0001,
		0xC78, 0x65300001,
		0xC78, 0x64310001,
		0xC78, 0x63320001,
		0xC78, 0x62330001,
		0xC78, 0x61340001,
		0xC78, 0x45350001,
		0xC78, 0x44360001,
		0xC78, 0x43370001,
		0xC78, 0x42380001,
		0xC78, 0x41390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0x90010001,0x00000000,0x40000000,0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFA020001,
		0xC78, 0xF9030001,
		0xC78, 0xF8040001,
		0xC78, 0xF7050001,
		0xC78, 0xF6060001,
		0xC78, 0xF5070001,
		0xC78, 0xF4080001,
		0xC78, 0xF3090001,
		0xC78, 0xF20A0001,
		0xC78, 0xF10B0001,
		0xC78, 0xF00C0001,
		0xC78, 0xEF0D0001,
		0xC78, 0xEE0E0001,
		0xC78, 0xED0F0001,
		0xC78, 0xEC100001,
		0xC78, 0xEB110001,
		0xC78, 0xEA120001,
		0xC78, 0xE9130001,
		0xC78, 0xE8140001,
		0xC78, 0xE7150001,
		0xC78, 0xE6160001,
		0xC78, 0xE5170001,
		0xC78, 0xE4180001,
		0xC78, 0xE3190001,
		0xC78, 0xE21A0001,
		0xC78, 0xE11B0001,
		0xC78, 0x8A1C0001,
		0xC78, 0x891D0001,
		0xC78, 0x881E0001,
		0xC78, 0x871F0001,
		0xC78, 0x86200001,
		0xC78, 0x85210001,
		0xC78, 0x84220001,
		0xC78, 0x83230001,
		0xC78, 0x82240001,
		0xC78, 0x6A250001,
		0xC78, 0x69260001,
		0xC78, 0x68270001,
		0xC78, 0x67280001,
		0xC78, 0x66290001,
		0xC78, 0x652A0001,
		0xC78, 0x642B0001,
		0xC78, 0x632C0001,
		0xC78, 0x622D0001,
		0xC78, 0x612E0001,
		0xC78, 0x602F0001,
		0xC78, 0x47300001,
		0xC78, 0x46310001,
		0xC78, 0x45320001,
		0xC78, 0x44330001,
		0xC78, 0x43340001,
		0xC78, 0x42350001,
		0xC78, 0x41360001,
		0xC78, 0x40370001,
		0xC78, 0x40380001,
		0xC78, 0x40390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0xA0000000,0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFB020001,
		0xC78, 0xFB030001,
		0xC78, 0xFB040001,
		0xC78, 0xFB050001,
		0xC78, 0xFA060001,
		0xC78, 0xF9070001,
		0xC78, 0xF8080001,
		0xC78, 0xF7090001,
		0xC78, 0xF60A0001,
		0xC78, 0xF50B0001,
		0xC78, 0xF40C0001,
		0xC78, 0xF30D0001,
		0xC78, 0xF20E0001,
		0xC78, 0xF10F0001,
		0xC78, 0xF0100001,
		0xC78, 0xEF110001,
		0xC78, 0xEE120001,
		0xC78, 0xED130001,
		0xC78, 0xEC140001,
		0xC78, 0xEB150001,
		0xC78, 0xEA160001,
		0xC78, 0xE9170001,
		0xC78, 0xE8180001,
		0xC78, 0xE7190001,
		0xC78, 0xC81A0001,
		0xC78, 0xC71B0001,
		0xC78, 0xC61C0001,
		0xC78, 0x071D0001,
		0xC78, 0x061E0001,
		0xC78, 0x051F0001,
		0xC78, 0x04200001,
		0xC78, 0x03210001,
		0xC78, 0xAA220001,
		0xC78, 0xA9230001,
		0xC78, 0xA8240001,
		0xC78, 0xA7250001,
		0xC78, 0xA6260001,
		0xC78, 0x85270001,
		0xC78, 0x84280001,
		0xC78, 0x83290001,
		0xC78, 0x252A0001,
		0xC78, 0x242B0001,
		0xC78, 0x232C0001,
		0xC78, 0x222D0001,
		0xC78, 0x672E0001,
		0xC78, 0x662F0001,
		0xC78, 0x65300001,
		0xC78, 0x64310001,
		0xC78, 0x63320001,
		0xC78, 0x62330001,
		0xC78, 0x61340001,
		0xC78, 0x45350001,
		0xC78, 0x44360001,
		0xC78, 0x43370001,
		0xC78, 0x42380001,
		0xC78, 0x41390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0xB0000000,0x00000000,
	0x80000400,0x00000000,0x40000000,0x00000000,
		0xC78, 0xFB400001,
		0xC78, 0xFB410001,
		0xC78, 0xFB420001,
		0xC78, 0xFB430001,
		0xC78, 0xFB440001,
		0xC78, 0xFB450001,
		0xC78, 0xFA460001,
		0xC78, 0xF9470001,
		0xC78, 0xF8480001,
		0xC78, 0xF7490001,
		0xC78, 0xF64A0001,
		0xC78, 0xF54B0001,
		0xC78, 0xF44C0001,
		0xC78, 0xF34D0001,
		0xC78, 0xF24E0001,
		0xC78, 0xF14F0001,
		0xC78, 0xF0500001,
		0xC78, 0xEF510001,
		0xC78, 0xEE520001,
		0xC78, 0xED530001,
		0xC78, 0xEC540001,
		0xC78, 0xEB550001,
		0xC78, 0xEA560001,
		0xC78, 0xE9570001,
		0xC78, 0xE8580001,
		0xC78, 0xE7590001,
		0xC78, 0xE65A0001,
		0xC78, 0xE55B0001,
		0xC78, 0xE45C0001,
		0xC78, 0xE35D0001,
		0xC78, 0xE25E0001,
		0xC78, 0xE15F0001,
		0xC78, 0x8A600001,
		0xC78, 0x89610001,
		0xC78, 0x88620001,
		0xC78, 0x87630001,
		0xC78, 0x86640001,
		0xC78, 0x85650001,
		0xC78, 0x84660001,
		0xC78, 0x83670001,
		0xC78, 0x82680001,
		0xC78, 0x6B690001,
		0xC78, 0x6A6A0001,
		0xC78, 0x696B0001,
		0xC78, 0x686C0001,
		0xC78, 0x676D0001,
		0xC78, 0x666E0001,
		0xC78, 0x656F0001,
		0xC78, 0x64700001,
		0xC78, 0x63710001,
		0xC78, 0x62720001,
		0xC78, 0x61730001,
		0xC78, 0x49740001,
		0xC78, 0x48750001,
		0xC78, 0x47760001,
		0xC78, 0x46770001,
		0xC78, 0x45780001,
		0xC78, 0x44790001,
		0xC78, 0x437A0001,
		0xC78, 0x427B0001,
		0xC78, 0x417C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040022,
		0xC50, 0x00040020,
	0x90010003,0x00000000,0x40000000,0x00000000,
		0xC78, 0xF7400001,
		0xC78, 0xF6410001,
		0xC78, 0xF5420001,
		0xC78, 0xF4430001,
		0xC78, 0xF3440001,
		0xC78, 0xF2450001,
		0xC78, 0xF1460001,
		0xC78, 0xF0470001,
		0xC78, 0xEF480001,
		0xC78, 0xEE490001,
		0xC78, 0xED4A0001,
		0xC78, 0xEC4B0001,
		0xC78, 0xEB4C0001,
		0xC78, 0xEA4D0001,
		0xC78, 0xCE4E0001,
		0xC78, 0xCD4F0001,
		0xC78, 0xCC500001,
		0xC78, 0xCB510001,
		0xC78, 0xCA520001,
		0xC78, 0xC9530001,
		0xC78, 0xC8540001,
		0xC78, 0xC7550001,
		0xC78, 0xC6560001,
		0xC78, 0x55570001,
		0xC78, 0xC4580001,
		0xC78, 0xC3590001,
		0xC78, 0x885A0001,
		0xC78, 0x875B0001,
		0xC78, 0x865C0001,
		0xC78, 0x855D0001,
		0xC78, 0x845E0001,
		0xC78, 0x6B5F0001,
		0xC78, 0x6A600001,
		0xC78, 0x69610001,
		0xC78, 0x68620001,
		0xC78, 0x67630001,
		0xC78, 0x66640001,
		0xC78, 0x65650001,
		0xC78, 0x64660001,
		0xC78, 0x63670001,
		0xC78, 0x4A680001,
		0xC78, 0x49690001,
		0xC78, 0x486A0001,
		0xC78, 0x476B0001,
		0xC78, 0x466C0001,
		0xC78, 0x456D0001,
		0xC78, 0x446E0001,
		0xC78, 0x436F0001,
		0xC78, 0x42700001,
		0xC78, 0x41710001,
		0xC78, 0x40720001,
		0xC78, 0x40730001,
		0xC78, 0x40740001,
		0xC78, 0x40750001,
		0xC78, 0x40760001,
		0xC78, 0x40770001,
		0xC78, 0x40780001,
		0xC78, 0x40790001,
		0xC78, 0x407A0001,
		0xC78, 0x407B0001,
		0xC78, 0x407C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040222,
		0xC50, 0x00040220,
	0x90010002,0x00000000,0x40000000,0x00000000,
		0xC78, 0xFB400001,
		0xC78, 0xFB410001,
		0xC78, 0xFB420001,
		0xC78, 0xFB430001,
		0xC78, 0xFB440001,
		0xC78, 0xFB450001,
		0xC78, 0xFA460001,
		0xC78, 0xF9470001,
		0xC78, 0xF8480001,
		0xC78, 0xF7490001,
		0xC78, 0xF64A0001,
		0xC78, 0xF54B0001,
		0xC78, 0xF44C0001,
		0xC78, 0xF34D0001,
		0xC78, 0xF24E0001,
		0xC78, 0xF14F0001,
		0xC78, 0xF0500001,
		0xC78, 0xEF510001,
		0xC78, 0xEE520001,
		0xC78, 0xED530001,
		0xC78, 0xEC540001,
		0xC78, 0xEB550001,
		0xC78, 0xEA560001,
		0xC78, 0xE9570001,
		0xC78, 0xE8580001,
		0xC78, 0xE7590001,
		0xC78, 0xE65A0001,
		0xC78, 0xE55B0001,
		0xC78, 0xE45C0001,
		0xC78, 0xE35D0001,
		0xC78, 0xE25E0001,
		0xC78, 0xE15F0001,
		0xC78, 0x8A600001,
		0xC78, 0x89610001,
		0xC78, 0x88620001,
		0xC78, 0x87630001,
		0xC78, 0x86640001,
		0xC78, 0x85650001,
		0xC78, 0x84660001,
		0xC78, 0x83670001,
		0xC78, 0x82680001,
		0xC78, 0x6B690001,
		0xC78, 0x6A6A0001,
		0xC78, 0x696B0001,
		0xC78, 0x686C0001,
		0xC78, 0x676D0001,
		0xC78, 0x666E0001,
		0xC78, 0x656F0001,
		0xC78, 0x64700001,
		0xC78, 0x63710001,
		0xC78, 0x62720001,
		0xC78, 0x61730001,
		0xC78, 0x49740001,
		0xC78, 0x48750001,
		0xC78, 0x47760001,
		0xC78, 0x46770001,
		0xC78, 0x45780001,
		0xC78, 0x44790001,
		0xC78, 0x437A0001,
		0xC78, 0x427B0001,
		0xC78, 0x417C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040222,
		0xC50, 0x00040220,
	0x90010001,0x00000000,0x40000000,0x00000000,
		0xC78, 0xF7400001,
		0xC78, 0xF6410001,
		0xC78, 0xF5420001,
		0xC78, 0xF4430001,
		0xC78, 0xF3440001,
		0xC78, 0xF2450001,
		0xC78, 0xF1460001,
		0xC78, 0xF0470001,
		0xC78, 0xEF480001,
		0xC78, 0xEE490001,
		0xC78, 0xED4A0001,
		0xC78, 0xEC4B0001,
		0xC78, 0xEB4C0001,
		0xC78, 0xEA4D0001,
		0xC78, 0xCE4E0001,
		0xC78, 0xCD4F0001,
		0xC78, 0xCC500001,
		0xC78, 0xCB510001,
		0xC78, 0xCA520001,
		0xC78, 0xC9530001,
		0xC78, 0xC8540001,
		0xC78, 0xC7550001,
		0xC78, 0xC6560001,
		0xC78, 0x55570001,
		0xC78, 0xC4580001,
		0xC78, 0xC3590001,
		0xC78, 0x885A0001,
		0xC78, 0x875B0001,
		0xC78, 0x865C0001,
		0xC78, 0x855D0001,
		0xC78, 0x845E0001,
		0xC78, 0x6B5F0001,
		0xC78, 0x6A600001,
		0xC78, 0x69610001,
		0xC78, 0x68620001,
		0xC78, 0x67630001,
		0xC78, 0x66640001,
		0xC78, 0x65650001,
		0xC78, 0x64660001,
		0xC78, 0x63670001,
		0xC78, 0x4A680001,
		0xC78, 0x49690001,
		0xC78, 0x486A0001,
		0xC78, 0x476B0001,
		0xC78, 0x466C0001,
		0xC78, 0x456D0001,
		0xC78, 0x446E0001,
		0xC78, 0x436F0001,
		0xC78, 0x42700001,
		0xC78, 0x41710001,
		0xC78, 0x40720001,
		0xC78, 0x40730001,
		0xC78, 0x40740001,
		0xC78, 0x40750001,
		0xC78, 0x40760001,
		0xC78, 0x40770001,
		0xC78, 0x40780001,
		0xC78, 0x40790001,
		0xC78, 0x407A0001,
		0xC78, 0x407B0001,
		0xC78, 0x407C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040222,
		0xC50, 0x00040220,
	0xA0000000,0x00000000,
		0xC78, 0xFB400001,
		0xC78, 0xFB410001,
		0xC78, 0xFB420001,
		0xC78, 0xFB430001,
		0xC78, 0xFB440001,
		0xC78, 0xFB450001,
		0xC78, 0xFA460001,
		0xC78, 0xF9470001,
		0xC78, 0xF8480001,
		0xC78, 0xF7490001,
		0xC78, 0xF64A0001,
		0xC78, 0xF54B0001,
		0xC78, 0xF44C0001,
		0xC78, 0xF34D0001,
		0xC78, 0xF24E0001,
		0xC78, 0xF14F0001,
		0xC78, 0xF0500001,
		0xC78, 0xEF510001,
		0xC78, 0xEE520001,
		0xC78, 0xED530001,
		0xC78, 0xEC540001,
		0xC78, 0xEB550001,
		0xC78, 0xEA560001,
		0xC78, 0xE9570001,
		0xC78, 0xE8580001,
		0xC78, 0xE7590001,
		0xC78, 0xE65A0001,
		0xC78, 0xE55B0001,
		0xC78, 0xE45C0001,
		0xC78, 0xE35D0001,
		0xC78, 0xE25E0001,
		0xC78, 0xE15F0001,
		0xC78, 0x8A600001,
		0xC78, 0x89610001,
		0xC78, 0x88620001,
		0xC78, 0x87630001,
		0xC78, 0x86640001,
		0xC78, 0x85650001,
		0xC78, 0x84660001,
		0xC78, 0x83670001,
		0xC78, 0x82680001,
		0xC78, 0x6B690001,
		0xC78, 0x6A6A0001,
		0xC78, 0x696B0001,
		0xC78, 0x686C0001,
		0xC78, 0x676D0001,
		0xC78, 0x666E0001,
		0xC78, 0x656F0001,
		0xC78, 0x64700001,
		0xC78, 0x63710001,
		0xC78, 0x62720001,
		0xC78, 0x61730001,
		0xC78, 0x49740001,
		0xC78, 0x48750001,
		0xC78, 0x47760001,
		0xC78, 0x46770001,
		0xC78, 0x45780001,
		0xC78, 0x44790001,
		0xC78, 0x437A0001,
		0xC78, 0x427B0001,
		0xC78, 0x417C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040022,
		0xC50, 0x00040020,
	0xB0000000,0x00000000,

};

void
ODM_ReadAndConfig_MP_8723D_AGC_TAB(
 	IN   PDM_ODM_T  pDM_Odm
 	)
{
    u4Byte     i         = 0;
    u4Byte     ArrayLen    = sizeof(Array_MP_8723D_AGC_TAB)/sizeof(u4Byte);
    pu4Byte    Array       = Array_MP_8723D_AGC_TAB;
	
    ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("===> ODM_ReadAndConfig_MP_8723D_AGC_TAB\n"));

    for (i = 0; i < ArrayLen; i += 2 )
    {
        u4Byte v1 = Array[i];
        u4Byte v2 = Array[i+1];
    
        // This (offset, data) pair doesn't care the condition.
        if ( v1 < 0x40000000 )
        {
           odm_ConfigBB_AGC_8723D(pDM_Odm, v1, bMaskDWord, v2);
           continue;
        }
        else
        {   // This line is the beginning of branch.
            BOOLEAN bMatched = TRUE;
            u1Byte  cCond  = (u1Byte)((v1 & (BIT29|BIT28)) >> 28);

            if (cCond == COND_ELSE) { // ELSE, ENDIF
                bMatched = TRUE;
                READ_NEXT_PAIR(v1, v2, i);
            } else if ( ! CheckPositive(pDM_Odm, v1, v2) ) { 
                bMatched = FALSE;
                READ_NEXT_PAIR(v1, v2, i);
                READ_NEXT_PAIR(v1, v2, i);
            } else {
                READ_NEXT_PAIR(v1, v2, i);
                if ( ! CheckNegative(pDM_Odm, v1, v2) )
                    bMatched = FALSE;
                else
                    bMatched = TRUE;
                READ_NEXT_PAIR(v1, v2, i);
            }

            if ( bMatched == FALSE )
            {   // Condition isn't matched. Discard the following (offset, data) pairs.
                while (v1 < 0x40000000 && i < ArrayLen -2)
                    READ_NEXT_PAIR(v1, v2, i);

                i -= 2; // prevent from for-loop += 2
            }
            else // Configure matched pairs and skip to end of if-else.
            {
                while (v1 < 0x40000000 && i < ArrayLen-2) {
                    odm_ConfigBB_AGC_8723D(pDM_Odm, v1, bMaskDWord, v2);
                    READ_NEXT_PAIR(v1, v2, i);
                }

                // Keeps reading until ENDIF.
                cCond = (u1Byte)((v1 & (BIT29|BIT28)) >> 28);
                while (cCond != COND_ENDIF && i < ArrayLen-2) {
                    READ_NEXT_PAIR(v1, v2, i);
                    cCond = (u1Byte)((v1 & (BIT29|BIT28)) >> 28);
                }
            }
        } 
    }
}

u4Byte
ODM_GetVersion_MP_8723D_AGC_TAB(void)
{
	   return 34;
}

/******************************************************************************
*                           PHY_REG.TXT
******************************************************************************/

u4Byte Array_MP_8723D_PHY_REG[] = { 
		0x800, 0x80040000,
		0x804, 0x00000003,
		0x808, 0x0000FC00,
		0x80C, 0x0000000A,
		0x810, 0x10001331,
		0x814, 0x020C3D10,
		0x818, 0x02220385,
		0x81C, 0x00000000,
		0x820, 0x01000100,
		0x824, 0x00390204,
		0x828, 0x01000100,
		0x82C, 0x00390204,
		0x830, 0x32323232,
		0x834, 0x30303030,
		0x838, 0x30303030,
		0x83C, 0x30303030,
		0x840, 0x00010000,
		0x844, 0x00010000,
		0x848, 0x28282828,
		0x84C, 0x28282828,
		0x850, 0x00000000,
		0x854, 0x00000000,
		0x858, 0x009A009A,
		0x85C, 0x01000014,
		0x860, 0x66F60000,
		0x864, 0x061F0000,
		0x868, 0x30303030,
		0x86C, 0x30303030,
		0x870, 0x00000000,
		0x874, 0x55004200,
		0x878, 0x08080808,
		0x87C, 0x00000000,
		0x880, 0xB0000C1C,
		0x884, 0x00000001,
		0x888, 0x00000000,
		0x88C, 0xCC0000C0,
		0x890, 0x00000800,
		0x894, 0xFFFFFFFE,
		0x898, 0x40302010,
		0x900, 0x00000000,
		0x904, 0x00000023,
		0x908, 0x00000000,
		0x90C, 0x81121313,
		0x910, 0x806C0001,
		0x914, 0x00000001,
		0x918, 0x00000000,
		0x91C, 0x00010000,
		0x924, 0x00000001,
		0x928, 0x00000000,
		0x92C, 0x00000000,
		0x930, 0x00000000,
		0x934, 0x00000000,
		0x938, 0x00000000,
		0x93C, 0x00000000,
		0x940, 0x00000000,
		0x944, 0x00000000,
		0x94C, 0x00000008,
		0xA00, 0x00D0C7C8,
		0xA04, 0x81FF800C,
		0xA08, 0x8C838300,
		0xA0C, 0x2E68120F,
		0xA10, 0x95009B78,
		0xA14, 0x1114D028,
		0xA18, 0x00881117,
		0xA1C, 0x89140F00,
	0x80010003,0x00000000,0x40000000,0x00000000,
		0xA20, 0x12130000,
		0xA24, 0x060A0D10,
		0xA28, 0x00000103,
	0x90010002,0x00000000,0x40000000,0x00000000,
		0xA20, 0x12130000,
		0xA24, 0x060A0D10,
		0xA28, 0x00000103,
	0xA0000000,0x00000000,
		0xA20, 0x1A1B0000,
		0xA24, 0x090E1317,
		0xA28, 0x00000204,
	0xB0000000,0x00000000,
		0xA2C, 0x00D30000,
		0xA70, 0x101FFF00,
		0xA74, 0x00000007,
		0xA78, 0x00000900,
		0xA7C, 0x225B0606,
		0xA80, 0x218075B1,
		0xB38, 0x00000000,
		0xC00, 0x48071D40,
		0xC04, 0x03A05633,
		0xC08, 0x000000E4,
		0xC0C, 0x6C6C6C6C,
		0xC10, 0x08800000,
		0xC14, 0x40000100,
		0xC18, 0x08800000,
		0xC1C, 0x40000100,
		0xC20, 0x00000000,
		0xC24, 0x00000000,
		0xC28, 0x00000000,
		0xC2C, 0x00000000,
		0xC30, 0x69E9AC47,
		0xC34, 0x469652AF,
		0xC38, 0x49795994,
		0xC3C, 0x0A97971C,
		0xC40, 0x1F7C403F,
		0xC44, 0x000100B7,
		0xC48, 0xEC020107,
		0xC4C, 0x007F037F,
	0x80010003,0x00000000,0x40000000,0x00000000,
		0xC50, 0x00340220,
	0x90010002,0x00000000,0x40000000,0x00000000,
		0xC50, 0x00340220,
	0x90010001,0x00000000,0x40000000,0x00000000,
		0xC50, 0x00340220,
	0xA0000000,0x00000000,
		0xC50, 0x00340020,
	0xB0000000,0x00000000,
		0xC54, 0x0080801F,
	0x80010003,0x00000000,0x40000000,0x00000000,
		0xC58, 0x00000220,
	0x90010002,0x00000000,0x40000000,0x00000000,
		0xC58, 0x00000220,
	0x90010001,0x00000000,0x40000000,0x00000000,
		0xC58, 0x00000220,
	0xA0000000,0x00000000,
		0xC58, 0x00000020,
	0xB0000000,0x00000000,
		0xC5C, 0x00248492,
		0xC60, 0x00000000,
		0xC64, 0x7112848B,
		0xC68, 0x47C00BFF,
		0xC6C, 0x00000036,
		0xC70, 0x00000600,
		0xC74, 0x02013169,
		0xC78, 0x0000001F,
		0xC7C, 0x00B91612,
	0x80010003,0x00000000,0x40000000,0x00000000,
		0xC80, 0x2D4000B5,
	0x90010002,0x00000000,0x40000000,0x00000000,
		0xC80, 0x2D4000B5,
	0xA0000000,0x00000000,
		0xC80, 0x40000100,
	0xB0000000,0x00000000,
		0xC84, 0x21F60000,
	0x80010003,0x00000000,0x40000000,0x00000000,
		0xC88, 0x2D4000B5,
	0x90010002,0x00000000,0x40000000,0x00000000,
		0xC88, 0x2D4000B5,
	0xA0000000,0x00000000,
		0xC88, 0x40000100,
	0xB0000000,0x00000000,
		0xC8C, 0xA0E40000,
		0xC90, 0x00121820,
		0xC94, 0x00000000,
		0xC98, 0x00121820,
		0xC9C, 0x00007F7F,
		0xCA0, 0x00000000,
		0xCA4, 0x000300A0,
		0xCA8, 0x00000000,
		0xCAC, 0x00000000,
		0xCB0, 0x00000000,
		0xCB4, 0x00000000,
		0xCB8, 0x00000000,
		0xCBC, 0x28000000,
		0xCC0, 0x00000000,
		0xCC4, 0x00000000,
		0xCC8, 0x00000000,
		0xCCC, 0x00000000,
		0xCD0, 0x00000000,
		0xCD4, 0x00000000,
		0xCD8, 0x64B22427,
		0xCDC, 0x00766932,
		0xCE0, 0x00222222,
		0xCE4, 0x00040000,
		0xCE8, 0x77644302,
		0xCEC, 0x2F97D40C,
		0xD00, 0x00080740,
		0xD04, 0x00020403,
		0xD08, 0x0000907F,
		0xD0C, 0x20010201,
		0xD10, 0xA0633333,
		0xD14, 0x3333BC43,
		0xD18, 0x7A8F5B6B,
		0xD1C, 0x0000007F,
		0xD2C, 0xCC979975,
		0xD30, 0x00000000,
		0xD34, 0x80608000,
		0xD38, 0x00000000,
		0xD3C, 0x00127353,
		0xD40, 0x00000000,
		0xD44, 0x00000000,
		0xD48, 0x00000000,
		0xD4C, 0x00000000,
		0xD50, 0x6437140A,
		0xD54, 0x00000000,
		0xD58, 0x00000282,
		0xD5C, 0x30032064,
		0xD60, 0x4653DE68,
		0xD64, 0x04518A3C,
		0xD68, 0x00002101,
		0xD6C, 0x2A201C16,
		0xD70, 0x1812362E,
		0xD74, 0x322C2220,
		0xD78, 0x000E3C24,
		0xD80, 0x01081008,
		0xD84, 0x00000800,
		0xD88, 0xF0B50000,
		0xE00, 0x30303030,
		0xE04, 0x30303030,
		0xE08, 0x03903030,
		0xE10, 0x30303030,
		0xE14, 0x30303030,
		0xE18, 0x30303030,
		0xE1C, 0x30303030,
		0xE28, 0x00000000,
		0xE30, 0x1000DC1F,
		0xE34, 0x10008C1F,
		0xE38, 0x02140102,
		0xE3C, 0x681604C2,
		0xE40, 0x01007C00,
		0xE44, 0x01004800,
		0xE48, 0xFB000000,
		0xE4C, 0x000028D1,
		0xE50, 0x1000DC1F,
		0xE54, 0x10008C1F,
		0xE58, 0x02140102,
		0xE5C, 0x28160D05,
		0xE60, 0x00000008,
		0xE68, 0x0FC05656,
		0xE6C, 0x03C09696,
		0xE70, 0x03C09696,
		0xE74, 0x0C005656,
		0xE78, 0x0C005656,
		0xE7C, 0x0C005656,
		0xE80, 0x0C005656,
		0xE84, 0x03C09696,
		0xE88, 0x0C005656,
		0xE8C, 0x03C09696,
		0xED0, 0x03C09696,
		0xED4, 0x03C09696,
		0xED8, 0x03C09696,
		0xEDC, 0x0000D6D6,
		0xEE0, 0x0000D6D6,
		0xEEC, 0x0FC01616,
		0xEE4, 0xB0000C1C,
		0xEE8, 0x00000001,
		0xF14, 0x00000003,
		0xF4C, 0x00000000,
		0xF00, 0x00000300,

};

void
ODM_ReadAndConfig_MP_8723D_PHY_REG(
 	IN   PDM_ODM_T  pDM_Odm
 	)
{
    u4Byte     i         = 0;
    u4Byte     ArrayLen    = sizeof(Array_MP_8723D_PHY_REG)/sizeof(u4Byte);
    pu4Byte    Array       = Array_MP_8723D_PHY_REG;
	
    ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("===> ODM_ReadAndConfig_MP_8723D_PHY_REG\n"));

    for (i = 0; i < ArrayLen; i += 2 )
    {
        u4Byte v1 = Array[i];
        u4Byte v2 = Array[i+1];
    
        // This (offset, data) pair doesn't care the condition.
        if ( v1 < 0x40000000 )
        {
           odm_ConfigBB_PHY_8723D(pDM_Odm, v1, bMaskDWord, v2);
           continue;
        }
        else
        {   // This line is the beginning of branch.
            BOOLEAN bMatched = TRUE;
            u1Byte  cCond  = (u1Byte)((v1 & (BIT29|BIT28)) >> 28);

            if (cCond == COND_ELSE) { // ELSE, ENDIF
                bMatched = TRUE;
                READ_NEXT_PAIR(v1, v2, i);
            } else if ( ! CheckPositive(pDM_Odm, v1, v2) ) { 
                bMatched = FALSE;
                READ_NEXT_PAIR(v1, v2, i);
                READ_NEXT_PAIR(v1, v2, i);
            } else {
                READ_NEXT_PAIR(v1, v2, i);
                if ( ! CheckNegative(pDM_Odm, v1, v2) )
                    bMatched = FALSE;
                else
                    bMatched = TRUE;
                READ_NEXT_PAIR(v1, v2, i);
            }

            if ( bMatched == FALSE )
            {   // Condition isn't matched. Discard the following (offset, data) pairs.
                while (v1 < 0x40000000 && i < ArrayLen -2)
                    READ_NEXT_PAIR(v1, v2, i);

                i -= 2; // prevent from for-loop += 2
            }
            else // Configure matched pairs and skip to end of if-else.
            {
                while (v1 < 0x40000000 && i < ArrayLen-2) {
                    odm_ConfigBB_PHY_8723D(pDM_Odm, v1, bMaskDWord, v2);
                    READ_NEXT_PAIR(v1, v2, i);
                }

                // Keeps reading until ENDIF.
                cCond = (u1Byte)((v1 & (BIT29|BIT28)) >> 28);
                while (cCond != COND_ENDIF && i < ArrayLen-2) {
                    READ_NEXT_PAIR(v1, v2, i);
                    cCond = (u1Byte)((v1 & (BIT29|BIT28)) >> 28);
                }
            }
        } 
    }
}

u4Byte
ODM_GetVersion_MP_8723D_PHY_REG(void)
{
	   return 34;
}

/******************************************************************************
*                           PHY_REG_PG.TXT
******************************************************************************/

u4Byte Array_MP_8723D_PHY_REG_PG[] = { 
	0, 0, 0, 0x00000e08, 0x0000ff00, 0x00003200,
	0, 0, 1, 0x00000e08, 0x0000ff00, 0x00003200,
	0, 0, 0, 0x0000086c, 0xffffff00, 0x32323200,
	0, 0, 1, 0x0000086c, 0xffffff00, 0x32323200,
	0, 0, 0, 0x00000e00, 0xffffffff, 0x36364040,
	0, 0, 1, 0x00000e00, 0xffffffff, 0x34343636,
	0, 0, 0, 0x00000e04, 0xffffffff, 0x28283234,
	0, 0, 1, 0x00000e04, 0xffffffff, 0x28283032,
	0, 0, 0, 0x00000e10, 0xffffffff, 0x38383840,
	0, 0, 1, 0x00000e10, 0xffffffff, 0x34363840,
	0, 0, 0, 0x00000e14, 0xffffffff, 0x26283038,
	0, 0, 1, 0x00000e14, 0xffffffff, 0x26283032,
	0, 0, 1, 0x00000e18, 0xffffffff, 0x36384040,
	0, 0, 1, 0x00000e1c, 0xffffffff, 0x24262832,
	0, 1, 0, 0x00000838, 0xffffff00, 0x32323200,
	0, 1, 1, 0x00000838, 0xffffff00, 0x32323200,
	0, 1, 0, 0x0000086c, 0x000000ff, 0x00000032,
	0, 1, 1, 0x0000086c, 0x000000ff, 0x00000032,
	0, 1, 0, 0x00000830, 0xffffffff, 0x36364040,
	0, 1, 1, 0x00000830, 0xffffffff, 0x34343636,
	0, 1, 0, 0x00000834, 0xffffffff, 0x28283234,
	0, 1, 1, 0x00000834, 0xffffffff, 0x28283032,
	0, 1, 0, 0x0000083c, 0xffffffff, 0x38383840,
	0, 1, 1, 0x0000083c, 0xffffffff, 0x34363840,
	0, 1, 0, 0x00000848, 0xffffffff, 0x26283038,
	0, 1, 1, 0x00000848, 0xffffffff, 0x26283032,
	0, 1, 1, 0x0000084c, 0xffffffff, 0x36384040,
	0, 1, 1, 0x00000868, 0xffffffff, 0x24262832
};

void
ODM_ReadAndConfig_MP_8723D_PHY_REG_PG(
 	IN   PDM_ODM_T  pDM_Odm
 	)
{
	u4Byte     hex = 0;
	u4Byte     i           = 0;
	u2Byte     count       = 0;
	pu4Byte    ptr_array   = NULL;
	u1Byte     platform    = pDM_Odm->SupportPlatform;
	u1Byte     _interface   = pDM_Odm->SupportInterface;
	u1Byte     board       = pDM_Odm->BoardType;  
	u4Byte     ArrayLen    = sizeof(Array_MP_8723D_PHY_REG_PG)/sizeof(u4Byte);
	pu4Byte    Array       = Array_MP_8723D_PHY_REG_PG;

	pDM_Odm->PhyRegPgVersion = 1;
	pDM_Odm->PhyRegPgValueType = PHY_REG_PG_EXACT_VALUE;
	hex += board;
	hex += _interface << 8;
	hex += platform << 16;
	hex += 0xFF000000;
	for (i = 0; i < ArrayLen; i += 6 )
	{
	    u4Byte v1 = Array[i];
	    u4Byte v2 = Array[i+1];
	    u4Byte v3 = Array[i+2];
	    u4Byte v4 = Array[i+3];
	    u4Byte v5 = Array[i+4];
	    u4Byte v6 = Array[i+5];

	    // this line is a line of pure_body
		 odm_ConfigBB_PHY_REG_PG_8723D(pDM_Odm, v1, v2, v3, v4, v5, v6);
	}
}



#endif // end of HWIMG_SUPPORT
