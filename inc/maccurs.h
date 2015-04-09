/* maccurs.h - Carbon/Toolbox curses emulator
 * Copyright 1997-2008, Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

/* About the Carbon/Toolbox curses emulator
 * ========================================
 *
 * This code is mostly written in an interoperable common subset of
 * Carbon and Toolbox.
 *
 * Use a PowerPC and/or x86 (32-bit) Mach-O toolchain (e.g. gcc from
 * Xcode) to build a "Universal", Intel x86, or 32-bit PowerPC Mach-O
 * Carbon application for Mac OS X. Since Quickdraw is deprecated
 * these days you will see lots of warning messages.
 *
 * Use a PowerPC PEF toolchain (e.g. MrC or powerpc-gcc in MPW) to
 * build a 32-bit PowerPC Carbon CFM application for Mac OS X 10.6 or
 * earlier, Mac OS 9.x, and Mac OS 8.6. Requires CarbonLib 1.6 runtime
 * on Mac OS 8.6/9.x.
 *
 * Use a PowerPC PEF toolchain (e.g. MrC or powerpc-gcc in MPW) and/or
 * a 68k 'CODE' fragment-capable toolchain (e.g. SC, MPW C or m68k-gcc
 * in MPW) to build a "Fat", PowerPC, or 68k Toolbox application for
 * Mac OS 9.x or earlier, although it is unlikely to work with System
 * versions prior to System 7 (or prior to OS version 4.1 if you use
 * the Universal Interfaces 2.1.5 SDK for the 68k compilation.)  This
 * also seems to work in the final freeware version of ARDI's
 * "Executor" cleanroom reimplementation of Toolbox and Mac OS/System
 * 7 for Intel Linux, Win32, etc.
 *
 * Use a Carbon-capable PowerPC PEF toolchain (e.g. MrC in MPW) and a
 * 68k 'CODE' fragment-capable toolchain (e.g. SC, MPW C or m68k-gcc
 * in MPW) to build a "Fat" PowerPC Carbon CFM + 68k Toolbox
 * application for Mac OS X 10.6 or earlier, Mac OS 9.x, and Mac OS
 * 8.6 on PowerPC and for Mac OS 8.1 or earlier on 68k (see previous
 * paragraphs for guesses about compatibility.) Although it is not as
 * well-tested, CFM-68K builds should work too on a subset of those
 * (under System 7.1 through Mac OS 8.1 on a 68020, 68030 or 68040
 * processor with 32-bit addressing enabled provided the CFM-68K
 * Runtime Enabler and the Appearance Extension [a.k.a. AppearanceLib]
 * are installed.)
 *
 * Use a PowerPC PEF toolchain (e.g. MrC or powerpc-gcc in MPW) and/or
 * a 68k 'CODE' fragment-capable toolchain (e.g. SC, MPW C or m68k-gcc
 * in MPW) to build a "Fat", PowerPC, or 68k MPW Tool or SIOW app.
 *
 * See utl/macbuild.txt for more information on installing and
 * building with MPW.
 *
 */

#ifndef MYMAN_MACCURS_H_INCLUDED
#define MYMAN_MACCURS_H_INCLUDED 1

#ifdef macintosh

#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON

/* Case 1: flat Carbon headers */
#include <Carbon.h>

#else /* ! (defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON) */

/* Case 2: Toolbox */
#ifndef ANCIENT_HEADERS
#include <ConditionalMacros.h>
#endif

/* we work around some overly aggressive deprecation in newer
 * <ConditionalMacros.h> so that we can also build with the older
 * Universal Interfaces 2.1.5 */

#ifdef __CARBON__
#ifdef TARGET_CPU_PPC
#undef GENERATINGPOWERPC
#define GENERATINGPOWERPC TARGET_CPU_PPC
#endif /* defined(TARGET_CPU_PPC) */
#ifdef TARGET_CPU_68K
#undef GENERATING68K
#define GENERATING68K TARGET_CPU_68K
#endif /* defined(TARGET_CPU_68K) */
#ifdef TARGET_RT_MAC_68881
#undef GENERATING68881
#define GENERATING68881 TARGET_RT_MAC_68881
#endif /* defined(TARGET_RT_MAC_68881) */
#ifdef TARGET_RT_MAC_CFM
#undef GENERATINGCFM
#define GENERATINGCFM TARGET_RT_MAC_CFM
#endif /* defined(TARGET_RT_MAC_CFM) */
#ifdef TARGET_RT_MAC_CFM
#undef CFMSYSTEMCALLS
#define CFMSYSTEMCALLS TARGET_RT_MAC_CFM
#endif /* defined(TARGET_RT_MAC_CFM) */
#endif /* defined(__CARBON__) */

/* when USE_OLD_TOOLBOX is non-zero we directly access Toolbox
 * internals which were deprecated or removed in Carbon, and avoid
 * routines which are available only in Carbon and MacOS PowerPC
 * runtime environments */
#ifndef USE_OLD_TOOLBOX
#if defined(__CARBON__) || (defined(GENERATINGPOWERPC) && GENERATINGPOWERPC)
#define USE_OLD_TOOLBOX 0
#else
#define USE_OLD_TOOLBOX 1
#endif
#endif /* ! defined(USE_OLD_TOOLBOX) */

/* when USE_FONT_MANAGER_9 is zero we avoid use of Font Manager 9
 * routines */
#ifndef USE_FONT_MANAGER_9
#define USE_FONT_MANAGER_9 (! USE_OLD_TOOLBOX)
#endif

/* when USE_TEXT_ENCODING_CONVERSION_MANAGER is zero we avoid use
 * of Text Encoding Conversion Manager routines */
#ifndef USE_TEXT_ENCODING_CONVERSION_MANAGER
#define USE_TEXT_ENCODING_CONVERSION_MANAGER (! USE_OLD_TOOLBOX)
#endif

#include <AppleEvents.h>
#include <CursorCtl.h>
#include <Devices.h>
#include <Errors.h>
#include <Files.h>
#include <Fonts.h>
#ifndef ANCIENT_HEADERS
#include <Gestalt.h>
#else
#include <GestaltEqu.h>
#endif
#include <Menus.h>
#ifndef ANCIENT_HEADERS
#include <Math64.h>
#endif
#include <QDOffscreen.h>
#include <Quickdraw.h>
#ifndef ANCIENT_HEADERS
#include <QuickdrawText.h>
#endif
#include <Resources.h>
#include <Script.h>
#include <Sound.h>
#ifndef ANCIENT_HEADERS
#include <TextUtils.h>
#endif
#include <Timer.h>
#include <ToolUtils.h>
#include <Traps.h>
#include <ioctl.h>
#include <time.h>

#ifdef UNICODE
#include <ATSUnicode.h>
#endif

#if defined(UNIVERSAL_INTERFACES_VERSION) && (UNIVERSAL_INTERFACES_VERSION >= 0x0300)

#include <MacTypes.h>
#include <MacWindows.h>
#include <Patches.h>

#else /* ! (defined(UNIVERSAL_INTERFACES_VERSION) && (UNIVERSAL_INTERFACES_VERSION >= 0x0300)) */

#include <Events.h>
#include <Types.h>
#include <Windows.h>
#include <OSUtils.h>

#ifdef ANCIENT_HEADERS
#include <Desk.h>
#include <OSEvents.h>
#endif

#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE
#define FALSE false
#endif

#ifdef ANCIENT_HEADERS

#ifndef WindowRef
#define WindowRef WindowPtr
#endif

#ifndef MenuRef
#define MenuRef MenuHandle
#endif

#ifndef UniversalProcPtr
/* HACK: This should be ProcPtr, but that's not what NGetTrapAddress
 * returns in the ancient headers... */
typedef long UniversalProcPtr;
#endif

#ifndef AEEventHandlerUPP
#define AEEventHandlerUPP EventHandlerProcPtr
#endif

#ifndef UInt32
typedef unsigned long UInt32;
#endif

#ifndef SInt32
typedef long SInt32;
#endif

#ifndef UInt16
typedef unsigned short UInt16;
#endif

#ifndef SInt16
typedef short SInt16;
#endif

#ifndef UInt8
typedef unsigned char UInt8;
#endif

#ifndef SInt8
typedef signed char SInt8;
#endif

#ifndef UnsignedWide
struct UnsignedWide {
    UInt32 hi;
    UInt32 lo;
};
typedef struct UnsignedWide UnsignedWide;
#endif

#ifndef EventMask
typedef UInt16 EventMask;
#endif

#ifndef OSStatus
typedef SInt32 OSStatus;
#endif

#ifndef GetWindowPort
#define GetWindowPort(windowRef) ((CGrafPtr) (windowRef))
#endif

#ifndef GetMenuHandle
#define GetMenuHandle(menuID) GetMHandle(menuID)
#endif

#ifndef GetMenuItemText
#define GetMenuItemText(menu, item, itemStr) GetItem((menu), (item), (itemStr))
#endif

#ifndef AppendResMenu
#define AppendResMenu(menu, typ) AddResMenu((menu), (typ))
#endif

#ifndef NewRoutineDescriptor
#define NewRoutineDescriptor(proc, procInfo, isa) ((UniversalProcPtr) (proc))
#endif

#ifndef GetWindowStructureRgn
#define GetWindowStructureRgn(winPtr, regionHandle) CopyRgn(*(RgnHandle *) (((UInt8 *) (winPtr)) + sizeof(GrafPort) + 0x6), (regionHandle))
#endif

#ifndef GetWindowContentRgn
#define GetWindowContentRgn(winPtr, regionHandle) CopyRgn(*(RgnHandle *) (((UInt8 *) winPtr) + sizeof(GrafPort) + 0xA), (regionHandle))
#endif

#ifndef GetWindowUpdateRgn
#define GetWindowUpdateRgn(winPtr, regionHandle) CopyRgn( *(RgnHandle *)(((UInt8 *) winPtr) + sizeof(GrafPort) + 0xE), (regionHandle))
#endif

#ifndef FontToScript
#define FontToScript(fontNum) Font2Script(fontNum)
#endif

#ifndef KeyTranslate
#define KeyTranslate(map, code, state) KeyTrans((map), (short) (code), (long *) (void *) (state))
#endif

#ifndef GetScriptManagerVariable
#define GetScriptManagerVariable(verb) GetEnvirons(verb)
#endif

#endif /* defined(ANCIENT_HEADERS) */

#ifndef MacGetCurrentProcess
#define MacGetCurrentProcess GetCurrentProcess
#endif

#ifndef MacDrawMenuBar
#define MacDrawMenuBar DrawMenuBar
#endif

#ifndef MacCheckMenuItem
#define MacCheckMenuItem CheckItem
#endif

#ifndef MacMoveWindow
#define MacMoveWindow MoveWindow
#endif

#ifndef CharParameter
#define CharParameter short
#endif

#ifndef WindowRegionCode
#define WindowRegionCode UInt16
#endif

#ifndef kWindowStructureRgn
#define kWindowStructureRgn 32
#endif

#ifndef kWindowContentRgn
#define kWindowContentRgn 33
#endif

#ifndef kWindowUpdateRgn
#define kWindowUpdateRgn 34
#endif

#ifndef GetMenuRef
#define GetMenuRef GetMenuHandle
#endif

#ifndef MenuBarHandle
#define MenuBarHandle Handle
#endif

#ifndef MenuItemIndex
#define MenuItemIndex UInt16
#endif

#ifndef WindowPartCode
#define WindowPartCode SInt16
#endif

#ifndef inCollapseBox
#define inCollapseBox 11
#endif

#ifndef inProxyIcon
#define inProxyIcon 12
#endif

#ifndef kLeftArrowCharCode
#define kLeftArrowCharCode 0x1c
#endif

#ifndef kRightArrowCharCode
#define kRightArrowCharCode 0x1d
#endif

#ifndef kUpArrowCharCode
#define kUpArrowCharCode 0x1e
#endif

#ifndef kDownArrowCharCode
#define kDownArrowCharCode 0x1f
#endif

#ifndef kDeleteCharCode
#define kDeleteCharCode 0x7f
#endif

#ifndef NewAEEventHandlerUPP
#define NewAEEventHandlerUPP(proc) (AEEventHandlerUPP) NewRoutineDescriptor((ProcPtr) (proc), uppAEEventHandlerProcInfo, GetCurrentArchitecture())
#endif

#ifndef IsWindowCollapsed
#define IsWindowCollapsed(win) EmptyRgn(((WindowPeek) (win))->contRgn)
#endif

#ifndef CollapseWindow
#define CollapseWindow(win, bCollapse) ((bCollapse == IsWindowCollapsed(win)) ? noErr : unimpErr)
#endif

#ifndef GetWindowPortBounds
#define GetWindowPortBounds maccurses_GetWindowPortBounds

static Rect *GetWindowPortBounds(WindowRef window, Rect *bounds)
{
    if (! window) return NULL;
    if (bounds)
    {
        *bounds = window->portRect;
    }
    return bounds;
}

#endif /* ! defined(GetWindowPortBounds) */

#endif /* ! (defined(UNIVERSAL_INTERFACES_VERSION) && (UNIVERSAL_INTERFACES_VERSION >= 0x0300)) */

#if USE_TEXT_ENCODING_CONVERSION_MANAGER

#include <TextEncodingConverter.h>

#else /* ! USE_TEXT_ENCODING_CONVERSION_MANAGER */

#ifndef __TEXTCOMMON__

#ifndef TextEncoding
#define TextEncoding UInt32
#endif

#ifndef kTextEncodingMacRoman
#define kTextEncodingMacRoman smRoman
#endif

#ifndef kTextEncodingMacJapanese
#define kTextEncodingMacJapanese smJapanese
#endif

#ifndef kTextEncodingMacChineseTrad
#define kTextEncodingMacChineseTrad smTradChinese
#endif

#ifndef kTextEncodingMacChineseSimp
#define kTextEncodingMacChineseSimp smSimpChinese
#endif

#ifndef kTextEncodingMacKorean
#define kTextEncodingMacKorean smKorean
#endif

#ifndef kTextEncodingMacVT100
#define kTextEncodingMacVT100 0xfc
#endif

#ifndef kTextEncodingDOSLatinUS
#define kTextEncodingDOSLatinUS 0x400
#endif

#ifndef kTextEncodingUS_ASCII
#define kTextEncodingUS_ASCII 0x600
#endif

#endif /* ! defined(__TEXTCOMMON__) */

#endif /* ! USE_TEXT_ENCODING_CONVERSION_MANAGER */

#if (! defined(UNIVERSAL_INTERFACES_VERSION)) || (UNIVERSAL_INTERFACES_VERSION < 0x0300)

#ifndef _AppearanceDispatch
#define _AppearanceDispatch 0xAA74
#endif

#endif /* (! defined(UNIVERSAL_INTERFACES_VERSION)) || (UNIVERSAL_INTERFACES_VERSION < 0x0300) */

#endif /* ! (defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON) */

#else /* ! defined(macintosh) */

/* Case 3: Carbon framework */
#include <Carbon/Carbon.h>

#endif /* ! defined(macintosh) */

#ifndef USE_OLD_TOOLBOX
#define USE_OLD_TOOLBOX 0
#endif

#ifndef USE_FONT_MANAGER_9
#define USE_FONT_MANAGER_9 1
#endif

#ifndef USE_TEXT_ENCODING_CONVERSION_MANAGER
#define USE_TEXT_ENCODING_CONVERSION_MANAGER 1
#endif

#if USE_TEXT_ENCODING_CONVERSION_MANAGER

static int maccurses_use_text_encoding_conversion_manager = 0;

static TextEncodingBase
maccurses_GetTextEncodingBase(TextEncoding e)
{
    if (maccurses_use_text_encoding_conversion_manager && GetTextEncodingBase)
    {
        return GetTextEncodingBase(e);
    }
    return e & 0xffff;
}
#undef GetTextEncodingBase
#define GetTextEncodingBase(e) maccurses_GetTextEncodingBase(e)

static OSStatus
maccurses_RevertTextEncodingToScriptInfo(TextEncoding enc, ScriptCode *pscript, LangCode *plang, Str255 fontname)
{
    if (maccurses_use_text_encoding_conversion_manager && RevertTextEncodingToScriptInfo)
    {
        return RevertTextEncodingToScriptInfo(enc, pscript, plang, fontname);
    }
    return unimpErr;
}
#undef RevertTextEncodingToScriptInfo
#define RevertTextEncodingToScriptInfo(e,s,l,f) maccurses_RevertTextEncodingToScriptInfo((e),(s),(l),(f))

#else

#ifndef GetTextEncodingBase
#define GetTextEncodingBase(e) ((e) & 0xffff)
#endif

#ifndef RevertTextEncodingToScriptInfo
#define RevertTextEncodingToScriptInfo(e,s,l,f) (unimpErr)
#endif

#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __CARBON__

/* we must provide storage for the global qd */
#ifndef ANCIENT_HEADERS
QDGlobals qd;
#endif

/* These accessors are only defined in Carbon */
#ifndef GetPortTextFont
#define GetPortTextFont(port) (((CGrafPtr)(port))->txFont)
#endif

#ifndef GetPortTextFace
#define GetPortTextFace(port) ((Style)(((CGrafPtr)(port))->txFace))
#endif

#ifndef GetPortTextMode
#define GetPortTextMode(port) (((CGrafPtr)(port))->txMode)
#endif

#ifndef GetPortTextSize
#define GetPortTextSize(port) (((CGrafPtr)(port))->txSize)
#endif

#ifndef GetPortSpExtra
#define GetPortSpExtra(port) (((CGrafPtr)(port))->spExtra)
#endif

#endif

/* when built for old Toolbox access, we simulate some newer functions
 * using the old Toolbox structures */
#if USE_OLD_TOOLBOX

/* Look up traps by A-Trap constant */
static UniversalProcPtr maccurses_getTrapAddress(UInt16 aTrapNum)
{
    if (aTrapNum & 0x0800)
    {
        if ((aTrapNum & 0x0200)
            &&
            (NGetTrapAddress(_InitGraf & 0x3ff, ToolTrap)
              ==
             (NGetTrapAddress((_InitGraf ^ 0x0200) & 0x3ff, ToolTrap))))
        {
            aTrapNum = _Unimplemented;
        }
        return NGetTrapAddress(aTrapNum & 0x3ff, ToolTrap);
    }
    return NGetTrapAddress(aTrapNum & 0xff, OSTrap);
}

static UniversalProcPtr maccurses_trapUnimplemented;

#if ! (defined(TARGET_RT_MAC_CFM) && TARGET_RT_MAC_CFM || defined(__CFM68K__))

#ifndef ANCIENT_HEADERS
static UniversalProcPtr maccurses_trapMicroseconds;
#endif

static void maccurses_Microseconds(UnsignedWide *microTickCount)
{
#ifndef ANCIENT_HEADERS
    if (! maccurses_trapUnimplemented)
    {
        maccurses_trapUnimplemented = maccurses_getTrapAddress(_Unimplemented);
    }
    if (! maccurses_trapMicroseconds)
    {
        maccurses_trapMicroseconds = maccurses_getTrapAddress(_Microseconds);
    }
    if (maccurses_trapMicroseconds == maccurses_trapUnimplemented)
#endif
    {
#if defined(UNIVERSAL_INTERFACES_VERSION) && (UNIVERSAL_INTERFACES_VERSION >= 0x0300)
        *microTickCount = UInt64ToUnsignedWide(U64Divide(U64Multiply(U64SetU((UInt32) clock()),
                                                                     U64SetU(1000000L)),
                                                         U64Set(CLOCKS_PER_SEC),
                                                         NULL));
#else
        double microTickCount_double;

        microTickCount_double = (((double) clock()) * (1e6 / CLOCKS_PER_SEC));
        microTickCount->lo = (UInt32) fmod(microTickCount_double, 2e32);
        microTickCount->hi = (UInt32) ((microTickCount_double - (double) (microTickCount->lo)) / 2e32);
#endif
        return;
    }
#ifndef ANCIENT_HEADERS
    Microseconds(microTickCount);
#endif
}
#undef Microseconds
#define Microseconds maccurses_Microseconds

#endif /* ! (defined(TARGET_RT_MAC_CFM) && TARGET_RT_MAC_CFM || defined(__CFM68K__)) */

static UniversalProcPtr maccurses_trapWaitNextEvent;

static Boolean maccurses_WaitNextEvent(EventMask eventMask,
                                       EventRecord *theEvent,
                                       UInt32 sleep,
                                       RgnHandle mouseRgn)
{
    if (! maccurses_trapUnimplemented)
    {
        maccurses_trapUnimplemented = maccurses_getTrapAddress(_Unimplemented);
    }
    if (! maccurses_trapWaitNextEvent)
    {
        maccurses_trapWaitNextEvent = maccurses_getTrapAddress(_WaitNextEvent);
    }
    if (maccurses_trapWaitNextEvent == maccurses_trapUnimplemented)
    {
        while (1)
        {
            Boolean ret;

            SystemTask();
            ret = GetNextEvent(eventMask, theEvent);
            if ((ret != FALSE) || (theEvent->what != nullEvent))
            {
                return ret;
            }
            if (! sleep) return FALSE;
            Delay(1, NULL);
            sleep --;
        }
    }
    return WaitNextEvent(eventMask, theEvent, sleep, mouseRgn);
}
#undef WaitNextEvent
#define WaitNextEvent maccurses_WaitNextEvent

