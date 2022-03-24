/***************************************

    Class to capture stdout and stderr

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brcapturestdout.h"

// Most platforms support Posix
#if (defined(BURGER_WINDOWS) || defined(BURGER_MACOSX) || \
    defined(BURGER_LINUX)) && \
    !defined(__MSL__)

#if defined(BURGER_MACOSX) || defined(BURGER_LINUX)
#include <unistd.h>
#else
#include <io.h>
#include <limits.h>
#endif

#include <fcntl.h>
#define USE_POSIX

/***************************************

    Metrowerks is... Uh... Yeah...
    Deal with it.

    Because, reasons.

    Metroworks Standard Library doesn't support pipe, and requires callback
    interception in order to redirect stdout and stderr.

    There is a difference in the callbacks for MSL version 0x700 which is
    reflected in the code with the __MSL__ version check.

***************************************/

#elif defined(__MSL__)
#endif

/*! ************************************

    \class Burger::CaptureStdout
    \brief Class to capture stdout and stderr

    For quick redirection of the stdout and stderr output stream, create an
    instance of this class, and when capturing is needed create code like the
    example below.

    \code

    void foo()
    {
        Burger::CaptureStdout Capture;

        // Use a small buffer.
        Capture.Init(128);

        // Send text to stdout.
        printf("Hello World!");

        // Obtain the captured text.
        Burger::String Text;
        if (Capture.GetCapture(&Text) == Burger::kErrorNone) {
            // Capture was successful.
            // Text will contain "Hello World!" and nothing will have been
            // printed to the console.
        }

        // Stop capturing and release buffers
        Capture.Shutdown();
    }
    \endcode

    \note This class affects stdout and stderr globally. Use this class with
        caution.

    \sa Burger::OutputMemoryStream

***************************************/

/*! ************************************

    \brief Construct a CaptureStdout class

***************************************/

Burger::CaptureStdout::CaptureStdout() BURGER_NOEXCEPT: m_uCaptureSize(0),
                                                        m_bActive(FALSE)
#if defined(USE_POSIX)
    ,
                                                        m_iInputPipe(-1),
                                                        m_iOutputPipe(-1),
                                                        m_iPreviousStdout(-1),
                                                        m_iPreviousStderr(-1)
#endif
{
}

/*! ************************************

    \brief Destroy a CaptureStdout class

***************************************/

Burger::CaptureStdout::~CaptureStdout()
{
    Shutdown();
}

/*! ************************************

    \brief Intercept stdout and stderr

    Create a pipe with a specific byte length and redirect stdout and stderr
    into the new pipe.

    \param uBufferSize Number of bytes to allocate for the pipe's buffer
    \return Error code
    \sa Shutdown(void) or GetCapture(String *)

***************************************/

