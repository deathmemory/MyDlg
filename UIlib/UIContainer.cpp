
#include "StdAfx.h"
#include "UIContainer.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIContainer::CUIContainer() : 
   m_pVScroll(NULL), 
   m_iPadding(0),
   m_iScrollPos(0),
   m_bAutoDestroy(true),
   m_bAllowScrollbars(false),
   m_pImageVScroll(NULL)
{
   m_cxyFixed.cx = m_cxyFixed.cy = 0;
   ::ZeroMemory(&m_rcInset, sizeof(m_rcInset));
   m_iScrollBtnWidth=0;
   m_iScrollBtnHeight=0;
}

CUIContainer::~CUIContainer()
{
   RemoveAll();
}

LPCTSTR CUIContainer::GetClass() const
{
   return _T("ContainerUI");
}

LPVOID CUIContainer::GetInterface(LPCTSTR pstrName)
{
   if( _tcscmp(pstrName, _T("Container")) == 0 ) return static_cast<IContainerUI*>(this);
   return CUIControl::GetInterface(pstrName);
}

CUIControl* CUIContainer::GetItem(int iIndex) const
{
   if( iIndex < 0 || iIndex >= m_items.GetSize() ) return NULL;
   return static_cast<CUIControl*>(m_items[iIndex]);
}

int CUIContainer::GetCount() const
{
   return m_items.GetSize();
}

bool CUIContainer::Add(CUIControl* pControl)
{
   if( m_pManager != NULL ) m_pManager->InitControls(pControl, this);
   if( m_pManager != NULL ) m_pManager->UpdateLayout();
   return m_items.Add(pControl);
}

bool CUIContainer::Remove(CUIControl* pControl)
{
   for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ )
   {
      if( static_cast<CUIControl*>(m_items[it]) == pControl )
	  {
		  if( m_pManager != NULL )
		  {
			  m_pManager->EmptyNameHashTable();
			  m_pManager->UpdateLayout();
		  }
         delete pControl;
         return m_items.Remove(it);
      }
   }
   return false;
}

void CUIContainer::RemoveAll()
{
	for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ )
	{
		delete static_cast<CUIControl*>(m_items[it]);
	}
	m_pVScroll=NULL;
	m_items.Empty();
	m_iScrollPos = 0;
	if( m_pManager != NULL )
	{
		m_pManager->EmptyNameHashTable();
		m_pManager->UpdateLayout();
	}
}

void CUIContainer::SetAutoDestroy(bool bAuto)
{
   m_bAutoDestroy = bAuto;
}

void CUIContainer::SetInset(SIZE szInset)
{
   m_rcInset.left = m_rcInset.right = szInset.cx;
   m_rcInset.top = m_rcInset.bottom = szInset.cy;
}

void CUIContainer::SetInset(RECT rcInset)
{
   m_rcInset = rcInset;
}

void CUIContainer::SetPadding(int iPadding)
{
   m_iPadding = iPadding;
}

void CUIContainer::SetWidth(int cx)
{
   m_cxyFixed.cx = cx;
}

void CUIContainer::SetHeight(int cy)
{
   m_cxyFixed.cy = cy;
}

void CUIContainer::SetVisible(bool bVisible)
{
   // Hide possible scrollbar control
   if( m_pVScroll != NULL ) m_pVScroll->SetVisible(bVisible);
   // Hide children as well
   for( int it = 0; it < m_items.GetSize(); it++ ) {
      static_cast<CUIControl*>(m_items[it])->SetVisible(bVisible);
   }
   CUIControl::SetVisible(bVisible);
}

void CUIContainer::Event(TEventUI& event)
{
   if( m_pVScroll != NULL ) 
   {
      if( event.Type == UIEVENT_VSCROLL ) 
      {
         switch( LOWORD(event.wParam) ) {
         case SB_THUMBPOSITION:
         case SB_THUMBTRACK:
            {
               SetScrollPos(m_pVScroll->GetCurrentPos());
            }
            break;
         case SB_LINEUP:
            SetScrollPos(m_pVScroll->GetCurrentPos());
            break;
         case SB_LINEDOWN:
            SetScrollPos(m_pVScroll->GetCurrentPos());
            break;
         case SB_PAGEUP:
            SetScrollPos(GetScrollPos() - GetScrollPage());
            break;
         case SB_PAGEDOWN:
            SetScrollPos(GetScrollPos() + GetScrollPage());
            break;
         }
      }
      if( event.Type == UIEVENT_KEYDOWN ) 
      {
         switch( event.chKey ) {
         case VK_DOWN:
            SetScrollPos(GetScrollPos() + GetScrollLine());
            return;
         case VK_UP:
            SetScrollPos(GetScrollPos() - GetScrollLine());
            return;
         case VK_NEXT:
            SetScrollPos(GetScrollPos() + GetScrollPage());
            return;
         case VK_PRIOR:
            SetScrollPos(GetScrollPos() - GetScrollPage());
            return;
         case VK_HOME:
            SetScrollPos(0);
            return;
         case VK_END:
            SetScrollPos(9999);
            return;
         }
      }
   }
   CUIControl::Event(event);
}