static UniversalProcPtr maccurses_trapAppearanceDispatch;

static Boolean maccurses_IsWindowCollapsed(WindowRef window)
{
    if (! maccurses_trapUnimplemented)
    {
        maccurses_trapUnimplemented = maccurses_getTrapAddress(_Unimplemented);
    }
    if (! maccurses_trapAppearanceDispatch)
    {
        maccurses_trapAppearanceDispatch = maccurses_getTrapAddress(_AppearanceDispatch);
    }
    if (maccurses_trapAppearanceDispatch == maccurses_trapUnimplemented)
    {
        return FALSE;
    }
    return IsWindowCollapsed(window);
}
#undef IsWindowCollapsed
#define IsWindowCollapsed maccurses_IsWindowCollapsed

static UniversalProcPtr maccurses_trapGestalt;

static OSErr maccurses_Gestalt(OSType selector, long *response)
{
    if (! maccurses_trapUnimplemented)
    {
        maccurses_trapUnimplemented = maccurses_getTrapAddress(_Unimplemented);
    }
    if (! maccurses_trapGestalt)
    {
        maccurses_trapGestalt = maccurses_getTrapAddress(_Gestalt);
    }
    if (maccurses_trapGestalt == maccurses_trapUnimplemented)
    {
        return gestaltUnknownErr;
    }
    return Gestalt(selector, response);
}
#undef Gestalt
#define Gestalt maccurses_Gestalt

#undef IsPortColor
#define IsPortColor maccurses_IsPortColor

static Boolean IsPortColor(CGrafPtr port)
{
    /* see About Color QuickDraw in Inside Macintosh */
    return (port->portVersion & 0xc000) ? TRUE : FALSE;
}

#undef GetRegionBounds
#define GetRegionBounds maccurses_GetRegionBounds

static Rect *
GetRegionBounds(RgnHandle region, Rect *bounds)
{
    if (bounds)
    {
        memcpy((void *) bounds, (void *) &((*region)->rgnBBox), sizeof(*bounds));
    }
    return bounds;
}

#undef SetWindowBounds
#define SetWindowBounds maccurses_SetWindowBounds

static OSStatus SetWindowBounds(WindowRef window,
                                WindowRegionCode regionCode,
                                const Rect *globalBounds)
{
    OSStatus ret = noErr;

    switch (regionCode)
    {
    case kWindowStructureRgn:
        RectRgn(((WindowPeek)window)->strucRgn, globalBounds);
        break;
    case kWindowContentRgn:
        RectRgn(((WindowPeek)window)->contRgn, globalBounds);
        break;
    default:
        ret = unimpErr;
    }
    return ret;
}

#undef GetQDGlobalsBlack
#define GetQDGlobalsBlack maccurses_GetQDGlobalsBlack

static Pattern *GetQDGlobalsBlack(Pattern *black)
{
    if (black)
    {
        memcpy((void *) black, (void*) &(qd.black), sizeof(Pattern));
    }
    return black;
}

#undef GetQDGlobalsScreenBits
#define GetQDGlobalsScreenBits maccurses_GetQDGlobalsScreenBits

static BitMap *GetQDGlobalsScreenBits(BitMap *screenBits)
{
    if (screenBits)
    {
        memcpy((void *) screenBits, (void*) &(qd.screenBits), sizeof(BitMap));
    }
    return screenBits;
}

#endif /* USE_OLD_TOOLBOX */

#ifndef __CARBON__

#if ! USE_OLD_TOOLBOX
static int maccurses_use_windowslib = 0;
#endif

static OSStatus
maccurses_GetWindowBounds(WindowRef window, WindowRegionCode regionCode, Rect *globalBounds)
{
    RgnHandle rgn;
    OSStatus ret = noErr;

#if ! USE_OLD_TOOLBOX
    if (maccurses_use_windowslib && GetWindowBounds)
    {
        return GetWindowBounds(window, regionCode, globalBounds);
    }
#endif
    rgn = NewRgn();
    switch (regionCode)
    {
    case kWindowStructureRgn:
        GetWindowStructureRgn(window, rgn);
        break;
    case kWindowContentRgn:
        GetWindowContentRgn(window, rgn);
        break;
    case kWindowUpdateRgn:
        GetWindowUpdateRgn(window, rgn);
        break;
    default:
        ret = unimpErr;
    }
    if (ret == noErr)
    {
        GetRegionBounds(rgn, globalBounds);
    }
    DisposeRgn(rgn);
    return ret;
}
#undef GetWindowBounds
#define GetWindowBounds(window, regionCode, globalBounds) maccurses_GetWindowBounds((window), (regionCode), (globalBounds))

static Boolean
maccurses_ResizeWindow(WindowRef window,
                       Point startPoint,
                       const Rect *sizeConstraints,
                       Rect *newContentRect)
{
    long newsize;

#if ! USE_OLD_TOOLBOX
    if (maccurses_use_windowslib && ResizeWindow)
    {
        return ResizeWindow(window, startPoint, sizeConstraints, newContentRect);
    }
#endif
    newsize = GrowWindow(window, startPoint, sizeConstraints);
    if (newsize)
    {
        SizeWindow(window, LoWord(newsize), HiWord(newsize), TRUE);
        GetWindowBounds(window, kWindowContentRgn, newContentRect);
        return TRUE;
    }
    return FALSE;
}
#undef ResizeWindow
#define ResizeWindow(window, startPoint, sizeConstraints, newContentRect) maccurses_ResizeWindow((window), (startPoint), (sizeConstraints), (newContentRect))

static OSStatus
maccurses_GetWindowStructureWidths(WindowRef window, Rect *rect)
{
    OSStatus ret;
    Rect strucRect, contRect;

#if ! USE_OLD_TOOLBOX
    if (maccurses_use_windowslib && GetWindowStructureWidths)
    {
        return GetWindowStructureWidths(window, rect);
    }
#endif
    ret = GetWindowBounds(window, kWindowStructureRgn, &strucRect);
    if (ret == noErr)
    {
        ret = GetWindowBounds(window, kWindowContentRgn, &contRect);
        if (ret == noErr)
        {
            rect->left = contRect.left - strucRect.left;
            rect->top = contRect.top - strucRect.top;
            rect->right = strucRect.right - contRect.right;
            rect->bottom = strucRect.bottom - contRect.bottom;
        }
    }
    return ret;
}
#undef GetWindowStructureWidths
#define GetWindowStructureWidths(window, rect) maccurses_GetWindowStructureWidths((window), (rect))

#endif /* ! defined(__CARBON__) */

#ifdef macintosh

#ifndef strcasecmp

#define strcasecmp(s1,s2) maccurses_strcasecmp(s1,s2)

int maccurses_strcasecmp(const char *s1, const char *s2)
{
    int i;
    int a, b;

    for (i = 0; s1[i] && s2[i] && (tolower(s1[i]) == tolower(s2[i])); i ++)
    {
        /* ... */
    }
    a = (unsigned char) s1[i];
    b = (unsigned char) s2[i];
    if (a) a = tolower(a);
    if (b) b = tolower(b);
    return (a == b) ? 0 : (a < b) ? -1 : 1;
}

#endif

#ifndef isatty

#if ! (defined(FIOFNAME) || defined(FIOINTERACTIVE))

#define isatty(fd) ((fd) != -1)

#else /* defined(FIOFNAME) || defined(FIOINTERACTIVE) */

#define isatty(fd) maccurses_isatty(fd)

int maccurses_isatty(int fd)
{
    long systemVersion = 0;

    if ((noErr == Gestalt(gestaltSystemVersion, &systemVersion))
        &&
        ((systemVersion & 0xffff) >= 0x0700))
    {
        char namebuf[512];

#ifdef FIOINTERACTIVE
        if (ioctl(fd, FIOINTERACTIVE, NULL) == 0)
        {
            return 1;
        }
#endif
        memset((void *) namebuf, 0, sizeof(namebuf));
#ifdef FIOFNAME
        if (ioctl(fd, FIOFNAME, (long *) (void *) namebuf) == -1)
        {
            return 0;
        }
#endif
        if ((! strcmp(namebuf, "dev:console"))
            ||
            (! strncmp(namebuf, "dev:tty", strlen("dev:tty")))
            ||
            (! strncmp(namebuf, "dev:pty", strlen("dev:pty"))))
        {
            return 1;
        }
    }
    return 0;
}

#endif /* defined(FIOFNAME) || defined(FIOINTERACTIVE) */

#endif

#ifndef access

#define access(fn,type) maccurses_access(fn)

static int maccurses_access(const char *fn)
{
    FILE *fp;

    fp = fopen(fn, "rb");
    if (fp)
    {
        fclose(fp);
        return 0;
    }
    return -1;
}

#endif

#ifndef gettimeofday

#define gettimeofday(tp,tzp) maccurses_gettimeofday(tp,tzp)

struct timeval
{
    long tv_sec;
    long tv_usec;
};

struct timezone
{
    int tz_minuteswest;
    int tz_dsttime;
};

static int maccurses_gettimeofday(struct timeval *tp, struct timezone *tzp)
{
    UnsignedWide microTickCount;
    double usecs;

    Microseconds(&microTickCount);
    usecs = (4294967296.0 * microTickCount.hi + microTickCount.lo);
    if (tp)
    {
        tp->tv_sec = (long) (usecs / 1e6);
        tp->tv_usec = (long) (usecs - (1e6 * tp->tv_sec));
    }
    if (tzp)
    {
        tzp->tz_minuteswest = 0;
        tzp->tz_dsttime = 0;
    }
    return 0;
}

#endif

#ifndef usleep

#define usleep(us) maccurses_usleep(us)

static int maccurses_usleep(unsigned long us)
{
    EventRecord er;
    struct timeval tv, tv2;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    myman_gettimeofday(&tv, NULL);
    tv2.tv_sec = tv.tv_sec + ((tv.tv_usec + us) / 1000000L);
    tv2.tv_usec = tv.tv_usec + ((tv.tv_usec + us) % 1000000L);
    while (1)
    {
        SpinCursor(32);
        if (WaitNextEvent(highLevelEventMask, &er, us / 16667, NULL))
        {
            if (er.what == kHighLevelEvent)
            {
                AEProcessAppleEvent(&er);
            }
            return -1;
        }
        myman_gettimeofday(&tv, NULL);
        if ((tv.tv_sec != tv2.tv_sec)
            ||
            (tv.tv_usec >= tv2.tv_usec))
        {
            break;
        }
        break;
    }
    return 0;
}

#endif

#endif /* macintosh */

#if USE_FONT_MANAGER_9

static int maccurses_use_font_manager_9 = 0;

static OSStatus maccurses_FMGetFontFamilyName(FMFontFamily family, Str255 name)
{
    if (maccurses_use_font_manager_9 && FMGetFontFamilyName)
    {
        return FMGetFontFamilyName(family, name);
    }
    GetFontName(family, name);
    return noErr;
}
#undef FMGetFontFamilyName
#define FMGetFontFamilyName(family, name) maccurses_FMGetFontFamilyName((family), (name))

static FMFontFamily maccurses_FMGetFontFamilyFromName(ConstStr255Param name)
{
    short familyID;

    if (maccurses_use_font_manager_9 && FMGetFontFamilyFromName)
    {
        return FMGetFontFamilyFromName(name);
    }
    GetFNum(name, &familyID);
    if (! familyID)
    {
        Str255 nameCanon;
        int i;

        GetFontName(familyID, nameCanon);
        if (name[0] != nameCanon[0])
        {
            return kInvalidFontFamily;
        }
        for (i = 0; i <= nameCanon[0]; i ++)
        {
            if (tolower(name[i]) != tolower(nameCanon[i]))
            {
                return kInvalidFontFamily;
            }
        }
    }
    return (FMFontFamily) familyID;
}
#undef FMGetFontFamilyFromName
#define FMGetFontFamilyFromName(name) maccurses_FMGetFontFamilyFromName(name)

static OSStatus
maccurses_FMGetFontFamilyTextEncoding(FMFontFamily family, TextEncoding *encoding)
{
    if (maccurses_use_font_manager_9 && FMGetFontFamilyTextEncoding)
    {
        return FMGetFontFamilyTextEncoding(family, encoding);
    }
    return kFMInvalidFontFamilyErr;
}
#undef FMGetFontFamilyTextEncoding
#define FMGetFontFamilyTextEncoding(family, encoding) maccurses_FMGetFontFamilyTextEncoding((family), (encoding))

#else /* ! USE_FONT_MANAGER_9 */

/* When built without Font Manager 9, we use macros to replace some
 * calls */

#ifndef kInvalidFontFamily
#define kInvalidFontFamily -1
#endif

#ifndef kFMInvalidFontFamilyErr
#define kFMInvalidFontFamilyErr -981L
#endif

#ifndef FMFontFamily
#define FMFontFamily SInt16
#endif

#ifndef FMFontStyle
#define FMFontStyle SInt16
#endif

#ifndef FMFontSize
#define FMFontSize SInt16
#endif

#ifndef FMGetFontFamilyFromName
#define FMGetFontFamilyFromName maccurses_FMGetFontFamilyFromName

const char *maccurses_standardFontNames =
    "(System Font)" "\0"
    "(Application Font)" "\0"
    "New York" "\0"
    "Geneva" "\0"
    "Monaco" "\0"
    "Venice" "\0"
    "London" "\0"
    "Athens" "\0"
    "San Francisco" "\0"
    "Toronto" "\0"
    "(Font 10)" "\0"
    "Cairo" "\0"
    "Los Angeles" "\0"
    "(Font 13)" "\0"
    "(Font 14)" "\0"
    "(Font 15)" "\0"
    "(Font 16)" "\0"
    "(Font 17)" "\0"
    "(Font 18)" "\0"
    "(Font 19)" "\0"
    "Times" "\0"
    "Helvetica" "\0"
    "Courier" "\0"
    "Symbol" "\0"
    "Mobile" "\0"
    "\0\0";

static UniversalProcPtr maccurses_trapGetFNum;

static UniversalProcPtr maccurses_trapGetFName;

static FMFontFamily FMGetFontFamilyFromName(ConstStr255Param name)
{
    short familyID;

    if (! maccurses_trapUnimplemented)
    {
        maccurses_trapUnimplemented = maccurses_getTrapAddress(_Unimplemented);
    }
    if (! maccurses_trapGetFNum)
    {
        maccurses_trapGetFNum = maccurses_getTrapAddress(_GetFNum);
    }
    if (! maccurses_trapGetFName)
    {
        maccurses_trapGetFName = maccurses_getTrapAddress(_GetFName);
    }
    if (maccurses_trapGetFNum == maccurses_trapUnimplemented)
    {
        familyID = 0;
    }
    else
    {
        GetFNum(name, &familyID);
    }
    if (! familyID)
    {
        int i, j, k, m;

        for (i = 0, j = 0, k = 0, m = 1;
             maccurses_standardFontNames[j]
                 ||
                 maccurses_standardFontNames[j + 1];
             ++ j)
        {
            if (! maccurses_standardFontNames[j])
            {
                if ((name[0] == (j - k)) && m)
                {
                    familyID = i;
                    break;
                }
                ++ i;
                k = j;
                m = 1;
            }
            else if (m)
            {
                if ((name[0] < (j - k))
                    ||
                    (tolower(name[j - k])
                     !=
                     tolower(maccurses_standardFontNames[j])))
                {
                    m = 0;
                }
            }
        }
    }
    if (maccurses_trapGetFName == maccurses_trapUnimplemented)
    {
        return kInvalidFontFamily;
    }
    if (! familyID)
    {
        Str255 nameCanon;
        int i;

        GetFontName(familyID, nameCanon);
        if (name[0] != nameCanon[0])
        {
            return kInvalidFontFamily;
        }
        for (i = 0; i <= nameCanon[0]; i ++)
        {
            if (tolower(name[i]) != tolower(nameCanon[i]))
            {
                return kInvalidFontFamily;
            }
        }
    }
    return (FMFontFamily) familyID;
}
#endif

#ifndef FMGetFontFamilyName
#define FMGetFontFamilyName maccurses_FMGetFontFamilyName
static OSStatus FMGetFontFamilyName(FMFontFamily family, Str255 name)
{
    GetFontName(family, name);
    return noErr;
}
#endif

#undef FMGetFontFamilyTextEncoding
#define FMGetFontFamilyTextEncoding(family, encoding) kFMInvalidFontFamilyErr

#endif /* ! USE_FONT_MANAGER_9 */

static OSStatus
maccurses_GetFontFamilyFromMenuSelection(MenuRef menu, MenuItemIndex item, FMFontFamily *outFontFamily, FMFontStyle *outStyle)
{
    Str255 fontName;
    FMFontFamily tmpff;

#ifdef __CARBON__
    if (GetFontFamilyFromMenuSelection)
    {
        return GetFontFamilyFromMenuSelection(menu, item, outFontFamily, outStyle);
    }
#endif
    GetMenuItemText(menu, item, fontName);
    tmpff = FMGetFontFamilyFromName(fontName);
    if (tmpff != kInvalidFontFamily)
    {
        *outFontFamily = tmpff;
        *outStyle = 0;
        return noErr;
    }
    return kFMInvalidFontFamilyErr;
}
#undef GetFontFamilyFromMenuSelection
#define GetFontFamilyFromMenuSelection(menu, item, outFontFamily, outStyle) maccurses_GetFontFamilyFromMenuSelection((menu), (item), (outFontFamily), (outStyle))

#ifndef __CARBON__

#undef GetPortBitMapForCopyBits
#define GetPortBitMapForCopyBits maccurses_GetPortBitMapForCopyBits
static const BitMap *GetPortBitMapForCopyBits(CGrafPtr port)
{
    return (const BitMap *) &(((struct GrafPort *) (void *) port)->portBits);
}

#ifndef CountMenuItems
#define CountMenuItems(mr) CountMItems(mr)
#endif

#endif /* ! defined(__CARBON__) */

#define ERR -1
#define OK 0

typedef long maccurses_chtype;
typedef maccurses_chtype maccurses_attr_t;

#undef KEY_RESIZE
#define KEY_RESIZE 0x100001L

#undef KEY_UP
#define KEY_UP 0x100002L

#undef KEY_DOWN
#define KEY_DOWN 0x100003L

#undef KEY_LEFT
#define KEY_LEFT 0x100004L

#undef KEY_RIGHT
#define KEY_RIGHT 0x100005L

#undef A_REVERSE
#define A_REVERSE 0x00800000L

#undef A_UNDERLINE
#define A_UNDERLINE 0x00400000L

#undef A_BOLD
#define A_BOLD 0x00200000L

#undef A_STANDOUT
#define A_STANDOUT A_REVERSE

#undef _MACCURSES_A_COLOR
#define _MACCURSES_A_COLOR 0xff000000L

/* note that this is not a bitflag -- instead, it's a prefix for the
 * non-Unicode "plane 31" which is a copy of the BMP (plane 0) in "ACS
 * mode". */
#undef _MACCURSES_A_ALTCHARSET
#define _MACCURSES_A_ALTCHARSET 0x1f0000L

#undef _MACCURSES_A_CHARTEXT
#define _MACCURSES_A_CHARTEXT 0x1fffffL

#undef _MACCURSES_A_ATTR
#define _MACCURSES_A_ATTR (~_MACCURSES_A_CHARTEXT)

#define _PAIR_SHIFT 24

#define COLORS 16

#define COLOR_PAIRS (COLORS*COLORS)

#define COLOR_BLACK 0

#define COLOR_RED 1

#define COLOR_GREEN 2

#define COLOR_YELLOW 3

#define COLOR_BLUE 4

#define COLOR_MAGENTA 5

#define COLOR_CYAN 6

#define COLOR_WHITE 7

#define PEN_BRIGHT 8

#define _COLOR_TO_MAC(c, dfl) \
                          (((c) == COLOR_BLACK) ? blackColor : \
                          ((c) == COLOR_RED) ? redColor : \
                          ((c) == COLOR_GREEN) ? greenColor : \
                          ((c) == COLOR_YELLOW) ? yellowColor : \
                          ((c) == COLOR_BLUE) ? blueColor : \
                          ((c) == COLOR_MAGENTA) ? magentaColor : \
                          ((c) == COLOR_CYAN) ? cyanColor : \
                          ((c) == COLOR_WHITE) ? whiteColor : \
                          (dfl))

#ifdef UNICODE

#define ACS_LRCORNER (0x2518 | _MACCURSES_A_ALTCHARSET)

#define ACS_URCORNER (0x2510 | _MACCURSES_A_ALTCHARSET)

#define ACS_ULCORNER (0x250c | _MACCURSES_A_ALTCHARSET)

#define ACS_LLCORNER (0x2514 | _MACCURSES_A_ALTCHARSET)

#define ACS_PLUS (0x253c | _MACCURSES_A_ALTCHARSET)

#define ACS_HLINE (0x2500 | _MACCURSES_A_ALTCHARSET)

#define ACS_LTEE (0x251c | _MACCURSES_A_ALTCHARSET)

#define ACS_RTEE (0x2524 | _MACCURSES_A_ALTCHARSET)

#define ACS_BTEE (0x2534 | _MACCURSES_A_ALTCHARSET)

#define ACS_TTEE (0x252c | _MACCURSES_A_ALTCHARSET)

#define ACS_VLINE (0x2502 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_MIDDOT (0x00b7 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_PUCE (0x2022 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_BLACKSQUARE (0x25A0 | _MACCURSES_A_ALTCHARSET)

