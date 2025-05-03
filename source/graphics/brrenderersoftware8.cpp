/***************************************

    8 bit software manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brrenderersoftware8.h"
#include "brmemoryfunctions.h"

/*! ************************************

    \class Burger::RendererSoftware8
    \brief 8 bit paletted software renderer

    This class contains functions to draw pixels in an 8 bit
    paletted buffer. It is 100% software driven, no hardware assist is
    performed at all

    \sa Renderer, RendererOpenGL and RendererSoftware16

***************************************/

/*! ************************************

    \brief Default constructor.

    Initializes all of the shared variables

***************************************/

Burger::RendererSoftware8::RendererSoftware8(void) BURGER_NOEXCEPT: Renderer()
{
}

/***************************************

    Draw 8 bit data as is

***************************************/

void Burger::RendererSoftware8::Draw8BitPixels(int iX, int iY, uint_t uWidth,
    uint_t uHeight, uintptr_t uStride, const uint8_t* pPixels) BURGER_NOEXCEPT
{
    // Clip the right side
    int iDelta = m_Clip.m_iRight - iX;
    if (iDelta >= 1) {
        if (static_cast<uint_t>(iDelta) < uWidth) {
            uWidth = static_cast<uint_t>(iDelta);
        }
        // Clip the bottom
        iDelta = m_Clip.m_iBottom - iY;
        if (iDelta >= 1) {
            if (static_cast<uint_t>(iDelta) < uHeight) {
                uHeight = static_cast<uint_t>(iDelta);
            }
            // Clip the top

            iDelta = iY - static_cast<int>(m_Clip.m_iTop - uHeight);
            if (iDelta >= 1) {
                if (static_cast<uint_t>(iDelta) < uHeight) {
                    iY = m_Clip.m_iTop;
                    pPixels = pPixels +
                        (uStride * static_cast<uint_t>(uHeight - iDelta));
                    uHeight = static_cast<uint_t>(iDelta);
                }

                // Clip the left side

                iDelta = iX - static_cast<int>(m_Clip.m_iLeft - uWidth);
                if (iDelta >= 1) {
                    if (static_cast<uint_t>(iDelta) < uWidth) {
                        iX = m_Clip.m_iLeft;
                        pPixels = pPixels + static_cast<uint_t>(uWidth - iDelta);
                        uWidth = static_cast<uint_t>(iDelta);
                    }

                    // Invalid shape?
                    if (uHeight && uWidth) {

                        // Get base address
                        uint8_t* pDest = &static_cast<uint8_t*>(
                            m_pFrameBuffer)[(m_uStride * iY) + iX];
                        // Precalc the offset
                        uintptr_t uOffset = m_uStride;
                        // Count in longs
                        uint_t uLongWidth = uWidth >> 2;
                        if (!uLongWidth) {
                            // Blit 1,2 or 3 byte wide shapes

                            switch (uWidth) {
                            // Copy a single byte
                            default:
                                do {
                                    pDest[0] = pPixels[0];
                                    pDest = pDest + uOffset;
                                    pPixels = pPixels + uStride;
                                } while (--uHeight);
                                break;
                            // Copy 2 bytes
                            case 2:
                                do {
#if defined(BURGER_INTEL)
                                    reinterpret_cast<uint16_t*>(pDest)[0] =
                                        reinterpret_cast<const uint16_t*>(
                                            pPixels)[0];
#else
                                    uint8_t a = pPixels[0];
                                    uint8_t b = pPixels[1];
                                    pDest[0] = a;
                                    pDest[1] = b;
#endif
                                    pDest = pDest + uOffset;
                                    pPixels = pPixels + uStride;
                                } while (--uHeight);
                                break;
                            // Copy 3 bytes
                            case 3:
                                do {
#if (defined(BURGER_AMD64) || defined(BURGER_X86))
                                    reinterpret_cast<uint16_t*>(pDest)[0] =
                                        reinterpret_cast<const uint16_t*>(
                                            pPixels)[0];
#else
                                    uint8_t a = pPixels[0];
                                    uint8_t b = pPixels[1];
                                    pDest[0] = a;
                                    pDest[1] = b;
#endif
                                    pDest[2] = pPixels[2];
                                    pDest = pDest + uOffset;
                                    pPixels = pPixels + uStride;
                                } while (--uHeight);
                            }
                        } else {
                            // Get count of byte fragments to copy
                            uStride -= uWidth;
                            uOffset -= uWidth;
                            uWidth &= 3;
                            if (uWidth) {
                                uint_t i;
                                // Copy longwords and bytes
                                do {
                                    i = uLongWidth;
                                    do {
                                        uint32_t uTemp =
                                            reinterpret_cast<const uint32_t*>(
                                                pPixels)[0];
                                        reinterpret_cast<uint32_t*>(pDest)[0] =
                                            uTemp;
                                        pDest = pDest + 4;
                                        pPixels = pPixels + 4;
                                    } while (--i);
                                    i = uWidth;
                                    do {
                                        pDest[0] = pPixels[0];
                                        pDest = pDest + 1;
                                        pPixels = pPixels + 1;
                                    } while (--i);
                                    pDest += uOffset;
                                    pPixels = pPixels + uStride;
                                } while (--uHeight);
                            } else {
                                // I only copy 4 byte data!
                                do {
                                    uint_t i = uLongWidth;
                                    do {
                                        uint32_t uTemp =
                                            reinterpret_cast<const uint32_t*>(
                                                pPixels)[0];
                                        reinterpret_cast<uint32_t*>(pDest)[0] =
                                            uTemp;
                                        pDest = pDest + 4;
                                        pPixels = pPixels + 4;
                                    } while (--i);
                                    pDest = pDest + uOffset;
                                    pPixels = pPixels + uStride;
                                } while (--uHeight);
                            }
                        }
                    }
                }
            }
        }
    }
}

