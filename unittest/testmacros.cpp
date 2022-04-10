/***************************************

	Determine which compiler is being used and create standardized typedefs and
	macros so generic code can be created cross platform

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testmacros.h"
#include "bratomic.h"
#include "brfloatingpoint.h"
#include "brglobals.h"
#include "brnumberstring.h"
#include "brstring.h"
#include "brstringfunctions.h"
#include "common.h"
#include <stdio.h>

#if defined(BURGER_STRUCT_ALIGN)
#pragma options align = native
#elif defined(BURGER_STRUCT_PACKPUSH)
#pragma pack(push, 8)
#elif defined(BURGER_STRUCT_PACK)
#pragma pack(8)
#endif

#if defined(BURGER_STRUCT_ALIGN)
#pragma options align = reset
#elif defined(BURGER_STRUCT_PACKPUSH)
#pragma pack(pop)
#elif defined(BURGER_STRUCT_PACK)
#pragma pack()
#endif

#if defined(BURGER_MSVC)
// Disable not enough actual parameters for macro
#pragma warning(disable : 4003)
// Disable structure was padded due to __declspec(align())
#pragma warning(disable : 4324)
#endif

/***************************************

	SHOW_DEFAULT_SIGN will pass an intrinsic data type to the template function
	TestDefaultSign() that instanciates a temporary variable of that data type.
	The variable is tested if it is signed or unsigned and the result of the
	test is printed out.

***************************************/

#define SHOW_DEFAULT_SIGN(X) TestDefaultSign<X>(#X)

template<class T>
static void BURGER_API TestDefaultSign(const char* pTypeName) BURGER_NOEXCEPT
{
	// Set to maximum value, which will become negative if signed
	T uTest = static_cast<T>(-1);
	Burger::String msg(
		pTypeName, " defaults to ", uTest > 0 ? "unsigned" : "signed");
	Message(msg.c_str());
}

/***************************************

	SHOW_TYPE_SIZE will pass an intrinsic data type to the function
	TestTypeSize() that will print out the sizeof() of the type and the type's
	name.

***************************************/

#define SHOW_TYPE_SIZE(X) TestTypeSize(#X, sizeof(X))

static void BURGER_API TestTypeSize(
	const char* pTypeName, uintptr_t uSize) BURGER_NOEXCEPT
{
	const Burger::NumberString str(uSize);
	Burger::String msg(pTypeName, " is ", str.c_str(), " bytes in size");
	Message(msg.c_str());
}

/***************************************

	SHOW_MACRO will pass the macro name as is as the first parameter and the
	macro value prepended with an equals sign to force a string, even when the
	macro is defined with no value.

***************************************/

// clang-format off
// Macro to call ShowMacro for every macro
#define SHOW_MACRO(X) ShowMacro(#X, BURGER_STRINGIZE(=X))
// clang-format on

static void BURGER_API ShowMacro(
	const char* pMacroName, const char* pMacroData) BURGER_NOEXCEPT
{
	Burger::String msg;

	// If the strings match, then the string doesn't exist because
	// BURGER_STRINGIZE treated the macro name as a literal string instead of a
	// value.

	if (Burger::StringCompare(pMacroName, pMacroData + 1)) {
		// Test if the string is "=" which is no data
		if (pMacroData[1]) {
			msg = "XX ";
			msg += pMacroName;
			msg += pMacroData;
		} else {
			msg = "X  ";
			msg += pMacroName;
			msg += " [no data]";
		}
	} else {
		msg = "   ";
		msg += pMacroName;
	}
	Message(msg.c_str());
}

/***************************************

	Print the default sign of integer values

***************************************/

static void BURGER_API ShowDefaultSigns(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MACROS) {
		Message("\nInteger value settings");

		SHOW_DEFAULT_SIGN(char);

#if defined(BURGER_HAS_CHAR8_T)
		SHOW_DEFAULT_SIGN(char8_t);
#endif

#if defined(BURGER_HAS_WCHAR_T)
		SHOW_DEFAULT_SIGN(wchar_t);
#endif

#if defined(BURGER_HAS_CHAR16_T)
		SHOW_DEFAULT_SIGN(char16_t);
		SHOW_DEFAULT_SIGN(char32_t);
#endif
	}
}

/***************************************

	Print the sizes of integer values

***************************************/

static void BURGER_API ShowIntrinsicSizes(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MACROS) {
		Message("\nData type sizes");

		SHOW_TYPE_SIZE(char);

#if defined(BURGER_HAS_CHAR8_T)
		SHOW_TYPE_SIZE(char8_t);
#endif

#if defined(BURGER_HAS_WCHAR_T)
		SHOW_TYPE_SIZE(wchar_t);
#endif

#if defined(BURGER_HAS_CHAR16_T)
		SHOW_TYPE_SIZE(char16_t);
		SHOW_TYPE_SIZE(char32_t);
#endif

		SHOW_TYPE_SIZE(short);
		SHOW_TYPE_SIZE(int);
		SHOW_TYPE_SIZE(long);
		SHOW_TYPE_SIZE(long long);
		SHOW_TYPE_SIZE(void*);
		SHOW_TYPE_SIZE(void (*)(void));
		SHOW_TYPE_SIZE(float);
		SHOW_TYPE_SIZE(double);
		SHOW_TYPE_SIZE(long double);
	}
}

/***************************************

	Print the contents of these macros so they can be manually checked for
	correctness

***************************************/

