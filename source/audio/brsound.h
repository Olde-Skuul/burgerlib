/***************************************

	Sound manager class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRENDIAN_H__
#include "brendian.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRREZFILE_H__
#include "brrezfile.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

#ifndef __BRDECOMPRESS_H__
#include "brdecompress.h"
#endif

#ifndef __BRSIMPLEARRAY_H__
#include "brsimplearray.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

#ifndef __BRARRAY_H__
#include "brarray.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

extern "C" {
	extern const GUID IID_IXAudio2_2_8;
	extern const GUID CLSID_AudioVolumeMeter_2_8;
	extern const GUID CLSID_AudioReverb_2_8;
	extern const GUID IID_IXAudio2_2_9;
}
#endif

namespace Burger {

class DecompressAudio;

class SoundManager : public Base {
    BURGER_DISABLE_COPY(SoundManager);
	BURGER_RTTI_IN_CLASS();
public:

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static const Int16 BURGER_ALIGN(g_DirectSoundVolumes[256],16);
#endif

	typedef void (BURGER_API *CompletionProc)(void *);	///< Function prototype for sound event callbacks
	static const Word cMaxVoiceCount = 32;			///< Maximum number of simultaneous tones to mix for output
	static const Word cMaxVolume = 255;				///< Maximum volume value
	static const Word cPanLeft = 0x0000;			///< Pan left value
	static const Word cPanCenter = 0x8000U;			///< Pan center value
	static const Word cPanRight = 0xFFFFU;			///< Pan right value
	static const Word cWAVHeaderSize = 0x2C;		///< Size in bytes of a WAV file header
	static const Word32 cRIFFASCII = 0x52494646U;	///< 'RIFF'
	static const Word32 cAIFFASCII = 0x41494646U;	///< 'AIFF'
	static const Word32 cAIFCASCII = 0x41494643U;	///< 'AIFC'
	static const Word32 cWAVEASCII = 0x57415645U;	///< 'WAVE'
	static const Word32 cFORMASCII = 0x464F524DU;	///< 'FORM'
	static const Word32 cDATAASCII = 0x64617461U;	///< 'data'
	static const Word32 cMAC6ASCII = 0x4D414336U;	///< 'MAC6'
	static const Word32 cMAC3ASCII = 0x4D414333U;	///< 'MAC3'
	static const Word32 cOggSASCII = 0x4F676753U;	///< 'OggS'
	static const Word32 cCOMMASCII = 0x434F4D4DU;	///< 'COMM'
	static const Word32 cSSNDASCII = 0x53534E44U;	///< 'SSND'
	static const Word32 calawASCII = 0x616C6177U;	///< 'alaw'
	static const Word32 culawASCII = 0x756C6177U;	///< 'ulaw'
	static const Word32 cfl32ASCII = 0x666C3332U;	///< 'fl32'

	enum {	
		FLAGDOUBLEBUFFER=0x4000		///< Double buffered
	};

	enum eCodecCommand {
		CODECBUFFERSIZE,	///< Size in byte for codec data
		CODECINIT,			///< Initialize a sound codec
		CODECSHUTDOWN,		///< Shut down a sound codec
		CODECDECODE,		///< Decode sound data
		CODECRESET			///< Reset a sound codec
	};

	enum eDataType {
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
		TYPEOGG,		///< OGG/Vorbis Audio
		TYPELFLOAT,		///< Little endian float
		TYPEBFLOAT,		///< Big endian float
		TYPEMASK = 0x3FFF,
		TYPESTEREO=0x8000,	///< Stereo sound
#if defined(BURGER_LITTLEENDIAN)
		TYPESHORT=TYPELSHORT,	///< 16 bit native endian
		TYPEFLOAT=TYPELFLOAT	///< 32 bit native endian
#else
		TYPESHORT=TYPEBSHORT,	///< 16 bit native endian
		TYPEFLOAT=TYPEBFLOAT	///< 32 bit native endian
#endif
	};

	class SoundCardDescription {
	public:
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		GUID m_GUID;							///< (Windows only) Device GUID
#endif
		String m_DeviceName;					///< Name of the device
		Word m_uDevNumber;						///< Device number in Burgerlib
		Word m_b8Bit;							///< Is 8 bit output available?
		Word m_b16Bit;							///< Is 16 bit output available?
		Word m_bStereo;							///< Is stereo available?
		Word m_bHardwareAccelerated;			///< Is hardware acceleration available?
		Word m_uMinimumSampleRate;				///< Minimum sample rate for audio rendering
		Word m_uMaximumSampleRate;				///< Maximum sample rate for audio rendering
		SoundCardDescription();
		~SoundCardDescription();
	};

	struct BufferDescription_t {
		const Word8 *m_pSoundImage;	///< Pointer to the data
		WordPtr m_uSoundLength;		///< Length of the data in bytes
		WordPtr m_uLoopStart;		///< Sample to start from
		WordPtr m_uLoopEnd;			///< Sample to end the loop (0 for no looping)
		Word m_uSampleRate;			///< Samples per second to play
		eDataType m_eDataType;		///< Type of data found
	};

	class BufferDecoder : public BufferDescription_t {
	protected:
		DecompressAudio *m_pDecompresser;	///< Which data conversion function is used
		WordPtr m_uCompressedLength;		///< Actual size of the compressed data
		WordPtr m_uSampleCount;				///< Number of samples in the data
	public:
		BufferDecoder();
		~BufferDecoder();
		void BURGER_API Clear(void);
		Word BURGER_API ParseSoundFileImage(const void *pInput,WordPtr uLength);
		Word BURGER_API Parse(const BufferDescription_t *pInput);
		BURGER_INLINE DecompressAudio * GetDecompresser(void) const { return m_pDecompresser; }
		BURGER_INLINE WordPtr GetCompressedSize(void) const { return m_uCompressedLength; }
		BURGER_INLINE WordPtr GetSampleCount(void) const { return m_uSampleCount; }
	};

	class Voice;

	class Buffer : public ReferenceCounter {
        BURGER_DISABLE_COPY(Buffer);
		BURGER_RTTI_IN_CLASS();
	protected:

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		IDirectSoundBuffer8 *m_pDirectSoundBuffer8;	///< DirectSound8 buffer (Windows Only)
#endif

#if !(defined(BURGER_WINDOWS)) || defined(DOXYGEN)
		const void *m_pBufferData;	///< Buffer data
		WordPtr m_uBufferSize;		///< Size of the buffer
#endif
		BufferDecoder m_Decoder;	///< Description of the sound data
		Word m_uPan;		///< Pan setting
		Word m_uVolume;		///< Volume for the buffer
		Buffer();
	public:
		virtual ~Buffer();
		Word BURGER_API Init(const void *pSoundFile,WordPtr uLength);
		Word BURGER_API Init(const BufferDescription_t *pRawData);
		virtual void Shutdown(void);
		virtual Word Upload(SoundManager *pSoundManager);
		BURGER_INLINE BufferDecoder *GetBufferDescription(void) { return &m_Decoder; }
		BURGER_INLINE const BufferDecoder *GetBufferDescription(void) const { return &m_Decoder; }
		BURGER_INLINE void SetSampleRate(Word uSampleRate) { m_Decoder.m_uSampleRate = uSampleRate; }
		BURGER_INLINE Word GetSampleRate(void) const { return m_Decoder.m_uSampleRate; }
		BURGER_INLINE void SetPan(Word uPan) { m_uPan = uPan; }
		BURGER_INLINE Word GetPan(void) const { return m_uPan; }
		BURGER_INLINE void SetVolume(Word uVolume) { m_uVolume = uVolume; }
		BURGER_INLINE Word GetVolume(void) const { return m_uVolume; }
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		BURGER_INLINE IDirectSoundBuffer8 * GetDirectSoundBuffer8(void) const { return m_pDirectSoundBuffer8; }
		BURGER_INLINE Word IsUploaded(void) const { return m_pDirectSoundBuffer8!=NULL; }
#else
		BURGER_INLINE const void *GetAudioBuffer(void) const { return m_pBufferData; }
		BURGER_INLINE WordPtr GetAudioBufferSize(void) const { return m_uBufferSize; }
		BURGER_INLINE Word IsUploaded(void) const { return m_pBufferData!=NULL; }
#endif
		static Buffer * BURGER_API New(void);
	};

	class Voice {
	protected:
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		IDirectSoundBuffer8 *m_pDirectSoundBuffer8;	///< DirectSound8 buffer (Windows Only)
		IDirectSoundNotify *m_pDirectSoundNotify;	///< DirectSoundNotify for alert when sound is complete (Windows Only)
#endif
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
		SoundManager *m_pManager;				///< Parent sound manager
		WordPtr m_uCurrentMark;					///< Mark into the waveform
	public:
#if __LP64__
		static signed int PlayCallback(void *pData,unsigned int *pActionFlags,const AudioTimeStamp *pAudioTimeStamp,unsigned int uBusNumber,unsigned int uNumberFrames,AudioBufferList *pAudioBufferList);
#else
		static signed long PlayCallback(void *pData,unsigned long *pActionFlags,const AudioTimeStamp *pAudioTimeStamp,unsigned long uBusNumber,unsigned long uNumberFrames,AudioBufferList *pAudioBufferList);
#endif
	protected:
#endif
		SmartPointer<Buffer> m_pBuffer;	///< Sound buffer connected to
		CompletionProc m_pCallBack;	///< Completion routine
		void *m_pCallBackData;		///< Data to pass for completion routine
		WordPtr	m_uLoopStart;	///< Sample to start from
		WordPtr m_uLoopEnd;		///< Sample to end the loop (0 for no looping)
		WordPtr m_uPausedMark;	///< Location of the pause
		Word m_uSampleRate;		///< Samples per second to play
		Word m_uPan;			///< Pan setting
		Word m_uVolume;			///< Volume for the buffer
		Word8 m_bPlaying;		///< \ref TRUE if the sound is currently playing
		Word8 m_bPaused;		///< \ref TRUE if the sound was paused
		Word8 m_bAvailable;		///< \ref TRUE if available for allocation
		Word8 m_bIsHeld;		///< \ref TRUE if the application is externally controlling this voice
	public:
		Voice();
		~Voice();
		Word BURGER_API Init(SoundManager *pManager,Buffer *pBuffer);
		void BURGER_API Shutdown(void);
		void BURGER_API Release(void);
		Word BURGER_API Start(void);
		Word BURGER_API Stop(void);
		Word BURGER_API Pause(void);
		Word BURGER_API Resume(void);
		Word BURGER_API SetVolume(Word uVolume);
		BURGER_INLINE Word GetVolume(void) const { return m_uVolume; }
		Word BURGER_API SetPan(Word uPan);
		BURGER_INLINE Word GetPan(void) const { return m_uPan; }
		Word BURGER_API SetSampleRate(Word uSamplesPerSecond);
		BURGER_INLINE Word GetSampleRate(void) const { return m_uSampleRate; }
		BURGER_INLINE void SetCompletionFunction(CompletionProc pCompletionProc, void *pData=NULL) { m_pCallBack = pCompletionProc; m_pCallBackData = pData; }
		BURGER_INLINE CompletionProc GetCompletionFunction(void) const { return m_pCallBack; }
		BURGER_INLINE void *GetCompletionFunctionData(void) const { return m_pCallBackData; }
		BURGER_INLINE Word IsPlaying(void) const { return m_bPlaying; }
		BURGER_INLINE Word IsPaused(void) const { return m_bPaused; }
		BURGER_INLINE Word IsAvailable(void) const { return m_bAvailable; }
		BURGER_INLINE Word IsHeld(void) const { return m_bIsHeld; }
		BURGER_INLINE void SetHeld(void) { m_bIsHeld = TRUE; }
		void BURGER_API ReachedEnd(void);
	};

protected:
	GameApp *m_pGameApp;						///< Reference to the game application

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	IXAudio2 *m_pIXAudio2;						///< XAudio2 object (Xbox 360, XBone, Windows Store)
	IXAudio2MasteringVoice *m_pIXAudio2MasteringVoice;		///< XAudio2 mastering voice buffer (Xbox 360, XBone, Windows Store)

	IDirectSound8 *m_pDirectSound8Device;		///< Primary DirectSound Device (Windows Only)
	IDirectSound3DListener *m_pDirectSound3DListener;	///< Object for 3D audio origin (Windows Only)
	IDirectSoundBuffer *m_pDirectSoundBuffer;	///< Primary DirectSoundBuffer (Windows Only)
	void *m_hCallback;							///< HANDLE of the callback thread (Windows only)
	unsigned long m_uCallbackID;				///< ID of the callback thread (Windows only)
	Word m_uBufferAllocationType;				///< Preferred method of allocating audio memory (Windows Only)
	void *m_hEvents[cMaxVoiceCount];			///< Events for DirectSound completion functions (Windows Only)
	static unsigned long __stdcall ThreadCallback(void *pThis);
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	OpaqueAUGraph *m_pGraph;			///< Main audio rendering graph
	ComponentInstanceRecord *m_pOutputUnit;		///< Audio component instance
	ComponentInstanceRecord *m_pMixerUnit;	///< Main mixer unit
	ComponentInstanceRecord *m_pSoundUnits[cMaxVoiceCount];	///< Waveform generator unit
	Int32 m_iSoundNodes[cMaxVoiceCount];	///< Waveform node
	Int32 m_iOutputNode;				///< Audio output node index
	Int32 m_iMixerNode;					///< Mixer node index
	Word32 m_uDeviceID;		///< Audio device ID
#endif

	Voice m_ActiveVoices[cMaxVoiceCount];	///< Array of active sound buffers being played
	Word m_uMaxVoices;				///< Maximum number of voices supported for mixing
	Word m_uVolume;					///< Current volume
	Word m_bStereoAvailable;		///< \ref TRUE if stereo is supported
	Word m_bMasterVolumeAvailable;	///< \ref TRUE if a master volume is supported
	Word m_uBufferDepth;			///< Bits per sample the output DAC supports (8 or 16 bit)
	Word m_uOutputSamplesPerSecond;	///< Maximum sample rate the mixer supports

public:
	SoundManager(GameApp *pGameApp);
	virtual ~SoundManager();
	static SoundManager * BURGER_API New(GameApp *pGameApp);
	Word BURGER_API Init(void);
	void BURGER_API Shutdown(void);
	Word BURGER_API Play(Buffer *pInput,Voice **ppCookie=NULL);
	Word BURGER_API Stop(void);
	Word BURGER_API Pause(void);
	Word BURGER_API Resume(void);
	void BURGER_API SetMaxVoices(Word uMax);
	BURGER_INLINE Word GetMaxVoices(void) const { return m_uMaxVoices; }
	Word BURGER_API GetActiveVoiceCount(void) const;
	BURGER_INLINE Word IsStereoSupported(void) const { return m_bStereoAvailable; }
	BURGER_INLINE Word IsVolumeAdjustSupported(void) const { return m_bMasterVolumeAvailable; }
	void BURGER_API SetVolume(Word uVolume);
	BURGER_INLINE Word GetVolume(void) const { return m_uVolume; }
	Buffer * BURGER_API NewBuffer(void *pWaveFile,WordPtr uLength);
	static Word BURGER_API GetAudioModes(ClassArray<SoundCardDescription> *pOutput);

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	BURGER_INLINE IXAudio2 *GetXAudio2(void) const { return m_pIXAudio2; }
	BURGER_INLINE IXAudio2MasteringVoice *GetXAudio2MasteringVoice(void) const { return m_pIXAudio2MasteringVoice; }
	BURGER_INLINE IDirectSound8 *GetDirectSound8(void) const { return m_pDirectSound8Device; }
	BURGER_INLINE IDirectSound3DListener * GetDirectSound3DListener(void) const { return m_pDirectSound3DListener; }
	BURGER_INLINE IDirectSoundBuffer * GetPrimaryDirectSoundBuffer(void) const { return m_pDirectSoundBuffer; }
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	BURGER_INLINE OpaqueAUGraph *GetGraph(void) const { return m_pGraph; }
	BURGER_INLINE ComponentInstanceRecord *GetAudioUnit(void) const { return m_pOutputUnit; }
	BURGER_INLINE ComponentInstanceRecord *GetMixerUnit(void) const { return m_pMixerUnit; }
	BURGER_INLINE Int32 GetMixerNode(void) const { return m_iMixerNode; }
	BURGER_INLINE Word32 GetDeviceID(void) const { return m_uDeviceID; }
#endif

protected:
	Voice * BURGER_API AllocVoice(void);
};

extern void BURGER_API CopySoundData(void *pOutput,const void *pInput,WordPtr uLength,SoundManager::eDataType eType);
extern void BURGER_API ClearSoundData(void *pOutput,WordPtr uLength,SoundManager::eDataType eType);
extern void BURGER_API CopyStereoInterleaved(void *pOutput,const Word8 *pLeft,const Word8 *pRight,WordPtr uOutputSize);
extern void BURGER_API CopyStereoInterleaved(void *pOutput,const Word16 *pLeft,const Word16 *pRight,WordPtr uOutputSize);
extern void BURGER_API ComputeReverb(Word8 *pOutput,const Word8 *pInput,WordPtr uCount,Word uPercentage);
extern void BURGER_API ComputeReverb(Int16 *pOutput,const Int16 *pInput,WordPtr uCount,Word uPercentage);
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
extern Word BURGER_API Upload(IDirectSoundBuffer *pBuffer,WordPtr uOffset,const Word8 *pInput,WordPtr uInputLength);
extern Word BURGER_API Upload(IDirectSoundBuffer *pBuffer,WordPtr uOffset,SoundManager::BufferDecoder *pBufferDecoder,WordPtr uInputLength);
#endif
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
extern float BURGER_API AudioDeviceGetMasterVolume(Word32 uDeviceID);
extern Word BURGER_API AudioDeviceSetMasterVolume(Word32 uDeviceID,float fInput);	
#endif
}
/* END */

#endif