#define ACS_BLOCK (0x2588 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_THALF (0x2580 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_BHALF (0x2584 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_LHALF (0x258c | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_RHALF (0x2590 | _MACCURSES_A_ALTCHARSET)

#else

#define ACS_LRCORNER (0xd9 | _MACCURSES_A_ALTCHARSET)

#define ACS_URCORNER (0xbf | _MACCURSES_A_ALTCHARSET)

#define ACS_ULCORNER (0xda | _MACCURSES_A_ALTCHARSET)

#define ACS_LLCORNER (0xc0 | _MACCURSES_A_ALTCHARSET)

#define ACS_PLUS (0xc5 | _MACCURSES_A_ALTCHARSET)

#define ACS_HLINE (0xc4 | _MACCURSES_A_ALTCHARSET)

#define ACS_LTEE (0xc3 | _MACCURSES_A_ALTCHARSET)

#define ACS_RTEE (0xb4 | _MACCURSES_A_ALTCHARSET)

#define ACS_BTEE (0xc1 | _MACCURSES_A_ALTCHARSET)

#define ACS_TTEE (0xc2 | _MACCURSES_A_ALTCHARSET)

#define ACS_VLINE (0xb3 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_MIDDOT (0xf9 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_PUCE (0xfa | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_BLACKSQUARE (0xfe | _MACCURSES_A_ALTCHARSET)

#define ACS_BLOCK (0xdb | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_THALF (0xdf | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_BHALF (0xdc | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_LHALF (0xdd | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_RHALF (0xde | _MACCURSES_A_ALTCHARSET)

#endif

#define ACS_BULLET _MACCURSES_ACS_MIDDOT

#ifndef SWAPDOTS
#define SWAPDOTS 1
#endif

#define MACCURSES_FOUR_CHAR_CODE(k) ((((long)((k)[0])) << 24) | ((255 & (long)(unsigned)((k)[1])) << 16) | ((255 & (unsigned)((k)[2])) << 8) | (255 & (unsigned)((k)[3])))

/* an arbitrary integer identifying "Da Window" */
#define MACCURSES_MAINWIN 0xda317d03LU

/* somewhat arbitrary menu identifiers */
#define MACCURSES_F8FFMENU 0x0080L
#define MACCURSES_FILEMENU 0x0a01L
#define MACCURSES_EDITMENU 0x0a02L
#define MACCURSES_SIZEMENU 0x0a03L
#define MACCURSES_FONTMENU 0x0f00L /* this and all higher codes are used by the font (sub-)menus */

static int maccurses_after_endwin = 1;
static int maccurses_x = 0;
static int maccurses_y = 0;

static maccurses_attr_t maccurses_attr = 0;

static int maccurses_w = 80;
static int maccurses_h = 25;

static int maccurses_new_w = 0;
static int maccurses_new_h = 0;

static int maccurses_resize_pending = 0;

#ifndef _MACCURSES_RESIZE_TIMER
#define _MACCURSES_RESIZE_TIMER 10
#endif

static int maccurses_use_acs_flag = TRUE;

static int maccurses_clearok_flag = FALSE;

static int maccurses_nodelay_flag = FALSE;

static int maccurses_intrflush_flag = FALSE;

static int maccurses_idlok_flag = FALSE;

static int maccurses_leaveok_flag = FALSE;

static int maccurses_wide_flag = FALSE;

static int maccurses_widefont_flag = FALSE;

static int maccurses_debug_flag = FALSE;

#ifndef UNICODE

static maccurses_chtype *maccurses_cvtt = 0;

static maccurses_chtype maccurses_macvt100[] = {
    0x80, 0x82, 0x81, 0x9f, 0x82, 0x8e, 0x83, 0x89,
    0x84, 0x8a, 0x85, 0x88, 0x86, 0x8c, 0x87, 0x8d,
    0x88, 0x90, 0x89, 0x91, 0x8a, 0x8f, 0x8b, 0x95,
    0x8c, 0x94, 0x8d, 0x93, 0x8e, 0x80, 0x8f, 0x81,
    0x90, 0x83, 0x91, 0xbe, 0x92, 0xae, 0x93, 0x99,
    0x94, 0x9a, 0x95, 0x98, 0x96, 0x9e, 0x97, 0x9d,
    0x98, 0xd8, 0x99, 0x85, 0x9a, 0x86, 0x9b, 0xa2,
    0x9c, 0xa3, 0x9d, 0xb4, 0x9f, 0xc4, 0xa0, 0x87,
    0xa1, 0x92, 0xa2, 0x97, 0xa3, 0x9c, 0xa4, 0x96,
    0xa5, 0x84, 0xa6, 0xbb, 0xa7, 0xbc, 0xa8, 0xc0,
    0xaa, 0xc2, 0xab, 0xc3, 0xac, 0xc5, 0xad, 0xc1,
    0xae, 0xc7, 0xaf, 0xc8, 0xb1, 0xbd, 0xb3, 0xfb,
    0xb4, 0xf8, 0xbf, 0xd3, 0xc0, 0xd5, 0xc1, 0xf9,
    0xc2, 0xfa, 0xc3, 0xf7, 0xc4, 0xe4, 0xc5, 0xda,
    0xd9, 0xd2, 0xda, 0xd4, 0xe1, 0xa7, 0xe3, 0xb9,
    0xe6, 0xb5, 0xf1, 0xb1, 0xf2, 0xb3, 0xf3, 0xb2,
    0xf6, 0xd6, 0xf8, 0xa1, 0xfa, 0xe1, 0xfd, 0xb7,
    0, 0
};
static maccurses_chtype maccurses_macjapanese[] = {
    0x01, 0x8199, 0x02, 0x819a, 0x03, 0x86a5, 0x04, 0x86a6,
    0x05, 0x86a4, 0x06, 0x86a3, 0x07, 0x819c, 0x08, 0x819c | A_REVERSE,
    0x09, 0x819b, 0x0a, 0x819b | A_REVERSE, 0x0b, 0x8189, 0x0c, 0x818a,
    0x0d, 0x81f4, 0x14, 0x81f7, 0x15, 0x8198,
    0x18, 0x81aa, 0x19, 0x81ab, 0x1a, 0x81a8, 0x1b, 0x81a9,
    0x1c, 0x8841, 0x1d, 0x81cc, 0x1e, 0x81a3, 0x1f, 0x81a5,
    /* start fullwidth additions */
    0x120, 0x8140, 0x121, 0x8149, 0x122, 0x8168, 0x123, 0x8194,
    0x124, 0x8190, 0x125, 0x8193, 0x126, 0x8195, 0x127, 0x8166,
    0x128, 0x8169, 0x129, 0x816a, 0x12a, 0x8196, 0x12b, 0x817b,
    0x12c, 0x8143, 0x12d, 0x815d, 0x12e, 0x8144, 0x12f, 0x815e,
    0x130, 0x824f, 0x131, 0x8250, 0x132, 0x8251, 0x133, 0x8252,
    0x134, 0x8253, 0x135, 0x8254, 0x136, 0x8255, 0x137, 0x8256,
    0x138, 0x8257, 0x139, 0x8258, 0x13a, 0x8146, 0x13b, 0x8147,
    0x13c, 0x8183, 0x13d, 0x8181, 0x13e, 0x8184, 0x13f, 0x8148,
    0x140, 0x8197, 0x141, 0x8260, 0x142, 0x8261, 0x143, 0x8262,
    0x144, 0x8263, 0x145, 0x8264, 0x146, 0x8265, 0x147, 0x8266,
    0x148, 0x8267, 0x149, 0x8268, 0x14a, 0x8269, 0x14b, 0x826a,
    0x14c, 0x826b, 0x14d, 0x826c, 0x14e, 0x826d, 0x14f, 0x826e,
    0x150, 0x826f, 0x151, 0x8270, 0x152, 0x8271, 0x153, 0x8272,
    0x154, 0x8273, 0x155, 0x8274, 0x156, 0x8275, 0x157, 0x8276,
    0x158, 0x8277, 0x159, 0x8278, 0x15a, 0x8279, 0x15b, 0x816d,
    0x15c, 0x815f, 0x15d, 0x816e, 0x15e, 0x81c8, 0x15f, 0x8151,
    0x160, 0x814d, 0x161, 0x8281, 0x162, 0x8282, 0x163, 0x8283,
    0x164, 0x8284, 0x165, 0x8285, 0x166, 0x8286, 0x167, 0x8287,
    0x168, 0x8288, 0x169, 0x8289, 0x16a, 0x828a, 0x16b, 0x828b,
    0x16c, 0x828c, 0x16d, 0x828d, 0x16e, 0x828e, 0x16f, 0x828f,
    0x170, 0x8290, 0x171, 0x8291, 0x172, 0x8292, 0x173, 0x8293,
    0x174, 0x8294, 0x175, 0x8295, 0x176, 0x8296, 0x177, 0x8297,
    0x178, 0x8298, 0x179, 0x8299, 0x17a, 0x829a, 0x17b, 0x816f,
    0x17c, 0x8162, 0x17d, 0x8170, 0x17e, 0x8160, 0x19d, 0x818f,
    0x1db, 0x8140 | A_REVERSE, 0x1ff, 0x8140,
    /* end fullwidth additions */
    0x5c,   0x80,
    0x7f, 0x83a2,
    0x9b, 0x8191, 0x9c, 0x8192, 0x9d,   0x5c,
    0xaa, 0x81ca, 0xae, 0x8173, 0xaf, 0x8174,
    0xb3, 0x84a0, 0xb4, 0x84a7, 0xb5, 0x84bc, 0xb6, 0x84b7,
    0xb9, 0x84b2, 0xba, 0x84ab, 0xbb, 0x84ad, 0xbc, 0x84ae,
    0xbf, 0x84a2, 0xc0, 0x84a4, 0xc1, 0x84a8, 0xc2, 0x84a6,
    0xc3, 0x84a5, 0xc4, 0x849f, 0xc5, 0x84a9, 0xc6, 0x84ba,
    0xc7, 0x84b5, 0xc8, 0x84af, 0xc9, 0x84ac, 0xca, 0x84b3,
    0xcb, 0x84b1, 0xcc, 0x84b0, 0xcd, 0x84aa, 0xce, 0x84b4,
    0xcf, 0x84b8, 0xd0, 0x84bd, 0xd1, 0x84b6, 0xd2, 0x84bb,
    0xd7, 0x84be, 0xd8, 0x84b9, 0xd9, 0x84a3, 0xda, 0x84a1,
    0xdb, 0xa0 | A_REVERSE,
    0xe0, 0x83bf, 0xe1, 0x83c0, 0xe2, 0x83a1, 0xe3, 0x83ce,
    0xe4, 0x83b0, 0xe5, 0x83d0, 0xe6, 0x83ca, 0xe7, 0x83d1,
    0xe8, 0x83b3, 0xe9, 0x83a6, 0xea, 0x83b6, 0xeb, 0x83c2,
    0xec, 0x8187, 0xed, 0x83d3, 0xee, 0x83c3, 0xef, 0x81bf,
    0xf0, 0x81df, 0xf1, 0x817d, 0xf2, 0x8186, 0xf3, 0x8185,
    0xf4, 0x81e7, 0xf5, 0x81e7, 0xf6, 0x8180, 0xf8, 0x818b,
    0xf8, 0x818b, 0xf9, 0x8145, 0xfa, 0x819c, 0xfb, 0x81e3,
    0xfe, 0x81a1, 0xff,   0xa0,
    0, 0
};
static maccurses_chtype maccurses_macchinesetrad[] = {
    0x01, 0xa1b8, 0x02, 0xa1b9, 0x04, 0xa1bb, 0x08, 0xa1b4 | A_REVERSE,
    0x09, 0xa1b3, 0x0a, 0xa1b3 | A_REVERSE, 0x0b, 0xa1f1, 0x0c, 0xa1f0,
    0x0f, 0xa1b3, 0x15, 0xa1b1, 0x18, 0xa1f4, 0x19, 0xa1f5,
    0x1a, 0xa1f7, 0x1b, 0xa1f6, 0x1c, 0xa1e8, 0x1e, 0xa1b6, 0x1f, 0xa1bf,
    /* start fullwidth additions */
    0x120, 0xa140, 0x121, 0xa149, 0x122, 0xa1a8, 0x123, 0xa1ad,
    0x124, 0xa243, 0x125, 0xa248, 0x126, 0xa1ae, 0x127, 0xa1a6,
    0x128, 0xa15d, 0x129, 0xa15e, 0x12a, 0xa1af, 0x12b, 0xa1cf,
    0x12c, 0xa141, 0x12d, 0xa1d0, 0x12e, 0xa144, 0x12f, 0xa241,
    0x130, 0xa2af, 0x131, 0xa2b0, 0x132, 0xa2b1, 0x133, 0xa2b2,
    0x134, 0xa2b3, 0x135, 0xa2b4, 0x136, 0xa2b5, 0x137, 0xa2b6,
    0x138, 0xa2b7, 0x139, 0xa2b8, 0x13a, 0xa147, 0x13b, 0xa146,
    0x13c, 0xa1d5, 0x13d, 0xa1d7, 0x13e, 0xa1d6, 0x13f, 0xa148,
    0x140, 0xa249, 0x141, 0xa2cf, 0x142, 0xa2d0, 0x143, 0xa2d1,
    0x144, 0xa2d2, 0x145, 0xa2d3, 0x146, 0xa2d4, 0x147, 0xa2d5,
    0x148, 0xa2d6, 0x149, 0xa2d7, 0x14a, 0xa2d8, 0x14b, 0xa2d9,
    0x14c, 0xa2da, 0x14d, 0xa2db, 0x14e, 0xa2dc, 0x14f, 0xa2dd,
    0x150, 0xa2de, 0x151, 0xa2df, 0x152, 0xa2e0, 0x153, 0xa2e1,
    0x154, 0xa2e2, 0x155, 0xa2e3, 0x156, 0xa2e4, 0x157, 0xa2e5,
    0x158, 0xa2e6, 0x159, 0xa2e7, 0x15a, 0xa2e8, 0x15b, 0xa165,
    0x15c, 0xa242, 0x15d, 0xa166, 0x15e, 0xa173, 0x15f, 0xa1c4,
    0x160, 0xa1a5, 0x161, 0xa2e9, 0x162, 0xa2ea, 0x163, 0xa2eb,
    0x164, 0xa2ec, 0x165, 0xa2ed, 0x166, 0xa2ee, 0x167, 0xa2ef,
    0x168, 0xa2f0, 0x169, 0xa2f1, 0x16a, 0xa2f2, 0x16b, 0xa2f3,
    0x16c, 0xa2f4, 0x16d, 0xa2f5, 0x16e, 0xa2f6, 0x16f, 0xa2f7,
    0x170, 0xa2f8, 0x171, 0xa2f9, 0x172, 0xa2fa, 0x173, 0xa2fb,
    0x174, 0xa2fc, 0x175, 0xa2fd, 0x176, 0xa2fe, 0x177, 0xa340,
    0x178, 0xa341, 0x179, 0xa342, 0x17a, 0xa343, 0x17b, 0xa161,
    0x17c, 0xa155, 0x17d, 0xa162, 0x17e, 0xa1ca, 0x1ff, 0xa140,
    /* end fullwidth additions */
    0x7f, 0xa347, 0x9b, 0xa246, 0x9c, 0xa247, 0x9d, 0xa244,
    0xae, 0xa16d, 0xaf, 0xa16e, 0xb3, 0xa278, 0xb4, 0xa274,
    0xbf, /*0xa27b*/0xa2a1, 0xc0, /*0xa27c*/0xa2a2,
    0xc1, 0xa272, 0xc2, 0xa273, 0xc3, 0xa275, 0xc4, 0xa277,
    0xc5, 0xa271,
    0xd9, /*0xa27d*/0xa2a3, 0xda, /*0xa27a*/0xa27e, 0xdb, 0xa269,
    0xdc, 0xa265, 0xdd, 0xa26d, 0xde, 0xa26d | A_REVERSE,
    0xdf, 0xa265 | A_REVERSE,
    0xe0, 0xa35c, 0xe1, 0xa35d, 0xe2, 0xa346, 0xe3, 0xa36b,
    0xe4, 0xa355, 0xe5, 0xa36d, 0xe6, 0xa367, 0xe7, 0xa36e,
    0xe8, 0xa358, 0xe9, 0xa34b, 0xea, 0xa35b, 0xeb, 0xa35f,
    0xec, 0xa1db, 0xed, 0xa370, 0xee, 0xa360, 0xef, 0xa1e4,
    0xf0, 0xa1dd, 0xf1, 0xa1d3, 0xf2, 0xa1d9, 0xf3, 0xa1d8,
    0xf4, 0xa1ec, 0xf5, 0xa1ec, 0xf6, 0xa1d2, 0xf8, 0xa258,
    0xf9, 0xa145, 0xfa, 0xa1b4, 0xfb, 0xa1d4, 0xfe, 0xa1b4,
    0, 0
};
static maccurses_chtype maccurses_macchinesesimp[] = {
    0x01, 0xa1ee, 0x02, 0xa1ef, 0x04, 0xa1f4, 0x07, 0xa1f1,
    0x08, 0xa1f1 | A_REVERSE, 0x09, 0xa1f0, 0x0a,
    0xa1f0 | A_REVERSE, 0x0b, 0xa1e1, 0x0c, 0xa1e2, 0x15, 0xa1ec,
    0x18, 0xa1fc, 0x19, 0xa1fd, 0x1a, 0xa1fa, 0x1b, 0xa1fb,
    0x1e, 0xa1f8,
    /* start fullwidth additions */
    0x120, 0xa1a1, 0x121, 0xa3a1, 0x122, 0xa3a2, 0x123, 0xa3a3,
    0x124, 0xa1e7, 0x125, 0xa3a5, 0x126, 0xa3a6, 0x127, 0xa3a7,
    0x128, 0xa3a8, 0x129, 0xa3a9, 0x12a, 0xa3aa, 0x12b, 0xa3ab,
    0x12c, 0xa3ac, 0x12d, 0xa3ad, 0x12e, 0xa3ae, 0x12f, 0xa3af,
    0x130, 0xa3b0, 0x131, 0xa3b1, 0x132, 0xa3b2, 0x133, 0xa3b3,
    0x134, 0xa3b4, 0x135, 0xa3b5, 0x136, 0xa3b6, 0x137, 0xa3b7,
    0x138, 0xa3b8, 0x139, 0xa3b9, 0x13a, 0xa3ba, 0x13b, 0xa3bb,
    0x13c, 0xa3bc, 0x13d, 0xa3bd, 0x13e, 0xa3be, 0x13f, 0xa3bf,
    0x140, 0xa3c0, 0x141, 0xa3c1, 0x142, 0xa3c2, 0x143, 0xa3c3,
    0x144, 0xa3c4, 0x145, 0xa3c5, 0x146, 0xa3c6, 0x147, 0xa3c7,
    0x148, 0xa3c8, 0x149, 0xa3c9, 0x14a, 0xa3ca, 0x14b, 0xa3cb,
    0x14c, 0xa3cc, 0x14d, 0xa3cd, 0x14e, 0xa3ce, 0x14f, 0xa3cf,
    0x150, 0xa3d0, 0x151, 0xa3d1, 0x152, 0xa3d2, 0x153, 0xa3d3,
    0x154, 0xa3d4, 0x155, 0xa3d5, 0x156, 0xa3d6, 0x157, 0xa3d7,
    0x158, 0xa3d8, 0x159, 0xa3d9, 0x15a, 0xa3da, 0x15b, 0xa3db,
    0x15c, 0xa3dc, 0x15d, 0xa3dd, 0x15e, 0xa3de, 0x15f, 0xa3df,
    0x160, 0xa3e0, 0x161, 0xa3e1, 0x162, 0xa3e2, 0x163, 0xa3e3,
    0x164, 0xa3e4, 0x165, 0xa3e5, 0x166, 0xa3e6, 0x167, 0xa3e7,
    0x168, 0xa3e8, 0x169, 0xa3e9, 0x16a, 0xa3ea, 0x16b, 0xa3eb,
    0x16c, 0xa3ec, 0x16d, 0xa3ed, 0x16e, 0xa3ee, 0x16f, 0xa3ef,
    0x170, 0xa3f0, 0x171, 0xa3f1, 0x172, 0xa3f2, 0x173, 0xa3f3,
    0x174, 0xa3f4, 0x175, 0xa3f5, 0x176, 0xa3f6, 0x177, 0xa3f7,
    0x178, 0xa3f8, 0x179, 0xa3f9, 0x17a, 0xa3fa, 0x17b, 0xa3fb,
    0x17c, 0xa3fc, 0x17d, 0xa3fd, 0x17e, 0xa1d7,
    0x1db, 0xa1a1 | A_REVERSE, 0x1ff, 0xa1a1,
    /* end fullwidth additions */
    0x7f, 0xa1f7, 0x81, 0xa8b9, 0x82, 0xa8a6, 0x85, 0xa8a4,
    0x88, 0xa8ba, 0x8d, 0xa8ac, 0x95, 0xa8b0, 0x97, 0xa8b4,
    0x9b, 0xa1e9, 0x9c, 0xa1ea, 0x9d, 0xa3a4,
    0xa0, 0xa8a2, 0xa1, 0xa8aa, 0xa2, 0xa8ae, 0xa3, 0xa8b2,
    0xae, 0xa1b6, 0xaf, 0xa1b7, 0xb3, 0xa9a6,
    0xb4, 0xa9c8, 0xb5, 0xa9c9, 0xb6, 0xa9cc, 0xb7, /*0xa9b6*/0xa9cb,
    0xb8, /*0xa9b5*/0xa9d1, 0xb9, 0xa9cf, 0xba, 0xa9a7, 0xbb, 0xa9b7,
    0xbc, 0xa9bf, 0xbd, /*0xa9be*/0xa9ca, 0xbe, /*0xa9bd*/0xa9d9, 0xbf, 0xa9b4,
    0xc0, 0xa9b8, 0xc1, 0xa9d8, 0xc2, 0xa9d0, 0xc3, 0xa9c0,
    0xc4, 0xa9a4, 0xc5, 0xa9e0, 0xc6, 0xa9c1, 0xc7, 0xa9c4,
    0xc8, 0xa9bb, 0xc9, 0xa9b3, 0xca, 0xa9df, 0xcb, 0xa9d7,
    0xcc, 0xa9c7, 0xcd, 0xa9a5, 0xce, 0xa9ef, 0xcf, 0xa9db,
    0xd0, 0xa9dc, 0xd1, 0xa9d3, 0xd2, 0xa9d4, 0xd3, /*0xa9ba*/0xa9c2,
    0xd4, /*0xa9b9*/0xa9da, 0xd5, /*0xa9b1*/0xa9d2, 0xd6, /*0xa9b2*/0xa9c3, 0xd7, 0xa9e6,
    0xd8, 0xa9e3, 0xd9, 0xa9bc, 0xda, 0xa9b0, 0xdb, 0x20 | A_REVERSE,
    0xe0, 0xa6c1, 0xe1, 0xa6c2, 0xe2, 0xa6a3, 0xe3, 0xa6d0,
    0xe4, 0xa6b2, 0xe5, 0xa6d2, 0xe6, 0xa6cc, 0xe7, 0xa6d3,
    0xe8, 0xa6b5, 0xe9, 0xa6a8, 0xea, 0xa6b8, 0xeb, 0xa6c4,
    0xec, 0xa1de, 0xed, 0xa6d5, 0xee, 0xa6c5, 0xef, 0xa1c9,
    0xf0, 0xa1d4, 0xf1, 0xa1c0, 0xf2, 0xa1dd, 0xf3, 0xa1dc,
    0xf4, 0xa1d2, 0xf5, 0xa1d2, 0xf6, 0xa1c2, 0xf7, 0xa1d6,
    0xf8, 0xa1e3, 0xf9, 0xa1a4, 0xfa, 0xa1f1, 0xfb, 0xa1cc,
    0xfe, 0xa1f6,
    0, 0
};
static maccurses_chtype maccurses_mackorean[] = {
    0x01, 0xa1d9, 0x02, 0xa1da, 0x03, 0xa2be,
    0x04, 0xa1df, 0x05, 0xa2c0, 0x06, 0xa2bc, 0x07, 0xa655,
    0x08, 0xa655 | A_REVERSE, 0x09, 0xa1db, 0x0a, 0xa1db | A_REVERSE, 0x0b, 0xa1ce, 
    0x0c, 0xa1cf, 0x0d, 0xa2dc, 0x0e, 0xa2dd, 0x0f, 0xa654,
    0x10, 0xa2ba, 0x11, 0xa2b8, 0x12, 0xa2d5, 0x13, 0xa784,
    0x14, 0xa2d2, 0x15, 0xa1d7,
    0x18, 0xa1e8, 0x19, 0xa1e9, 0x1a, 0xa1e6, 0x1b, 0xa1e7,
    0x1c, 0xa753, 0x1d, 0xa1ea, 0x1e, 0xa1e3, 0x1f, 0xa1e5,
    /* start fullwidth additions */
    0x121, 0xa3a1, 0x122, 0xa1b1, 0x123, 0xa3a3,
    0x124, 0xa3a4, 0x125, 0xa3a5, 0x126, 0xa3a6, 0x127, 0xa1af,
    0x128, 0xa3a8, 0x129, 0xa3a9, 0x12a, 0xa3aa, 0x12b, 0xa3ab,
    0x12c, 0xa3ac, 0x12d, 0xa3ad, 0x12e, 0xa3ae, 0x12f, 0xa3af,
    0x130, 0xa3b0, 0x131, 0xa3b1, 0x132, 0xa3b2, 0x133, 0xa3b3,
    0x134, 0xa3b4, 0x135, 0xa3b5, 0x136, 0xa3b6, 0x137, 0xa3b7,
    0x138, 0xa3b8, 0x139, 0xa3b9, 0x13a, 0xa3ba, 0x13b, 0xa3bb,
    0x13c, 0xa3bc, 0x13d, 0xa3bd, 0x13e, 0xa3be, 0x13f, 0xa3bf,
    0x140, 0xa3c0, 0x141, 0xa3c1, 0x142, 0xa3c2, 0x143, 0xa3c3,
    0x144, 0xa3c4, 0x145, 0xa3c5, 0x146, 0xa3c6, 0x147, 0xa3c7,
    0x148, 0xa3c8, 0x149, 0xa3c9, 0x14a, 0xa3ca, 0x14b, 0xa3cb,
    0x14c, 0xa3cc, 0x14d, 0xa3cd, 0x14e, 0xa3ce, 0x14f, 0xa3cf,
    0x150, 0xa3d0, 0x151, 0xa3d1, 0x152, 0xa3d2, 0x153, 0xa3d3,
    0x154, 0xa3d4, 0x155, 0xa3d5, 0x156, 0xa3d6, 0x157, 0xa3d7,
    0x158, 0xa3d8, 0x159, 0xa3d9, 0x15a, 0xa3da, 0x15b, 0xa3db,
    0x15c, 0xa1ac, 0x15d, 0xa3dd, 0x15e, 0xa3de, 0x15f, 0xa3df,
    0x160, 0xa1ae, 0x161, 0xa3e1, 0x162, 0xa3e2, 0x163, 0xa3e3,
    0x164, 0xa3e4, 0x165, 0xa3e5, 0x166, 0xa3e6, 0x167, 0xa3e7,
    0x168, 0xa3e8, 0x169, 0xa3e9, 0x16a, 0xa3ea, 0x16b, 0xa3eb,
    0x16c, 0xa3ec, 0x16d, 0xa3ed, 0x16e, 0xa3ee, 0x16f, 0xa3ef,
    0x170, 0xa3f0, 0x171, 0xa3f1, 0x172, 0xa3f2, 0x173, 0xa3f3,
    0x174, 0xa3f4, 0x175, 0xa3f5, 0x176, 0xa3f6, 0x177, 0xa3f7,
    0x178, 0xa3f8, 0x179, 0xa3f9, 0x17a, 0xa3fa, 0x17b, 0xa3fb,
    0x17c, 0xa3fc, 0x17d, 0xa3fd, 0x17e, 0xa1ad,
    /* end fullwidth additions */
    0x7f, 0xa1e2, 0x9b, 0xa1cb, 0x9c, 0xa1cc, 0x9d, 0xa1cd,
    0xae, 0xa1b6, 0xaf, 0xa1b7,
    0xa6, 0xa8a3, 0xa7, 0xa8ac,
    0xa8, 0xa2af, 0xaa, 0xa1fe, 0xab, 0xa8f6,
    0xac, 0xa8f9, 0xad, 0xa2ae, 0xae, 0xa65c, 0xaf, 0xa65d,
    0xb0, 0xa2c6, 0xb1, 0xa2c6, 0xb2, 0xa2c6, 0xb3, 0xa6a2,
    0xb4, 0xa6a9, 0xb5, 0xa6be, 0xb6, 0xa6b9, 0xb7, /*0xa6c1*/0xa6ce,
    0xb8, /*0xa6c2*/0xa6d1, 0xb9, 0xa6b4, 0xba, 0xa6ad, 0xbb, 0xa6af,
    0xbc, 0xa6b0, 0xbd, /*0xa6c3*/0xa6cd, 0xbe, /*0xa6c4*/0xa6d5, 0xbf, 0xa6a4,
    0xc0, 0xa6a6, 0xc1, 0xa6aa, 0xc2, 0xa6a8, 0xc3, 0xa6a7,
    0xc4, 0xa6a1, 0xc5, 0xa6ab, 0xc6, 0xa6bc, 0xc7, 0xa6b7,
    0xc8, 0xa6b1, 0xc9, 0xa6ae, 0xca, 0xa6b5, 0xcb, 0xa6b3,
    0xcc, 0xa6b2, 0xcd, 0xa6ac, 0xce, 0xa6b6, 0xcf, 0xa6ba,
    0xd0, 0xa6bf, 0xd1, 0xa6b8, 0xd2, 0xa6bd, 0xd3, /*0xa6c5*/0xa6c9,
    0xd4, /*0xa6c6*/0xa6d6, 0xd5, /*0xa6c8*/0xa6d2, 0xd6, /*0xa6c7*/0xa6ca, 0xd7, 0xa6c0,
    0xd8, 0xa6bb, 0xd9, 0xa6a5, 0xda, 0xa6a3, 0xdb, 0xa78f,
    0xe0, 0xa5e1, 0xe1, 0xa5e2, 0xe2, 0xa5c3, 0xe3, 0xa5f0,
    0xe4, 0xa5d2, 0xe5, 0xa5f2, 0xe6, 0xa5ec, 0xe7, 0xa5f3,
    0xe8, 0xa5d5, 0xe9, 0xa5c8, 0xea, 0xa5d8, 0xeb, 0xa5e4,
    0xec, 0xa1c4, 0xed, 0xa5f5, 0xee, 0xa5e5, 0xef, 0xa1fb,
    0xf0, 0xa1d5, 0xf1, 0xa1be, 0xf2, 0xa75b, 0xf3, 0xa75a,
    0xf4, 0xa1f2, 0xf5, 0xa1f2, 0xf6, 0xa1c0, 0xf7, 0xa49b,
    0xf8, 0xa1c6, 0xf9, 0xa1a4, 0xfa, 0xa1dc, 0xfb, 0xa1ee,
    0xfc, 0xa9fa, 0xfd, 0xa9f7, 0xfe, 0xa1e1,
    0, 0
};
static maccurses_chtype maccurses_macroman[] = {
    0x80, 0x82, 0x81, 0x9f, 0x82, 0x8e, 0x83, 0x89,
    0x84, 0x8a, 0x85, 0x88, 0x86, 0x8c, 0x87, 0x8d,
    0x88, 0x90, 0x89, 0x91, 0x8a, 0x8f, 0x8b, 0x95,
    0x8c, 0x94, 0x8d, 0x93, 0x8e, 0x80, 0x8f, 0x81,
    0x90, 0x83, 0x91, 0xbe, 0x92, 0xae, 0x93, 0x99,
    0x94, 0x9a, 0x95, 0x98, 0x96, 0x9e, 0x97, 0x9d,
    0x98, 0xd8, 0x99, 0x85, 0x9a, 0x86, 0x9b, 0xa2,
    0x9c, 0xa3, 0x9d, 0xb4, 0x9f, 0xc4, 0xa0, 0x87,
    0xa1, 0x92, 0xa2, 0x97, 0xa3, 0x9c, 0xa4, 0x96,
    0xa5, 0x84, 0xa6, 0xbb, 0xa7, 0xbc, 0xa8, 0xc0,
    0xaa, 0xc2, 0xad, 0xc1, 0xae, 0xc7, 0xaf, 0xc8,
    0xe1, 0xa7, 0xe3, 0xb9, 0xe6, 0xb5, 0xea, 0xbd,
    0xec, 0xb0, 0xf1, 0xb1, 0xf2, 0xb3, 0xf3, 0xb2,
    0xf6, 0xd6, 0xf7, 0xc5, 0xf8, 0xa1,
    0, 0
};
static maccurses_chtype maccurses_cp437[] = {
    0, 0
};

