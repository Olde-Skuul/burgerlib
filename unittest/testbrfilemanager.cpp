/***************************************

	Unit tests for the File Manager library

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrfilemanager.h"
#include "common.h"

#include "brfilename.h"
#include "brfilemanager.h"
#include "brfile.h"
#include "brdirectorysearch.h"
#include "brmemoryansi.h"

#define FULLTESTS

#if defined(BURGER_WINDOWS)
#include <windows.h>
#undef CopyFile
#undef DeleteFile
#endif

#define TM "\xE2\x84\xA2"
#define COPYRIGHT "\xC2\xA9"
#if defined(BURGER_MSDOS)
#define SAILORMOON "Bishojo Senshi Sailor Moon"
#define SAILORMOONSHORT "MOON"
#else
#define SAILORMOON "\xE7\xBE\x8E\xE5\xB0\x91\xE5\xA5\xB3\xE6\x88" \
	"\xA6\xE5\xA3\xAB\xE3\x82\xBB\xE3\x83\xBC\xE3\x83\xA9\xE3\x83\xBC" \
	"\xE3\x83\xA0\xE3\x83\xBC\xE3\x83\xB3"
#endif
using namespace Burger;

//
// This string it longer than 512 bytes. This is IMPORTANT for certain
// unit tests.
//
 
static const char g_LongFileName[] = 
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename"
;

static const char g_SampleData[] = "\nThis is a test, this only a test.";
static const char g_ShortFileName[] = "Foobar";

/***************************************

	Primary test!

	Verify that the class is exactly 512 bytes in size
	This is so that filenames on local storage don't take up 
	too much room on the stack so the stack is filled

	This test MUST pass. Failure of this test will mean
	that assumptions may be invalid as to stack memory usage.

***************************************/

static Word TestFilenameClass(void)
{
	// The structure size test
	Word uFailure = (sizeof(Filename)!=512);
	ReportFailure("sizeof(Filename) = %d / Wanted (512)",uFailure,sizeof(Filename));

	// Test for default string initialization
	Filename TestName;
	const char *pTest = TestName.GetPtr();
	Word uTest = pTest==NULL;
	uFailure |= uTest;
	ReportFailure("Filename.GetPtr() = %p, expected non NULL)",uTest,pTest);

	// Test for default empty string
	// If NULL, the previous test would have reported the failure
	if (pTest) {
		uTest = StringCompare("",pTest)!=0;
		uFailure |= uTest;
		ReportFailure("Filename = \"%s\", expected \"\"",uTest,pTest);
	}
	return uFailure;
}

/***************************************

	Test Filename.Set()

***************************************/

static Word TestFilenameSet(void)
{
	Word uFailure = FALSE;
	// This test will start with a large string and then 
	// get smaller.

	{
		Filename TestName;
		Word i = static_cast<Word>(sizeof(g_LongFileName));
		const char *pWork = g_LongFileName;
		do {
			// Set the string
			TestName.Set(pWork);
			const char *pTest = TestName.GetPtr();
			Word uTest = StringCompare(pWork,pTest)!=0;
			uFailure |= uTest;
			ReportFailure("Filename.Set(%s) = \"%s\", expected \"\"",uTest,pTest,pWork);
			++pWork;
		} while (--i);
	}

	{
		Word i = static_cast<Word>(sizeof(g_LongFileName));
		const char *pWork = g_LongFileName;
		do {
			// Set the string
			Filename TestName(pWork);
			const char *pTest = TestName.GetPtr();
			Word uTest = StringCompare(pWork,pTest)!=0;
			uFailure |= uTest;
			ReportFailure("Filename(%s) = \"%s\", expected \"\"",uTest,pTest,pWork);
			++pWork;
		} while (--i);
	}

	// Do the reverse. Start small, get larger
	{
		Word i = static_cast<Word>(sizeof(g_LongFileName));
		const char *pWork = g_LongFileName+(sizeof(g_LongFileName)-1);
		Filename TestName;
		do {
			// Set the string
			TestName.Set(pWork);
			const char *pTest = TestName.GetPtr();
			Word uTest = StringCompare(pWork,pTest)!=0;
			uFailure |= uTest;
			ReportFailure("Filename.Set(%s) = \"%s\", expected \"\"",uTest,pTest,pWork);
			--pWork;
		} while (--i);
	}

	{
		Word i = static_cast<Word>(sizeof(g_LongFileName));
		const char *pWork = g_LongFileName+(sizeof(g_LongFileName)-1);
		do {
			// Set the string
			Filename TestName(pWork);
			const char *pTest = TestName.GetPtr();
			Word uTest = StringCompare(pWork,pTest)!=0;
			uFailure |= uTest;
			ReportFailure("Filename(%s) = \"%s\", expected \"\"",uTest,pTest,pWork);
			--pWork;
		} while (--i);
	}

	return uFailure;
}

/***************************************

	Test Filename.Clear()

***************************************/

static Word TestFilenameClear(void)
{
	Word uFailure = FALSE;

	// Test if clearing resets the buffer pointer

	Filename TestName;
	// Get the default pointer (Known to be the original buffer)
	const char *pReference = TestName.GetPtr();
	TestName.Clear();
	const char *pTest = TestName.GetPtr();
	Word uTest = pReference!=pTest;
	uFailure |= uTest;
	ReportFailure("Filename::Clear() = %p, expected %p",uTest,pTest,pReference);

	// Set a short string, then blank it
	TestName.Set(g_ShortFileName);
	TestName.Clear();
	pTest = TestName.GetPtr();
	uTest = pTest[0]!=0;
	uFailure |= uTest;
	ReportFailure("Filename::Clear() = \"%s\", expected \"\"",uTest,pTest);

	// Set the string, then blank it
	TestName.Set(g_LongFileName);
	TestName.Clear();
	pTest = TestName.GetPtr();
	uTest = pTest[0]!=0;
	uFailure |= uTest;
	ReportFailure("Filename::Clear() = \"%s\", expected \"\"",uTest,pTest);

	// Verify the buffer is the default one after a memory allocation
	// since the previous test created a giant string
	uTest = pReference!=pTest;
	uFailure |= uTest;
	ReportFailure("Filename::Clear() = %p, expected %p",uTest,pTest,pReference);

	return uFailure;
}