static void BURGER_API ShowCompilerMacros(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MACROS) {

		Message("\nKnown compiler macros found");

		// Print known C++ compiler flags
		SHOW_MACRO(__STDC__);
		SHOW_MACRO(__STDC_VERSION__);
		SHOW_MACRO(__STDC_IEC_559__);
		SHOW_MACRO(__STDC_IEC_559_COMPLEX__);
		SHOW_MACRO(__STDC_ISO_10646__);
		SHOW_MACRO(__cplusplus);
		SHOW_MACRO(__cplusplus_cli);
		SHOW_MACRO(__embedded_cplusplus);
		SHOW_MACRO(__OBJC__);
		SHOW_MACRO(__COUNTER__);
		SHOW_MACRO(__LINE__);

		// Unix
		SHOW_MACRO(unix);
		SHOW_MACRO(__unix__);
		SHOW_MACRO(__unix);

		// Posix
		SHOW_MACRO(_POSIX_VERSION);
		SHOW_MACRO(_POSIX2_C_VERSION);

		// X/Open
		SHOW_MACRO(_XOPEN_VERSION);
		SHOW_MACRO(_XOPEN_UNIX);

		// Linux Standards base
		SHOW_MACRO(__LSB_VERSION__);

		// Data models
		SHOW_MACRO(_ILP32);
		SHOW_MACRO(__ILP32__);
		SHOW_MACRO(_LP64);
		SHOW_MACRO(__LP64__);
		SHOW_MACRO(__CHAR_SIGNED__);
		SHOW_MACRO(__INLINE_FUNCTIONS__);
		SHOW_MACRO(__FLAT__);

		// Operating system macros

		// IBM AIX
		SHOW_MACRO(_AIX);
		SHOW_MACRO(__TOS_AIX__);

		// Android
		SHOW_MACRO(__ANDROID__);
		SHOW_MACRO(__ANDROID_API__);

		// Commodore Amiga
		SHOW_MACRO(AMIGA);
		SHOW_MACRO(__amigaos__);

		// BeOS
		SHOW_MACRO(__BEOS__);
		SHOW_MACRO(__be_os);

		// Berkeley Software Distribution (BSD)
		SHOW_MACRO(BSD);
		SHOW_MACRO(__bsdi__);
		SHOW_MACRO(__DragonFly__);
		SHOW_MACRO(__FreeBSD__);
		SHOW_MACRO(__FreeBSD_kernel__);
		SHOW_MACRO(__NetBSD__);
		SHOW_MACRO(__OpenBSD__);
		SHOW_MACRO(_SYSTYPE_BSD);

		// Cygwin
		SHOW_MACRO(__CYGWIN__);

		// Linux
		SHOW_MACRO(linux);
		SHOW_MACRO(__linux);
		SHOW_MACRO(__linux__);
		SHOW_MACRO(__gnu_linux__);

		// Macintosh
		SHOW_MACRO(macintosh);
		SHOW_MACRO(Macintosh);
		SHOW_MACRO(__APPLE__);
		SHOW_MACRO(__MACH__);
		SHOW_MACRO(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__);
		SHOW_MACRO(__CFM68K);
		SHOW_MACRO(__CFM68K__);

		// Morph OS
		SHOW_MACRO(__MORPHOS__);

		// MSDOS
		SHOW_MACRO(MSDOS);
		SHOW_MACRO(__DOS__);
		SHOW_MACRO(__MSDOS__);
		SHOW_MACRO(_MSDOS);
		SHOW_MACRO(__DOS4G__);
		SHOW_MACRO(__X32__);

		// OS/2
		SHOW_MACRO(__OS2__);

		// QNX
		SHOW_MACRO(__QNX__);

		// Novell Netware
		SHOW_MACRO(__NETWARE_386__);

		// Solaris
		SHOW_MACRO(sun);
		SHOW_MACRO(__sun);

		// Windows
		SHOW_MACRO(_WIN16);
		SHOW_MACRO(_WIN32);
		SHOW_MACRO(_WIN64);
		SHOW_MACRO(__WIN32__);
		SHOW_MACRO(__WINDOWS__);
		SHOW_MACRO(__NT__);
		SHOW_MACRO(__TOS_WIN__);
		SHOW_MACRO(_WIN32_WCE);
		SHOW_MACRO(__CHEAP_WINDOWS__);

		// Sony platforms
		SHOW_MACRO(SN_TARGET_PS2);
		SHOW_MACRO(__CELLOS_LV2__);
		SHOW_MACRO(__ORBIS__);
		SHOW_MACRO(SN_TARGET_PSP);
		SHOW_MACRO(__psp2__);

		// Xbox
		SHOW_MACRO(_XBOX);
		SHOW_MACRO(XBOX);

		// Nintendo
		SHOW_MACRO(SDK_ARM9);
		SHOW_MACRO(SDK_TS);
		SHOW_MACRO(RVL_SDK);
		SHOW_MACRO(NN_NINTENDO_SDK);

		// Print known C++ compiler version identifiers
		SHOW_MACRO(__clang__);
		SHOW_MACRO(__ghs__);
		SHOW_MACRO(__llvm__);
		SHOW_MACRO(__xlc__);
		SHOW_MACRO(__APPLE_CPP__);
		SHOW_MACRO(__APPLE_CC__);
		SHOW_MACRO(__NEXT_CPP__);
		SHOW_MACRO(__BORLANDC__);
		SHOW_MACRO(__CC65__);
		SHOW_MACRO(__DJGPP__);
		SHOW_MACRO(__GNUC__);
		SHOW_MACRO(__INTEL_COMPILER);
		SHOW_MACRO(__ICL);
		SHOW_MACRO(__LCC__);
		SHOW_MACRO(__MWERKS__);
		SHOW_MACRO(__CWCC__);
		SHOW_MACRO(__MSL__);
		SHOW_MACRO(_MSC_VER);
		SHOW_MACRO(_MSVC_LANG);
		SHOW_MACRO(__MINGW32__);
		SHOW_MACRO(__MINGW64__);
		SHOW_MACRO(__MRC__);
		SHOW_MACRO(__CC_NORCROFT);
		SHOW_MACRO(__SC__);
		SHOW_MACRO(MPW_CPLUS);
		SHOW_MACRO(MPW_C);
		SHOW_MACRO(__SN__);
		SHOW_MACRO(__SNC__);
		SHOW_MACRO(__TINYC__);
		SHOW_MACRO(__TURBOC__);
		SHOW_MACRO(__WATCOMC__);
		SHOW_MACRO(__ZTC__);

		// Microsoft compiler extensions
		SHOW_MACRO(_CHAR_UNSIGNED);
		SHOW_MACRO(__CLR_VER);
		SHOW_MACRO(_CONTROL_FLOW_GUARD);
		SHOW_MACRO(_CPPRTTI);
		SHOW_MACRO(_CPPUNWIND);
		SHOW_MACRO(_DLL);
		SHOW_MACRO(_INTEGRAL_MAX_BITS);
		SHOW_MACRO(__INTELLISENSE__);
		SHOW_MACRO(_ISO_VOLATILE);
		SHOW_MACRO(_KERNEL_MODE);
		SHOW_MACRO(_M_FP_EXCEPT);
		SHOW_MACRO(_M_FP_FAST);
		SHOW_MACRO(_M_FP_PRECISE);
		SHOW_MACRO(_M_FP_STRICT);
		SHOW_MACRO(_MANAGED);
		SHOW_MACRO(_MSC_BUILD);
		SHOW_MACRO(_MSC_EXTENSIONS);
		SHOW_MACRO(_MSVC_LANG);
		SHOW_MACRO(__MSVC_RUNTIME_CHECKS);
		SHOW_MACRO(_MSVC_TRADITIONAL);
		SHOW_MACRO(_MT);
		SHOW_MACRO(_NATIVE_WCHAR_T_DEFINED);
		SHOW_MACRO(_WCHAR_T_DEFINED);
		SHOW_MACRO(_OPENMP);
		SHOW_MACRO(_PREFAST_);
		SHOW_MACRO(_VC_NODEFAULTLIB);

		// Watcom C predefined macros
		SHOW_MACRO(__CHAR_SIGNED__);
		SHOW_MACRO(__CHEAP_WINDOWS__);
		SHOW_MACRO(__INLINE_FUNCTIONS__);
		SHOW_MACRO(NO_EXT_KEYS);
		SHOW_MACRO(__FLAT__);

		// SN Systems predefined macros
		SHOW_MACRO(__WCHAR_T_IS_KEYWORD);
		SHOW_MACRO(_NO_EX);
		SHOW_MACRO(__EDG_VERSION__);
		SHOW_MACRO(__BOOL_IS_KEYWORD);
		SHOW_MACRO(__SIGNED_CHARS__);
		SHOW_MACRO(__EXCEPTIONS);
		SHOW_MACRO(__PLACEMENT_DELETE);

		// Print known CPU identifiers

		// Mips
		SHOW_MACRO(__R5900__);
		SHOW_MACRO(__R5900);
		SHOW_MACRO(__MIPS__);
		SHOW_MACRO(__mips__);

		// PowerPC
		SHOW_MACRO(__PPC__);
		SHOW_MACRO(__PPCGEKKO__);
		SHOW_MACRO(__PPCBROADWAY__);
		SHOW_MACRO(_M_MPPC);
		SHOW_MACRO(__ppc__);
		SHOW_MACRO(__ppc64__);
		SHOW_MACRO(_ARCH_PPC64);
		SHOW_MACRO(__powerpc64__);
		SHOW_MACRO(_M_PPCBE);
		SHOW_MACRO(_M_PPCLE);
		SHOW_MACRO(__espresso__);
		SHOW_MACRO(__ALTIVEC__);

		// 8086

		SHOW_MACRO(M_I86);
		SHOW_MACRO(__FPI__);
		SHOW_MACRO(M_I86SM);
		SHOW_MACRO(__SMALL__);
		SHOW_MACRO(M_I86M);
		SHOW_MACRO(__MEDIUM__);
		SHOW_MACRO(M_I86CM);
		SHOW_MACRO(__COMPACT__);
		SHOW_MACRO(M_I86LM);
		SHOW_MACRO(__LARGE__);
		SHOW_MACRO(M_I86HM);
		SHOW_MACRO(__HUGE__);

		// 32 bit Intel / AMD
		SHOW_MACRO(_M_IX86);
		SHOW_MACRO(__i386__);
		SHOW_MACRO(M_I386);
		SHOW_MACRO(__386__);

		// AMD64 instruction set
		SHOW_MACRO(_M_AMD64);
		SHOW_MACRO(_M_X64);
		SHOW_MACRO(__x86_64__);
		SHOW_MACRO(__ATOM__);
		SHOW_MACRO(__SSE__);
		SHOW_MACRO(__SSE2__);
		SHOW_MACRO(__AVX__);
		SHOW_MACRO(__AVX2__);
		SHOW_MACRO(__AVX512BW__);
		SHOW_MACRO(__AVX512CD__);
		SHOW_MACRO(__AVX512DQ__);
		SHOW_MACRO(__AVX512F__);
		SHOW_MACRO(__AVX512VL__);

		// Itanium
		SHOW_MACRO(_M_IA64);

		// Motorola 680x0
		SHOW_MACRO(_M_MRX000);
		SHOW_MACRO(_M_M68K);
		SHOW_MACRO(__MC68K__);
		SHOW_MACRO(__m68k__);
		SHOW_MACRO(mc68881);
		SHOW_MACRO(__MC68881__);

		// ARM CPUs
		SHOW_MACRO(_M_ARM);
		SHOW_MACRO(_M_ARM64);
		SHOW_MACRO(__arm__);
		SHOW_MACRO(__arm64__);
		SHOW_MACRO(__aarch64__);
		SHOW_MACRO(__ARM_NEON__);
		SHOW_MACRO(_M_ARM_FP);

		// C++ feature macros, usually found in GNU or clang

		// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0096r5.html
		// C++98 / C++03 [3.8]
		SHOW_MACRO(__cpp_exceptions);
		SHOW_MACRO(__cpp_rtti);

		// C++11 [3.6]
		SHOW_MACRO(__cpp_unicode_characters);
		SHOW_MACRO(__cpp_raw_strings);
		SHOW_MACRO(__cpp_unicode_literals);
		SHOW_MACRO(__cpp_user_defined_literals);
		SHOW_MACRO(__cpp_threadsafe_static_init);
		SHOW_MACRO(__cpp_lambdas);
		SHOW_MACRO(__cpp_constexpr);
		SHOW_MACRO(__cpp_range_based_for);
		SHOW_MACRO(__cpp_static_assert);
		SHOW_MACRO(__cpp_decltype);
		SHOW_MACRO(__cpp_attributes);
		SHOW_MACRO(__cpp_rvalue_references);
		SHOW_MACRO(__cpp_variadic_templates);
		SHOW_MACRO(__cpp_initializer_lists);
		SHOW_MACRO(__cpp_delegating_constructors);
		SHOW_MACRO(__cpp_nsdmi);
		SHOW_MACRO(__cpp_inheriting_constructors);
		SHOW_MACRO(__cpp_ref_qualifiers);
		SHOW_MACRO(__cpp_alias_templates);

		// C++14 [3.5]
		SHOW_MACRO(__cpp_binary_literals);
		SHOW_MACRO(__cpp_init_captures);
		SHOW_MACRO(__cpp_generic_lambdas);
		SHOW_MACRO(__cpp_sized_deallocation);
		SHOW_MACRO(__cpp_decltype_auto);
		SHOW_MACRO(__cpp_return_type_deduction);
		SHOW_MACRO(__cpp_aggregate_nsdmi);
		SHOW_MACRO(__cpp_variable_templates);

#if 0
        SHOW_MACRO(__cpp_lib_integer_sequence);
        SHOW_MACRO(__cpp_lib_exchange_function);
        SHOW_MACRO(__cpp_lib_tuples_by_type);
        SHOW_MACRO(__cpp_lib_tuple_element_t);
        SHOW_MACRO(__cpp_lib_make_unique);
        SHOW_MACRO(__cpp_lib_transparent_operators);
        SHOW_MACRO(__cpp_lib_integral_constant_callable);
        SHOW_MACRO(__cpp_lib_transformation_trait_aliases);
        SHOW_MACRO(__cpp_lib_result_of_sfinae);
        SHOW_MACRO(__cpp_lib_is_final);
        SHOW_MACRO(__cpp_lib_is_null_pointer);
        SHOW_MACRO(__cpp_lib_generic_associative_lookup);
        SHOW_MACRO(__cpp_lib_null_iterators);
        SHOW_MACRO(__cpp_lib_make_reverse_iterator);
        SHOW_MACRO(__cpp_lib_robust_nonmodifying_seq_ops);
        SHOW_MACRO(__cpp_lib_complex_udls);
        SHOW_MACRO(__cpp_lib_quoted_string_io);
        SHOW_MACRO(__cpp_lib_shared_timed_mutex);
#endif

		// C++17 [3.4]
		SHOW_MACRO(__cpp_hex_float);
		SHOW_MACRO(__cpp_inline_variables);
		SHOW_MACRO(__cpp_aligned_new);
		SHOW_MACRO(__cpp_guaranteed_copy_elision);
		SHOW_MACRO(__cpp_noexcept_function_type);
		SHOW_MACRO(__cpp_fold_expressions);
		SHOW_MACRO(__cpp_capture_star_this);
		SHOW_MACRO(__cpp_if_constexpr);
		SHOW_MACRO(__cpp_deduction_guides);
		SHOW_MACRO(__cpp_nontype_template_parameter_auto);
		SHOW_MACRO(__cpp_namespace_attributes);
		SHOW_MACRO(__cpp_enumerator_attributes);
		SHOW_MACRO(__cpp_variadic_using);
		SHOW_MACRO(__cpp_structured_bindings);
		SHOW_MACRO(__cpp_aggregate_bases);
		SHOW_MACRO(__cpp_nontype_template_args);
		SHOW_MACRO(__cpp_template_template_args);
		SHOW_MACRO(__cpp_fold_expressions);
		// SHOW_MACRO(__has_include);

#if 0
        SHOW_MACRO(__cpp_lib_byte);
        SHOW_MACRO(__cpp_lib_hardware_interference_size);
        SHOW_MACRO(__cpp_lib_launder);
        SHOW_MACRO(__cpp_lib_uncaught_exceptions);
        SHOW_MACRO(__cpp_lib_as_const);
        SHOW_MACRO(__cpp_lib_make_from_tuple);
        SHOW_MACRO(__cpp_lib_apply);
        SHOW_MACRO(__cpp_lib_optional);
        SHOW_MACRO(__cpp_lib_any);
        SHOW_MACRO(__cpp_lib_memory_resource);
        SHOW_MACRO(__cpp_lib_boyer_moore_searcher);
        SHOW_MACRO(__cpp_lib_string_view);
        SHOW_MACRO(__cpp_lib_sample);
        SHOW_MACRO(__cpp_lib_optional);
        SHOW_MACRO(__cpp_lib_variant);
        SHOW_MACRO(__cpp_lib_addressof_constexpr);
        SHOW_MACRO(__cpp_lib_raw_memory_algorithms);
        SHOW_MACRO(__cpp_lib_transparent_operators);
#endif

		// C++20
		// https://en.cppreference.com/w/cpp/feature_test
		SHOW_MACRO(__cpp_aggregate_paren_init);
		SHOW_MACRO(__cpp_char8_t);
		SHOW_MACRO(__cpp_char8_type);
		SHOW_MACRO(__cpp_conditional_explicit);
		SHOW_MACRO(__cpp_coroutines);
		SHOW_MACRO(__cpp_impl_destroying_delete);
		SHOW_MACRO(__cpp_impl_three_way_comparison);
		SHOW_MACRO(__cpp_nontype_template_parameter_class);
	}
}