Burger::eError BURGER_API Burger::CaptureStdout::Init(
    uintptr_t uBufferSize) BURGER_NOEXCEPT
{
    BURGER_UNUSED(uBufferSize);

    // Stop capturing
    eError uError = Shutdown();
    if (uError == kErrorNone) {

        // Most platforms support Posix, use it.
#if defined(USE_POSIX)
        // Assume error
        uError = kErrorNoMoreFileDescriptors;

        // Create the pipes with a specific buffer
        int Pipes[2];

        // MacOSX and Linux use self adjusting buffers
#if defined(BURGER_MACOSX) || defined(BURGER_LINUX)
        if (pipe(Pipes) != -1)
#else

        // Windows and DOS require a specific buffer
        // Since the parameter is only 32 bit, handle the rare overflow case.

#if defined(BURGER_64BITCPU)
        uBufferSize = Min(uBufferSize, static_cast<uintptr_t>(UINT_MAX));
#endif

        if (_pipe(Pipes, static_cast<unsigned int>(uBufferSize), O_BINARY) !=
            -1)
#endif
        {
            m_iInputPipe = Pipes[0];
            m_iOutputPipe = Pipes[1];

            // Make a copy of the stdout and stderr file descriptors
            int iTemp = dup(fileno(stdout));
            if (iTemp != -1) {
                m_iPreviousStdout = iTemp;

                iTemp = dup(fileno(stderr));
                if (iTemp != -1) {
                    m_iPreviousStderr = iTemp;

                    // Ready to do the switch, ensure the streams are flushed
                    fflush(stdout);
                    fflush(stderr);

                    // Set output to the new output pipe for capturing
                    dup2(m_iOutputPipe, fileno(stdout));
                    dup2(m_iOutputPipe, fileno(stderr));
                    m_bActive = TRUE;
                    m_uCaptureSize = uBufferSize;

                    // Initialized just fine
                    uError = kErrorNone;
                }
            }
        }

#elif defined(__MSL__)

        // Ready to do the switch, ensure the streams are flushed
        fflush(stdout);
        fflush(stderr);

        // Copy the file records for stdout and stderr
        std::_FILE* pSource = &std::__files[1];
        void** ppDest = m_pSaved;
        uintptr_t i = 2;
        do {
            // Copy the raw data

            // Intercept writes
            ppDest[0] = pSource->read_proc;
            pSource->read_proc =
                reinterpret_cast<std::__io_proc>(&ReadIntercept);

            ppDest[1] = pSource->write_proc;
            pSource->write_proc =
                reinterpret_cast<std::__io_proc>(&WriteIntercept);

            ppDest[2] = pSource->position_proc;
            pSource->position_proc =
                reinterpret_cast<std::__pos_proc>(&SeekIntercept);
            // Save the refcon
#if __MSL__ >= 0x7000
            ppDest[3] = pSource->ref_con;
            pSource->ref_con = reinterpret_cast<std::__ref_con>(this);
#else
            // Older MSL support
            ppDest[3] = pSource->idle_proc;
            pSource->idle_proc = reinterpret_cast<std::__idle_proc>(this);
#endif

            ++pSource;
            ppDest += 4;
        } while (--i);
        m_bActive = TRUE;
        m_uCaptureSize = uBufferSize;

        // Initialized just fine
        uError = kErrorNone;

#else
        // Not supported
        uError = kErrorNotSupportedOnThisPlatform;
#endif
    }
    return uError;
}

/*! ************************************

    \brief Restore stdout and stderr

    Detach the interception pipe and dispose of the
    pipe.

    \return Error code
    \sa Init(uintptr_t) or GetCapture(String *)

***************************************/

Burger::eError BURGER_API Burger::CaptureStdout::Shutdown(void) BURGER_NOEXCEPT
{
    // Flush the output (To ensure everything is captured)
    fflush(stdout);
    fflush(stderr);

#if defined(USE_POSIX)
    // Restore the pipes back the way they were before interception
    if (m_iPreviousStdout > 0) {
        dup2(m_iPreviousStdout, fileno(stdout));
        close(m_iPreviousStdout);
        m_iPreviousStdout = -1;
    }
    if (m_iPreviousStderr > 0) {
        dup2(m_iPreviousStderr, fileno(stderr));
        close(m_iPreviousStderr);
        m_iPreviousStderr = -1;
    }

    // Dispose of the created pipes
    if (m_iInputPipe > 0) {
        close(m_iInputPipe);
        m_iInputPipe = -1;
    }
    if (m_iOutputPipe > 0) {
        close(m_iOutputPipe);
        m_iOutputPipe = -1;
    }

#elif defined(__MSL__)
    if (m_bActive) {

        void** ppSource = m_pSaved;
        std::_FILE* pDest = &std::__files[1];

        uintptr_t i = 2;
        do {
            pDest->read_proc = static_cast<std::__io_proc>(ppSource[0]);
            pDest->write_proc = static_cast<std::__io_proc>(ppSource[1]);
            pDest->position_proc = static_cast<std::__pos_proc>(ppSource[2]);
#if __MSL__ >= 0x7000
            pDest->ref_con = static_cast<std::__pos_proc>(ppSource[3]);
#else
            pDest->idle_proc = static_cast<std::__idle_proc>(ppSource[3]);
#endif
            ppSource += 4;
            ++pDest;
        } while (--i);
        m_Stream.Clear();
    }
#endif

    m_uCaptureSize = 0;
    m_bActive = FALSE;
    return kErrorNone;
}

