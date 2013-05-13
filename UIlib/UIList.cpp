
#include "StdAfx.h"
#include "UIlist.h"

#include "UILabel.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIListElement::CUIListElement() : 
   m_iIndex(-1),
   m_pOwner(NULL), 
   m_bSelected(false)
{
}

UINT CUIListElement::GetControlFlags() const
{
   return UIFLAG_WANTRETURN;
}

LPVOID CUIListElement::GetInterface(LPCTSTR pstrName)
{
   if( _tcscmp(pstrName, _T("ListItem")) == 0 ) return static_cast<IListItemUI*>(this);
   return CUIControl::GetInterface(pstrName);
}

void CUIListElement::SetOwner(CUIControl* pOwner)
{
   m_pOwner = static_cast<IUIListOwner*>(pOwner->GetInterface(_T("ListOwner")));
}

CUIControl* CUIListElement::GetOwner()
{
	return reinterpret_cast<CUIControl*>(m_pOwner);
}

int CUIListElement::GetIndex() const
{
   return m_iIndex;
}

void CUIListElement::SetIndex(int iIndex)
{
   m_iIndex = iIndex;
}

bool CUIListElement::Activate()
{
   if( !CUIControl::Activate() ) return false;
   if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_ITEM_ACTIVATE);
   return true;
}

bool CUIListElement::IsSelected() const
{
   return m_bSelected;
}

bool CUIListElement::Select(bool bSelect)
{
	if( !IsEnabled() ) return false;
	if( bSelect == m_bSelected )
	{
		return true;
	}
   m_bSelected = bSelect;
   if( bSelect && m_pOwner != NULL ) m_pOwner->SetCurSel(m_iIndex);

   Invalidate();
   return true;
}

bool CUIListElement::IsExpanded() const
{
   return false;
}

bool CUIListElement::Expand(bool /*bExpand = true*/)
{
   return false;
}

void CUIListElement::Event(TEventUI& event)
{
   if( event.Type == UIEVENT_DBLCLICK && IsEnabled() )
   {
      Activate();
      Invalidate();
      return;
   }
   if( event.Type == UIEVENT_KEYDOWN && IsEnabled() )
   {
      if( event.chKey == VK_RETURN ) {
         Activate();
         Invalidate();
         return;
      }
   }
   // An important twist: The list-item will send the event not to its immediate
   // parent but to the "attached" list. A list may actually embed several components
   // in its path to the item, but key-presses etc. needs to go to the actual list.
   if( m_pOwner != NULL ) m_pOwner->Event(event); else CUIControl::Event(event);
}

void CUIListElement::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   if( _tcscmp(pstrName, _T("selected")) == 0 ) Select();
   else CUIControl::SetAttribute(pstrName, pstrValue);
}

bool CUIListElement::SetHot(bool bHot)
{
	return false;
}

DWORD_PTR CUIListElement::GetItemData()
{
	return m_dwData;
}
bool CUIListElement::SetItemData(DWORD_PTR dwData)
{
	m_dwData = dwData;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIListHeader::CUIListHeader()
{
	SetInset(CUISize(0, 0));
}

LPCTSTR CUIListHeader::GetClass() const
{
	return _T("ListHeaderUI");
}

LPVOID CUIListHeader::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("ListHeader")) == 0 ) return this;
	return CHorizontalLayoutUI::GetInterface(pstrName);
}

SIZE CUIListHeader::EstimateSize(SIZE /*szAvailable*/)
{
	return CUISize(0, 26 + m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void CUIListHeader::DoPaint(HDC hDC, const RECT& rcPaint)
{
	// Draw background
	COLORREF clrBack1, clrBack2;
	m_pManager->GetThemeColorPair(UICOLOR_HEADER_BACKGROUND, clrBack1, clrBack2);
	CUIBlueRenderEngine::DoPaintFrame(hDC, m_pManager, m_rcItem, UICOLOR_HEADER_BORDER, UICOLOR_HEADER_BORDER, UICOLOR_HEADER_BACKGROUND, 0);
	RECT rcBottom = { m_rcItem.left + 1, m_rcItem.bottom - 3, m_rcItem.right - 1, m_rcItem.bottom };
	CUIBlueRenderEngine::DoPaintGradient(hDC, m_pManager, rcBottom, clrBack1, clrBack2, true, 4);
	// Draw headers too...
	CHorizontalLayoutUI::DoPaint(hDC, rcPaint);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIListHeaderItem::CUIListHeaderItem() : m_uDragState(0)
{
}

LPCTSTR CUIListHeaderItem::GetClass() const
{
	return _T("ListHeaderItemUI");
}

void CUIListHeaderItem::SetText(LPCTSTR pstrText)
{
	m_sText = pstrText;
	UpdateLayout();
}

void CUIListHeaderItem::SetWidth(int cxWidth)
{
	m_cxWidth = cxWidth;
	UpdateLayout();
}

void CUIListHeaderItem::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("width")) == 0 ) SetWidth(_ttol(pstrValue));
	else CUIControl::SetAttribute(pstrName, pstrValue);
}

UINT CUIListHeaderItem::GetControlFlags() const
{
	return UIFLAG_SETCURSOR;
}