int CUIContainer::GetScrollPos() const
{
   return m_iScrollPos;
}

int CUIContainer::GetScrollPage() const
{
   // TODO: Determine this dynamically
   if( m_pVScroll == NULL )
   {
	   return 40;
   }
   else
   {
	   return m_pVScroll->GetPageSize();
   }
}

int CUIContainer::GetScrollLine() const
{
	// TODO: Determine this dynamically
	if( m_pVScroll == NULL )
	{
		return 1;
	}
	else
	{
		return m_pVScroll->GetLineSize();
	}
}

SIZE CUIContainer::GetScrollRange() const
{
   if( m_pVScroll == NULL ) return CUISize();
   int cx = 0, cy = 0;

   m_pVScroll->GetRange(cx, cy);
   return CUISize(cx, cy);
}

void CUIContainer::SetScrollPos(int iScrollPos)
{
   if( m_pVScroll == NULL ) return;
   int iRange1 = 0, iRange2 = 0;
   m_pVScroll->GetRange(iRange1, iRange2);
  
   iScrollPos = CLAMP(iScrollPos, iRange1, iRange2);

   m_pVScroll->SetCurrentPos(iScrollPos);

   m_iScrollPos = m_pVScroll->GetCurrentPos();

   // Reposition children to the new viewport.
   SetPos(m_rcItem);
   Invalidate();
}

void CUIContainer::EnableScrollBar(bool bEnable)
{
   if( m_bAllowScrollbars == bEnable ) return;
   m_iScrollPos = 0;
   m_bAllowScrollbars = bEnable;
}

int CUIContainer::FindSelectable(int iIndex, bool bForward /*= true*/) const
{
   // NOTE: This is actually a helper-function for the list/combo/ect controls
   //       that allow them to find the next enabled/available selectable item
   if( GetCount() == 0 ) return -1;
   iIndex = CLAMP(iIndex, 0, GetCount() - 1);
   if( bForward ) {
      for( int i = iIndex; i < GetCount(); i++ ) {
         if( GetItem(i)->GetInterface(_T("ListItem")) != NULL 
             && GetItem(i)->IsVisible()
             && GetItem(i)->IsEnabled() ) return i;
      }
      return -1;
   }
   else {
      for( int i = iIndex; i >= 0; --i ) {
         if( GetItem(i)->GetInterface(_T("ListItem")) != NULL 
             && GetItem(i)->IsVisible()
             && GetItem(i)->IsEnabled() ) return i;
      }
      return FindSelectable(0, true);
   }
}

void CUIContainer::SetPos(RECT rc)
{
   CUIControl::SetPos(rc);
   if( m_items.IsEmpty() ) return;
   rc.left += m_rcInset.left;
   rc.top += m_rcInset.top;
   rc.right -= m_rcInset.right;
   rc.bottom -= m_rcInset.bottom;
   // We'll position the first child in the entire client area.
   // Do not leave a container empty.
   ASSERT(m_items.GetSize()==1);
   static_cast<CUIControl*>(m_items[0])->SetPos(rc);
}

SIZE CUIContainer::EstimateSize(SIZE /*szAvailable*/)
{
   return m_cxyFixed;
}

void CUIContainer::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   if( _tcscmp(pstrName, _T("inset")) == 0 ) SetInset(CUISize(_ttoi(pstrValue), _ttoi(pstrValue)));
   else if( _tcscmp(pstrName, _T("padding")) == 0 ) SetPadding(_ttoi(pstrValue));
   else if( _tcscmp(pstrName, _T("width")) == 0 ) SetWidth(_ttoi(pstrValue));
   else if( _tcscmp(pstrName, _T("height")) == 0 ) SetHeight(_ttoi(pstrValue));
   else if( _tcscmp(pstrName, _T("scrollbar")) == 0 ) EnableScrollBar(_tcscmp(pstrValue, _T("true")) == 0);
   else CUIControl::SetAttribute(pstrName, pstrValue);
}

void CUIContainer::SetManager(CUIPaintManager* pManager, CUIControl* pParent)
{
   for( int it = 0; it < m_items.GetSize(); it++ ) {
      static_cast<CUIControl*>(m_items[it])->SetManager(pManager, this);
   }
   CUIControl::SetManager(pManager, pParent);
}

CUIControl* CUIContainer::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
   // Check if this guy is valid
   if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
   if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
   if( (uFlags & UIFIND_HITTEST) != 0 && !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData))) ) return NULL;
   if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
      CUIControl* pControl = CUIControl::FindControl(Proc, pData, uFlags);
      if( pControl != NULL ) return pControl;
   }
   for( int it = 0; it != m_items.GetSize(); it++ ) {
      CUIControl* pControl = static_cast<CUIControl*>(m_items[it])->FindControl(Proc, pData, uFlags);
      if( pControl != NULL ) return pControl;
   }
   return CUIControl::FindControl(Proc, pData, uFlags);
}

