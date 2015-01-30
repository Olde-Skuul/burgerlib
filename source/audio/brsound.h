/***************************************

	Sound manager class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSOUND_H__
#define __BRSOUND_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRREZFILE_H__
#include "brrezfile.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
#if !defined(DOXYGEN)
#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct WAVEFORMATEX {
	Word16 wFormatTag;		///< format type
	Word16 nChannels;		///< number of channels (i.e. mono, stereo...)
	Word32 nSamplesPerSec;	///< sample rate
	Word32 nAvgBytesPerSec;	///< for buffer estimation
	Word16 nBlockAlign;		///< block size of data
	Word16 wBitsPerSample;	///< number of bits per sample of mono data
	Word16 cbSize;			///< the count in bytes of the size of extra information (after cbSize)
} WAVEFORMATEX,*PWAVEFORMATEX,*NPWAVEFORMATEX, *LPWAVEFORMATEX;
#endif
#endif

namespace Burger {
class Sound {
	BURGER_DISABLECOPYCONSTRUCTORS(Sound);
public:
	typedef void (BURGER_API *CompletionProc)(void *);
	enum {
		MAXVOICECOUNT = 32,			///< Maximum number of simultainous tones to mix for output
		FLAGDOUBLEBUFFER=0x4000,	///< Double buffered
		FLAGSTEREO=0x8000			///< Stereo sound
	};
	enum CodecCommand {
		CODECINIT,		///< Initialize a sound codec
		CODECSHUTDOWN,	///< Shut down a sound codec
		CODECDECODE,	///< Decode sound data
		CODECRESET		///< Reset a sound codec
	};
	enum DataType {
		TYPEBYTE,		///< Unsigned 8 bit data
		TYPECHAR,		///< Signed 8 bit data
		TYPELSHORT,		///< Little endian short
		TYPEBSHORT,		///< Big endian short
		TYPEADPCM,		///< MS ADPCM compression
		TYPEDVIPCM,		///< Intel DVI ADPCM compression
		TYPEMP3,		///< MP3 Audio
		TYPEULAW,		///< MuLaw
		TYPEALAW,		///< ALaw
		TYPEMACE3,		///< Mace 3:1
		TYPEMACE6,		///< Mace 6:1
		TYPEOGG			///< OGG/Vorbis Audio
	};

	class Voice;
	class Buffer {
		BURGER_DISABLECOPYCONSTRUCTORS(Buffer);
	public:
		Buffer();
		Buffer(const void *pBuffer,WordPtr uLength,DataType eType);
		virtual ~Buffer();
		void Shutdown(void);
		//virtual void FillBuffer(void);
		BURGER_INLINE void SetDefaultVolume(Word uVolume) { m_uDefaultVolume = uVolume; }
		BURGER_INLINE Word GetDefaultVolume(void) const { return m_uDefaultVolume; }
		BURGER_INLINE void SetDefaultPan(Word uPan) { m_uDefaultPan = uPan; }
		BURGER_INLINE Word GetDefaultPan(void) const { return m_uDefaultPan; }
		BURGER_INLINE void SetDefaultSampleRate(Word uSampleRate) { m_uDefaultSampleRate = uSampleRate; }
		BURGER_INLINE Word GetSampleRate(void) const { return m_uDefaultSampleRate; }
		BURGER_INLINE void SetCompletionFunction(CompletionProc pCompletionProc, void *pData=NULL) { m_CompletionProc = pCompletionProc; m_pCompletionData = pData; }
		BURGER_INLINE CompletionProc GetCompletionFunction(void) const { return m_CompletionProc; }
		BURGER_INLINE void *GetCompletionFunctionData(void) const { return m_pCompletionData; }
		BURGER_INLINE Word IsPlaying(void) const { return m_bPlaying; }
	protected:
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		IDirectSoundBuffer *m_pDirectSoundBuffer8;	///< DirectSound buffer (Windows Only)
		volatile Word32 m_bTaken;	///< \ref FALSE if a voice is using the m_pDirectSoundBuffer8
		Voice *m_pOwnerVoice;		///< Voice object that is borrowing the m_pDirectSoundBuffer8
#endif
		Word8 *m_pBuffer;		///< Pointer to the data
		CompletionProc m_CompletionProc;	///< Completion routine
		void *m_pCompletionData;			///< Data to pass for completion routine
		WordPtr m_uBufferSize;	///< Length of the data in bytes
		WordPtr	m_uLoopStart;	///< Sample to start from
		WordPtr m_uLoopEnd;		///< Sample to end the loop (0 for no looping)
		DataType m_eDataType;	///< Type of data found
		Word m_uDefaultSampleRate;	///< Samples per second to play
		Word m_uDefaultPan;			///< Pan setting
		Word m_uDefaultVolume;		///< Volume for the buffer
		Word m_bPlaying;			///< \ref TRUE if the sound is currently playing
	};


	class Voice {
	public:
		Voice();
		~Voice();
		void Shutdown(void);
		Word SetVolume(Word uVolume);
		BURGER_INLINE Word GetVolume(void) const { return m_uVolume; }
		Word SetPan(Word uPan);
		BURGER_INLINE Word GetPan(void) const { return m_uPan; }
		Word SetSampleRate(Word uSamplesPerSecond);
		BURGER_INLINE Word GetSampleRate(void) const { return m_uSampleRate; }
		BURGER_INLINE Word IsPlaying(void) const { return m_bPlaying; }
	private:
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		IDirectSoundBuffer *m_pDirectSoundBuffer8;	///< DirectSound buffer (Windows Only)
#endif
		Buffer *m_pBuffer;		///< Sound buffer to play
		WordPtr	m_uLoopStart;	///< Sample to start from
		WordPtr m_uLoopEnd;		///< Sample to end the loop (0 for no looping)
		Word m_uSampleRate;		///< Samples per second to play
		Word m_uPan;			///< Pan setting
		Word m_uVolume;			///< Volume for the buffer
		Word m_bPlaying;		///< \ref TRUE if the sound is currently playing
	};
	class Cookie {
		BURGER_DISABLECOPYCONSTRUCTORS(Cookie);
	public:
		Cookie() : m_pVoice(NULL) {}
	private:
		Voice *m_pVoice;		///< Pointer to the active voice
	};

	Sound(GameApp *pGameApp);
	~Sound();
	Word Init(void);
	void Shutdown(void);
	Word Play(Buffer *pInput,Voice *pCookie=NULL);
	Word Stop(void);
	Word Pause(void);
	Word Resume(void);
	void SetMaxVoices(Word uMax);
	BURGER_INLINE Word GetMaxVoices(void) const { return m_uMaxVoices; }
	BURGER_INLINE Word GetActiveVoiceCount(void) const { return m_uActiveVoices; }
	void SetVolume(Word uVolume);
	BURGER_INLINE Word GetVolume(void) const { return m_uVolume; }
private:
	static RunQueue::eReturnCode BURGER_API Callback(void *);

	GameApp *m_pGameApp;						///< Reference to the game application
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static unsigned long __stdcall ThreadCallback(void *pThis);
public:
	static const Int16 BURGER_ALIGN(g_DirectSoundVolumes[256],16);
private:
	IDirectSound8 *m_pDirectSound8Device;		///< Primary DirectSound Device (Windows Only)
	IDirectSound3DListener *m_pDirectSound3DListener;	///< Object for 3D audio origin (Windows Only)
	void *m_hCallback;						///< HANDLE of the callback thread (Windows only)
	unsigned long m_uCallbackID;					///< ID of the callback thread (Windows only)
	Word m_uDeviceFlags;					///< DSCAPS for DirectSound Device (Windows Only)
	Word m_uBufferAllocationType;			///< Preferred method of allocating audio memory (Windows Only)
	void *m_hEvents[MAXVOICECOUNT];			///< Events for DirectSound completion functions (Windows Only)
#endif
	Voice m_ActiveVoices[MAXVOICECOUNT];	///< Array of active sound buffers being played
	Word m_uActiveVoices;			///< Number of active voices
	Word m_uMaxVoices;				///< Maximum number of voices supported for mixing
	Word m_uVolume;					///< Current volume
	Word m_bStereoAvailable;		///< \ref TRUE if stereo is supported
	Word m_bBufferDepth;			///< Bits per sample the output DAC supports (8 or 16 bit)
	Word m_uOutputSamplesPerSecond;	///< Maximum sample rate the mixer supports
};
class SoundBufferRez : public Sound::Buffer {
public:
	SoundBufferRez(void);
	SoundBufferRez(RezFile *pRezFile,Word uRezNum);
	~SoundBufferRez();
	void Shutdown();
	void Set(RezFile *pRezFile,Word uRezNum);
	void Set(Word uRezNum);
private:
	RezFile *m_pRezFile;
	void **m_ppHandle;
	Word m_uRezNum;
};
}
/* END */

#endif
