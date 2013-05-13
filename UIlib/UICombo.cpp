
#include "StdAfx.h"
#include "UICombo.h"
#include "coolsb_detours.h"

/////////////////////////////////////////////////////////////////////////////////////
//
//

class CUIComboBoxWnd : public CWindowWnd
{
public:
   void Init(CUIComboBox* pOwner);
   LPCTSTR GetWindowClassName() const;
   void OnFinalMessage(HWND hWnd);

   LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
   CUIPaintManager m_pm;
   CUIComboBox* m_pOwner;
   int m_iOldSel;
};


void CUIComboBoxWnd::Init(CUIComboBox* pOwner)
{
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

LPCTSTR CUIComboBoxWnd::GetWindowClassName() const
{
   return _T("DropDownWnd");
}

void CUIComboBoxWnd::OnFinalMessage(HWND hWnd)
{
	if (m_pOwner->m_pWindow != NULL)
	{
		m_pOwner->m_pWindow = NULL;
		m_pOwner->SendSelectNotify();
	}
   delete this;
}

LRESULT CUIComboBoxWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if( uMsg == WM_CREATE ) {   
      m_pm.Init(m_hWnd);
      // The trick is to add the items to the new container. Their owner gets
      // reassigned by this operation - which is why it is important to reassign
      // the items back to the righfull owner/manager when the window closes.
      CComboCanvasUI* pWindow = new CComboCanvasUI;
      CVerticalLayoutUI* pLayout = new CVerticalLayoutUI;
      for( int i = 0; i < m_pOwner->GetCount(); i++ ) 
	  {
		  pLayout->Add(static_cast<CUIControl*>(m_pOwner->GetItem(i)));
	  }
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

	   RECT rect;
	   ::GetWindowRect(m_hWnd, &rect);
	   POINT pt;
	   ::GetCursorPos(&pt);

	   if (m_pOwner->IsHasVScroll() && CUIPaintManager::m_pImageSkinVScroll != NULL)
	   {
		   rect.right -= CUIPaintManager::m_pImageSkinVScroll->GetWidth() / 4;
	   }

	   if (::PtInRect(&rect, pt))
	   {
			 PostMessage(WM_KILLFOCUS);
	   }  
   }
   else if( uMsg == WM_KEYDOWN ) {
      switch( wParam ) {
      case VK_ESCAPE:
         m_pOwner->SetCurSel(m_iOldSel);
         // FALL THROUGH...
      case VK_RETURN:
         PostMessage(WM_KILLFOCUS);
         break;
      default:
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


CUIComboBox::CUIComboBox() : 
   m_iCurSel(-1), 
   m_uButtonState(0)
{
   m_szDropBox = CUISize(0, 150);
   m_pWindow = NULL;
   ::ZeroMemory(&m_rcButton, sizeof(RECT));
   m_pImageOwner = NULL;

   m_nItemHeight = 10;

   m_bIsHasVScroll = false;
   m_nDropBoxHeight = 150;
}

CUIComboBox::~CUIComboBox()
{
   if (m_pImageOwner)
   {
	   delete m_pImageOwner;
	   m_pImageOwner = NULL;
   }
}

LPCTSTR CUIComboBox::GetClass() const
{
   return _T("DropDownUI");
}

LPVOID CUIComboBox::GetInterface(LPCTSTR pstrName)
{
   if( _tcscmp(pstrName, _T("ListOwner")) == 0 ) return static_cast<IUIListOwner*>(this);
   return CUIContainer::GetInterface(pstrName);
}

UINT CUIComboBox::GetControlFlags() const
{
   return UIFLAG_TABSTOP  | UIFLAG_SETCURSOR;
}

void CUIComboBox::Init()
{
   if( m_iCurSel < 0 ) SetCurSel(0);
}

int CUIComboBox::GetCurSel() const
{
   return m_iCurSel;
}

bool CUIComboBox::SetCurSel(int iIndex)
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

   CUIContainer* pContainer = dynamic_cast<CUIContainer*>(pListItem->GetOwner());
   if (pContainer)
   {
	   RECT rcItem = pControl->GetPos();
	   RECT rcList = pContainer->GetPos();
	   int iPos = pContainer->GetScrollPos();
	   if( rcItem.top < rcList.top || rcItem.bottom > rcList.bottom )
	   {
		   int dx = 0;
		   if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
		   if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
			pContainer->SetScrollPos(pContainer->GetScrollPos() + dx);
	   }

   }

   Invalidate();
   return true;
}

void CUIComboBox::SendSelectNotify()
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

bool CUIComboBox::Add(CUIControl* pControl)
{
	if (m_pWindow) 
	{
		return false;
	}

   IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
   if( pListItem != NULL ) {
      pListItem->SetOwner(this);
      pListItem->SetIndex(m_items.GetSize());
   }
   return CUIContainer::Add(pControl);
}

bool CUIComboBox::Remove(CUIControl* pControl)
{
   ASSERT(!"Not supported");
   return false;
}

void CUIComboBox::RemoveAll()
{
	if (m_pWindow) 
	{
		return;
	}

   m_iCurSel = -1;
   CUIContainer::RemoveAll();

   if (m_pManager) m_pManager->EmptyNameHashTable();
}

void CUIComboBox::Event(TEventUI& event)
{
	if( !CUIControl::Activate() ) return;

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
      if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
         if( ::PtInRect(&m_rcButton, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
         else m_uButtonState &= ~UISTATE_PUSHED;
         Invalidate();
      }

	  if (m_pWindow != NULL)
	  {
		  m_uButtonState |= UISTATE_PUSHED;
		  Invalidate();	 
	  }
   }
   if( event.Type == UIEVENT_BUTTONUP )
   {
      if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
         m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
         Invalidate();
      }
   }
   if( event.Type == UIEVENT_KEYDOWN )
   {
      switch( event.chKey ) {
      case VK_F4:
         Activate();
         return;
      case VK_UP:
         SetCurSel(FindSelectable(m_iCurSel - 1, false));
         return;
      case VK_DOWN:
         SetCurSel(FindSelectable(m_iCurSel + 1, true));
         return;
      }
   }
   if( event.Type == UIEVENT_SCROLLWHEEL )
   {
      bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
      //SetCurSel(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
      return;
   }

   if( event.Type == UIEVENT_MOUSEENTER)
   {
	   m_uButtonState |= UISTATE_HOT;
	   Invalidate();
   }
   if( event.Type == UIEVENT_MOUSELEAVE)
   {
	   m_uButtonState &= ~UISTATE_HOT;
	   Invalidate();
   }
   CUIControl::Event(event);
}

bool CUIComboBox::Activate()
{
   if( !CUIControl::Activate() ) return false;
   m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;

   if (m_pWindow) return true;
   m_pWindow = new CUIComboBoxWnd;

   SIZE szAvailable = { 0, 30 };
    m_szDropBox.cy = 0;
   for( int i = 0; i < m_items.GetSize(); i++ )
   {
	   CUIControl* pControl = static_cast<CUIControl*>(m_items[i]);

	   if (pControl)
	   {
		   SIZE size = pControl->EstimateSize(szAvailable);
		   if (m_nItemHeight < size.cy)
		   {
				m_szDropBox.cy += size.cy;
		   }
		   else
		   {
				m_szDropBox.cy += m_nItemHeight;

				CUIListLabelElement* pLabel = dynamic_cast<CUIListLabelElement*>(pControl);
				if (pLabel)
				{
					pLabel->SetHeight(m_nItemHeight);
				}

		   }

	   }
   }
   m_szDropBox.cy += 6;

   if (m_szDropBox.cy > m_nDropBoxHeight)
   {
	   m_szDropBox.cy = m_nDropBoxHeight;

	   m_bIsHasVScroll = true;
   }

   m_pWindow->Init(this);
   if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_DROPDOWN);
   Invalidate();
   return true;
}

