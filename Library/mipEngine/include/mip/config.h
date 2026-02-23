#pragma once

//#define USE_DX9
#define USE_OPENGL
#define USE_QT
#define USE_RIGHT_HAND

#ifdef USE_QT
// ----------------------------- QT Code



// -----------------------------
#else
// ------------------------------ WX Widgets
#ifdef __WXUNIVERSAL__
#define USE_WXUNIVERSAL 1
#else
#define USE_WXUNIVERSAL 0
#endif

#ifdef WXUSINGDLL
#define USE_DLL 1
#else
#define USE_DLL 0
#endif

#if defined(__WXMSW__)
#define USE_WXMSW 1
#else
#define USE_WXMSW 0
#endif

#ifdef __WXMAC__
#define USE_WXMAC 1
#else
#define USE_WXMAC 0
#endif

#if USE_NATIVE_FONT_DIALOG_FOR_MACOSX
#define USE_WXMACFONTDLG 1
#else
#define USE_WXMACFONTDLG 0
#endif

#ifdef __WXGTK__
#define USE_WXGTK 1
#else
#define USE_WXGTK 0
#endif

#define USE_GENERIC_DIALOGS (!USE_WXUNIVERSAL && !USE_DLL)

#define USE_COLOURDLG_GENERIC \
    ((USE_WXMSW || USE_WXMAC) && USE_GENERIC_DIALOGS && wxUSE_COLOURDLG)
#define USE_DIRDLG_GENERIC \
    ((USE_WXMSW || USE_WXMAC) && USE_GENERIC_DIALOGS && wxUSE_DIRDLG)
#define USE_FILEDLG_GENERIC \
    ((USE_WXMSW || USE_WXMAC) && USE_GENERIC_DIALOGS  && wxUSE_FILEDLG)
#define USE_FONTDLG_GENERIC \
    ((USE_WXMSW || USE_WXMACFONTDLG) && USE_GENERIC_DIALOGS && wxUSE_FONTDLG)

// Turn USE_MODAL_PRESENTATION to 0 if there is any reason for not presenting difference
// between modal and modeless dialogs (ie. not implemented it in your port yet)
#if !wxUSE_BOOKCTRL
#define USE_MODAL_PRESENTATION 0
#else
#define USE_MODAL_PRESENTATION 1
#endif


// Turn USE_SETTINGS_DIALOG to 0 if supported
#if wxUSE_BOOKCTRL
#define USE_SETTINGS_DIALOG 1
#else
#define USE_SETTINGS_DIALOG 0
#endif

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/string.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/toolbar.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/frame.h>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP    
#include <wx/wx.h>
#endif

#endif USE_QT
// ------------------------------ WX Widgets end


#ifdef USE_DX9
// ------------------------------ DirectX9
#include <D3dx9math.h>
#include <d3dx9.h>
#include <d3d9.h>
// ------------------------------ 
#else
#ifdef USE_OPENGL
#include <windows.h>
#else
// ------------------------------ DirectX11
#include <D3dx11effect.h>
#include "dxerr.h"
#include <D3D11.h>
//#include <D3DX11.h> // do not use
#pragma warning(disable:4005)
#endif
#endif
