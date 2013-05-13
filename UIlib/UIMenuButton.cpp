
#include "StdAfx.h"
#include "UIMenuButton.h"

/////////////////////////////////////////////////////////////////////////////////////
//
//

class CMenuButtonWnd : public CWindowWnd
{
public:
	void Init(CUIMenuButton* pOwner);
	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CUIPaintManager m_pm;
	CUIMenuButton* m_pOwner;
	int m_iOldSel;
};

/////////////////////////////////////////////////////////////////////////////////////
//
//

void CMenuButtonWnd::Init(CUIMenuButton* pOwner)
{
	TRACE(_T("the Init process is called\n"));
   m_pOwner = pOwner;
   m_iOldSel = m_pOwner->GetCurSel();
   // Position the popup window in absolute space
   SIZE szDrop = m_pOwner->GetDropDownSize();
   RECT rc = pOwner->GetPos();
   rc.top = rc.bottom;
   rc.bottom = rc.top + szDrop.cy;
   if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;
   MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
   Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
   // HACK: Don't deselect the parent's caption
   HWND hWndParent = m_hWnd;
   while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
   ::ShowWindow(m_hWnd, SW_SHOW);
   ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CMenuButtonWnd::GetWindowClassName() const
{
   return _T("MenuButtonWnd");
}

void CMenuButtonWnd::OnFinalMessage(HWND hWnd)
{
	TRACE(_T("the OnFinalMessage process is called\n"));

	if (m_pOwner->m_pMenuWnd != NULL)
	{
		m_pOwner->m_pMenuWnd = NULL;

		m_pOwner->SendSelectNotify();
	}

   delete this;

}

LRESULT CMenuButtonWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if( uMsg == WM_CREATE ) {     
      m_pm.Init(m_hWnd);
      // The trick is to add the items to the new container. Their owner gets
      // reassigned by this operation - which is why it is important to reassign
      // the items back to the righfull owner/manager when the window closes.
      CMenuCanvasUI* pWindow = new CMenuCanvasUI;
      CVerticalLayoutUI* pLayout = new CVerticalLayoutUI;
      for( int i = 0; i < m_pOwner->GetCount(); i++ ) pLayout->Add(static_cast<CUIControl*>(m_pOwner->GetItem(i)));
      pLayout->SetAutoDestroy(false);
      pLayout->EnableScrollBar();
      pWindow->Add(pLayout);
      m_pm.AttachDialog(pWindow);
      return 0;
   }
   else if( uMsg == WM_CLOSE ) {
      m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent());
      m_pOwner->SetPos(m_pOwner->GetPos());
      m_pOwner->SetFocus();
   }
   else if( uMsg == WM_LBUTTONUP ) {
	   if (m_pOwner->GetCurSel() != -1)
	   {
			PostMessage(WM_KILLFOCUS);
	   }
   }
    else if( uMsg == WM_SYSCHAR ) {
		PostMessage(WM_KILLFOCUS);
	}
   else if( uMsg == WM_KEYDOWN ) {
      switch( wParam ) {
      case VK_ESCAPE:
         m_pOwner->SetCurSel(m_iOldSel);
         // FALL THROUGH...
      case VK_RETURN:
         PostMessage(WM_KILLFOCUS);
          return 1;
      default:
		  {
			   for( int i = 0; i < m_pOwner->GetCount(); i++ )
			   {
				   CUIControl* pControl = static_cast<CUIControl*>(m_pOwner->GetItem(i));

				   if (toupper(pControl->GetShortcut()) == (TCHAR)wParam)
				   {
						 m_pOwner->SetCurSel(i);
						 PostMessage(WM_KILLFOCUS);
						 return 0;
				   }
			   }
		  }

         TEventUI event;
         event.Type = UIEVENT_KEYDOWN;
         event.chKey = wParam;
         m_pOwner->Event(event);
         return 0;
      }
   }
   else if( uMsg == WM_KILLFOCUS ) {
      if( m_hWnd != (HWND) wParam ) PostMessage(WM_CLOSE);
   }
   LRESULT lRes = 0;
   if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
   return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////
CMenuButtonWnd* CUIMenuButton::m_pMenuWnd = NULL;
UINT CUIMenuButton::m_uSelectId = 0;

