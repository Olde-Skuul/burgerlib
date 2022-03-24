/***************************************

	Localization manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRLOCALIZATION_H__
#define __BRLOCALIZATION_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

/* BEGIN */
namespace Burger {

class Localization {

	/** Enumerations for languages */
	enum eLanguage {

		/** Not declared */
		kNone,
		/** American English */
		kEnglish,
		/** French French */
		kFrench,
		/** Catalan Spanish (Spain) */
		kSpanish,
		/** German German */
		kGerman,
		/** Netherlands Dutch */
		kDutch,
		/** Russia Russian */
		kRussian,
		/** Japan Japanese */
		kJapanese,
		/** Traditional Chinese (People's Republic of China) */
		kChinese,
		/** South Korea Korean */
		kKorean,
		/** Poland Polish */
		kPolish,
		/** Italy Italian */
		kItalian,
		/** Sweden Swedish */
		kSwedish,
		/** Norway Norwegian */
		kNorwegian,
		/** Finland Finnish */
		kFinnish,

		/** United Kingdom English */
		kEnglishUK = kEnglish + 0x100,
		/** Australian English */
		kEnglishAustralia = kEnglish + 0x200,
		/** Canadian English */
		kEnglishCanada = kEnglish + 0x300,
		/** New Zealand English */
		kEnglishNewZealand = kEnglish + 0x400,
		/** Ireland English */
		kEnglishIreland = kEnglish + 0x500,
		/** South African English */
		kEnglishSouthAfrica = kEnglish + 0x600,
		/** Jamaican English */
		kEnglishJamaica = kEnglish + 0x700,
		/** Caribbean English */
		kEnglishCaribbean = kEnglish + 0x800,
		/** Belize English */
		kEnglishBelize = kEnglish + 0x900,
		/** Trinidad English */
		kEnglishTrinidad = kEnglish + 0xA00,
		/** Zimbabwe English */
		kEnglishZimbabwe = kEnglish + 0xB00,
		/** Philippine English */
		kEnglishPhilippines = kEnglish + 0xC00,

		/** Belgian French */
		kFrenchBelgium = kFrench + 0x100,
		/** Canadian French */
		kFrenchCanada = kFrench + 0x200,
		/** Swiss French */
		kFrenchSwiss = kFrench + 0x300,
		/** Luxembourg French */
		kFrenchLuxembourg = kFrench + 0x400,
		/** Monaco French */
		kFrenchMonaco = kFrench + 0x500,

		/** Mexican Spanish, Ole!!! */
		kSpanishMexican = kSpanish + 0x100,
		/** Guatemalan Spanish */
		kSpanishGuatemala = kSpanish + 0x200,
		/** Costa Rican Spanish */
		kSpanishCostaRica = kSpanish + 0x300,
		/** Panamanian Spanish */
		kSpanishPanama = kSpanish + 0x400,
		/** Dominican Republic Spanish */
		kSpanishDominicanRepublic = kSpanish + 0x500,
		/** Venezuelan Spanish */
		kSpanishVenezuela = kSpanish + 0x600,
		/** Columbian Spanish */
		kSpanishColumbia = kSpanish + 0x700,
		/** Peruvian Spanish */
		kSpanishPeru = kSpanish + 0x800,
		/** Argentinian Spanish */
		kSpanishArgentina = kSpanish + 0x900,
		/** Ecuadorian Spanish */
		kSpanishEcuador = kSpanish + 0xA00,
		/** Chilean Spanish */
		kSpanishChile = kSpanish + 0xB00,
		/** Uruguay Spanish */
		kSpanishUruguay = kSpanish + 0xC00,
		/** Paraguay Spanish */
		kSpanishParaguay = kSpanish + 0xD00,
		/** Bolivian Spanish */
		kSpanishBolivia = kSpanish + 0xE00,
		/** El Salvadorian Spanish */
		kSpanishElSalvador = kSpanish + 0xF00,
		/** Honduran Spanish */
		kSpanishHonduras = kSpanish + 0x1000,
		/** Nicaraguan Spanish */
		kSpanishNicaragua = kSpanish + 0x1100,
		/** Puerto Rican Spanish */
		kSpanishPuertoRico = kSpanish + 0x1200,

		/** Swiss German */
		kGermanSwiss = kGerman + 0x100,
		/** Austrian German */
		kGermanAustria = kGerman + 0x200,
		/** Luxembourg German */
		kGermanLuxembourg = kGerman + 0x300,
		/** Liechtenstein German */
		kGermanLiechtenstien = kGerman + 0x400,

		/** Belgian Dutch */
		kDutchBelgium = kDutch + 0x100,

		/** Taiwanese Chinese */
		kChineseTraditonal = kChinese + 0x100,
		/** Hong Kong Chinese */
		kChineseHongKong = kChinese + 0x200,
		/** Singapore Chinese */
		kChineseSingapore = kChinese + 0x300,
		/** Macau Chinese */
		kChineseMacau = kChinese + 0x400,

		/** Swiss Italian */
		kItalianSwiss = kItalian + 0x100,

		/** Swedish spoken in Finland */
		kSwedishFinland = kSwedish + 0x100,

		/** Bokmål Norway (Official written language of Norway) */
		kNorwegianBokmal = kNorwegian + 0x100,
		/** Nynorsk Norway (New Norwegian) */
		kNorwegianNynorsk = kNorwegian + 0x200
	};

protected:
	/** Language being used */
	eLanguage m_uLanguage;

public:
	Localization() BURGER_NOEXCEPT;
	BURGER_INLINE eLanguage GetLanguage(void) const BURGER_NOEXCEPT
	{
		return m_uLanguage;
	}
	BURGER_INLINE eLanguage GetBaseLanguage(void) const BURGER_NOEXCEPT
	{
		return eLanguage(static_cast<uint32_t>(m_uLanguage) & 0xFFU);
	}
	eError SetLanguage(eLanguage uLanguage) BURGER_NOEXCEPT
	{
		m_uLanguage = uLanguage;
		return kErrorNone;
	}
};

}
/* END */

#endif