void CUIContainer::DoPaint(HDC hDC, const RECT& rcPaint)
{
   RECT rcTemp = { 0 };
   if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

   CRenderClip clip;
   CUIBlueRenderEngine::GenerateClip(hDC, m_rcItem, clip);

   if (m_pVScroll)
   {
	   if(m_pVScroll->IsVisible())
	   {
		   m_pVScroll->DoPaint(hDC, rcPaint);
	   }
   }
  

   for( int it = 0; it < m_items.GetSize(); it++ )
   {
      CUIControl* pControl = static_cast<CUIControl*>(m_items[it]);
      if( !pControl->IsVisible() ) continue;
      if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
      if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
      pControl->DoPaint(hDC, rcPaint);
   }
}

void CUIContainer::ProcessScrollbar(RECT rc, int cyRequired, int cyLine)
{
   // Need the scrollbar control, but it's been created already?
   if( cyRequired > rc.bottom - rc.top && m_pVScroll == NULL && m_bAllowScrollbars )
   {
	   m_pVScroll = new CUIVScrollBar;

	  Add(m_pVScroll);

	  if (m_pImageVScroll)
	  {
			m_pVScroll->SetImage(m_pImageVScroll);
			m_pVScroll->SetScrollBtnSize(m_iScrollBtnWidth, m_iScrollBtnHeight);
			m_pVScroll->SetBtnUpPos(m_rcImageBtnUp);
			m_pVScroll->SetBtnDownPos(m_rcImageBtnDown);
			m_pVScroll->SetPageUpPos(m_rcImagePageUp);
			m_pVScroll->SetPageDownPos(m_rcImagePageDown);

			m_pVScroll->SetSliderPos(m_rcImageSlider);
			m_pVScroll->SetSlider_MPos(m_rcImageSlider_M);
			m_pVScroll->SetSlider_MTPos(m_rcImageSlider_MT);


	  }
	  else
	  {
		  m_pVScroll->SetImage(CUIPaintManager::m_pImageSkinVScroll);
	  }
	  
      SetPos(m_rcItem);
      return;
   }
   // No scrollbar required
   if( m_pVScroll == NULL ) return;

   // Move it into place
   int cxScroll = m_pVScroll->GetScrollBtnSize().cx;

   m_pVScroll->SetPos(CUIRect(rc.right, rc.top, rc.right + cxScroll, rc.bottom));

   // Scroll not needed anymore?
   int cyScroll = cyRequired;
   if( cyScroll < 0 )
   {
      if( m_iScrollPos != 0 ) SetScrollPos(0);
      cyScroll = 0;
   }
   // Scroll range changed?
   int cyOld1, cyOld2;

   m_pVScroll->GetRange(cyOld1, cyOld2);
   if( cyOld2 != cyScroll ) {
      m_pVScroll->SetRange(0, cyScroll);

	  m_pVScroll->SetPageSize(rc.bottom - rc.top);
	  m_pVScroll->SetLineSize(cyLine);
   }
   int liBottomSpace=m_pVScroll->GetPageSize()-(cyScroll-m_iScrollPos);
   if(liBottomSpace > 0)
   {
	   if(m_iScrollPos - liBottomSpace >= 0)
	   {
		   int liNewScrollPos=m_iScrollPos-liBottomSpace;
		   m_pVScroll->SetCurrentPos(liNewScrollPos);
		   SetScrollPos(liNewScrollPos);
	   }
   }
   if(cyScroll <= GetScrollPage())
   {
	   Remove(m_pVScroll);
	   m_pVScroll=NULL;
	   SetPos(m_rcItem);
	   return;
   }
}

void CUIContainer::SetVScrollImage(Image*	pImage)
{
	if (pImage)
	{
		m_pImageVScroll = pImage->Clone();
	}
}

void CUIContainer::SetVScrollImage(UINT nResourceID)
{
	CUIPaintManager::SetImage(&m_pImageVScroll, nResourceID);
}

void CUIContainer::SetVScrollImage(LPCTSTR pstrName)
{
	m_pImageVScroll = Image::FromFile(pstrName);
}

void CUIContainer::SetVScrollBtnUpPos(const RECT& rc)
{
	m_rcImageBtnUp = rc;
}
void CUIContainer::SetVScrollBtnDownPos(const RECT& rc)
{
	m_rcImageBtnDown = rc;
}
void CUIContainer::SetVScrollPageUpPos(const RECT& rc)
{
	m_rcImagePageUp = rc;
}
void CUIContainer::SetVScrollPageDownPos(const RECT& rc)
{
	m_rcImagePageDown = rc;
}

void CUIContainer::SetVScrollSliderPos(const RECT& rc)
{
	m_rcImageSlider = rc;
}
void CUIContainer::SetVScrollSlider_MPos(const RECT& rc)
{
	m_rcImageSlider_M = rc;
}
void CUIContainer::SetVScrollSlider_MTPos(const RECT& rc)
{
	m_rcImageSlider_MT = rc;
}