/***************************************

	Test Filename.IsFullPathname()

***************************************/

struct IsTests_t {
	const char *m_pTest;
	Word m_uFull;
	Word m_uOnly;
	Word m_uPrefix;
};

static const IsTests_t IsFullTests[] = {
	{".d3:foo",TRUE,FALSE,FileManager::PREFIXINVALID},
	{".d31:foo",TRUE,FALSE,FileManager::PREFIXINVALID},
	{".d:foo",FALSE,TRUE,FileManager::PREFIXINVALID},
	{":foo:bar",TRUE,FALSE,FileManager::PREFIXINVALID},
	{".:folder",FALSE,TRUE,FileManager::PREFIXINVALID},
	{"temp.txt",FALSE,TRUE,FileManager::PREFIXINVALID},
	{"temp",FALSE,TRUE,FileManager::PREFIXINVALID},
	{"8:",FALSE,FALSE,8},
	{"12:this:is:a:path",FALSE,FALSE,12},
	{"20:twenty.txt",FALSE,FALSE,20},
	{"8:foo",FALSE,FALSE,8},
	{"$:foo",FALSE,FALSE,FileManager::PREFIXSYSTEM},
	{"@:foo",FALSE,FALSE,FileManager::PREFIXPREFS},
	{"*:foo",FALSE,FALSE,FileManager::PREFIXBOOT}
};

static Word TestFilenameIs(void)
{
	Word uFailure = FALSE;
	Filename TestName;

	Word i = static_cast<Word>(BURGER_ARRAYSIZE(IsFullTests));
	const IsTests_t *pIsTests = IsFullTests;
	do {
		TestName.Set(pIsTests->m_pTest);
		Word uResult = TestName.IsFullPathname();
		Word uTest = uResult!=pIsTests->m_uFull;
		uFailure |= uTest;
		ReportFailure("Filename::IsFullPathname(\"%s\") = %d, expected %d",uTest,pIsTests->m_pTest,uResult,pIsTests->m_uFull);

		uResult = TestName.IsFilenameOnly();
		uTest = uResult!=pIsTests->m_uOnly;
		uFailure |= uTest;
		ReportFailure("Filename::IsFilenameOnly(\"%s\") = %d, expected %d",uTest,pIsTests->m_pTest,uResult,pIsTests->m_uOnly);

		uResult = TestName.ParsePrefixNumber();
		uTest = uResult!=pIsTests->m_uPrefix;
		uFailure |= uTest;
		ReportFailure("Filename::ParsePrefixNumber(\"%s\") = %d, expected %d",uTest,pIsTests->m_pTest,uResult,pIsTests->m_uPrefix);

		++pIsTests;
	} while (--i);

	return uFailure;
}

/***************************************

	Test FileManager.DefaultPrefixes()

***************************************/

struct ExpandTest_t {
	const char *m_pOriginal;			// Burgerlib pathname to test from
	const char *m_pExpand;				// Expected expanded pathname
	const char *m_pNativeWindows;		// Expected WINDOWS pathname (Only for Windows targets)
	const char *m_pWindowsToBurger;		// Expected Windows to Burgerlib (Only for Windows targets)
	const char *m_pNativeMSDOS;			// Expected MSDOS pathname (Only for MSDOS targets)
	const char *m_pMSDOSToBurger;		// Expected MSDOS to Burgerlib (Only for MSDOS targets)
	const char *m_pNativeMacOSX;		// Expected MacOSX pathname (Only for MacOSX targets)
	const char *m_pMacOSXToBurger;		// Expected MacOSX to Burgerlib (Only for MacOSX targets)
	const char *m_pNativeMacOS;			// Expected MacOS pathname (Only for MacOS targets)
	const char *m_pMacOSToBurger;		// Expected MacOS to Burgerlib (Only for MacOS targets)
	long m_NativeMacOSDirID;			// Expected MacOS directory ID
	short m_NativeMacOSVolumeReference;	// Expected MacOS volume reference number
};

static Word TestSingleFileName(const ExpandTest_t *pInput)
{
	Word uFailure = FALSE;

	Filename MyFilename(pInput->m_pOriginal);
	Word uTest = StringCompare(pInput->m_pOriginal,MyFilename.GetPtr())!=0;
	uFailure |= uTest;
	ReportFailure("Original is \"%s\"",uTest,MyFilename.GetPtr());

	MyFilename.Expand();
	uTest = StringCompare(pInput->m_pExpand,MyFilename.GetPtr())!=0;
	uFailure |= uTest;
	ReportFailure("Expand from \"%s\" to \"%s\" but got \"%s\"",uTest,pInput->m_pOriginal,pInput->m_pExpand,MyFilename.GetPtr());

#if defined(BURGER_WINDOWS)
	Filename Copy(MyFilename);
	const char *pNative = MyFilename.GetNative();
	uTest = StringCompare(pInput->m_pNativeWindows,pNative)!=0;
	uFailure |= uTest;
	ReportFailure("GetNative from \"%s\" to \"%s\" but got \"%s\"",uTest,pInput->m_pOriginal,pInput->m_pNativeWindows,pNative);

	uTest = StringCompare(Copy.GetPtr(),MyFilename.GetPtr())!=0;
	uFailure |= uTest;
	ReportFailure("Expand after GetNative() from \"%s\" to \"%s\" but got \"%s\"",uTest,pInput->m_pOriginal,Copy.GetPtr(),MyFilename.GetPtr());

	Copy.SetFromNative(pNative);
	uTest = StringCompare(pInput->m_pWindowsToBurger,Copy.GetPtr())!=0;
	uFailure |= uTest;
	ReportFailure("ToNative from \"%s\" to \"%s\" but got \"%s\"",uTest,pInput->m_pOriginal,pInput->m_pWindowsToBurger,Copy.GetPtr());
#endif

	return uFailure;
}

