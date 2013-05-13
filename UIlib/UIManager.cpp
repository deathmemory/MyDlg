
#include "StdAfx.h"
#include "UIManager.h"

#include <zmouse.h>

/////////////////////////////////////////////////////////////////////////////////////
//
//

static UINT GetNameHash(LPCTSTR pstrName)
{
   UINT i = 0;
   SIZE_T len = _tcslen(pstrName);
   while( len-- > 0 ) i = (i << 5) + i + pstrName[len];
   return i;
}

static UINT MapKeyState()
{
   UINT uState = 0;
   if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
   if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_LBUTTON;
   if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_RBUTTON;
   if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
   if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
   return uState;
}


/////////////////////////////////////////////////////////////////////////////////////
//
typedef struct tagFINDTABINFO
{
   CUIControl* pFocus;
   CUIControl* pLast;
   bool bForward;
   bool bNextIsIt;
} FINDTABINFO;

typedef struct tagFINDSHORTCUT
{
   TCHAR ch;
   bool bPickNext;
} FINDSHORTCUT;

typedef struct tagTIMERINFO
{
   CUIControl* pSender;
   UINT nLocalID;
   HWND hWnd;
   UINT uWinTimer;
} TIMERINFO;


/////////////////////////////////////////////////////////////////////////////////////
//

HPEN m_hPens[UICOLOR__LAST] = { 0 };
HFONT m_hFonts[UIFONT__LAST] = { 0 };
HBRUSH m_hBrushes[UICOLOR__LAST] = { 0 };
LOGFONT m_aLogFonts[UIFONT__LAST] = { 0 };
COLORREF m_clrColors[UICOLOR__LAST][2] = { 0 };
TEXTMETRIC m_aTextMetrics[UIFONT__LAST] = { 0 };


/////////////////////////////////////////////////////////////////////////////////////
HINSTANCE CUIPaintManager::m_hInstance = NULL;
HINSTANCE CUIPaintManager::m_hLangInst = NULL;
CStdPtrArray CUIPaintManager::m_aPreMessages;

Image* CUIPaintManager::m_pImageSkinMenu = NULL;
Image* CUIPaintManager::m_pImageSkinCombo = NULL;
Image* CUIPaintManager::m_pImageSkinCommonDialog = NULL;
Image* CUIPaintManager::m_pImageSkinCloseButton = NULL;
Image* CUIPaintManager::m_pImageSkinCommonButton = NULL;
Image* CUIPaintManager::m_pImageSkinCheckbox = NULL;
Image* CUIPaintManager::m_pImageSkinRadiobox = NULL;
Image* CUIPaintManager::m_pImageSkinVScroll = NULL;

std::map<UINT, IconInfo> CUIPaintManager::m_mapSkinCommonIcon;


CUIPaintManager::CUIPaintManager() :
   m_hWndPaint(NULL),
   m_hDcPaint(NULL),
   m_hDcOffscreen(NULL),
   m_hbmpOffscreen(NULL),
   m_hwndTooltip(NULL),
   m_uTimerID(0x1000),
   m_pRoot(NULL),
   m_pFocus(NULL),
   m_pEventHover(NULL),
   m_pEventClick(NULL),
   m_pEventKey(NULL),
   m_bFirstLayout(true),
   m_bFocusNeeded(false),
   m_bResizeNeeded(false),
   m_bMouseTracking(false),
   m_bOffscreenPaint(true),
   m_aPostPaint(sizeof(TPostPaintUI))
{
   if( m_hFonts[1] == NULL ) 
   {
      // Fill in default font information
      LOGFONT lf = { 0 };
      ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
      _tcscpy(lf.lfFaceName, _T("Tahoma"));
      // TODO: Handle "large fonts" or other font sizes when
      //       screen resolution changes!!!
      lf.lfHeight = -12;
      m_aLogFonts[UIFONT_NORMAL] = lf;
      m_aLogFonts[UIFONT_CAPTION] = lf;
      LOGFONT lfBold = lf;
      lfBold.lfWeight += FW_BOLD;
      m_aLogFonts[UIFONT_BOLD] = lfBold;
      lfBold.lfHeight -= 3;
      m_aLogFonts[UIFONT_TITLE] = lfBold;
      lfBold.lfHeight -= 4;
      m_aLogFonts[UIFONT_HEADLINE] = lfBold;
      LOGFONT lfSubscript = lf;
      lfSubscript.lfHeight -= 4;
      m_aLogFonts[UIFONT_SUBSCRIPT] = lfSubscript;
      LOGFONT lfLink = lf;
      lfLink.lfUnderline = TRUE;
      m_aLogFonts[UIFONT_LINK] = lfLink;

	  LOGFONT lfSmall = lf;
	  lfSmall.lfHeight += 2;
	  m_aLogFonts[UIFONT_SMALL] = lfSmall;

      // Fill the color table
      m_clrColors[UICOLOR_WINDOW_BACKGROUND][0]            = RGB(239,239,235);
      m_clrColors[UICOLOR_DIALOG_BACKGROUND][0]            = RGB(238,238,238);
      m_clrColors[UICOLOR_DIALOG_TEXT_NORMAL][0]           = RGB(0,0,0);
      m_clrColors[UICOLOR_DIALOG_TEXT_DARK][0]             = RGB(96,96,80);
      

      m_clrColors[UICOLOR_BUTTON_BACKGROUND_NORMAL][0]     = RGB(250,250,252);
      m_clrColors[UICOLOR_BUTTON_BACKGROUND_NORMAL][1]    = RGB(215,215,227);
      m_clrColors[UICOLOR_BUTTON_BACKGROUND_DISABLED][0]   = RGB(248,248,248);
      m_clrColors[UICOLOR_BUTTON_BACKGROUND_DISABLED][1]  = RGB(214,214,214);
      m_clrColors[UICOLOR_BUTTON_BACKGROUND_PUSHED][0]     = RGB(215,215,227);
      m_clrColors[UICOLOR_BUTTON_BACKGROUND_PUSHED][1]    = RGB(250,250,252);
      m_clrColors[UICOLOR_BUTTON_TEXT_NORMAL][0]           = RGB(0,0,0);
      m_clrColors[UICOLOR_BUTTON_TEXT_PUSHED][0]           = RGB(0,0,20);
      m_clrColors[UICOLOR_BUTTON_TEXT_DISABLED][0]         = RGB(204,204,204);
      m_clrColors[UICOLOR_BUTTON_BORDER_LIGHT][0]          = RGB(123,158,189);
      m_clrColors[UICOLOR_BUTTON_BORDER_DARK][0]           = RGB(123,158,189);
      m_clrColors[UICOLOR_BUTTON_BORDER_DISABLED][0]       = RGB(204,204,204);
      m_clrColors[UICOLOR_BUTTON_BORDER_FOCUS][0]          = RGB(140,140,140);
     

      m_clrColors[UICOLOR_EDIT_BACKGROUND_DISABLED][0]     = RGB(255,251,255);
      m_clrColors[UICOLOR_EDIT_BACKGROUND_READONLY][0]     = RGB(255,251,255);
      m_clrColors[UICOLOR_EDIT_BACKGROUND_NORMAL][0]       = RGB(255,255,255);
      m_clrColors[UICOLOR_EDIT_BACKGROUND_HOVER][0]        = RGB(255,251,255);
      m_clrColors[UICOLOR_EDIT_TEXT_NORMAL][0]             = RGB(0,0,0);
      m_clrColors[UICOLOR_EDIT_TEXT_DISABLED][0]           = RGB(167,166,170);
      m_clrColors[UICOLOR_EDIT_TEXT_READONLY][0]           = RGB(167,166,170);

	  m_clrColors[UICOLOR_EDIT_BORDER_NORMAL][0]			= RGB(0xAB,0xAD,0xB3);
	  m_clrColors[UICOLOR_EDIT_BORDER_NORMAL_DARK][0]		= RGB(0xE2,0xE3,0xEA);
	  m_clrColors[UICOLOR_EDIT_BORDER_SELECTED][0]			= RGB(0x3D,0x7B,0xAd);
	  m_clrColors[UICOLOR_EDIT_BORDER_SELECTED_DARK][0]		= RGB(0xB7,0xD9,0xED);
	  m_clrColors[UICOLOR_EDIT_BORDER_DISABLED][0]			= RGB(204,204,204);
	  m_clrColors[UICOLOR_EDIT_BORDER_DISABLED_DARK][0]			= RGB(204,204,204);
     

      m_clrColors[UICOLOR_LINK_TEXT_NORMAL][0]             = RGB(129,81,25);
      m_clrColors[UICOLOR_LINK_TEXT_HOVER][0]              = RGB(208,180,80);

	  m_clrColors[UICOLOR_HEADER_BACKGROUND][0]            = RGB(233,231,215);
	  m_clrColors[UICOLOR_HEADER_BACKGROUND][1]            = RGB(150,150,147);
	  m_clrColors[UICOLOR_HEADER_BORDER][0]                = RGB(218,219,201);
	  m_clrColors[UICOLOR_HEADER_SEPARATOR][0]             = RGB(197,193,177);
	  m_clrColors[UICOLOR_HEADER_TEXT][0]                  = RGB(0,0,0);


      m_clrColors[UICOLOR_CONTROL_BACKGROUND_NORMAL][0]    = RGB(255,255,255);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_SELECTED][0]  = RGB(173,195,231);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_READONLY][0]  = RGB(255,255,255);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_DISABLED][0]  = RGB(255,255,255);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_HOVER][0]     = RGB(233,245,255);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_SORTED][0]    = RGB(242,242,246);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_EXPANDED][0]  = RGB(255,255,255);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_EXPANDED][1]  = RGB(236,242,255);
      m_clrColors[UICOLOR_CONTROL_BORDER_NORMAL][0]        = RGB(0xAB,0xAD,0xB3);
      m_clrColors[UICOLOR_CONTROL_BORDER_SELECTED][0]      = RGB(0x3D,0x7B,0xAd);
      m_clrColors[UICOLOR_CONTROL_BORDER_DISABLED][0]      = RGB(204,204,204);
      m_clrColors[UICOLOR_CONTROL_TEXT_NORMAL][0]          = RGB(0,0,0);
      m_clrColors[UICOLOR_CONTROL_TEXT_SELECTED][0]        = RGB(0,0,0);
      m_clrColors[UICOLOR_CONTROL_TEXT_DISABLED][0]        = RGB(204,204,204);

      m_clrColors[UICOLOR_STANDARD_BLACK][0]               = RGB(0,0,0);
      m_clrColors[UICOLOR_STANDARD_YELLOW][0]              = RGB(255,255,204);
      m_clrColors[UICOLOR_STANDARD_RED][0]                 = RGB(255,204,204);
      m_clrColors[UICOLOR_STANDARD_GREY][0]                = RGB(145,146,119);
      m_clrColors[UICOLOR_STANDARD_LIGHTGREY][0]           = RGB(195,196,179);
      m_clrColors[UICOLOR_STANDARD_WHITE][0]               = RGB(255,255,255);
	  m_clrColors[UICOLOR_STANDARD_BLUE][0]                = RGB(0,0,255);


	  m_clrColors[UICOLOR_TAB_BACKGROUND_NORMAL][0]        = RGB(255,251,255);
	  m_clrColors[UICOLOR_TAB_FOLDER_NORMAL][0]            = RGB(255,251,255);
	  m_clrColors[UICOLOR_TAB_FOLDER_NORMAL][1]            = RGB(233,231,215);
	  m_clrColors[UICOLOR_TAB_FOLDER_SELECTED][0]          = RGB(255,251,255);
	  m_clrColors[UICOLOR_TAB_BORDER][0]                   = RGB(148,166,181);
	  m_clrColors[UICOLOR_TAB_TEXT_NORMAL][0]              = RGB(0,0,0);
	  m_clrColors[UICOLOR_TAB_TEXT_SELECTED][0]            = RGB(0,0,0);
	  m_clrColors[UICOLOR_TAB_TEXT_DISABLED][0]            = RGB(0,0,0);   


      // Boot Windows Common Controls (for the ToolTip control)
      ::InitCommonControls();

      // We need the image library for effects. It is however optional in Windows so
      // we'll also need to provide a gracefull fallback.
      ::LoadLibrary(_T("msimg32.dll"));
   }
   m_szMinWindow.cx = 140;
   m_szMinWindow.cy = 200;
   m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
   m_uMsgMouseWheel = ::RegisterWindowMessage(MSH_MOUSEWHEEL);
   // System Config
   m_SystemConfig.bShowKeyboardCues = false;
   m_SystemConfig.bScrollLists = false;
   // System Metrics
   m_SystemMetrics.cxvscroll = (int) ::GetSystemMetrics(SM_CXVSCROLL);
   m_SystemMetrics.cyvscroll = (int) ::GetSystemMetrics(SM_CXVSCROLL);


   GdiplusStartupInput gdiplusStartupInput;
   GdiplusStartup( &m_gdiplusToken, &gdiplusStartupInput, NULL );

   m_pImageSkinNormal = NULL;
   m_pImageSkinFocused = NULL;
   m_pImageSkinPushed = NULL;
   m_pImageSkinDisabled = NULL;
}