/***************************************

	Print the contents of these macros so they
	can be manually checked for correctness

***************************************/

static void BURGER_API ShowMacros(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(NULL);
		SHOW_MACRO(TRUE);
		SHOW_MACRO(FALSE);

		SHOW_MACRO(BURGER_ALIGN(__type, __name, __a));
		SHOW_MACRO(BURGER_PREALIGN(__a));
		SHOW_MACRO(BURGER_POSTALIGN(__a));

		SHOW_MACRO(BURGER_API);
		SHOW_MACRO(BURGER_ANSIAPI);
		SHOW_MACRO(BURGER_INLINE);
		SHOW_MACRO(BURGER_DECLSPECNAKED);
		SHOW_MACRO(BURGER_ASM);
		SHOW_MACRO(BURGER_MININT);
		SHOW_MACRO(BURGER_MAXINT);
		SHOW_MACRO(BURGER_MAXUINT);
		SHOW_MACRO(BURGER_MININT64);
		SHOW_MACRO(BURGER_MAXINT64);
		SHOW_MACRO(BURGER_MAXUINT64);

		// 64 bit supported?
		SHOW_MACRO(BURGER_LONGLONG);

		// Display optional macros
		SHOW_MACRO(BURGER_STRUCT_PACKPUSH);
		SHOW_MACRO(BURGER_STRUCT_ALIGN);
		SHOW_MACRO(BURGER_STRUCT_PACK);
		SHOW_MACRO(BURGER_FASTCALLENABLED);
		SHOW_MACRO(BURGER_OPENGL);
		SHOW_MACRO(BURGER_OPENGLES);
		SHOW_MACRO(BURGER_VULKAN);
	}
}