static const char g_ExpandTextPrefix[] = ":SailorCallisto:Two:Three:Four:Five";

static const ExpandTest_t ExpandTests[] = {
{
	".D:Help me",
	":SailorCallisto:Two:Three:Four:Five:.D:Help me:",
	"C:\\Two\\Three\\Four\\Five\\.D\\Help me",
	".D2:Two:Three:Four:Five:.D:Help me:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	".D2:Help me",
	".D2:Help me:",
	"C:\\Help me",
	".D2:Help me:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	".D2x:Help me",
	":SailorCallisto:Two:Three:Four:Five:.D2x:Help me:",
	"C:\\Two\\Three\\Four\\Five\\.D2x\\Help me",
	".D2:Two:Three:Four:Five:.D2x:Help me:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	".Help me start",
	":SailorCallisto:Two:Three:Four:Five:.Help me start:",
	"C:\\Two\\Three\\Four\\Five\\.Help me start",
	".D2:Two:Three:Four:Five:.Help me start:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	".:Help me pfx",
	":SailorCallisto:Two:Three:Four:Five:Help me pfx:",
	"C:\\Two\\Three\\Four\\Five\\Help me pfx",
	".D2:Two:Three:Four:Five:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	"..:Help me pfx",
	":SailorCallisto:Two:Three:Four:Help me pfx:",
	"C:\\Two\\Three\\Four\\Help me pfx",
	".D2:Two:Three:Four:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	"...:Help me pfx",
	":SailorCallisto:Two:Three:Help me pfx:",
	"C:\\Two\\Three\\Help me pfx",
	".D2:Two:Three:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	"....:Help me pfx",
	":SailorCallisto:Two:Help me pfx:",
	"C:\\Two\\Help me pfx",
	".D2:Two:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	".....:Help me pfx",
	":SailorCallisto:Help me pfx:",
	"C:\\Help me pfx",
	".D2:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	"......:Help me pfx",
	":SailorCallisto:Help me pfx:",
	"C:\\Help me pfx",
	".D2:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	".......:Help me pfx",
	":SailorCallisto:Help me pfx:",
	"C:\\Help me pfx",
	".D2:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	"8:Six:Help me pfx",
	":SailorCallisto:Two:Three:Four:Five:Six:Help me pfx:",
	"C:\\Two\\Three\\Four\\Five\\Six\\Help me pfx",
	".D2:Two:Three:Four:Five:Six:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	"*:Help me pfx",
	".D2:Help me pfx:",
	"C:\\Help me pfx",
	".D2:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	"@:Help me pfx",
	".D2:Users:burgerbecky:AppData:Roaming:Help me pfx:",
	"C:\\Users\\burgerbecky\\AppData\\Roaming\\Help me pfx",
	".D2:Users:burgerbecky:AppData:Roaming:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	"$:Help me pfx",
	".D2:Windows:system32:Help me pfx:",
	"C:\\Windows\\system32\\Help me pfx",
	".D2:Windows:system32:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	"8:Help me pfx",
	":SailorCallisto:Two:Three:Four:Five:Help me pfx:",
	"C:\\Two\\Three\\Four\\Five\\Help me pfx",
	".D2:Two:Three:Four:Five:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	"9:Help me pfx",
	".D3:projects:burger:burgertools:unittest:bin:Help me pfx:",
	"D:\\projects\\burger\\burgertools\\unittest\\bin\\Help me pfx",
	".D3:projects:burger:burgertools:unittest:bin:Help me pfx:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
},
{
	":TestVolumeName:Foo",
	":TestVolumeName:Foo:",
	"\\\\TestVolumeName\\Foo",
	":TestVolumeName:Foo:",
	"MSDOS",
	"MSDOSToBurger",
	"MacOSX",
	"MacOSXToBurger",
	"MacOS",
	"MacOSToBurger",
	0,
	0
}
};

static Word TestFilenameExpand(void)
{
	Word uFailure = 0;
	Filename Save;
	FileManager::GetPrefix(&Save,FileManager::PREFIXCURRENT);
	FileManager::SetPrefix(FileManager::PREFIXCURRENT,g_ExpandTextPrefix);
	const ExpandTest_t *pExpandTests = ExpandTests;
	Word i = static_cast<Word>(BURGER_ARRAYSIZE(ExpandTests));
	do {
		uFailure |= TestSingleFileName(pExpandTests);
		++pExpandTests;
	} while (--i);
	FileManager::SetPrefix(FileManager::PREFIXCURRENT,Save.GetPtr());
	return uFailure;
}

/***************************************

	Test FileManager.DirName()

***************************************/

struct TestDirName_t {
	const char *m_pOriginal;
	const char *m_pExpected;
};

static const TestDirName_t TestDirNames[] = {
	{":foo:bar",":foo:"},
	{".d3:foo:bar",".d3:foo:"},
	{":foo:",":foo:"},
	{".d3:",".d3:"},
	{":foo:bar:",":foo:"},
	{":one:two:three:four:five",":one:two:three:four:"},
	{":one:two:three:four:five:",":one:two:three:four:"},
	{"one","one"},
	{"one:two","one:"},
	{"one:two:","one:"}
};

static Word TestFilenameDirName(void)
{
	Word uFailure = 0;
	const TestDirName_t *pTestDirName = TestDirNames;
	Word i = static_cast<Word>(BURGER_ARRAYSIZE(TestDirNames));
	do {
		Filename Test(pTestDirName->m_pOriginal);
		Test.DirName();
		Word uTest = StringCompare(Test.GetPtr(),pTestDirName->m_pExpected)!=0;
		uFailure |= uTest;
		ReportFailure("DirName \"%s\" to \"%s\" but got \"%s\"",uTest,pTestDirName->m_pOriginal,pTestDirName->m_pExpected,Test.GetPtr());
		++pTestDirName;
	} while (--i);
	return uFailure;
}