void CUIListHeaderItem::Event(TEventUI& event)
{
	if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
	{
		RECT rcSeparator = GetThumbRect(m_rcItem);
		if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
			m_uDragState |= UISTATE_CAPTURED;
			ptLastMouse = event.ptMouse;
			m_pManager->SendNotify(this, UI_NOTIFY_HEADER_DRAGGING);
		}
		else {
			m_pManager->SendNotify(this, UI_NOTIFY_HEADER_CLICK);
		}
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		if( (m_uDragState & UISTATE_CAPTURED) != 0 ) {
			m_uDragState &= ~UISTATE_CAPTURED;
			m_pManager->SendNotify(this, UI_NOTIFY_HEADER_DRAGGED);
			m_pManager->UpdateLayout();
		}
	}
	if( event.Type == UIEVENT_MOUSEMOVE )
	{
		if( (m_uDragState & UISTATE_CAPTURED) != 0 ) {
			RECT rc = m_rcItem;
			rc.right -= ptLastMouse.x - event.ptMouse.x;
			const int MIN_DRAGSIZE = 40;
			if( rc.right - rc.left > MIN_DRAGSIZE ) {
				m_rcItem = rc;
				m_cxWidth = rc.right - rc.left;
				ptLastMouse = event.ptMouse;
				m_pParent->Invalidate();
			}
		}
	}
	if( event.Type == UIEVENT_SETCURSOR )
	{
		RECT rcSeparator = GetThumbRect(m_rcItem);
		if( IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
			return;
		}
	}
	CUIControl::Event(event);
}

SIZE CUIListHeaderItem::EstimateSize(SIZE /*szAvailable*/)
{
	return CUISize(m_cxWidth,14 + m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight);
}

void CUIListHeaderItem::DoPaint(HDC hDC, const RECT& rcPaint)
{
	// Paint text (with some indent)
	RECT rcMessage = m_rcItem;
	rcMessage.left += 6;
	rcMessage.bottom -= 1;
	int nLinks = 0;
	CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcMessage, m_sText, UICOLOR_HEADER_TEXT, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_VCENTER);
	// Draw gripper
	POINT ptTemp = { 0 };
	RECT rcThumb = GetThumbRect(m_rcItem);
	RECT rc1 = { rcThumb.left + 2, rcThumb.top + 4, rcThumb.left + 2, rcThumb.bottom - 1 };
	CUIBlueRenderEngine::DoPaintLine(hDC, m_pManager, rc1, UICOLOR_HEADER_SEPARATOR);
	RECT rc2 = { rcThumb.left + 3, rcThumb.top + 4, rcThumb.left + 3, rcThumb.bottom - 1 };
	CUIBlueRenderEngine::DoPaintLine(hDC, m_pManager, rc2, UICOLOR_STANDARD_WHITE);
}

RECT CUIListHeaderItem::GetThumbRect(RECT rc) const
{
	return CUIRect(rc.right - 4, rc.top, rc.right, rc.bottom - 3);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIList::CUIList() : m_iCurSel(-1), m_iExpandedItem(-1)
{
	m_pControlsList = new CVerticalLayoutUI;
	m_pListHeader = new CUIListHeader;

	CWhiteCanvasUI* pWhite = new CWhiteCanvasUI;
	pWhite->Add(m_pControlsList);

	Add(m_pListHeader);
	CUIContainer::Add(pWhite);
	m_pControlsList->EnableScrollBar();

	::ZeroMemory(&m_oListInfo, sizeof(TListInfoUI));
}

LPCTSTR CUIList::GetClass() const
{
	return _T("ListUI");
}

UINT CUIList::GetControlFlags() const
{
	return UIFLAG_TABSTOP;
}

LPVOID CUIList::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("List")) == 0 ) return static_cast<IUIList*>(this);
	if( _tcscmp(pstrName, _T("ListOwner")) == 0 ) return static_cast<IUIListOwner*>(this);
	return CVerticalLayoutUI::GetInterface(pstrName);
}

bool CUIList::Add(CUIControl* pControl)
{
	// Override the Add() method so we can add items specifically to
	// the intended widgets. Headers and footers are assumed to be
	// answer the correct interface so we can add multiple list headers/footers.
	if( pControl->GetInterface(_T("ListHeader")) != NULL )
	{
		return CVerticalLayoutUI::Add(pControl);
	}

	// We also need to recognize header sub-items
	if( _tcsstr(pControl->GetClass(), _T("Header")) != NULL ) 
	{
		return m_pListHeader->Add(pControl);
	}
	bool bRet=FALSE;
	// The list items should know about us
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
	if( pListItem != NULL )
	{
		pListItem->SetOwner(this);
		pListItem->SetIndex(m_pControlsList->GetCount());

		bRet=m_pControlsList->Add(pControl);
	}

	ResetColumnRect();

	ResetItemIndexList();

	return bRet;
}

bool CUIList::Remove(CUIControl* pControl)
{
	return m_pControlsList->Remove(pControl);
}

bool CUIList::Remove( UINT uIndex )
{
	bool lbRet=false;
	CUIControl *lpControl=GetItem(uIndex);
	if(lpControl != NULL)
	{
		lbRet=m_pControlsList->Remove(lpControl);
		ResetColumnRect();
		ResetItemIndexList();
		m_iCurSel=-1;
		if(uIndex != 0)
		{
			SetCurSel(uIndex-1);
		}
		else
		{
			SetCurSel(0);
		}
	}

	return lbRet;
}
void CUIList::RemoveAll()
{
	m_iCurSel = -1;
	m_iExpandedItem = -1;
	m_pControlsList->RemoveAll();
	m_mItemIndexList.clear();
}

CUIControl* CUIList::GetItem(int iIndex) const
{
	if(iIndex < 0 || iIndex >= GetCount())
	{
		return NULL;
	}
	map<int, int>::const_iterator liter=m_mItemIndexList.find(iIndex);
	if(liter != m_mItemIndexList.end())
	{
		return m_pControlsList->GetItem(liter->second);
	}

	return NULL;
}

int CUIList::GetCount() const
{
	//return m_pControlsList->GetCount();
	return m_mItemIndexList.size();
}

void CUIList::SetPos(RECT rc)
{
	CVerticalLayoutUI::SetPos(rc);
}