void CUIContainer::SetVScrollBtnSize( int nWidth, int nHeight )
{
	m_iScrollBtnWidth=nWidth;
	m_iScrollBtnHeight=nHeight;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CCanvasUI::CCanvasUI() : m_hBitmap(NULL), m_iOrientation(HTBOTTOMRIGHT)
{
}

CCanvasUI::~CCanvasUI()
{
   if( m_hBitmap != NULL ) ::DeleteObject(m_hBitmap);
}

LPCTSTR CCanvasUI::GetClass() const
{
   return _T("CanvasUI");
}

bool CCanvasUI::SetWatermark(UINT iBitmapRes, int iOrientation)
{
   return SetWatermark(MAKEINTRESOURCE(iBitmapRes), iOrientation);
}

bool CCanvasUI::SetWatermark(LPCTSTR pstrBitmap, int iOrientation)
{
   if( m_hBitmap != NULL ) ::DeleteObject(m_hBitmap);
   m_hBitmap = (HBITMAP) ::LoadImage(m_pManager->GetResourceInstance(), pstrBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
   ASSERT(m_hBitmap!=NULL);
   if( m_hBitmap == NULL ) return false;
   ::GetObject(m_hBitmap, sizeof(BITMAP), &m_BitmapInfo);
   m_iOrientation = iOrientation;
   Invalidate();
   return true;
}

void CCanvasUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Fill background
   //RECT rcFill = { 0 };
   //if( ::IntersectRect(&rcFill, &rcPaint, &m_rcItem) ) {
	  //CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcFill, m_clrBack);
	  //CUIBlueRenderEngine::DoPaintBorder(hDC, m_pManager, m_rcItem, UICOLOR_CONTROL_BORDER_NORMAL);
 
   //}
   CUIContainer::DoPaint(hDC, rcPaint);
}

void CCanvasUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   if( _tcscmp(pstrName, _T("watermark")) == 0 ) SetWatermark(pstrValue);
   else CUIContainer::SetAttribute(pstrName, pstrValue);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CWindowCanvasUI::CWindowCanvasUI()
{
   SetInset(CUISize(10, 10));
   m_clrBack = m_pManager->GetThemeColor(UICOLOR_WINDOW_BACKGROUND);
}

LPCTSTR CWindowCanvasUI::GetClass() const
{
   return _T("WindowCanvasUI");
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CControlCanvasUI::CControlCanvasUI()
{
   SetInset(CUISize(2, 2));
   m_clrBack = m_pManager->GetThemeColor(UICOLOR_DIALOG_BACKGROUND);
}

LPCTSTR CControlCanvasUI::GetClass() const
{
   return _T("ControlCanvasUI");
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CWhiteCanvasUI::CWhiteCanvasUI()
{
   SetInset(CUISize(0, 0));
   m_clrBack = m_pManager->GetThemeColor(UICOLOR__INVALID);
}

LPCTSTR CWhiteCanvasUI::GetClass() const
{
   return _T("WhiteCanvasUI");
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CDialogCanvasUI::CDialogCanvasUI()
{
   SetInset(CUISize(10, 10));
   m_clrBack = m_pManager->GetThemeColor(UICOLOR_DIALOG_BACKGROUND);
}

LPCTSTR CDialogCanvasUI::GetClass() const
{
   return _T("DialogCanvasUI");
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CVerticalLayoutUI::CVerticalLayoutUI() : m_cyNeeded(0)
{
}

LPCTSTR CVerticalLayoutUI::GetClass() const
{
   return _T("VertialLayoutUI");
}

void CVerticalLayoutUI::SetPos(RECT rc)
{
   m_rcItem = rc;
   // Adjust for inset
   rc.left += m_rcInset.left; // wangpf
   rc.top += m_rcInset.top;
   rc.right -= m_rcInset.right; //wangpf
   rc.bottom -= m_rcInset.bottom;
   if( m_pVScroll != NULL ) 
   {
	   rc.right -= m_pVScroll->GetScrollBtnSize().cx + 1;
   }
   // Determine the minimum size
   SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
   int nAdjustables = 0;
   int cyFixed = 0;
   for( int it1 = 0; it1 < m_items.GetSize(); it1++ )
   {
      CUIControl* pControl = static_cast<CUIControl*>(m_items[it1]);
      if( !pControl->IsVisible() ) continue;

	  CUIVScrollBar* pScroll = dynamic_cast<CUIVScrollBar*>(pControl);
	  if (pScroll)
	  {
		  continue;
	  }
	
	  LPCTSTR lp=pControl->GetClass();
      SIZE sz = pControl->EstimateSize(szAvailable);
      if( sz.cy == 0 ) nAdjustables++;
      cyFixed += sz.cy + m_iPadding;
   }
   // Place elements
   int cyNeeded = 0;
   int cyExpand = 0;
   int cyLine = rc.bottom - rc.top;
   if( nAdjustables > 0 ) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
   // Position the elements
   SIZE szRemaining = szAvailable;
   int iPosY = rc.top - m_iScrollPos;
   int iAdjustable = 0;
   for( int it2 = 0; it2 < m_items.GetSize(); it2++ )
   {
      CUIControl* pControl = static_cast<CUIControl*>(m_items[it2]);
      if( !pControl->IsVisible() ) continue;

	  CUIVScrollBar* pScroll = dynamic_cast<CUIVScrollBar*>(pControl);
	  if (pScroll)
	  {
		  continue;
	  }

      SIZE sz = pControl->EstimateSize(szRemaining);
      if( sz.cy == 0 )
	  {
         iAdjustable++;
         sz.cy = cyExpand;
         // Distribute remaining to last element (usually round-off left-overs)
         if( iAdjustable == nAdjustables ) sz.cy += MAX(0, szAvailable.cy - (cyExpand * nAdjustables) - cyFixed);
      }
      RECT rcCtrl = { rc.left, iPosY, rc.right, iPosY + sz.cy };
      pControl->SetPos(rcCtrl);
      iPosY += sz.cy + m_iPadding;
      cyNeeded += sz.cy + m_iPadding;
      szRemaining.cy -= sz.cy + m_iPadding;

	  cyLine = MIN(cyLine, sz.cy);
   }
   // Handle overflow with scrollbars
   ProcessScrollbar(rc, cyNeeded, cyLine);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CHorizontalLayoutUI::CHorizontalLayoutUI()
{
}

LPCTSTR CHorizontalLayoutUI::GetClass() const
{
   return _T("HorizontalLayoutUI");
}

void CHorizontalLayoutUI::SetPos(RECT rc)
{
   m_rcItem = rc;
   // Adjust for inset
   rc.left += m_rcInset.left;
   rc.top += m_rcInset.top;
   rc.right -= m_rcInset.right;
   rc.bottom -= m_rcInset.bottom;
   // Determine the width of elements that are sizeable
   SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
   int nAdjustables = 0;
   int cxFixed = 0;
   for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
      CUIControl* pControl = static_cast<CUIControl*>(m_items[it1]);
      if( !pControl->IsVisible() ) continue;
      SIZE sz = pControl->EstimateSize(szAvailable);
      if( sz.cx == 0 ) nAdjustables++;
      cxFixed += sz.cx + m_iPadding;
   }
   int cxExpand = 0;
   if( nAdjustables > 0 ) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
   // Position the elements
   SIZE szRemaining = szAvailable;
   int iPosX = rc.left;
   int iAdjustable = 0;
   for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
      CUIControl* pControl = static_cast<CUIControl*>(m_items[it2]);
      if( !pControl->IsVisible() ) continue;
      SIZE sz = pControl->EstimateSize(szRemaining);
      if( sz.cx == 0 ) {
         iAdjustable++;
         sz.cx = cxExpand;
         if( iAdjustable == nAdjustables ) sz.cx += MAX(0, szAvailable.cx - (cxExpand * nAdjustables) - cxFixed);
      }
      RECT rcCtrl = { iPosX, rc.top, iPosX + sz.cx, rc.bottom };
      pControl->SetPos(rcCtrl);
      iPosX += sz.cx + m_iPadding;
      szRemaining.cx -= sz.cx + m_iPadding;
   }
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CTileLayoutUI::CTileLayoutUI() : m_nColumns(2), m_cyNeeded(0)
{
   SetPadding(10);
   SetInset(CUISize(10, 10));
}

LPCTSTR CTileLayoutUI::GetClass() const
{
   return _T("TileLayoutUI");
}

void CTileLayoutUI::SetColumns(int nCols)
{
   if( nCols <= 0 ) return;
   m_nColumns = nCols;
   UpdateLayout();
}

void CTileLayoutUI::SetPos(RECT rc)
{
   m_rcItem = rc;
   // Adjust for inset
   rc.left += m_rcInset.left;
   rc.top += m_rcInset.top;
   rc.right -= m_rcInset.right;
   rc.bottom -= m_rcInset.bottom;

   if( m_pVScroll != NULL ) 
   {
	   rc.right -= m_pVScroll->GetScrollBtnSize().cx;
   }
   // Position the elements
   int cxWidth = (rc.right - rc.left) / m_nColumns;
   int cyHeight = 0;
   int iCount = 0;
   POINT ptTile = { rc.left, rc.top - m_iScrollPos };
   for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
      CUIControl* pControl = static_cast<CUIControl*>(m_items[it1]);
      if( !pControl->IsVisible() ) continue;
      // Determine size
      RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
      // Adjust with element padding
      if( (iCount % m_nColumns) == 0 ) rcTile.right -= m_iPadding / 2;
      else if( (iCount % m_nColumns) == m_nColumns - 1 ) rcTile.left += m_iPadding / 2;
      else ::InflateRect(&rcTile, -(m_iPadding / 2), 0);
      // If this panel expands vertically
      if( m_cxyFixed.cy == 0) {
         SIZE szAvailable = { rcTile.right - rcTile.left, 9999 };
         int iIndex = iCount;
         for( int it2 = it1; it2 < m_items.GetSize(); it2++ ) {
            SIZE szTile = static_cast<CUIControl*>(m_items[it2])->EstimateSize(szAvailable);
            cyHeight = MAX(cyHeight, szTile.cy);
            if( (++iIndex % m_nColumns) == 0) break;
         }
      }
      // Set position
      rcTile.bottom = rcTile.top + cyHeight;
      pControl->SetPos(rcTile);
      // Move along...
      if( (++iCount % m_nColumns) == 0 ) {
         ptTile.x = rc.left;
         ptTile.y += cyHeight + m_iPadding;
         cyHeight = 0;
      }
      else {
         ptTile.x += cxWidth;
      }
      m_cyNeeded = rcTile.bottom - (rc.top - m_iScrollPos);
   }
   // Process the scrollbar
   ProcessScrollbar(rc, m_cyNeeded, 20);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CDialogLayoutUI::CDialogLayoutUI() : m_bFirstResize(true), m_aModes(sizeof(STRETCHMODE))
{
   ::ZeroMemory(&m_rcDialog, sizeof(m_rcDialog));

   m_clrBack = INVALID__UICOLOR;
}

LPCTSTR CDialogLayoutUI::GetClass() const
{
   return _T("DialogLayoutUI");
}

LPVOID CDialogLayoutUI::GetInterface(LPCTSTR pstrName)
{
   if( _tcscmp(pstrName, _T("DialogLayout")) == 0 ) return this;
   return CUIContainer::GetInterface(pstrName);
}

void CDialogLayoutUI::SetStretchMode(CUIControl* pControl, UINT uMode)
{
   STRETCHMODE mode;
   mode.pControl = pControl;
   mode.uMode = uMode;
   mode.rcItem = pControl->GetPos();
   m_aModes.Add(&mode);
}

SIZE CDialogLayoutUI::EstimateSize(SIZE szAvailable)
{
   RecalcArea();
   return CUISize(m_rcDialog.right - m_rcDialog.left, m_rcDialog.bottom - m_rcDialog.top);
}

void CDialogLayoutUI::SetPos(RECT rc)
{
   m_rcItem = rc;
   RecalcArea();
   // Do Scrollbar
   ProcessScrollbar(rc, m_rcDialog.bottom - m_rcDialog.top, 20);

   if( m_pVScroll != NULL ) 
   {
	   rc.right -= m_pVScroll->GetScrollBtnSize().cx;
   }

   // Determine how "scaled" the dialog is compared to the original size
   int cxDiff = (rc.right - rc.left) - (m_rcDialog.right - m_rcDialog.left);
   int cyDiff = (rc.bottom - rc.top) - (m_rcDialog.bottom - m_rcDialog.top);
   if( cxDiff < 0 ) cxDiff = 0;
   if( cyDiff < 0 ) cyDiff = 0;
   // Stretch each control
   // Controls are coupled in "groups", which determine a scaling factor.
   // A "line" indicator is used to apply the same scaling to a new group of controls.
   int nCount, cxStretch, cyStretch, cxMove, cyMove;
   for( int i = 0; i < m_aModes.GetSize(); i++ ) {
      STRETCHMODE* pItem = static_cast<STRETCHMODE*>(m_aModes[i]);
      if( i == 0 || (pItem->uMode & UISTRETCH_NEWGROUP) != 0 ) {
         nCount = 0;
         for( int j = i + 1; j < m_aModes.GetSize(); j++ ) {
            STRETCHMODE* pNext = static_cast<STRETCHMODE*>(m_aModes[j]);
            if( (pNext->uMode & (UISTRETCH_NEWGROUP | UISTRETCH_NEWLINE)) != 0 ) break;
            if( (pNext->uMode & (UISTRETCH_SIZE_X | UISTRETCH_SIZE_Y)) != 0 ) nCount++;
         }
         if( nCount == 0 ) nCount = 1;
         cxStretch = cxDiff / nCount;
         cyStretch = cyDiff / nCount;
         cxMove = 0;
         cyMove = 0;
      }
      if( (pItem->uMode & UISTRETCH_NEWLINE) != 0 ) {
         cxMove = 0;
         cyMove = 0;
      }
      RECT rcPos = pItem->rcItem;
      ::OffsetRect(&rcPos, rc.left, rc.top - m_iScrollPos);
      if( (pItem->uMode & UISTRETCH_MOVE_X) != 0 ) ::OffsetRect(&rcPos, cxMove, 0);
      if( (pItem->uMode & UISTRETCH_MOVE_Y) != 0 ) ::OffsetRect(&rcPos, 0, cyMove);
      if( (pItem->uMode & UISTRETCH_SIZE_X) != 0 ) rcPos.right += cxStretch;
      if( (pItem->uMode & UISTRETCH_SIZE_Y) != 0 ) rcPos.bottom += cyStretch;
      if( (pItem->uMode & (UISTRETCH_SIZE_X | UISTRETCH_SIZE_Y)) != 0 ) {
         cxMove += cxStretch;
         cyMove += cyStretch;
      }      
      pItem->pControl->SetPos(rcPos);
   }
}

void CDialogLayoutUI::RecalcArea()
{
   if( !m_bFirstResize ) return;
   // Add the remaining control to the list
   // Controls that have specific stretching needs will define them in the XML resource
   // and by calling SetStretchMode(). Other controls needs to be added as well now...
   for( int it = 0; it < m_items.GetSize(); it++ ) {         
      CUIControl* pControl = static_cast<CUIControl*>(m_items[it]);
      bool bFound = false;
      for( int i = 0; !bFound && i < m_aModes.GetSize(); i++ ) {
         if( static_cast<STRETCHMODE*>(m_aModes[i])->pControl == pControl ) bFound = true;
      }
      if( !bFound ) {
         STRETCHMODE mode;
         mode.pControl = pControl;
         mode.uMode = UISTRETCH_NEWGROUP;
         mode.rcItem = pControl->GetPos();
         m_aModes.Add(&mode);
      }
   }
   // Figure out the actual size of the dialog so we can add proper scrollbars later
   CUIRect rcDialog(9999, 9999, 0,0);
   for( int i = 0; i < m_items.GetSize(); i++ ) {
      const RECT& rcPos = static_cast<CUIControl*>(m_items[i])->GetPos();
      rcDialog.Join(rcPos);
   }
   for( int j = 0; j < m_aModes.GetSize(); j++ ) {
      RECT& rcPos = static_cast<STRETCHMODE*>(m_aModes[j])->rcItem;
      ::OffsetRect(&rcPos, -rcDialog.left, -rcDialog.top);
   }
   m_rcDialog = rcDialog;
   // We're done with initialization
   m_bFirstResize = false;
}

void CDialogLayoutUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	// Fill background

	Image *pImage =  m_pManager ? m_pManager->m_pImageSkinNormal : NULL;

	RECT rcFill = { 0 };
	if( ::IntersectRect(&rcFill, &rcPaint, &m_rcItem) ) {

		if (m_clrBack != INVALID__UICOLOR)
		{
			CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcFill, m_clrBack);
		}
		else
		{
			LONG nStyle =  GetWindowLong(m_pManager->GetPaintWindow(), GWL_STYLE) & WS_CHILD;
			if ((pImage == NULL) && nStyle != 0)
			{
				HWND hParent = ::GetParent(m_pManager->GetPaintWindow());

				if (::IsWindow(hParent))
				{
					RECT rect;
					GetWindowRect(m_pManager->GetPaintWindow(), &rect);

					POINT pt = {rect.left, rect.top};

					ScreenToClient(hParent, &pt);
					HDC dcParent = GetDC(hParent);

					::BitBlt(hDC, 0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, dcParent, pt.x, pt.y, SRCCOPY);
				}
			}
		}
	}

	Graphics graph(hDC);

	if (pImage)
	{
		int width = pImage->GetWidth();
		int height = pImage->GetHeight();


		if (width == m_rcItem.right - m_rcItem.left && height == m_rcItem.bottom - m_rcItem.top)
		{
			RectF rfDest(PointF((REAL)0, (REAL)0), SizeF((REAL)width, (REAL)height));
			DWORD dwTime = ::GetTickCount();

			graph.DrawImage(pImage, rfDest, (REAL)0, (REAL)0, (REAL)width, (REAL)height, UnitPixel, NULL);

			dwTime = ::GetTickCount() - dwTime;
		}
		else
		{
			RECT table = {0, 0, width , height};
			CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, table, pImage);
		}

		

		//TRACE(_T("the Background Draw time is %d\n\n"), dwTime);
	}
	else
	{
		LONG nStyle =  GetWindowLong(m_pManager->GetPaintWindow(), GWL_STYLE) & WS_CHILD;
		if(m_pManager != NULL && nStyle == 0)
		{
			if (m_pManager->m_pImageSkinCommonDialog)
			{
				RECT table = {0, 0, m_pManager->m_pImageSkinCommonDialog->GetWidth() , m_pManager->m_pImageSkinCommonDialog->GetHeight()};
				CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, table, m_pManager->m_pImageSkinCommonDialog);
			}
		}

	}
	

	CUIContainer::DoPaint(hDC, rcPaint);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CMenuCanvasUI::CMenuCanvasUI()
{
	SetInset(CUISize(3, 3));
	m_clrBack = m_pManager->GetThemeColor(UICOLOR_DIALOG_BACKGROUND);
}

LPCTSTR CMenuCanvasUI::GetClass() const
{
	return _T("MenuCanvasUI");
}

void CMenuCanvasUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	Image* pImage = m_pManager ? m_pManager->m_pImageSkinMenu : NULL;
	if (pImage)
	{
		int nWidth = pImage->GetWidth();
		int nHeight = (pImage->GetHeight() - 8) / 3;

		RECT table = {0, nHeight * 0, nWidth , nHeight * 1};

		CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, table, pImage);
		
	}
	
	CUIContainer::DoPaint(hDC, rcPaint);
	
}