CUIPaintManager::~CUIPaintManager()
{
   // Delete the control-tree structures
   int i;
   for( i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) delete static_cast<CUIControl*>(m_aDelayedCleanup[i]);
   delete m_pRoot;
   // Release other collections
   for( i = 0; i < m_aTimers.GetSize(); i++ ) delete static_cast<TIMERINFO*>(m_aTimers[i]);
   // Reset other parts...
   if( m_hwndTooltip != NULL ) ::DestroyWindow(m_hwndTooltip);
   if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
   if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
   if( m_hDcPaint != NULL ) ::ReleaseDC(m_hWndPaint, m_hDcPaint);
   m_aPreMessages.Remove(m_aPreMessages.Find(this));

   if (m_pImageSkinFocused != NULL && m_pImageSkinFocused->GetLastStatus() == Gdiplus::Ok)
   {
	   delete m_pImageSkinFocused;
	   m_pImageSkinFocused = NULL;
   }

   if (m_pImageSkinPushed != NULL && m_pImageSkinPushed->GetLastStatus() == Gdiplus::Ok)
   {
	   delete m_pImageSkinPushed;
	   m_pImageSkinPushed = NULL;
   }

   if (m_pImageSkinDisabled != NULL && m_pImageSkinDisabled->GetLastStatus() == Gdiplus::Ok)
   {
	   delete m_pImageSkinDisabled;
	   m_pImageSkinDisabled = NULL;
   }

   if (m_pImageSkinNormal != NULL && m_pImageSkinNormal->GetLastStatus() == Gdiplus::Ok)
   {
	   delete m_pImageSkinNormal;
	   m_pImageSkinNormal = NULL;
   }


   GdiplusShutdown(m_gdiplusToken);
}

void CUIPaintManager::ReleaseCommonSKin()
{
	if (m_pImageSkinMenu != NULL)
	{
		delete m_pImageSkinMenu;
		m_pImageSkinMenu = NULL;
	}

	if (m_pImageSkinCombo != NULL)
	{
		delete m_pImageSkinCombo;
		m_pImageSkinCombo = NULL;
	}

	if (m_pImageSkinCommonDialog != NULL)
	{
		delete m_pImageSkinCommonDialog;
		m_pImageSkinCommonDialog = NULL;
	}

	if (m_pImageSkinCloseButton != NULL)
	{
		delete m_pImageSkinCloseButton;
		m_pImageSkinCloseButton = NULL;
	}

	if (m_pImageSkinCommonButton != NULL)
	{
		delete m_pImageSkinCommonButton;
		m_pImageSkinCommonButton = NULL;
	}

	if (m_pImageSkinCheckbox != NULL)
	{
		delete m_pImageSkinCheckbox;
		m_pImageSkinCheckbox = NULL;
	}

	if (m_pImageSkinRadiobox != NULL)
	{
		delete m_pImageSkinRadiobox;
		m_pImageSkinRadiobox = NULL;
	}

	if (m_pImageSkinVScroll != NULL)
	{
		delete m_pImageSkinVScroll;
		m_pImageSkinVScroll = NULL;
	}
}

void CUIPaintManager::Init(HWND hWnd)
{
   ASSERT(::IsWindow(hWnd));
   // Remember the window context we came from
   m_hWndPaint = hWnd;
   m_hDcPaint = ::GetDC(hWnd);
   // We'll want to filter messages globally too
   m_aPreMessages.Add(this);
}

HINSTANCE CUIPaintManager::GetResourceInstance()
{
   return m_hInstance;
}

HINSTANCE CUIPaintManager::GetLanguageInstance()
{
   return m_hLangInst;
}

void CUIPaintManager::SetResourceInstance(HINSTANCE hInst)
{
   m_hInstance = hInst;
   if( m_hLangInst == NULL ) m_hLangInst = hInst;
}

void CUIPaintManager::SetLanguageInstance(HINSTANCE hInst)
{
   m_hLangInst = hInst;
}

HWND CUIPaintManager::GetPaintWindow() const
{
   return m_hWndPaint;
}

HDC CUIPaintManager::GetPaintDC() const
{
   return m_hDcPaint;
}

POINT CUIPaintManager::GetMousePos() const
{
   return m_ptLastMousePos;
}

SIZE CUIPaintManager::GetClientSize() const
{
   RECT rcClient = { 0 };
   ::GetClientRect(m_hWndPaint, &rcClient);
   return CUISize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
}

void CUIPaintManager::SetMinMaxInfo(int cx, int cy)
{
   ASSERT(cx>=0 && cy>=0);
   m_szMinWindow.cx = cx;
   m_szMinWindow.cy = cy;
}

