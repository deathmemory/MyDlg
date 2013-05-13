
#include "StdAfx.h"
#include "UIEdit.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CUIEditWnd : public CWindowWnd
{
public:
   CUIEditWnd();

   void Init(CUIEdit* pOwner);

   LPCTSTR GetWindowClassName() const;
   LPCTSTR GetSuperClassName() const;
   void OnFinalMessage(HWND hWnd);

   LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
   LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
   CUIEdit* m_pOwner;
};


CUIEditWnd::CUIEditWnd() : m_pOwner(NULL)
{
}

void CUIEditWnd::Init(CUIEdit* pOwner)
{
   CUIRect rcPos = pOwner->GetPos();
   UINT luBorderWidth=pOwner->GetBorderWidth();

   if (luBorderWidth == 0)
   {
	   luBorderWidth = 1;
   }

   rcPos.Deflate(luBorderWidth, luBorderWidth);

   if(pOwner->IsMultiLine() == false)
   {
		Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_CHILD|pOwner->m_uEditStyle, 0, rcPos);
   }
   else
   {
	  if (pOwner->IsWantReturn() == false)
	  {
		   Create(pOwner->m_pManager->GetPaintWindow(), NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | ES_MULTILINE | ES_AUTOVSCROLL, 0, rcPos);
	  }
	  else
	  {
		   Create(pOwner->m_pManager->GetPaintWindow(), NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | ES_WANTRETURN | ES_MULTILINE | ES_AUTOVSCROLL, 0, rcPos);
	  }
   }
   SetWindowFont(m_hWnd, pOwner->GetManager()->GetThemeFont(UIFONT_NORMAL), TRUE);

   if(pOwner->IsMultiLine() == false)
   {
	   //-----------------------------------------
	   CDC* pDC = CDC::FromHandle(GetDC(m_hWnd));
	   TEXTMETRIC tm;
	   pDC->GetTextMetrics(&tm);
	   int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
	   int nMargin = (rcPos.GetHeight() - nFontHeight) / 2;
	   rcPos.Deflate(0,nMargin);
	   ::SetWindowPos(m_hWnd, NULL, rcPos.left, rcPos.top+1, rcPos.GetWidth(), rcPos.GetHeight(), SWP_NOZORDER);
	   //-----------------------------------------
   }
   Edit_SetText(m_hWnd, pOwner->GetText());


   Edit_SetModify(m_hWnd, FALSE);
   SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
   Edit_SetSel(m_hWnd, 0, -1);
   Edit_Enable(m_hWnd, pOwner->IsEnabled() == true);
   Edit_SetReadOnly(m_hWnd, pOwner->IsReadOnly() == true);

   if (pOwner->GetLimitTextCount() > 0)
   {
		Edit_LimitText(m_hWnd, pOwner->GetLimitTextCount());
   }

   if (pOwner->IsVisible())
   {
	  ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
   }

   if(pOwner->IsMultiLine() == false)
   {
	   ::SetFocus(m_hWnd);
   }
   
   m_pOwner = pOwner;
}

LPCTSTR CUIEditWnd::GetWindowClassName() const
{
   return _T("SingleLineEditWnd");
}

LPCTSTR CUIEditWnd::GetSuperClassName() const
{
   return WC_EDIT;
}

void CUIEditWnd::OnFinalMessage(HWND /*hWnd*/)
{
   // Clear reference and die
   m_pOwner->m_pWindow = NULL;
   delete this;
}

LRESULT CUIEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   LRESULT lRes = 0;
   BOOL bHandled = TRUE;
   if( uMsg == WM_KILLFOCUS )
   {
	   if (m_pOwner->IsMultiLine())
	   {
			 bHandled = FALSE;
	   }
	   else
	   {
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
	   }
   }
   else if( uMsg == WM_LBUTTONDOWN )
   {
	   if (m_pOwner && m_pOwner->IsMultiLine())
	   {
		   m_pOwner->SetFocus();
	   }
	   bHandled = FALSE;
   }
   else if( uMsg == OCM_COMMAND && GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) 
   {
	   lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
   }
   else
   {
	   bHandled = FALSE;
   }

   if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
   return lRes;
}

LRESULT CUIEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

   if (m_pOwner)
   {
	   m_pOwner->KillFocus();
   }

   ShowWindow(SW_HIDE);
   PostMessage(WM_CLOSE);
 
   return lRes;
}