/***************************************

	Test the endian macros

***************************************/

static uint_t BURGER_API TestEndianMacros(uint_t uVerbose) BURGER_NOEXCEPT
{
	union {
		uint32_t m_uInt32;   // 32 bit integer to store
		uint8_t m_uBytes[4]; // Byte array to test the endian with
	} EndianTest;

	uint_t uFailure = 0;

#if defined(BURGER_LITTLEENDIAN) && defined(BURGER_BIGENDIAN)
	Message(
		"Error: Both BURGER_LITTLEENDIAN and BURGER_BIGENDIAN are defined!");
	uFailure = 10;
#endif

#if !defined(BURGER_LITTLEENDIAN) && !defined(BURGER_BIGENDIAN)
	Message(
		"Error: Neither BURGER_LITTLEENDIAN or BURGER_BIGENDIAN are defined!");
	uFailure = 10;
#endif

	//
	// Test the endian ACTUALLY matches
	//

	EndianTest.m_uInt32 = 0x12345678U;
	uint_t uTest;

#if defined(BURGER_BIGENDIAN)
	uTest = (EndianTest.m_uBytes[0] == 0x78);
	uFailure |= uTest;
	ReportFailure(
		"BURGER_BIGENDIAN was defined on a Little endian machine!", uTest);
#endif

#if defined(BURGER_LITTLEENDIAN)
	uTest = (EndianTest.m_uBytes[0] == 0x12);
	uFailure |= uTest;
	ReportFailure(
		"BURGER_LITTLEENDIAN was defined on a Big endian machine!", uTest);
#endif

	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_BIGENDIAN);
		SHOW_MACRO(BURGER_LITTLEENDIAN);
	}
	return uFailure;
}