bool CUIPaintManager::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
{
   switch( uMsg ) {
   case WM_KEYDOWN:
      {
         // Tabbing between controls
         if( wParam == VK_TAB ) {
            SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
            m_SystemConfig.bShowKeyboardCues = true;
			// 防止闪烁临时解决方案
            //::InvalidateRect(m_hWndPaint, NULL, FALSE);
            return true;
         }
         // Handle default dialog controls OK and CANCEL.
         // If there are controls named "ok" or "cancel" they
         // will be activated on keypress.
         if( wParam == VK_RETURN ) {
            CUIControl* pControl = FindControl(_T("ok"));
            if( pControl != NULL && m_pFocus != pControl ) {
               if( m_pFocus == NULL || (m_pFocus->GetControlFlags() & UIFLAG_WANTRETURN) == 0 ) {
                  pControl->Activate();
                  return true;
               }
            }
         }
         if( wParam == VK_ESCAPE ) {
            CUIControl* pControl = FindControl(_T("cancel"));
            if( pControl != NULL ) {
               pControl->Activate();
               return true;
            }
         }
      }
      break;
   case WM_SYSCHAR:
      {
         // Handle ALT-shortcut key-combinations
         FINDSHORTCUT fs = { 0 };
         fs.ch = toupper(wParam);
         CUIControl* pControl = m_pRoot->FindControl(__FindControlFromShortcut, &fs, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
         if( pControl != NULL ) {
            pControl->SetFocus();
            pControl->Activate();
            return true;
         }
      }
      break;
   case WM_SYSKEYDOWN:
      {
         // Press ALT once and the shortcuts will be shown in view
         if( wParam == VK_MENU && !m_SystemConfig.bShowKeyboardCues ) {
            m_SystemConfig.bShowKeyboardCues = true;
            ::InvalidateRect(m_hWndPaint, NULL, FALSE);
         }
         if( m_pFocus != NULL ) {
            TEventUI event = { 0 };
            event.Type = UIEVENT_SYSKEY;
            event.chKey = wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
         }
      }
      break;
   }
   return false;
}

bool CUIPaintManager::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{

	// Not ready yet?
	if( m_hWndPaint == NULL || !::IsWindow(m_hWndPaint)) return false;
	// Cycle through listeners
	for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) 
	{
		bool bHandled = false;
		LRESULT lResult = static_cast<IMessageFilterUI*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
		if( bHandled ) {
			lRes = lResult;
			return true;
		}
	}
	// Custom handling of events
	switch( uMsg ) {
   case WM_APP + 1:
	   {
		   // Delayed control-tree cleanup. See AttachDialog() for details.
		   for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) delete static_cast<CUIControl*>(m_aDelayedCleanup[i]);
		   m_aDelayedCleanup.Empty();
	   }
	   break;
   case WM_CLOSE:
	   {
		   // Make sure all matching "closing" evm_pEventHoverents are sent
		   TEventUI event = { 0 };
		   event.ptMouse = m_ptLastMousePos;
		   event.dwTimestamp = ::GetTickCount();
		   if( m_pEventHover != NULL ) {
			   event.Type = UIEVENT_MOUSELEAVE;
			   event.pSender = m_pEventHover;
			   m_pEventHover->Event(event);
		   }
		   if( m_pEventClick != NULL ) {
			   event.Type = UIEVENT_BUTTONUP;
			   event.pSender = m_pEventClick;
			   m_pEventClick->Event(event);
		   }
		   SetFocus(NULL);
		   // Hmmph, the usual Windows tricks to avoid
		   // focus loss...
		   HWND hwndParent = GetWindowOwner(m_hWndPaint);
		   if( hwndParent != NULL ) ::SetFocus(hwndParent);
	   }
	   break;
   case WM_ERASEBKGND:
	   {
		   // We'll do the painting here...
		   lRes = 1;
	   }
	   return true;
   case WM_PAINT:
	   {
		   // Should we paint?
		   RECT rcPaint = { 0 };
		  if( !::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE) ) return true;
		   // Do we need to resize anything?
		   // This is the time where we layout the controls on the form.
		   // We delay this even from the WM_SIZE messages since resizing can be
		   // a very expensize operation.
		   if( m_bResizeNeeded ) {
			   RECT rcClient = { 0 };
			   ::GetClientRect(m_hWndPaint, &rcClient);
			   if( !::IsRectEmpty(&rcClient) ) {
				   HDC hDC = ::CreateCompatibleDC(m_hDcPaint);
				   m_pRoot->SetPos(rcClient);
				   ::DeleteDC(hDC);
				   m_bResizeNeeded = false;
				   // We'll want to notify the window when it is first initialized
				   // with the correct layout. The window form would take the time
				   // to submit swipes/animations.
				   if( m_bFirstLayout ) {
					   m_bFirstLayout = false;
					   SendNotify(m_pRoot, UI_NOTIFY_WINDOW_INIT);
				   }
			   }
			   // Reset offscreen device
			   if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
			   if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
			   m_hDcOffscreen = NULL;
			   m_hbmpOffscreen = NULL;
		   }
		   // Set focus to first control?
		   if( m_bFocusNeeded ) {
			   SetNextTabControl();
		   }
		   //
		   // Render screen
		   //
	
		   {
			   // Standard painting of control-tree - no 3D animation now.
			   // Prepare offscreen bitmap?
			   if( m_bOffscreenPaint && m_hbmpOffscreen == NULL )
			   {
				   RECT rcClient = { 0 };
				   ::GetClientRect(m_hWndPaint, &rcClient);
				   m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
				   m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
				   ASSERT(m_hDcOffscreen);
				   ASSERT(m_hbmpOffscreen);
			   }
			   // Begin Windows paint
			   PAINTSTRUCT ps = { 0 };
			   ::BeginPaint(m_hWndPaint, &ps);
			   if( m_bOffscreenPaint )
			   {
				   // We have an offscreen device to paint on for flickerfree display.
				   HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
				   // Paint the image on the offscreen bitmap
				   int iSaveDC = ::SaveDC(m_hDcOffscreen);

				   DWORD dwTime = ::GetTickCount();
				   m_pRoot->DoPaint(m_hDcOffscreen, ps.rcPaint);

				   ::RestoreDC(m_hDcOffscreen, iSaveDC);
				   // Draw alpha bitmaps on top?
				   for( int i = 0; i < m_aPostPaint.GetSize(); i++ ) {
					   TPostPaintUI* pBlit = static_cast<TPostPaintUI*>(m_aPostPaint[i]);
					   CUIBlueRenderEngine::DoPaintAlphaBitmap(m_hDcOffscreen, this, pBlit->hBitmap, pBlit->rc, pBlit->iAlpha);
				   }
				   m_aPostPaint.Empty();
				   // Blit offscreen bitmap back to display

				   ::BitBlt(ps.hdc, 
				   ps.rcPaint.left, 
				   ps.rcPaint.top, 
				   ps.rcPaint.right - ps.rcPaint.left,
				   ps.rcPaint.bottom - ps.rcPaint.top,
				   m_hDcOffscreen,
				   ps.rcPaint.left,
				   ps.rcPaint.top,
				   SRCCOPY);	

				   dwTime = ::GetTickCount() - dwTime;

				   //TRACE(_T("the m_pRoot Draw time is %d\n\n"), dwTime);

				   //TRACE(_T("the paint width is %d\n"), ps.rcPaint.right - ps.rcPaint.left);

				   ::SelectObject(m_hDcOffscreen, hOldBitmap);
			   }
			   else
			   {
				   // A standard paint job
				   int iSaveDC = ::SaveDC(ps.hdc);
				   m_pRoot->DoPaint(ps.hdc, ps.rcPaint);
				   ::RestoreDC(ps.hdc, iSaveDC);
			   }
			   // All Done!
			   ::EndPaint(m_hWndPaint, &ps);
		   }
	   }
	   // If any of the painting requested a resize again, we'll need
	   // to invalidate the entire window once more.
	   if( m_bResizeNeeded ) ::InvalidateRect(m_hWndPaint, NULL, FALSE);
	   return true;
   case WM_PRINTCLIENT:
	   {
		   RECT rcClient;
		   ::GetClientRect(m_hWndPaint, &rcClient);
		   HDC hDC = (HDC) wParam;
		   int save = ::SaveDC(hDC);
		   m_pRoot->DoPaint(hDC, rcClient);
		   // Check for traversing children. The crux is that WM_PRINT will assume
		   // that the DC is positioned at frame coordinates and will paint the child
		   // control at the wrong position. We'll simulate the entire thing instead.
		   if( (lParam & PRF_CHILDREN) != 0 ) {
			   HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
			   while( hWndChild != NULL ) {
				   RECT rcPos = { 0 };
				   ::GetWindowRect(hWndChild, &rcPos);
				   ::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
				   ::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
				   // NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
				   //       since the latter will not print the nonclient correctly for
				   //       EDIT controls.
				   ::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
				   hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
			   }
		   }
		   ::RestoreDC(hDC, save);
	   }
	   break;
   case WM_GETMINMAXINFO:
	   {
		   LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
		   lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
		   lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
	   }
	   break;
   case WM_SIZE:
	   {
		   if( m_pFocus != NULL ) {
			   TEventUI event = { 0 };
			   event.Type = UIEVENT_WINDOWSIZE;
			   event.dwTimestamp = ::GetTickCount();
			   m_pFocus->Event(event);
		   }
		   m_bResizeNeeded = true;
	   }
	   return true;
   case WM_TIMER:
	   {
		   for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
			   const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			   if( pTimer->hWnd == m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) ) {
				   TEventUI event = { 0 };
				   event.Type = UIEVENT_TIMER;
				   event.wParam = pTimer->nLocalID;
				   event.dwTimestamp = ::GetTickCount();
				   pTimer->pSender->Event(event);
				   break;
			   }
		   }
	   }
	   break;
   case WM_MOUSEHOVER:
	   {
		   m_bMouseTracking = false;
		   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		   CUIControl* pHover = FindControl(pt);
		   if( pHover == NULL ) break;
		   // Generate mouse hover event
		   if( m_pEventHover != NULL ) {
			   TEventUI event = { 0 };
			   event.ptMouse = pt;
			   event.Type = UIEVENT_MOUSEHOVER;
			   event.pSender = pHover;
			   event.dwTimestamp = ::GetTickCount();
			   m_pEventHover->Event(event);
		   }
		   // Create tooltip information
		   CStdString sToolTip = pHover->GetToolTip();
		   if( sToolTip.IsEmpty() ) return true;
		   sToolTip.ProcessResourceTokens();
		   ::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
		   m_ToolTip.cbSize = sizeof(TOOLINFO);
		   m_ToolTip.uFlags = TTF_IDISHWND;
		   m_ToolTip.hwnd = m_hWndPaint;
		   m_ToolTip.uId = (UINT) m_hWndPaint;
		   m_ToolTip.hinst = m_hInstance;
		   m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) sToolTip );
		   m_ToolTip.rect = pHover->GetPos();
		   if( m_hwndTooltip == NULL ) {
			   m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
			   ::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
		   }
		   ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
		   ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
	   }
	   return true;
   case WM_MOUSELEAVE:
	   {
		   if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
		   if( m_bMouseTracking ) ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) -1);
		   m_bMouseTracking = false;
	   }
	   break;
   case WM_MOUSEMOVE:
	   {
		   // Start tracking this entire window again...
		   if( !m_bMouseTracking ) {
			   TRACKMOUSEEVENT tme = { 0 };
			   tme.cbSize = sizeof(TRACKMOUSEEVENT);
			   tme.dwFlags = TME_HOVER | TME_LEAVE;
			   tme.hwndTrack = m_hWndPaint;
			   tme.dwHoverTime = m_hwndTooltip == NULL ? 1000UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
			   _TrackMouseEvent(&tme);
			   m_bMouseTracking = true;
		   }
		   // Generate the appropriate mouse messages
		   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		   m_ptLastMousePos = pt;
		   CUIControl* pNewHover = FindControl(pt);
		   if( pNewHover != NULL && pNewHover->GetManager() != this ) break;
		   TEventUI event = { 0 };
		   event.ptMouse = pt;
		   event.dwTimestamp = ::GetTickCount();
		   if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
			   event.Type = UIEVENT_MOUSELEAVE;
			   event.pSender = pNewHover;
			   m_pEventHover->Event(event);
			   m_pEventHover = NULL;
			   if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
		   }
		   if( pNewHover != m_pEventHover && pNewHover != NULL ) {
			   event.Type = UIEVENT_MOUSEENTER;
			   event.pSender = m_pEventHover;
			   pNewHover->Event(event);
			   m_pEventHover = pNewHover;
		   }
		   if( m_pEventClick != NULL ) {
			   event.Type = UIEVENT_MOUSEMOVE;
			   event.pSender = NULL;
			   m_pEventClick->Event(event);
		   }
		   else if( pNewHover != NULL ) {
			   event.Type = UIEVENT_MOUSEMOVE;
			   event.pSender = NULL;
			   pNewHover->Event(event);
		   }
	   }
	   break;
   case WM_LBUTTONDOWN:
	   {
		   // We alway set focus back to our app (this helps
		   // when Win32 child windows are placed on the dialog
		   // and we need to remove them on focus change).
		   ::SetFocus(m_hWndPaint);
		   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		   m_ptLastMousePos = pt;
		   CUIControl* pControl = FindControl(pt);
		   if( pControl == NULL ) break;
		   if( pControl->GetManager() != this ) break;

		   // 临时解决方案
		   if( pControl == m_pRoot ) break;

		   m_pEventClick = pControl;
		   pControl->SetFocus();
		   TEventUI event = { 0 };
		   event.Type = UIEVENT_BUTTONDOWN;
		   event.wParam = wParam;
		   event.lParam = lParam;
		   event.ptMouse = pt;
		   event.wKeyState = wParam;
		   event.dwTimestamp = ::GetTickCount();
		   pControl->Event(event);
		   // No need to burden user with 3D animations
		   // We always capture the mouse
		   ::SetCapture(m_hWndPaint);
	   }
	   break;
   case WM_LBUTTONUP:
	   {
		   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		   m_ptLastMousePos = pt;
		   if( m_pEventClick == NULL ) break;
		   ::ReleaseCapture();
		   TEventUI event = { 0 };
		   event.Type = UIEVENT_BUTTONUP;
		   event.wParam = wParam;
		   event.lParam = lParam;
		   event.ptMouse = pt;
		   event.wKeyState = wParam;
		   event.dwTimestamp = ::GetTickCount();
		   m_pEventClick->Event(event);
		   m_pEventClick = NULL;
	   }
	   break;
   case WM_RBUTTONDOWN:
	   {
		   // We alway set focus back to our app (this helps
		   // when Win32 child windows are placed on the dialog
		   // and we need to remove them on focus change).
		   ::SetFocus(m_hWndPaint);
		   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		   m_ptLastMousePos = pt;
		   CUIControl* pControl = FindControl(pt);
		   if( pControl == NULL ) break;
		   if( pControl->GetManager() != this ) break;

		   // 临时解决方案
		   if( pControl == m_pRoot ) break;

		   m_pEventClick = pControl;
		   pControl->SetFocus();
		   TEventUI event = { 0 };
		   event.Type = UIEVENT_CONTEXTMENU;
		   event.wParam = wParam;
		   event.lParam = lParam;
		   event.ptMouse = pt;
		   event.wKeyState = wParam;
		   event.dwTimestamp = ::GetTickCount();
		   pControl->Event(event);
	   }
	   break;
   case WM_LBUTTONDBLCLK:
	   {
		   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		   m_ptLastMousePos = pt;
		   CUIControl* pControl = FindControl(pt);
		   if( pControl == NULL ) break;
		   if( pControl->GetManager() != this ) break;
		   TEventUI event = { 0 };
		   event.Type = UIEVENT_DBLCLICK;
		   event.ptMouse = pt;
		   event.wKeyState = wParam;
		   event.dwTimestamp = ::GetTickCount();
		   pControl->Event(event);
		   m_pEventClick = pControl;
	   }
	   break;
   case WM_CHAR:
	   {
		   if( m_pFocus == NULL ) break;
		   TEventUI event = { 0 };
		   event.Type = UIEVENT_CHAR;
		   event.chKey = wParam;
		   event.ptMouse = m_ptLastMousePos;
		   event.wKeyState = MapKeyState();
		   event.dwTimestamp = ::GetTickCount();
		   m_pFocus->Event(event);
	   }
	   break;
   case WM_KEYDOWN:
	   {
		   if( m_pFocus == NULL ) break;
		   TEventUI event = { 0 };
		   event.Type = UIEVENT_KEYDOWN;
		   event.chKey = wParam;
		   event.ptMouse = m_ptLastMousePos;
		   event.wKeyState = MapKeyState();
		   event.dwTimestamp = ::GetTickCount();
		   m_pFocus->Event(event);
		   m_pEventKey = m_pFocus;
	   }
	   break;
   case WM_KEYUP:
	   {
		   if( m_pEventKey == NULL ) break;
		   TEventUI event = { 0 };
		   event.Type = UIEVENT_KEYUP;
		   event.chKey = wParam;
		   event.ptMouse = m_ptLastMousePos;
		   event.wKeyState = MapKeyState();
		   event.dwTimestamp = ::GetTickCount();
		   m_pEventKey->Event(event);
		   m_pEventKey = NULL;
	   }
	   break;
   case WM_SETCURSOR:
	   {
		   POINT pt = { 0 };
		   ::GetCursorPos(&pt);
		   ::ScreenToClient(m_hWndPaint, &pt);
		   CUIControl* pControl = FindControl(pt);
		   if( pControl == NULL ) break;
		   if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 || !::IsWindowEnabled(m_hWndPaint)) break;
		   TEventUI event = { 0 };
		   event.Type = UIEVENT_SETCURSOR;
		   event.wParam = wParam;
		   event.lParam = lParam;
		   event.ptMouse = pt;
		   event.wKeyState = MapKeyState();
		   event.dwTimestamp = ::GetTickCount();
		   pControl->Event(event);
	   }
	   return true;
   case WM_CTLCOLOREDIT:
	   {
		   HDC hDC = (HDC) wParam;
		   ::SetTextColor(hDC, GetThemeColor(UICOLOR_EDIT_TEXT_NORMAL));
		   ::SetBkColor(hDC, GetThemeColor(UICOLOR_EDIT_BACKGROUND_NORMAL));
		   lRes = (LRESULT) GetThemeBrush(UICOLOR_EDIT_BACKGROUND_NORMAL);		   
	   }
	   return true;
   case WM_MEASUREITEM:
	   {
		   if( wParam == 0 ) break;
		   HWND hWndChild = ::GetDlgItem(m_hWndPaint, ((LPMEASUREITEMSTRUCT) lParam)->CtlID);
		   lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
		   return true;
	   }
	   break;
   case WM_DRAWITEM:
	   {
		   if( wParam == 0 ) break;
		   HWND hWndChild = ((LPDRAWITEMSTRUCT) lParam)->hwndItem;
		   lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
		   return true;
	   }
	   break;
   case WM_VSCROLL:
	   {
		   if( lParam == NULL ) break;
		   CUIContainer* pContainer = static_cast<CUIContainer*>(::GetProp((HWND) lParam, _T("WndX")));
		   if( pContainer == NULL ) break;
		   TEventUI event = { 0 };
		   event.Type = UIEVENT_VSCROLL;
		   event.wParam = wParam;
		   event.lParam = lParam;
		   event.dwTimestamp = ::GetTickCount();
		   pContainer->Event(event);
	   }
	   break;
   case WM_NOTIFY:
	   {
		   LPNMHDR lpNMHDR = (LPNMHDR) lParam;
		   if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
		   return true;
	   }
	   break;
   case WM_COMMAND:
	   {
		   if( lParam == 0 ) break;
		   HWND hWndChild = (HWND) lParam;
		   lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
		   return true;
	   }
	   break;
   default:
	   // Handle WM_MOUSEWHEEL
	   if( (uMsg == m_uMsgMouseWheel || uMsg == 0x020A) && m_pFocus != NULL )
	   {
		   int zDelta = (int) (short) HIWORD(wParam);
		   TEventUI event = { 0 };
		   event.Type = UIEVENT_SCROLLWHEEL;
		   event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
		   event.lParam = lParam;
		   event.dwTimestamp = ::GetTickCount();
		   event.ptMouse = m_ptLastMousePos;
		   m_pFocus->Event(event);
		   // Simulate regular scrolling by sending scroll events
		   event.Type = UIEVENT_VSCROLL;
		   for( int i = 0; i < abs(zDelta); i += 40 ) m_pFocus->Event(event);
		   // Let's make sure that the scroll item below the cursor is the same as before...
		   ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
	   }
	   break;
	}
	return false;
}