CUIMenuButton::CUIMenuButton() : 
   m_iCurSel(-1), 
   m_iCurHot(-1),
   m_uButtonState(0)
{
   m_szDropBox = CUISize(0, 150);
   ::ZeroMemory(&m_rcButton, sizeof(RECT));
}

LPCTSTR CUIMenuButton::GetClass() const
{
   return _T("MenuButtonUI");
}

LPVOID CUIMenuButton::GetInterface(LPCTSTR pstrName)
{
   if( _tcscmp(pstrName, _T("ListOwner")) == 0 ) return static_cast<IUIListOwner*>(this);
   return CUIContainer::GetInterface(pstrName);
}

UINT CUIMenuButton::GetControlFlags() const
{
   return UIFLAG_TABSTOP  | UIFLAG_SETCURSOR;
}

void CUIMenuButton::Init()
{
   if( m_iCurSel < 0 ) SetCurSel(0);
}

int CUIMenuButton::GetCurSel() const
{
   return m_iCurSel;
}

bool CUIMenuButton::SetCurSel(int iIndex)
{
   if( iIndex == m_iCurSel ) return true;
   if( m_iCurSel >= 0 ) {
      CUIControl* pControl = static_cast<CUIControl*>(m_items[m_iCurSel]);
      IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
      if( pListItem != NULL ) pListItem->Select(false);
      m_iCurSel = -1;
   }
   if( m_items.GetSize() == 0 ) return false;
   if( iIndex < 0 ) iIndex = 0;
   if( iIndex >= m_items.GetSize() ) iIndex = m_items.GetSize() - 1;
   CUIControl* pControl = static_cast<CUIControl*>(m_items[iIndex]);
   if( !pControl->IsVisible() ) return false;
   if( !pControl->IsEnabled() ) return false;
   IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
   if( pListItem == NULL ) return false;
   m_iCurSel = iIndex;
   pControl->SetFocus();
   pListItem->Select(true);


   TRACE(_T("the SetCurSel value is %d\n"), iIndex);	

   Invalidate();
   return true;
}

bool CUIMenuButton::SetHotItem(int iIndex)
{
	for( int i = 0; i < m_items.GetSize(); i++ )
	{
		CUIControl* pControl = static_cast<CUIControl*>(m_items[i]);

		if (pControl)
		{
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem != NULL )
			{
				pListItem->SetHot(false);
			}
		}
	}

	if( m_items.GetSize() == 0 ) return false;
	if( iIndex < 0 )return false;
	if( iIndex >= m_items.GetSize() ) return false;

	CUIControl* pControl = static_cast<CUIControl*>(m_items[iIndex]);
	
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
	if( pListItem == NULL ) return false;

	pControl->SetFocus();
	pListItem->SetHot(true);

	m_iCurHot = iIndex;

	return true;
}

void CUIMenuButton::SendSelectNotify()
{
	m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
	Invalidate();

	if (m_iCurSel < 0 || m_iCurSel > m_items.GetSize())
	{
		return;
	}

	CUIControl* pControl = static_cast<CUIControl*>(m_items[m_iCurSel]);

	if (pControl != NULL)
	{
		if( m_pManager != NULL ) m_pManager->SendNotify(pControl, UI_NOTIFY_ITEM_CLICK);
		if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_ITEM_SELECT_CHANGE);
	}

}

bool CUIMenuButton::Add(CUIControl* pControl)
{
   IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
   if( pListItem != NULL ) {
      pListItem->SetOwner(this);
      pListItem->SetIndex(m_items.GetSize());
   }
   return CUIContainer::Add(pControl);
}

bool CUIMenuButton::Remove(CUIControl* pControl)
{
   ASSERT(!"Not supported");
   return false;
}

void CUIMenuButton::RemoveAll()
{
   m_iCurSel = -1;
   CUIContainer::RemoveAll();
}