/***************************************

	Test the debug flag macros

***************************************/

static uint_t BURGER_API TestDebugMacros(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;

#if defined(NDEBUG) && defined(_DEBUG)
	Message("Error: Both NDEBUG and _DEBUG are defined!");
	uFailure = 10;
#endif

#if !defined(NDEBUG) && !defined(_DEBUG)
	Message("Error: Neither NDEBUG or _DEBUG are defined!");
	uFailure = 10;
#endif

	//
	// Display the debug/release macros
	//

	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(NDEBUG);
		SHOW_MACRO(_DEBUG);
	}

	return uFailure;
}

/***************************************

	Test the compiler macros

***************************************/

static uint_t BURGER_API TestCompilerMacros(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;
	uint_t uTest = 0;

	if (uVerbose & VERBOSE_MACROS) {
		// Compiler level
		SHOW_MACRO(BURGER_CPP89);
		SHOW_MACRO(BURGER_CPP98);
		SHOW_MACRO(BURGER_CPP11);
		SHOW_MACRO(BURGER_CPP14);
		SHOW_MACRO(BURGER_CPP17);
		SHOW_MACRO(BURGER_CPP20);
		SHOW_MACRO(BURGER_COMPILER_VERSION);
		SHOW_MACRO(BURGER_COMPILER_NAME);
		SHOW_MACRO(BURGER_CONSTEXPR);
		SHOW_MACRO(BURGER_NOEXCEPT);
		SHOW_MACRO(BURGER_OVERRIDE);
		SHOW_MACRO(BURGER_FINAL);
		SHOW_MACRO(BURGER_RVALUE_REFERENCES);
		SHOW_MACRO(BURGER_DISABLE_ASAN);
		SHOW_MACRO(BURGER_DISABLE_MSAN);
		SHOW_MACRO(BURGER_MAYBE_UNUSED);
		SHOW_MACRO(BURGER_NODISCARD);
		SHOW_MACRO(BURGER_FALLTHROUGH);
		SHOW_MACRO(BURGER_USED);
		SHOW_MACRO(BURGER_NO_INLINE);
		SHOW_MACRO(BURGER_NORETURN);
		SHOW_MACRO(BURGER_PRINTF_ATTRIBUTE);
		SHOW_MACRO(nullptr);
		SHOW_MACRO(BURGER_ENUM_TYPE(SampleEnum, uint_t));
		SHOW_MACRO(BURGER_ENUM_CLASS_START(SampleEnum, uint_t));
		SHOW_MACRO(BURGER_ENUM_CLASS_END(SampleEnum));
		SHOW_MACRO(BURGER_HAS_WCHAR_T);
		SHOW_MACRO(BURGER_HAS_CHAR8_T);
		SHOW_MACRO(BURGER_HAS_CHAR16_T);

		// Disable, the macro is huge
		// SHOW_MACRO(BURGER_ENUM_CLASS_END_MATH(SampleEnum, uint_t));

		SHOW_MACRO(BURGER_STATIC_ASSERT(sizeof(int) == 4));
	}

#if defined(BURGER_WATCOM)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_WATCOM);
	}
	++uTest;
