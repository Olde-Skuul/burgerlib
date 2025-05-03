/***************************************

    Sound manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
struct IDirectSoundBuffer8;
struct IDirectSoundNotify;
struct IDirectSound8;
struct IDirectSound3DListener;
struct IDirectSoundBuffer;
struct IXAudio2;
struct IXAudio2MasteringVoice;

namespace Burger {

class DecompressAudio;

class SoundManager : public Base {
    BURGER_DISABLE_COPY(SoundManager);
	BURGER_RTTI_IN_CLASS();
public:

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
    BURGER_ALIGN(static const int16_t, g_DirectSoundVolumes[256], 16);
#endif

    typedef void (BURGER_API* CompletionProc)(void*);	///< Function prototype for sound event callbacks
	static const uint_t cMaxVoiceCount = 32;			///< Maximum number of simultaneous tones to mix for output
	static const uint_t cMaxVolume = 255;				///< Maximum volume value
	static const uint_t cPanLeft = 0x0000;			///< Pan left value
	static const uint_t cPanCenter = 0x8000U;			///< Pan center value
	static const uint_t cPanRight = 0xFFFFU;			///< Pan right value
	static const uint_t cWAVHeaderSize = 0x2C;		///< Size in bytes of a WAV file header
	static const uint32_t cRIFFASCII = 0x52494646U;	///< 'RIFF'
	static const uint32_t cAIFFASCII = 0x41494646U;	///< 'AIFF'
	static const uint32_t cAIFCASCII = 0x41494643U;	///< 'AIFC'
	static const uint32_t cWAVEASCII = 0x57415645U;	///< 'WAVE'
	static const uint32_t cFORMASCII = 0x464F524DU;	///< 'FORM'
	static const uint32_t cDATAASCII = 0x64617461U;	///< 'data'
	static const uint32_t cMAC6ASCII = 0x4D414336U;	///< 'MAC6'
	static const uint32_t cMAC3ASCII = 0x4D414333U;	///< 'MAC3'
	static const uint32_t cOggSASCII = 0x4F676753U;	///< 'OggS'
	static const uint32_t cCOMMASCII = 0x434F4D4DU;	///< 'COMM'
	static const uint32_t cSSNDASCII = 0x53534E44U;	///< 'SSND'
	static const uint32_t calawASCII = 0x616C6177U;	///< 'alaw'
	static const uint32_t culawASCII = 0x756C6177U;	///< 'ulaw'
	static const uint32_t cfl32ASCII = 0x666C3332U;	///< 'fl32'

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
		uint_t m_uDevNumber;						///< Device number in Burgerlib
		uint_t m_b8Bit;							///< Is 8 bit output available?
		uint_t m_b16Bit;							///< Is 16 bit output available?
		uint_t m_bStereo;							///< Is stereo available?
		uint_t m_bHardwareAccelerated;			///< Is hardware acceleration available?
		uint_t m_uMinimumSampleRate;				///< Minimum sample rate for audio rendering
		uint_t m_uMaximumSampleRate;				///< Maximum sample rate for audio rendering
		SoundCardDescription();
		~SoundCardDescription();
	};

	struct BufferDescription_t {
		const uint8_t *m_pSoundImage;	///< Pointer to the data
		uintptr_t m_uSoundLength;		///< Length of the data in bytes
		uintptr_t m_uLoopStart;		///< Sample to start from
		uintptr_t m_uLoopEnd;			///< Sample to end the loop (0 for no looping)
		uint_t m_uSampleRate;			///< Samples per second to play
		eDataType m_eDataType;		///< Type of data found
	};

	class BufferDecoder : public BufferDescription_t {
	protected:
		DecompressAudio *m_pDecompresser;	///< Which data conversion function is used
		uintptr_t m_uCompressedLength;		///< Actual size of the compressed data
		uintptr_t m_uSampleCount;				///< Number of samples in the data
	public:
		BufferDecoder();
		~BufferDecoder();
		void BURGER_API Clear(void);
		uint_t BURGER_API ParseSoundFileImage(const void *pInput,uintptr_t uLength);
		uint_t BURGER_API Parse(const BufferDescription_t *pInput);
		BURGER_INLINE DecompressAudio * GetDecompresser(void) const { return m_pDecompresser; }
		BURGER_INLINE uintptr_t GetCompressedSize(void) const { return m_uCompressedLength; }
		BURGER_INLINE uintptr_t GetSampleCount(void) const { return m_uSampleCount; }
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
		uintptr_t m_uBufferSize;		///< Size of the buffer
#endif
		BufferDecoder m_Decoder;	///< Description of the sound data
		uint_t m_uPan;		///< Pan setting
		uint_t m_uVolume;		///< Volume for the buffer
		Buffer();
	public:
		virtual ~Buffer();
		uint_t BURGER_API Init(const void *pSoundFile,uintptr_t uLength);
		uint_t BURGER_API Init(const BufferDescription_t *pRawData);
		virtual void Shutdown(void);
		virtual uint_t Upload(SoundManager *pSoundManager);
		BURGER_INLINE BufferDecoder *GetBufferDescription(void) { return &m_Decoder; }
		BURGER_INLINE const BufferDecoder *GetBufferDescription(void) const { return &m_Decoder; }
		BURGER_INLINE void SetSampleRate(uint_t uSampleRate) { m_Decoder.m_uSampleRate = uSampleRate; }
		BURGER_INLINE uint_t GetSampleRate(void) const { return m_Decoder.m_uSampleRate; }
		BURGER_INLINE void SetPan(uint_t uPan) { m_uPan = uPan; }
		BURGER_INLINE uint_t GetPan(void) const { return m_uPan; }
		BURGER_INLINE void SetVolume(uint_t uVolume) { m_uVolume = uVolume; }
		BURGER_INLINE uint_t GetVolume(void) const { return m_uVolume; }
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		BURGER_INLINE IDirectSoundBuffer8 * GetDirectSoundBuffer8(void) const { return m_pDirectSoundBuffer8; }
		BURGER_INLINE uint_t IsUploaded(void) const { return m_pDirectSoundBuffer8!=NULL; }
#else
		BURGER_INLINE const void *GetAudioBuffer(void) const { return m_pBufferData; }
		BURGER_INLINE uintptr_t GetAudioBufferSize(void) const { return m_uBufferSize; }
		BURGER_INLINE uint_t IsUploaded(void) const { return m_pBufferData!=NULL; }
#endif
		static Buffer * BURGER_API new_object(void);
	};

	class Voice {
	protected:
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		IDirectSoundBuffer8 *m_pDirectSoundBuffer8;	///< DirectSound8 buffer (Windows Only)
		IDirectSoundNotify *m_pDirectSoundNotify;	///< DirectSoundNotify for alert when sound is complete (Windows Only)
#endif
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
		SoundManager *m_pManager;				///< Parent sound manager
		uintptr_t m_uCurrentMark;					///< Mark into the waveform
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
		uintptr_t	m_uLoopStart;	///< Sample to start from
		uintptr_t m_uLoopEnd;		///< Sample to end the loop (0 for no looping)
		uintptr_t m_uPausedMark;	///< Location of the pause
		uint_t m_uSampleRate;		///< Samples per second to play
		uint_t m_uPan;			///< Pan setting
		uint_t m_uVolume;			///< Volume for the buffer
		uint8_t m_bPlaying;		///< \ref TRUE if the sound is currently playing
		uint8_t m_bPaused;		///< \ref TRUE if the sound was paused
		uint8_t m_bAvailable;		///< \ref TRUE if available for allocation
		uint8_t m_bIsHeld;		///< \ref TRUE if the application is externally controlling this voice
	public:
		Voice();
		~Voice();
		uint_t BURGER_API Init(SoundManager *pManager,Buffer *pBuffer);
		void BURGER_API Shutdown(void);
		void BURGER_API Release(void);
		uint_t BURGER_API Start(void);
		uint_t BURGER_API Stop(void);
		uint_t BURGER_API Pause(void);
		uint_t BURGER_API Resume(void);
		uint_t BURGER_API SetVolume(uint_t uVolume);
		BURGER_INLINE uint_t GetVolume(void) const { return m_uVolume; }
		uint_t BURGER_API SetPan(uint_t uPan);
		BURGER_INLINE uint_t GetPan(void) const { return m_uPan; }
		uint_t BURGER_API SetSampleRate(uint_t uSamplesPerSecond);
		BURGER_INLINE uint_t GetSampleRate(void) const { return m_uSampleRate; }
		BURGER_INLINE void SetCompletionFunction(CompletionProc pCompletionProc, void *pData=NULL) { m_pCallBack = pCompletionProc; m_pCallBackData = pData; }
		BURGER_INLINE CompletionProc GetCompletionFunction(void) const { return m_pCallBack; }
		BURGER_INLINE void *GetCompletionFunctionData(void) const { return m_pCallBackData; }
		BURGER_INLINE uint_t IsPlaying(void) const { return m_bPlaying; }
		BURGER_INLINE uint_t IsPaused(void) const { return m_bPaused; }
		BURGER_INLINE uint_t IsAvailable(void) const { return m_bAvailable; }
		BURGER_INLINE uint_t IsHeld(void) const { return m_bIsHeld; }
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
	uint_t m_uBufferAllocationType;				///< Preferred method of allocating audio memory (Windows Only)
	void *m_hEvents[cMaxVoiceCount];			///< Events for DirectSound completion functions (Windows Only)
	static unsigned long __stdcall ThreadCallback(void *pThis);
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	OpaqueAUGraph *m_pGraph;			///< Main audio rendering graph
	ComponentInstanceRecord *m_pOutputUnit;		///< Audio component instance
	ComponentInstanceRecord *m_pMixerUnit;	///< Main mixer unit
	ComponentInstanceRecord *m_pSoundUnits[cMaxVoiceCount];	///< Waveform generator unit
	int32_t m_iSoundNodes[cMaxVoiceCount];	///< Waveform node
	int32_t m_iOutputNode;				///< Audio output node index
	int32_t m_iMixerNode;					///< Mixer node index
	uint32_t m_uDeviceID;		///< Audio device ID
#endif

	Voice m_ActiveVoices[cMaxVoiceCount];	///< Array of active sound buffers being played
	uint_t m_uMaxVoices;				///< Maximum number of voices supported for mixing
	uint_t m_uVolume;					///< Current volume
	uint_t m_bStereoAvailable;		///< \ref TRUE if stereo is supported
	uint_t m_bMasterVolumeAvailable;	///< \ref TRUE if a master volume is supported
	uint_t m_uBufferDepth;			///< Bits per sample the output DAC supports (8 or 16 bit)
	uint_t m_uOutputSamplesPerSecond;	///< Maximum sample rate the mixer supports

public:
	SoundManager(GameApp *pGameApp);
	virtual ~SoundManager();
	static SoundManager * BURGER_API new_object(GameApp *pGameApp);
	uint_t BURGER_API Init(void);
	void BURGER_API Shutdown(void);
	uint_t BURGER_API Play(Buffer *pInput,Voice **ppCookie=NULL);
	uint_t BURGER_API Stop(void);
	uint_t BURGER_API Pause(void);
	uint_t BURGER_API Resume(void);
	void BURGER_API SetMaxVoices(uint_t uMax);
	BURGER_INLINE uint_t GetMaxVoices(void) const { return m_uMaxVoices; }
	uint_t BURGER_API GetActiveVoiceCount(void) const;
	BURGER_INLINE uint_t IsStereoSupported(void) const { return m_bStereoAvailable; }
	BURGER_INLINE uint_t IsVolumeAdjustSupported(void) const { return m_bMasterVolumeAvailable; }
	void BURGER_API SetVolume(uint_t uVolume);
	BURGER_INLINE uint_t GetVolume(void) const { return m_uVolume; }
	Buffer * BURGER_API NewBuffer(void *pWaveFile,uintptr_t uLength);
	static uint_t BURGER_API GetAudioModes(ClassArray<SoundCardDescription> *pOutput);

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
	BURGER_INLINE int32_t GetMixerNode(void) const { return m_iMixerNode; }
	BURGER_INLINE uint32_t GetDeviceID(void) const { return m_uDeviceID; }
#endif

protected:
	Voice * BURGER_API AllocVoice(void);
};

extern void BURGER_API CopySoundData(void *pOutput,const void *pInput,uintptr_t uLength,SoundManager::eDataType eType);
extern void BURGER_API ClearSoundData(void *pOutput,uintptr_t uLength,SoundManager::eDataType eType);
extern void BURGER_API CopyStereoInterleaved(void *pOutput,const uint8_t *pLeft,const uint8_t *pRight,uintptr_t uOutputSize);
extern void BURGER_API CopyStereoInterleaved(void *pOutput,const uint16_t *pLeft,const uint16_t *pRight,uintptr_t uOutputSize);
extern void BURGER_API ComputeReverb(uint8_t *pOutput,const uint8_t *pInput,uintptr_t uCount,uint_t uPercentage);
extern void BURGER_API ComputeReverb(int16_t *pOutput,const int16_t *pInput,uintptr_t uCount,uint_t uPercentage);
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
extern uint_t BURGER_API Upload(IDirectSoundBuffer *pBuffer,uintptr_t uOffset,const uint8_t *pInput,uintptr_t uInputLength);
extern uint_t BURGER_API Upload(IDirectSoundBuffer *pBuffer,uintptr_t uOffset,SoundManager::BufferDecoder *pBufferDecoder,uintptr_t uInputLength);
#endif
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
extern float BURGER_API AudioDeviceGetMasterVolume(uint32_t uDeviceID);
extern uint_t BURGER_API AudioDeviceSetMasterVolume(uint32_t uDeviceID,float fInput);	
#endif
}
/* END */

#endif
