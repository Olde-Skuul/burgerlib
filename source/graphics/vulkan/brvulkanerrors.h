/***************************************

	Vulkan error helpers.

	Copyright (c) 2020-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRVULKANERRORS_H__
#define __BRVULKANERRORS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace Vulkan {
extern const char* BURGER_API VkResultToString(
	int32_t iVkResult) BURGER_NOEXCEPT;
extern const char* BURGER_API VkPresentModeKHRToString(
	int32_t iVkPresentModeKHR) BURGER_NOEXCEPT;
extern const char* BURGER_API VkColorSpaceKHRToString(
	int32_t iVkColorSpaceKHR) BURGER_NOEXCEPT;
extern const char* BURGER_API VkDriverIdToString(
	int32_t iVkDriverId) BURGER_NOEXCEPT;
extern const char* BURGER_API VkFormatToString(
	int32_t iVkFormat) BURGER_NOEXCEPT;
extern const char* BURGER_API VkImageTilingToString(
	int32_t iVkImageTiling) BURGER_NOEXCEPT;
extern const char* BURGER_API VkPointClippingBehaviorToString(
	int32_t iVkPointClippingBehavior) BURGER_NOEXCEPT;
extern const char* BURGER_API VkShaderFloatControlsIndependenceToString(
	int32_t iVkShaderFloatControlsIndependence) BURGER_NOEXCEPT;
extern const char* BURGER_API VkPhysicalDeviceTypeToString(
	int32_t iVkPhysicalDeviceType) BURGER_NOEXCEPT;
}}
/* END */

#endif