#endif

#if defined(BURGER_MINGW)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_MINGW);
	}
	++uTest;
#endif

#if defined(BURGER_MRC)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_MRC);
	}
	++uTest;
#endif

#if defined(BURGER_APPLE_SC)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_APPLE_SC);
	}
	++uTest;
#endif

#if defined(BURGER_INTEL_COMPILER)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_INTEL_COMPILER);
	}
	++uTest;
#endif

#if defined(BURGER_SNSYSTEMS)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_SNSYSTEMS);
	}
	++uTest;
#endif

#if defined(BURGER_METROWERKS)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_METROWERKS);
	}
	++uTest;
#endif

#if defined(BURGER_MSVC)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_MSVC);
	}
	++uTest;
#endif

#if defined(BURGER_GNUC)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_GNUC);
	}
	++uTest;
#endif

#if defined(BURGER_CLANG)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_CLANG);
	}
	++uTest;
#endif

	if (uTest >= 2) {
		Message("Multiple compilers have been defined!");
		uFailure = 10;
	}
	if (!uTest) {
		Message("Unknown compiler detected!");
		uFailure = 10;
	}
	return uFailure;
}

/***************************************

	Test the CPU macros

***************************************/

static uint_t BURGER_API TestCPUMacros(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;
	uint_t uTest = 0;

#if defined(BURGER_68K)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_68K);
	}
	++uTest;
#endif

#if defined(BURGER_POWERPC)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_POWERPC);
	}
	++uTest;
#endif

#if defined(BURGER_POWERPC64)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_POWERPC64);
	}
	++uTest;
#endif

#if defined(BURGER_ARM32)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_ARM32);
	}
	++uTest;
#endif

#if defined(BURGER_ARM64)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_ARM64);
	}
	++uTest;
#endif

#if defined(BURGER_X86)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_X86);
	}
	++uTest;
#endif

#if defined(BURGER_AMD64)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_AMD64);
	}
	++uTest;
#endif

#if defined(BURGER_MIPS32)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_MIPS32);
	}
	++uTest;
#endif

#if defined(BURGER_MIPS64)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_MIPS64);
	}
	++uTest;
#endif

	// This is an enhancement, not a CPU type, so it's okay to
	// be defined with a CPU type

	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_SIZEOF_INT);
		SHOW_MACRO(BURGER_SIZEOF_LONG);
		SHOW_MACRO(BURGER_NEED_UINTPTR_REMAP);
		SHOW_MACRO(BURGER_64BITCPU);
		SHOW_MACRO(BURGER_INTEL);
		SHOW_MACRO(BURGER_PPC);
		SHOW_MACRO(BURGER_ARM);
		SHOW_MACRO(BURGER_MIPS);
		SHOW_MACRO(BURGER_SPARC);
		SHOW_MACRO(BURGER_NEON);
		SHOW_MACRO(BURGER_SSE);
		SHOW_MACRO(BURGER_SSE2);
		SHOW_MACRO(BURGER_3DNOW);
		SHOW_MACRO(BURGER_ALTIVEC);
		SHOW_MACRO(BURGER_AVX);
		SHOW_MACRO(BURGER_AVX2);
	}

	if (uTest != 1) {
		Message("Multiple CPUs have been defined!");
		uFailure = 10;
	}

	if (!uTest) {
		Message("Unknown CPU detected!");
		uFailure = 10;
	}
	return uFailure;
}