void CUIPaintManager::UpdateLayout()
{
   m_bResizeNeeded = true;
   ::InvalidateRect(m_hWndPaint, NULL, FALSE);
}

void CUIPaintManager::Invalidate(RECT rcItem)
{
   ::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
}

bool CUIPaintManager::AttachDialog(CUIControl* pControl)
{
   ASSERT(::IsWindow(m_hWndPaint));
   // Reset any previous attachment
   SetFocus(NULL);
   m_pEventKey = NULL;
   m_pEventHover = NULL;
   m_pEventClick = NULL;
   m_aNameHash.Empty();
   // Remove the existing control-tree. We might have gotten inside this function as
   // a result of an event fired or similar, so we cannot just delete the objects and
   // pull the internal memory of the calling code. We'll delay the cleanup.
   if( m_pRoot != NULL ) {
      m_aDelayedCleanup.Add(m_pRoot);
      ::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
   }
   // Set the dialog root element
   m_pRoot = pControl;
   // Go ahead...
   m_bResizeNeeded = true;
   m_bFirstLayout = true;
   m_bFocusNeeded = true;
   // Initiate all control
   return InitControls(pControl);
}

bool CUIPaintManager::InitControls(CUIControl* pControl, CUIControl* pParent /*= NULL*/)
{
   ASSERT(pControl);
   if( pControl == NULL ) return false;
   pControl->SetManager(this, pParent != NULL ? pParent : pControl->GetParent());
   // We're usually initializing the control after adding some more of them to the tree,
   // and thus this would be a good time to request the name-map rebuilt.
   m_aNameHash.Empty();
   return true;
}

