/***************************************

	Unit tests for the File Manager library

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrfilemanager.h"
#include "common.h"

#include "brdirectorysearch.h"
#include "brfile.h"
#include "brfilemanager.h"
#include "brfilename.h"
#include "brmemoryansi.h"
#include "brmemoryfunctions.h"

#if defined(BURGER_WINDOWS)
#include <windows.h>
#undef CopyFile
#undef DeleteFile
#endif

// Characters shared among classic character sets
#define FHOOK "\xC6\x92"
#define I_DIERESIS "\xC3\xAF"

#define SAILORMOONSHORT "MOON"
#define SAILORMOON \
	"\xE7\xBE\x8E\xE5\xB0\x91\xE5\xA5\xB3\xE6\x88" \
	"\xA6\xE5\xA3\xAB\xE3\x82\xBB\xE3\x83\xBC\xE3\x83\xA9\xE3\x83\xBC" \
	"\xE3\x83\xA0\xE3\x83\xBC\xE3\x83\xB3"

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
	"ThisIsAReallyLongFilenameThisIsAReallyLongFilenameThisIsAReallyLongFilename";

static const char g_SampleData[] = "\nThis is a test, this only a test.";
static const char g_ShortFileName[] = "Foobar";

/***************************************

	File name generator for various OS rules

***************************************/

static const char* BURGER_API GetWorkFilename(void) BURGER_NOEXCEPT
{

#if defined(BURGER_MSDOS)
	// MS Dos only supports Win437 encoding, so don't go out of bounds

	if (FileManager::MSDOS_HasLongFilenames()) {
		return "9:Bishojo Senshi Sailor Moon" FHOOK ".txt";
	}

	// Dos BOX doesn't support extended ASCII in filenames
	if (FileManager::MSDos_GetFlavor() == 0x77) {
		return "9:" SAILORMOONSHORT ".txt";
	}
	return "9:" SAILORMOONSHORT FHOOK ".txt";

#elif defined(BURGER_MAC)
	// MacOS used MacRoman US unless it's HFS+
	if (FileManager::IsUTF8FileSystem()) {
		return "9:" SAILORMOON ".txt";
	}
	return "9:" SAILORMOONSHORT " " FHOOK ".txt";

	// These consoles only support ISO-9660 CD ROM encoding
#elif defined(BURGER_VITA) || defined(BURGER_XBOX) || \
	defined(BURGER_XBOX360) || defined(BURGER_PS3)
	return "9:" SAILORMOONSHORT ".txt";
#else
	// Assume everyone else is full UTF8
	return "9:" SAILORMOON ".txt";
#endif
}

/***************************************

	Primary test!

	Verify that the class is exactly 512 bytes in size.
	This is so that filenames on local storage don't take up too much room on
	the stack so the stack is filled

	This test MUST pass. Failure of this test will mean that assumptions may be
	invalid as to stack memory usage.

***************************************/

static uint_t BURGER_API TestFilenameClass(void) BURGER_NOEXCEPT
{
	// Test for default string initialization
	Filename TestName;
	const char* pTest = TestName.c_str();
	uint_t uTest = !pTest;
	uint_t uFailure = uTest;
	ReportFailure("Filename.c_str() = %p, expected non nullptr)", uTest, pTest);

	// Test for default empty string
	// If nullptr, the previous test would have reported the failure
	if (pTest) {
		uTest = StringCompare("", pTest) != 0;
		uFailure |= uTest;
		ReportFailure("Filename = \"%s\", expected \"\"", uTest, pTest);
	}
	return uFailure;
}

/***************************************

	Test Filename.Set()

***************************************/

static uint_t BURGER_API TestFilenameSet(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	// This test will start with a large string and then get smaller.
	uintptr_t i;
	const char* pWork;

	{
		Filename TestName;
		i = sizeof(g_LongFileName);
		pWork = g_LongFileName;
		do {
			// Set the string
			TestName.assign(pWork);
			const char* pTest = TestName.c_str();
			const uint_t uTest = StringCompare(pWork, pTest) != 0;
			uFailure |= uTest;
			ReportFailure("Filename.Set(%s) = \"%s\", expected \"\"", uTest,
				pTest, pWork);
			++pWork;
		} while (--i);
	}

	i = sizeof(g_LongFileName);
	pWork = g_LongFileName;
	do {
		// Set the string
		Filename TestName2(pWork);
		const char* pTest = TestName2.c_str();
		const uint_t uTest = StringCompare(pWork, pTest) != 0;
		uFailure |= uTest;
		ReportFailure(
			"Filename(%s) = \"%s\", expected \"\"", uTest, pTest, pWork);
		++pWork;
	} while (--i);

	// Do the reverse. Start small, get larger
	{
		i = sizeof(g_LongFileName);
		pWork = g_LongFileName + (sizeof(g_LongFileName) - 1);
		Filename TestName3;
		do {
			// Set the string
			TestName3.assign(pWork);
			const char* pTest = TestName3.c_str();
			const uint_t uTest = StringCompare(pWork, pTest) != 0;
			uFailure |= uTest;
			ReportFailure("Filename.Set(%s) = \"%s\", expected \"\"", uTest,
				pTest, pWork);
			--pWork;
		} while (--i);
	}

	i = sizeof(g_LongFileName);
	pWork = g_LongFileName + (sizeof(g_LongFileName) - 1);
	do {
		// Set the string
		Filename TestName4(pWork);
		const char* pTest = TestName4.c_str();
		const uint_t uTest = StringCompare(pWork, pTest) != 0;
		uFailure |= uTest;
		ReportFailure(
			"Filename(%s) = \"%s\", expected \"\"", uTest, pTest, pWork);
		--pWork;
	} while (--i);

#if defined(BURGER_WINDOWS) || defined(BURGER_MSDOS)
	{
		Filename Test;
		Test.SetFromNative("C:\\");
		const uint_t uTest = StringCompare(Test.c_str(), ".D2:") != 0;
		uFailure |= uTest;
		ReportFailure("SetFromNative(\"C:\\\"), got \"%s\", expected \".D2:\" ",
			uTest, Test.c_str());
	}

#endif

	return uFailure;
}

/***************************************

	Test Filename.clear()

***************************************/