/***************************************

	Test the platform macros

***************************************/

static uint_t BURGER_API TestPlatformMacros(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;
	uint_t uTest = 0;

	//
	// Handle MSDOS
	//

#if defined(BURGER_MSDOS)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_MSDOS);
	}
	++uTest;
#endif

#if defined(BURGER_DOS4G)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_DOS4G);
	}
#endif

#if defined(BURGER_X32)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_X32);
	}
#endif

	//
	// Handle Windows
	//

#if defined(BURGER_WIN32)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_WIN32);
	}
	++uTest;
#endif

#if defined(BURGER_WIN64)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_WIN64);
	}
	++uTest;
#endif

#if defined(BURGER_WINDOWS)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_WINDOWS);
	}
#endif

	//
	// Dead platforms
	//

#if defined(BURGER_BEOS)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_BEOS);
	}
	++uTest;
#endif

#if defined(BURGER_NGAGE)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_NGAGE);
	}
	++uTest;
#endif

#if defined(BURGER_SYMBIAN)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_SYMBIAN);
	}
	++uTest;
#endif

	//
	// MacOS
	//

#if defined(BURGER_MAC)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_MAC);
	}
	++uTest;
#endif

#if defined(BURGER_MACOSX)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_MACOSX);
	}
	++uTest;
#endif

	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_MACOS);
		SHOW_MACRO(BURGER_MACCLASSIC);
		SHOW_MACRO(BURGER_MACCARBON);
		SHOW_MACRO(TARGET_API_MAC_CARBON);
		SHOW_MACRO(BURGER_CFM);
		SHOW_MACRO(BURGER_DARWIN);
		SHOW_MACRO(BURGER_UNIX);
	}

	//
	// iOS
	//
#if defined(BURGER_IOS)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_IOS);
	}
	++uTest;
#endif

	//
	// Android and their sub platforms
	//
#if defined(BURGER_ANDROID)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_ANDROID);
	}
	++uTest;
#endif

#if defined(BURGER_OUYA)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_OUYA);
	}
#endif

#if defined(BURGER_SHIELD)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_SHIELD);
	}
#endif

#if defined(BURGER_AMICO)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_AMICO);
	}
#endif

#if defined(BURGER_LINUX)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_LINUX);
	}
	++uTest;
#endif

	//
	// Video game consoles
	//

#if defined(BURGER_XBOX)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_XBOX);
	}
	++uTest;
#endif

#if defined(BURGER_XBOX360)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_XBOX360);
	}
	++uTest;
#endif

#if defined(BURGER_XBOXONE)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_XBOXONE);
	}
	++uTest;
#endif

#if defined(BURGER_PS2)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_PS2);
	}
	++uTest;
#endif

#if defined(BURGER_PS3)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_PS3);
	}
	++uTest;
#endif

#if defined(BURGER_PS4)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_PS4);
	}
	++uTest;
#endif

#if defined(BURGER_PSP)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_PSP);
	}
	++uTest;
#endif

#if defined(BURGER_VITA)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_VITA);
	}
	++uTest;
#endif

#if defined(BURGER_GAMECUBE)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_GAMECUBE);
	}
	++uTest;
#endif

#if defined(BURGER_WII)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_WII);
	}
	++uTest;
#endif

#if defined(BURGER_WIIU)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_WIIU);
	}
	++uTest;
#endif

#if defined(BURGER_SWITCH)
	if (uVerbose & VERBOSE_MACROS) {
		SHOW_MACRO(BURGER_SWITCH);
	}
	++uTest;
#endif

#if defined(BURGER_DS)
	if (uVerbose & VERBOSE_MACROS) {
		Message("BURGER_DS is defined");
	}
	++uTest;
#endif

	if (uTest >= 2) {
		Message("Multiple Platforms have been defined!");
		uFailure = 10;
	}
	if (!uTest) {
		Message("Unknown Platform detected!");
		uFailure = 10;
	}
	return uFailure;
}

/***************************************

	Test the platform macros

***************************************/

BURGER_ENUM_TYPE(eTypeTest32, uint32_t) {
	ALPHA = 1, BETA = 2, GAMMA = 4
};
BURGER_ENUM_MATH(eTypeTest32, uint32_t)

BURGER_ENUM_CLASS_START(eEnumClass32, uint32_t) {
	DELTA = 1, EPSILON = 2, OMEGA = 4
}
BURGER_ENUM_CLASS_END(eEnumClass32);

BURGER_ENUM_CLASS_START(eEnumMath32, uint32_t) {
	DELTA = 1, EPSILON = 2, OMEGA = 4
}
BURGER_ENUM_CLASS_END_MATH(eEnumMath32, uint32_t)