void CUIPaintManager::ReapObjects(CUIControl* pControl)
{
   if( pControl == m_pEventKey ) m_pEventKey = NULL;
   if( pControl == m_pEventHover ) m_pEventHover = NULL;
   if( pControl == m_pEventClick ) m_pEventClick = NULL;
   // TODO: Do something with name-hash-map
   //m_aNameHash.Empty();
}

void CUIPaintManager::MessageLoop()
{
   MSG msg = { 0 };
   while( ::GetMessage(&msg, NULL, 0, 0) ) {
      if( !CUIPaintManager::TranslateMessage(&msg) ) {
         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
   }
}

bool CUIPaintManager::TranslateMessage(const LPMSG pMsg)
{
   // Pretranslate Message takes care of system-wide messages, such as
   // tabbing and shortcut key-combos. We'll look for all messages for
   // each window and any child control attached.
   HWND hwndParent = ::GetParent(pMsg->hwnd);
   UINT uStyle = GetWindowStyle(pMsg->hwnd);
   LRESULT lRes = 0;
   for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
      CUIPaintManager* pT = static_cast<CUIPaintManager*>(m_aPreMessages[i]);
      if( pMsg->hwnd == pT->GetPaintWindow()
         || (hwndParent == pT->GetPaintWindow() && ((uStyle & WS_CHILD) != 0)) )
      {
         if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) return true;
      }
   }
   return false;
}

bool CUIPaintManager::AddPostPaintBlit(const TPostPaintUI& job)
{
   return m_aPostPaint.Add(&job);
}

CUIControl* CUIPaintManager::GetFocus() const
{
   return m_pFocus;
}

void CUIPaintManager::SetFocus(CUIControl* pControl)
{
	if (pControl == NULL)
	{
		m_pFocus = NULL;
		return;
	}
   // Paint manager window has focus?
   if( ::GetFocus() != m_hWndPaint ) 
   {
	   ::SetFocus(m_hWndPaint);
   }
   // Already has focus?
   if( pControl == m_pFocus ) return;
   // Remove focus from old control
   if( m_pFocus != NULL ) 
   {
      TEventUI event = { 0 };
      event.Type = UIEVENT_KILLFOCUS;
      event.pSender = pControl;
      event.dwTimestamp = ::GetTickCount();
      m_pFocus->Event(event);
      SendNotify(m_pFocus, UI_NOTIFY_KILLFOCUS);
      m_pFocus = NULL;
   }
   // Set focus to new control
   if( pControl != NULL 
       && pControl->GetManager() == this 
       && pControl->IsVisible() 
       && pControl->IsEnabled() ) 
   {
	  m_bFocusNeeded = false;
      m_pFocus = pControl;
      TEventUI event = { 0 };
      event.Type = UIEVENT_SETFOCUS;
      event.pSender = pControl;
      event.dwTimestamp = ::GetTickCount();
      m_pFocus->Event(event);
      SendNotify(m_pFocus, UI_NOTIFY_SETFOCUS);
   }
}

bool CUIPaintManager::SetTimer(CUIControl* pControl, UINT nTimerID, UINT uElapse)
{
   ASSERT(pControl!=NULL);
   ASSERT(uElapse>0);
   m_uTimerID = (++m_uTimerID) % 0xFF;
   if( !::SetTimer(m_hWndPaint, m_uTimerID, uElapse, NULL) ) return FALSE;
   TIMERINFO* pTimer = new TIMERINFO;
   if( pTimer == NULL ) return FALSE;
   pTimer->hWnd = m_hWndPaint;
   pTimer->pSender = pControl;
   pTimer->nLocalID = nTimerID;
   pTimer->uWinTimer = m_uTimerID;
   return m_aTimers.Add(pTimer);
}

bool CUIPaintManager::KillTimer(CUIControl* pControl, UINT nTimerID)
{
   ASSERT(pControl!=NULL);
   for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
      TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
      if( pTimer->pSender == pControl
          && pTimer->hWnd == m_hWndPaint
          && pTimer->nLocalID == nTimerID )
      {
         ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
         delete pTimer;
         return m_aTimers.Remove(i);
      }
   }
   return false;
}

bool CUIPaintManager::SetNextTabControl(bool bForward)
{
   // If we're in the process of restructuring the layout we can delay the
   // focus calulation until the next repaint.
   if( m_bResizeNeeded && bForward ) {
      m_bFocusNeeded = true;
      ::InvalidateRect(m_hWndPaint, NULL, FALSE);
      return true;
   }
   // Find next/previous tabbable control
   FINDTABINFO info1 = { 0 };
   info1.pFocus = m_pFocus;
   info1.bForward = bForward;
   CUIControl* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
   if( pControl == NULL ) {  
      if( bForward ) {
         // Wrap around
         FINDTABINFO info2 = { 0 };
         info2.pFocus = bForward ? NULL : info1.pLast;
         info2.bForward = bForward;
         pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
      }
      else {
         pControl = info1.pLast;
      }
   }
   if( pControl != NULL ) SetFocus(pControl);
   m_bFocusNeeded = false;
   return true;
}

TSystemSettingsUI CUIPaintManager::GetSystemSettings() const
{
   return m_SystemConfig;
}

void CUIPaintManager::SetSystemSettings(const TSystemSettingsUI Config)
{
   m_SystemConfig = Config;
}

TSystemMetricsUI CUIPaintManager::GetSystemMetrics() const
{
   return m_SystemMetrics;
}