static uint_t BURGER_API TestFilenameClear(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	// Test if clearing resets the buffer pointer

	Filename TestName;
	// Get the default pointer (Known to be the original buffer)
	const char* pReference = TestName.c_str();
	TestName.clear();
	const char* pTest = TestName.c_str();
	uint_t uTest = pReference != pTest;
	uFailure |= uTest;
	ReportFailure(
		"Filename::clear() = %p, expected %p", uTest, pTest, pReference);

	// Set a short string, then blank it
	TestName.assign(g_ShortFileName);
	TestName.clear();
	pTest = TestName.c_str();
	uTest = pTest[0] != 0;
	uFailure |= uTest;
	ReportFailure("Filename::clear() = \"%s\", expected \"\"", uTest, pTest);

	// Set the string, then blank it
	TestName.assign(g_LongFileName);
	TestName.clear();
	pTest = TestName.c_str();
	uTest = pTest[0] != 0;
	uFailure |= uTest;
	ReportFailure("Filename::clear() = \"%s\", expected \"\"", uTest, pTest);

	// Verify the buffer is the default one after a memory allocation
	// since the previous test created a giant string
	uTest = pReference != pTest;
	uFailure |= uTest;
	ReportFailure(
		"Filename::clear() = %p, expected %p", uTest, pTest, pReference);

	return uFailure;
}

/***************************************

	Test Filename.IsFullPathname()

***************************************/

struct IsTests_t {
	const char* m_pTest;
	uint_t m_uFull;
	uint_t m_uOnly;
	uint_t m_uPrefix;
};

static const IsTests_t IsFullTests[] = {
	{".d3:foo", TRUE, FALSE, FileManager::kPrefixInvalid},
	{".d31:foo", TRUE, FALSE, FileManager::kPrefixInvalid},
	{".d:foo", FALSE, TRUE, FileManager::kPrefixInvalid},
	{":foo:bar", TRUE, FALSE, FileManager::kPrefixInvalid},
	{".:folder", FALSE, TRUE, FileManager::kPrefixInvalid},
	{"temp.txt", FALSE, TRUE, FileManager::kPrefixInvalid},
	{"temp", FALSE, TRUE, FileManager::kPrefixInvalid}, {"8:", FALSE, FALSE, 8},
	{"12:this:is:a:path", FALSE, FALSE, 12},
	{"20:twenty.txt", FALSE, FALSE, 20}, {"8:foo", FALSE, FALSE, 8},
	{"$:foo", FALSE, FALSE, FileManager::kPrefixSystem},
	{"@:foo", FALSE, FALSE, FileManager::kPrefixPrefs},
	{"*:foo", FALSE, FALSE, FileManager::kPrefixBoot}};

static uint_t BURGER_API TestFilenameIs(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	Filename TestName;

	uint_t i = static_cast<uint_t>(BURGER_ARRAYSIZE(IsFullTests));
	const IsTests_t* pIsTests = IsFullTests;
	do {
		TestName.assign(pIsTests->m_pTest);
		uint_t uResult = TestName.IsFullPathname();
		uint_t uTest = uResult != pIsTests->m_uFull;
		uFailure |= uTest;
		ReportFailure("Filename::IsFullPathname(\"%s\") = %d, expected %d",
			uTest, pIsTests->m_pTest, uResult, pIsTests->m_uFull);

		uResult = TestName.IsFilenameOnly();
		uTest = uResult != pIsTests->m_uOnly;
		uFailure |= uTest;
		ReportFailure("Filename::IsFilenameOnly(\"%s\") = %d, expected %d",
			uTest, pIsTests->m_pTest, uResult, pIsTests->m_uOnly);

		uResult = TestName.ParsePrefixNumber();
		uTest = uResult != pIsTests->m_uPrefix;
		uFailure |= uTest;
		ReportFailure("Filename::ParsePrefixNumber(\"%s\") = %d, expected %d",
			uTest, pIsTests->m_pTest, uResult, pIsTests->m_uPrefix);

		++pIsTests;
	} while (--i);

	return uFailure;
}

/***************************************

	Test Filename.join()

***************************************/

static uint_t BURGER_API TestFilenameJoin(void) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;
	Filename Test(":BaseFilename");
	Test.join("foo");

	uint_t uTest = StringCompare(Test.c_str(), ":BaseFilename:foo:") != 0;
	uFailure |= uTest;
	ReportFailure(
		"Filename::join(\"foo\") = expected :BaseFilename:foo:, got %s", uTest,
		Test.c_str());

	Test.join("a:b:c:");
	uTest = StringCompare(Test.c_str(), ":BaseFilename:foo:a:b:c:") != 0;
	uFailure |= uTest;
	ReportFailure(
		"Filename::join(\"a:b:c:\") = expected :BaseFilename:foo:a:b:c:, got %s",
		uTest, Test.c_str());

	Test = "foo.txt";
	Test.set_file_extension(".jpg");
	uTest = StringCompare(Test.c_str(), "foo.jpg:") != 0;
	uFailure |= uTest;
	ReportFailure(
		"Filename::set_file_extension(\".jpg\") = expected foo.jpg:, got %s",
		uTest, Test.c_str());

	Test.set_file_extension("jpg");
	uTest = StringCompare(Test.c_str(), "foo.jpg:") != 0;
	uFailure |= uTest;
	ReportFailure(
		"Filename::set_file_extension(\"jpg\") = expected foo.jpg:, got %s",
		uTest, Test.c_str());

	Test = ".foo";
	Test.set_file_extension(".bmp");
	uTest = StringCompare(Test.c_str(), ".foo.bmp:") != 0;
	uFailure |= uTest;
	ReportFailure(
		"Filename::set_file_extension(\".bmp\") = expected .foo.bmp:, got %s",
		uTest, Test.c_str());

	Test.set_file_extension("bmp");
	uTest = StringCompare(Test.c_str(), ".foo.bmp:") != 0;
	uFailure |= uTest;
	ReportFailure(
		"Filename::set_file_extension(\"bmp\") = expected .foo.bmp:, got %s",
		uTest, Test.c_str());

	Test = ":foo:bar:foo.txt";
	String Base;
	Test.get_basename(&Base);
	uTest = StringCompare(Base.c_str(), "foo.txt") != 0;
	uFailure |= uTest;
	ReportFailure("Filename::get_basename(&String) = expected foo.txt, got %s",
		uTest, Base.c_str());

	Test.get_dirname(&Base);
	Test.dirname();
	uTest = StringCompare(Test.c_str(), ":foo:bar:") != 0;
	uFailure |= uTest;
	ReportFailure(
		"Filename::dirname(\":foo:bar:foo.txt\") = expected :foo:bar:, got %s",
		uTest, Test.c_str());
	uTest = StringCompare(Base.c_str(), ":foo:bar:") != 0;
	uFailure |= uTest;
	ReportFailure("Filename::get_dirname(&Base) = expected :foo:bar:, got %s",
		uTest, Base.c_str());

	Test.get_dirname(&Base);
	Test.dirname();
	uTest = StringCompare(Test.c_str(), ":foo:") != 0;
	uFailure |= uTest;
	ReportFailure("Filename::dirname(\":foo:bar:\") = expected :foo:, got %s",
		uTest, Test.c_str());
	uTest = StringCompare(Base.c_str(), ":foo:") != 0;
	uFailure |= uTest;
	ReportFailure("Filename::get_dirname(&Base) = expected :foo:, got %s",
		uTest, Base.c_str());

	Test.get_dirname(&Base);
	Test.dirname();
	uTest = StringCompare(Test.c_str(), ":foo:") != 0;
	uFailure |= uTest;
	ReportFailure("Filename::dirname(\":foo:\") = expected :foo:, got %s",
		uTest, Test.c_str());
	uTest = StringCompare(Base.c_str(), ":foo:") != 0;
	uFailure |= uTest;
	ReportFailure("Filename::get_dirname(&Base) = expected :foo:, got %s",
		uTest, Base.c_str());
	return uFailure;
}