LRESULT CUIEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if( m_pOwner == NULL ) return 0;
   // Copy text back
   int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
   LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
   ASSERT(pstr);
   if( pstr == NULL ) return 0;
   ::GetWindowText(m_hWnd, pstr, cchLen);

   if (m_pOwner->IsMultiLine())
   {
	   m_pOwner->m_sText = pstr;
	   m_pOwner->GetManager()->SendNotify(m_pOwner, UI_NOTIFY_TEXT_CHANGE);
   }
   else
   {
		m_pOwner->SetText(pstr);
   }
   
   return 0;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIEdit::CUIEdit() : m_pWindow(NULL), m_uEditStyle(ES_AUTOHSCROLL), m_bReadOnly(false), m_bPassword(false), m_uBorderWidth(1),m_nLimitTextCount(0),
m_bMultiLine(false), m_bWantReturn(false), m_pImageOwer(NULL)
{

}
CUIEdit::~CUIEdit()
{
	if( m_pWindow != NULL && ::IsWindow(*m_pWindow) ) m_pWindow->Close();
}

LPCTSTR CUIEdit::GetClass() const
{
   return _T("EditUI");
}

UINT CUIEdit::GetControlFlags() const
{
   return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

void CUIEdit::Event(TEventUI& event)
{
	if (m_bMultiLine == false)
	{
	   if( event.Type == UIEVENT_SETCURSOR )
	   {
		  if( IsEnabled() ) {
			 ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			 return;
		  }
	   }
	   if( event.Type == UIEVENT_WINDOWSIZE )
	   {
		  if( m_pWindow != NULL ) m_pManager->SetFocus(NULL);
	   }
	   if( event.Type == UIEVENT_SETFOCUS ) 
	   {
		  if( IsEnabled() && !IsReadOnly()) {
			 m_pWindow = new CUIEditWnd();
			 ASSERT(m_pWindow);
			 m_pWindow->Init(this);

			 TRACE(_T("UIEVENT_SETFOCUS       \n"));
		  }
	   }
	   if( event.Type == UIEVENT_BUTTONDOWN && IsFocused() && m_pWindow == NULL && !IsReadOnly()) 
	   {
		  // FIX: In the case of window having lost focus, editor is gone, but
		  //      we don't get another SetFocus when we click on the control again.
		  m_pWindow = new CUIEditWnd();
		  ASSERT(m_pWindow);
		  m_pWindow->Init(this);
		  return;
	   }
	}
	else
	{
		if( event.Type == UIEVENT_WINDOWSIZE )
		{
			if( m_pWindow != NULL ) m_pManager->SetFocus(NULL);
		}
		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			if( m_pWindow != NULL ) ::SetFocus(*m_pWindow);
		}
	}

   CUIControl::Event(event);
}

void CUIEdit::SetText(LPCTSTR pstrText, bool bNotidyChange)
{
   m_sText = CStdString(pstrText);
   if (m_bMultiLine)
   {
		if( m_pWindow != NULL ) SetWindowText(*m_pWindow, pstrText);
   }
   
   if (bNotidyChange)
   {
	   if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_TEXT_CHANGE);
   }
   
   Invalidate();
}

void CUIEdit::SetReadOnly(bool bReadOnly)
{
   m_bReadOnly = bReadOnly;
   if (m_bMultiLine)
   {
		if( m_pWindow != NULL ) Edit_SetReadOnly(*m_pWindow, bReadOnly == true);
   }
   Invalidate();
}

bool CUIEdit::IsReadOnly() const
{
   return m_bReadOnly;
}

void CUIEdit::SetEditStyle(UINT uStyle)
{
   m_uEditStyle = uStyle;
   Invalidate();
}

