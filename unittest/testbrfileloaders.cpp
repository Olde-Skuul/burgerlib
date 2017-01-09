/***************************************

	Unit tests for the File Manager library

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrfileloaders.h"
#include "common.h"

#include "brfilename.h"
#include "brfilemanager.h"
#include "brfile.h"
#include "brdirectorysearch.h"
#include "brmemoryansi.h"
#include "brfilexml.h"
#include "brfileini.h"
#include "brfileapf.h"
#include "brfilegif.h"
#include "brfilebmp.h"
#include "brfilepng.h"
#include "brkeyboard.h"
#include "brfixedpoint.h"

//#define FULLTESTS
//#define TESTXML
//#define TESTINI
//#define TESTAPF
//#define TESTGIF
//#define TESTPNG
using namespace Burger;

#if defined(FULLTESTS)

//
// Unit test for the FileXML class
//

static Word TestFileXML(void)
{
	Word uResult = 0;
#if defined(TESTXML)
	InputMemoryStream Input;
	if (!Input.Open("9:..:..:data:test.xml")) {
		FileXML LoadFile;
		Word uResult1 = LoadFile.Init(&Input);
		if (!uResult1) {
			FileXML::Element *pElement = LoadFile.FindElement("Config",TRUE);
			if (pElement) {
				Word MySound = pElement->ElementGetWord("SoundVolume",255,0,255);
				Word MyMusic = pElement->ElementGetWord("MusicVolume",255,0,255);
				Word PlayerEvents0 = pElement->ElementGetWord("TriggerEvent",0x101,0,0xFFFF);
				Word PlayerEvents1 = pElement->ElementGetWord("SwitchAimEvent",Keyboard::SC_SPACE,0,0xFFFF);
				Word PlayerEvents2 = pElement->ElementGetWord("ReloadEvent",0x0102,0,0xFFFF);
				Word PlayerEvents3 = pElement->ElementGetWord("FireRateEvent",Keyboard::SC_F,0,0xFFFF);
				Word PlayerEvents4 = pElement->ElementGetWord("BreathEvent",Keyboard::SC_B,0,0xFFFF);
				Word DropoffMode = pElement->ElementGetWord("DropoffMode",FALSE,0,1);
				Word LowResolutionMode = pElement->ElementGetWord("LowResolutionMode",FALSE,0,1);
				Fixed32 MouseSpeed = FLOATTOFIXED(pElement->ElementGetFloat("MouseSpeed",1.0f,0.001f,999999.0f));
				Word BoolVal = pElement->ElementGetBoolean("Boolean",FALSE);
				pElement->ElementSetWord("FireRateEvent",666);
				pElement->ElementSetWord("NewRecord",1234);
				pElement->ElementSetBoolean("Boolean",TRUE);
			}
			pElement = LoadFile.FindElement("_2ndrecord");
			if (pElement) {
				const char *pQuote = pElement->ElementGetString("String2","Default");
				pElement->ElementSetString("NewString","This has \"a\" quote in it");
			}
			OutputMemoryStream Foo;
			LoadFile.Save(&Foo);
			Foo.SaveFile("9:..:..:bin:foo.xml");
		} else {
			ReportFailure("Error",TRUE);
			uResult = 1;
		}
	}
#endif
	return uResult;
}

//
// Unit test for the FileINI class
//

static Word TestFileINI(void)
{
	Word uResult = 0;
#if defined(TESTINI)
	InputMemoryStream Input;
	if (!Input.Open("9:..:..:data:test.ini")) {
		FileINI LoadFile;
		Word uResult1 = LoadFile.Init(&Input);
		if (!uResult1) {
			FileINI::Section *pSection = LoadFile.FindSection("Config",TRUE);
			if (pSection) {
				Word MySound = pSection->GetWord("SoundVolume",255,0,255);
				Word MyMusic = pSection->GetWord("MusicVolume",255,0,255);
				Word PlayerEvents0 = pSection->GetWord("TriggerEvent",0x101,0,0xFFFF);
				Word PlayerEvents1 = pSection->GetWord("SwitchAimEvent",Keyboard::SC_SPACE,0,0xFFFF);
				Word PlayerEvents2 = pSection->GetWord("ReloadEvent",0x0102,0,0xFFFF);
				Word PlayerEvents3 = pSection->GetWord("FireRateEvent",Keyboard::SC_F,0,0xFFFF);
				Word PlayerEvents4 = pSection->GetWord("BreathEvent",Keyboard::SC_B,0,0xFFFF);
				Word DropoffMode = pSection->GetWord("DropoffMode",FALSE,0,1);
				Word LowResolutionMode = pSection->GetWord("LowResolutionMode",FALSE,0,1);
				Fixed32 MouseSpeed = FLOATTOFIXED(pSection->GetFloat("MouseSpeed",1.0f,0.001f,999999.0f));
				Word BoolVal = pSection->GetBoolean("Boolean",FALSE);
				pSection->SetWord("FireRateEvent",666);
				pSection->SetWord("NewRecord",1234);
			}
			pSection = LoadFile.FindSection("2nd record");
			if (pSection) {
				String Quote;
				pSection->GetString(&Quote,"String2","Default");
				pSection->SetString("NewString","This has \"a\" quote in it");
			}
			OutputMemoryStream Foo;
			LoadFile.Save(&Foo);
			Foo.SaveFile("9:..:..:bin:foo.ini");
		} else {
			ReportFailure("Error",TRUE);
			uResult = 1;
		}
	}
#endif
	return uResult;
}

//
// Unit test for the FileAPF class
//

static Word TestFileAPF(void)
{
	Word uResult = 0;
#if defined(TESTAPF)
	InputMemoryStream Input;
	if (!Input.Open("9:..:..:data:titlepage.apf")) {
		FileAPF LoadFile;
		Image *pImage = LoadFile.Load(&Input);
		if (pImage) {
			OutputMemoryStream Output;
			FileBMP SaveFile;
			SaveFile.SetPalette(LoadFile.GetPalette());
			if (!SaveFile.Save(&Output,pImage)) {
				Output.SaveFile("9:..:..:bin:titlepageapf.bmp");
			}
		}
		Delete(pImage);
	}
#endif
	return uResult;
}

//
// Unit test for the FileGIF class
//

static Word TestFileGIF(void)
{
	Word uResult = 0;
#if defined(TESTGIF)
	InputMemoryStream Input;
	if (!Input.Open("9:..:..:data:test8bitcompressed.gif")) {
		FileGIF LoadFile;
		Image MyImage;
		if (!LoadFile.Load(&MyImage,&Input)) {
			OutputMemoryStream Output;
			FileGIF SaveFile;
			SaveFile.SetPalette(LoadFile.GetPalette());
			if (!SaveFile.Save(&Output,&MyImage)) {
				Output.SaveFile("9:..:..:bin:test8bitcompressed.gif");
			}
		}
	}
#endif
	return uResult;
}


//
// Unit test for the FilePNG class
//

static Word TestFilePNG(void)
{
	Word uResult = 0;
#if defined(TESTPNG)
	InputMemoryStream Input;
	if (!Input.Open("9:..:..:data:cursor1.png")) {
		FilePNG LoadFile;
		Image MyImage;
		if (!LoadFile.Load(&MyImage,&Input)) {
			OutputMemoryStream Output;
			FileBMP SaveFile;
			SaveFile.SetPalette(LoadFile.GetPalette());
			if (!SaveFile.Save(&Output,&MyImage)) {
				Output.SaveFile("9:..:..:bin:cursor1.bmp");
			}
		}
	}
#endif
	return uResult;
}

#endif



/***************************************

	Test if setting the filename explicitly works.

***************************************/

Word FileLoaderTest(Word /* uVerbose */)
{	
	Word uTotal = 0;
#if defined(FULLTESTS)

	MemoryManagerGlobalANSI Memory;
	FileManager::Init();

	// Test Filename
	Message("Running File loader tests");
	uTotal |= TestFileXML(); 
	uTotal |= TestFileINI();
	uTotal |= TestFileAPF();
	uTotal |= TestFileGIF();
	uTotal |= TestFilePNG();
	FileManager::Shutdown();
#endif
	return uTotal;
}