CStdString CUIComboBox::GetText() const
{
   if( m_iCurSel < 0 ) return _T("");
   CUIControl* pControl = static_cast<CUIControl*>(m_items[m_iCurSel]);
   return pControl->GetText();
}

SIZE CUIComboBox::GetDropDownSize() const
{
   return m_szDropBox;
}

void CUIComboBox::SetDropDownSize(SIZE szDropBox)
{
   m_szDropBox = szDropBox;
}

void CUIComboBox::SetPos(RECT rc)
{
   // Put all elements out of sight
   RECT rcNull = { 0 };
   for( int i = 0; i < m_items.GetSize(); i++ ) static_cast<CUIControl*>(m_items[i])->SetPos(rcNull);
   // Position this control
   CUIControl::SetPos(rc);
}

SIZE CUIComboBox::EstimateSize(SIZE /*szAvailable*/)
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

void CUIComboBox::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Paint the nice frame
   int cy = m_rcItem.bottom - m_rcItem.top;
   ::SetRect(&m_rcButton, m_rcItem.right - cy, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
   RECT rcText = { m_rcItem.left + 5, m_rcItem.top, m_rcButton.left + 1, m_rcItem.bottom };

   UINT uState = m_uButtonState;
   if( IsFocused() ) uState |= UISTATE_FOCUSED;
   if( !IsEnabled() ) uState |= UISTATE_DISABLED;


	RECT rcPadding = { 0 };

   // Paint dropdown button
   if (m_pImageOwner != NULL)
   {
	   RECT rcImage = {0};
	   int nWidth = m_pImageOwner->GetWidth() / 4;
	   int nHeight = m_pImageOwner->GetHeight();

	   // Draw frame and body
	   if( (uState & UISTATE_DISABLED) != 0 ) {
		   RECT rcTemp = {nWidth *3, 0, nWidth * 4, nHeight};
		   rcImage = rcTemp;
	   }
	   else if( (uState & UISTATE_PUSHED) != 0 ) {
		   RECT rcTemp ={nWidth *2, 0, nWidth * 3, nHeight};
		   rcImage = rcTemp;
	   }
	   else if( (uState & UISTATE_HOT) != 0 ) {
		   RECT rcTemp ={nWidth *1, 0, nWidth * 2, nHeight};
		   rcImage = rcTemp;
	   }
	   else {
		   RECT rcTemp ={nWidth *0, 0, nWidth * 1, nHeight};
		   rcImage = rcTemp;
	   }

	   CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, rcImage, m_pImageOwner);

   }
   else
   {
	   CUIBlueRenderEngine::DoPaintButton(hDC, m_pManager, m_rcItem, uState, NULL, 0);
   }


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

   // Paint dropdown edit box
   ::InflateRect(&rcText, -1, -1);
   if( m_iCurSel >= 0 ) {
      CUIControl* pControl = static_cast<CUIControl*>(m_items[m_iCurSel]);
      IListItemUI* pElement = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
      if( pElement != NULL ) {
         // Render item with specific draw-style
		  int nLinks = 0;
		  CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcText, pControl->GetText(), UICOLOR_CONTROL_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_VCENTER);
      }
      else {
         //// Allow non-listitems to render as well.
         RECT rcOldPos = pControl->GetPos();
         pControl->SetPos(rcText);
         pControl->DoPaint(hDC, rcText);
         pControl->SetPos(rcOldPos);
      }
   }
}
bool CUIComboBox::SetHotItem(int iIndex)
 {
	return true;
 }