void CUIList::Event(TEventUI& event)
{
	switch( event.Type ) 
	{
   case UIEVENT_KEYDOWN:
	   //**************************************************
	   switch( event.chKey )
	   {
	   case VK_UP:
		   SetCurSel(FindSelectable(m_iCurSel - 1, false));
		   EnsureVisible(m_iCurSel);
		   return;
	   case VK_DOWN:
		   SetCurSel(FindSelectable(m_iCurSel + 1, true));
		   EnsureVisible(m_iCurSel);
		   return;
	   case VK_PRIOR:
		   SetCurSel(FindSelectable(m_iCurSel - 10, false));
		   EnsureVisible(m_iCurSel);
		   return;
	   case VK_NEXT:
		   SetCurSel(FindSelectable(m_iCurSel + 10, true));
		   EnsureVisible(m_iCurSel);
		   return;
	   case VK_HOME:
		   SetCurSel(FindSelectable(0, false));
		   EnsureVisible(m_iCurSel);
		   return;
	   case VK_END:
		   SetCurSel(FindSelectable(GetCount() - 1, true));
		   EnsureVisible(m_iCurSel);
		   return;
	   case VK_RETURN:
		   if( m_iCurSel != -1 ) GetItem(m_iCurSel)->Activate();
		   return;
	   }
	   break;
	   //**************************************************
   case UIEVENT_SCROLLWHEEL:
	   {
		   switch( LOWORD(event.wParam) )
		   {
		   case SB_LINEUP:
			   m_pControlsList->SetScrollPos(m_pControlsList->GetScrollPos() - m_pControlsList->GetScrollLine());
			   return;
		   case SB_LINEDOWN:
			   m_pControlsList->SetScrollPos(m_pControlsList->GetScrollPos() + m_pControlsList->GetScrollLine());
			   return;
		   }
	   }
	   break;
   case UIEVENT_CONTEXTMENU:
	   {
		   if( m_pManager != NULL )
		   {
			   m_pManager->SendNotify(this, UI_NOTIFY_MENU_CLICK);
			   return;
		   }
	   }
	   break;
   case UIEVENT_DBLCLICK:
	   {
		   if( m_pManager != NULL )
		   {
			   m_pManager->SendNotify(this, UI_NOTIFY_DBCLICK);
			   return;
		   }
	   }
	   break;
	}
	CUIControl::Event(event);
}

CUIListHeader* CUIList::GetHeader() const
{
	return m_pListHeader;
}


CUIContainer* CUIList::GetList() const
{
	return m_pControlsList;
}

int CUIList::GetCurSel() const
{
	return m_iCurSel;
}

const TListInfoUI* CUIList::GetListInfo() const
{
	return &m_oListInfo;
}

void CUIList::SetExpanding(bool bExpandable)
{
	m_oListInfo.bExpandable = bExpandable;
}

bool CUIList::ExpandItem(int iIndex, bool bExpand /*= true*/)
{
	if( !m_oListInfo.bExpandable ) return false;
	if( m_iExpandedItem >= 0 ) {
		CUIControl* pControl = GetItem(m_iExpandedItem);
		if( pControl != NULL ) {
			IListItemUI* pItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pItem != NULL ) pItem->Expand(false);
		}
		m_iExpandedItem = -1;
	}
	if( bExpand ) {
		CUIControl* pControl = GetItem(iIndex);
		if( pControl == NULL ) return false;
		if( !pControl->IsVisible() ) return false;
		IListItemUI* pItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pItem == NULL ) return false;
		m_iExpandedItem = iIndex;
		if( !pItem->Expand(true) ) {
			m_iExpandedItem = -1;
			return false;
		}
	}
	UpdateLayout();
	return true;
}

int CUIList::GetExpandedItem() const
{
	return m_iExpandedItem;
}

void CUIList::EnsureVisible(int iIndex)
{
	if( m_iCurSel < 0 )
	{
		return;
	}
	RECT rcItem = m_pControlsList->GetItem(iIndex)->GetPos();
	RECT rcList = m_pControlsList->GetPos();
	int iPos = m_pControlsList->GetScrollPos();
	if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
	int dx = 0;
	if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
	if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
	Scroll(0, dx);
}

void CUIList::Scroll(int dx, int dy)
{
	if( dx == 0 && dy == 0 ) return;
	m_pControlsList->SetScrollPos(m_pControlsList->GetScrollPos() + dy);
}

void CUIList::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("header")) == 0 ) GetHeader()->SetVisible(_tcscmp(pstrValue, _T("hidden")) != 0);
	else if( _tcscmp(pstrName, _T("expanding")) == 0 ) SetExpanding(_tcscmp(pstrValue, _T("true")) == 0);
	else CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
}

bool CUIList::SetHotItem(int iIndex)
{
	return false;
}

int CUIList::InsertItem( UINT uIndex, LPCTSTR pstrText )
{
	CUIListTextElement *lpTextItem = new CUIListTextElement;
	if (lpTextItem == NULL)
	{
		return false;
	}

	lpTextItem->SetOwner(this);

	TCHAR buffer[10];

	UINT uId = GetId() * 10 + uIndex;

	_itot(uId, buffer, 10);

	lpTextItem->SetName(buffer);

	lpTextItem->SetSubItemText(0, pstrText);
	lpTextItem->SetId(uId);
	lpTextItem->SetItemHeight(m_oListInfo.ItemHeight);

	Add(lpTextItem);

	return GetCount()-1;
}

void CUIList::SetTextColor( COLORREF acrTextColor )
{
	m_oListInfo.Text=acrTextColor;
}

void CUIList::SetTextBkColor( COLORREF acrBkColor )
{
	m_oListInfo.Background=acrBkColor;
}