/***************************************

	Show directories

***************************************/

static void BURGER_API TestShowDirectories(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Filename Test;

		// Check if MS/DOS long file names are supported
#if defined(BURGER_MSDOS)
		Message("FileManager::MSDOS_HasLongFilenames() is %u",
			FileManager::MSDOS_HasLongFilenames());
		Message("FileManager::MSDOS_GetOSVersion() is 0x%04X",
			FileManager::MSDOS_GetOSVersion());
		Message("FileManager::MSDos_GetOSTrueVersion() is 0x%04X",
			FileManager::MSDos_GetOSTrueVersion());
		Message(
			"FileManager::MSDOS_GetName() is %s", FileManager::MSDOS_GetName());
		Message("FileManager::MSDos_GetFlavor() is 0x%04X",
			FileManager::MSDos_GetFlavor());
#endif

		// Display the directories
		Test.SetSystemWorkingDirectory();
		Message("Current working directory is %s", Test.c_str());
		Test.SetApplicationDirectory();
		Message("Application directory is %s", Test.c_str());
		Test.SetBootVolumeDirectory();
		Message("Boot volume directory is %s", Test.c_str());
		Test.SetMachinePrefsDirectory();
		Message("Machine prefs directory is %s", Test.c_str());
		Test.SetUserPrefsDirectory();
		Message("User prefs directory is %s", Test.c_str());
	}
}

/***************************************

	Test FileManager.GetVolumeName()

***************************************/

static uint_t BURGER_API TestGetVolumeName(uint_t uVerbose) BURGER_NOEXCEPT
{
	// No automated error check is possible since this will generate different
	// output depending on the machine this is run on

	if (uVerbose & VERBOSE_MSG) {
		Filename MyFilename;
		uint_t i = 0;
		do {
			if (!FileManager::GetVolumeName(&MyFilename, i)) {
				Message("Drive %u is named \"%s\"", i, MyFilename.c_str());
			}
		} while (++i < 10);
	}
	return 0;
}

/***************************************

	Test FileManager.DefaultPrefixes()

***************************************/

typedef struct PrefixName_t {
	uint_t m_uPrefix;
	const char* m_pPrefixName;
} PrefixName_t;

static const PrefixName_t PrefixNameTable[] = {
	{FileManager::kPrefixCurrent, "kPrefixCurrent"},
	{FileManager::kPrefixApplication, "kPrefixApplication"},
	{FileManager::kPrefixBoot, "kPrefixBoot"},
	{FileManager::kPrefixPrefs, "kPrefixPrefs"},
	{FileManager::kPrefixSystem, "kPrefixSystem"}};

static uint_t BURGER_API TestPrefixes(uint_t uVerbose) BURGER_NOEXCEPT
{
	Filename MyFileName;

	// For testing, print out the current prefixes. Useful for debugging
	// on new platforms.

	if (uVerbose & VERBOSE_MSG) {
		uintptr_t uCount = BURGER_ARRAYSIZE(PrefixNameTable);
		const PrefixName_t* pWork = PrefixNameTable;
		do {
			FileManager::GetPrefix(&MyFileName, pWork->m_uPrefix);
			Message("FileManager::%s = \"%s\"", pWork->m_pPrefixName,
				MyFileName.c_str());
			++pWork;
		} while (--uCount);
	}

	// Check if all the other prefixes are empty on startup.

	uint_t i = 0;
	uint_t uFailure = FALSE;
	do {
		if ((i != FileManager::kPrefixCurrent) &&
			(i != FileManager::kPrefixApplication)) {
			FileManager::GetPrefix(&MyFileName, i);
			const char* pTest = MyFileName.c_str();
			const uint_t uTest = pTest[0] != 0;
			uFailure |= uTest;
			ReportFailure(
				"FileManager::GetPrefix(%d) = \"%s\"", uTest, i, pTest);
		}
	} while (++i < FileManager::kPrefixBoot);
	return uFailure;
}

/***************************************

	Test Filename.DirName()

***************************************/

struct TestDirName_t {
	const char* m_pOriginal;
	const char* m_pExpected;
};

static const TestDirName_t TestDirNames[] = {{":foo:bar", ":foo:"},
	{".d3:foo:bar", ".d3:foo:"}, {":foo:", ":foo:"}, {".d3:", ".d3:"},
	{":foo:bar:", ":foo:"},
	{":one:two:three:four:five", ":one:two:three:four:"},
	{":one:two:three:four:five:", ":one:two:three:four:"}, {"one", "one"},
	{"one:two", "one:"}, {"one:two:", "one:"}};

static uint_t BURGER_API TestFilenameDirName(void) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;
	const TestDirName_t* pTestDirName = TestDirNames;
	uint_t i = static_cast<uint_t>(BURGER_ARRAYSIZE(TestDirNames));
	do {
		Filename Test(pTestDirName->m_pOriginal);
		Test.dirname();
		const uint_t uTest =
			StringCompare(Test.c_str(), pTestDirName->m_pExpected) != 0;
		uFailure |= uTest;
		ReportFailure("DirName \"%s\" to \"%s\" but got \"%s\"", uTest,
			pTestDirName->m_pOriginal, pTestDirName->m_pExpected, Test.c_str());
		++pTestDirName;
	} while (--i);
	return uFailure;
}

/***************************************

	Test FileManager.Append()

***************************************/

struct TestAppend_t {
	const char* m_pOriginal;
	const char* m_pAppend;
	const char* m_pExpected;
};

static const TestAppend_t TestAppends[] = {
	{":foo:bar", "foo:", ":foo:bar:foo:"},
	{".d3:foo:bar", ".d3:foo:", ".d3:foo:bar:.d3:foo:"},
	{":foo:", "foo", ":foo:foo:"}, {".d3:", "bar:", ".d3:bar:"},
	{":foo:bar:", "foo:", ":foo:bar:foo:"},
	{":one:two:three:four:five",
		"one:two:three:four:", ":one:two:three:four:five:one:two:three:four:"},
	{":one:two:three:four:five:", "one:two:three:four:",
		":one:two:three:four:five:one:two:three:four:"},
	{"one", "one", "one:one:"}, {"one:two", "one:", "one:two:one:"},
	{"one:two:", "one:", "one:two:one:"}};