/***************************************

    Draw 8 bit data without drawing the zero values

***************************************/

void Burger::RendererSoftware8::Draw8BitPixelsMasked(int iX, int iY,
    uint_t uWidth, uint_t uHeight, uintptr_t uStride,
    const uint8_t* pPixels) BURGER_NOEXCEPT
{
    // Clip the right side
    int iDelta = m_Clip.m_iRight - iX;
    if (iDelta >= 1) {
        if (static_cast<uint_t>(iDelta) < uWidth) {
            uWidth = static_cast<uint_t>(iDelta);
        }
        // Clip the bottom
        iDelta = m_Clip.m_iBottom - iY;
        if (iDelta >= 1) {
            if (static_cast<uint_t>(iDelta) < uHeight) {
                uHeight = static_cast<uint_t>(iDelta);
            }
            // Clip the top

            iDelta = iY - static_cast<int>(m_Clip.m_iTop - uHeight);
            if (iDelta >= 1) {
                if (static_cast<uint_t>(iDelta) < uHeight) {
                    iY = m_Clip.m_iTop;
                    pPixels = pPixels +
                        (uStride * static_cast<uint_t>(uHeight - iDelta));
                    uHeight = static_cast<uint_t>(iDelta);
                }

                // Clip the left side

                iDelta = iX - static_cast<int>(m_Clip.m_iLeft - uWidth);
                if (iDelta >= 1) {
                    if (static_cast<uint_t>(iDelta) < uWidth) {
                        iX = m_Clip.m_iLeft;
                        pPixels = pPixels + static_cast<uint_t>(uWidth - iDelta);
                        uWidth = static_cast<uint_t>(iDelta);
                    }
                    if (uWidth && uHeight) {
                        // Get base address
                        uint8_t* pDest = &static_cast<uint8_t*>(
                            m_pFrameBuffer)[(m_uStride * iY) + iX];
                        // Precalc the pDest stride
                        uintptr_t uDestStride = m_uStride - uWidth;
                        uStride -= uWidth;
                        uint_t uLongWidth = uWidth >> 2;
                        if (!uLongWidth) {
                            // 1-3 bytes wide
                            do {
                                // Reset the width count
                                uint_t i = uWidth;
                                do {
                                    // Only draw non zero pixels!
                                    uint8_t uTemp1 = pPixels[0];
                                    if (uTemp1) {
                                        pDest[0] = uTemp1;
                                    }
                                    pDest = pDest + 1;
                                    pPixels = pPixels + 1;
                                } while (--i);
                                // Adjust the dest pointer
                                pDest = pDest + uDestStride;
                                pPixels = pPixels + uStride;
                                // Totally done?
                            } while (--uHeight);
                        } else {
                            uWidth = uWidth & 3;
                            do {
                                // Reset the width count
                                uint_t i = uLongWidth;
                                do {
                                    uint8_t uTempA = pPixels[0];
                                    uint8_t uTempB = pPixels[1];
                                    if (uTempA) {
                                        pDest[0] = uTempA;
                                    }
                                    if (uTempB) {
                                        pDest[1] = uTempB;
                                    }
                                    uTempA = pPixels[2];
                                    uTempB = pPixels[3];
                                    if (uTempA) {
                                        pDest[2] = uTempA;
                                    }
                                    if (uTempB) {
                                        pDest[3] = uTempB;
                                    }
                                    pDest = pDest + 4;
                                    pPixels = pPixels + 4;
                                } while (--i); /* All done? */
                                if (uWidth) {
                                    i = uWidth;
                                    do {
                                        uint8_t uTempC = pPixels[0];
                                        if (uTempC) {
                                            pDest[0] = uTempC;
                                        }
                                        pDest = pDest + 1;
                                        pPixels = pPixels + 1;
                                        // Line done?
                                    } while (--i);
                                }
                                // Adjust the dest pointer
                                pDest = pDest + uDestStride;
                                pPixels = pPixels + uStride;
                                // Totally done?
                            } while (--uHeight);
                        }
                    }
                }
            }
        }
    }
}