void CUIList::InsertColumn(LPCTSTR lpszItem, int nWidth)
{
	CUIListHeaderItem *pHeaderItem=new CUIListHeaderItem;
	if(pHeaderItem != NULL)
	{
		pHeaderItem->SetText(lpszItem);
		pHeaderItem->SetWidth(nWidth);

		Add(pHeaderItem);

		m_oListInfo.vColumnRectList.push_back(pHeaderItem->GetPos());

		ResetColumnRect();
	}
}
void CUIList::SetColumnHeaderVisible( bool bVisible )
{
	if(m_pListHeader != NULL)
	{
		m_pListHeader->SetVisible(bVisible);
	}
}

void CUIList::ResetColumnRect()
{
	if( m_pListHeader == NULL)
	{
			return;
	}
	int liColCount=m_oListInfo.vColumnRectList.size();
	RECT rc=CVerticalLayoutUI::GetPos();
	if( m_pListHeader->IsVisible() ) 
	{
		for( int i = 0; i < liColCount; i++ )
		{
			m_oListInfo.vColumnRectList.at(i) = m_pListHeader->GetItem(i)->GetPos();
		}
	}
	else 
	{
		RECT rcCol = { rc.left, 0, rc.left, 0 };
		for( int i = 0; i < liColCount; i++ ) 
		{
			SIZE sz = m_pListHeader->GetItem(i)->EstimateSize(CUISize(rc.right - rc.left, rc.bottom - rc.top));
			rcCol.right += sz.cx;
			m_oListInfo.vColumnRectList.at(i) = rcCol;
			::OffsetRect(&rcCol, sz.cx, 0);
		}
	}

	CVerticalLayoutUI::SetPos(rc);
}

bool CUIList::SetItemText( UINT uIndex, UINT uSubItem, LPCTSTR pstrText )
{
	if(uIndex != -1 && uSubItem != -1)
	{
		CUIListTextElement *lpTextControl=dynamic_cast<CUIListTextElement *>(GetItem(uIndex));
		if(lpTextControl != NULL)
		{
			lpTextControl->SetSubItemText(uSubItem, pstrText);

			return true;
		}
	}

	return false;
}

void CUIList::ResetItemIndexList()
{
	int liIndex=0;
	m_mItemIndexList.clear();
	int liCount=m_pControlsList->GetCount();
	for(int i=0;i < liCount;i++)
	{
		CUIControl *lpUIControl=m_pControlsList->GetItem(i);
		IListItemUI* pListItem = static_cast<IListItemUI*>(lpUIControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL )
		{
			pListItem->SetIndex(liIndex);
			m_mItemIndexList[liIndex]=i;
			liIndex++;
		}
	}
}

DWORD_PTR CUIList::GetItemData( int nItem )
{
	CUIControl *lpUIControl=GetItem(nItem);
	IListItemUI* pListItem = static_cast<IListItemUI*>(lpUIControl->GetInterface(_T("ListItem")));
	if( pListItem != NULL )
	{
		return pListItem->GetItemData();
	}

	return NULL;
}

bool CUIList::SetItemData( int nItem, DWORD_PTR dwData )
{
	CUIControl *lpUIControl=GetItem(nItem);
	IListItemUI* pListItem = static_cast<IListItemUI*>(lpUIControl->GetInterface(_T("ListItem")));
	if( pListItem != NULL )
	{
		return pListItem->SetItemData(dwData);
	}

	return false;
}

void CUIList::SetTextBkSelImage( Image *pImage )
{
	if(pImage != NULL)
	{
		m_oListInfo.SelImage=pImage->Clone();
	}
}

void CUIList::SetTextSelColor( COLORREF acrTextSelColor )
{
	m_oListInfo.SelText=acrTextSelColor;
}

void CUIList::SetTextBkSelColor( COLORREF acrBkSelColor )
{
	m_oListInfo.SelBackground=acrBkSelColor;
}

void CUIList::SetItemHeight( int iHeight )
{
	m_oListInfo.ItemHeight=iHeight;
}

bool CUIList::SetCurSel( int iIndex )
{
	if( iIndex == m_iCurSel ) return true;
	// We should first unselect the currently selected item
	if( m_iCurSel >= 0 )
	{
		CUIControl* pControl = GetItem(m_iCurSel);
		if( pControl != NULL ) 
		{
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->Select(false);
		}
	}
	// Now figure out if the control can be selected
	// TODO: Redo old selected if failure
	CUIControl* pControl = GetItem(iIndex);
	if( pControl == NULL ) return false;
	if( !pControl->IsVisible() ) return false;
	if( !pControl->IsEnabled() ) return false;
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
	if( pListItem == NULL ) return false;
	m_iCurSel = iIndex;
	if( !pListItem->Select(true) )
	{
		m_iCurSel = -1;
		return false;
	}
	
	pControl->SetFocus();

	if( m_pManager != NULL )
	{
		m_pManager->SendNotify(pControl, UI_NOTIFY_ITEM_CLICK);
		m_pManager->SendNotify(this, UI_NOTIFY_ITEM_SELECT_CHANGE);
	}
	int liNewScrollPos=iIndex*m_pControlsList->GetScrollLine();
	if(liNewScrollPos < m_pControlsList->GetScrollPos())
	{
		m_pControlsList->SetScrollPos(liNewScrollPos);
	}
	else if(liNewScrollPos > m_pControlsList->GetScrollPage()+m_pControlsList->GetScrollPos())
	{
		m_pControlsList->SetScrollPos(liNewScrollPos);
	}

	Invalidate();
	return true;
}

LPCTSTR CUIList::GetItemText( UINT uIndex )
{
	CUIListTextElement *lpTextControl=dynamic_cast<CUIListTextElement *>(GetItem(uIndex));
	if(lpTextControl != NULL)
	{
		return lpTextControl->GetSubItemText(0);
	}

	return NULL;
}