/*! ************************************

    \brief Store the contents of the pipe into a string

    Flush the pipe and store the contents of the pipe into
    a passed String class instance. After this call, the
    interception pipe is reset so it can be refilled
    with intercepted output.

    \param pOutput Pointer to a \ref String to store the data
    \return Error code
    \sa Init(uintptr_t) or Shutdown(void)

***************************************/

Burger::eError BURGER_API Burger::CaptureStdout::GetCapture(
    String* pOutput) BURGER_NOEXCEPT
{
    pOutput->clear();
    eError uError = kErrorNotReady;
    if (m_bActive) {

#if defined(USE_POSIX) || defined(BURGER_METROWERKS)
        // Flush the characters
        fflush(stdout);
        fflush(stderr);
#endif

#if defined(USE_POSIX)

        // For Posix, read from the pipe into the data stream

        // Loop until the data is all flushed
        char Buffer[1024];
        // Any data in the pipe?
#if defined(BURGER_MACOSX) || defined(BURGER_LINUX)
        for (;;)
#else
        while (!eof(m_iInputPipe))
#endif
        {
            // Read it.
            long iBytesRead = read(m_iInputPipe, Buffer, sizeof(Buffer));
            if (iBytesRead <= 0) {
                break;
            }
            // Add it to the string
            pOutput->Append(Buffer, static_cast<uint_t>(iBytesRead));
            // Not a full read?
            if (iBytesRead != sizeof(Buffer)) {
                break;
            }
        }
        uError = kErrorNone;

#elif defined(__MSL__)

        // Grab the data from the stream.
        m_Stream.Save(pOutput);
        m_Stream.Clear();
        uError = kErrorNone;
#endif
    }
    return uError;
}

/*! ************************************

    \brief Intercept output from FILE streams.

    The Metrowerks Standard Library uses a hard coded array of FILE records for
    stdin, stdout, stderr and null (In that order). Due to this, pipe is not
    supported, and hocus pocus is required to handle stream interception. This
    function replaces the write_proc callback to capture the text output.

    \param pFile Index into the __files array, 1 = stdout, 2 = stderr.
    \param pBuff Bytes to transfer.
    \param pCount Pointer to a count of bytes to transfer.
    \param pThat Pointer to the Burger::CaptureStdout instance.

    \return Error code or zero if no error.

    \note Only available with the Metrowerks compiler.

***************************************/

#if defined(__MSL__) || defined(DOXYGEN)

int BURGER_ANSIAPI Burger::CaptureStdout::WriteIntercept(
    unsigned long pFile, unsigned char* pBuff, uintptr_t* pCount, void* pThat)
{
    BURGER_UNUSED(pFile);

    CaptureStdout* pThis = static_cast<CaptureStdout*>(pThat);

    // Copy the data into the stream (Which captures the error)
    pThis->m_Stream.Append(pBuff, *pCount);

    // Never error
    return std::__no_io_error;
}

/*! ************************************

    \brief Intercept input from FILE streams.

    Return error

    \param pFile Not used.
    \param pBuff Not used.
    \param pCount Not used.
    \param pThat Not used.

    \return EOF error.

    \note Only available with the Metrowerks compiler.

***************************************/

int BURGER_ANSIAPI Burger::CaptureStdout::ReadIntercept(
    unsigned long pFile, unsigned char* pBuff, uintptr_t* pCount, void* pThat)
{
    BURGER_UNUSED(pFile);
    BURGER_UNUSED(pBuff);
    BURGER_UNUSED(pCount);
    BURGER_UNUSED(pThat);

    // Always error
    return std::__io_EOF;
}

/*! ************************************

    \brief Intercept seek from FILE streams.

    Return error

    \param pFile Not used.
    \param pPosition Not used.
    \param mode Not used.
    \param pThat Not used.

    \return I/O error.

    \note Only available with the Metrowerks compiler.

***************************************/

int BURGER_ANSIAPI Burger::CaptureStdout::SeekIntercept(
    unsigned long pFile, unsigned long* pPosition, int mode, void* pThat)
{
    BURGER_UNUSED(pFile);
    BURGER_UNUSED(pPosition);
    BURGER_UNUSED(mode);
    BURGER_UNUSED(pThat);

    // Always error

    return std::__io_error;
}

#endif