bool CUIPaintManager::AddNotifier(INotifyUI* pNotifier)
{
	if(m_aNotifiers.Find(pNotifier) < 0 )
	{
		return m_aNotifiers.Add(pNotifier);
	}

	return false;
}

bool CUIPaintManager::RemoveNotifier(INotifyUI* pNotifier)
{
   for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
      if( static_cast<INotifyUI*>(m_aNotifiers[i]) == pNotifier ) {
         return m_aNotifiers.Remove(i);
      }
   }
   return false;
}

bool CUIPaintManager::AddMessageFilter(IMessageFilterUI* pFilter)
{
   ASSERT(m_aMessageFilters.Find(pFilter)<0);
   return m_aMessageFilters.Add(pFilter);
}

bool CUIPaintManager::RemoveMessageFilter(IMessageFilterUI* pFilter)
{
   for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
      if( static_cast<IMessageFilterUI*>(m_aMessageFilters[i]) == pFilter ) {
         return m_aMessageFilters.Remove(i);
      }
   }
   return false;
}

void CUIPaintManager::SendNotify(CUIControl* pControl, UI_NOTIFY_TYPE nMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
   TNotifyUI Msg;
   Msg.pSender = pControl;
   Msg.nType = nMessage;
   Msg.wParam = 0;
   Msg.lParam = 0;
   SendNotify(Msg);
}

void CUIPaintManager::SendNotify(TNotifyUI& Msg)
{
   // Pre-fill some standard members
   Msg.ptMouse = m_ptLastMousePos;
   Msg.dwTimestamp = ::GetTickCount();
   // Allow sender control to react
   if (Msg.pSender) Msg.pSender->Notify(Msg);
   // Send to all listeners
   for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
      static_cast<INotifyUI*>(m_aNotifiers[i])->Notify(Msg);
   }
}

HFONT CUIPaintManager::GetThemeFont(UITYPE_FONT Index) const
{
   if( Index <= UIFONT__FIRST || Index >= UIFONT__LAST ) return NULL;
   if( m_hFonts[Index] == NULL ) m_hFonts[Index] = ::CreateFontIndirect(&m_aLogFonts[Index]);
   return m_hFonts[Index];
}

HPEN CUIPaintManager::GetThemePen(UITYPE_COLOR Index) const
{
   if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return NULL;
   if( m_hPens[Index] == NULL ) m_hPens[Index] = ::CreatePen(PS_SOLID, 1, m_clrColors[Index][0]);
   return m_hPens[Index];
}

HBRUSH CUIPaintManager::GetThemeBrush(UITYPE_COLOR Index) const
{
   if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return NULL;
   if( m_hBrushes[Index] == NULL ) m_hBrushes[Index] = ::CreateSolidBrush(m_clrColors[Index][0]);
   return m_hBrushes[Index];
}

const TEXTMETRIC& CUIPaintManager::GetThemeFontInfo(UITYPE_FONT Index) const
{
   if( Index <= UIFONT__FIRST || Index >= UIFONT__LAST ) return m_aTextMetrics[0];
   if( m_aTextMetrics[Index].tmHeight == 0 ) {
      HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, GetThemeFont(Index));
      ::GetTextMetrics(m_hDcPaint, &m_aTextMetrics[Index]);
      ::SelectObject(m_hDcPaint, hOldFont);
   }
   return m_aTextMetrics[Index];
}

COLORREF CUIPaintManager::GetThemeColor(UITYPE_COLOR Index) const
{
   if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return RGB(0,0,0);
   return m_clrColors[Index][0];
}

bool CUIPaintManager::GetThemeColorPair(UITYPE_COLOR Index, COLORREF& clr1, COLORREF& clr2) const
{
   if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return false;
   clr1 = m_clrColors[Index][0];
   clr2 = m_clrColors[Index][1];
   return true;
}

CUIControl* CUIPaintManager::FindControl(LPCTSTR pstrName)
{
   //ASSERT(m_pRoot);
   if (m_pRoot == NULL)
   {
	   return NULL;
   }

   // First time here? Build hash array...
   if( m_aNameHash.GetSize() == 0 ) {
      int nCount = 0;
      m_pRoot->FindControl(__FindControlFromCount, &nCount, UIFIND_ALL);
      m_aNameHash.Resize(nCount + (nCount / 10));
      m_pRoot->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
   }
   // Find name in the hash array
   int nCount = 0;
   int nSize = m_aNameHash.GetSize();
   int iNameHash = (int) (GetNameHash(pstrName) % nSize);
   while( m_aNameHash[iNameHash] != NULL ) {
      if( static_cast<CUIControl*>(m_aNameHash[iNameHash])->GetName() == pstrName ) return static_cast<CUIControl*>(m_aNameHash[iNameHash]);
      iNameHash = (iNameHash + 1) % nSize;
      if( ++nCount >= nSize ) break;
   }
   return NULL;
}
CUIControl* CUIPaintManager::FindControl(UINT uID)
{
	TCHAR buffer[10];

	_itot(uID, buffer, 10);

	return FindControl(buffer);

}

CUIControl* CUIPaintManager::FindControl(POINT pt) const
{
   ASSERT(m_pRoot);
   return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST|UIFIND_POINT_CUSTOM);
}

CUIControl* CALLBACK CUIPaintManager::__FindControlFromCount(CUIControl* /*pThis*/, LPVOID pData)
{
   int* pnCount = static_cast<int*>(pData);
   (*pnCount)++;
   return NULL;  // Count all controls
}

CUIControl* CALLBACK CUIPaintManager::__FindControlFromTab(CUIControl* pThis, LPVOID pData)
{
   FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
   if( pInfo->pFocus == pThis ) {
      if( pInfo->bForward ) pInfo->bNextIsIt = true;
      return pInfo->bForward ? NULL : pInfo->pLast;
   }
   if( (pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0 ) return NULL;
   pInfo->pLast = pThis;
   if( pInfo->bNextIsIt ) return pThis;
   if( pInfo->pFocus == NULL ) return pThis;
   return NULL;  // Examine all controls
}

CUIControl* CALLBACK CUIPaintManager::__FindControlFromNameHash(CUIControl* pThis, LPVOID pData)
{
   CUIPaintManager* pManager = static_cast<CUIPaintManager*>(pData);
   // No name?
   const CStdString& sName = pThis->GetName();
   if( sName.IsEmpty() ) return NULL;
   // Add this control to the hash list
   int nCount = 0;
   int nSize = pManager->m_aNameHash.GetSize();
   int iNameHash = (int) (GetNameHash(sName) % nSize);
   while( pManager->m_aNameHash[iNameHash] != NULL ) {
      iNameHash = (iNameHash + 1) % nSize;
      if( ++nCount == nSize ) return NULL;
   }
   pManager->m_aNameHash.SetAt(iNameHash, pThis);
   return NULL; // Attempt to add all controls
}

CUIControl* CALLBACK CUIPaintManager::__FindControlFromShortcut(CUIControl* pThis, LPVOID pData)
{
   FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
   if( pFS->ch == toupper(pThis->GetShortcut()) ) pFS->bPickNext = true;

   if( _tcsstr(pThis->GetClass(), _T("ListMenuLabelElementUI")) != NULL ) pFS->bPickNext = false;

   if( _tcsstr(pThis->GetClass(), _T("Label")) != NULL ) return NULL;   // Labels never get focus!
   return pFS->bPickNext ? pThis : NULL;
}

CUIControl* CALLBACK CUIPaintManager::__FindControlFromPoint(CUIControl* pThis, LPVOID pData)
{
   LPPOINT pPoint = static_cast<LPPOINT>(pData);
   return ::PtInRect(&pThis->GetPos(), *pPoint) ? pThis : NULL;
}

bool CUIPaintManager::SetImage(Image **pImage, const BYTE* pData, UINT nSize)
{
	bool bRet = false;
	HGLOBAL hBuffer = NULL;

	if (pData == NULL)
	{
		goto exit;
	}

	hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, nSize);
	if (hBuffer)
	{
		void* pBuffer = ::GlobalLock(hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pData, nSize);

			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
			{
				*pImage = Bitmap::FromStream(pStream);
				pStream->Release();
				if (*pImage)
				{ 
					if ((*pImage)->GetLastStatus() == Gdiplus::Ok)
					{
						bRet = true;

						goto exit;
					}

					delete *pImage;
					*pImage = NULL;
				}
			}

			*pImage = NULL;
		}
	}

exit:
	if (hBuffer)
	{
		GlobalUnlock(hBuffer);
		GlobalFree(hBuffer);
	}

	return bRet;
}