#endif /* defined(UNICODE) */

static int maccurses_valid = FALSE;
static int maccurses_gotQuitEvent = FALSE;

static long maccurses_qdversion;
static WindowRef maccurses_hwnd;
static GDHandle maccurses_device;
static GWorldPtr maccurses_world;
static PixMapHandle maccurses_pixmap;

static Point maccurses_size;
static int maccurses_ideal_w = 0;
static int maccurses_ideal_h = 0;
static Rect maccurses_ideal_rect;
static Boolean maccurses_maximized = false;
static CGrafPtr maccurses_port;

/* Text parameters saved prior to set-up, resored prior to exit for
 * strange environments like MPW where this is needed to avoid
 * corrupting the worksheet text rendering. */
static short maccurses_oldTextFont;
static Style maccurses_oldTextFace;
static short maccurses_oldTextMode;
static short maccurses_oldTextSize;
static Fixed maccurses_oldSpaceExtra;

static Str255 maccurses_title = { 0 };

static int maccurses_zoom = 0;

static short maccurses_widMax = 0;
static FontInfo maccurses_fontinfo;
static int maccurses_fontsize = 12;
static FMFontFamily maccurses_fontfamily = kInvalidFontFamily;
static FMFontFamily maccurses_custom_fontfamily = kInvalidFontFamily;
static FMFontStyle maccurses_custom_fontstyle = 0;
static TextEncoding maccurses_encoding = kTextEncodingMacRoman;

static short maccurses_pairs[COLOR_PAIRS][2];

static RGBColor maccurses_palette[COLORS + COLORS * COLORS + 1];

#define COLOR_PAIR(p) ((p) << _PAIR_SHIFT)

#define PAIR_NUMBER(a) (((a) & _MACCURSES_A_COLOR) >> _PAIR_SHIFT)

static void maccurses_cleanup(void)
{
    if (maccurses_valid != FALSE)
    {
        maccurses_valid = FALSE;
        DisposeWindow(maccurses_hwnd);
    }
}

static int maccurses_wcwidth(wchar_t wc)
{
    if (! maccurses_use_acs_flag)
    {
        wc &= ~_MACCURSES_A_ALTCHARSET;
    }
    if (maccurses_use_acs_flag == TRUE)
    {
        switch ((wc & _MACCURSES_A_ALTCHARSET)
                ?
                wc
                :
                (wc | _MACCURSES_A_ALTCHARSET))
        {
        case ACS_BLOCK:
        case _MACCURSES_ACS_THALF:
        case _MACCURSES_ACS_BHALF:
        case _MACCURSES_ACS_LHALF:
        case _MACCURSES_ACS_RHALF:
            return 1;
        }
    }
    if ((wc >= 0x20) && (wc < 0x7f))
    {
        return 1;
    }
    if (((wc & _MACCURSES_A_ALTCHARSET) == _MACCURSES_A_ALTCHARSET)
        &&
        (maccurses_use_acs_flag == TRUE))
    {
        wc ^= _MACCURSES_A_ALTCHARSET;
    }
    else
    {
#ifndef UNICODE
        if (maccurses_cvtt)
        {
            int j;

            if (! maccurses_cvtt[0])
            {
                return 1;
            }
            for (j = 0; maccurses_cvtt[j]; j += 2)
            {
                if ((maccurses_cvtt[j] & ~((maccurses_wide_flag == TRUE) ? 0x100 : 0))
                    ==
                    wc)
                {
                    if ((maccurses_cvtt[j + 1] & ~_MACCURSES_A_ATTR) > 0xff)
                    {
                        return (maccurses_widefont_flag == TRUE) ? 1 : 2;
                    }
                    else
                    {
                        return 1;
                    }
                }
            }
        }
#endif
#ifdef UNICODE
        if (((wc & _MACCURSES_A_ALTCHARSET) != _MACCURSES_A_ALTCHARSET)
            &&
            maccurses_valid)
        {
            ATSUStyle sty;
            Boolean atsuok = false;
            Rect dimen;
            UniChar uc[3];
            UniCharCount unilen;
            maccurses_chtype chuni;
            ATSUTextLayout lyo;
            ATSUAttributeTag fontSizeAttrTag, fontAttrTag;
            Fixed fontSizeAttrValue;
            ATSUFontID fontAttrValue;
            FMFontStyle whatwegot;
            ByteCount fontSizeAttrValueSize, fontAttrValueSize;
            ATSUAttributeValuePtr fontSizeAttrValuePtr, fontAttrValuePtr;

            chuni = (((wc & _MACCURSES_A_ALTCHARSET) == _MACCURSES_A_ALTCHARSET)
                     ?
                     (wc ^ _MACCURSES_A_ALTCHARSET)
                     :
                     wc);
            if ((chuni < 0) || (chuni > 0x10fffdL) || ((chuni & 0xfffeL) == 0xfffeL))
            {
                chuni = 0xfffd;
            }
            unilen = 1 + (chuni > 0xffffL);
            uc[0] = (chuni > 0xffffL) ? (((chuni - 0x10000L) >> 10) | 0xd800L) : chuni;
            uc[1] = (chuni > 0xffffL) ? (((chuni - 0x10000L) & 0x3ffL) | 0xd800L) : 0;
            uc[2] = 0;
            if (noErr == ATSUCreateStyle(&sty))
            {
                fontSizeAttrTag = kATSUSizeTag;
                fontSizeAttrValueSize = sizeof(fontSizeAttrValue);
                fontSizeAttrValuePtr = &fontSizeAttrValue;
                fontSizeAttrValue = maccurses_fontsize << 16;
                fontAttrTag = kATSUFontTag;
                fontAttrValueSize = sizeof(fontAttrValue);
                fontAttrValuePtr = &fontAttrValue;
                if ((noErr == FMGetFontFromFontFamilyInstance(maccurses_fontfamily,
                                                              maccurses_custom_fontstyle
                                                              |
                                                              ((maccurses_attr & A_UNDERLINE) ? underline : 0)
                                                              |
                                                              ((maccurses_attr & A_BOLD) ? bold : 0),
                                                              &fontAttrValue,
                                                              &whatwegot))
                    &&
                    (noErr == ATSUSetAttributes(sty, 1, &fontAttrTag, &fontAttrValueSize, &fontAttrValuePtr))
                    &&
                    (noErr == ATSUSetAttributes(sty, 1, &fontSizeAttrTag, &fontSizeAttrValueSize, &fontSizeAttrValuePtr))
                    &&
                    (noErr == ATSUCreateTextLayoutWithTextPtr(uc,
                                                              kATSUFromTextBeginning,
                                                              kATSUToTextEnd,
                                                              unilen,
                                                              1,
                                                              &unilen,
                                                              &sty,
                                                              &lyo)))
                {
                    ATSUFontID substFontAttrValue = kATSUInvalidFontID;
                    UniCharArrayOffset firstChanged = 0;
                    UniCharCount numChanged = 0;

                    if (noErr == ATSUMatchFontsToText(lyo,
                                                      kATSUFromTextBeginning,
                                                      kATSUToTextEnd,
                                                      &substFontAttrValue,
                                                      &firstChanged,
                                                      &numChanged))
                    {
                        ATSUSetTransientFontMatching(lyo, true);
                        if (noErr == ATSUMeasureTextImage(lyo,
                                                          kATSUFromTextBeginning,
                                                          kATSUToTextEnd,
                                                          0,
                                                          0,
                                                          &dimen))
                        {
                            /* woot. */
                            atsuok = true;
                        }
                    }
                    ATSUDisposeTextLayout(lyo);
                }
                ATSUDisposeStyle(sty);
            }
            if (atsuok != false)
            {
                if ((dimen.right - dimen.left) > (maccurses_widMax + (maccurses_widMax / 2)))
                {
                    return 2;
                }
                /* FIXME: this is a horrible hack! */
                if ((chuni >= 0x0020) && (chuni < 0x3000)) return 1;
                return 2;
            }
        }
#endif /* defined(UNICODE) */
    }
    if ((! (wc & _MACCURSES_A_ALTCHARSET))
        &&
        (maccurses_use_acs_flag == TRUE))
    {
        switch (wc | _MACCURSES_A_ALTCHARSET)
        {
        case ACS_LRCORNER:
        case ACS_URCORNER:
        case ACS_ULCORNER:
        case ACS_LLCORNER:
        case ACS_PLUS:
        case ACS_LTEE:
        case ACS_RTEE:
        case ACS_BTEE:
        case ACS_TTEE:
        case ACS_HLINE:
        case ACS_VLINE:
        case _MACCURSES_ACS_MIDDOT:
        case _MACCURSES_ACS_PUCE:
        case _MACCURSES_ACS_BLACKSQUARE:
        case ACS_BLOCK:
        case _MACCURSES_ACS_THALF:
        case _MACCURSES_ACS_BHALF:
        case _MACCURSES_ACS_LHALF:
        case _MACCURSES_ACS_RHALF:
            return 1;
        }
    }
    return -1;
}

static int maccurses_endwin(void);

static int maccurses_init_pair(short pair, short f, short b);

static int maccurses_initscrWithHints(int h, int w, const char *title, const char *shortname);

static int maccurses_has_colors(void)
{
    if (! maccurses_valid)
    {
        maccurses_initscrWithHints(maccurses_h, maccurses_w, 0, 0);
    }
    return IsPortColor(maccurses_port) ? TRUE : FALSE;
}

static int maccurses_can_change_color(void)
{
    if (! maccurses_has_colors())
    {
        return FALSE;
    }
    return TRUE;
}