bool CUIList::SetItemToolTip( UINT uIndex, LPCTSTR pstrText )
{
	if(uIndex != -1)
	{
		CUIListTextElement *lpTextControl=dynamic_cast<CUIListTextElement *>(GetItem(uIndex));
		if(lpTextControl != NULL)
		{
			lpTextControl->SetToolTip(pstrText);

			return true;
		}
	}

	return false;
}
/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIListLabelElement::CUIListLabelElement() : m_cxWidth(0), m_uTextStyle(DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS), m_uButtonState(0)
{
	m_pImage = NULL;
	m_nHeight = 0;
}

LPCTSTR CUIListLabelElement::GetClass() const
{
   return _T("ListLabelElementUI");
}

void CUIListLabelElement::SetWidth(int cx)
{
   m_cxWidth = cx;
   Invalidate();
}

void CUIListLabelElement::SetHeight(int cx)
{
	m_nHeight = cx;
	Invalidate();
}

void CUIListLabelElement::SetTextStyle(UINT uStyle)
{
   m_uTextStyle = uStyle;
   Invalidate();
}

void CUIListLabelElement::Event(TEventUI& event)
{
	if(IsEnabled())
	{
		if( event.Type == UIEVENT_BUTTONDOWN
			|| event.Type == UIEVENT_CONTEXTMENU
			|| event.Type == UIEVENT_DBLCLICK)
		{
			Select();
			Invalidate();
			if(m_pOwner != NULL)
			{
				m_pOwner->Event(event);
			}
			return;
		}
	}

   if( event.Type == UIEVENT_MOUSEENTER )
   {
      m_uButtonState |= UISTATE_HOT;
      Invalidate();
   }
   if( event.Type == UIEVENT_MOUSELEAVE )
   {
      if( (m_uButtonState & UISTATE_HOT) != 0 ) {
         m_uButtonState &= ~UISTATE_HOT;
         Invalidate();
      }
   }
   CUIListElement::Event(event);
}

void CUIListLabelElement::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   if( _tcscmp(pstrName, _T("width")) == 0 ) {
      SetWidth(_ttoi(pstrValue));
   }
   else if( _tcscmp(pstrName, _T("align")) == 0 ) {
      if( _tcsstr(pstrValue, _T("center")) != NULL ) m_uTextStyle |= DT_CENTER;
      if( _tcsstr(pstrValue, _T("right")) != NULL ) m_uTextStyle |= DT_RIGHT;
   }
   else CUIListElement::SetAttribute(pstrName, pstrValue);
}

SIZE CUIListLabelElement::EstimateSize(SIZE /*szAvailable*/)
{
	if (m_nHeight > 0)
	{
		return CUISize(m_cxWidth - 80, m_nHeight);
	}
	return CUISize(m_cxWidth - 80, m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight + 2);
}

void CUIListLabelElement::DoPaint(HDC hDC, const RECT& rcPaint)
{
   DrawItem(hDC, GetPos(), 0);
}

void CUIListLabelElement::SetImage(Image *pImage)
{
	m_pImage = pImage;
}

void CUIListLabelElement::DrawItem(HDC hDC, const RECT& rcItem, UINT uDrawStyle)
{
   UITYPE_COLOR iTextColor = UICOLOR_CONTROL_TEXT_NORMAL;
   UITYPE_COLOR iBackColor = UICOLOR__INVALID;
   if( (m_uButtonState & UISTATE_HOT) != 0 ) {
      iTextColor = UICOLOR_CONTROL_TEXT_NORMAL;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_HOVER;
   }
   if( m_bSelected ) {
      iTextColor = UICOLOR_CONTROL_TEXT_SELECTED;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_SELECTED;
   }
   if( (uDrawStyle & UIDRAWSTYLE_INPLACE) != 0 )
   {
      iTextColor = UICOLOR_CONTROL_TEXT_NORMAL;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_NORMAL;
   }
   if( (uDrawStyle & UIDRAWSTYLE_FOCUS) != 0 )
   {
      iTextColor = UICOLOR_CONTROL_TEXT_NORMAL;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_HOVER;
   }
   if( !m_bEnabled ) {
      iTextColor = UICOLOR_CONTROL_TEXT_DISABLED;
      iBackColor = UICOLOR_CONTROL_BACKGROUND_DISABLED;
   }
   // Paint background (because we're vertically centering the text area
   // so it cannot paint the entire item rectangle)

   Image* pImage =  m_pManager ? m_pManager->m_pImageSkinCombo : NULL;
   if (pImage == NULL)
   {
	   if( iBackColor != UICOLOR__INVALID ) {
		   CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcItem, iBackColor);
	   }
   }
   else
   {
	   int nWidth = pImage->GetWidth() / 4;
	   int nHeight = pImage->GetHeight();
	   

	   if ((m_uButtonState & UISTATE_HOT) != 0)
	   {
		   int nIdx = 1;
		   RECT table = {nWidth * nIdx, 0, nWidth * (nIdx + 1) , nHeight};
		   CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcItem, table, pImage);
	   }

	   if (m_bSelected)
	   {
		   int nIdx = 2;
		   RECT table = {nWidth * nIdx, 0, nWidth * (nIdx + 1) , nHeight};
		   CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcItem, table, pImage);
	   }

   }

   // Paint text
   RECT rcText = rcItem;
   ::InflateRect(&rcText, -4, 0);
   int nLinks = 0;
   CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcText, m_sText, iTextColor, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIListTextElement::CUIListTextElement() : m_iItemHeight(0), m_nLinks(0), m_pOwner(NULL)
{
   ::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
}

LPCTSTR CUIListTextElement::GetClass() const
{
   return _T("ListTextElementUI");
}

UINT CUIListTextElement::GetControlFlags() const
{
   return UIFLAG_WANTRETURN | (m_nLinks > 0 ? UIFLAG_SETCURSOR : 0);
}