bool CUIComboBox::AddSubItem(UINT uIndex, LPCTSTR pstrText)
{
	CUIListLabelElement *pLabel = new CUIListLabelElement;
	if (pLabel == NULL)
	{
		return false;
	}

	TCHAR buffer[10];

	UINT uId = GetId() * 10 + uIndex;

	_itot(uId, buffer, 10);

	pLabel->SetName(buffer);

	pLabel->SetText(pstrText);
	pLabel->SetId(uId);
	
	if (pLabel->GetText() == _T(""))
	{
		pLabel->SetEnabled(false);
	}

	return Add(pLabel);
}

void CUIComboBox::SetImage(Image*	pImage)
{
	if (pImage)
	{
		m_pImageOwner = pImage->Clone();
	}
	Invalidate();
}

void CUIComboBox::SetImage(UINT nResourceID)
{
	CUIPaintManager::SetImage(&m_pImageOwner, nResourceID);
	Invalidate();
}

void CUIComboBox::SetImage(LPCTSTR pstrName)
{
	m_pImageOwner = Image::FromFile(pstrName);
	Invalidate();
}

DWORD_PTR CUIComboBox::GetItemData(int nItem)
{
	if( m_items.GetSize() == 0 )
	{
		return NULL;
	}

	if( nItem < 0 || nItem >= (int)m_items.GetSize())
	{
		return NULL;
	}

	CUIControl* pControl = static_cast<CUIControl*>(m_items[nItem]);
	if (pControl == NULL)
	{
		return NULL;
	}
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
	
	if( pListItem == NULL )
	{
		return NULL;
	}

	return pListItem->GetItemData();
}
bool CUIComboBox::SetItemData(int nItem, DWORD_PTR dwData)
{
	if( nItem < 0 || nItem >= (int)m_items.GetSize())
	{
		return false;
	}

	CUIControl* pControl = static_cast<CUIControl*>(m_items[nItem]);
	if (pControl == NULL)
	{
		return false;
	}
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));

	if( pListItem == NULL )
	{
		return false;
	}

	return pListItem->SetItemData(dwData);
}

void CUIComboBox::SetItemHeight(int nHeight)
{
	m_nItemHeight = nHeight;
}

int CUIComboBox::GetItemHeight()
{
	return m_nItemHeight;
}

CStdString CUIComboBox::GetSubItemText( UINT auIndex )
{
	if( auIndex < 0 || auIndex >= (int)m_items.GetSize())
	{
		return _T("");
	}

	CUIControl* lpControl = static_cast<CUIControl*>(m_items[auIndex]);
	if (lpControl == NULL)
	{
		lpControl->GetText();
		return _T("");
	}
	return lpControl->GetText();
}

void CUIComboBox::SetDropBoxHeight(int nHeight)
{
	m_nDropBoxHeight = nHeight;
}
int CUIComboBox::GetDropBoxHeight()
{
	return m_nDropBoxHeight;
}

bool CUIComboBox::IsHasVScroll()
{
	return m_bIsHasVScroll;
}