SIZE CUIEdit::EstimateSize(SIZE /*szAvailable*/)
{
   return CUISize(0, 12 + m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void CUIEdit::DoPaint(HDC hDC, const RECT& /*rcPaint*/)
{
   UINT uState = 0;
   if( IsFocused() ) uState |= UISTATE_FOCUSED;
   if( IsReadOnly() ) uState |= UISTATE_READONLY;
   if( !IsEnabled() ) uState |= UISTATE_DISABLED;

   if (m_bMultiLine == false)
   {
	   if (m_bPassword)
	   {
		   CStdString str = m_sText;

		   for (int i = 0; i < str.GetLength(); i++)
		   {
			   str.SetAt(i, _T('*'));
		   }

		   CUIBlueRenderEngine::DoPaintEditBox(hDC, m_pManager, m_rcItem, str, uState, m_uEditStyle, m_uBorderWidth, m_pImageOwer);
	   }
	   else
	   {
		   CUIBlueRenderEngine::DoPaintEditBox(hDC, m_pManager, m_rcItem, m_sText, uState, m_uEditStyle, m_uBorderWidth, m_pImageOwer);
	   }
   }
   else
   {
	   CUIBlueRenderEngine::DoPaintEditBox(hDC, m_pManager, m_rcItem, _T(""), uState, 0, m_uBorderWidth, m_pImageOwer);
   }

}
void CUIEdit::SetPassword(bool bPassword)
{
	m_bPassword = bPassword;

	SetEditStyle(m_uEditStyle | ES_PASSWORD);
}

void CUIEdit::SetWantReturn(bool bWantReturn)
{
	m_bWantReturn = bWantReturn;
}

bool CUIEdit::IsWantReturn() const
{
	return m_bWantReturn;
}
bool CUIEdit::LimitText(int nChars)
{
	m_nLimitTextCount = nChars;

	if (m_pWindow != NULL && ::IsWindow(m_pWindow->GetHWND()))
	{
		Edit_LimitText(m_pWindow->GetHWND(), m_nLimitTextCount);
	}

	return true;
}

int CUIEdit::GetLimitTextCount()
{
	return m_nLimitTextCount;
}

void CUIEdit::SetMultiLine(bool bMulti)
{
	m_bMultiLine = bMulti;
}

bool CUIEdit::IsMultiLine() const
{
	return m_bMultiLine;
}

void CUIEdit::Init()
{
	if (m_bMultiLine)
	{
		m_pWindow = new CUIEditWnd();
		ASSERT(m_pWindow);
		m_pWindow->Init(this);
	}
}

void CUIEdit::SetPos(RECT rc)
{
	if (m_bMultiLine)
	{
		if( m_pWindow != NULL ) {
			CUIRect rcEdit = rc;
			rcEdit.Deflate(3, 3);
			::SetWindowPos(*m_pWindow, HWND_TOP, rcEdit.left, rcEdit.top, rcEdit.GetWidth(), rcEdit.GetHeight(), SWP_NOACTIVATE);
		}
	}
	CUIControl::SetPos(rc);
}

void CUIEdit::SetVisible(bool bVisible)
{
	CUIControl::SetVisible(bVisible);
	if (m_bMultiLine)
	{
		if( m_pWindow != NULL ) ::ShowWindow(*m_pWindow, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	}
}

void CUIEdit::SetEnabled(bool bEnabled)
{
	CUIControl::SetEnabled(bEnabled);
	if (m_bMultiLine)
	{
		if( m_pWindow != NULL ) ::EnableWindow(*m_pWindow, bEnabled == true);
	}
}

void CUIEdit::SetImage( Image* pImage )
{
	if (pImage)
	{
		m_pImageOwer = pImage->Clone();
	}
	Invalidate();
}

void CUIEdit::SetImage( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	CUIPaintManager::SetImage(&m_pImageOwer, nResourceID, hResHandle);
	Invalidate();
}

void CUIEdit::SetImage( LPCTSTR pstrName )
{
	m_pImageOwer = Image::FromFile(pstrName);
	Invalidate();
}

void CUIEdit::SetBorderWidth( UINT auWidth )
{
	m_uBorderWidth=auWidth;
}

UINT CUIEdit::GetBorderWidth()
{
	return m_uBorderWidth;
}

Image * CUIEdit::GetImage()
{
	return m_pImageOwer;
}

void CUIEdit::SetSel(int nStartChar, int nEndChar)
{
	if( m_pWindow != NULL )
	{
		m_pWindow->SendMessage(EM_SETSEL, (WPARAM)nStartChar, (LPARAM)nEndChar);
	}
}

void CUIEdit::SetFocus()
{
	CUIControl::SetFocus();
}

void CUIEdit::KillFocus()
{
	if( m_pManager != NULL ) m_pManager->SetFocus(NULL);

	if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_KILLFOCUS);

	m_bFocused = false;

	Invalidate();
}