/***************************************

	Test FileManager.Append()

***************************************/

struct TestAppend_t {
	const char *m_pOriginal;
	const char *m_pAppend;
	const char *m_pExpected;
};
static const TestAppend_t TestAppends[] = {
	{":foo:bar","foo:",":foo:bar:foo:"},
	{".d3:foo:bar",".d3:foo:",".d3:foo:bar:.d3:foo:"},
	{":foo:","foo",":foo:foo:"},
	{".d3:","bar:",".d3:bar:"},
	{":foo:bar:","foo:",":foo:bar:foo:"},
	{":one:two:three:four:five","one:two:three:four:",":one:two:three:four:five:one:two:three:four:"},
	{":one:two:three:four:five:","one:two:three:four:",":one:two:three:four:five:one:two:three:four:"},
	{"one","one","one:one:"},
	{"one:two","one:","one:two:one:"},
	{"one:two:","one:","one:two:one:"}
};

static Word TestFilenameAppend(void)
{
	Word uFailure = 0;
	const TestAppend_t *pTestAppend = TestAppends;
	Word i = static_cast<Word>(BURGER_ARRAYSIZE(TestAppends));
	do {
		Filename Test(pTestAppend->m_pOriginal);
		Test.Append(pTestAppend->m_pAppend);
		Word uTest = StringCompare(Test.GetPtr(),pTestAppend->m_pExpected)!=0;
		uFailure |= uTest;
		ReportFailure("Append \"%s\" to \"%s\" but got \"%s\" and expected \"%s\"",uTest,pTestAppend->m_pOriginal,pTestAppend->m_pAppend,Test.GetPtr(),pTestAppend->m_pExpected);
		++pTestAppend;
	} while (--i);
	return uFailure;
}

/***************************************

	Test File

***************************************/

static Word TestFile(Word uVerbose)
{
	Word uFailure = 0;
	TimeDate_t Time;
	char Output[128];
#if defined(BURGER_MSDOS)
	Filename TestName;
	if (FileManager::AreLongFilenamesAllowed()) {
		TestName.Set("9:" SAILORMOON ".txt");
	} else {
		TestName.Set("9:" SAILORMOONSHORT "2.txt");
	}
#else
	Filename TestName("9:" SAILORMOON ".txt");
#endif
	File TestFile(&TestName,File::WRITEONLY);

	Word uTest = TestFile.Write("This is a test",sizeof("This is a test")-1)!=14;
	uFailure |= uTest;
	ReportFailure("File::Write",uTest);

	uTest = TestFile.GetModificationTime(&Time)!=File::OKAY;
	uFailure |= uTest;
	ReportFailure("File::GetModificationTime",uTest);
	if (uTest || uVerbose) {
		Time.DateToStringVerbose(Output);
		Message(Output);
		Time.TimeToStringPM(Output);
		Message(Output);
	}
	WordPtr uSize = TestFile.GetSize();
	uTest = uSize!=14;
	uFailure |= uTest;
	ReportFailure("File::GetSize return %d, expected 14",uTest,uSize);
	
	uTest = TestFile.Close()!=File::OKAY;
	uFailure |= uTest;
	ReportFailure("File::Close",uTest);
	return uFailure;
}

/***************************************

	Test FileManager.DefaultPrefixes()

***************************************/

typedef struct PrefixName_t {
	Word m_uPrefix;
	const char *m_pPrefixName;
} PrefixName_t;

static const PrefixName_t PrefixNameTable[] = {
	{FileManager::PREFIXCURRENT,"PREFIXCURRENT"},
	{FileManager::PREFIXAPPLICATION,"PREFIXAPPLICATION"},
	{FileManager::PREFIXBOOT,"PREFIXBOOT"},
	{FileManager::PREFIXPREFS,"PREFIXPREFS"},
	{FileManager::PREFIXSYSTEM,"PREFIXSYSTEM"}
};

static Word TestPrefixes(Word uVerbose)
{
	Filename MyFileName;

	// For testing, print out the current prefixes. Useful for debugging
	// on new platforms.

	if (uVerbose) {
		Word i = static_cast<Word>(BURGER_ARRAYSIZE(PrefixNameTable));
		const PrefixName_t *pWork = PrefixNameTable;
		do {
			FileManager::GetPrefix(&MyFileName,pWork->m_uPrefix);
			Message("FileManager::%s = \"%s\"",pWork->m_pPrefixName,MyFileName.GetPtr());
			++pWork;
		} while (--i);
	}

	// Check if all the other prefixes are empty on startup.

	Word i = 0;
	Word uFailure = FALSE;
	do {
		if ((i!=FileManager::PREFIXCURRENT) &&
			(i!=FileManager::PREFIXAPPLICATION)) {
			FileManager::GetPrefix(&MyFileName,i);
			const char *pTest = MyFileName.GetPtr();
			Word uTest = pTest[0]!=0;
			uFailure |= uTest;
			ReportFailure("FileManager::GetPrefix(%d) = \"%s\"",uTest,i,pTest);
		}
	} while (++i<FileManager::PREFIXBOOT);
	return uFailure;
}

/***************************************

	Test FileManager.GetVolumeName()

***************************************/

static Word TestGetVolumeName(Word uVerbose)
{
	if (uVerbose) {
		Filename MyFilename;
		Word i = 0;
		do {
			if (!FileManager::GetVolumeName(&MyFilename,i)) {
				Message("Drive %d is named \"%s\"",i,MyFilename.GetPtr());
			}
		} while (++i<6);
	}
	// No automated error check is possible
	// since this will generate different output depending
	// on the machine this is run on
	return 0;
}

/***************************************

	Test FileManager.DoesFileExist()

***************************************/

