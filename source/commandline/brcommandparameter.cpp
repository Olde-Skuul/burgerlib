/***************************************

	Base class for command parameters

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brcommandparameter.h"
#include "brstringfunctions.h"
#include <stdio.h>

static const char *s_HelpCommands[] = {
	"?",
	"h",
	"help"
};

/*! ************************************

	\class Burger::CommandParameter
	\brief Base class for command line parameters

	For ease of parsing command lines for tools,
	derived classes for each data type are 
	created and then pooled to handle most parsing
	needs. It is modeled after the argparse class
	found in python.

	\code
	Burger::CommandParameterBooleanTrue ShowAsm("Save as asm hex source","h");
	Burger::CommandParameterBooleanTrue ShowC("Save as \"C\" source","c");

	Burger::CommandParameterWordPtr Frequency("Playback frequency","f",44100,400,100000);
	Burger::CommandParameterWordPtr Length("Length to dump","l",BURGER_MAXWORDPTR);

	const CommandParameter *MyParms[] = {
		&ShowAsm,
		&ShowC,
		&Frequency,
		&Length
	};
	argc = CommandParameter::Process(argc,argv,MyParms,sizeof(MyParms)/sizeof(MyParms[0]),
		"Dump's the contents of a file to stdout.\nCopyright by Rebecca Ann Heineman\n",2);
	if (argc<0) {
		Globals::SetErrorCode(10);
	} else {
		if (ShowAsm.GetValue()) {
			ShowAsmCode();
		}
		if (ShowC.GetValue()) {
			ShowCCode();
		}
		PlaySound(Frequency.GetValue(),Length.GetValue());
	}
	\endcode

***************************************/

/*! ************************************

	\fn Burger::CommandParameter::CommandParameter(const char *pHelp,const char * const *ppParameterNames,WordPtr uParameterCount)
	\brief Base class constructor

	Construct the base data needed for a CommandParameter

	\param pHelp Message Pointer to a "C" string containing the help message for this parameter or \ref NULL if help is not available.
	\param ppParameterNames	Either a pointer to an array of "C" strings or a pointer to a single "C" string.
	\param uParameterCount Number of entries in the array (0 if the array is a single direct pointer)

	\sa Burger::CommandParameterBooleanTrue, Burger::CommandParameterWordPtr

***************************************/

/*! ************************************

	\fn Word Burger::CommandParameter::IsHelpAvailable(void) const
	\brief Determine if help is available.

	\return \ref TRUE if there is a help string available, \ref FALSE if not.

	\sa GetHelp(void) const

***************************************/

/*! ************************************

	\fn Burger::CommandParameter::GetHelp(void) const
	\brief Return the help string

	\return A pointer to a "C" string for the help or \ref NULL if there isn't any.

	\sa IsHelpAvailable(void) const

***************************************/

/*! ************************************

	\fn int Burger::CommandParameter::Action(int argc,const char **argv)
	\brief Process the parameters for a command

	Process the parameters and return the number of parameters used including the
	parameter that invoked this command.

	Example: If "-a", then return 1, if "-length 2000", return 2

	If there was a parsing error, return -1

	\note Do not attempt to manually remove parameters from the input. It will
		be performed by the caller with the returned value (Number of parameters processed)

	\param argc Number of entries remaining in the parameter array (Can be zero)
	\param argv Pointer to an array of "C" string pointers.
	\return Number of parameters processed or -1 on error or 1 for no parameters needed.

	\sa RemoveParms() or Process()

***************************************/

/*! ************************************

	\brief Remove entries from a "C" command list

	Index into an array of "C" string pointers, index into the array
	and remove the requested number of entries. Returns the number of
	entries remaining in the array.

	\param argc Number of parameters in the argv list.
	\param argv Pointer to an array of "C" strings to parse a command line from.
	\param iIndexToArgv Entry into the array to start entry removal from.
	\param iRemoveCount Number of entries to remove.

	\return The number of entries remaining in the arrray.

***************************************/