bool CUIPaintManager::SetImage( Image **pImage, UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	bool bRet = false;
	HGLOBAL hBuffer = NULL;

	if (!nResourceID)
	{
		goto exit;
	}
	if(hResHandle == NULL)
	{
		hResHandle=m_hInstance;
	}

	HRSRC hResource = ::FindResource(hResHandle, MAKEINTRESOURCE(nResourceID), _T("PNG"));
	if (!hResource)
	{
		goto exit;
	}

	DWORD imageSize = ::SizeofResource(hResHandle, hResource);
	if (!imageSize)
	{
		goto exit;
	}

	const void* pResourceData = ::LockResource(::LoadResource(hResHandle, hResource));
	if (!pResourceData)
	{
		goto exit;
	}

	hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (hBuffer)
	{
		void* pBuffer = ::GlobalLock(hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);

			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
			{
				Bitmap* pImageTemp;
				pImageTemp = Bitmap::FromStream(pStream);
				pStream->Release();
				if (pImageTemp)
				{ 
					if ((pImageTemp)->GetLastStatus() == Gdiplus::Ok)
					{
						BitmapData* pBitmapData = new BitmapData
							;
						Rect rcImage(0,0, pImageTemp->GetWidth(), pImageTemp->GetHeight());
						pImageTemp->LockBits(&rcImage, ImageLockModeRead, pImageTemp->GetPixelFormat(), pBitmapData); 

						*pImage = new Bitmap(pBitmapData->Width, pBitmapData->Height, pBitmapData->PixelFormat);

						pImageTemp->UnlockBits(pBitmapData);

						Graphics graph(*pImage);

						RectF rfDest(PointF((REAL)0, (REAL)0), SizeF((REAL)pImageTemp->GetWidth(), (REAL)pImageTemp->GetHeight()));

						graph.DrawImage(pImageTemp, rfDest, (REAL)0, (REAL)0, (REAL)pImageTemp->GetWidth(), (REAL)pImageTemp->GetHeight(), UnitPixel, NULL);
			

						delete pBitmapData;

						delete pImageTemp;
					}


				}

				if (*pImage)
				{ 
					if ((*pImage)->GetLastStatus() == Gdiplus::Ok)
					{
						bRet = true;
						goto exit;
					}

					delete *pImage;
					*pImage = NULL;
				}
			}

			*pImage = NULL;
		}
	}

exit:
	if (hBuffer)
	{
		GlobalUnlock(hBuffer);
		GlobalFree(hBuffer);
	}

	return bRet;
}

bool CUIPaintManager::SetNormalSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	bool bRet = SetImage(&m_pImageSkinNormal, nResourceID, hResHandle);

	return bRet;
}


bool CUIPaintManager::SetNormalSkin( Image* apImage )
{
	bool bRet = (apImage == NULL ? FALSE:TRUE);
	if (bRet)
	{
		m_pImageSkinNormal=apImage->Clone();
	}
	
	return bRet;
}


bool CUIPaintManager::SetFocusSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	bool bRet = SetImage(&m_pImageSkinFocused, nResourceID, hResHandle);

	return bRet;
}

bool CUIPaintManager::SetFocusSkin( Image* apImage )
{
	bool bRet = (apImage == NULL ? FALSE:TRUE);
	if (bRet)
	{
		m_pImageSkinFocused=apImage->Clone();
	}

	return bRet;

}

bool CUIPaintManager::SetPushedSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	bool bRet = SetImage(&m_pImageSkinPushed, nResourceID, hResHandle);

	return bRet;
}

bool CUIPaintManager::SetPushedSkin( Image* apImage )
{
	bool bRet = (apImage == NULL ? FALSE:TRUE);
	if (bRet)
	{
		m_pImageSkinPushed=apImage->Clone();
	}

	return bRet;
}

bool CUIPaintManager::SetDisabledSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	bool bRet = SetImage(&m_pImageSkinDisabled, nResourceID, hResHandle);

	return bRet;
}

bool CUIPaintManager::SetDisabledSkin( Image* apImage )
{
	bool bRet = (apImage == NULL ? FALSE:TRUE);
	if (bRet)
	{
		m_pImageSkinDisabled=apImage->Clone();
	}
	
	return bRet;
}

bool CUIPaintManager::SetCommonMenuSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return SetImage(&m_pImageSkinMenu, nResourceID, hResHandle);
}

bool CUIPaintManager::SetCommonComboSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return SetImage(&m_pImageSkinCombo, nResourceID, hResHandle);
}
bool CUIPaintManager::SetCommonDialogSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return SetImage(&m_pImageSkinCommonDialog, nResourceID, hResHandle);
}
bool CUIPaintManager::SetCommonButtonSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return SetImage(&m_pImageSkinCommonButton, nResourceID, hResHandle);
}
bool CUIPaintManager::SetCommonCheckBoxSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return SetImage(&m_pImageSkinCheckbox, nResourceID, hResHandle);
}
bool CUIPaintManager::SetCommonRadioBoxSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return SetImage(&m_pImageSkinRadiobox, nResourceID, hResHandle);
}
bool CUIPaintManager::SetCommonVScrollSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return SetImage(&m_pImageSkinVScroll, nResourceID, hResHandle);
}
bool CUIPaintManager::SetCommonIcon( UINT nResourceID, UINT nIndex, UINT nWidth /*= 16*/, HMODULE hResHandle/*=NULL*/ )
{
	IconInfo info;
	info.nWindth = nWidth;
	bool bRet = SetImage(&info.pImage, nResourceID, hResHandle);
	if (bRet == false)
	{
		return bRet;
	}

	m_mapSkinCommonIcon[nIndex] = info;

	return true;
}
bool CUIPaintManager::SetCommonCloseButtonSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return SetImage(&m_pImageSkinCloseButton, nResourceID, hResHandle);
}
void CUIPaintManager::SetEditBoxBKColor_Normal(COLORREF color)
{
	m_clrColors[UICOLOR_EDIT_BACKGROUND_NORMAL][0] = color;

	if (m_hBrushes[UICOLOR_EDIT_BACKGROUND_NORMAL] != NULL)
	{
		::DeleteObject(m_hBrushes[UICOLOR_EDIT_BACKGROUND_NORMAL]);
		m_hBrushes[UICOLOR_EDIT_BACKGROUND_NORMAL] = NULL;
	}
}
void CUIPaintManager::SetEditBoxTextColor_Normal(COLORREF color)
{
	m_clrColors[UICOLOR_EDIT_TEXT_NORMAL][0] = color;
}

void CUIPaintManager::SetEditBoxBKColor_Disable(COLORREF color)
{
	m_clrColors[UICOLOR_EDIT_BACKGROUND_DISABLED][0] = color;

	if (m_hBrushes[UICOLOR_EDIT_BACKGROUND_DISABLED] != NULL)
	{
		::DeleteObject(m_hBrushes[UICOLOR_EDIT_BACKGROUND_DISABLED]);
		m_hBrushes[UICOLOR_EDIT_BACKGROUND_DISABLED] = NULL;
	}
}
void CUIPaintManager::SetEditBoxTextColor_Disable(COLORREF color)
{
	m_clrColors[UICOLOR_EDIT_TEXT_DISABLED][0] = color;
}

void CUIPaintManager::SetEditBoxBoderColor_Normal(COLORREF colorLight, COLORREF colorDark)
{
	m_clrColors[UICOLOR_EDIT_BORDER_NORMAL][0] = colorLight;
	if (colorDark == INVALID__UICOLOR)
	{
		m_clrColors[UICOLOR_EDIT_BORDER_NORMAL_DARK][0] = colorLight;
	}
	else
	{
		m_clrColors[UICOLOR_EDIT_BORDER_NORMAL_DARK][0] = colorDark;
	}

	if (m_hPens[UICOLOR_EDIT_BORDER_NORMAL] != NULL)
	{
		::DeleteObject(m_hBrushes[UICOLOR_EDIT_BORDER_NORMAL]);
		m_hPens[UICOLOR_EDIT_BORDER_NORMAL] = NULL;
	}

	if (m_hPens[UICOLOR_EDIT_BORDER_NORMAL_DARK] != NULL)
	{
		::DeleteObject(m_hBrushes[UICOLOR_EDIT_BORDER_NORMAL_DARK]);
		m_hPens[UICOLOR_EDIT_BORDER_NORMAL_DARK] = NULL;
	}
}
void CUIPaintManager::SetEditBoxBoderColor_Focused(COLORREF colorLight, COLORREF colorDark)
{
	m_clrColors[UICOLOR_EDIT_BORDER_SELECTED][0] = colorLight;
	if (colorDark == INVALID__UICOLOR)
	{
		m_clrColors[UICOLOR_EDIT_BORDER_SELECTED_DARK][0] = colorLight;
	}
	else
	{
		m_clrColors[UICOLOR_EDIT_BORDER_SELECTED_DARK][0] = colorDark;
	}

	if (m_hPens[UICOLOR_EDIT_BORDER_SELECTED] != NULL)
	{
		::DeleteObject(m_hBrushes[UICOLOR_EDIT_BORDER_SELECTED]);
		m_hPens[UICOLOR_EDIT_BORDER_SELECTED] = NULL;
	}

	if (m_hPens[UICOLOR_EDIT_BORDER_SELECTED_DARK] != NULL)
	{
		::DeleteObject(m_hBrushes[UICOLOR_EDIT_BORDER_SELECTED_DARK]);
		m_hPens[UICOLOR_EDIT_BORDER_SELECTED_DARK] = NULL;
	}
}
void CUIPaintManager::SetEditBoxBoderColor_Disabled(COLORREF colorLight, COLORREF colorDark)
{
	m_clrColors[UICOLOR_EDIT_BORDER_DISABLED][0] = colorLight;
	if (colorDark == INVALID__UICOLOR)
	{
		m_clrColors[UICOLOR_EDIT_BORDER_DISABLED_DARK][0] = colorLight;
	}
	else
	{
		m_clrColors[UICOLOR_EDIT_BORDER_DISABLED_DARK][0] = colorDark;
	}

	if (m_hPens[UICOLOR_EDIT_BORDER_DISABLED] != NULL)
	{
		::DeleteObject(m_hBrushes[UICOLOR_EDIT_BORDER_DISABLED]);
		m_hPens[UICOLOR_EDIT_BORDER_DISABLED] = NULL;
	}

	if (m_hPens[UICOLOR_EDIT_BORDER_DISABLED_DARK] != NULL)
	{
		::DeleteObject(m_hBrushes[UICOLOR_EDIT_BORDER_DISABLED_DARK]);
		m_hPens[UICOLOR_EDIT_BORDER_DISABLED_DARK] = NULL;
	}
}