static uint_t BURGER_API TestEnumMacros(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;
	uint_t uTest = 0;

	if (uVerbose & VERBOSE_MSG) {
		Message("Running Enum Macro tests");
	}

	// Should not generate compiler warnings, if it does, consider it
	// a unit test failure.

	// Test BURGER_ENUM_TYPE() and BURGER_ENUM_MATH()
	eTypeTest32 Foo = BETA | ALPHA;
	Foo |= GAMMA;

	uTest = Foo != 7;
	uFailure |= uTest;
	ReportFailure("eTypeTest32 ALPHA|BETA|GAMMA should be 7, got %u.", uTest,
		static_cast<uint_t>(Foo));

	Foo = Foo & ~ALPHA;
	uTest = Foo != 6;
	uFailure |= uTest;
	ReportFailure("eTypeTest32 7 & ~ALPHA should be 6, got %u.", uTest,
		static_cast<uint_t>(Foo));

	Foo &= ~BETA;
	uTest = Foo != 4;
	uFailure |= uTest;
	ReportFailure("eTypeTest32 6 &= ~BETA should be 4, got %u.", uTest,
		static_cast<uint_t>(Foo));

	Foo = ALPHA ^ Foo;
	uTest = Foo != 5;
	uFailure |= uTest;
	ReportFailure("eTypeTest32 4 = ALPHA ^ Foo should be 5, got %u.", uTest,
		static_cast<uint_t>(Foo));

	Foo ^= BETA;
	uTest = Foo != 7;
	uFailure |= uTest;
	ReportFailure("eTypeTest32 5 ^= BETA should be 7, got %u.", uTest,
		static_cast<uint_t>(Foo));

	uTest = !Foo;
	uFailure |= uTest;
	ReportFailure("eTypeTest32 !Foo should be true, got %u.", uTest, uTest);

	Foo &= ~(ALPHA | BETA | GAMMA);
	uTest = !!Foo;
	uFailure |= uTest;
	ReportFailure("eTypeTest32 !!Foo should be false, got %u.", uTest, uTest);

	// Test BURGER_ENUM_CLASS_START()

	// Test constructors
	eEnumClass32 Bar = eEnumClass32::DELTA;
	eEnumClass32 Bar2;

#if !defined(BURGER_MACOSX)
	// Test if the enum is switchable and assignment operator
	Bar = eEnumClass32::EPSILON;
	Bar2 = eEnumClass32::DELTA;
	switch (Bar) {
	case eEnumClass32::EPSILON:
		uTest = FALSE;
		break;
	default:
	case eEnumClass32::DELTA:
	case eEnumClass32::OMEGA:
		uTest = TRUE;
		break;
	}
	uFailure |= uTest;
	ReportFailure("eEnumClass32 switch failed, got %u.", uTest, uTest);
#endif

	uTest = Bar < eEnumClass32::DELTA;
	uFailure |= uTest;
	ReportFailure("Bar < eEnumClass32::DELTA got %u.", uTest, uTest);

	uTest = Bar <= Bar2;
	uFailure |= uTest;
	ReportFailure("Bar <= Bar2 got %u.", uTest, uTest);

	uTest = Bar != eEnumClass32::EPSILON;
	uFailure |= uTest;
	ReportFailure("Bar != eEnumClass32::EPSILON got %u.", uTest, uTest);

	uTest = Bar == eEnumClass32::DELTA;
	uFailure |= uTest;
	ReportFailure("Bar == eEnumClass32::DELTA got %u.", uTest, uTest);

	// Test BURGER_ENUM_CLASS_END_MATH()

	eEnumMath32 Foobar;
	eEnumMath32 Barf = eEnumMath32::DELTA;
	Foobar = eEnumMath32::EPSILON;
	uTest = Foobar == Barf;
	uFailure |= uTest;
	ReportFailure("Foobar == Barf got %u.", uTest, uTest);

	Foobar |= Barf;
	Foobar |= eEnumMath32::OMEGA;
	uTest = Barf != eEnumMath32::DELTA;
	uFailure |= uTest;
	ReportFailure("Barf != eEnumMath32::DELTA got %u.", uTest, uTest);

	uTest = !(Barf == eEnumMath32::DELTA);
	uFailure |= uTest;
	ReportFailure("Barf == eEnumMath32::DELTA got %u.", uTest, uTest);

	uTest = !(Barf != Foobar);
	uFailure |= uTest;
	ReportFailure("Barf != eEnumMath32::DELTA got %u.", uTest, uTest);

	Barf = eEnumMath32::EPSILON ^ eEnumMath32::DELTA;
	uTest = !(Barf == eEnumMath32(eEnumMath32::DELTA | eEnumMath32::EPSILON));
	uFailure |= uTest;
	ReportFailure("Barf != 3 got %u.", uTest, uTest);

	Foobar = eEnumMath32::DELTA;
	Barf &= ~Foobar;
	uTest = !(Barf == eEnumMath32::EPSILON);
	uFailure |= uTest;
	ReportFailure("Barf == eEnumMath32::EPSILON got %u.", uTest, uTest);

	if (!uFailure && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all Enum Macro tests!");
	}
	return uFailure;
}

/***************************************

	Perform the tests for the macros and compiler
	settings

***************************************/

int BURGER_API TestMacros(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;

	if (uVerbose & VERBOSE_MSG) {
		Message("Testing macros");
	}

	ShowDefaultSigns(uVerbose);
	ShowIntrinsicSizes(uVerbose);
	ShowCompilerMacros(uVerbose);
	ShowMacros(uVerbose);

	uFailure = TestEndianMacros(uVerbose);
	uFailure |= TestDebugMacros(uVerbose);
	uFailure |= TestCompilerMacros(uVerbose);
	uFailure |= TestCPUMacros(uVerbose);
	uFailure |= TestPlatformMacros(uVerbose);
	uFailure |= TestEnumMacros(uVerbose);
	return static_cast<int>(uFailure);
}