int BURGER_API Burger::CommandParameter::RemoveParms(int argc,const char **argv,int iIndexToArgv,int iRemoveCount)
{
	if (iRemoveCount) {				// Any work to be done?
		if ((iIndexToArgv+iRemoveCount) >= argc) {	// Off the end?
			argc = iIndexToArgv;	// Just truncate the table to the end
		} else {
			argc-=iRemoveCount;				// Get the adjusted count
			iRemoveCount+=iIndexToArgv;		// Index to the NEXT entry
			do {
				argv[iIndexToArgv] = argv[iRemoveCount];	// Copy the entry
				++iRemoveCount;				// Next source
			} while (++iIndexToArgv<argc);	// All done?
		}
	}
	return argc;		// Return the new count
}

/*! ************************************

	\brief Process an array of command parameters

	Given a list of command table parsers, scan
	the parameter list for matches and invoke their
	functions to calculate the values.

	Each parameter is checked for a prefix of '-' and then
	it's matched to the command strings found in each
	CommandParameter and if processed, the entry will be removed
	from the list. When this function is complete, only the
	unprocessed entries will remain

	\note On Windows and MSDOS platforms, the prefix '/' is accepted as
		well as '-'

	\param argc Number of parameters in the argv list.
	\param argv Pointer to an array of "C" strings to parse a command line from.
	\param ppParms Array of pointers to CommandParameter objects
	\param uParmCount Number of entries in the ppParms array
	\param pUsage Pointer to a "C" string with copyright and usage, or \ref NULL if no usage is present
	\param iMin Minimum number of allowable parameters or zero if no min/max test is to be performed
	\param iMax Maximum number of allowable parameters or if it must match iMin
	\return -1 on error (Or help printed) or the number of unprocessed command line entries.

	\sa Burger::CommandParameter

***************************************/