void CUIListTextElement::SetOwner(CUIControl* pOwner)
{
   CUIListElement::SetOwner(pOwner);
   m_pOwner = static_cast<IUIList*>(pOwner->GetInterface(_T("List")));
}

void CUIListTextElement::Event(TEventUI& event)
{
   // When you hover over a link
   if( event.Type == UIEVENT_SETCURSOR ) {
      for( int i = 0; i < m_nLinks; i++ ) {
         if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
            return;
         }
      }      
   }
   if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE ) Invalidate();
   if( m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE ) Invalidate();
   CUIListLabelElement::Event(event);
}

SIZE CUIListTextElement::EstimateSize(SIZE szAvailable)
{
   if( m_pOwner == NULL ) return CUISize();
   // We calculate the item height only once, because it will not wrap on the
   // line anyway when the screen is resized.
   if( m_iItemHeight == 0 )
   {
	   LPCTSTR pstrText = _T("XXX");
	   RECT rcText = { 0, 0, 9999, 9999 };
	   int nLinks = 0;
	   CUIBlueRenderEngine::DoPaintPrettyText(m_pManager->GetPaintDC(), m_pManager, rcText, pstrText, UICOLOR__INVALID, UICOLOR__INVALID, NULL, nLinks, DT_CALCRECT | m_uTextStyle);
	   m_iItemHeight = rcText.bottom - rcText.top;
   }

   return CUISize(m_cxWidth, m_iItemHeight + 9);
}

void CUIListTextElement::DoPaint(HDC hDC, const RECT& rcPaint)
{
   DrawItem(hDC, m_rcItem, 0);
}

void CUIListTextElement::DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle)
{
   ASSERT(m_pOwner);
   if( m_pOwner == NULL ) return;
   COLORREF lcrTextColor=INVALID__UICOLOR;
   COLORREF lcrBackColor=INVALID__UICOLOR;
   Image *lpSelImage=NULL;

   const TListInfoUI* pInfo = m_pOwner->GetListInfo();

    lcrTextColor= pInfo->Text;
    lcrBackColor= pInfo->Background;

   if( IsSelected())
   {
      lcrTextColor = pInfo->SelText;    
	  lpSelImage = pInfo->SelImage;
	  if(lpSelImage == NULL)
	  {
		  lcrBackColor = pInfo->SelBackground;
	  }
	  RECT rcItem={m_rcItem.left, m_rcItem.top + 1, m_rcItem.right, m_rcItem.bottom};
	  if(lpSelImage != NULL)
	  {
		  RECT rcImage={0, 0, lpSelImage->GetWidth(), lpSelImage->GetHeight()};
		  CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcItem, rcImage, lpSelImage);
	  }
	  else if( lcrBackColor != INVALID__UICOLOR)
	  {
		  CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcItem, lcrBackColor);
	  }
   }
   m_nLinks = 0;
   int nLinks = lengthof(m_rcLinks);
   int liColCount=pInfo->vColumnRectList.size();
   for( int i = 0; i < liColCount; i++ )
   {
      // Paint text
      RECT rcItem = { pInfo->vColumnRectList.at(i).left, m_rcItem.top + 1, pInfo->vColumnRectList.at(i).right, m_rcItem.bottom - 1 };
      LPCTSTR pstrText = m_mSubItemList[i];
      ::InflateRect(&rcItem, -4, 0);
      CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcItem, pstrText, lcrTextColor, INVALID__UICOLOR, m_rcLinks, nLinks, DT_SINGLELINE | m_uTextStyle);
      if( nLinks > 0 )
	  {
		  m_nLinks = nLinks, nLinks = 0;
	  }
	  else
	  {
		  nLinks = lengthof(m_rcLinks);
	  }
   }
}

void CUIListTextElement::SetSubItemText( int iSubIndex, LPCTSTR pstrSubText )
{
	int iCollumnCount=0;
	if( m_pOwner == NULL )
	{
		return;
	}
	const TListInfoUI* pInfo = m_pOwner->GetListInfo();
	if(pInfo != NULL)
	{
		iCollumnCount=pInfo->vColumnRectList.size();
	}
	if(iSubIndex >= 0 && iSubIndex < iCollumnCount)
	{
		m_mSubItemList[iSubIndex]=pstrSubText;
	}
}

LPCTSTR CUIListTextElement::GetSubItemText( int iSubIndex )
{
	map<int , CStdString>::iterator literFind=m_mSubItemList.find(iSubIndex);
	if(literFind != m_mSubItemList.end())
	{
		return literFind->second;
	}
	return NULL;
}

void CUIListTextElement::SetItemHeight( int iHeight )
{
	m_iItemHeight=iHeight-9 < 0 ? 0 : iHeight-9;
}
/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIListExpandElement::CUIListExpandElement() : m_bExpanded(false), m_cyExpanded(0), m_pContainer(NULL)
{
}

CUIListExpandElement::~CUIListExpandElement()
{
   delete m_pContainer;
}

LPCTSTR CUIListExpandElement::GetClass() const
{
   return _T("ListExpandElementUI");
}

bool CUIListExpandElement::Expand(bool bExpand)
{
   ASSERT(m_pOwner);
   if( m_pOwner == NULL ) return false;  
   if( bExpand == m_bExpanded ) return true;
   m_bExpanded = bExpand;
   if( !m_pOwner->ExpandItem(m_iIndex, bExpand) ) return false;
   // We need to manage the "expanding items", which are actually embedded controls
   // that we selectively display or not.
   if( bExpand ) 
   {
      delete m_pContainer;
      CTileLayoutUI* pTile = new CTileLayoutUI;
      pTile->SetPadding(4);
      m_pContainer = pTile;
      if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_ITEM_EXPAND);
      m_pManager->InitControls(m_pContainer, this);
   }
   else
   {
      if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_ITEM_COLLAPSE);
   }
   m_cyExpanded = 0;
   return true;
}