static uint_t BURGER_API TestFilenameAppend(void) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;
	const TestAppend_t* pTestAppend = TestAppends;
	uint_t i = static_cast<uint_t>(BURGER_ARRAYSIZE(TestAppends));
	do {
		Filename Test(pTestAppend->m_pOriginal);
		Test.join(pTestAppend->m_pAppend);
		const uint_t uTest =
			StringCompare(Test.c_str(), pTestAppend->m_pExpected) != 0;
		uFailure |= uTest;
		ReportFailure(
			"Append \"%s\" to \"%s\" but got \"%s\" and expected \"%s\"", uTest,
			pTestAppend->m_pOriginal, pTestAppend->m_pAppend, Test.c_str(),
			pTestAppend->m_pExpected);
		++pTestAppend;
	} while (--i);
	return uFailure;
}

/***************************************

	Test Filename.Expand() prepending the current directory

***************************************/

static uint_t BURGER_API TestExpandFull(const char* pInput) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	String TempString(pInput);
	Filename MyFilename(pInput);
	uint_t uTest = StringCompare(pInput, MyFilename.c_str()) != 0;
	uFailure |= uTest;
	ReportFailure(
		"TestExpandFull() Original is \"%s\"", uTest, MyFilename.c_str());

	MyFilename.Expand();
	TempString += ':';
	uTest = StringCompare(TempString.c_str(), MyFilename.c_str()) != 0;
	uFailure |= uTest;
	ReportFailure(
		"TestExpandFull() Expand from \"%s\" to \"%s\" but got \"%s\"", uTest,
		pInput, TempString.c_str(), MyFilename.c_str());
	return uFailure;
}

/***************************************

	Test Filename.Expand() prepending the current directory

***************************************/

static uint_t BURGER_API TestPrepend(
	const char* pInput, const Filename* pWorkDir) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	String TempString;

	Filename MyFilename(pInput);
	uint_t uTest = StringCompare(pInput, MyFilename.c_str()) != 0;
	uFailure |= uTest;
	ReportFailure(
		"TestPrepend() Original is \"%s\"", uTest, MyFilename.c_str());

	MyFilename.Expand();
	TempString.assign(pWorkDir->c_str());
	TempString += pInput;
	TempString += ':';
	uTest = StringCompare(TempString.c_str(), MyFilename.c_str()) != 0;
	uFailure |= uTest;
	ReportFailure("Expand from \"%s\" to \"%s\" but got \"%s\"", uTest, pInput,
		TempString.c_str(), MyFilename.c_str());
	return uFailure;
}

/***************************************

	Test Filename.Expand() popping current directory

***************************************/

static const char g_PeriodName[] = "........:Help me pfx";

static uint_t BURGER_API TestPopDir(const Filename* pWorkDir) BURGER_NOEXCEPT
{
	const uint_t uPeriodCount = 8;
	uint_t uFailure = FALSE;
	String TempString;
	Filename MyFilename;
	uint_t i = uPeriodCount;
	do {
		TempString.assign((g_PeriodName + i) - 1);
		MyFilename.assign(TempString);

		uint_t uTest =
			StringCompare(TempString.c_str(), MyFilename.c_str()) != 0;
		uFailure |= uTest;
		ReportFailure(
			"TestPopDir() Original is \"%s\"", uTest, MyFilename.c_str());

		MyFilename.Expand();

		Filename TempDir(*pWorkDir);
		uint_t y = uPeriodCount - i;
		if (y) {
			do {
				TempDir.dirname();
			} while (--y);
		}

		TempDir.join(g_PeriodName + uPeriodCount + 1);
		uTest = StringCompare(TempDir.c_str(), MyFilename.c_str()) != 0;
		uFailure |= uTest;
		ReportFailure("Expand from \"%s\" to \"%s\" but got \"%s\"", uTest,
			TempString.c_str(), TempDir.c_str(), MyFilename.c_str());

	} while (--i);
	return uFailure;
}

/***************************************

	Test Filename.Expand() inserting a prefix

***************************************/

static uint_t BURGER_API TestPrefixDir(
	const char* pInput, uint_t uPrefix) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	Filename MyFilename(pInput);
	uint_t uTest = StringCompare(pInput, MyFilename.c_str()) != 0;
	uFailure |= uTest;
	ReportFailure(
		"TestPrefixDir() Original is \"%s\"", uTest, MyFilename.c_str());

	MyFilename.Expand();
	Filename TempString;
	FileManager::GetPrefix(&TempString, uPrefix);
	TempString.join(StringCharacter(pInput, ':') + 1);

	uTest = StringCompare(TempString.c_str(), MyFilename.c_str()) != 0;
	uFailure |= uTest;
	ReportFailure("Expand from \"%s\" to \"%s\" but got \"%s\"", uTest, pInput,
		TempString.c_str(), MyFilename.c_str());
	return uFailure;
}

/***************************************

	Test Filename.Expand()

***************************************/

static uint_t BURGER_API TestFilenameExpand(void) BURGER_NOEXCEPT
{
	// Boot volume name is needed for tests to work on multiple platforms
	Filename BootVolume;
	Filename WorkBootDirectory;
	Filename SavedCurrentDir;
	FileManager::GetPrefix(&BootVolume, FileManager::kPrefixBoot);
	FileManager::GetPrefix(&SavedCurrentDir, FileManager::kPrefixCurrent);
	WorkBootDirectory = BootVolume;
	WorkBootDirectory.join("Two:Three:Four:Five");
	FileManager::SetPrefix(
		FileManager::kPrefixCurrent, WorkBootDirectory.c_str());

	uint_t uFailure = TestExpandFull(".D2:Help me");
	uFailure |= TestExpandFull(":Burger:foo.txt");
	uFailure |= TestExpandFull(":Burger:a:foo.txt");
	uFailure |= TestExpandFull(":Burger:9.txt");

	uFailure |= TestPrepend(".D:Help me", &WorkBootDirectory);
	uFailure |= TestPrepend(".D2x:Help me", &WorkBootDirectory);
	uFailure |= TestPrepend(".Help me start", &WorkBootDirectory);

	uFailure |= TestPopDir(&WorkBootDirectory);
	uFailure |= TestPrefixDir("8:Six:Help me pfx", FileManager::kPrefixCurrent);
	uFailure |= TestPrefixDir("8:Help me pfx", FileManager::kPrefixCurrent);
	uFailure |= TestPrefixDir("*:Help me pfx", FileManager::kPrefixBoot);
	uFailure |= TestPrefixDir("$:Help me pfx", FileManager::kPrefixSystem);
	uFailure |= TestPrefixDir("@:Help me pfx", FileManager::kPrefixPrefs);
	uFailure |= TestPrefixDir("9:Help me pfx", FileManager::kPrefixApplication);

	// Restore the current directory
	FileManager::SetPrefix(
		FileManager::kPrefixCurrent, SavedCurrentDir.c_str());
	return uFailure;
}