static Word TestDoesFileExist(void)
{
	Word uReturn = FileManager::DoesFileExist("9:FileNotHere");
	Word uTest = uReturn!=0;
	Word uFailure = uTest;
	ReportFailure("FileManager::DoesFileExist(\"9:FileNotHere\") = %d",uTest,uReturn);

	uReturn = FileManager::DoesFileExist("9:testfile.txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::DoesFileExist(\"9:testfile.txt\") = %d",uTest,uReturn);

	uReturn = FileManager::DoesFileExist("9:testfile" TM ".txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::DoesFileExist(\"9:testfile" TM ".txt\") = %d",uTest,uReturn);

	uReturn = FileManager::DoesFileExist("9:" SAILORMOON ".txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::DoesFileExist(\"9:" SAILORMOON ".txt\") = %d",uTest,uReturn);

	return uFailure;		
}

/***************************************

	Test the load and save commands

***************************************/

static Word TestLoadSave(void)
{
	// Save the file
	Word32 uReturn = FileManager::SaveFile("9:tempfile" COPYRIGHT ".txt",g_SampleData,sizeof(g_SampleData)-1);
	Word uTest = uReturn==0;
	Word uFailure = uTest;
	ReportFailure("FileManager::SaveFile(\"9:tempfile" COPYRIGHT ".txt\",g_SampleData,sizeof(g_SampleData)-1) = %d",uTest,uReturn);

	uReturn = FileManager::SaveFile("9:" SAILORMOON ".txt",g_SampleData,sizeof(g_SampleData)-1);
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::SaveFile(\"9:tempfile" SAILORMOON ".txt\",g_SampleData,sizeof(g_SampleData)-1) = %d",uTest,uReturn);

	// Determine if the file is present
	uReturn = FileManager::DoesFileExist("9:tempfile" COPYRIGHT ".txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::SaveFile(\"9:tempfile" COPYRIGHT ".txt\",g_SampleData,sizeof(g_SampleData)-1 didn't save!",uTest);

	// Load the file
	WordPtr uLength;
	void *pReturn = FileManager::LoadFile("9:tempfile" COPYRIGHT ".txt",&uLength);
	uTest = (uReturn==NULL) || (uLength!=(sizeof(g_SampleData)-1));
	uFailure |= uTest;
	ReportFailure("FileManager::LoadFile(\"9:tempfile" COPYRIGHT ".txt\") = %08X",uTest,static_cast<unsigned int>(uLength));
	if (pReturn) {
		uTest = MemoryCompare(pReturn,g_SampleData,sizeof(g_SampleData)-1)!=0;
		uFailure |= uTest;
		ReportFailure("FileManager::LoadFile(\"9:tempfile" COPYRIGHT ".txt\") return bad data",uTest);
		Free(pReturn);
	}

	// Copy the file
	uReturn = FileManager::CopyFile("9:tempfile" TM ".txt","9:tempfile" COPYRIGHT ".txt");
	uTest = uReturn!=0;
	uFailure = uTest;
	ReportFailure("FileManager::CopyFile(\"9:tempfile" TM ".txt\",\"9:tempfile" COPYRIGHT ".txt\") = %d",uTest,uReturn);

	// Determine if the file is present
	uReturn = FileManager::DoesFileExist("9:tempfile" TM ".txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::CopyFile(\"9:tempfile" TM ".txt\",\"9:tempfile" COPYRIGHT ".txt\") didn't copy!",uTest);

	// Rename the file
	uReturn = FileManager::RenameFile("9:tempfile" TM TM ".txt","9:tempfile" TM ".txt");
	uTest = uReturn!=0;
	uFailure = uTest;
	ReportFailure("FileManager::RenameFile(\"9:tempfile" TM TM ".txt\",\"9:tempfile" TM ".txt\") = %d",uTest,uReturn);

	// Determine if the file is present
	uReturn = FileManager::DoesFileExist("9:tempfile" TM TM ".txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::RenameFile(\"9:tempfile" TM TM ".txt\",\"9:tempfile" TM ".txt\") didn't rename!",uTest);

	// Check for fp opening
	FILE *fp = FileManager::OpenFile("9:tempfile" TM TM ".txt","rb");
	uTest = fp==NULL;
	uFailure |= uTest;
	ReportFailure("FileManager::OpenFile(\"9:tempfile" TM TM ".txt\",\"rb\")",uTest);
	if (fp) {
		fclose(fp);
	}
	
	// Delete the files
	uReturn = FileManager::DeleteFile("9:tempfile" COPYRIGHT ".txt");
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::DeleteFile(\"9:tempfile" COPYRIGHT ".txt\") = %d",uTest,uReturn);

	uReturn = FileManager::DeleteFile("9:tempfile" TM TM ".txt");
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::DeleteFile(\"9:tempfile" TM TM ".txt\") = %d",uTest,uReturn);

	// This SHOULD fail, because the file is gone. Test for failure case
	uReturn = FileManager::DeleteFile("9:tempfile" COPYRIGHT ".txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::DeleteFile(\"9:tempfile" COPYRIGHT ".txt\") (Should fail) = %d",uTest,uReturn);

	return uFailure;
}

/***************************************

	Create some temp files

***************************************/

static void TestCreateTempFiles(void)
{
	FileManager::SaveFile("9:testfile.txt",g_SampleData,sizeof(g_SampleData)-1);
	FileManager::SaveFile("9:testfile" TM ".txt",g_SampleData,sizeof(g_SampleData)-1);
	FileManager::SaveFile("9:" SAILORMOON ".txt",g_SampleData,sizeof(g_SampleData)-1);
#if defined(BURGER_MACOS)
	FileManager::SetFileAndAuxType("9:testfile.txt",'TEXT','CWIE');
	FileManager::SetFileAndAuxType("9:testfile" TM ".txt",'TEXT','CWIE');
	FileManager::SetFileAndAuxType("9:" SAILORMOON ".txt",'TEXT','CWIE');
#endif
}

/***************************************

	Dispose of the temp files

***************************************/

static void TestDisposeTempFiles(void)
{
	FileManager::DeleteFile("9:testfile.txt");
	FileManager::DeleteFile("9:testfile" TM ".txt");
	FileManager::DeleteFile("9:" SAILORMOON ".txt");
}