/////////////////////////////////////////////////////////////////////////////////////
//////
CComboCanvasUI::CComboCanvasUI()
{
	SetInset(CUISize(1, 3));
	m_clrBack = m_pManager->GetThemeColor(UICOLOR_DIALOG_BACKGROUND);
}

LPCTSTR CComboCanvasUI::GetClass() const
{
	return _T("ComboCanvasUI");
}

void CComboCanvasUI::DoPaint(HDC hDC, const RECT& rcPaint)
{

	Image* pImage =  m_pManager ? m_pManager->m_pImageSkinCombo : NULL;
	if (pImage == NULL)
	{
		CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, m_clrBack);
	}
	else
	{
		int nWidth = pImage->GetWidth() / 4;
		int nHeight = pImage->GetHeight();
		int nIdx = 0;

		RECT table = {nWidth * nIdx, 0, nWidth * (nIdx + 1) , nHeight};
		CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, table, pImage);
	}


	CUIContainer::DoPaint(hDC, rcPaint);

}

////////////////////////////////////////////////////////////////////////////////////////////////
//////
CUIGroup::CUIGroup():m_uTextStyle(DT_SINGLELINE | DT_VCENTER)
{
	m_pImageOwner = NULL;
	SetInset(CUISize(3, 3));
	m_clrBack = m_pManager->GetThemeColor(UICOLOR_DIALOG_BACKGROUND);

	m_colorTextBk = INVALID__UICOLOR;
}