/***************************************

	Test File

***************************************/

static uint_t BURGER_API TestFile(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;
	TimeDate_t Time;

	// Assume everyone else is stressing
	Filename TestName(GetWorkFilename());

	File TestFile(&TestName, File::kWriteOnly);

	uint_t uTest =
		TestFile.Write("This is a test", sizeof("This is a test") - 1) != 14;
	uFailure |= uTest;
	ReportFailure("File::Write", uTest);

	uTest = TestFile.GetModificationTime(&Time) != kErrorNone;
	uFailure |= uTest;
	ReportFailure("File::GetModificationTime", uTest);
	if (uTest || (uVerbose & VERBOSE_MSG)) {
		char Output[128];
		char Output2[128];
		Time.DateToStringVerbose(Output);
		Time.TimeToStringPM(Output2);
		Message("File modification time = %s %s", Output, Output2);
	}
	const uint64_t uSize = TestFile.GetSize();
	uTest = uSize != 14;
	uFailure |= uTest;
	ReportFailure("File::GetSize return %u, expected 14", uTest,
		static_cast<uint_t>(uSize));

	uTest = TestFile.Close() != kErrorNone;
	uFailure |= uTest;
	ReportFailure("File::Close", uTest);

	FileManager::DeleteFile(&TestName);
	return uFailure;
}

/***************************************

	Create some temp files

***************************************/

static void BURGER_API TestCreateTempFiles(void) BURGER_NOEXCEPT
{
	FileManager::SaveFile(
		"9:testfile.txt", g_SampleData, sizeof(g_SampleData) - 1);
	FileManager::SaveFile(
		GetWorkFilename(), g_SampleData, sizeof(g_SampleData) - 1);
#if defined(BURGER_MACOS)
	FileManager::SetFileAndAuxType("9:testfile.txt", 'TEXT', 'CWIE');
	FileManager::SetFileAndAuxType(GetWorkFilename(), 'TEXT', 'CWIE');
#endif
}

/***************************************

	Dispose of the temp files

***************************************/

static void BURGER_API TestDisposeTempFiles(void) BURGER_NOEXCEPT
{
	FileManager::DeleteFile("9:testfile.txt");
	FileManager::DeleteFile(GetWorkFilename());
}

/***************************************

	Test FileManager.DoesFileExist()

***************************************/

static uint_t BURGER_API TestDoesFileExist(void) BURGER_NOEXCEPT
{
	uint_t uReturn = FileManager::DoesFileExist("9:nothere.txt");
	uint_t uTest = uReturn != 0;
	uint_t uFailure = uTest;
	ReportFailure(
		"FileManager::DoesFileExist(\"9:nothere.txt\") = %u", uTest, uReturn);

	uReturn = FileManager::DoesFileExist("9:testfile.txt");
	uTest = uReturn == 0;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::DoesFileExist(\"9:testfile.txt\") = %u", uTest, uReturn);

	uReturn = FileManager::DoesFileExist(GetWorkFilename());
	uTest = uReturn == 0;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::DoesFileExist(GetWorkFilename()) = %u", uTest, uReturn);

	return uFailure;
}

/***************************************

	Test the load and save commands

***************************************/

static uint_t BURGER_API TestLoadSave(void) BURGER_NOEXCEPT
{
	// Save the file
	Burger::eError uReturn = FileManager::SaveFile(
		"9:ooga.txt", g_SampleData, sizeof(g_SampleData) - 1);
	uint_t uTest = uReturn != kErrorNone;
	uint_t uFailure = uTest;
	ReportFailure(
		"FileManager::SaveFile(\"9:ooga.txt\",g_SampleData,sizeof(g_SampleData)-1) = %d",
		uTest, uReturn);

	// Determine if the file is present
	uint_t uTrue = FileManager::DoesFileExist("9:ooga.txt");
	uTest = uTrue == 0;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::SaveFile(\"9:ooga.txt\",g_SampleData,sizeof(g_SampleData)-1 didn't save!",
		uTest);

	// Load the file
	uintptr_t uLength;
	void* pReturn = FileManager::LoadFile("9:ooga.txt", &uLength);
	uTest = (!pReturn) || (uLength != (sizeof(g_SampleData) - 1));
	uFailure |= uTest;
	ReportFailure("FileManager::LoadFile(\"9:ooga.txt\") = %08X", uTest,
		static_cast<unsigned int>(uLength));
	if (pReturn) {
		uTest =
			MemoryCompare(pReturn, g_SampleData, sizeof(g_SampleData) - 1) != 0;
		uFailure |= uTest;
		ReportFailure(
			"FileManager::LoadFile(\"9:ooga.txt\") return bad data", uTest);
		Free(pReturn);
	}

	// Copy the file
	uReturn = FileManager::CopyFile("9:booga.txt", "9:ooga.txt");
	uTest = uReturn != kErrorNone;
	uFailure = uTest;
	ReportFailure("FileManager::CopyFile(\"9:booga.txt\",\"9:ooga.txt\") = %u",
		uTest, uReturn);

	// Determine if the file is present
	uTrue = FileManager::DoesFileExist("9:booga.txt");
	uTest = uTrue == 0;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::CopyFile(\"9:booga.txt\",\"9:ooga.txt\") didn't copy!",
		uTest);

	// Rename the file
	uReturn = FileManager::RenameFile("9:wooga.txt", "9:booga.txt");
	uTest = uReturn != kErrorNone;
	uFailure = uTest;
	ReportFailure(
		"FileManager::RenameFile(\"9:wooga.txt\",\"9:booga.txt\") = %u", uTest,
		uReturn);

	// Determine if the file is present
	uTrue = FileManager::DoesFileExist("9:wooga.txt");
	uTest = uTrue == 0;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::RenameFile(\"9:wooga.txt\",\"9:booga.txt\") didn't rename!",
		uTest);

	// Check for fp opening
	FILE* fp = FileManager::OpenFile("9:wooga.txt", "rb");
	uTest = fp == nullptr;
	uFailure |= uTest;
	ReportFailure("FileManager::OpenFile(\"9:wooga.txt\",\"rb\")", uTest);
	if (fp) {
		fclose(fp);
	}

	// Delete the files
	uReturn = FileManager::DeleteFile("9:ooga.txt");
	uTest = uReturn != kErrorNone;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::DeleteFile(\"9:ooga.txt\") = %d", uTest, uReturn);

	uReturn = FileManager::DeleteFile("9:wooga.txt");
	uTest = uReturn != kErrorNone;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::DeleteFile(\"9:wooga.txt\") = %d", uTest, uReturn);

	// This SHOULD fail, because the file is gone. Test for failure case
	uReturn = FileManager::DeleteFile("9:booga.txt");
	uTest = uReturn == kErrorNone;
	uFailure |= uTest;
	ReportFailure("FileManager::DeleteFile(\"9:booga.txt\") (Should fail) = %d",
		uTest, uReturn);

	return uFailure;
}

