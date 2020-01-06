/***************************************

    Class to capture stdout and stderr

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCAPTURESTDOUT_H__
#define __BRCAPTURESTDOUT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

#ifndef __BROUTPUTMEMORYSTREAM_H__
#include "broutputmemorystream.h"
#endif

/* BEGIN */
namespace Burger {

class CaptureStdout {
    BURGER_DISABLE_COPY(CaptureStdout);

private:
    uintptr_t m_uCaptureSize; ///< Maximum size of the capture pipe
    uint_t m_bActive; ///< \ref TRUE if currently capturing stream output

#if ((defined(BURGER_WINDOWS) || defined(BURGER_MACOSX) || \
         defined(BURGER_LINUX) || defined(BURGER_MSDOS)) && \
    !defined(__MSL__)) || \
    defined(DOXYGEN)
    int m_iInputPipe;      ///< New input pipe
    int m_iOutputPipe;     ///< New output pipe
    int m_iPreviousStdout; ///< Previous stdout pipe
    int m_iPreviousStderr; ///< Previous stderr pipe
#endif

#if defined(__MSL__) || defined(DOXYGEN)
    void* m_pSaved[2 * 4]; ///< Saved function pointers from stdout, stderr
    OutputMemoryStream m_Stream; ///< Memory stream for capturing output

    static int BURGER_ANSIAPI ReadIntercept(unsigned long pFile,
        unsigned char* pBuff, uintptr_t* pCount, void* pThat);
    static int BURGER_ANSIAPI WriteIntercept(unsigned long pFile,
        unsigned char* pBuff, uintptr_t* pCount, void* pThat);
    static int BURGER_ANSIAPI SeekIntercept(
        unsigned long pFile, unsigned long* pPosition, int mode, void* pThat);
#endif

public:
    CaptureStdout() BURGER_NOEXCEPT;
    ~CaptureStdout();
    eError BURGER_API Init(uintptr_t uBufferSize = 65536U) BURGER_NOEXCEPT;
    eError BURGER_API Shutdown(void) BURGER_NOEXCEPT;
    eError BURGER_API GetCapture(String* pOutput) BURGER_NOEXCEPT;
};

}
/* END */

#endif