CUIGroup::~CUIGroup()
{
	if (m_pImageOwner)
	{
		delete m_pImageOwner;
		m_pImageOwner = NULL;
	}
}

LPCTSTR CUIGroup::GetClass() const
{
	return _T("GroupUI");
}

void CUIGroup::DoPaint(HDC hDC, const RECT& rcPaint)
{
	int nTextHeight = m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight + 10;

	if (m_pImageOwner == NULL)
	{
		CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, m_clrBack);
	}
	else
	{
		int nWidth = m_pImageOwner->GetWidth();
		int nHeight = m_pImageOwner->GetHeight();
		int nIdx = 0;

		RECT rcImage = m_rcItem;

		RECT table = {nWidth * nIdx, 0, nWidth * (nIdx + 1) , nHeight};

		if(m_sText.GetLength() > 0)
		{
			rcImage.top = rcImage.top + nTextHeight / 2;
		}
		CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcImage, table, m_pImageOwner);
	}


	if (m_sText.GetLength() > 0)
	{
		RECT rcTitle = { 0, 0, 9999, 100 };
		int nLinks = 0;
		CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcTitle, m_sText, UICOLOR_CONTROL_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT | m_uTextStyle);


		int nTextWidth = rcTitle.right - rcTitle.left;

		RECT rcText = m_rcItem;

		rcText.bottom = rcText.top + nTextHeight;
		rcText.left = m_rcItem.left + 10;
		

		if ((m_uTextStyle & DT_CENTER) != 0)
		{
			rcText.left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - nTextWidth)/ 2;
		}
		else if ((m_uTextStyle & DT_RIGHT) != 0)
		{
			rcText.left = m_rcItem.right - nTextWidth - 10;
		}

		rcText.right = rcText.left + nTextWidth;

		if (m_colorTextBk == INVALID__UICOLOR)
		{
			Image *pImage =  m_pManager ? m_pManager->m_pImageSkinNormal : NULL;

			Graphics graph(hDC);

			if (pImage)
			{
				RectF rfDest(PointF((REAL)rcText.left, (REAL)rcText.top), SizeF((REAL)(rcText.right - rcText.left), (REAL)(rcText.bottom - rcText.top)));
				graph.DrawImage(pImage, rfDest, (REAL)rcText.left, (REAL)(REAL)rcText.top, (REAL)(rcText.right - rcText.left), (REAL)(rcText.bottom - rcText.top), UnitPixel, NULL);
			}
			else
			{
				if (m_pManager != NULL && m_pManager->m_pImageSkinCommonDialog)
				{
					pImage = m_pManager->m_pImageSkinCommonDialog;
					int width = pImage->GetWidth();
					int height = pImage->GetHeight();

					RectF rfDest(PointF((REAL)rcText.left, (REAL)rcText.top), SizeF((REAL)(rcText.right - rcText.left), (REAL)(rcText.bottom - rcText.top)));
					graph.DrawImage(pImage, rfDest, (REAL)width / 3, (REAL)(REAL)height / 3, (REAL)(rcText.right - rcText.left), (REAL)(rcText.bottom - rcText.top), UnitPixel, NULL);
				}
			}
		}
		else
		{
			CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcText, m_colorTextBk);
		}


		CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcText, m_sText, RGB(255, 255, 255), INVALID__UICOLOR, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
	}

	CUIContainer::DoPaint(hDC, rcPaint);

}

void CUIGroup::SetImage(Image*	pImage)
{
	if (pImage)
	{
		m_pImageOwner = pImage->Clone();
	}
	Invalidate();
}

void CUIGroup::SetImage(UINT nResourceID)
{
	CUIPaintManager::SetImage(&m_pImageOwner, nResourceID);
	Invalidate();
}

void CUIGroup::SetImage(LPCTSTR pstrName)
{
	m_pImageOwner = Image::FromFile(pstrName);
	Invalidate();
}

void CUIGroup::SetTextStyle(UINT uStyle)
{
	if ((uStyle & DT_CENTER) != 0 || (uStyle & DT_RIGHT) != 0)
	{
		uStyle |= DT_SINGLELINE | DT_VCENTER;
	}

	m_uTextStyle = uStyle;
	Invalidate();
}

void CUIGroup::SetTextBKColor(COLORREF color)
{
	m_colorTextBk = color;
	Invalidate();
}