bool CUIListExpandElement::IsExpanded() const
{
   return m_bExpanded;
}

void CUIListExpandElement::Event(TEventUI& event)
{
   if( event.Type == UIEVENT_BUTTONUP )
   {
      if( m_pOwner == NULL ) return;
      const TListInfoUI* pInfo = m_pOwner->GetListInfo();
      RECT rcExpander = { m_rcItem.left, m_rcItem.top, m_rcItem.left + 20, m_rcItem.bottom };;
      if( pInfo->bExpandable && ::PtInRect(&rcExpander, event.ptMouse) ) Expand(!m_bExpanded);
   }
   if( event.Type == UIEVENT_KEYDOWN )
   {
      switch( event.chKey ) {
      case VK_LEFT:
         Expand(false);
         return;
      case VK_RIGHT:
         Expand(true);
         return;
      }
   }
   CUIListTextElement::Event(event);
}

SIZE CUIListExpandElement::EstimateSize(SIZE szAvailable)
{
   if( m_pOwner == NULL ) return CUISize();
   // We calculate the item height only once, because it will not wrap on the
   // line anyway when the screen is resized.
   if( m_iItemHeight == 0 ) {
      LPCTSTR pstrText = _T("XXX");
      RECT rcText = { 0, 0, 9999, 9999 };
      int nLinks = 0;
      CUIBlueRenderEngine::DoPaintPrettyText(m_pManager->GetPaintDC(), m_pManager, rcText, pstrText, UICOLOR__INVALID, UICOLOR__INVALID, NULL, nLinks, DT_CALCRECT | m_uTextStyle);
      m_iItemHeight = (rcText.bottom - rcText.top) + 8;
   }
   int cyItem = m_iItemHeight;
   if( m_bExpanded && m_cyExpanded == 0 && m_pContainer != NULL && m_pContainer->GetCount() > 0 ) {
      RECT rcItem = m_pContainer->GetItem(0)->GetPos();
      // HACK: Here we boldy assume that the item height is the same as the listitem.
      m_cyExpanded = ((rcItem.bottom - rcItem.top) + 14) * ((m_pContainer->GetCount() + 1) / 2);
      m_cyExpanded += 4 * ((m_pContainer->GetCount() - 1) / 2);
      m_cyExpanded += 26;
   }
   return CUISize(m_cxWidth, cyItem + m_cyExpanded + 1);
}

void CUIListExpandElement::SetPos(RECT rc)
{
   if( m_pContainer != NULL ) {
      RECT rcSubItems = { rc.left + 14, rc.top + m_iItemHeight, rc.right - 8, rc.bottom - 6 };
      m_pContainer->SetPos(rcSubItems);
   }
   CUIListTextElement::SetPos(rc);
}

bool CUIListExpandElement::Add(CUIControl* pControl)
{
   ASSERT(m_pContainer);
   if( m_pContainer == NULL ) return false;
   return m_pContainer->Add(pControl);
}

bool CUIListExpandElement::Remove(CUIControl* pControl)
{
   ASSERT(!"Not supported yet");
   return false; 
}

void CUIListExpandElement::RemoveAll()
{
   if( m_pContainer != NULL ) m_pContainer->RemoveAll();
}

CUIControl* CUIListExpandElement::GetItem(int iIndex) const
{
   if( m_pContainer == NULL ) return NULL;
   return m_pContainer->GetItem(iIndex);
}

int CUIListExpandElement::GetCount() const
{
   if( m_pContainer == NULL ) return 0;
   return m_pContainer->GetCount();
}

void CUIListExpandElement::DoPaint(HDC hDC, const RECT& rcPaint)
{
   CUIListTextElement::DoPaint(hDC, rcPaint);
   // Paint the expanded items?
   if( m_bExpanded && m_pContainer != NULL ) {
      // Paint gradient box for the items
      RECT rcFrame = m_pContainer->GetPos();
      COLORREF clr1, clr2;
      m_pManager->GetThemeColorPair(UICOLOR_CONTROL_BACKGROUND_EXPANDED, clr1, clr2);
      CUIBlueRenderEngine::DoPaintGradient(hDC, m_pManager, rcFrame, clr1, clr2, true, 64);
      CUIBlueRenderEngine::DoPaintRectangle(hDC, m_pManager, rcFrame, UICOLOR_CONTROL_BORDER_NORMAL, UICOLOR__INVALID);
      RECT rcLine = { m_rcItem.left, rcFrame.top, m_rcItem.right, rcFrame.top };
      CUIBlueRenderEngine::DoPaintLine(hDC, m_pManager, rcLine, UICOLOR_STANDARD_BLACK);
      // We'll draw the items then...
      m_pContainer->DoPaint(hDC, rcPaint);
   }
}

