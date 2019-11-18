/***************************************

	Localization manager
	
	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	// Enumerations for languages
	BURGER_ENUMSTART(eLanguage,Word32) {
		None,
		English,				///< American English
		French,					///< French French
		Spanish,				///< Catalan Spanish (Spain)
		German,					///< German German
		Dutch,					///< Netherlands Dutch
		Russian,				///< Russia Russian
		Japanese,				///< Japan Japanese
		Chinese,				///< Traditional Chinese (People's Republic of China)
		Korean,					///< South Korea Korean
		Polish,					///< Poland Polish
		Italian,				///< Italy Italian
		Swedish,				///< Sweden Swedish
		Norwegian,				///< Norway Norwegian
		Finnish,				///< Finland Finnish

		EnglishUK=English+0x100,			///< United Kingdom English
		EnglishAustralia=English+0x200,		///< Australian English
		EnglishCanada=English+0x300,		///< Canadian English
		EnglishNewZealand=English+0x400,	///< New Zealand English
		EnglishIreland=English+0x500,		///< Ireland English
		EnglishSouthAfrica=English+0x600,	///< South African English
		EnglishJamaica=English+0x700,		///< Jamaican English
		EnglishCaribbean=English+0x800,		///< Caribbean English
		EnglishBelize=English+0x900,		///< Belize English
		EnglishTrinidad=English+0xA00,		///< Trinidad English
		EnglishZimbabwe=English+0xB00,		///< Zimbabwe English
		EnglishPhilippines=English+0xC00,	///< Philippine English

		FrenchBelgium=French+0x100,			///< Belgian French
		FrenchCanada=French+0x200,			///< Canadian French
		FrenchSwiss=French+0x300,			///< Swiss French
		FrenchLuxembourg=French+0x400,		///< Luxembourg French
		FrenchMonaco=French+0x500,			///< Monaco French

		SpanishMexican=Spanish+0x100,		///< Mexican Spanish, Ole!!!
		SpanishGuatemala=Spanish+0x200,		///< Guatemalan Spanish
		SpanishCostaRica=Spanish+0x300,		///< Costa Rican Spanish
		SpanishPanama=Spanish+0x400,		///< Panamanian Spanish
		SpanishDominicanRepublic=Spanish+0x500,	///< Dominican Republic Spanish
		SpanishVenezuela=Spanish+0x600,		///< Venezuelan Spanish
		SpanishColumbia=Spanish+0x700,		///< Columbian Spanish
		SpanishPeru=Spanish+0x800,			///< Peruvian Spanish
		SpanishArgentina=Spanish+0x900,		///< Argentinian Spanish
		SpanishEcuador=Spanish+0xA00,		///< Ecuadorian Spanish
		SpanishChile=Spanish+0xB00,			///< Chilean Spanish
		SpanishUruguay=Spanish+0xC00,		///< Uruguay Spanish
		SpanishParaguay=Spanish+0xD00,		///< Paraguay Spanish
		SpanishBolivia=Spanish+0xE00,		///< Bolivian Spanish
		SpanishElSalvador=Spanish+0xF00,	///< El Salvadorian Spanish
		SpanishHonduras=Spanish+0x1000,		///< Honduran Spanish
		SpanishNicaragua=Spanish+0x1100,	///< Nicaraguan Spanish
		SpanishPuertoRico=Spanish+0x1200,	///< Puerto Rican Spanish

		GermanSwiss=German+0x100,			///< Swiss German
		GermanAustria=German+0x200,			///< Austrian German
		GermanLuxembourg=German+0x300,		///< Luxembourg German
		GermanLiechtenstien=German+0x400,	///< Liechtenstein German

		DutchBelgium=Dutch+0x100,			///< Belgian Dutch

		ChineseTraditonal=Chinese+0x100,	///< Taiwanese Chinese
		ChineseHongKong=Chinese+0x200,		///< Hong Kong Chinese
		ChineseSingapore=Chinese+0x300,		///< Singapore Chinese
		ChineseMacau=Chinese+0x400,			///< Macau Chinese

		ItalianSwiss=Italian+0x100,			///< Swiss Italian

		SwedishFinland=Swedish+0x100,		///< Swedish spoken in Finland

		NorwegianBokmal=Norwegian+0x100,	///< Bokmål Norway (Official written language of Norway)
		NorwegianNynorsk=Norwegian+0x200	///< Nynorsk Norway (New Norwegian)

	} BURGER_ENUMEND(eLanguage)
protected:
	eLanguageType m_uLanguage;

public:
	Localization();
	~Localization();
	BURGER_INLINE eLanguageType GetLanguage(void) const { return m_uLanguage; }
	BURGER_INLINE eLanguageType GetBaseLanguage(void) const { return static_cast<eLanguageType>(static_cast<Word32>(m_uLanguage)&0xFFU); }
	eError BURGER_API SetLanguage(eLanguageType uLanguage);
};

}
/* END */

#endif