static int maccurses_erase(void);

/* should be in <math.h> but sometimes it's missing... */

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880168872420969808
#endif

static unsigned short maccurses_mix(unsigned short a, unsigned short b)
{
    unsigned long r;

    r = 0xffffL * sqrt(1.0 * a / 0xffffL * a / 0xffffL + 1.0 * b / 0xffffL * b / 0xffffL) / M_SQRT2;
    if (r > 0xffffL)
    {
        r = 0xffffL;
    }
    return (unsigned short) r;
}

static void maccurses_updateFontCheckMarks(MenuRef mr)
{
    int i;
    UInt16 count;

#if ! (defined(CountMenuItems) || USE_OLD_TOOLBOX)
    if (! CountMenuItems)
    {
        return;
    }
#endif
    count = CountMenuItems(mr);
    for (i = 0; i <= count; i ++)
    {
        FMFontFamily fam;
        FMFontStyle sty;
        CharParameter ch;

        GetItemMark(mr, i, &ch);
        if ((ch == noMark) || (ch == checkMark))
        {
            fam = kInvalidFontFamily;
            sty = 0;
            GetFontFamilyFromMenuSelection(mr, i, &fam, &sty);
#ifdef __CARBON__
            if (MacCheckMenuItem)
            {
                MacCheckMenuItem(mr, i,
                                 ((fam == maccurses_fontfamily)
                                  &&
                                  (sty == maccurses_custom_fontstyle)) ? true : false);
            }
#else
            CheckItem(mr, i,
                      ((fam == maccurses_fontfamily)
                       &&
                       (sty == maccurses_custom_fontstyle)) ? true : false);
#endif
        }
#ifdef __CARBON__
        else
        {
            MenuRef submr;

            if (GetMenuItemHierarchicalMenu
                &&
                (noErr == GetMenuItemHierarchicalMenu(mr, i, &submr)))
            {
                maccurses_updateFontCheckMarks(submr);
            }
        }
#endif /* defined(__CARBON__) */
    }
}

static MenuRef maccurses_fontmenu = NULL;

static void maccurses_updateFontMenu(void)
{
    if (maccurses_fontmenu)
    {
#ifdef __CARBON__
        if (UpdateStandardFontMenu)
        {
            UpdateStandardFontMenu(maccurses_fontmenu, NULL);
        }
#endif /* defined(__CARBON__) */
        maccurses_updateFontCheckMarks(maccurses_fontmenu);
    }
}

static pascal OSErr maccurses_onQuitEvent(const AppleEvent *ev, AppleEvent *reply, long handlerRefcon)
{
    if ((! ev) || (! reply) || handlerRefcon)
    {
        /* we should care... */
    }
    maccurses_gotQuitEvent = TRUE;
    return noErr;
}

static MenuBarHandle maccurses_oldMainMenu;