/***************************************

	Test FileManager.DoesFileExist()

***************************************/

static Word TestGetModificationTime(Word uVerbose)
{
	TimeDate_t MyTime;
	char TempBuffer[128];
	char TempBuffer2[128];
	Word32 uReturn = FileManager::GetModificationTime("9:FileNotHere",&MyTime);
	Word uTest = uReturn==0;
	Word uFailure = uTest;
	ReportFailure("FileManager::GetModificationTime(\"9:FileNotHere\") = %d",uTest,uReturn);

	uReturn = FileManager::GetModificationTime("9:testfile.txt",&MyTime);
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetModificationTime(\"9:testfile.txt\") = %d",uTest,uReturn);
	if (!uReturn && uVerbose) {
		MyTime.DateToStringVerbose(TempBuffer);
		MyTime.TimeToStringPM(TempBuffer2);
		Message("9:testfile.txt's modification was %s %s",TempBuffer,TempBuffer2);
	}

	uReturn = FileManager::GetModificationTime("9:testfile" TM ".txt",&MyTime);
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetModificationTime(\"9:testfile" TM ".txt\") = %d",uTest,uReturn);
	if (!uReturn && uVerbose) {
		MyTime.DateToStringVerbose(TempBuffer);
		MyTime.TimeToStringPM(TempBuffer2);
		Message("9:testfile" TM ".txt's modification was %s %s",TempBuffer,TempBuffer2);
	}
	return uFailure;		
}

/***************************************

	Test FileManager.GetCreationTime()

***************************************/

static Word TestGetCreationTime(Word uVerbose)
{
	TimeDate_t MyTime;
	char TempBuffer[128];
	char TempBuffer2[128];
	Word32 uReturn = FileManager::GetCreationTime("9:FileNotHere",&MyTime);
	Word uTest = uReturn==0;
	Word uFailure = uTest;
	ReportFailure("FileManager::GetCreationTime(\"9:FileNotHere\") = %d",uTest,uReturn);

	uReturn = FileManager::GetCreationTime("9:testfile.txt",&MyTime);
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetCreationTime(\"9:testfile.txt\") = %d",uTest,uReturn);
	if (!uReturn && uVerbose) {
		MyTime.DateToStringVerbose(TempBuffer);
		MyTime.TimeToStringPM(TempBuffer2);
		Message("9:testfile.txt's creation was %s %s",TempBuffer,TempBuffer2);
	}

	uReturn = FileManager::GetCreationTime("9:testfile" TM ".txt",&MyTime);
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetCreationTime(\"9:testfile" TM ".txt\") = %d",uTest,uReturn);
	if (!uReturn && uVerbose) {
		MyTime.DateToStringVerbose(TempBuffer);
		MyTime.TimeToStringPM(TempBuffer2);
		Message("9:testfile" TM ".txt's creation was %s %s",TempBuffer,TempBuffer2);
	}
	return uFailure;		
}

/***************************************
 
	Test FileManager.GetFileType()
 
***************************************/

static Word TestGetFileType(Word uVerbose)
{
#if defined(BURGER_MACOS)
	Word32 uReturn = FileManager::GetFileType("9:FileNotHere");
	Word uTest = uReturn!=0;
	Word uFailure = uTest;
	ReportFailure("FileManager::GetFileType(\"9:FileNotHere\") = 0x%08X",uTest,uReturn);
	
	uReturn = FileManager::GetFileType("9:testfile.txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetFileType(\"9:testfile.txt\") = 0x%08X",uTest,uReturn);
	if (uReturn && uVerbose) {
		Message("9:testfile.txt's file type was 0x%08X",uReturn);
	}
	
	uReturn = FileManager::GetFileType("9:testfile" TM ".txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetFileType(\"9:testfile" TM ".txt\") = 0x%08X",uTest,uReturn);
	if (uReturn && uVerbose) {
		Message("9:testfile" TM ".txt's file type was 0x%08X",uReturn);
	}
	return uFailure;
#else
	BURGER_UNUSED(uVerbose);
	return 0;
#endif
}

/***************************************
 
	Test FileManager.GetAuxType()
 
***************************************/

static Word TestGetAuxType(Word uVerbose)
{
#if defined(BURGER_MACOS)
	Word32 uReturn = FileManager::GetAuxType("9:FileNotHere");
	Word uTest = uReturn!=0;
	Word uFailure = uTest;
	ReportFailure("FileManager::GetAuxType(\"9:FileNotHere\") = 0x%08X",uTest,uReturn);
	
	uReturn = FileManager::GetAuxType("9:testfile.txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetAuxType(\"9:testfile.txt\") = 0x%08X",uTest,uReturn);
	if (uReturn && uVerbose) {
		Message("9:testfile.txt's aux type was 0x%08X",uReturn);
	}
	
	uReturn = FileManager::GetAuxType("9:testfile" TM ".txt");
	uTest = uReturn==0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetAuxType(\"9:testfile" TM ".txt\") = 0x%08X",uTest,uReturn);
	if (uReturn && uVerbose) {
		Message("9:testfile" TM ".txt's aux type was 0x%08X",uReturn);
	}
	return uFailure;		
#else
	BURGER_UNUSED(uVerbose);
	return 0;
#endif
}

/***************************************
 
	Test FileManager.GetFileAndAuxType()
 
***************************************/