void CUIMenuButton::Event(TEventUI& event)
{
	if( event.Type == UIEVENT_SETCURSOR )
	{
		if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
	}

	if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
	{
		m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
		Invalidate();
		if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_CLICK);

		Activate();
	}

   if( event.Type == UIEVENT_MOUSEMOVE )
   {
      if( (m_uButtonState & UISTATE_CAPTURED) != 0) 
	  {
         if( ::PtInRect(&m_rcButton, event.ptMouse)) 
		 {
			 m_uButtonState |= UISTATE_PUSHED;
		 }
         else 
		 {
			 m_uButtonState &= ~UISTATE_PUSHED;
		 }

         Invalidate();
      }

	  if (m_pMenuWnd != NULL && m_uSelectId == GetId())
	  {
		  m_uButtonState |= UISTATE_PUSHED;
		  Invalidate();	 
	  }
   }
   if( event.Type == UIEVENT_BUTTONUP )
   {
      if( (m_uButtonState & UISTATE_CAPTURED) != 0) 
	  {
         m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
         Invalidate();
      }
   }
   if( event.Type == UIEVENT_KEYDOWN )
   {
      switch( event.chKey ) {
      case VK_UP:
         SetHotItem(FindSelectable(m_iCurHot - 1, false));
         return;
      case VK_DOWN:
         SetHotItem(FindSelectable(m_iCurHot + 1, true));
         return;
      }
   }
   if( event.Type == UIEVENT_SCROLLWHEEL )
   {
      bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
      SetCurSel(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
      return;
   }

   if( event.Type == UIEVENT_MOUSEENTER)
   {
	   m_uButtonState |= UISTATE_HOT;
	   Invalidate();

	   if (m_pMenuWnd != NULL && ::IsWindow(m_pMenuWnd->GetHWND()) && m_uSelectId != GetId())
	   {
		   m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;

		   m_pMenuWnd->SendMessage(WM_CLOSE);	 
		   Activate();
	  } 
	 	    
   }
   if( event.Type == UIEVENT_MOUSELEAVE)
   {
	   m_uButtonState &= ~UISTATE_HOT;

	   Invalidate();
   }
   CUIControl::Event(event);
}

bool CUIMenuButton::NotActivate()
{
	if( !CUIControl::Activate() ) return false;

	TRACE(_T(" Activate \n"));

	if (m_pMenuWnd != NULL)
	{
		m_pMenuWnd->SendMessage(WM_CLOSE);
	}
	
}

bool CUIMenuButton::Activate()
{
   if( !CUIControl::Activate() ) return false;

   TRACE(_T(" Activate \n"));

	if (m_pMenuWnd != NULL) 
	{
		return true;
	}
   

   m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;

   m_pMenuWnd = new CMenuButtonWnd;

   m_iCurSel = -1;
   m_iCurHot = -1;

	SIZE szAvailable = { 100, 30 };

   // 计算最大宽度 wangpf

   m_szDropBox.cy = 0;

   int nWidthMax = m_rcItem.right - m_rcItem.left;
    for( int i = 0; i < m_items.GetSize(); i++ )
	{
		CStdString strText = static_cast<CUIControl*>(m_items[i])->GetText();

		int nLinks = 0;
		RECT rcText = { 0, 0, 9999, 100 };
		CUIBlueRenderEngine::DoPaintPrettyText(m_pManager->GetPaintDC(), m_pManager, rcText, strText, UICOLOR_CONTROL_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_VCENTER | DT_CALCRECT);

		int nWidthTemp = rcText.right - rcText.left + 40;
		if ( nWidthTemp > nWidthMax)
		{
			nWidthMax = nWidthTemp;
		}

		CUIControl* pControl = static_cast<CUIControl*>(m_items[i]);

		if (pControl)
		{
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->Select(false);

			SIZE size = pControl->EstimateSize(szAvailable);
			m_szDropBox.cy += size.cy;
		}
	}

	m_szDropBox.cx = nWidthMax;
	m_szDropBox.cy +=  6;

   m_pMenuWnd->Init(this);

   m_uSelectId = GetId();
   if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_DROPDOWN);
   Invalidate();

   return true;
}

CStdString CUIMenuButton::GetText() const
{
   if( m_iCurSel < 0 ) return _T("");
   CUIControl* pControl = static_cast<CUIControl*>(m_items[m_iCurSel]);
   return pControl->GetText();
}