/***************************************

	Test FileManager.DoesFileExist()

***************************************/

static uint_t BURGER_API TestGetModificationTime(
	uint_t uVerbose) BURGER_NOEXCEPT
{
	TimeDate_t MyTime;
	char TempBuffer[128];
	char TempBuffer2[128];

	uint32_t uReturn =
		FileManager::GetModificationTime("9:nothere.txt", &MyTime);
	uint_t uTest = uReturn == kErrorNone;
	uint_t uFailure = uTest;
	ReportFailure("FileManager::GetModificationTime(\"9:nothere.txt\") = %u",
		uTest, uReturn);

	uReturn = FileManager::GetModificationTime("9:testfile.txt", &MyTime);
	uTest = uReturn != 0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetModificationTime(\"9:testfile.txt\") = %d",
		uTest, uReturn);
	if (!uReturn && (uVerbose & VERBOSE_MSG)) {
		MyTime.DateToStringVerbose(TempBuffer);
		MyTime.TimeToStringPM(TempBuffer2);
		Message(
			"9:testfile.txt's modification was %s %s", TempBuffer, TempBuffer2);
	}

	uReturn = FileManager::GetModificationTime(GetWorkFilename(), &MyTime);
	uTest = uReturn != 0;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::GetModificationTime(\"GetWorkFilename()\") = %u", uTest,
		uReturn);
	if (!uReturn && (uVerbose & VERBOSE_MSG)) {
		MyTime.DateToStringVerbose(TempBuffer);
		MyTime.TimeToStringPM(TempBuffer2);
		Message("GetWorkFilename()'s modification was %s %s", TempBuffer,
			TempBuffer2);
	}
	return uFailure;
}

/***************************************

	Test FileManager.GetCreationTime()

***************************************/

static uint_t BURGER_API TestGetCreationTime(uint_t uVerbose) BURGER_NOEXCEPT
{
	TimeDate_t MyTime;
	char TempBuffer[128];
	char TempBuffer2[128];
	Burger::eError uReturn =
		FileManager::GetCreationTime("9:nope.txt", &MyTime);
	uint_t uTest = uReturn == kErrorNone;
	uint_t uFailure = uTest;
	ReportFailure(
		"FileManager::GetCreationTime(\"9:nope.txt\") = %u", uTest, uReturn);

	uReturn = FileManager::GetCreationTime("9:testfile.txt", &MyTime);
	uTest = uReturn != kErrorNone;
	uFailure |= uTest;
	ReportFailure("FileManager::GetCreationTime(\"9:testfile.txt\") = %u",
		uTest, uReturn);
	if (!uReturn && (uVerbose & VERBOSE_MSG)) {
		MyTime.DateToStringVerbose(TempBuffer);
		MyTime.TimeToStringPM(TempBuffer2);
		Message("9:testfile.txt's creation was %s %s", TempBuffer, TempBuffer2);
	}

	uReturn = FileManager::GetCreationTime(GetWorkFilename(), &MyTime);
	uTest = uReturn != kErrorNone;
	uFailure |= uTest;
	ReportFailure("FileManager::GetCreationTime(\"GetWorkFilename()\") = %u",
		uTest, uReturn);
	if (!uReturn && (uVerbose & VERBOSE_MSG)) {
		MyTime.DateToStringVerbose(TempBuffer);
		MyTime.TimeToStringPM(TempBuffer2);
		Message(
			"GetWorkFilename()'s creation was %s %s", TempBuffer, TempBuffer2);
	}
	return uFailure;
}

/***************************************

	Test FileManager.GetFileType()

***************************************/