static int maccurses_initscrWithHints(int h, int w, const char *title, const char *shortname)
{
    int basefontsize = 12;
    char *font;
    const char *encoding_name;
    Str255 font_pascal;
    Rect coords;
    Boolean encoding_determined = false;
    ScriptCode scriptID;
    int i;
    int j;
    int firsttime;
    int managers_already_inited;

    managers_already_inited = 0;
    maccurses_after_endwin = 0;
    if (maccurses_valid == FALSE)
    {
        maccurses_valid = TRUE;
        firsttime = 1;
#ifndef __CARBON__
        MaxApplZone();
#if defined(HAVE_COMMAND_LINE)
        managers_already_inited = HAVE_COMMAND_LINE;
#endif
#endif
    }
    else
    {
        firsttime = 0;
        managers_already_inited = 1;
    }
    if (maccurses_new_w && maccurses_new_h)
    {
        w = maccurses_new_w;
        h = maccurses_new_h;
        maccurses_new_w = 0;
        maccurses_new_h = 0;
    }
    if (! w)
    {
        w = maccurses_w;
    }
    if (! h)
    {
        h = maccurses_h;
    }
    if (! w)
    {
        w = 80;
    }
    if (! h)
    {
        h = 25;
    }
    if (w < 8)
    {
        w = 8;
    }
    if (h < 4)
    {
        h = 4;
    }
    maccurses_x = 0;
    maccurses_y = 0;
    maccurses_attr = 0;
    if (title
        ||
        (! maccurses_title[0]))
    {
        if ((! title)
            ||
            (! *title))
        {
            title = "Untitled";
        }
        maccurses_title[0] = 0;
        for (i = 0; (i < 256) && title && title[i]; i ++)
        {
            maccurses_title[0] = i + 1;
            maccurses_title[i + 1] = title[i];
        }
    }
    if (firsttime)
    {
        ProcessSerialNumber psn;
        long osAttrs = 0;

        atexit(maccurses_cleanup);
        if ((noErr == Gestalt(gestaltOSAttr, &osAttrs))
            &&
            (osAttrs & (1 << gestaltLaunchControl))
            &&
            (noErr == MacGetCurrentProcess(&psn)))
        {
#ifndef macintosh
            TransformProcessType(&psn, kProcessTransformToForegroundApplication);
#endif /* ! defined(macintosh) */
            SetFrontProcess(&psn);
        }
#ifndef __CARBON__
#if ! USE_OLD_TOOLBOX
        maccurses_use_windowslib = 1;
        if (myman_getenv("MACCURSES_WM")
            &&
            *myman_getenv("MACCURSES_WM"))
        {
            maccurses_use_windowslib = atoi(myman_getenv("MACCURSES_WM"));
        }
#endif
#endif
#if USE_FONT_MANAGER_9
        maccurses_use_font_manager_9 = 1;
        if (myman_getenv("MACCURSES_FM9")
            &&
            *myman_getenv("MACCURSES_FM9"))
        {
            maccurses_use_font_manager_9 = atoi(myman_getenv("MACCURSES_FM9"));
        }
#endif
#if USE_TEXT_ENCODING_CONVERSION_MANAGER
        maccurses_use_text_encoding_conversion_manager = 1;
        if (myman_getenv("MACCURSES_TECM")
            &&
            *myman_getenv("MACCURSES_TECM"))
        {
            maccurses_use_text_encoding_conversion_manager = atoi(myman_getenv("MACCURSES_TECM"));
        }
        if (! (CreateTextEncoding && TECConvertText && TECCreateConverter && TECDisposeConverter && TECFlushText))
        {
            maccurses_use_text_encoding_conversion_manager = 0;
        }
#endif /* USE_TEXT_ENCODING_CONVERSION_MANAGER */
#ifndef __CARBON__
        InitGraf((void *) &(qd.thePort));
        if (! managers_already_inited)
        {
            InitFonts();
            InitWindows();
            InitMenus();
#if USE_TEXT_ENCODING_CONVERSION_MANAGER
            if (maccurses_use_text_encoding_conversion_manager)
            {
                TEInit();
            }
#endif /* USE_TEXT_ENCODING_CONVERSION_MANAGER */
        }
#endif /* ! defined(__CARBON__) */

        if (! managers_already_inited)
        {
            InitCursor();
        }
        {
#if USE_OLD_TOOLBOX
            static UniversalProcPtr maccurses_trapFontDispatch;

            if (! maccurses_trapFontDispatch)
            {
                maccurses_trapFontDispatch = maccurses_getTrapAddress(_FontDispatch);
            }
            if (maccurses_trapFontDispatch != maccurses_trapUnimplemented)
#endif
            {
                SetOutlinePreferred(TRUE);
                SetPreserveGlyph(FALSE);
            }
        }
        SetFScaleDisable(TRUE);
    }

    maccurses_debug_flag = FALSE;
    if (myman_getenv("MACCURSES_DEBUG")
        &&
        *myman_getenv("MACCURSES_DEBUG")
        &&
        atoi(myman_getenv("MACCURSES_DEBUG")))
    {
        maccurses_debug_flag = TRUE;
    }
    maccurses_use_acs_flag = TRUE;
    if (myman_getenv("MACCURSES_ACS")
        &&
        *myman_getenv("MACCURSES_ACS"))
    {
        maccurses_use_acs_flag = atoi(myman_getenv("MACCURSES_ACS")) ? TRUE : FALSE;
    }
    maccurses_wide_flag = FALSE;
    if (myman_getenv("MACCURSES_WIDE")
        &&
        *myman_getenv("MACCURSES_WIDE"))
    {
        maccurses_wide_flag = atoi(myman_getenv("MACCURSES_WIDE")) ? TRUE : FALSE;
    }
    maccurses_widefont_flag = maccurses_wide_flag;
    if (myman_getenv("MACCURSES_WIDEFONT")
        &&
        *myman_getenv("MACCURSES_WIDEFONT"))
    {
        maccurses_widefont_flag = atoi(myman_getenv("MACCURSES_WIDEFONT")) ? TRUE : FALSE;
    }
    maccurses_fontsize = basefontsize = (GetDefFontSize() >= 11) ? 12 : 9;
#ifdef MACCURSES_FONTSIZE
    if (MACCURSES_FONTSIZE) maccurses_fontsize = MACCURSES_FONTSIZE;
#endif
    if (myman_getenv("MACCURSES_FONTSIZE")
        &&
        *myman_getenv("MACCURSES_FONTSIZE")
        &&
        atoi(myman_getenv("MACCURSES_FONTSIZE")))
    {
        maccurses_fontsize = atoi(myman_getenv("MACCURSES_FONTSIZE"));
        if (maccurses_fontsize < 1) maccurses_fontsize = 1;
        if (maccurses_fontsize > 32767) maccurses_fontsize = 32767;
    }
    for (i = 0; i < maccurses_zoom; i ++)
    {
        if (maccurses_fontsize < 3)
        {
            maccurses_fontsize ++;
        }
        else if (maccurses_fontsize < 32767)
        {
            maccurses_fontsize = maccurses_fontsize * 4 / 3;
        }
        else
        {
            maccurses_zoom --;
        }
    }
    for (i = 0; i > maccurses_zoom; i --)
    {
        if (maccurses_fontsize > 1)
        {
            maccurses_fontsize = maccurses_fontsize * 3 / 4;
        }
        else
        {
            maccurses_zoom ++;
        }
    }
    if (maccurses_fontsize < 1) maccurses_fontsize = 1;
    if (maccurses_fontsize > 32767) maccurses_fontsize = 32767;
    {
        SetRect(&coords, 100, 100,
                100 + (w * maccurses_fontsize / 2), 100 + (h * maccurses_fontsize));
    }
    if (firsttime)
    {
        if (noErr != Gestalt(gestaltQuickdrawVersion, &maccurses_qdversion))
        {
            maccurses_qdversion = gestaltOriginalQD;
        }
        maccurses_hwnd = 0;
        if (maccurses_qdversion != gestaltOriginalQD)
        {
            maccurses_hwnd = NewCWindow(0, &coords, maccurses_title, true, zoomDocProc, (WindowRef)-1L, true, MACCURSES_MAINWIN);
        }
        if (! maccurses_hwnd)
        {
            maccurses_hwnd = NewWindow(0, &coords, maccurses_title, true, zoomDocProc, (WindowRef)-1L, true, MACCURSES_MAINWIN);
        }
        if (! maccurses_hwnd)
        {
            fprintf(stderr, "NewWindow failed\n");
            fflush(stderr);
            return 0;
        }
        SetEventMask(keyDownMask | autoKeyMask | activMask | (maccurses_pixmap ? updateMask : 0) | mUpMask | mDownMask | osMask | highLevelEventMask);
        maccurses_port = GetWindowPort(maccurses_hwnd);
        SetPort((GrafPtr) maccurses_port);
        maccurses_oldTextFont = GetPortTextFont(maccurses_port);
        maccurses_oldTextFace = GetPortTextFace(maccurses_port);
        maccurses_oldTextMode = GetPortTextMode(maccurses_port);
        maccurses_oldTextSize = GetPortTextSize(maccurses_port);
        maccurses_oldSpaceExtra = GetPortSpExtra(maccurses_port);
    }
    encoding_name = myman_getenv("MACCURSES_FONTENCODING");
#ifdef MACCURSES_FONTENCODING
    if (! encoding_name) encoding_name = MACCURSES_FONTENCODING;
#endif
    maccurses_fontfamily = maccurses_custom_fontfamily;
    font_pascal[0] = 0;
    font = 0;
    font = myman_getenv("MACCURSES_FONT");
#ifdef MACCURSES_FONT
    if (! font) font = MACCURSES_FONT;
#endif
    if (maccurses_custom_fontfamily != kInvalidFontFamily)
    {
        if (noErr == FMGetFontFamilyName(maccurses_custom_fontfamily, font_pascal))
        {
            if (maccurses_debug_flag == TRUE)
            {
                int iii;

                fprintf(stderr, "MACCURSES_FONT=");
                for (iii = 0; iii < font_pascal[0]; ++iii)
                {
                    int c;

                    c = (int) (unsigned) (unsigned char) font_pascal[iii + 1];
                    if ((c == '\\') || (c == ' '))
                    {
                        fprintf(stderr, "\\");
                    }
                    if ((c >= 0x20) && (c < 0x7f))
                    {
                        fprintf(stderr, "%c", c);
                    }
                    else
                    {
                        fprintf(stderr, "\\%o", c);
                    }
                }
                fprintf(stderr, "\nMACCURSES_FONTSIZE=%d\n", maccurses_fontsize);
                fflush(stderr);
            }
            maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
        }
    }
    if ((maccurses_fontfamily == kInvalidFontFamily)
        &&
        font
        &&
        font[0])
    {
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
        if (maccurses_fontfamily == kInvalidFontFamily)
        {
            GetFNum(font_pascal, &maccurses_fontfamily);
            if (! maccurses_fontfamily)
            {
                encoding_name = 0;
                maccurses_fontfamily = kInvalidFontFamily;
            }
        }
    }
#if UNICODE
    if (maccurses_fontfamily == kInvalidFontFamily)
    {
        font = "DejaVu Sans Mono";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
    }
    if (maccurses_fontfamily == kInvalidFontFamily)
    {
        font = "Liberation Mono";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
    }
#endif
    if ((maccurses_fontfamily == kInvalidFontFamily)
        &&
        ((maccurses_fontsize == 9)
         ||
         (maccurses_fontsize == 12)))
    {
        font = "VT100";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
    }
#if 0
    if (maccurses_fontfamily == kInvalidFontFamily)
    {
        /* Osaka Mono (Shift_JIS/MacJapanese) */
        font = "Osaka\201\174\223\231\225\235";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
    }
#endif
    if (maccurses_fontfamily == kInvalidFontFamily)
    {
        font = "Monaco";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
    }
    if (maccurses_fontfamily == kInvalidFontFamily)
    {
        font = "Courier";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
    }
    maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
    if (maccurses_fontfamily == kInvalidFontFamily)
    {
        GetFNum(font_pascal, &maccurses_fontfamily);
    }
    if (noErr != FMGetFontFamilyName(maccurses_fontfamily, font_pascal))
    {
        GetFontName(maccurses_fontfamily, font_pascal);
    }
    maccurses_encoding = kTextEncodingMacRoman;
    GetFontName(maccurses_fontfamily, font_pascal);
    if ((noErr != FMGetFontFamilyTextEncoding(maccurses_fontfamily, &maccurses_encoding))
        ||
        (noErr != RevertTextEncodingToScriptInfo(maccurses_encoding, &scriptID, NULL, NULL)))
    {
        scriptID = FontToScript(maccurses_fontfamily);
        if (scriptID == smJapanese)
        {
            maccurses_encoding = kTextEncodingMacJapanese;
            if (! encoding_name) encoding_name = "MacJapanese";
        }
        else if (scriptID == smTradChinese)
        {
            maccurses_encoding = kTextEncodingMacChineseTrad;
            if (! encoding_name) encoding_name = "MacChineseTrad";
        }
        else if (scriptID == smSimpChinese)
        {
            maccurses_encoding = kTextEncodingMacChineseSimp;
            if (! encoding_name) encoding_name = "MacChineseSimp";
        }
        else if (scriptID == smKorean)
        {
            maccurses_encoding = kTextEncodingMacKorean;
            if (! encoding_name) encoding_name = "MacKorean";
        }
        else if (scriptID == smRoman)
        {
            maccurses_encoding = kTextEncodingMacRoman;
            if ((scriptID == smRoman)
                &&
                (font_pascal[0] == strlen("VT100"))
                &&
                (! memcmp((void *) (font_pascal + 1), (void *) "VT100", strlen("VT100"))))
            {
                maccurses_encoding = kTextEncodingMacVT100;
                if (! encoding_name) encoding_name = "MacVT100";
            }
            else
            {
                if (! encoding_name) encoding_name = "MacRoman";
            }
        }
        else if ((scriptID == smUninterp)
                 &&
                 (font_pascal[0] == strlen("VT100"))
                 &&
                 (! memcmp((void *) (font_pascal + 1), (void *) "VT100", strlen("VT100"))))
        {
            maccurses_encoding = kTextEncodingMacVT100;
            if (! encoding_name) encoding_name = "MacVT100";
        }
        else
        {
            maccurses_encoding = kTextEncodingUS_ASCII;
        }
    }
    if (encoding_name && *encoding_name)
    {
        Str255 encoding_name_pascal;

        encoding_determined = true;
        encoding_name_pascal[0] = 0;
        for (i = 0; (i < 255) && encoding_name[i]; i ++)
        {
            encoding_name_pascal[0] = i + 1;
            encoding_name_pascal[i + 1] = encoding_name[i];
        }
#if USE_TEXT_ENCODING_CONVERSION_MANAGER
        if (maccurses_use_text_encoding_conversion_manager
            &&
            TECGetTextEncodingFromInternetName
            &&
            (noErr == TECGetTextEncodingFromInternetName(&maccurses_encoding, encoding_name_pascal)))
        {
        }
        else
#endif /* USE_TEXT_ENCODING_CONVERSION_MANAGER */
        {
            if ((! strcasecmp(encoding_name, "MacRoman"))
                ||
                (! strcasecmp(encoding_name, "Macintosh"))
                ||
                (! strcasecmp(encoding_name, "csMacintosh"))
                ||
                (! strcasecmp(encoding_name, "Mac")))
            {
                maccurses_encoding = kTextEncodingMacRoman;
            }
            else if (! strcasecmp(encoding_name, "MacJapanese"))
            {
                maccurses_encoding = kTextEncodingMacJapanese;
            }
            else if ((! strcasecmp(encoding_name, "MacChineseTrad"))
                     ||
                     (! strcasecmp(encoding_name, "MacTradChinese")))
            {
                maccurses_encoding = kTextEncodingMacChineseTrad;
            }
            else if ((! strcasecmp(encoding_name, "MacChineseSimp"))
                     ||
                     (! strcasecmp(encoding_name, "MacSimpChinese")))
            {
                maccurses_encoding = kTextEncodingMacChineseSimp;
            }
            else if (! strcasecmp(encoding_name, "MacKorean"))
            {
                maccurses_encoding = kTextEncodingMacKorean;
            }
            else if (! strcasecmp(encoding_name, "MacVT100"))
            {
                maccurses_encoding = kTextEncodingMacVT100;
            }
            else if ((! strcasecmp(encoding_name, "cp437"))
                     ||
                     (! strcasecmp(encoding_name, "437"))
                     ||
                     (! strcasecmp(encoding_name, "ibm437"))
                     ||
                     (! strcasecmp(encoding_name, "DOSLatinUS")))
            {
                maccurses_encoding = kTextEncodingDOSLatinUS;
            }
            else
            {
                maccurses_encoding = kTextEncodingUS_ASCII;
            }
        }
    }
#ifndef UNICODE
    if (GetTextEncodingBase(maccurses_encoding) == kTextEncodingMacRoman)
    {
        maccurses_cvtt = maccurses_macroman;
    }
    else if (GetTextEncodingBase(maccurses_encoding) == kTextEncodingMacJapanese)
    {
        maccurses_cvtt = maccurses_macjapanese;
    }
    else if (GetTextEncodingBase(maccurses_encoding) == kTextEncodingMacChineseTrad)
    {
        maccurses_cvtt = maccurses_macchinesetrad;
    }
    else if (GetTextEncodingBase(maccurses_encoding) == kTextEncodingMacChineseSimp)
    {
        maccurses_cvtt = maccurses_macchinesesimp;
    }
    else if (GetTextEncodingBase(maccurses_encoding) == kTextEncodingMacKorean)
    {
        maccurses_cvtt = maccurses_mackorean;
    }
    else if (GetTextEncodingBase(maccurses_encoding) == kTextEncodingMacVT100)
    {
        maccurses_cvtt = maccurses_macvt100;
    }
    else if (GetTextEncodingBase(maccurses_encoding) == kTextEncodingDOSLatinUS)
    {
        maccurses_cvtt = maccurses_cp437;
    }
    else
    {
        maccurses_cvtt = 0;
    }

#if USE_TEXT_ENCODING_CONVERSION_MANAGER
    if (maccurses_use_text_encoding_conversion_manager
        &&
        ((encoding_determined == true)
         ||
         (noErr == FMGetFontFamilyTextEncoding(maccurses_fontfamily, &maccurses_encoding))
         ||
         (UpgradeScriptInfoToTextEncoding
          &&
          (noErr == UpgradeScriptInfoToTextEncoding(scriptID, kTextLanguageDontCare, kTextRegionDontCare, font_pascal, &maccurses_encoding)))))
    {
        TextEncoding cp437;
        TECObjectRef cvt;
        OSStatus ret;
        static maccurses_chtype cvtd[256 * 2];

        cvt = NULL;
        encoding_determined = true;
        cp437 = CreateTextEncoding(kTextEncodingDOSLatinUS, kTextEncodingDefaultVariant, kTextEncodingDefaultFormat);
        ret = TECCreateConverter(&cvt, cp437, maccurses_encoding);
        if (noErr == ret)
        {
            UInt8 ibyte;
            ByteCount nread = 0;
            UInt8 obytes[32];
            ByteCount nwritten = 0;
            int cvtu = 0;

            for (i = 0x20; i <= 0xff; i ++)
            {
                if (i != 0x7f)
                {
                    ibyte = i;
                    if (noErr == TECConvertText(cvt, &ibyte, 1, &nread, obytes, sizeof(obytes), &nwritten))
                    {
                        ByteCount moar = 0;

                        if (noErr == TECFlushText(cvt, obytes + nwritten, sizeof(obytes) - nwritten, &moar))
                        {
                            nwritten += moar;
                        }
                        obytes[nwritten] = 0;
                        if ((nwritten == 1)
                            &&
                            (obytes[0] > 0x7f))
                        {
                            cvtd[2 * cvtu] = i;
                            cvtd[2 * cvtu + 1] = obytes[0];
                            cvtu ++;
                        }
                        else if ((nwritten == 2)
                                 &&
                                 (obytes[0] > 0x7f))
                        {
                            cvtd[2 * cvtu] = i;
                            cvtd[2 * cvtu + 1] = (obytes[0] << 8) | obytes[1];
                            cvtu ++;
                        }
                    }
                }
            }
            if (noErr != TECDisposeConverter(cvt))
            {
                /* there's not much to be done about a failure to
                 * dispose of the converter... */
            }
            if (cvtu || maccurses_cvtt)
            {
                cvtd[cvtu * 2] = 0;
                cvtd[cvtu * 2 + 1] = 0;
                if ((! maccurses_cvtt)
                    ||
                    (myman_getenv("MACCURSES_CONVERT")
                     &&
                     *myman_getenv("MACCURSES_CONVERT")
                     &&
                     atoi(myman_getenv("MACCURSES_CONVERT"))))
                {
                    maccurses_cvtt = cvtd;
                }
            }
        }
    }
    if ((maccurses_debug_flag == TRUE) && maccurses_use_text_encoding_conversion_manager && GetTextEncodingName)
    {
        UInt8 name[256];
        ByteCount namesz;

        if (noErr == GetTextEncodingName(
                maccurses_encoding,
                kTextEncodingBaseName,
                kTextRegionDontCare,
                kTextEncodingMacRoman,
                sizeof(name) - 1,
                &namesz,
                NULL,
                NULL,
                name))
        {
            name[namesz] = '\0';
            fprintf(stderr, "MACCURSES_FONTENCODING=%s\n", (char *) name);
            fflush(stderr);
        }
    }
#endif /* USE_TEXT_ENCODING_CONVERSION_MANAGER */
    if (maccurses_debug_flag == TRUE)
    {
        fprintf(stderr, "MACCURSES_FONTENCODING=0x%lx\n", (unsigned long) maccurses_encoding);
        fflush(stderr);
    }
#endif /* ! defined(UNICODE) */

    maccurses_oldMainMenu = GetMenuBar();
    {
        int i;
        /* 0x14, a.k.a. kMenuAppleLogoFilledGlyph */
        const char *f8ffmenu_title = "\x14";
        const char *filemenu_title = "File";
        const char *editmenu_title = "Edit";
        const char *sizemenu_title = "Size";
        const char *fontmenu_title = "Font";
        Str255 f8ffmenu_title_pascal = { 0 };
        Str255 filemenu_title_pascal = { 0 };
        Str255 editmenu_title_pascal = { 0 };
        Str255 sizemenu_title_pascal = { 0 };
        Str255 fontmenu_title_pascal = { 0 };
        MenuRef f8ffmenu = NULL;
        MenuRef filemenu = NULL;
        MenuRef editmenu = NULL;
        MenuRef sizemenu = NULL;

        /* FIXME: we should figure out how to use shortname when running as an MPW tool */
        if (shortname && 0)
        {
            f8ffmenu_title = (shortname && *shortname) ? shortname : "maccurses";
        }
        f8ffmenu_title_pascal[0] = 0;
        for (i = 0; (i < 255) && f8ffmenu_title[i]; i ++)
        {
            f8ffmenu_title_pascal[i + 1] = f8ffmenu_title[i];
            f8ffmenu_title_pascal[0] = i + 1;
        }
        f8ffmenu = NewMenu(MACCURSES_F8FFMENU, f8ffmenu_title_pascal);
        if (f8ffmenu)
        {
            const char *f8ffmenu_items = "About/I;-";
            Str255 f8ffmenu_items_pascal = { 0 };

            f8ffmenu_items_pascal[0] = 0;
            for (i = 0; (i < 255) && f8ffmenu_items[i]; i ++)
            {
                f8ffmenu_items_pascal[i + 1] = f8ffmenu_items[i];
                f8ffmenu_items_pascal[0] = i + 1;
            }
            AppendMenu(f8ffmenu, f8ffmenu_items_pascal);
            AppendResMenu(f8ffmenu, MACCURSES_FOUR_CHAR_CODE("DRVR"));
            InsertMenu(f8ffmenu, 0);
        }
        filemenu_title_pascal[0] = 0;
        for (i = 0; (i < 255) && filemenu_title[i]; i ++)
        {
            filemenu_title_pascal[i + 1] = filemenu_title[i];
            filemenu_title_pascal[0] = i + 1;
        }
        filemenu = NewMenu(MACCURSES_FILEMENU, filemenu_title_pascal);
        if (filemenu)
        {
            const char *filemenu_items = "Help/?;Interrupt/.;-;Quit/Q";
            Str255 filemenu_items_pascal = { 0 };

            filemenu_items_pascal[0] = 0;
            for (i = 0; (i < 255) && filemenu_items[i]; i ++)
            {
                filemenu_items_pascal[i + 1] = filemenu_items[i];
                filemenu_items_pascal[0] = i + 1;
            }
            AppendMenu(filemenu, filemenu_items_pascal);
            InsertMenu(filemenu, 0);
        }
        editmenu_title_pascal[0] = 0;
        for (i = 0; (i < 255) && editmenu_title[i]; i ++)
        {
            editmenu_title_pascal[i + 1] = editmenu_title[i];
            editmenu_title_pascal[0] = i + 1;
        }
        editmenu = NewMenu(MACCURSES_EDITMENU, editmenu_title_pascal);
        if (editmenu)
        {
            const char *editmenu_items = "Zoom/Z;-;Redraw/R";
            Str255 editmenu_items_pascal = { 0 };

            editmenu_items_pascal[0] = 0;
            for (i = 0; (i < 255) && editmenu_items[i]; i ++)
            {
                editmenu_items_pascal[i + 1] = editmenu_items[i];
                editmenu_items_pascal[0] = i + 1;
            }
            AppendMenu(editmenu, editmenu_items_pascal);
            InsertMenu(editmenu, 0);
        }
        fontmenu_title_pascal[0] = 0;
        for (i = 0; (i < 255) && fontmenu_title[i]; i ++)
        {
            fontmenu_title_pascal[i + 1] = fontmenu_title[i];
            fontmenu_title_pascal[0] = i + 1;
        }
        maccurses_fontmenu = NewMenu(MACCURSES_FONTMENU, fontmenu_title_pascal);
        if (maccurses_fontmenu)
        {
            const char *fontmenu_items = "Bigger Font/+;Smaller Font/-;Original Font Size/=";
            Str255 fontmenu_items_pascal = { 0 };

            fontmenu_items_pascal[0] = 0;
            for (i = 0; (i < 255) && fontmenu_items[i]; i ++)
            {
                fontmenu_items_pascal[i + 1] = fontmenu_items[i];
                fontmenu_items_pascal[0] = i + 1;
            }
            AppendMenu(maccurses_fontmenu, fontmenu_items_pascal);
            {
                fontmenu_items = "-;Original Font/T;-";
                fontmenu_items_pascal[0] = 0;
                for (i = 0; (i < 255) && fontmenu_items[i]; i ++)
                {
                    fontmenu_items_pascal[i + 1] = fontmenu_items[i];
                    fontmenu_items_pascal[0] = i + 1;
                }
                AppendMenu(maccurses_fontmenu, fontmenu_items_pascal);
                if ((maccurses_debug_flag == TRUE)
                    &&
                    ((maccurses_custom_fontfamily != kInvalidFontFamily)
                     ||
                     (maccurses_fontfamily != kInvalidFontFamily)))
                {
                    Str255 font_pascal;

                    if (noErr == FMGetFontFamilyName(
                            (maccurses_custom_fontfamily != kInvalidFontFamily)
                            ? maccurses_custom_fontfamily
                            : maccurses_fontfamily,
                            font_pascal))
                    {
                        UInt16 count;

                        AppendMenu(maccurses_fontmenu, font_pascal);
                        count = CountMenuItems(maccurses_fontmenu);
#if ! USE_OLD_TOOLBOX
                        SetMenuItemTextEncoding(
                            maccurses_fontmenu,
                            count,
                            maccurses_encoding);
#endif
                    }
                }
#ifdef __CARBON__
                if (CreateStandardFontMenu
                    &&
                    noErr == CreateStandardFontMenu(maccurses_fontmenu,
                                                    CountMenuItems(maccurses_fontmenu) + 1,
                                                    MACCURSES_FONTMENU + 1,
                                                    kHierarchicalFontMenuOption,
                                                    NULL))
                {
                }
                else
#endif /* ! defined(__CARBON__) */
                {
                    AppendResMenu(maccurses_fontmenu, MACCURSES_FOUR_CHAR_CODE("FONT"));
                }
            }
            InsertMenu(maccurses_fontmenu, 0);
        }
        sizemenu_title_pascal[0] = 0;
        for (i = 0; (i < 255) && sizemenu_title[i]; i ++)
        {
            sizemenu_title_pascal[i + 1] = sizemenu_title[i];
            sizemenu_title_pascal[0] = i + 1;
        }
        sizemenu = NewMenu(MACCURSES_SIZEMENU, sizemenu_title_pascal);
        if (sizemenu)
        {
            const char *sizemenu_items = "-;Bigger Font/+;Smaller Font/-;Original Font Size/=;-;Zoom/Z";
            Str255 sizemenu_items_pascal = { 0 };

            sizemenu_items_pascal[0] = 0;
            for (i = 0; (i < 255) && sizemenu_items[i]; i ++)
            {
                sizemenu_items_pascal[i + 1] = sizemenu_items[i];
                sizemenu_items_pascal[0] = i + 1;
            }
            AppendMenu(sizemenu, sizemenu_items_pascal);
            InsertMenu(sizemenu, 0);
        }
        MacDrawMenuBar();
    }

    TextFont(maccurses_fontfamily);
    /*TextMode(transparent);*/
    TextMode(srcCopy);
    TextFace(0);
    TextSize(maccurses_fontsize);
    SpaceExtra(0);
    GetFontInfo(&maccurses_fontinfo);
    maccurses_widMax = 0;
    for (i = 0x20; i < 0x7f; i ++)
    {
        Str255 si;
        short wid;

        si[0] = 1;
        si[1] = i;
        if ((i != '#')
            &&
            (i != '$')
            &&
            (i != '\\')
            &&
            (i != '~'))
        {
            wid = StringWidth(si);
            if (wid > maccurses_widMax)
            {
                maccurses_widMax = wid;
            }
        }
    }
#ifndef UNICODE
    if (maccurses_cvtt)
    {
        if (! maccurses_cvtt[0])
        {
            for (i = 0x80; i <= 0xff; i ++)
            {
                if ((i != 0xdb)
                    &&
                    (i != 0xf3)
                    &&
                    (i != 0xf7))
                {
                    Str255 si;
                    short wid;

                    si[0] = 1;
                    si[1] = i;
                    wid = StringWidth(si);
                    if (wid > maccurses_widMax)
                    {
                        maccurses_widMax = wid;
                    }
                }
            }
        }
        for (j = 0; maccurses_cvtt[j]; j += 2)
        {
            if ((((maccurses_cvtt[j + 1] & ~_MACCURSES_A_ATTR) <= 0xff)
                 ||
                 maccurses_wide_flag
                 ||
                 maccurses_widefont_flag)
                &&
                ((i = (maccurses_cvtt[j] & ~(maccurses_wide_flag ? 0x100 : 0))) <= 0xff)
                &&
                (i >= 0x20)
                &&
                ((i <= 0x7a) || (i > 0x7f))
                &&
                (i != 0xdb)
                &&
                (i != 0xf3)
                &&
                (i != 0xf7)
                &&
                (i != '#')
                &&
                (i != '$')
                &&
                (i != '\\'))
            {
                Str255 si;
                short wid;

                if ((maccurses_cvtt[j + 1] & ~_MACCURSES_A_ATTR) <= 0xff)
                {
                    si[0] = 1;
                    si[1] = maccurses_cvtt[j + 1] & 0xff;
                }
                else
                {
                    si[0] = 2;
                    si[1] = (maccurses_cvtt[j + 1] >> 8) & 0xff;
                    si[1] = maccurses_cvtt[j + 1] & 0xff;
                }
                wid = StringWidth(si);
                if (wid > maccurses_widMax)
                {
                    maccurses_widMax = wid;
                }
            }
        }
    }
#endif /* ! defined(UNICODE) */
    if (! maccurses_widMax) maccurses_widMax = maccurses_fontinfo.widMax;
    if (myman_getenv("MACCURSES_SQUARE")
        &&
        *myman_getenv("MACCURSES_SQUARE")
        &&
        atoi(myman_getenv("MACCURSES_SQUARE")))
    {
        maccurses_widMax = (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
        if ((maccurses_wide_flag == FALSE)
            &&
            (maccurses_widefont_flag == FALSE))
        {
            maccurses_widMax /= 2;
        }
        if (maccurses_widMax < 1)
        {
            maccurses_widMax = 1;
        }
    }
    if ((w * maccurses_widMax) > 4096)
    {
        w = 4096 / maccurses_widMax;
    }
    if ((h * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent)) > 2048)
    {
        h = 2048 / (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
    }
    maccurses_size.h = w * maccurses_widMax;
    maccurses_size.v = h * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
#ifndef __CARBON__
    if (1)
    {
        Rect chrome;
        Rect gray;
        Point windPos;
        Rect screen;
        BitMap screenBits;

        GetWindowStructureWidths(maccurses_hwnd, &chrome);
        if (GetRegionBounds(GetGrayRgn(), &gray))
        {
            if (maccurses_size.h > (gray.right - gray.left - chrome.left - chrome.right))
            {
                maccurses_size.h = gray.right - gray.left - chrome.left - chrome.right;
            }
            if (maccurses_size.v > (gray.bottom - gray.top - chrome.top - chrome.bottom))
            {
                maccurses_size.v = gray.bottom - gray.top - chrome.bottom - chrome.bottom;
            }
        }
        GetQDGlobalsScreenBits(&screenBits);
        screen = screenBits.bounds;
        screen.top += GetMBarHeight();
        if (maccurses_size.h > (screen.right - screen.left - chrome.left - chrome.right))
        {
            maccurses_size.h = screen.right - screen.left - chrome.left - chrome.right;
        }
        if (maccurses_size.v > (screen.bottom - screen.top - chrome.top - chrome.bottom))
        {
            maccurses_size.v = screen.bottom - screen.top - chrome.top - chrome.bottom;
        }
        windPos.h = 0;
        windPos.v = 0;
        LocalToGlobal(&windPos);
        if (windPos.h < (screen.left + chrome.left))
        {
            windPos.h = (screen.left + chrome.left);
        }
        if (windPos.v < (screen.top + chrome.top))
        {
            windPos.v = (screen.top + chrome.top);
        }
        if ((windPos.h + maccurses_size.h) > (screen.right - chrome.right))
        {
            windPos.h -= (windPos.h + maccurses_size.h) - (screen.right - chrome.right);
        }
        if ((windPos.v + maccurses_size.v) > (screen.bottom - chrome.bottom))
        {
            windPos.v -= (windPos.v + maccurses_size.v) - (screen.bottom - chrome.bottom);
        }
        MacMoveWindow(maccurses_hwnd, windPos.h, windPos.v, false);
    }
#endif /* ! defined(__CARBON__) */
    SizeWindow(maccurses_hwnd,
               maccurses_size.h,
               maccurses_size.v,
               true);
#ifdef __CARBON__
    if (ConstrainWindowToScreen)
    {
        ConstrainWindowToScreen(maccurses_hwnd,
                                kWindowStructureRgn,
                                kWindowConstrainMayResize,
                                NULL,
                                NULL);
    }
#endif /* ! defined(__CARBON__) */
    GetWindowBounds(maccurses_hwnd,
                    kWindowContentRgn,
                    &coords);
    maccurses_device = 0;
    maccurses_world = 0;
    maccurses_pixmap = 0;
    if ((maccurses_qdversion != gestaltOriginalQD)
        &&
/* FIXME: enable this once we have incremental redraw from backing
 * store working */
        0)
    {
        Rect portBounds;

        GetWindowPortBounds(maccurses_hwnd, &portBounds);
        GetGWorld(&maccurses_port, &maccurses_device);
        if (maccurses_port
            &&
            maccurses_device
            &&
            (noErr == NewGWorld(&maccurses_world, 0,
                                &portBounds,
                                NULL, NULL,
                                0UL)))
        {
            SetGWorld(maccurses_world, NULL);
            maccurses_pixmap = GetGWorldPixMap(maccurses_world);
            if (maccurses_pixmap
                &&
                (! LockPixels(maccurses_pixmap)))
            {
                maccurses_pixmap = 0;
            }
            SetGWorld(maccurses_port, maccurses_device);
            if (! maccurses_pixmap)
            {
                DisposeGWorld(maccurses_world);
                maccurses_world = 0;
            }
        }
    }
    if (maccurses_pixmap)
    {
        SetGWorld(maccurses_world, NULL);
        TextFont(maccurses_fontfamily);
        /*TextMode(transparent);*/
        TextMode(srcCopy);
        TextFace(0);
        TextSize(maccurses_fontsize);
        SetGWorld(maccurses_port, maccurses_device);
    }
    maccurses_size.v = coords.bottom - coords.top;
    maccurses_size.h = coords.right - coords.left;
    maccurses_w = maccurses_size.h / maccurses_widMax;
    maccurses_h = maccurses_size.v / (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
    for (i = 0; i < COLORS; i ++)
        for (j = 0; j < COLORS; j ++)
        {
            maccurses_init_pair((i * COLORS) + j, i, j);
        }
    maccurses_init_pair(0, COLOR_BLACK, COLOR_WHITE);
    for (i = 0; i < COLORS; i ++)
    {
        maccurses_palette[i].red = (i & 1) ? 0xffff : 0;
        maccurses_palette[i].green = (i & 2) ? 0xffff : 0;
        maccurses_palette[i].blue = (i & 4) ? 0xffff : 0;
        if (i & PEN_BRIGHT)
        {
            maccurses_palette[i].red |= 0x4000;
            maccurses_palette[i].green |= 0x4000;
            maccurses_palette[i].blue |= 0x4000;
        }
        else
        {
            maccurses_palette[i].red -= maccurses_palette[i].red / 3;
            maccurses_palette[i].green -= maccurses_palette[i].green / 3;
            maccurses_palette[i].blue -= maccurses_palette[i].blue / 3;
        }
    }
    for (i = 0; i < COLORS; i ++)
    {
        for (j = 0; j < COLORS; j ++)
        {
            maccurses_palette[COLORS + (i * COLORS) + j].red = maccurses_mix(maccurses_palette[i].red, maccurses_palette[j].red);
            maccurses_palette[COLORS + (i * COLORS) + j].green = maccurses_mix(maccurses_palette[i].green, maccurses_palette[j].green);
            maccurses_palette[COLORS + (i * COLORS) + j].blue = maccurses_mix(maccurses_palette[i].blue, maccurses_palette[j].blue);
        }
    }
    maccurses_palette[COLORS + COLORS * COLORS].red = maccurses_mix(0xffffU, 0);
    maccurses_palette[COLORS + COLORS * COLORS].green = maccurses_mix(0xffffU, 0);
    maccurses_palette[COLORS + COLORS * COLORS].blue = maccurses_mix(0xffffU, 0);
    SetWTitle(maccurses_hwnd, maccurses_title);
    {
        long appleEventsAttrs = 0;

        if ((noErr == Gestalt(gestaltAppleEventsAttr, &appleEventsAttrs))
            &&
            (appleEventsAttrs & (1 << gestaltAppleEventsPresent)))
        {
            AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
                                  NewAEEventHandlerUPP(maccurses_onQuitEvent), 0, false);
        }
    }
    maccurses_updateFontMenu();
    maccurses_erase();
    return 1;
}

static int maccurses_endwin(void)
{
    if (maccurses_after_endwin)
    {
        return OK;
    }
    maccurses_after_endwin = 1;
    TextFont(maccurses_oldTextFont);
    TextFace(maccurses_oldTextFace);
    TextMode(maccurses_oldTextMode);
    TextSize(maccurses_oldTextSize);
    SpaceExtra(maccurses_oldSpaceExtra);
    if (maccurses_pixmap)
    {
        UnlockPixels(maccurses_pixmap);
        maccurses_pixmap = 0;
    }
    if (maccurses_world)
    {
        DisposeGWorld(maccurses_world);
        maccurses_world = 0;
    }
    if (maccurses_oldMainMenu != NULL)
    {
        SetMenuBar(maccurses_oldMainMenu);
        DisposeHandle(maccurses_oldMainMenu);
    }
    return OK;
}

static int maccurses_clearok(int flag)
{
    maccurses_clearok_flag = flag;
    return OK;
}

static int maccurses_nodelay(int flag)
{
    maccurses_nodelay_flag = flag;
    return OK;
}

static int maccurses_intrflush(int flag)
{
    maccurses_intrflush_flag = flag;
    return OK;
}

static int maccurses_idlok(int flag)
{
    maccurses_idlok_flag = flag;
    return OK;
}

static int maccurses_leaveok(int flag)
{
    maccurses_leaveok_flag = flag;
    return OK;
}

static int maccurses_getyx(int *y, int *x)
{
    *y = maccurses_y;
    *x = maccurses_x;
    return OK;
}

static int maccurses_refresh(void)
{
    if (maccurses_valid && maccurses_world && maccurses_pixmap)
    {
        const BitMap *sourceBitMap, *destBitMap;
        Rect sourceRect, destRect;

        sourceRect.top = 0;
        sourceRect.left = 0;
        sourceRect.right = maccurses_size.h;
        sourceRect.bottom = maccurses_size.v;
        destRect = sourceRect;
        sourceBitMap = GetPortBitMapForCopyBits((CGrafPtr) maccurses_world);
        destBitMap = GetPortBitMapForCopyBits(maccurses_port);
        CopyBits(sourceBitMap,
                 destBitMap,
                 &sourceRect,
                 &destRect,
                 srcCopy,
                 NULL);
    }
    return OK;
}

static int maccurses_beep(void)
{
    SysBeep(30);
    return OK;
}

static int maccurses_doMenuItem(long msr)
{
    CharParameter ch;
    MenuRef mr;
    MenuItemIndex item;

    mr = GetMenuRef(msr >> 16);
    item = msr & 0xffffL;
    ch = 0;
    {
#if USE_OLD_TOOLBOX
        static UniversalProcPtr maccurses_trapGetItemCmd;

        if (! maccurses_trapGetItemCmd)
        {
            maccurses_trapGetItemCmd = maccurses_getTrapAddress(_GetItemCmd);
        }
        if (maccurses_trapGetItemCmd == maccurses_trapUnimplemented)
        {
            Str255 itemString;

            itemString[0] = 0;
            GetMenuItemText(mr, item, itemString);
            if (itemString[0])
            {
                char *accelText;

                accelText = (char *) memchr((void *) (itemString + 1), '/', itemString[0]);
                if (accelText && accelText[1])
                {
                    ch = accelText[1];
                }
            }
        }
        else
#endif
        {
            GetItemCmd(mr, item, &ch);
        }
    }
    switch (ch)
    {
    case '-':
        maccurses_zoom --;
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        break;
    case '+':
        maccurses_zoom ++;
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        break;
    case '=':
        maccurses_zoom = 0;
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        break;
    case 'r':
    case 'R':
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        break;
    case '?':
        HiliteMenu(0);
        return '?';
    case '.':
        HiliteMenu(0);
        return 0x1b;
    case 'i':
    case 'I':
        HiliteMenu(0);
        return '!';
    case 'q':
    case 'Q':
        /* CTRL('C') */
        HiliteMenu(0);
        return 0x03;
    case 't':
    case 'T':
        maccurses_custom_fontfamily = kInvalidFontFamily;
        maccurses_custom_fontstyle = 0;
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        break;
    case 'z':
    case 'Z':
    case '\r':
    case '\n':
        if (1)
        {
            if (maccurses_maximized)
            {
                maccurses_maximized = false;
                maccurses_new_w = maccurses_ideal_w;
                maccurses_new_h = maccurses_ideal_h;
                SetWindowBounds(maccurses_hwnd,
                                kWindowContentRgn,
                                &maccurses_ideal_rect);
            }
            else
            {
                maccurses_maximized = true;
                maccurses_ideal_w = maccurses_w;
                maccurses_ideal_h = maccurses_h;
                maccurses_new_w = 4096 / maccurses_widMax;
                maccurses_new_h = 2048 / (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
                GetWindowBounds(maccurses_hwnd,
                                kWindowContentRgn,
                                &maccurses_ideal_rect);
            }
            maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
            break;
        }
    default:
        if ((msr >> 16) >= MACCURSES_FONTMENU)
        {
            if (noErr == GetFontFamilyFromMenuSelection(mr, item, &maccurses_custom_fontfamily, &maccurses_custom_fontstyle))
            {
                maccurses_new_w = maccurses_w;
                maccurses_new_h = maccurses_h;
                maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
            }
        }
#ifndef __CARBON__
        else if ((msr >> 16) == MACCURSES_F8FFMENU)
        {
            Str255 itemName;

            GetMenuItemText(mr, item, itemName);
            OpenDeskAcc(itemName);
            SetPort((GrafPtr) maccurses_port);
        }
#endif /* ! defined(__CARBON__) */
        else
        {
            maccurses_beep();
        }
        break;
    }
    HiliteMenu(0);
    return ERR;
}

static int maccurses_getch(void)
{
    EventRecord er;
    int ret = ERR;

    if (maccurses_gotQuitEvent == TRUE)
    {
        maccurses_gotQuitEvent = FALSE;
        return 'q';
    }
    if (maccurses_resize_pending
        &&
        (! (-- maccurses_resize_pending)))
    {
        ret = KEY_RESIZE;
    }
    if (! maccurses_valid)
    {
        return ret;
    }
    if (IsWindowCollapsed(maccurses_hwnd)
        &&
        (! maccurses_resize_pending))
    {
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
    }
    while (WaitNextEvent(keyDownMask | autoKeyMask | activMask | (maccurses_pixmap ? updateMask : 0) | mUpMask | mDownMask | osMask | highLevelEventMask,
                         &er, IsWindowCollapsed(maccurses_hwnd) ? 30 : 0, NULL)
           ||
           IsWindowCollapsed(maccurses_hwnd))
    {
        if (er.what == kHighLevelEvent)
        {
            AEProcessAppleEvent(&er);
            if (maccurses_gotQuitEvent == TRUE)
            {
                maccurses_gotQuitEvent = FALSE;
                return 'q';
            }
            continue;
        }
        if (er.what == updateEvt)
        {
            BeginUpdate(maccurses_hwnd);
            maccurses_refresh();
            EndUpdate(maccurses_hwnd);
            continue;
        }
        if (IsWindowCollapsed(maccurses_hwnd)
            &&
            er.what == nullEvent)
        {
            continue;
        }
        if ((er.what == osEvt)
            &&
            (! (er.message & resumeFlag)))
        {
            while (1)
            {
                WaitNextEvent(osMask | activMask | (maccurses_pixmap ? updateMask : 0) | mUpMask | mDownMask | highLevelEventMask, &er, 60, NULL);
                if (er.what == kHighLevelEvent)
                {
                    AEProcessAppleEvent(&er);
                    if (maccurses_gotQuitEvent == TRUE)
                    {
                        maccurses_gotQuitEvent = FALSE;
                        return 'q';
                    }
                    continue;
                }
                if (er.what == updateEvt)
                {
                    BeginUpdate(maccurses_hwnd);
                    maccurses_refresh();
                    EndUpdate(maccurses_hwnd);
                    continue;
                }
                if (((er.what == osEvt)
                     &&
                     (er.message & resumeFlag))
                    ||
                    (((er.what == activateEvt) || (er.what == mouseDown))
                     &&
                     (er.modifiers & activeFlag)))
                {
                    maccurses_new_w = maccurses_w;
                    maccurses_new_h = maccurses_h;
                    maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
                    maccurses_updateFontMenu();
                    break;
                }
            }
            SetPort((GrafPtr) maccurses_port);
        }
        if (er.what == mouseDown)
        {
            WindowPartCode wpc;
            WindowRef win;

            win = 0;
            wpc = FindWindow(er.where, &win);
            if (win != maccurses_hwnd)
            {
                switch (wpc)
                {
                case inMenuBar:
                {
                    long msr;

                    msr = MenuSelect(er.where);
                    if (msr >> 16)
                    {
                        ret = maccurses_doMenuItem(msr);
                        if (ret != ERR)
                        {
                            return ret;
                        }
                    }
                    break;
                }
#ifndef __CARBON__
                case inSysWindow:
                {
                    SystemClick(&er, win);
                    break;
                }
#endif
                }
            }
            else
            {
                switch (wpc)
                {
                case inDesk:
                    break;
                case inMenuBar:
                {
                    long msr;

                    msr = MenuSelect(er.where);
                    if (msr >> 16)
                    {
                        ret = maccurses_doMenuItem(msr);
                        if (ret != ERR)
                        {
                            return ret;
                        }
                    }
                    break;
                }
                case inSysWindow:
                    break;
                case inContent:
                    SelectWindow(maccurses_hwnd);
                    ShowWindow(maccurses_hwnd);
                    break;
                case inDrag:
                    if (1)
                    {
                        Rect dragbounds;

                        GetRegionBounds(GetGrayRgn(), &dragbounds);
                        DragWindow(win, er.where, &dragbounds);
                    }
                    break;
                case inGrow:
                    if (1)
                    {
                        Rect resizebounds;

                        if (ResizeWindow(win, er.where, 0, &resizebounds))
                        {
                            maccurses_size.h = resizebounds.right - resizebounds.left;
                            maccurses_size.v = resizebounds.bottom - resizebounds.top;
                            maccurses_new_w = maccurses_size.h / maccurses_widMax;
                            maccurses_new_h = maccurses_size.v / (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
                            if ((maccurses_new_w != maccurses_w)
                                ||
                                (maccurses_new_h != maccurses_h))
                            {
                                maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
                                continue;
                            }
                        }
                    }
                    break;
                case inGoAway:
                    if (TrackGoAway(win, er.where))
                    {
                        return 0x3;
                    }
                    break;
                case inZoomIn:
                case inZoomOut:
                    if (TrackBox(win, er.where, wpc))
                    {
                        if (maccurses_maximized)
                        {
                            maccurses_maximized = false;
                            maccurses_new_w = maccurses_ideal_w;
                            maccurses_new_h = maccurses_ideal_h;
                            SetWindowBounds(maccurses_hwnd,
                                            kWindowContentRgn,
                                            &maccurses_ideal_rect);
                        }
                        else
                        {
                            maccurses_maximized = true;
                            maccurses_ideal_w = maccurses_w;
                            maccurses_ideal_h = maccurses_h;
                            maccurses_new_w = 4096 / maccurses_widMax;
                            maccurses_new_h = 2048 / (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
                            GetWindowBounds(maccurses_hwnd,
                                            kWindowContentRgn,
                                            &maccurses_ideal_rect);
                        }
                        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
                        continue;
                    }
                    break;
                case inCollapseBox:
                    if (TrackBox(win, er.where, wpc))
                    {
#if ! (defined(CollapseWindow) || USE_OLD_TOOLBOX)
                        if (CollapseWindow)
#endif
                        {
                            CollapseWindow(win, IsWindowCollapsed(win) ? false : true);
                        }
                        maccurses_new_w = maccurses_w;
                        maccurses_new_h = maccurses_h;
                        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
                        continue;
                    }
                    break;
                case inProxyIcon:
                    break;
                }
            }
        }
        else if ((er.what == keyDown)
                 ||
                 (er.what == autoKey))
        {
            if (er.modifiers & cmdKey)
            {
                UInt32 msr;

                /* The Carbon version of MenuEvent works correctly for
                 * Shift-Command-<Key> combinations; the older
                 * Appearance Manager version does not, and
                 * furthermore does not exist on pre-Appearance
                 * Manager versions of the system. */
#ifdef __CARBON__
                msr = MenuEvent(&er);
#else /* ! defined(__CARBON__) */
                UInt16 ch;

                ch = er.message & charCodeMask;
                /* Handle other modifiers by trying to re-map the
                 * keypress without the Command key; we only use the
                 * result if it's plain-ASCII. */
                if (er.modifiers & ~cmdKey)
                {
                    void *keymap;

                    keymap = (void *) GetScriptManagerVariable(smKCHRCache);
                    if (keymap)
                    {
                        UInt32 keystate, nch;

                        keystate = 0;
                        {
#if USE_OLD_TOOLBOX
                            static UniversalProcPtr maccurses_trapKeyTrans;

                            nch = 0;
                            if (! maccurses_trapKeyTrans)
                            {
                                maccurses_trapKeyTrans = maccurses_getTrapAddress(_KeyTrans);
                            }
                            if (maccurses_trapKeyTrans != maccurses_trapUnimplemented)
#endif
                            {
                                nch = KeyTranslate(keymap,
                                                   ((er.message & keyCodeMask) >> 8) | (er.modifiers & ~(cmdKey | 0xff)),
                                                   &keystate);
                            }
                        }
                        if ((nch > 0)
                            &&
                            (nch <= 0x7f))
                        {
                            ch = nch;
                        }
                    }
                }
                msr = MenuKey(ch);
#endif /* ! defined(__CARBON__) */
                if (msr >> 16)
                {
                    ret = maccurses_doMenuItem(msr);
                    if (ret != ERR)
                    {
                        return ret;
                    }
                }
                continue;
            }
            switch (er.message & charCodeMask)
            {
            case kLeftArrowCharCode: return KEY_LEFT;
            case kRightArrowCharCode: return KEY_RIGHT;
            case kUpArrowCharCode: return KEY_UP;
            case kDownArrowCharCode: return KEY_DOWN;
            case kDeleteCharCode: return 0x7f;
            default:
                return er.message & charCodeMask;
            }
        }
    }
    return ret;
}

static int maccurses_resizeterm(int lines, int columns)
{
    maccurses_h = lines;
    maccurses_w = columns;
    return OK;
}

static int maccurses_move(int y, int x)
{
    maccurses_y = y;
    maccurses_x = x;
    return OK;
}

static int maccurses_init_pair(short pair, short f, short b)
{
    if (! maccurses_has_colors())
    {
        return ERR;
    }
    if ((pair < 0)
        ||
        (pair >= COLOR_PAIRS)
        ||
        (f < 0)
        ||
        (f >= COLORS)
        ||
        (b < 0)
        ||
        (b >= COLORS))
    {
        return ERR;
    }
    maccurses_pairs[pair][0] = f;
    maccurses_pairs[pair][1] = b;
    return OK;
}

static int maccurses_pair_content(short pair, short *fp, short *bp)
{
    if (! maccurses_has_colors())
    {
        return ERR;
    }
    if ((pair < 0)
        ||
        (pair >= COLOR_PAIRS))
    {
        return ERR;
    }
    *fp = maccurses_pairs[pair][0];
    *bp = maccurses_pairs[pair][1];
    return OK;
}

static int maccurses_addch(maccurses_chtype ch)
{
    Rect cell;
    Str255 si;
    short fg, bg;
    int wcw;

    if (! maccurses_use_acs_flag)
    {
        ch &= ~_MACCURSES_A_ALTCHARSET;
    }
    if (ch == '\n')
    {
        maccurses_move(maccurses_y + 1, 0);
        return OK;
    }
    if (ch == '\r')
    {
        maccurses_move(maccurses_y, 0);
        return OK;
    }
    if (ch == '\t')
    {
        do
        {
            maccurses_addch(' ');
        }
        while (maccurses_x % 8);
        return OK;
    }
    if (maccurses_x >= maccurses_w)
    {
        maccurses_x = 0;
        maccurses_y ++;
    }
    if (maccurses_y >= maccurses_h)
    {
        maccurses_y = maccurses_h - 1;
    }
    if (maccurses_x < 0)
    {
        maccurses_x = maccurses_w - 1;
        maccurses_y --;
    }
    if (maccurses_y < 0)
    {
        maccurses_y = 0;
    }
    wcw = maccurses_wcwidth(ch);
    if (wcw < 1)
    {
        return ERR;
    }
    if (maccurses_valid)
    {
        Boolean substituted = false;
        Boolean usefake = false;
        maccurses_attr_t attr, attr_extra;
        Pattern black;
        CGrafPtr oldPort;
        GDHandle oldDevice;

        oldPort = 0;
        oldDevice = 0;
        if (maccurses_world && maccurses_pixmap)
        {
            GetGWorld(&oldPort, &oldDevice);
            SetGWorld(maccurses_world, NULL);
        }
        GetQDGlobalsBlack(&black);
        attr = maccurses_attr | (ch & _MACCURSES_A_ATTR);
        ch &= _MACCURSES_A_CHARTEXT;
        attr_extra = 0;
        si[0] = 1;
        if ((ch >= 0x20) && (ch < 0x7f))
        {
            si[1] = ch;
            if (ch == ' ')
            {
                usefake = true;
            }
        }
        else
        {
            substituted = true;
            si[1] = '?';
            switch ((ch & _MACCURSES_A_ALTCHARSET) ? ch : (ch | _MACCURSES_A_ALTCHARSET))
            {
            case ACS_LRCORNER:
            case ACS_URCORNER:
            case ACS_ULCORNER:
            case ACS_LLCORNER:
            case ACS_PLUS:
            case ACS_LTEE:
            case ACS_RTEE:
            case ACS_BTEE:
            case ACS_TTEE:
                si[1] = '+';
                break;
            case ACS_HLINE:
                si[1] = '-';
                break;
            case ACS_VLINE:
                si[1] = '|';
                break;
            case _MACCURSES_ACS_MIDDOT:
                si[1] = '.';
                break;
            case _MACCURSES_ACS_PUCE:
            case _MACCURSES_ACS_BLACKSQUARE:
                si[1] = 'o';
                break;
            case ACS_BLOCK:
                si[1] = ' ';
                attr_extra = A_REVERSE;
                usefake = true;
                break;
            case _MACCURSES_ACS_THALF:
                si[1] = '\"';
                usefake = true;
                break;
            case _MACCURSES_ACS_BHALF:
                si[1] = ',';
                usefake = true;
                break;
            case _MACCURSES_ACS_LHALF:
            case _MACCURSES_ACS_RHALF:
                usefake = true;
                si[1] = '#';
                break;
            }
        }
#ifndef UNICODE
        if ((((ch & _MACCURSES_A_ALTCHARSET) != _MACCURSES_A_ALTCHARSET)
             ||
             (maccurses_wcwidth(ch ^ _MACCURSES_A_ALTCHARSET) == 1))
            &&
            maccurses_cvtt)
        {
            int j;

            if (! maccurses_cvtt[0])
            {
                substituted = false;
                attr_extra = 0;
                si[1] = ch;
            }
            for (j = 0; maccurses_cvtt[j]; j += 2)
            {
                if ((maccurses_cvtt[j] & ~(maccurses_wide_flag ? 0x100 : 0))
                    ==
                    (((ch & _MACCURSES_A_ALTCHARSET) == _MACCURSES_A_ALTCHARSET)
                     ?
                     (ch ^ _MACCURSES_A_ALTCHARSET)
                     :
                     ch))
                {
                    if (((maccurses_cvtt[j + 1] & ~_MACCURSES_A_ATTR) > 0xff)
                        &&
                        (ch & _MACCURSES_A_ALTCHARSET) != _MACCURSES_A_ALTCHARSET)
                    {
                        substituted = false;
                        usefake = false;
                        attr_extra = maccurses_cvtt[j + 1] & _MACCURSES_A_ATTR;
                        si[0] = 2;
                        si[1] = (maccurses_cvtt[j + 1] >> 8) & 0xff;
                        si[2] = maccurses_cvtt[j + 1] & 0xff;
                    }
                    else if ((maccurses_cvtt[j + 1] & ~_MACCURSES_A_ATTR) <= 0xff)
                    {
                        substituted = false;
                        usefake = false;
                        attr_extra = maccurses_cvtt[j + 1] & _MACCURSES_A_ATTR;
                        si[1] = maccurses_cvtt[j + 1] & 0xff;
                    }
                    break;
                }
            }
        }
#endif /* ! defined(UNICODE) */
        attr ^= attr_extra;
        fg = COLOR_WHITE | PEN_BRIGHT;
        bg = COLOR_BLACK;
        if ((attr & _MACCURSES_A_COLOR) && maccurses_has_colors())
        {
            short pair;

            pair = PAIR_NUMBER(attr);
            maccurses_pair_content(pair, &fg, &bg);
        }
        if (attr & A_REVERSE)
        {
            int tmp;

            tmp = fg;
            fg = bg;
            bg = tmp;
        }
        if (fg == bg)
        {
            fg = COLOR_BLACK;
        }
        if (fg == bg)
        {
            bg = COLOR_WHITE;
        }
        PenPat(&black);
        ForeColor(_COLOR_TO_MAC(fg & 7, whiteColor));
        BackColor(_COLOR_TO_MAC(bg & 7, blackColor));
        if ((attr & _MACCURSES_A_COLOR) && maccurses_has_colors())
        {
            RGBForeColor(maccurses_palette + fg);
            RGBBackColor(maccurses_palette + bg);
        }
        TextFace(maccurses_custom_fontstyle
                 |
                 ((attr & A_UNDERLINE) ? underline : 0)
                 |
                 ((attr & A_BOLD) ? bold : 0));
        SetRect(&cell,
                maccurses_x * maccurses_widMax,
                maccurses_y * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent),
                (maccurses_x + wcw) * maccurses_widMax,
                (maccurses_y + 1) * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent));
        EraseRect(&cell);
#ifdef UNICODE
        if (((ch & _MACCURSES_A_ALTCHARSET) != _MACCURSES_A_ALTCHARSET)
            &&
            (! usefake))
        {
            ATSUStyle sty;
            Boolean atsuok = false;
            UniChar uc[3];
            UniCharCount unilen;
            maccurses_chtype chuni;
            ATSUTextLayout lyo;
            ATSUAttributeTag fontSizeAttrTag, fontAttrTag;
            Fixed fontSizeAttrValue;
            ATSUFontID fontAttrValue;
            FMFontStyle whatwegot;
            ByteCount fontSizeAttrValueSize, fontAttrValueSize;
            ATSUAttributeValuePtr fontSizeAttrValuePtr, fontAttrValuePtr;
            Rect dimen;

            if (noErr == ATSUCreateStyle(&sty))
            {
                chuni = (((ch & _MACCURSES_A_ALTCHARSET) == _MACCURSES_A_ALTCHARSET)
                         ?
                         (ch ^ _MACCURSES_A_ALTCHARSET)
                         :
                         ch);
                if ((chuni < 0) || (chuni > 0x10fffdL) || ((chuni & 0xfffeL) == 0xfffeL))
                {
                    chuni = 0xfffd;
                }
                unilen = 1 + (chuni > 0xffffL);
                uc[0] = (chuni > 0xffffL) ? (((chuni - 0x10000L) >> 10) | 0xd800L) : chuni;
                uc[1] = (chuni > 0xffffL) ? (((chuni - 0x10000L) & 0x3ffL) | 0xd800L) : 0;
                uc[2] = 0;
                fontSizeAttrTag = kATSUSizeTag;
                fontSizeAttrValueSize = sizeof(fontSizeAttrValue);
                fontSizeAttrValuePtr = &fontSizeAttrValue;
                fontSizeAttrValue = maccurses_fontsize << 16;
                fontAttrTag = kATSUFontTag;
                fontAttrValueSize = sizeof(fontAttrValue);
                fontAttrValuePtr = &fontAttrValue;
                if ((noErr == FMGetFontFromFontFamilyInstance(maccurses_fontfamily,
                                                              maccurses_custom_fontstyle
                                                              |
                                                              ((attr & A_UNDERLINE) ? underline : 0)
                                                              |
                                                              ((attr & A_BOLD) ? bold : 0),
                                                              &fontAttrValue,
                                                              &whatwegot))
                    &&
                    (noErr == ATSUSetAttributes(sty, 1, &fontAttrTag, &fontAttrValueSize, &fontAttrValuePtr))
                    &&
                    (noErr == ATSUSetAttributes(sty, 1, &fontSizeAttrTag, &fontSizeAttrValueSize, &fontSizeAttrValuePtr))
                    &&
                    (noErr == ATSUCreateTextLayoutWithTextPtr(uc,
                                                              kATSUFromTextBeginning,
                                                              kATSUToTextEnd,
                                                              unilen,
                                                              1,
                                                              &unilen,
                                                              &sty,
                                                              &lyo)))
                {
                    ATSUFontID substFontAttrValue = kATSUInvalidFontID;
                    UniCharArrayOffset firstChanged = 0;
                    UniCharCount numChanged = 0;

                    if (noErr == ATSUMatchFontsToText(lyo,
                                                      kATSUFromTextBeginning,
                                                      kATSUToTextEnd,
                                                      &substFontAttrValue,
                                                      &firstChanged,
                                                      &numChanged))
                    {
                        ATSUSetTransientFontMatching(lyo, true);
                        if (noErr == ATSUMeasureTextImage(lyo,
                                                          kATSUFromTextBeginning,
                                                          kATSUToTextEnd,
                                                          0,
                                                          0,
                                                          &dimen))
                        {
                            MoveTo(maccurses_x * maccurses_widMax /* + (wcw * maccurses_widMax - (dimen.right - dimen.left)) / 2 */,
                                   maccurses_y * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent) + maccurses_fontinfo.ascent);
                            if (noErr == ATSUDrawText(lyo,
                                                      kATSUFromTextBeginning,
                                                      kATSUToTextEnd,
                                                      kATSUUseGrafPortPenLoc,
                                                      kATSUUseGrafPortPenLoc))
                            {
                                /* woot. */
                                atsuok = true;
                            }
                        }
                    }
                    ATSUDisposeTextLayout(lyo);
                }
                ATSUDisposeStyle(sty);
            }
            if (atsuok != false)
            {
                goto done;
            }
        }
#endif /* defined(UNICODE) */
        MoveTo(maccurses_x * maccurses_widMax + (wcw * maccurses_widMax - StringWidth(si)) / 2,
               maccurses_y * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent) + maccurses_fontinfo.ascent);
        if (((substituted == true)
             ||
             usefake
             ||
             (maccurses_widMax < 2)
             ||
             ((maccurses_fontinfo.ascent + maccurses_fontinfo.descent) < 2))
            &&
            (maccurses_use_acs_flag == TRUE))
        {
            int x0, x1, x2, y0, y1, y2;
            maccurses_chtype chacs;

            if ((maccurses_widMax < 2)
                ||
                ((maccurses_fontinfo.ascent + maccurses_fontinfo.descent) < 2))
            {
                switch (ch)
                {
                case '\"':
                    ch = _MACCURSES_ACS_THALF;
                    break;
                case ',':
                    ch = _MACCURSES_ACS_BHALF;
                    break;
                case 'o':
                    ch = _MACCURSES_ACS_PUCE;
                    break;
                case '.':
                    ch = _MACCURSES_ACS_MIDDOT;
                    break;
                case '+':
                    ch = ACS_PLUS;
                    break;
                case '-':
                    ch = ACS_HLINE;
                    break;
                case '|':
                    ch = ACS_VLINE;
                    break;
                }
            }
            x0 = maccurses_x * maccurses_widMax;
            x2 = x0 + wcw * maccurses_widMax;
            x1 = (x0 + x2) / 2;
            y0 = maccurses_y * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
            y2 = y0 + maccurses_fontinfo.ascent + maccurses_fontinfo.descent;
            y1 = (y0 + y2) / 2;
            chacs = ((ch & _MACCURSES_A_ALTCHARSET)
                     ?
                     ch
                     :
                     (ch | _MACCURSES_A_ALTCHARSET));
            switch (chacs)
            {
            case (' ' | _MACCURSES_A_ALTCHARSET):
                break;
            case ACS_LRCORNER:
            case ACS_URCORNER:
            case ACS_ULCORNER:
            case ACS_LLCORNER:
            case ACS_PLUS:
            case ACS_LTEE:
            case ACS_RTEE:
            case ACS_BTEE:
            case ACS_TTEE:
            case ACS_HLINE:
            case ACS_VLINE:
                if (1)
                {
                    int u, d, l, r;

                    u = ((chacs == ACS_LRCORNER)
                         ||
                         (chacs == ACS_LLCORNER)
                         ||
                         (chacs == ACS_PLUS)
                         ||
                         (chacs == ACS_BTEE)
                         ||
                         (chacs == ACS_LTEE)
                         ||
                         (chacs == ACS_RTEE)
                         ||
                         (chacs == ACS_VLINE));
                    d = ((chacs == ACS_URCORNER)
                         ||
                         (chacs == ACS_ULCORNER)
                         ||
                         (chacs == ACS_PLUS)
                         ||
                         (chacs == ACS_TTEE)
                         ||
                         (chacs == ACS_LTEE)
                         ||
                         (chacs == ACS_RTEE)
                         ||
                         (chacs == ACS_VLINE));
                    l = ((chacs == ACS_LRCORNER)
                         ||
                         (chacs == ACS_URCORNER)
                         ||
                         (chacs == ACS_PLUS)
                         ||
                         (chacs == ACS_TTEE)
                         ||
                         (chacs == ACS_BTEE)
                         ||
                         (chacs == ACS_RTEE)
                         ||
                         (chacs == ACS_HLINE));
                    r = ((chacs == ACS_LLCORNER)
                         ||
                         (chacs == ACS_ULCORNER)
                         ||
                         (chacs == ACS_PLUS)
                         ||
                         (chacs == ACS_TTEE)
                         ||
                         (chacs == ACS_BTEE)
                         ||
                         (chacs == ACS_LTEE)
                         ||
                         (chacs == ACS_HLINE));
                    if (u)
                    {
                        MoveTo(x1, y0);
                        LineTo(x1, y1);
                    }
                    if (d)
                    {
                        MoveTo(x1, y1);
                        LineTo(x1, y2 - 1);
                    }
                    if (l)
                    {
                        MoveTo(x0, y1);
                        LineTo(x1, y1);
                    }
                    if (r)
                    {
                        MoveTo(x1, y1);
                        LineTo(x2 - 1, y1);
                    }
                }
                break;
            case _MACCURSES_ACS_MIDDOT:
                if (1)
                {
                    int bullet_radius;

                    bullet_radius = (x1 - x0) / 2;
                    if (((y1 - y0) / 2) < bullet_radius)
                    {
                        bullet_radius = (y1 - y0) / 2;
                    }
                    if (bullet_radius > 1) bullet_radius = 1;
                    if (bullet_radius)
                    {
                        SetRect(&cell, x1 - bullet_radius, y1 - bullet_radius, x1 + bullet_radius, y1 + bullet_radius);
                    }
                    else
                    {
                        SetRect(&cell, x1, y1, x1 + 1, y1 + 1);
                    }
                    PaintOval(&cell);
                }
                break;
            case _MACCURSES_ACS_PUCE:
            case _MACCURSES_ACS_BLACKSQUARE:
                if (1)
                {
                    int puce_radius;

                    puce_radius = x1 - x0;
                    if ((y1 - y0) < puce_radius)
                    {
                        puce_radius = y1 - y0;
                    }
                    if (puce_radius)
                    {
                        SetRect(&cell, x1 - puce_radius, y1 - puce_radius, x1 + puce_radius, y1 + puce_radius);
                    }
                    else
                    {
                        SetRect(&cell, x1, y1, x1 + 1, y1 + 1);
                    }
                    PaintOval(&cell);
                }
                break;
            case _MACCURSES_ACS_THALF:
            case _MACCURSES_ACS_BHALF:
                if (y2 == (y0 + 1))
                {
                    if ((attr & _MACCURSES_A_COLOR) && maccurses_has_colors())
                    {
                        RGBForeColor(maccurses_palette + COLORS + fg + COLORS * bg);
                        RGBBackColor(maccurses_palette + COLORS + fg + COLORS * bg);
                    }
                    else if (maccurses_has_colors())
                    {
                        RGBForeColor(maccurses_palette + COLORS + COLORS * COLORS);
                        RGBBackColor(maccurses_palette + COLORS + COLORS * COLORS);
                    }
                    EraseRect(&cell);
                }
                if (chacs == _MACCURSES_ACS_THALF)
                {
                    SetRect(&cell, x0, y0, x2, y1 + (y0 == y1));
                }
                else if (chacs == _MACCURSES_ACS_BHALF)
                {
                    SetRect(&cell, x0, y1, x2, y2);
                }
                PaintRect(&cell);
                break;
            case _MACCURSES_ACS_LHALF:
            case _MACCURSES_ACS_RHALF:
                if (x2 == (x0 + 1))
                {
                    if ((attr & _MACCURSES_A_COLOR) && maccurses_has_colors())
                    {
                        RGBForeColor(maccurses_palette + COLORS + fg + COLORS * bg);
                        RGBBackColor(maccurses_palette + COLORS + fg + COLORS * bg);
                    }
                    else if (maccurses_has_colors())
                    {
                        RGBForeColor(maccurses_palette + COLORS + COLORS * COLORS);
                        RGBBackColor(maccurses_palette + COLORS + COLORS * COLORS);
                    }
                    EraseRect(&cell);
                }
                if (chacs == _MACCURSES_ACS_LHALF)
                {
                    SetRect(&cell, x0, y0, x1 + (x0 == x1), y2);
                }
                else if (chacs == _MACCURSES_ACS_RHALF)
                {
                    SetRect(&cell, x1, y0, x2, y2);
                }
                PaintRect(&cell);
                break;
            case ACS_BLOCK:
                if (! substituted)
                {
                    PaintRect(&cell);
                }
                break;
            default:
                if ((maccurses_widMax < 2)
                    ||
                    ((maccurses_fontinfo.ascent + maccurses_fontinfo.descent) < 2))
                {
                    if ((ch != ' ')
                        &&
                        (si[1] != ' '))
                    {
                        PaintRect(&cell);
                    }
                }
                else
                {
                    DrawString(si);
                }
            }
        }
        else
        {
            DrawString(si);
        }
#ifdef UNICODE
      done:
#endif /* defined(UNICODE) */
        if (maccurses_world && maccurses_pixmap)
        {
            SetGWorld(oldPort, oldDevice);
        }
    }
    maccurses_move(maccurses_y, maccurses_x + wcw);
    return OK;
}