static Word TestGetFileAndAuxType(Word uVerbose)
{
#if defined(BURGER_MACOS)
	Word32 uFileType;
	Word32 uAuxType;
	Word uReturn = FileManager::GetFileAndAuxType("9:FileNotHere",&uFileType,&uAuxType);
	Word uTest = uReturn==0;
	Word uFailure = uTest;
	ReportFailure("FileManager::GetFileAndAuxType(\"9:FileNotHere\") = %d File = 0x%08X, Aux = 0x%08X",uTest,uReturn,uFileType,uAuxType);
	
	uReturn = FileManager::GetFileAndAuxType("9:testfile.txt",&uFileType,&uAuxType);
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetFileAndAuxType(\"9:testfile.txt\") = %d File = 0x%08X, Aux = 0x%08X",uTest,uReturn,uFileType,uAuxType);
	if (!uReturn && uVerbose) {
		Message("9:testfile.txt's file was 0x%08X, aux was 0x%08X",uFileType,uAuxType);
	}
	
	uReturn = FileManager::GetFileAndAuxType("9:testfile" TM ".txt",&uFileType,&uAuxType);
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetFileAndAuxType(\"9:testfile" TM ".txt\") = %d File = 0x%08X, Aux = 0x%08X",uTest,uReturn,uFileType,uAuxType);
	if (!uReturn && uVerbose) {
		Message("9:testfile" TM ".txt's file was 0x%08X, aux was 0x%08X",uFileType,uAuxType);
	}
	return uFailure;		
#else
	BURGER_UNUSED(uVerbose);
	return 0;
#endif
}

/***************************************
 
 Test FileManager.SetFileType()
 
 ***************************************/

static Word TestSetFileType(Word uVerbose)
{
#if defined(BURGER_MACOS)
	Word32 uReturn = FileManager::SetFileType("9:testfile" TM ".txt",'ABCD');
	Word uTest = uReturn!=0;
	Word uFailure = uTest;
	ReportFailure("FileManager::SetFileType(\"9:testfile" TM ".txt\",'ABCD') = %d",uTest,uReturn);
	if (!uTest) {
		uReturn = FileManager::GetFileType("9:testfile" TM ".txt");
		uTest = uReturn!='ABCD';
		uFailure|=uTest;
		ReportFailure("FileManager::SetFileType(\"9:testfile" TM ".txt\",'ABCD') = got 0x%08X",uTest,uReturn);
	}
	
	uReturn = FileManager::SetFileType("9:testfile" TM ".txt",'TEXT');
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::SetFileType(\"9:testfile" TM ".txt\",'TEXT') = %d",uTest,uReturn);
	if (!uTest) {
		uReturn = FileManager::GetFileType("9:testfile" TM ".txt");
		uTest = uReturn!='TEXT';
		uFailure|=uTest;
		ReportFailure("FileManager::SetFileType(\"9:testfile" TM ".txt\",'TEXT') = got 0x%08X",uTest,uReturn);
	}
	return uFailure;
#else
	BURGER_UNUSED(uVerbose);
	return 0;
#endif
}

/***************************************
 
	Test FileManager.SetAuxType()
 
***************************************/

static Word TestSetAuxType(Word uVerbose)
{
#if defined(BURGER_MACOS)
	Word32 uReturn = FileManager::SetAuxType("9:testfile" TM ".txt",'ABCD');
	Word uTest = uReturn!=0;
	Word uFailure = uTest;
	ReportFailure("FileManager::SetAuxType(\"9:testfile" TM ".txt\",'ABCD') = %d",uTest,uReturn);
	if (!uTest) {
		uReturn = FileManager::GetAuxType("9:testfile" TM ".txt");
		uTest = uReturn!='ABCD';
		uFailure|=uTest;
		ReportFailure("FileManager::SetAuxType(\"9:testfile" TM ".txt\",'ABCD') = got 0x%08X",uTest,uReturn);
	}
	
	uReturn = FileManager::SetAuxType("9:testfile" TM ".txt",'CWIE');
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::SetAuxType(\"9:testfile" TM ".txt\",'CWIE') = %d",uTest,uReturn);
	if (!uTest) {
		uReturn = FileManager::GetAuxType("9:testfile" TM ".txt");
		uTest = uReturn!='CWIE';
		uFailure|=uTest;
		ReportFailure("FileManager::SetAuxType(\"9:testfile" TM ".txt\",'CWIE') = got 0x%08X",uTest,uReturn);
	}
	return uFailure;
#else
	BURGER_UNUSED(uVerbose);
	return 0;
#endif
}

/***************************************
 
	Test FileManager.SetFileAndAuxType()
 
***************************************/

static Word TestSetFileAndAuxType(Word uVerbose)
{
#if defined(BURGER_MACOS)
	Word32 uFileType;
	Word32 uAuxType;
	Word32 uReturn = FileManager::SetFileAndAuxType("9:testfile" TM ".txt",'ABCD','ABCD');
	Word uTest = uReturn!=0;
	Word uFailure = uTest;
	ReportFailure("FileManager::SetFileAndAuxType(\"9:testfile" TM ".txt\",'ABCD','ABCD') = %d",uTest,uReturn);
	if (!uTest) {
		uReturn = FileManager::GetFileAndAuxType("9:testfile" TM ".txt",&uFileType,&uAuxType);
		uTest = (uReturn!=0) || (uFileType!='ABCD') || (uAuxType!='ABCD');
		uFailure|=uTest;
		ReportFailure("FileManager::SetFileAndAuxType(\"9:testfile" TM ".txt\",'ABCD','ABCD') = got 0x%08X,0x%08X",uTest,uReturn);
	}
	
	uReturn = FileManager::SetFileAndAuxType("9:testfile" TM ".txt",'TEXT','CWIE');
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("FileManager::SetFileAndAuxType(\"9:testfile" TM ".txt\",'TEXT','CWIE') = %d",uTest,uReturn);
	if (!uTest) {
		uReturn = FileManager::GetFileAndAuxType("9:testfile" TM ".txt",&uFileType,&uAuxType);
		uTest = (uReturn!=0) || (uFileType!='TEXT') || (uAuxType!='CWIE');
		uFailure|=uTest;
		ReportFailure("FileManager::SetFileAndAuxType(\"9:testfile" TM ".txt\",'TEXT','CWIE') = got 0x%08X,0x%08X",uTest,uReturn);
	}
	return uFailure;
#else
	BURGER_UNUSED(uVerbose);
	return 0;
#endif
}

struct DirectoryTests_t {
	const char *m_pName;
	Word m_uDir;
};