void CUIListExpandElement::DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle)
{
   ASSERT(m_pOwner);
   if( m_pOwner == NULL ) return;
   const TListInfoUI* pInfo = m_pOwner->GetListInfo();
   COLORREF lcrTextColor = pInfo->Text;
   COLORREF lcrBackColor = pInfo->Background;
   if( IsSelected() ) {
      lcrTextColor = pInfo->SelText;
      lcrBackColor = pInfo->SelBackground;
   }
   if( lcrBackColor != UICOLOR__INVALID ) {
      RECT rcItem = { m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.top + m_iItemHeight };
      CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcItem, lcrBackColor);
   }
   m_nLinks = 0;
   CStdString sColText;
   int nLinks = lengthof(m_rcLinks);
   int liColCount=pInfo->vColumnRectList.size();
   for( int i = 0; i < liColCount; i++ )
   {
      // Paint text
      RECT rcItem = { pInfo->vColumnRectList.at(i).left, m_rcItem.top, pInfo->vColumnRectList.at(i).right, m_rcItem.top + m_iItemHeight };
      LPCTSTR pstrText =_T("DrawItem"); //pCallback->GetItemText(this, m_iIndex, i);
      // If list control is expandable then we'll automaticially draw
      // the expander-box at the first column.
      if( i == 0 && pInfo->bExpandable ) {
         sColText = (m_bExpanded ? _T("<i 14> ") : _T("<i 13> "));
         sColText += pstrText;
         pstrText = sColText;
      }
      ::InflateRect(&rcItem, -4, 0);
      CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcItem, pstrText, lcrTextColor, UICOLOR__INVALID, m_rcLinks, nLinks, DT_SINGLELINE | m_uTextStyle);
      if( nLinks > 0 ) m_nLinks = nLinks, nLinks = 0; else nLinks = lengthof(m_rcLinks);
   }
   RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
   CUIBlueRenderEngine::DoPaintLine(hDC, m_pManager, rcLine, UICOLOR_DIALOG_BACKGROUND);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUISubMenu::CUISubMenu() : m_cxWidth(0), m_uTextStyle(DT_VCENTER), m_uButtonState(0)
{
}

LPCTSTR CUISubMenu::GetClass() const
{
	return _T("ListMenuLabelElementUI");
}

void CUISubMenu::SetWidth(int cx)
{
	m_cxWidth = cx;
	Invalidate();
}

void CUISubMenu::SetTextStyle(UINT uStyle)
{
	m_uTextStyle = uStyle;
	Invalidate();
}

void CUISubMenu::Event(TEventUI& event)
{
	if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
	{
		if (!IsEnabled()) return;
		if(  m_pOwner != NULL ) m_pOwner->SetCurSel(m_iIndex);
		Invalidate();
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if(  m_pOwner != NULL ) m_pOwner->SetHotItem(m_iIndex);
		
		m_uButtonState |= UISTATE_HOT;
		Invalidate();
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
	}
	CUIListElement::Event(event);
}

void CUISubMenu::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("width")) == 0 ) {
		SetWidth(_ttoi(pstrValue));
	}
	else if( _tcscmp(pstrName, _T("align")) == 0 ) {
		if( _tcsstr(pstrValue, _T("center")) != NULL ) m_uTextStyle |= DT_CENTER;
		if( _tcsstr(pstrValue, _T("right")) != NULL ) m_uTextStyle |= DT_RIGHT;
	}
	else CUIListElement::SetAttribute(pstrName, pstrValue);
}

SIZE CUISubMenu::EstimateSize(SIZE /*szAvailable*/)
{
	if (GetText() == _T(""))
	{
		return CUISize(m_cxWidth, 8);
	}

	return CUISize(m_cxWidth, m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight + 11);
}

void CUISubMenu::DoPaint(HDC hDC, const RECT& rcPaint)
{
	DrawItem(hDC, GetPos(), 0);
}

void CUISubMenu::SetText(LPCTSTR pstrText)
{
	CUIControl::SetText(pstrText);
	// Automatic assignment of keyboard shortcut
	if( _tcschr(pstrText, '&') != NULL ) m_chShortcut = *(_tcschr(pstrText, '&') + 1);
}

void CUISubMenu::DrawItem(HDC hDC, const RECT& rcItem, UINT uDrawStyle)
{
	if (GetText() == _T(""))
	{

		Image* pImage = m_pManager ? m_pManager->m_pImageSkinMenu : NULL;
		if (pImage)
		{
			int nWidth = pImage->GetWidth();
			int nHeight = (pImage->GetHeight() - 8) / 3;
			RECT table = {0, nHeight * 3, nWidth , nHeight * 3 + 8};
			CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, table, pImage);
		}
		return;
	}


	UITYPE_COLOR iTextColor = UICOLOR_CONTROL_TEXT_NORMAL;
	UITYPE_COLOR iBackColor = UICOLOR__INVALID;

	if( !m_bEnabled ) {
		iTextColor = UICOLOR_CONTROL_TEXT_DISABLED;
		iBackColor = UICOLOR_CONTROL_BACKGROUND_DISABLED;
	}

	// Paint background (because we're vertically centering the text area
	// so it cannot paint the entire item rectangle)

	if( (m_uButtonState & UISTATE_HOT)) 
	{
		Image* pImage = m_pManager ? m_pManager->m_pImageSkinMenu : NULL;
		if (pImage)
		{
			int nWidth = pImage->GetWidth();
			int nHeight = (pImage->GetHeight() - 8) / 3;
			
			if (!m_bEnabled)
			{
				RECT table = {0, nHeight * 2, nWidth , nHeight * 3};
				CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, table, pImage);
			}
			else
			{
				RECT table = {0, nHeight * 1, nWidth , nHeight * 2};
				CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, table, pImage);
			}
		}

	}

	if( iBackColor != UICOLOR__INVALID && IsEnabled()) {
	
	}

	RECT rcText = rcItem;


	if (m_sText.GetLength() > 2 && (m_sText.GetAt(0) == '<' && m_sText.GetAt(1) == 'i'))
	{
		rcText.left += 4;
	}
	else
	{
		rcText.left += 20;
	}
	
	rcText.top += 3;
	rcText.bottom -= 3;

	int nLinks = 0;
	CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcText, m_sText, iTextColor, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
}

bool CUISubMenu::Activate()
{
	if( !CUIControl::Activate() ) return false;
	Select();

	m_uButtonState |= UISTATE_HOT;
	return true;
}

bool CUISubMenu::SetHot(bool bHot)
{
	if (bHot)
	{
		m_uButtonState |= UISTATE_HOT;
		Invalidate();
	}
	else
	{
		m_uButtonState &= ~UISTATE_HOT;
		Invalidate();
	}
	return true;
}