static uint_t BURGER_API TestGetFileType(uint_t uVerbose) BURGER_NOEXCEPT
{
#if defined(BURGER_MACOS)
	uint32_t uReturn = FileManager::GetFileType("9:floof.txt");
	uint_t uTest = uReturn != 0;
	uint_t uFailure = uTest;
	ReportFailure(
		"FileManager::GetFileType(\"9:floof.txt\") = 0x%08X", uTest, uReturn);

	uReturn = FileManager::GetFileType("9:testfile.txt");
	uTest = uReturn == 0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetFileType(\"9:testfile.txt\") = 0x%08X",
		uTest, uReturn);
	if (uReturn && (uVerbose & VERBOSE_MSG)) {
		Message("9:testfile.txt's file type was 0x%08X", uReturn);
	}

	uReturn = FileManager::GetFileType(GetWorkFilename());
	uTest = uReturn == 0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetFileType(\"GetWorkFilename()\") = 0x%08X",
		uTest, uReturn);
	if (uReturn && (uVerbose & VERBOSE_MSG)) {
		Message("GetWorkFilename()'s file type was 0x%08X", uReturn);
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

static uint_t BURGER_API TestGetAuxType(uint_t uVerbose) BURGER_NOEXCEPT
{
#if defined(BURGER_MACOS)
	uint32_t uReturn = FileManager::GetAuxType("9:FileNotHere");
	uint_t uTest = uReturn != 0;
	uint_t uFailure = uTest;
	ReportFailure(
		"FileManager::GetAuxType(\"9:FileNotHere\") = 0x%08X", uTest, uReturn);

	uReturn = FileManager::GetAuxType("9:testfile.txt");
	uTest = uReturn == 0;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::GetAuxType(\"9:testfile.txt\") = 0x%08X", uTest, uReturn);
	if (uReturn && (uVerbose & VERBOSE_MSG)) {
		Message("9:testfile.txt's aux type was 0x%08X", uReturn);
	}

	uReturn = FileManager::GetAuxType(GetWorkFilename());
	uTest = uReturn == 0;
	uFailure |= uTest;
	ReportFailure("FileManager::GetAuxType(\"GetWorkFilename()\") = 0x%08X",
		uTest, uReturn);
	if (uReturn && (uVerbose & VERBOSE_MSG)) {
		Message("GetWorkFilename()'s aux type was 0x%08X", uReturn);
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

static uint_t BURGER_API TestGetFileAndAuxType(uint_t uVerbose) BURGER_NOEXCEPT
{
#if defined(BURGER_MACOS)
	uint32_t uFileType;
	uint32_t uAuxType;
	Burger::eError uReturn =
		FileManager::GetFileAndAuxType("9:FileNotHere", &uFileType, &uAuxType);
	uint_t uTest = uReturn == kErrorNone;
	uint_t uFailure = uTest;
	ReportFailure(
		"FileManager::GetFileAndAuxType(\"9:FileNotHere\") = %u File = 0x%08X, Aux = 0x%08X",
		uTest, uReturn, uFileType, uAuxType);

	uReturn =
		FileManager::GetFileAndAuxType("9:testfile.txt", &uFileType, &uAuxType);
	uTest = uReturn != kErrorNone;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::GetFileAndAuxType(\"9:testfile.txt\") = %u File = 0x%08X, Aux = 0x%08X",
		uTest, uReturn, uFileType, uAuxType);
	if (!uReturn && (uVerbose & VERBOSE_MSG)) {
		Message("9:testfile.txt's file was 0x%08X, aux was 0x%08X", uFileType,
			uAuxType);
	}

	uReturn = FileManager::GetFileAndAuxType(
		GetWorkFilename(), &uFileType, &uAuxType);
	uTest = uReturn != kErrorNone;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::GetFileAndAuxType(\"GetWorkFilename()\") = %u File = 0x%08X, Aux = 0x%08X",
		uTest, uReturn, uFileType, uAuxType);
	if (!uReturn && (uVerbose & VERBOSE_MSG)) {
		Message("GetWorkFilename()'s file was 0x%08X, aux was 0x%08X",
			uFileType, uAuxType);
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

static uint_t BURGER_API TestSetFileType(uint_t uVerbose) BURGER_NOEXCEPT
{
#if defined(BURGER_MACOS)
	BURGER_UNUSED(uVerbose);
	eError uReturn = FileManager::SetFileType("9:testfile.txt", 'ABCD');
	uint_t uTest = uReturn != kErrorNone;
	uint_t uFailure = uTest;
	ReportFailure("FileManager::SetFileType(\"9:testfile.txt\",'ABCD') = %d",
		uTest, uReturn);
	if (!uTest) {
		uint32_t uType = FileManager::GetFileType("9:testfile.txt");
		uTest = uType != 'ABCD';
		uFailure |= uTest;
		ReportFailure(
			"FileManager::SetFileType(\"9:testfile.txt\",'ABCD') = got 0x%08X",
			uTest, uType);
	}

	uReturn = FileManager::SetFileType("9:testfile.txt", 'TEXT');
	uTest = uReturn != kErrorNone;
	uFailure |= uTest;
	ReportFailure("FileManager::SetFileType(\"9:testfile.txt\",'TEXT') = %d",
		uTest, uReturn);
	if (!uTest) {
		uint32_t uType = FileManager::GetFileType("9:testfile.txt");
		uTest = uType != 'TEXT';
		uFailure |= uTest;
		ReportFailure(
			"FileManager::SetFileType(\"9:testfile.txt\",'TEXT') = got 0x%08X",
			uTest, uType);
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

static uint_t BURGER_API TestSetAuxType(uint_t uVerbose) BURGER_NOEXCEPT
{
#if defined(BURGER_MACOS)
	BURGER_UNUSED(uVerbose);
	Burger::eError uReturn = FileManager::SetAuxType("9:testfile.txt", 'ABCD');
	uint_t uTest = uReturn != kErrorNone;
	uint_t uFailure = uTest;
	ReportFailure("FileManager::SetAuxType(\"9:testfile.txt\",'ABCD') = %u",
		uTest, uReturn);
	if (!uTest) {
		uint32_t uType = FileManager::GetAuxType("9:testfile.txt");
		uTest = uType != 'ABCD';
		uFailure |= uTest;
		ReportFailure(
			"FileManager::SetAuxType(\"9:testfile.txt\",'ABCD') = got 0x%08X",
			uTest, uType);
	}

	uReturn = FileManager::SetAuxType("9:testfile.txt", 'CWIE');
	uTest = uReturn != kErrorNone;
	uFailure |= uTest;
	ReportFailure("FileManager::SetAuxType(\"9:testfile.txt\",'CWIE') = %u",
		uTest, uReturn);
	if (!uTest) {
		uint32_t uType = FileManager::GetAuxType("9:testfile.txt");
		uTest = uType != 'CWIE';
		uFailure |= uTest;
		ReportFailure(
			"FileManager::SetAuxType(\"9:testfile.txt\",'CWIE') = got 0x%08X",
			uTest, uType);
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

static uint_t BURGER_API TestSetFileAndAuxType(uint_t uVerbose) BURGER_NOEXCEPT
{
#if defined(BURGER_MACOS)
	BURGER_UNUSED(uVerbose);
	uint32_t uFileType;
	uint32_t uAuxType;
	uint32_t uReturn =
		FileManager::SetFileAndAuxType("9:testfile.txt", 'ABCD', 'ABCD');
	uint_t uTest = uReturn != 0;
	uint_t uFailure = uTest;
	ReportFailure(
		"FileManager::SetFileAndAuxType(\"9:testfile.txt\",'ABCD','ABCD') = %d",
		uTest, uReturn);
	if (!uTest) {
		uReturn = FileManager::GetFileAndAuxType(
			"9:testfile.txt", &uFileType, &uAuxType);
		uTest = (uReturn != 0) || (uFileType != 'ABCD') || (uAuxType != 'ABCD');
		uFailure |= uTest;
		ReportFailure(
			"FileManager::SetFileAndAuxType(\"9:testfile.txt\",'ABCD','ABCD') = got 0x%08X,0x%08X",
			uTest, uReturn);
	}

	uReturn = FileManager::SetFileAndAuxType("9:testfile.txt", 'TEXT', 'CWIE');
	uTest = uReturn != 0;
	uFailure |= uTest;
	ReportFailure(
		"FileManager::SetFileAndAuxType(\"9:testfile.txt\",'TEXT','CWIE') = %d",
		uTest, uReturn);
	if (!uTest) {
		uReturn = FileManager::GetFileAndAuxType(
			"9:testfile.txt", &uFileType, &uAuxType);
		uTest = (uReturn != 0) || (uFileType != 'TEXT') || (uAuxType != 'CWIE');
		uFailure |= uTest;
		ReportFailure(
			"FileManager::SetFileAndAuxType(\"9:testfile.txt\",'TEXT','CWIE') = got 0x%08X,0x%08X",
			uTest, uReturn);
	}
	return uFailure;
#else
	BURGER_UNUSED(uVerbose);
	return 0;
#endif
}

struct DirectoryTests_t {
	const char* m_pName;
	uint_t m_uDir;
};

static const DirectoryTests_t g_DirectoryFiles[] = {{"apple.txt", FALSE},
	{"berry.txt", FALSE}, {"cat.txt", FALSE}, {"delta", TRUE},
	{"fooey.txt", FALSE}};

static const char g_DirectoryToTest[] = "9:test";

static uint_t BURGER_API TestDirectorySearch(uint_t uVerbose) BURGER_NOEXCEPT
{
	// Table to test for files found when iterating
	uint_t HitTable[BURGER_ARRAYSIZE(g_DirectoryToTest)];
	uint_t DirTable[BURGER_ARRAYSIZE(g_DirectoryToTest)];

	FileManager::CreateDirectoryPath(g_DirectoryToTest);
	FileManager::SetPrefix(20, g_DirectoryToTest);
	uint_t i = 0;
	const DirectoryTests_t* pDirTests = g_DirectoryFiles;
	do {
		HitTable[i] = FALSE;
		DirTable[i] = BURGER_MAXUINT;
		Filename TestName("20:");
		TestName.join(pDirTests->m_pName);
		if (pDirTests->m_uDir) {
			FileManager::CreateDirectoryPath(&TestName);
		} else {
			FileManager::SaveFile(
				&TestName, g_SampleData, sizeof(g_SampleData));
		}
		++pDirTests;
	} while (++i < BURGER_ARRAYSIZE(g_DirectoryFiles));

	// Test directory traversal
	DirectorySearch MyDir;
	uint_t uTest;
	uint_t uFailure = FALSE;
	uint_t uReturn;

#if defined(BURGER_WINDOWS)
	uTest = sizeof(WIN32_FIND_DATAW) != 592;
	uFailure |= uTest;
	ReportFailure(
		"sizeof(MyDir.m_MyFindT) != %d", uTest, sizeof(WIN32_FIND_DATAW));
#endif

	uReturn = MyDir.Open(g_DirectoryToTest);
	uTest = uReturn != 0;
	uFailure |= uTest;
	ReportFailure("MyDir.Open(\"%s\") = %d", uTest, g_DirectoryToTest, uReturn);

	// Scan the directory and mark all the files that are found.
	uint_t uEntries = 0;
	while (!MyDir.GetNextEntry()) {
		i = 0;
		do {
			if (!StringCaseCompare(MyDir.m_Name, g_DirectoryFiles[i].m_pName)) {
				HitTable[i] = TRUE;
				DirTable[i] = MyDir.m_bDir;
				break;
			}
		} while (++i < BURGER_ARRAYSIZE(g_DirectoryFiles));
		if (i == BURGER_ARRAYSIZE(g_DirectoryFiles)) {
			uFailure |= 1;
			ReportFailure("MyDir.GetNextEntry() found unknown file %s", uTest,
				MyDir.m_Name);
		}
		if (uVerbose & VERBOSE_MSG) {

			Message("\nFile %s found!", MyDir.m_Name);
			char TempBuffer[128];
			char TempBuffer2[128];
			MyDir.m_CreationDate.DateToStringVerbose(TempBuffer);
			MyDir.m_CreationDate.TimeToStringPM(TempBuffer2);
			Message("Creation was %s %s", TempBuffer, TempBuffer2);
			MyDir.m_ModificatonDate.DateToStringVerbose(TempBuffer);
			MyDir.m_ModificatonDate.TimeToStringPM(TempBuffer2);
			Message("Modification was %s %s", TempBuffer, TempBuffer2);
			Message("Size %u, Hidden %d, System %d, Directory %d, Locked %d",
				MyDir.m_uFileSize, MyDir.m_bHidden, MyDir.m_bSystem,
				MyDir.m_bDir, MyDir.m_bLocked);
#if defined(BURGER_MACOS)
			Message("File type 0x%08X, Aux Type 0x%08X", MyDir.m_uFileType,
				MyDir.m_uAuxType);
#endif
		}
		++uEntries;
	}
	MyDir.Close();

	uTest = uEntries == 0;
	uFailure |= uTest;
	ReportFailure("MyDir.GetNextEntry(\"9\") didn't iterate", uTest, uReturn);
	// Disable to run manual tests of modification/creation time difference

	i = 0;
	do {
		if (!HitTable[i]) {
			uFailure |= 1;
			ReportFailure("MyDir.GetNextEntry() did not file the file %s",
				uTest, g_DirectoryFiles[i].m_pName);
		} else {
			uTest = (g_DirectoryFiles[i].m_uDir != DirTable[i]);
			uFailure |= uTest;
			ReportFailure(
				"MyDir.GetNextEntry() wrong dir flag for %s. expected %u, got %u",
				uTest, g_DirectoryFiles[i].m_pName,
				g_DirectoryFiles[i].m_uDir, DirTable[i]);
		}
	} while (++i < BURGER_ARRAYSIZE(g_DirectoryFiles));

	pDirTests = g_DirectoryFiles;
	i = 0;
	do {
		Filename TestName("20:");
		TestName.join(pDirTests->m_pName);
		FileManager::DeleteFile(&TestName);
		++pDirTests;
	} while (++i < BURGER_ARRAYSIZE(g_DirectoryFiles));
	FileManager::DeleteFile(g_DirectoryToTest);

	return uFailure;
}

/***************************************

	Test if setting the filename explicitly works.

***************************************/

uint_t BURGER_API TestBrFileManager(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uTotal = 0;

	if (uVerbose & VERBOSE_FILE) {

		// Test Filename
		if (uVerbose & VERBOSE_MSG) {
			Message("Running Filename tests");

			// Supports UTF8 filenames?
			const uint_t uTest = FileManager::IsUTF8FileSystem();
			Message("FileManager::IsUTF8FileSystem() = %u", uTest);
		}

		// Verify Filename
		uTotal |= TestFilenameClass();
		uTotal |= TestFilenameSet();
		uTotal |= TestFilenameClear();
		uTotal |= TestFilenameIs();
		uTotal |= TestFilenameJoin();

		// Test default prefixes and volume names
		TestShowDirectories(uVerbose);
		uTotal |= TestGetVolumeName(uVerbose);
		uTotal |= TestPrefixes(uVerbose);
		uTotal |= TestFilenameDirName();
		uTotal |= TestFilenameAppend();
		uTotal |= TestFilenameExpand();

		uTotal |= TestFile(uVerbose);

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
		if (uVerbose & VERBOSE_MSG) {
			Message("Running Directory tests");
		}
		uTotal |= TestDirectorySearch(uVerbose);
	}
	return uTotal;
}