static int maccurses_addstr(const char *s)
{
    while (*s)
    {
        maccurses_addch((maccurses_chtype) *s);
        s ++;
    }
    return OK;
}

static int maccurses_mvprintw(int y, int x, const char *s)
{
    maccurses_move(y, x);
    maccurses_addstr(s);
    return OK;
}

static int maccurses_clrtoeol(void)
{
    int ox, oy;
    int x;

    maccurses_getyx(&oy, &ox);
    for (x = ox; x < maccurses_w; x ++)
    {
        maccurses_move(oy, x);
        maccurses_addch(' ');
    }
    maccurses_move(oy, ox);
    return OK;
}

static int maccurses_clrtobot(void)
{
    int ox, oy;
    int y;

    maccurses_clrtoeol();
    maccurses_getyx(&oy, &ox);
    for (y = oy + 1; y < maccurses_h; y ++)
    {
        maccurses_move(y, 0);
        maccurses_clrtoeol();
    }
    maccurses_move(oy, ox);
    return OK;
}

static int maccurses_erase(void)
{
    Rect coords;
    CGrafPtr oldPort;
    GDHandle oldDevice;

    if (! maccurses_valid)
    {
        return ERR;
    }
    maccurses_clrtoeol();
    GetWindowBounds(maccurses_hwnd,
                    kWindowContentRgn,
                    &coords);
    coords.right -= coords.left;
    coords.left -= coords.left;
    coords.bottom -= coords.top;
    coords.top -= coords.top;
    oldPort = 0;
    oldDevice = 0;
    if (maccurses_world && maccurses_pixmap)
    {
        GetGWorld(&oldPort, &oldDevice);
        SetGWorld(maccurses_world, NULL);
    }
    EraseRect(&coords);
    if (maccurses_world && maccurses_pixmap)
    {
        SetGWorld(oldPort, oldDevice);
    }
    return OK;
}