static const DirectoryTests_t g_DirectoryFiles[] = {
	{"apple.txt",FALSE},
	{"berry.txt",FALSE},
	{"cat.txt",FALSE},
	{"delta",TRUE},
	{TM COPYRIGHT ".txt",FALSE}
};

static const char g_DirectoryToTest[] = "9:test";

static Word TestDirectorySearch(Word uVerbose)
{
	FileManager::CreateDirectoryPath(g_DirectoryToTest);
	FileManager::SetPrefix(20,g_DirectoryToTest);
	Word i = 0;
	const DirectoryTests_t *pDirTests = g_DirectoryFiles;
	do {
		Filename TestName("20:");
		TestName.Append(pDirTests->m_pName);
		if (pDirTests->m_uDir) {
			FileManager::CreateDirectoryPath(&TestName);
		} else {
			FileManager::SaveFile(&TestName,g_SampleData,sizeof(g_SampleData));
		}
		++pDirTests;
	} while (++i<BURGER_ARRAYSIZE(g_DirectoryFiles));

	// Test directory traversal
	DirectorySearch MyDir;
	Word uTest;
	Word uFailure = FALSE;
	Word uReturn;

#if defined(BURGER_WINDOWS)
	uTest = sizeof(WIN32_FIND_DATAW) != 592;
	uFailure |= uTest;
	ReportFailure("sizeof(MyDir.m_MyFindT) != %d",uTest,sizeof(WIN32_FIND_DATAW));
#endif

	uReturn = MyDir.Open(g_DirectoryToTest);
	uTest = uReturn!=0;
	uFailure |= uTest;
	ReportFailure("MyDir.Open(\"%s\") = %d",uTest,g_DirectoryToTest,uReturn);
		
	Word uEntries = 0;
	while (!MyDir.GetNextEntry()) {
		if (uEntries<BURGER_ARRAYSIZE(g_DirectoryFiles)) {
			uTest = StringCompare(MyDir.m_Name,g_DirectoryFiles[uEntries].m_pName)!=0;
			uFailure |= uTest;
			ReportFailure("MyDir.GetNextEntry() returned %s, was expecting %s",uTest,MyDir.m_Name,g_DirectoryFiles[uEntries],uReturn);
			uTest = (g_DirectoryFiles[uEntries].m_uDir!=MyDir.m_bDir);
			uFailure |= uTest;
			ReportFailure("MyDir.GetNextEntry() wrong dir flag for %s. expected %d, got %d",uTest,MyDir.m_Name,g_DirectoryFiles[uEntries].m_uDir,MyDir.m_bDir);
		}
		if (uVerbose) {
			Message("File %s found!",MyDir.m_Name);
			char TempBuffer[128];
			char TempBuffer2[128];
			MyDir.m_CreationDate.DateToStringVerbose(TempBuffer);
			MyDir.m_CreationDate.TimeToStringPM(TempBuffer2);
			Message("Creation was %s %s",TempBuffer,TempBuffer2);
			MyDir.m_ModificatonDate.DateToStringVerbose(TempBuffer);
			MyDir.m_ModificatonDate.TimeToStringPM(TempBuffer2);
			Message("Modification was %s %s",TempBuffer,TempBuffer2);
			Message("Size %u, Hidden %d, System %d, Directory %d, Locked %d",MyDir.m_uFileSize,MyDir.m_bHidden,MyDir.m_bSystem,MyDir.m_bDir,MyDir.m_bLocked);
#if defined(BURGER_MACOS)
			Message("File type 0x%08X, Aux Type 0x%08X",MyDir.m_uFileType,MyDir.m_uAuxType);
#endif
		}
		++uEntries;
	}
	MyDir.Close();

	uTest = uEntries==0;
	uFailure |= uTest;
	ReportFailure("MyDir.GetNextEntry(\"9\") didn't iterate",uTest,uReturn);
	// Disable to run manual tests of modification/creation time difference
#if 1
	pDirTests = g_DirectoryFiles;
	i = 0;
	do {
		Filename TestName("20:");
		TestName.Append(pDirTests->m_pName);
		FileManager::DeleteFile(&TestName);
		++pDirTests;
	} while (++i<BURGER_ARRAYSIZE(g_DirectoryFiles));
	FileManager::DeleteFile(g_DirectoryToTest);
#endif
	return uFailure;
}

/***************************************

	Test if setting the filename explicitly works.

***************************************/

Word FileManagerTest(Word uVerbose)
{	
	Word uTotal = 0;
	MemoryManagerGlobalANSI Memory;
	FileManager::Init();

#if defined(FULLTESTS)
	// Test Filename
	Message("Running Filename tests");
	uTotal |= TestFilenameClass(); 
	uTotal |= TestFilenameSet(); 
	uTotal |= TestFilenameClear();
	uTotal |= TestFilenameIs();
	uTotal |= TestFilenameExpand();
	uTotal |= TestFilenameDirName();
	uTotal |= TestFilenameAppend();
	
	// Test File
	Message("Running File tests");
	uTotal |= TestFile(uVerbose);
#endif

	// Test File Manager
	Message("Running File Manager tests");
	uTotal |= TestPrefixes(uVerbose);

#if defined(FULLTESTS)
	uTotal |= TestGetVolumeName(uVerbose);
	TestCreateTempFiles();
	uTotal |= TestDoesFileExist();

	// Test file manipulation
	uTotal |= TestLoadSave();

	// Test file information
	uTotal |= TestGetModificationTime(uVerbose);
	uTotal |= TestGetCreationTime(uVerbose);
	uTotal |= TestGetFileType(uVerbose);
	uTotal |= TestGetAuxType(uVerbose);
	uTotal |= TestGetFileAndAuxType(uVerbose);
	uTotal |= TestSetFileType(uVerbose);
	uTotal |= TestSetAuxType(uVerbose);
	uTotal |= TestSetFileAndAuxType(uVerbose);
	TestDisposeTempFiles();

	Message("Running Directory tests");
	uTotal |= TestDirectorySearch(uVerbose);
#endif
	FileManager::Shutdown();
	return uTotal;
}