/***************************************

    Draw a single 8 bit pixel

***************************************/

void Burger::RendererSoftware8::DrawPixel(
    int iX, int iY, uint_t uColorIndex) BURGER_NOEXCEPT
{
    if ((iX >= m_Clip.m_iLeft) && (iY >= m_Clip.m_iTop) &&
        (iX < m_Clip.m_iRight) && (iY < m_Clip.m_iBottom)) { // Plot the pixel
        static_cast<uint8_t*>(m_pFrameBuffer)[(m_uStride * iY) + iX] =
            static_cast<uint8_t>(uColorIndex);
    }
}

/***************************************

    Draw 8 bit color rectangle

***************************************/

void Burger::RendererSoftware8::DrawRect(
    int iX, int iY, uint_t uWidth, uint_t uHeight, uint_t uColorIndex) BURGER_NOEXCEPT
{
    // Clip the right side
    int iDelta = m_Clip.m_iRight - iX;
    if (iDelta >= 1) {
        if (static_cast<uint_t>(iDelta) < uWidth) {
            uWidth = static_cast<uint_t>(iDelta);
        }
        // Clip the bottom
        iDelta = m_Clip.m_iBottom - iY;
        if (iDelta >= 1) {
            if (static_cast<uint_t>(iDelta) < uHeight) {
                uHeight = static_cast<uint_t>(iDelta);
            }
            // Clip the top

            iDelta = iY - static_cast<int>(m_Clip.m_iTop - uHeight);
            if (iDelta >= 1) {
                if (static_cast<uint_t>(iDelta) < uHeight) {
                    iY = m_Clip.m_iTop;
                    uHeight = static_cast<uint_t>(iDelta);
                }

                // Clip the left side

                iDelta = iX - static_cast<int>(m_Clip.m_iLeft - uWidth);
                if (iDelta >= 1) {
                    if (static_cast<uint_t>(iDelta) < uWidth) {
                        iX = m_Clip.m_iLeft;
                        uWidth = static_cast<uint_t>(iDelta);
                    }
                    if (uWidth && uHeight) {
                        // Get the screen pointer
                        uint8_t* pDest = &static_cast<uint8_t*>(
                            m_pFrameBuffer)[(m_uStride * iY) + iX];
                        // Optimization for memory fill
                        if ((m_uStride == uWidth) && !iX) {
                            memory_set(pDest, static_cast<uint8_t>(uColorIndex),
                                uWidth * uHeight);
                        } else {
                            do {
                                // Fill memory
                                memory_set(pDest,
                                    static_cast<uint8_t>(uColorIndex), uWidth);
                                // Next line down
                                pDest += m_uStride;
                                // All done?
                            } while (--uHeight);
                        }
                    }
                }
            }
        }
    }
}