int Burger::CommandParameter::Process(int argc,const char **argv,const CommandParameter **ppParms,
	WordPtr uParmCount,const char *pUsage,int iMin,int iMax)
{
	// Don't trigger help

	Word uHelp = FALSE;

	// Any parms in the list?
	if (uParmCount) {
		// Start at #1, since #0 is the name if the application
		int i = 1;
		while (i<argc) {				// As long as there are parms left
			Word uLetter = argv[i][0];	// Get the first char

			// All platforms support '-' for commands
			if (uLetter=='-'
				// Windows allows /a /b type commands
#if defined(BURGER_MSDOS) || defined(BURGER_WINDOWS)
				|| uLetter=='/'
#endif
				) {	
				// Valid prefix char?
				// Get second char
				const char *pParmText = &argv[i][1];

				// Iterate over the list of commands to test against
				const CommandParameter **ppWorkParms = ppParms;		// Reset the table
				WordPtr uListCount = uParmCount;

				do {
					CommandParameter *pWorkParm = const_cast<CommandParameter *>(ppWorkParms[0]);

					const char *pTableOfOne;		// Table for single entry

					WordPtr uCommandCount = pWorkParm->m_uParameterCount;	// Get command count
					const char * const *ppParameterNames;
					if (!uCommandCount) {
						pTableOfOne = reinterpret_cast<const char *>(pWorkParm->m_ppParameterNames);
						ppParameterNames = &pTableOfOne;
						uCommandCount = 1;
					} else {
						ppParameterNames = pWorkParm->m_ppParameterNames;
					}

					// Test again all of the entries for this object

					do {
						const char *pParameterName = ppParameterNames[0];
						++ppParameterNames;
						WordPtr uCommandLen = StringLength(pParameterName);
						// Is this a match?
						if (!MemoryCaseCompare(pParameterName,pParmText,uCommandLen)) {
							pParmText += uCommandLen;
							int iParmRemoval;
							if (pParmText[0]) {
								// Cache the original
								const char *pCache = argv[i];
								// Change to remove the -foo prefix
								argv[i] = pParmText;
								// Perform the command
								iParmRemoval = pWorkParm->Action(argc-i,argv+i);
								// Only one entry to remove? This is an error
								if (iParmRemoval == 1) {
									iParmRemoval = -1;
								} else {
									// Prevent a double removal of an entry
									--iParmRemoval;
								}
								// Restore the original
								argv[i] = pCache;
							} else {
								// Process normally
								iParmRemoval = pWorkParm->Action((argc-i)-1,argv+i+1);
							}
							// If negative, an error occurred
							if (iParmRemoval<0) {
								// Force help
								uHelp = TRUE;
								// Force loop exit
								i = argc-1;
							} else {
								// Remove the requested parameters
								argc = RemoveParms(argc,argv,i,iParmRemoval);
								--i;		// Undo the future ++i
							}
							// Force outer loop exit
							uListCount = 1;
							break;
						}
					} while (--uCommandCount);
					++ppWorkParms;	/* Next entry */
				} while (--uListCount);
			}
			++i;			// Parse the next parm
		}
	}

	//
	// After all the parameters are tested, test for help
	//

	if (pUsage && !uHelp) {
		// Start at #1, since #0 is the name if the application
		int i = 1;
		while (i<argc) {				// As long as there are parms left
			Word uLetter = argv[i][0];	// Get the first char

			// All platforms support '-' for commands
			if (uLetter=='-'
				// Windows allows /a /b type commands
#if defined(BURGER_MSDOS) || defined(BURGER_WINDOWS)
				|| uLetter=='/'
#endif
				) {	
				// Valid prefix char?
				// Get second char
				const char *pParmText = &argv[i][1];

				// Test again all of the entries for this object
				const char **ppParameterNames = s_HelpCommands;
				WordPtr uListCount = sizeof(s_HelpCommands)/sizeof(s_HelpCommands[0]);
				do {
					const char *pParameterName = ppParameterNames[0];
					++ppParameterNames;
					// Is this a match?
					if (!StringCaseCompare(pParameterName,pParmText)) {
						uHelp = TRUE;
						argc = i-1;
						break;
					}
				} while (--uListCount);
			}
			++i;			// Parse the next parm
		}
	}
	//
	// Test the min/max bounds and force help if out of bounds
	//

	if (iMin) {
		if (argc<iMin) {
			uHelp = TRUE;
		} else {
			if (!iMax) {
				iMax = iMin;
			}
			if (argc>iMax) {
				uHelp = TRUE;
			}
		}
	}

	//
	// If help was invoked, display it and return an error
	//

	if (uHelp) {

		// Print the pUsage if available
		Word uFirst = TRUE;
		if (pUsage) {
			printf("%s\nOptional arguments:\n  -?, -h, -help\tShow this help message and exit\n",pUsage);
			uFirst = FALSE;
		}

		// Print the command parameter help
		if (uParmCount) {
			// Iterate over the list of commands to test against
			const CommandParameter **ppWorkParms = ppParms;		// Reset the table
			do {
				CommandParameter *pWorkParm = const_cast<CommandParameter *>(ppWorkParms[0]);
				const char *pHelp = pWorkParm->m_pHelp;
				if (pHelp) {
					if (uFirst) {
						uFirst = FALSE;
						printf("Optional arguments:\n");
					}
					WordPtr uEntries = pWorkParm->m_uParameterCount;
					if (uEntries) {
						const char * const *ppNames = pWorkParm->m_ppParameterNames;
						printf(" ");
						do {
							printf(" -%s",ppNames[0]);
							if (uEntries!=1) {
								printf(",");
							}
							++ppNames;
						} while (--uEntries);
					} else {
						printf("  -%s",reinterpret_cast<const char *>(pWorkParm->m_ppParameterNames));
					}
					printf("\t%s\n",pHelp);
				}
				++ppWorkParms;	/* Next entry */
			} while (--uParmCount);		
		}
		// Force an error
		argc = -1;
	}
	return argc;		// Return the NEW count
}