static int maccurses_clear(void)
{
    return maccurses_erase();
}

static int maccurses_attrset(maccurses_attr_t a)
{
    maccurses_attr = a;
    return OK;
}

static int maccurses_insch(maccurses_chtype ch)
{
    if (ch)
    {
        /* we should care... */
    }
    return ERR;
}

static int maccurses_delch(void)
{
    return ERR;
}

static int maccurses_init_color(short i, short r, short g, short b)
{
    int j;

    if (! maccurses_can_change_color())
    {
        return ERR;
    }
    if ((i < 0)
        ||
        (i >= COLORS))
    {
        return ERR;
    }
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (r > 1000) r = 1000;
    if (g > 1000) g = 1000;
    if (b > 1000) b = 1000;
    maccurses_palette[i].red = (unsigned short) (65535UL * (unsigned long) r / 1000UL);
    maccurses_palette[i].green = (unsigned short) (65535UL * (unsigned long) g / 1000UL);
    maccurses_palette[i].blue = (unsigned short) (65535UL * (unsigned long) b / 1000UL);
    for (j = 0; j < COLORS; j ++)
    {
        maccurses_palette[COLORS + (i * COLORS) + j].red = maccurses_mix(maccurses_palette[i].red, maccurses_palette[j].red);
        maccurses_palette[COLORS + (i * COLORS) + j].green = maccurses_mix(maccurses_palette[i].green, maccurses_palette[j].green);
        maccurses_palette[COLORS + (i * COLORS) + j].blue = maccurses_mix(maccurses_palette[i].blue, maccurses_palette[j].blue);
        maccurses_palette[COLORS + (j * COLORS) + i].red = maccurses_mix(maccurses_palette[i].red, maccurses_palette[j].red);
        maccurses_palette[COLORS + (j * COLORS) + i].green = maccurses_mix(maccurses_palette[i].green, maccurses_palette[j].green);
        maccurses_palette[COLORS + (j * COLORS) + i].blue = maccurses_mix(maccurses_palette[i].blue, maccurses_palette[j].blue);
    }
    return OK;
}

static int maccurses_color_content(short i, short *rp, short *gp, short *bp)
{
    if (! maccurses_can_change_color())
    {
        return ERR;
    }
    if ((i < 0)
        ||
        (i >= COLORS))
    {
        return ERR;
    }
    *rp = (short) (1000UL * (unsigned long) maccurses_palette[i].red / 65535UL);
    *gp = (short) (1000UL * (unsigned long) maccurses_palette[i].green / 65535UL);
    *bp = (short) (1000UL * (unsigned long) maccurses_palette[i].blue / 65535UL);
    return OK;
}

static int maccurses_start_color(void)
{
    return maccurses_has_colors() ? OK : ERR;
}

#undef has_colors
#define has_colors() (maccurses_has_colors() ? TRUE : FALSE)

#undef start_color
#define start_color() maccurses_start_color()

#undef can_change_color
#define can_change_color() (maccurses_can_change_color() ? TRUE : FALSE)

#undef color_content
#define color_content(i,rp,gp,bp) maccurses_color_content(i,rp,gp,bp)

#undef init_pair
#define init_pair(p,f,b) maccurses_init_pair(p,f,b)

#undef pair_content
#define pair_content(p,fp,bp) maccurses_pair_content(p,fp,bp)

#undef clear
#define clear() maccurses_clear()

#undef clrtobot
#define clrtobot() maccurses_clrtobot()

#undef clrtoeol
#define clrtoeol() maccurses_clrtoeol()

#undef erase
#define erase() maccurses_erase()

#undef attrset
#define attrset(a) maccurses_attrset(a)

#undef standout
#define standout() maccurses_attrset(A_STANDOUT)

#undef standend
#define standend() maccurses_attrset(0)

#undef resizeterm
#define resizeterm(h,w) maccurses_resizeterm(h,w)

#undef use_env
#define use_env(flag)

#undef keypad
#define keypad(stdscr,flag)

#undef addstr
#define addstr(s) maccurses_addstr(s)

#undef addch
#define addch(ch) maccurses_addch(ch)

#undef insch
#define insch(ch) maccurses_insch(ch)

#undef delch
#define delch() maccurses_delch()

#undef move
#define move(y,x) maccurses_move(y,x)

#undef wcwidth
#define wcwidth(wc) maccurses_wcwidth(wc)

#undef getyx
#define getyx(stdscr,y,x) maccurses_getyx(&(y),&(x))

#undef clearok
#define clearok(stdscr,flag) maccurses_clearok(flag)

#undef nodelay
#define nodelay(stdscr,flag) maccurses_nodelay(flag)

#undef intrflush
#define intrflush(stdscr,flag) maccurses_intrflush(flag)

#undef idlok
#define idlok(stdscr,flag) maccurses_idlok(flag)

#undef leaveok
#define leaveok(stdscr,flag) maccurses_leaveok(flag)

#undef wrefresh
#define wrefresh(stdscr) maccurses_refresh()

#undef refresh
#define refresh() maccurses_refresh()

#undef cbreak
#define cbreak()

#undef noecho
#define noecho()

#undef echo
#define echo()

#undef nonl
#define nonl()

#undef curs_set
#define curs_set(vis)

#undef beep
#define beep() maccurses_beep()

#undef getch
#define getch() maccurses_getch()

#undef mvprintw
#define mvprintw(y,x,s) maccurses_mvprintw(y,x,s)

#undef init_color
#define init_color(i,r,g,b) maccurses_init_color(i,r,g,b)

#define INITSCR_WITH_HINTS 1

#undef initscrWithHints
#define initscrWithHints maccurses_initscrWithHints

#undef endwin
#define endwin() maccurses_endwin()

#undef chtype
#define chtype maccurses_chtype

#undef attr_t
#define attr_t maccurses_attr_t

#undef COLS
#define COLS maccurses_w

#undef LINES
#define LINES maccurses_h

#ifndef USE_IOCTL
#define USE_IOCTL 0
#endif

#ifndef USE_WCWIDTH
#define USE_WCWIDTH 1
#endif

#ifndef USE_ICONV
#define USE_ICONV 0
#endif

#ifndef USE_SIGWINCH
#define USE_SIGWINCH 0
#endif

#ifndef USE_RAW_UCS
#ifdef UNICODE
#define USE_RAW_UCS 1
#else
#define USE_RAW_UCS 0
#endif
#endif

#endif /* ! defined(MYMAN_MACCURS_H_INCLUDED) */