/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIControl::CUIControl() : 
   m_pManager(NULL), 
   m_pParent(NULL), 
   m_pTag(NULL),
   m_chShortcut('\0'),
   m_bVisible(true), 
   m_bFocused(false),
   m_bEnabled(true),
   m_nId(0)

{
   ::ZeroMemory(&m_rcItem, sizeof(RECT));
   m_boundaryList.clear();

}

CUIControl::~CUIControl()
{
   if( m_pManager != NULL ) m_pManager->ReapObjects(this);
}

bool CUIControl::IsVisible() const
{
   return m_bVisible;
}

bool CUIControl::IsEnabled() const
{
   return m_bEnabled;
}

bool CUIControl::IsFocused() const
{
   return m_bFocused;
}

UINT CUIControl::GetControlFlags() const
{
   return 0;
}

void CUIControl::SetVisible(bool bVisible)
{
   if( m_bVisible == bVisible ) return;
   m_bVisible = bVisible;
   if( m_pManager != NULL ) m_pManager->UpdateLayout();
}

void CUIControl::SetEnabled(bool bEnabled)
{
   m_bEnabled = bEnabled;
   Invalidate();
}

bool CUIControl::Activate()
{
   if( !IsVisible() ) return false;
   if( !IsEnabled() ) return false;
   return true;
}

CUIControl* CUIControl::GetParent() const
{
   return m_pParent;
}

void CUIControl::SetFocus()
{
   if( m_pManager != NULL ) m_pManager->SetFocus(this);
}

void CUIControl::SetShortcut(TCHAR ch)
{
   m_chShortcut = ch;
}

TCHAR CUIControl::GetShortcut() const
{
   return m_chShortcut;
}

CStdString CUIControl::GetText() const
{
   return m_sText;
}

void CUIControl::SetText(LPCTSTR pstrText)
{
   m_sText = pstrText;
   Invalidate();
}

UINT_PTR CUIControl::GetTag() const
{
   return m_pTag;
}

void CUIControl::SetTag(UINT_PTR pTag)
{
   m_pTag = pTag;
}

void CUIControl::SetToolTip(LPCTSTR pstrText)
{
   m_sToolTip = pstrText;
}

CStdString CUIControl::GetToolTip() const
{
   return m_sToolTip;
}

void CUIControl::Init()
{
}

CUIPaintManager* CUIControl::GetManager() const
{
   return m_pManager;
}

void CUIControl::SetManager(CUIPaintManager* pManager, CUIControl* pParent)
{
   bool bInit = m_pManager == NULL;
   m_pManager = pManager;
   m_pParent = pParent;
   if( bInit ) Init();
}

CStdString CUIControl::GetName() const
{
   return m_sName;
}

void CUIControl::SetName(LPCTSTR pstrName)
{
   m_sName = pstrName;
}

UINT CUIControl::GetId() const
{
	return m_nId; 
}
void CUIControl::SetId(UINT nId)
{
	m_nId = nId;
}

LPVOID CUIControl::GetInterface(LPCTSTR pstrName)
{
   if( _tcscmp(pstrName, _T("Control")) == 0 ) return this;
   return NULL;
}

void CUIControl::SetBoundary(int Num, ...)
{
	ASSERT(Num>=3);
	va_list ap;
	va_start(ap, Num);
	m_boundaryList.clear();
	for(int i = 0; i < Num; i++) 
	{
		Point BoundaryPoint = va_arg(ap, Point);
		m_boundaryList.push_back(BoundaryPoint);
	}
	va_end(ap);
	 
}
void CUIControl::SetBoundary(Point * Points ,int Num)
{
	ASSERT(Num>=3);
	for (int i =0;i<Num;i++)
	{
		m_boundaryList.push_back(Points[i]);
	}
}
CUIControl* CUIControl::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
   if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
   if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
   if( (uFlags & UIFIND_HITTEST) != 0 && !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData)) ) return NULL;
   if ((uFlags & UIFIND_POINT_CUSTOM) != 0 &&!IsCustomPoint(* static_cast<LPPOINT>(pData)))  return NULL;
   return Proc(this, pData);
}

bool CUIControl::IsCustomPoint(POINT point)
{
	if (!(m_boundaryList.empty()))
	{
		int boundaryCount = m_boundaryList.size();
		Point *points  = new Point[boundaryCount];
		int count =0;
		for(std::vector<Point>::iterator item = m_boundaryList.begin();item!=m_boundaryList.end();item++)
		{
			points[count] = *item;
			count ++;
		}
		GraphicsPath * gp = new GraphicsPath();
		gp->AddPolygon(points,boundaryCount);
		if (gp->IsVisible(Point(point.x, point.y)))
		{
			return true;
		}else
		{
			return false;
		}
	}
	return true;
}

RECT CUIControl::GetPos() const
{
   return m_rcItem;
}

void CUIControl::SetPos(RECT rc)
{
   m_rcItem = rc;
   // NOTE: SetPos() is usually called during the WM_PAINT cycle where all controls are
   //       being laid out. Calling UpdateLayout() again would be wrong. Refreshing the
   //       window won't hurt (if we're already inside WM_PAINT we'll just validate it out).
   Invalidate();
}

void CUIControl::Invalidate()
{
   if( m_pManager != NULL ) m_pManager->Invalidate(m_rcItem);
}

void CUIControl::UpdateLayout()
{
   if( m_pManager != NULL ) m_pManager->UpdateLayout();
}

void CUIControl::Event(TEventUI& event)
{
   if( event.Type == UIEVENT_SETCURSOR )
   {
      ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
      return;
   }
   if( event.Type == UIEVENT_SETFOCUS ) 
   {
      m_bFocused = true;
      Invalidate();
      return;
   }
   if( event.Type == UIEVENT_KILLFOCUS ) 
   {
      m_bFocused = false;
      Invalidate();
      return;
   }
   if( event.Type == UIEVENT_TIMER )
   {
      m_pManager->SendNotify(this, UI_NOTIFY_TIMER, event.wParam, event.lParam);
      return;
   }
   if( m_pParent != NULL ) m_pParent->Event(event);
}

void CUIControl::Notify(TNotifyUI& /*msg*/)
{
}

void CUIControl::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   if( _tcscmp(pstrName, _T("pos")) == 0 ) {
      RECT rcPos = { 0 };
      LPTSTR pstr = NULL;
      rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
      rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
      rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
      rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
      SetPos(rcPos);
   }
   else if( _tcscmp(pstrName, _T("name")) == 0 ) SetName(pstrValue);
   else if( _tcscmp(pstrName, _T("text")) == 0 ) SetText(pstrValue);
   else if( _tcscmp(pstrName, _T("tooltip")) == 0 ) SetToolTip(pstrValue);
   else if( _tcscmp(pstrName, _T("enabled")) == 0 ) SetEnabled(_tcscmp(pstrValue, _T("true")) == 0);
   else if( _tcscmp(pstrName, _T("visible")) == 0 ) SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
   else if( _tcscmp(pstrName, _T("shortcut")) == 0 ) SetShortcut(pstrValue[0]);
}

CUIControl* CUIControl::ApplyAttributeList(LPCTSTR pstrList)
{
   CStdString sItem;
   CStdString sValue;
   while( *pstrList != '\0' ) {
      sItem.Empty();
      sValue.Empty();
      while( *pstrList != '\0' && *pstrList != '=' ) sItem += *pstrList++;
      ASSERT(*pstrList=='=');
      if( *pstrList++ != '=' ) return this;
      ASSERT(*pstrList=='\"');
      if( *pstrList++ != '\"' ) return this;
      while( *pstrList != '\0' && *pstrList != '\"' ) sValue += *pstrList++;
      ASSERT(*pstrList=='\"');
      if( *pstrList++ != '\"' ) return this;
      SetAttribute(sItem, sValue);
      if( *pstrList++ != ',' ) return this;
   }
   return this;
}

bool CUIControl::SetTimer(UINT nTimerID, UINT uElapse)
{
	 if( m_pManager != NULL ) 
	 {
		 return m_pManager->SetTimer(this, nTimerID, uElapse);
	 }
	 else
	 {
		 return false;
	 }
}

bool CUIControl::KillTimer(UINT nTimerID)
{
	if( m_pManager != NULL ) 
	{
		return m_pManager->KillTimer(this, nTimerID);
	}
	else
	{
		return false;
	}
}