void CUIMenuButton::SetText(LPCTSTR pstrText)
{
	CUIControl::SetText(pstrText);
	// Automatic assignment of keyboard shortcut
	if( _tcschr(pstrText, '&') != NULL ) m_chShortcut = *(_tcschr(pstrText, '&') + 1);
}

SIZE CUIMenuButton::GetDropDownSize() const
{
   return m_szDropBox;
}

void CUIMenuButton::SetDropDownSize(SIZE szDropBox)
{
   m_szDropBox = szDropBox;
}

void CUIMenuButton::SetPos(RECT rc)
{
   // Put all elements out of sight
   RECT rcNull = { 0 };
   for( int i = 0; i < m_items.GetSize(); i++ ) static_cast<CUIControl*>(m_items[i])->SetPos(rcNull);
   // Position this control
   CUIControl::SetPos(rc);
}

SIZE CUIMenuButton::EstimateSize(SIZE /*szAvailable*/)
{
   SIZE sz = { 0, 12 + m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight };
   // Once there is an element in the list, we'll use the first one to
   // determine the size of the dropdown base control.
   if( m_cxyFixed.cx > 0 && !m_items.IsEmpty() ) {
      RECT rcText = m_rcItem;
      ::InflateRect(&rcText, -4, -2);
      sz = static_cast<CUIControl*>(m_items[0])->EstimateSize(CUISize(rcText.right - rcText.left, 0));
   }
   return sz;
}

void CUIMenuButton::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Paint the nice frame
   int cy = m_rcItem.bottom - m_rcItem.top;
   ::SetRect(&m_rcButton, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
   RECT rcText = { m_rcItem.left + 5, m_rcItem.top, m_rcButton.left + 1, m_rcItem.bottom };

   // Paint dropdown button
   RECT rcPadding = { 0 };
   CUIBlueRenderEngine::DoPaintButton(hDC, m_pManager, m_rcItem, m_uButtonState, 0);

   if (m_sText.GetLength() > 0)
   {
	   UITYPE_COLOR clrText;
	   if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
		   clrText = UICOLOR_BUTTON_TEXT_DISABLED;  
	   }
	   else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
		   clrText = UICOLOR_BUTTON_TEXT_PUSHED;
	   }
	   else {
		   clrText = UICOLOR_BUTTON_TEXT_NORMAL;
	   }


	   RECT rcText = m_rcItem;
	   ::InflateRect(&rcText, -1, -1);
	   rcText.left += rcPadding.left;
	   rcText.top += rcPadding.top;
	   rcText.right -= rcPadding.right;
	   rcText.bottom -= rcPadding.bottom;

	   int nLinks = 0;

	   CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcText, m_sText, clrText, UICOLOR__INVALID, NULL, nLinks, 0);
   }

}

bool CUIMenuButton::AddSubMenu(UINT uMenuId, LPCTSTR pstrText)
{
	CUISubMenu *pLabel = new CUISubMenu;
	if (pLabel == NULL)
	{
		return false;
	}

	TCHAR buffer[10];

	_itot(uMenuId, buffer, 10);

	pLabel->SetName(buffer);

	pLabel->SetText(pstrText);
	pLabel->SetId(uMenuId);

	if (pLabel->GetText() == _T(""))
	{
		pLabel->SetEnabled(false);
	}

	Add(pLabel);

	return true;
}

int CUIMenuButton::FindSelectable(int iIndex, bool bForward/* = true*/) const
{
	if( GetCount() == 0 ) return -1;
	iIndex = CLAMP(iIndex, 0, GetCount() - 1);
	if( bForward ) {
		for( int i = iIndex; i < GetCount(); i++ ) {
			if( GetItem(i)->GetInterface(_T("ListItem")) != NULL 
				&& GetItem(i)->IsVisible()
				&& GetItem(i)->GetText() != _T("")) return i;
		}
		return -1;
	}
	else {
		for( int i = iIndex; i >= 0; --i ) {
			if( GetItem(i)->GetInterface(_T("ListItem")) != NULL 
				&& GetItem(i)->IsVisible()
			&& GetItem(i)->GetText() != _T("")) return i;
		}
		return FindSelectable(0, true);
	}
}


