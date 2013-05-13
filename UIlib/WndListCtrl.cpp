// ImageListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "WndListCtrl.h"
#include <algorithm>
int GetMaxHeight(Image* p1,Image* p2,Image* p3)
{
	int max=0;
	if (p1)
	{
		max=p1->GetWidth();
	}
	if (p2)
	{
		int temp = p2->GetWidth();
		if (temp>max)
		{
			max=temp;
		}
	}
	if (p3)
	{
		int temp = p3->GetWidth();
		if (temp>max)
		{
			max=temp;
		}
	}
	return max;
}
typedef struct 
{
	int x, y;
	int width, height;
} CustomDrawTable;

BOOL fCustomDraw = TRUE;

LRESULT CWndListCtrl::HandleCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm)
{
	CustomDrawTable cdt_vert_normal[] = 
	{
		{ m_nScrollWidth * 0, m_nScrollHeight * 0,		m_nScrollWidth, m_nScrollHeight }, //up arrow   NORMAL
		{ m_nScrollWidth * 0, m_nScrollHeight * 1,		m_nScrollWidth, m_nScrollHeight }, //down arrow NORMAL
		{ m_nScrollWidth * 0, m_nScrollHeight * 2,		m_nScrollWidth, m_nScrollHeight }, //page up	 NORMAL
		{ m_nScrollWidth * 0, m_nScrollHeight * 2,		m_nScrollWidth, m_nScrollHeight }, //page down  NORMAL

		{ 1, 1, 1, 1 },	//padding

		{ m_nScrollWidth * 0, m_nScrollHeight * 3,		m_nScrollWidth, 2 }, //vert thumb (up)
		{ m_nScrollWidth * 0, m_nScrollHeight * 5,		m_nScrollWidth, m_nScrollHeight }, //vert thumb (middle)
		{ m_nScrollWidth * 0, m_nScrollHeight * 4 - 2,	m_nScrollWidth, 2 }, //vert thumb (down)

		{ m_nScrollWidth * 0, m_nScrollHeight * 4,		m_nScrollWidth, m_nScrollHeight }, //vert thumb (middle 横线处)
	};

	CustomDrawTable cdt_vert_hot[] = 
	{
		{ m_nScrollWidth * 1, m_nScrollHeight * 0,		m_nScrollWidth, m_nScrollHeight }, //up arrow   NORMAL
		{ m_nScrollWidth * 1, m_nScrollHeight * 1,		m_nScrollWidth, m_nScrollHeight }, //down arrow NORMAL
		{ m_nScrollWidth * 1, m_nScrollHeight * 2,		m_nScrollWidth, m_nScrollHeight }, //page up	 NORMAL
		{ m_nScrollWidth * 1, m_nScrollHeight * 2,		m_nScrollWidth, m_nScrollHeight }, //page down  NORMAL

		{ -1, -1, -1, -1 },	//padding

		{ m_nScrollWidth * 1, m_nScrollHeight * 3,		m_nScrollWidth, 2 }, //vert thumb (up)
		{ m_nScrollWidth * 1, m_nScrollHeight * 5,		m_nScrollWidth, m_nScrollHeight }, //vert thumb (middle)
		{ m_nScrollWidth * 1, m_nScrollHeight * 4 - 2,	m_nScrollWidth, 2 }, //vert thumb (down)

		{ m_nScrollWidth * 1, m_nScrollHeight * 4,		m_nScrollWidth, m_nScrollHeight }, //vert thumb (middle 横线处)
	};

	CustomDrawTable cdt_vert_active[] = 
	{
		{ m_nScrollWidth * 2, m_nScrollHeight * 0,		m_nScrollWidth, m_nScrollHeight }, //up arrow   NORMAL
		{ m_nScrollWidth * 2, m_nScrollHeight * 1,		m_nScrollWidth, m_nScrollHeight }, //down arrow NORMAL
		{ m_nScrollWidth * 2, m_nScrollHeight * 2,		m_nScrollWidth, m_nScrollHeight }, //page up	 NORMAL
		{ m_nScrollWidth * 2, m_nScrollHeight * 2,		m_nScrollWidth, m_nScrollHeight }, //page down  NORMAL

		{ -1, -1, -1, -1 },	//padding

		{ m_nScrollWidth * 2, m_nScrollHeight * 3,		m_nScrollWidth, 2 }, //vert thumb (up)
		{ m_nScrollWidth * 2, m_nScrollHeight * 5,		m_nScrollWidth, m_nScrollHeight }, //vert thumb (middle)
		{ m_nScrollWidth * 2, m_nScrollHeight * 4 - 2,	m_nScrollWidth, 2 }, //vert thumb (down)

		{ m_nScrollWidth * 2, m_nScrollHeight * 4,		m_nScrollWidth, m_nScrollHeight }, //vert thumb (middle 横线处)
	};


	RECT *rc;
	CustomDrawTable *cdt;
	UINT code = NM_CUSTOMDRAW;

	UNREFERENCED_PARAMETER(ctrlid);

	// inserted buttons do not use PREPAINT etc..
	if(nm->nBar == SB_INSBUT)
	{
		return CDRF_SKIPDEFAULT;
	}


	if(!fCustomDraw) return CDRF_DODEFAULT;

	if(nm->dwDrawStage == CDDS_PREPAINT)
	{
		if(fCustomDraw)
			return CDRF_SKIPDEFAULT;
		else
			return CDRF_DODEFAULT;
	}

	if(nm->dwDrawStage == CDDS_POSTPAINT)
	{

	}

	//the sizing gripper in the bottom-right corner
	if(nm->nBar == SB_BOTH)	
	{
		return CDRF_SKIPDEFAULT;
	}
	else if(nm->nBar == SB_HORZ)
	{

	}
	else if(nm->nBar == SB_VERT)
	{
		rc = &nm->rect;

		if(nm->uState == CDIS_HOT)		
			cdt = &cdt_vert_hot[nm->uItem];
		else if(nm->uState == CDIS_SELECTED)  
			cdt = &cdt_vert_active[nm->uItem];
		else				    
			cdt = &cdt_vert_normal[nm->uItem];

		if(nm->uItem == HTSCROLL_THUMB)
		{
			// top
			StretchBlt(nm->hdc, rc->left, rc->top,   rc->right-rc->left, 2, m_hdcSkin, cdt->x, cdt->y, cdt->width, cdt->height, SRCCOPY);
			
			// middle
			cdt++;
			StretchBlt(nm->hdc, rc->left, rc->top + 2, rc->right-rc->left, (rc->bottom-rc->top)-4, m_hdcSkin, cdt->x, cdt->y, cdt->width, cdt->height, SRCCOPY);
			// bottom
			cdt++;
			StretchBlt(nm->hdc, rc->left, rc->top+(rc->bottom-rc->top)-2, rc->right-rc->left, 2,m_hdcSkin, cdt->x, cdt->y, cdt->width, cdt->height, SRCCOPY);

			// 2 + x + 10 + x +2

			int nTop = (rc->bottom - rc->top - m_nScrollHeight) / 2;
			cdt++;
			StretchBlt(nm->hdc, rc->left, rc->top + nTop, rc->right-rc->left, m_nScrollHeight,m_hdcSkin, cdt->x, cdt->y, cdt->width, cdt->height, SRCCOPY);
			return CDRF_SKIPDEFAULT;
		}
	}
	//INSERTED BUTTONS are handled here...
	else if(nm->nBar == SB_INSBUT)
	{
		return CDRF_SKIPDEFAULT;
	}
	else
	{
		return CDRF_DODEFAULT;
	}

	//normal bitmaps, use same code for HORZ and VERT
	StretchBlt(nm->hdc, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top,
		m_hdcSkin, cdt->x, cdt->y, cdt->width, cdt->height, SRCCOPY);

	return CDRF_SKIPDEFAULT;

}


CWndListCtrl::CellInfo::CellInfo(int nColumn)
:m_clrText(0x00000000)
,m_nColumn(nColumn)
,m_nOffsetX(5)
,m_pImage1(NULL)
,m_pImage2(NULL)
,m_pImage3(NULL)
,m_nCurrLinkerIdx(0)
,m_nLinkerCount(8)
,m_bSingleLine(TRUE)
,m_bBtn(FALSE)
{
	memset(&m_rectLinker, 0, sizeof(RECT)* 8);
	memset(&m_rectBtn, 0, sizeof(RECT));
}

CWndListCtrl::ItemInfo::ItemInfo()
:m_clrBack(INVALID__UICOLOR)
,m_nItemHeight(20)
,m_nMinHeight(20)
,m_bSelected(FALSE)
,m_bChecked(FALSE)
,m_dwData(NULL)
,m_bCursorInSide(false)
{
	
}

CWndListCtrl::ColumnInfo::ColumnInfo()
:m_nColumnWidth(20)
,m_nFormat(0)
{
}

IMPLEMENT_DYNAMIC(CWndListCtrl, CWnd)
CWndListCtrl::CWndListCtrl()
{
	ClearParam();

	m_nLeftSpan = 0;
	m_nRightSpan = 0;
	m_nTopSpan = 0;
	m_nBottomSpan = 0;

	m_nTitleHeight = 0;
	m_colorTitleTop = RGB(134, 134, 134);
	m_colorTitleBottom = RGB(196, 196, 196);

	m_bMouseOnLinker = FALSE;

	m_nSelectedItem = -1;
	m_bCanSelected = FALSE;

	m_bRedraw = TRUE;

	m_nColCount = 1;

	m_nImagePos = ImagePos_Top;

	m_bShowScrollBar = FALSE;

	m_colorBackGround = GetSysColor(COLOR_WINDOW);
	m_colorItemSelectedBk = GetSysColor(COLOR_HIGHLIGHT);
	m_colorItemSelectedBorder = GetSysColor(COLOR_HIGHLIGHT);

	{
		m_rBlendProps.BlendOp = AC_SRC_OVER;
		m_rBlendProps.BlendFlags = 0;
		m_rBlendProps.AlphaFormat = 0;
		m_rBlendProps.SourceConstantAlpha = 0;
		m_rBlendProps.SourceConstantAlpha = 20;
	}

	m_hCursorHander = AfxGetApp()->LoadStandardCursor(IDC_HAND);

	m_pImageScrollBar = NULL;
	m_pImageCommonIcon = NULL;
	m_pImageCheckBox = NULL;

	m_pImageBackground = NULL;
	m_pImageSelectedBK = NULL;

	m_pImageItemSplit =NULL;

	m_nScrollWidth = 11;
	m_nScrollHeight = 10;

	m_bShowScrollBarAlway = FALSE;

	m_bResetItemHeight = TRUE;

	m_pImageButton = NULL;
	m_nBtnStatusCount = 4;

	m_colorImageBorderInside = INVALID__UICOLOR;
	m_colorImageBorderOutside = INVALID__UICOLOR;

	m_colorFrameInside = INVALID__UICOLOR;
	m_colorFrameOutside = INVALID__UICOLOR;

	m_colorGird = INVALID__UICOLOR;

	m_nItemCountPerRow = 1;

	m_nTooltipMaxWidth = 0;

	m_hwndTooltip = NULL;
	m_bMouseTracking = FALSE;

	m_bCurrentItemBkColorOpened = false;
	m_currentItemBkColor = RGB(255,255,255);

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup( &m_gdiplusToken, &gdiplusStartupInput, NULL );
}

CWndListCtrl::~CWndListCtrl()
{
	DeleteAllItems();

	if (m_pImageScrollBar)
	{
		delete m_pImageScrollBar;
		m_pImageScrollBar = NULL;
	}

	if (m_pImageCheckBox)
	{
		delete m_pImageCheckBox;
		m_pImageCheckBox = NULL;
	}

	if (m_pImageCommonIcon)
	{
		delete m_pImageCommonIcon;
		m_pImageCommonIcon = NULL;
	}

	if (m_pImageBackground)
	{
		delete m_pImageBackground;
		m_pImageBackground = NULL;
	}

	if (m_pImageSelectedBK)
	{
		delete m_pImageSelectedBK;
		m_pImageSelectedBK = NULL;
	}

	if (m_pImageButton)
	{
		delete m_pImageButton;
		m_pImageButton = NULL;
	}

	GdiplusShutdown(m_gdiplusToken);
}


BEGIN_MESSAGE_MAP(CWndListCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_NCCALCSIZE()
	ON_WM_ERASEBKGND()
	//ON_WM_CTLCOLOR_REFLECT()
	//ON_MESSAGE(WM_NCPAINT, OnNcPaint)
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_NCPAINT()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

int CWndListCtrl::InsertColumn(int nItem, LPCTSTR lpszItem, int nWidth, int nFormat)
{
	ColumnInfo info;
	info.nItem = nItem;
	info.m_strText = lpszItem;
	info.m_nColumnWidth = nWidth;
	info.m_nFormat = nFormat;
	m_Columns.push_back(info);

	m_nColCount = (int)m_Columns.size();

	return m_nColCount;
}

int CWndListCtrl::InsertItem(
							   int nItem,
							   LPCTSTR lpszItem,
							   Image* pImage1,
							   Image* pImage2,
							   Image* pImage3
							   )
{
	ItemInfo itemData;
	for (int i = 0; i < m_nColCount; i++)
	{
		CellInfo info(i);
		info.m_nColumn = i;
		itemData.m_aCellInfo.push_back(info);

	}
	ClearParam();
	
	itemData.m_nItem     = nItem;
	itemData.m_aCellInfo[0].m_strText = lpszItem;
	itemData.m_aCellInfo[0].m_pImage1 = pImage1;
	itemData.m_aCellInfo[0].m_pImage2 = pImage2;
	itemData.m_aCellInfo[0].m_pImage3 = pImage3;


	m_Items.push_back(itemData);


	if (m_bRedraw)
	{
		Draw();
		Invalidate(FALSE);
	}

	return itemData.m_nItem;
}

void CWndListCtrl::SetItemText(
								int nItem,
								int nSubItem,
								LPCTSTR lpszItem,
								Image* pImage1,
								Image* pImage2,
								Image* pImage3
								)
{
	 CellInfo* pCellInfo = GetCellInfo(nItem, nSubItem);
	 if (pCellInfo)
	 {
		 pCellInfo->m_strText = lpszItem;
		 pCellInfo->m_pImage1 = pImage1;
		 pCellInfo->m_pImage2 = pImage2;
		 pCellInfo->m_pImage3 = pImage3;
	 }

	 if (m_bRedraw)
	 {
		 Draw();
		 Invalidate(FALSE);
	 }
}

CString CWndListCtrl::GetItemText(int nItem, int nSubItem)
{
	CellInfo* pCellInfo = GetCellInfo(nItem, nSubItem);
	if (pCellInfo)
	{
		return pCellInfo->m_strText;
	}

	return _T("");
}

int	CWndListCtrl::GetColumnCount(void)
{
	return m_nColCount;
}

int CWndListCtrl::GetTopIndex(void)
{
	return m_ILParam.m_nItemAtTop;
}

int CWndListCtrl::GetItemCount(void)
{
	return m_Items.size();
}

int CWndListCtrl::GetCountPerPage(void)
{
	return m_ILParam.m_nRowCnt;
}

int CWndListCtrl::GetColumnWidth(int nIdx)
{
	if (nIdx < 0 || nIdx > (int)m_Columns.size() -1)
	{
		return NULL;
	}

	return m_Columns[nIdx].m_nColumnWidth;
}

int CWndListCtrl::GetColumnFormat(int nIdx)
{
	if (nIdx < 0 || nIdx > (int)m_Columns.size() -1)
	{
		return NULL;
	}

	return m_Columns[nIdx].m_nFormat;
}

CString CWndListCtrl::GetColumnText(int nIdx)
{
	if (nIdx < 0 || nIdx > (int)m_Columns.size() -1)
	{
		return _T("");
	}

	return m_Columns[nIdx].m_strText;
}

void CWndListCtrl::SetItemHeight(int nItem, int nHeight)
{
	ItemInfo* pItemInfo = GetItemInfo(nItem);

	if (pItemInfo)
	{
		pItemInfo->m_nItemHeight = nHeight;
		pItemInfo->m_nMinHeight = nHeight;
	}

	if (m_bRedraw)
	{
		Draw();
		Invalidate(FALSE);
	}
}

void CWndListCtrl::SetItemLinker(int nItem, int nSubItem, LPCTSTR lpszLinker)
{
	CellInfo *pCellInfo = GetCellInfo(nItem, nSubItem);
	if (pCellInfo)
	{
		pCellInfo->m_strLinker[0] = lpszLinker;
	}
}

void CWndListCtrl::SetItemSingleLine(int nItem, int nSubItem, BOOL bSingleLine)
{
	CellInfo *pCellInfo = GetCellInfo(nItem, nSubItem);
	if (pCellInfo)
	{
		pCellInfo->m_bSingleLine = bSingleLine;
	}
}

void CWndListCtrl::SetItemIsButton(int nItem, int nSubItem, BOOL bBtn)
{
	CellInfo *pCellInfo = GetCellInfo(nItem, nSubItem);
	if (pCellInfo)
	{
		pCellInfo->m_bBtn = bBtn;
	}
}

void CWndListCtrl::SetItemBKColor(int nItem, int nSubItem, COLORREF color)
{
	ItemInfo* pItemInfo = GetItemInfo(nItem);

	if (pItemInfo)
	{
		pItemInfo->m_clrBack = color;
	}

	if (m_bRedraw)
	{
		Draw();
		Invalidate(FALSE);
	}
}

void CWndListCtrl::SetItemTextOffsetX(int nItem, int nSubItem, int nOffset)
{
	CellInfo *pCellInfo = GetCellInfo(nItem, nSubItem);
	if (pCellInfo)
	{
		pCellInfo->m_nOffsetX = nOffset;
	}
}


void CWndListCtrl::SetItemTextColor(int nItem, int nSubItem, COLORREF color)
{
	CellInfo *pCellInfo = GetCellInfo(nItem, nSubItem);
	if (pCellInfo)
	{
		pCellInfo->m_clrText = color;
	}

	if (m_bRedraw)
	{
		Draw();
		Invalidate(FALSE);
	}
}


void CWndListCtrl::SetItemToolTip(int nItem, int nSubItem, LPCTSTR lpszTip)
{
	CellInfo *pCellInfo = GetCellInfo(nItem, nSubItem);
	if (pCellInfo)
	{
		pCellInfo->m_strToolTip = lpszTip;
	}
}

DWORD_PTR CWndListCtrl::GetItemData(int nItem)
{
	ItemInfo* pItemInfo = GetItemInfo(nItem);

	if (pItemInfo)
	{
		return pItemInfo->m_dwData;
	}

	return NULL;
}
BOOL CWndListCtrl::SetItemData(int nItem, DWORD_PTR dwData)
{
	ItemInfo* pItemInfo = GetItemInfo(nItem);

	if (pItemInfo)
	{
		pItemInfo->m_dwData = dwData;

		return TRUE;
	}

	return FALSE;
}

BOOL CWndListCtrl::GetCheck(int nItem)
{
	ItemInfo* pItemInfo = GetItemInfo(nItem);

	if (pItemInfo)
	{
		return pItemInfo->m_bChecked;
	}

	return NULL;
}
void CWndListCtrl::SetCheck(int nItem, BOOL bCheck)
{
	ItemInfo* pItemInfo = GetItemInfo(nItem);

	if (pItemInfo)
	{
		pItemInfo->m_bChecked = bCheck;
	}
}

void CWndListCtrl::RefreshParam(void)
{

	int nTotal = (int)m_Items.size();

	int nW = m_rectWork.Width();
	int nH = m_rectWork.Height();

	m_ILParam.m_nColCnt = m_nItemCountPerRow;

	m_ILParam.m_nItemH = 0;
	for (int i = 0; i < nTotal;i ++)
	{
		if (m_Items[i].m_nItemHeight > m_ILParam.m_nItemH)
		{
			m_ILParam.m_nItemH = m_Items[i].m_nItemHeight;
		}
	}

	if(m_ILParam.m_nColCnt == 0)
		m_ILParam.m_nColCnt = 1;

	// 能显示出大多数，则显示，所以+0.9
	m_ILParam.m_nRowCnt = (double)(nH - m_nTopSpan - m_nBottomSpan) / (m_ILParam.m_nItemH ) + 0.999;

	m_ILParam.m_nItemCntPerPage = m_ILParam.m_nColCnt * m_ILParam.m_nRowCnt;

	m_ILParam.m_nTotalRowCnt = nTotal/m_ILParam.m_nColCnt;
	if ( (nTotal % m_ILParam.m_nColCnt) > 0 )
		m_ILParam.m_nTotalRowCnt += 1;

	if ( 0 == m_ILParam.m_nCurTopLine )
		m_ILParam.m_nItemAtTop = m_ILParam.m_nCurTopLine;
	else{
		m_ILParam.m_nItemAtTop = m_ILParam.m_nCurTopLine * m_ILParam.m_nColCnt;
	}

	if ((m_ILParam.m_nTotalRowCnt > 0 && m_ILParam.m_nTotalRowCnt >= m_ILParam.m_nItemCntPerPage / m_ILParam.m_nColCnt) || m_bShowScrollBarAlway)
	{
		if (m_bShowScrollBar == FALSE)
		{
			CoolSB_ShowScrollBar(m_hWnd, SB_VERT, TRUE);
			m_bShowScrollBar = TRUE;
		}

		UpdateScrollbars();
	}
	else
	{
		if (m_bShowScrollBar == TRUE)
		{
			m_bShowScrollBar = FALSE;
			CoolSB_ShowScrollBar(m_hWnd, SB_VERT, FALSE);

			CoolSB_EnableScrollBar(m_hWnd, SB_VERT,FALSE);
		}

	}
}

void CWndListCtrl::ScrollUPLine(void)
{
	if ( GetItemCount() <= 0 )
		return;

	if ( m_ILParam.m_nCurTopLine == 0 )
		return;

	m_ILParam.m_nCurTopLine -= 1;

	Draw();
	Invalidate(FALSE);
}

void CWndListCtrl::ScrollDWLine(void)
{
	if ( GetItemCount() <= 0 )
		return;

	if ( m_ILParam.m_nItemAtTop + m_ILParam.m_nItemCntPerPage >= GetItemCount() +  m_ILParam.m_nColCnt)
		return;

	m_ILParam.m_nCurTopLine += 1;

	Draw();
	Invalidate(FALSE);
}
void CWndListCtrl::ScrollUPPage(void)
{
	if ( GetItemCount() <= 0 )
		return;

	if ( m_ILParam.m_nCurTopLine == 0 )
		return;

	m_ILParam.m_nCurTopLine -= m_ILParam.m_nRowCnt - 1;

	if (m_ILParam.m_nCurTopLine < 0)
	{
		m_ILParam.m_nCurTopLine = 0;
	}
	if (m_ILParam.m_nCurTopLine > m_ILParam.m_nTotalRowCnt - m_ILParam.m_nRowCnt)
	{
		m_ILParam.m_nCurTopLine = m_ILParam.m_nTotalRowCnt - m_ILParam.m_nRowCnt;
	}

	Draw();
	Invalidate(FALSE);
}

void CWndListCtrl::ScrollDWPage(void)
{
	if ( GetItemCount() <= 0 )
		return;

	m_ILParam.m_nCurTopLine += m_ILParam.m_nRowCnt - 1;
	if (m_ILParam.m_nCurTopLine < 0)
	{
		m_ILParam.m_nCurTopLine = 0;
	}
	if (m_ILParam.m_nCurTopLine > m_ILParam.m_nTotalRowCnt - m_ILParam.m_nRowCnt)
	{
		m_ILParam.m_nCurTopLine = m_ILParam.m_nTotalRowCnt - m_ILParam.m_nRowCnt;
	}

	Draw();
	Invalidate(FALSE);
}

void CWndListCtrl::ClearParam(void)
{
	memset(&m_ILParam, 0, sizeof(CtrlParam));
}

CWndListCtrl::CellInfo* CWndListCtrl::GetCellInfo(int nItem, int nSubItem)
{
	if (nItem < 0 || nItem > (int)m_Items.size() - 1)
	{
		return NULL;
	}

	if (nSubItem < 0 || nSubItem > (int)m_Items[nItem].m_aCellInfo.size() - 1)
	{
		return NULL;
	}
	
	return &m_Items[nItem].m_aCellInfo[nSubItem];
}

CWndListCtrl::ItemInfo* CWndListCtrl::GetItemInfo(int nItem)
{
	if (nItem < 0 || nItem > (int)m_Items.size() - 1)
	{
		return NULL;
	}

	return &m_Items[nItem];
}

void CWndListCtrl::OnPaint()
{
	CPaintDC dc(this); 

	CRect rect = m_rectClient;

	dc.BitBlt(
		0,
		0,
		rect.Width(),
		rect.Height(),
		&m_MemDC,
		0,
		0,
		SRCCOPY
		);
}

void CWndListCtrl::Draw()
{
	if (!::IsWindow(m_hWnd))
	{
		return;
	}


	RefreshParam();

	int iOldBkMode = m_MemDC.SetBkMode(TRANSPARENT);


	Graphics graph(m_MemDC.m_hDC);

	// Fill background
	// 如果调用了SetBkImage 设置则用图片填充，否则backGround色填充
	if (m_pImageBackground) 
	{
		RectF rfDest(PointF((REAL)(m_rectClient.left), (REAL)(m_rectClient.top)), SizeF((REAL)(m_rectClient.Width()), (REAL)(m_rectClient.Height())));

		graph.DrawImage(m_pImageBackground, rfDest, (REAL)m_rectClient.left, (REAL)m_rectClient.top, (REAL)(m_pImageBackground->GetWidth()), (REAL)(m_pImageBackground->GetHeight()), UnitPixel, NULL);
	}
	else
	{
		m_MemDC.FillSolidRect(&m_rectClient, m_colorBackGround);
	}

	// 画标题

	if (m_nTitleHeight > 0)
	{
		// 标题栏背景
		{
			
			CRect rectTitle = m_rectClient;
			rectTitle.bottom = rectTitle.top + m_nTitleHeight;

			m_MemDC.FillSolidRect(&rectTitle, m_colorTitleTop);

			TRIVERTEX tv[] = {
				{ rectTitle.left, rectTitle.top, GetRValue(m_colorTitleTop) << 8, GetGValue(m_colorTitleTop) << 8, GetBValue(m_colorTitleTop) << 8, 20},
				{ rectTitle.right, rectTitle.bottom - 1, GetRValue(m_colorTitleBottom) << 8, GetGValue(m_colorTitleBottom) << 8, GetBValue(m_colorTitleBottom) << 8, 20}
			};
			GRADIENT_RECT gr = { 0, 1 };

			m_MemDC.GradientFill ( tv, 2, &gr, 1, GRADIENT_FILL_RECT_V );

		}

		for (int nIdx = 0; nIdx < m_nItemCountPerRow; nIdx++)
		{
			int nWindth = m_rectClient.Width() / m_nItemCountPerRow;
			CRect rectCol = m_rectClient;

			rectCol.left = m_rectClient.left + nIdx * nWindth;
			rectCol.right = rectCol.left;

			rectCol.bottom = rectCol.top + m_nTitleHeight;

			for(int nCol = 0; nCol < GetColumnCount(); nCol++)
			{
				rectCol.left = rectCol.right;
				rectCol.right = rectCol.left + GetColumnWidth(nCol);

				UINT uFormat    = DT_LEFT;
				switch(GetColumnFormat(nCol)) 
				{
				case 1:
					uFormat = DT_RIGHT;
					break;
				case 2:
					uFormat = DT_CENTER;
					break;
				default:
					break;
				}

				uFormat |= DT_VCENTER | DT_SINGLELINE;

				CString stsLabel = GetColumnText(nCol);
				int nLinkCount = 0;
				int nMinHeight = m_nTitleHeight;
				int nNeedHeight = m_nTitleHeight;

				CRect rectTemp = rectCol;
				rectTemp.left = rectTemp.left + 5;

				DoPaintPrettyText(m_MemDC.m_hDC, rectTemp, stsLabel, RGB(0, 0, 0), INVALID__UICOLOR, NULL, nLinkCount, nNeedHeight, nMinHeight, uFormat);
		
			}
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////
	////开始绘画Items 
	m_visableItemRectList.clear();

	int nTotal = (int)m_Items.size();
	int nCnt = 0;

	CRect rectItem;
	
	//m_rectWork是通过Span调整后的RECT。
	rectItem.left = m_rectWork.left;
	rectItem.right = m_rectWork.right;

	rectItem.top = m_rectWork.top;
	rectItem.bottom = m_rectWork.top;

	int nItemWindth = (m_rectWork.right - m_rectWork.left) / m_nItemCountPerRow;
    //开始画Items
	for( int nItem = m_ILParam.m_nItemAtTop; nItem < nTotal; nItem++ )
	{
		if ( nItem > nTotal )
			break;

		if ( nCnt + 1 > m_ILParam.m_nItemCntPerPage)
			break;

		
		ItemInfo* pItemInfo = GetItemInfo(nItem);
		if (pItemInfo == NULL)
		{
			continue;
		}


		rectItem.left = m_rectWork.left + (nCnt % m_nItemCountPerRow) * nItemWindth;
		rectItem.right = m_rectWork.left + (nCnt % m_nItemCountPerRow + 1) * nItemWindth;
		

		if (nCnt > 0 && nCnt % m_nItemCountPerRow == 0)
		{
			rectItem.top = rectItem.bottom;	
		}
		else
		{
			if (nCnt == 0)
			{
				rectItem.top = m_rectWork.top;
			}
			
		}
        //设置RectItem的底部位置
		rectItem.bottom = rectItem.top  + pItemInfo->m_nItemHeight;

		if (pItemInfo->m_bSelected) //判断此Item是否可被Selected
		{
			if(m_pImageSelectedBK)
			{
				DoFillRect(m_MemDC.m_hDC, rectItem, CRect(0, 0, m_pImageSelectedBK->GetWidth(), m_pImageSelectedBK->GetHeight()), m_pImageSelectedBK);
			}
			else
			{
				CRect rectSel = rectItem;

				if (NULL != m_pImageItemSplit)
				{
					rectSel.bottom -= m_pImageItemSplit->GetHeight();
					rectSel.bottom -= 2;
				}

				CBrush br(m_colorItemSelectedBk);
				CPen pen(PS_SOLID, 0, m_colorItemSelectedBorder);
				CBrush *pOldBrush = m_MemDC.SelectObject(&br);
				CPen *pOldPen = m_MemDC.SelectObject(&pen);

				m_MemDC.Rectangle(&rectSel);
				m_MemDC.SelectObject(pOldBrush);
				m_MemDC.SelectObject(pOldPen);
			}

		}
		else 
		{
			if (pItemInfo->m_clrBack != INVALID__UICOLOR)
			{
				CBrush br(pItemInfo->m_clrBack);
				CPen pen(PS_SOLID, 0, pItemInfo->m_clrBack);
				CBrush *pOldBrush = m_MemDC.SelectObject(&br);
				CPen *pOldPen = m_MemDC.SelectObject(&pen);

				m_MemDC.Rectangle(&rectItem);
				m_MemDC.SelectObject(pOldBrush);
				m_MemDC.SelectObject(pOldPen);
			}

		}
		if (m_bCurrentItemBkColorOpened)
		{	
             m_visableItemRectList.insert(make_pair(nItem,rectItem));  		

			if (pItemInfo->m_bCursorInSide)
			{			
				if (INVALID__UICOLOR != m_currentItemBkColor)
				{ 
					CBrush brush(m_currentItemBkColor);
					CPen pen(PS_SOLID, 0, m_currentItemBkColor);
					CBrush *pOldBrush = m_MemDC.SelectObject(&brush);
					CPen *pOldPen = m_MemDC.SelectObject(&pen);

					m_MemDC.Rectangle(&rectItem);
					m_MemDC.SelectObject(pOldBrush);
					m_MemDC.SelectObject(pOldPen);
				}
			}
		}

		if (GetItemSplitIsOpen())
		{
			if(nItem <  nTotal - 1)
			{	
				if (NULL != m_pImageItemSplit)
				{
					CRect rt = rectItem;
					rt.top = rectItem.bottom - m_iItemSplitHeight;
					rt.bottom = rectItem.bottom;				
					graph.DrawImage(m_pImageItemSplit,rt.left,rt.top,rt.Width(),rt.Height());

				}
			}
		}

	    //开始绘画Cells
		CRect rectCell;
		rectCell.top = rectItem.top;
		rectCell.bottom = rectItem.bottom;
		

		rectCell.left = rectItem.left;
		rectCell.right = rectItem.left;
    
		{
			for(int nSubItem = 0; nSubItem < GetColumnCount(); nSubItem++) 
			{
				rectCell.left = rectCell.right;
				rectCell.right += GetColumnWidth(nSubItem);

				if (rectCell.right > rectItem.right)
				{
					rectCell.right = rectItem.right;
				}

				CellInfo* pCellInfo = GetCellInfo(nItem, nSubItem);

				if (pCellInfo == NULL)
				{
					continue;
				}

				rectCell.left += pCellInfo->m_nOffsetX;

				if (nSubItem == 0)
				{
					Image* pImage = m_pImageCheckBox;
					if (pImage)
					{
						RECT rcImage = {0};
						RECT rcDest = {0};
				
						int nWidth = pImage->GetWidth() / 4;
						int nHeight = pImage->GetHeight() / 2;

						rcDest.top = rectCell.top + (rectCell.Height() - nHeight) / 2;
						rcDest.bottom = rcDest.top + nHeight;

						rcDest.left = rectCell.left;
						rcDest.right = rcDest.left + nWidth;

						pItemInfo->m_rectBoxArea = rcDest;

						UINT uState = 0;

						if( ::PtInRect(&rcDest, GetLastMousePos()))
						{
							uState = 1;
							if (GetKeyState(VK_LBUTTON)&0x80000000) 
							{ 
								uState = 2;
							}
						}

						if (pItemInfo->m_bChecked == FALSE)
						{
							RECT rcTemp = {nWidth *uState, 0, nWidth * (uState + 1), nHeight};
							rcImage = rcTemp;
						}
						else
						{
							RECT rcTemp = {nWidth *uState, nHeight, nWidth * (uState + 1), nHeight * 2};
							rcImage = rcTemp;
						}
					
						RectF rfDest(PointF((REAL)rcDest.left, (REAL)rcDest.top), SizeF((REAL)(rcImage.right - rcImage.left), (REAL)(rcImage.bottom - rcImage.top)));

						graph.DrawImage(pImage, rfDest, (REAL)rcImage.left, (REAL)rcImage.top, (REAL)(rcImage.right - rcImage.left), (REAL)(rcImage.bottom - rcImage.top), UnitPixel, NULL);
						
						rectCell.left += nWidth + pCellInfo->m_nOffsetX;
						
					}
				}
                
				if (pCellInfo->m_bBtn == TRUE)
				{
					Image* pImage = m_pImageButton;
					if (pImage)
					{
						RECT rcDest = {0};

						int nWidth = pImage->GetWidth() / m_nBtnStatusCount;
						int nHeight = pImage->GetHeight();

						rcDest.top = rectCell.top + (rectCell.Height() - nHeight) / 2;
						rcDest.bottom = rcDest.top + nHeight;

						rcDest.left = rectCell.left + (rectCell.Width() - nWidth) / 2;
						rcDest.right = rcDest.left + nWidth;

						pCellInfo->m_rectBtn = rcDest;

						UINT uState = 0;

						if( ::PtInRect(&rcDest, GetLastMousePos()))
						{
							uState = 1;
							if (GetKeyState(VK_LBUTTON)&0x80000000) 
							{ 
								uState = 2;
							}
						}

						if (uState > m_nBtnStatusCount - 1)
						{
							uState = 0;
						}

						RECT rcImage = {nWidth *uState, 0, nWidth * (uState + 1), nHeight};
						
						RectF rfDest(PointF((REAL)rcDest.left, (REAL)rcDest.top), SizeF((REAL)(rcImage.right - rcImage.left), (REAL)(rcImage.bottom - rcImage.top)));

						graph.DrawImage(pImage, rfDest, (REAL)rcImage.left, (REAL)rcImage.top, (REAL)(rcImage.right - rcImage.left), (REAL)(rcImage.bottom - rcImage.top), UnitPixel, NULL);
					}

					continue;
				}

				CString strLabel = GetItemText(nItem, nSubItem);
				if (strLabel.GetLength() == 0 && pCellInfo->m_pImage1 == NULL)
					continue;
				///开始绘画Item
				
				
				//判断此列是否是用户自定义画图位置的列
				CustomSetImagePosColumn csipc;
				memset(&csipc,0,sizeof(csipc));
				
				bool ret = GetCustomImagePos(nSubItem,csipc);   
				//如果是，则调用自定义的Image位置画图
				if (ret)   
				{ 
					int nImageMaxHeight=GetMaxHeight(pCellInfo->m_pImage1,pCellInfo->m_pImage1,pCellInfo->m_pImage1); //获取3个图的最大高度
					//get the text justification
					UINT uFormat    = DT_LEFT;

					switch(GetColumnFormat(nSubItem)) 
					{
					case 1:
						uFormat = DT_RIGHT;
						break;
					case 2:
						uFormat = DT_CENTER;
						break;
					default:
						break;
					}

					if (pCellInfo->m_bSingleLine)
					{
						uFormat |= DT_VCENTER | DT_SINGLELINE;
					}

					CRect rcImage(rectCell.left, 
						rectCell.top, 
						(pCellInfo->m_pImage1 != NULL) ? (rectCell.left + pCellInfo->m_pImage1->GetWidth()) : (rectCell.left), 
						rectCell.bottom);
                    
                    int rcImageRight = 0;
                    if (pCellInfo->m_pImage1)
                    {
						rcImageRight = pCellInfo->m_pImage1->GetWidth();
						
                    }
					//Image1 的位置
					CRect rcImage1(rectCell.left + csipc.m_xImage1Offset, 
						rectCell.top, 
						rectCell.left + csipc.m_xImage1Offset + rcImageRight, 
						rectCell.bottom);					
					if (pCellInfo->m_pImage2)
					{
						rcImageRight = pCellInfo->m_pImage2->GetWidth();
					}
                    //Image2 的位置
					CRect rcImage2(rectCell.left + csipc.m_xImage2Offset, 
						rectCell.top, 
						rectCell.left + csipc.m_xImage2Offset + rcImageRight, 
						rectCell.bottom);
                   
					if (pCellInfo->m_pImage3)
					{
						rcImageRight = pCellInfo->m_pImage3->GetWidth();
					}
					//Image3 的位置
					CRect rcImage3(rectCell.left + csipc.m_xImage3Offset, 
						rectCell.top, 
						rectCell.left + csipc.m_xImage3Offset + rcImageRight, 
						rectCell.bottom);
                    //Text的 位置
					CRect rcText(rcImage.right + pCellInfo->m_nOffsetX,
						rectCell.top, 
						rectCell.right - pCellInfo->m_nOffsetX, 
						rectCell.bottom
						);

					if (m_nImagePos == ImagePos_Top) 
					{
						rcImage1.top = rectCell.top ;
						rcImage1.bottom = (pCellInfo->m_pImage1 != NULL) ? (rectCell.top +  pCellInfo->m_pImage1->GetHeight()+10) : rectCell.top;

						rcImage2.top = rectCell.top ;
						rcImage2.bottom = (pCellInfo->m_pImage2 != NULL) ? (rectCell.top +  pCellInfo->m_pImage2->GetHeight()+10) : rectCell.top;

						rcImage3.top = rectCell.top ; ;
						rcImage3.bottom = (pCellInfo->m_pImage3 != NULL) ? (rectCell.top +  pCellInfo->m_pImage3->GetHeight()+10) : rectCell.top;


						rcText.left = rectCell.left;
						rcText.right = rectCell.right - pCellInfo->m_nOffsetX;
						rcText.top = rectCell.top + nImageMaxHeight + 15;				
						rcText.bottom = rectCell.bottom ;		
					}

					else if (m_nImagePos == ImagePos_Bottom)
					{	
						rcImage1.top = (pCellInfo->m_pImage1 != NULL) ? (rectCell.bottom - pCellInfo->m_pImage1->GetHeight() - 10) : (rectCell.bottom) ;
						rcImage1.bottom = rectCell.bottom;

						rcImage2.top = (pCellInfo->m_pImage2 != NULL) ? (rectCell.bottom - pCellInfo->m_pImage1->GetHeight() - 10) : (rectCell.bottom) ;
						rcImage2.bottom = rectCell.bottom;

						rcImage3.top = (pCellInfo->m_pImage3 != NULL) ? (rectCell.bottom - pCellInfo->m_pImage1->GetHeight() - 10) : (rectCell.bottom) ;
						rcImage3.bottom = rectCell.bottom;

					
						rcText.left = rectCell.left;
						rcText.right = rectCell.right - pCellInfo->m_nOffsetX;
						rcText.top = rectCell.top;						
						rcText.bottom = rectCell.bottom - nImageMaxHeight - 10;
					}
					AdJustItemText_ImagePosAdded(rcText,rcImage1,rcImage2,rcImage3,csipc);  //根据设定值对原有位置上下进行调整
					if (pCellInfo->m_pImage1 != NULL)
					{
						Image *pImage = pCellInfo->m_pImage1;

						int width = pImage->GetWidth();
						int height = pImage->GetHeight();

						RectF rfDest(PointF((REAL)rcImage1.left, (REAL)rcImage1.top + (rcImage1.Height() - height) / 2), SizeF((REAL)width, (REAL)height));
						graph.DrawImage(pImage, rfDest, (REAL)0, (REAL)0, (REAL)width, (REAL)height, UnitPixel, NULL);

						DoPaintBorder(m_MemDC.m_hDC, CRect(rfDest.X, rfDest.Y, rfDest.X + rfDest.Width, rfDest.Y + rfDest.Height), m_colorImageBorderInside, m_colorImageBorderOutside);

						
					}

					if (pCellInfo->m_pImage2 != NULL)
					{
						Image *pImage = pCellInfo->m_pImage2;

						int width = pImage->GetWidth();
						int height = pImage->GetHeight();

						RectF rfDest(PointF((REAL)rcImage2.left, (REAL)rcImage2.top + (rcImage2.Height() - height) / 2), SizeF((REAL)width, (REAL)height));
						graph.DrawImage(pImage, rfDest, (REAL)0, (REAL)0, (REAL)width, (REAL)height, UnitPixel, NULL);

						DoPaintBorder(m_MemDC.m_hDC, CRect(rfDest.X, rfDest.Y, rfDest.X + rfDest.Width, rfDest.Y + rfDest.Height), m_colorImageBorderInside, m_colorImageBorderOutside);

						

					}

					if (pCellInfo->m_pImage3 != NULL)
					{
						Image *pImage = pCellInfo->m_pImage3;

						int width = pImage->GetWidth();
						int height = pImage->GetHeight();

						RectF rfDest(PointF((REAL)rcImage3.left, (REAL)rcImage3.top + (rcImage3.Height() - height) / 2), SizeF((REAL)width, (REAL)height));
						graph.DrawImage(pImage, rfDest, (REAL)0, (REAL)0, (REAL)width, (REAL)height, UnitPixel, NULL);

						DoPaintBorder(m_MemDC.m_hDC, CRect(rfDest.X, rfDest.Y, rfDest.X + rfDest.Width, rfDest.Y + rfDest.Height), m_colorImageBorderInside, m_colorImageBorderOutside);
					}

					int nNeedHeight = rcText.Height();
					int nMinHeight = pItemInfo->m_nMinHeight;

					//DWORD dwTime = ::GetTickCount();
					DoPaintPrettyText(m_MemDC.m_hDC, rcText, strLabel, pCellInfo->m_clrText, INVALID__UICOLOR, pCellInfo->m_rectLinker, pCellInfo->m_nLinkerCount, nNeedHeight, nMinHeight, uFormat);
					//dwTime = GetTickCount() - dwTime;

					//TRACE(_T("Draw time is %d\n\n"), dwTime);

					if (nNeedHeight > pItemInfo->m_nItemHeight)
					{
						pItemInfo->m_nItemHeight = nNeedHeight;

						m_bResetItemHeight = TRUE;
					}

					//m_MemDC.DrawText(strLabel, -1, rcText, uFormat);


					m_MemDC.SelectStockObject(SYSTEM_FONT) ;
				}

                else  //如果是此列不是自定义
				{
					//get the text justification
					UINT uFormat    = DT_LEFT;

					switch(GetColumnFormat(nSubItem)) 
					{
					case 1:
						uFormat = DT_RIGHT;
						break;
					case 2:
						uFormat = DT_CENTER;
						break;
					default:
						break;
					}

					if (pCellInfo->m_bSingleLine)
					{
						uFormat |= DT_VCENTER | DT_SINGLELINE;
					}

					CRect rcImage(rectCell.left, 
						rectCell.top, 
						(pCellInfo->m_pImage1 != NULL) ? (rectCell.left + pCellInfo->m_pImage1->GetWidth()) : (rectCell.left), 
						rectCell.bottom);

					CRect rcText(rcImage.right + pCellInfo->m_nOffsetX,
						rectCell.top, 
						rectCell.right - pCellInfo->m_nOffsetX, 
						rectCell.bottom
						);

					if (m_nImagePos == ImagePos_Top)
					{
						rcImage.left = rectCell.left;
						rcImage.right = rectCell.right - pCellInfo->m_nOffsetX;
						rcImage.top = rectCell.top;
						rcImage.bottom = (pCellInfo->m_pImage1 != NULL) ? (rectCell.top + pCellInfo->m_pImage1->GetHeight() + 10) : (rectCell.top);

						if (pCellInfo->m_pImage1 != NULL && rcImage.bottom < rectCell.bottom - 20)
						{
							rcImage.bottom =  rectCell.bottom - 20;
						}


						rcText.left = rectCell.left;
						rcText.right = rectCell.right - pCellInfo->m_nOffsetX;
						rcText.top = rcImage.bottom;
						rcText.bottom = rectCell.bottom;
					}

					else if (m_nImagePos == ImagePos_Bottom)
					{
						rcImage.left = rectCell.left;
						rcImage.right = rectCell.right - pCellInfo->m_nOffsetX;
						rcImage.top = (pCellInfo->m_pImage1 != NULL) ? (rectCell.bottom - pCellInfo->m_pImage1->GetHeight() - 10) : (rectCell.bottom);
						rcImage.bottom = rectCell.bottom;


						rcText.left = rectCell.left;
						rcText.right = rectCell.right - pCellInfo->m_nOffsetX;
						rcText.top = rectCell.top;
						rcText.bottom = rcImage.top;
					}

					int nAllImageWidth = 0;

					if (pCellInfo->m_pImage1 != NULL)
					{
						nAllImageWidth += pCellInfo->m_pImage1->GetWidth();
					}

					if (pCellInfo->m_pImage2 != NULL)
					{
						nAllImageWidth += pCellInfo->m_pImage2->GetWidth();
						nAllImageWidth += 5;
					}

					if (pCellInfo->m_pImage3 != NULL)
					{
						nAllImageWidth += pCellInfo->m_pImage3->GetWidth();
						nAllImageWidth += 5;
					}

					if (uFormat & DT_CENTER)
					{
						rcImage.left = rcImage.left + (rcImage.Width() - nAllImageWidth) / 2;
					}
					else if (uFormat & DT_RIGHT)
					{
						rcImage.left = rcImage.right - nAllImageWidth;
					}


					if (pCellInfo->m_pImage1 != NULL)
					{
						Image *pImage = pCellInfo->m_pImage1;

						int width = pImage->GetWidth();
						int height = pImage->GetHeight();

						RectF rfDest(PointF((REAL)rcImage.left, (REAL)rcImage.top + (rcImage.Height() - height) / 2), SizeF((REAL)width, (REAL)height));
						graph.DrawImage(pImage, rfDest, (REAL)0, (REAL)0, (REAL)width, (REAL)height, UnitPixel, NULL);

						DoPaintBorder(m_MemDC.m_hDC, CRect(rfDest.X, rfDest.Y, rfDest.X + rfDest.Width, rfDest.Y + rfDest.Height), m_colorImageBorderInside, m_colorImageBorderOutside);

						rcImage.left += width + 5;
					}

					if (pCellInfo->m_pImage2 != NULL)
					{
						Image *pImage = pCellInfo->m_pImage2;

						int width = pImage->GetWidth();
						int height = pImage->GetHeight();

						RectF rfDest(PointF((REAL)rcImage.left, (REAL)rcImage.top + (rcImage.Height() - height) / 2), SizeF((REAL)width, (REAL)height));
						graph.DrawImage(pImage, rfDest, (REAL)0, (REAL)0, (REAL)width, (REAL)height, UnitPixel, NULL);

						DoPaintBorder(m_MemDC.m_hDC, CRect(rfDest.X, rfDest.Y, rfDest.X + rfDest.Width, rfDest.Y + rfDest.Height), m_colorImageBorderInside, m_colorImageBorderOutside);

						rcImage.left += width + 5;

					}

					if (pCellInfo->m_pImage3 != NULL)
					{
						Image *pImage = pCellInfo->m_pImage3;

						int width = pImage->GetWidth();
						int height = pImage->GetHeight();

						RectF rfDest(PointF((REAL)rcImage.left, (REAL)rcImage.top + (rcImage.Height() - height) / 2), SizeF((REAL)width, (REAL)height));
						graph.DrawImage(pImage, rfDest, (REAL)0, (REAL)0, (REAL)width, (REAL)height, UnitPixel, NULL);

						DoPaintBorder(m_MemDC.m_hDC, CRect(rfDest.X, rfDest.Y, rfDest.X + rfDest.Width, rfDest.Y + rfDest.Height), m_colorImageBorderInside, m_colorImageBorderOutside);

						rcImage.left += width + 5;
					}

					int nNeedHeight = rcText.Height();
					int nMinHeight = pItemInfo->m_nMinHeight;

					//DWORD dwTime = ::GetTickCount();
    				DoPaintPrettyText(m_MemDC.m_hDC, rcText, strLabel, pCellInfo->m_clrText, INVALID__UICOLOR, pCellInfo->m_rectLinker, pCellInfo->m_nLinkerCount, nNeedHeight, nMinHeight, uFormat);
					//dwTime = GetTickCount() - dwTime;

					//TRACE(_T("Draw time is %d\n\n"), dwTime);

					if (nNeedHeight > pItemInfo->m_nItemHeight)
					{
						pItemInfo->m_nItemHeight = nNeedHeight;

						m_bResetItemHeight = TRUE;
					}

					//m_MemDC.DrawText(strLabel, -1, rcText, uFormat);


					m_MemDC.SelectStockObject(SYSTEM_FONT) ;

				}
				

			}


		}
			    	
		nCnt++;
		
	}
	//结束画Items



	m_MemDC.SetBkMode(iOldBkMode);

	if (m_bResetItemHeight && nTotal > 0)
	{
		m_bResetItemHeight = FALSE;
		Draw();
		Invalidate();
	}

	DrawGird();
}

BOOL CWndListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	//cs.style |= WS_CHILD | WS_VSCROLL | SS_NOTIFY &~WS_TABSTOP;

	return CWnd::PreCreateWindow(cs);
}

void CWndListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	this->SetFocus();

	if(m_ILParam.m_nColCnt == 0)
		return;

	switch(nSBCode)
	{
	case SB_LINEDOWN:
		ScrollDWLine();
		break;

	case SB_LINEUP:
		ScrollUPLine();
		break;

	case SB_PAGEDOWN:

		ScrollDWPage();
		break;

	case SB_PAGEUP:
		ScrollUPPage();
		break;

	case SB_THUMBTRACK:
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_TRACKPOS;

		CoolSB_GetScrollInfo(m_hWnd, SB_VERT, &si);

		if (m_ILParam.m_nCurTopLine != si.nTrackPos)
		{
			m_ILParam.m_nCurTopLine = si.nTrackPos;
			m_ILParam.m_nItemAtTop = si.nTrackPos * m_ILParam.m_nColCnt ;

			Draw();
			Invalidate(FALSE);
		}


		break;

	}

	//CoolSB_SetScrollPos(m_hWnd, SB_VERT, nPos, TRUE);

	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CWndListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CRect rect = m_rectClient;

	ClientToScreen(&rect);
	if (rect.PtInRect(pt))
	{
		if(zDelta > 0)
		{
			OnVScroll(SB_LINEUP, 0, NULL);
		}
		else
		{
			OnVScroll(SB_LINEDOWN, 0, NULL);
		}
	}
	
	return TRUE;
}

int CWndListCtrl::HitTestEx(CPoint &point, int *col)
{
	int colnum = 0;
	if ( col ) *col = 0;

	int nTotal = (int)m_Items.size();

	// Get the number of columns
	int nColumnCount = GetColumnCount();

	int nCnt = 0;

	CRect rectItem;

	rectItem.left = m_rectWork.left;
	rectItem.right = m_rectWork.right;

	rectItem.top = m_rectWork.top;
	rectItem.bottom = m_rectWork.top;

	int nItemWindth = (m_rectWork.right - m_rectWork.left) / m_nItemCountPerRow;

	for( int nItem = GetTopIndex(); nItem < nTotal; nItem++ )
	{
		if ( nItem > nTotal )
			break;

		if ( nCnt + 1 > m_ILParam.m_nItemCntPerPage)
			break;


		ItemInfo* pItemInfo = GetItemInfo(nItem);
		if (pItemInfo == NULL)
		{
			continue;
		}


		rectItem.left = m_rectWork.left + (nCnt % m_nItemCountPerRow) * nItemWindth;
		rectItem.right = m_rectWork.left + (nCnt % m_nItemCountPerRow + 1) * nItemWindth;


		if (nCnt > 0 && nCnt % m_nItemCountPerRow == 0)
		{
			rectItem.top = rectItem.bottom;	
		}
		else
		{
			if (nCnt == 0)
			{
				rectItem.top = m_rectWork.top;
			}

		}

		rectItem.bottom = rectItem.top  + pItemInfo->m_nItemHeight;

		if ( rectItem.PtInRect(point) ) {
			// Now find the column
			for ( colnum = 0; colnum < nColumnCount; colnum++ ) {
				int colwidth = GetColumnWidth(colnum);
				if ( point.x >= rectItem.left
					&& point.x <= (rectItem.left + colwidth ) ) {
						if ( col ) *col = colnum;
						return nItem;
				}
				rectItem.left += colwidth;
			}
		}

		nCnt++;
	}

	return -1;
}

void CWndListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->SetFocus();
	CWnd::OnLButtonDown(nFlags, point);

	int nItem;

	int nSubItem;
	if ( (( nItem = HitTestEx( point, &nSubItem )) != -1) ) {
		if (m_nSelectedItem < 0 || m_nSelectedItem > (int)m_Items.size() - 1)
		{
			m_nSelectedItem = -1;
		}

		if (nItem >= 0 && nItem < (int)m_Items.size() && m_bCanSelected)
		{
			if (m_nSelectedItem != nItem)
			{
				if (m_nSelectedItem != -1)
				{
					m_Items[m_nSelectedItem].m_bSelected =  ~m_Items[m_nSelectedItem].m_bSelected;
				}
				m_Items[nItem].m_bSelected = ~m_Items[nItem].m_bSelected;

				m_nSelectedItem = nItem;

				SendMessageToParent(WM_WND_LIST_SELECTED_CHANGED, (WPARAM)m_hWnd);
				SendMessageToParent(WM_WND_LIST_ITEM_LBUTTON_CLICK, (WPARAM)m_hWnd, (LPARAM)0);		

				Draw();
				Invalidate(FALSE);
			}
			else
			{
				SendMessageToParent(WM_WND_LIST_ITEM_LBUTTON_CLICK, (WPARAM)m_hWnd, (LPARAM)0);	
			}
		}

		if (nSubItem == 0)
		{
			ItemInfo* pItem = GetItemInfo(nItem);
			if (pItem)
			{
				CRect rectBoxArea(pItem->m_rectBoxArea);
				if (rectBoxArea.PtInRect(point))
				{
					pItem->m_bChecked = !pItem->m_bChecked;

					SendMessageToParent(WM_WND_LIST_SELECTED_CHANGED, (WPARAM)m_hWnd);
					SendMessageToParent(WM_WND_LIST_ITEM_LBUTTON_CLICK, (WPARAM)m_hWnd, (LPARAM)0);		

					Draw();
					Invalidate(FALSE);

				}
			}
		}

		CellInfo* pCellInfo = GetCellInfo(nItem, nSubItem);
		if (pCellInfo)
		{
			for (int i = 0; i < pCellInfo->m_nLinkerCount; i ++)
			{
				CRect rectLinker(pCellInfo->m_rectLinker[i]);

				if (rectLinker.PtInRect(point))
				{
					pCellInfo->m_nCurrLinkerIdx = i;

					::SetCursor(m_hCursorHander);

					SendMessageToParent(WM_WND_LIST_ITEM_LBUTTON_CLICK, (WPARAM)m_hWnd, (LPARAM)_tcsdup(pCellInfo->m_strLinker[i]));		
				}
			}

			if (pCellInfo->m_bBtn == TRUE)
			{
				CRect rectLinker(pCellInfo->m_rectBtn);

				if (rectLinker.PtInRect(point))
				{
					::SetCursor(m_hCursorHander);
					Draw();
					Invalidate(FALSE);
				}
			}
		}
	}

}

void CWndListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	this->SetFocus();
	CWnd::OnRButtonDown(nFlags, point);

	int nItem;

	int nSubItem;
	if ( (( nItem = HitTestEx( point, &nSubItem )) != -1) ) {

		if (m_nSelectedItem < 0 || m_nSelectedItem > (int)m_Items.size() - 1)
		{
			m_nSelectedItem = -1;
		}

		if (nItem >= 0 && nItem < (int)m_Items.size() && m_bCanSelected)
		{
			CellInfo* pCellInfo = GetCellInfo(nItem, nSubItem);
			SendMessageToParent(WM_WND_LIST_ITEM_RBUTTON_CLICK, (WPARAM)m_hWnd, (LPARAM)NULL);

			if (m_nSelectedItem != nItem)
			{
				if (m_nSelectedItem != -1)
				{
					m_Items[m_nSelectedItem].m_bSelected =  ~m_Items[m_nSelectedItem].m_bSelected;
				}
				m_Items[nItem].m_bSelected = ~m_Items[nItem].m_bSelected;

				m_nSelectedItem = nItem;

				SendMessageToParent(WM_WND_LIST_SELECTED_CHANGED, (WPARAM)m_hWnd);

				Draw();
				Invalidate(FALSE);
			}
		}
	}
}


void CWndListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	/////////////////////////////////
	CWnd::OnLButtonUp(nFlags, point);

	int nItem;
	int nSubItem;
	BOOL bMouseOnLinker = FALSE;

	if ( (( nItem = HitTestEx( point, &nSubItem )) != -1) ) 
	{
		CellInfo* pCellInfo = GetCellInfo(nItem, nSubItem);
		if (pCellInfo)
		{
			
			if (pCellInfo->m_bBtn == TRUE)
			{
				CRect rectLinker(pCellInfo->m_rectBtn);

				if (rectLinker.PtInRect(point))
				{
					::SetCursor(m_hCursorHander);
					SendMessageToParent(WM_WND_LIST_ITEM_BUTTON_CLICK, (WPARAM)m_hWnd);
				}
			}
		}
	}
}

void CWndListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnMouseMove(nFlags, point);
	if( !m_bMouseTracking ) {
		TRACKMOUSEEVENT tme = { 0 };
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = m_hwndTooltip == NULL ? 1000UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
		_TrackMouseEvent(&tme);
		m_bMouseTracking = TRUE;
	}


	m_ptLastMouse = point;

	int nItem = -1;
	int nSubItem = -1;
	BOOL bMouseOnLinker = FALSE;
	BOOL bHideToolTip = FALSE;

	if ( (( nItem = HitTestEx( point, &nSubItem )) != -1) ) 
	{
		CellInfo* pCellInfo = GetCellInfo(nItem, nSubItem);
		if (pCellInfo)
		{
			if (pCellInfo->m_strToolTip.GetLength() == 0)
			{
				bHideToolTip = TRUE;
			}

			for (int i = 0; i < pCellInfo->m_nLinkerCount; i ++)
			{
				CRect rectLinker(pCellInfo->m_rectLinker[i]);

				if (rectLinker.PtInRect(point))
				{
					bMouseOnLinker = TRUE;
					::SetCursor(m_hCursorHander);
				}
			}

			if (pCellInfo->m_bBtn == TRUE)
			{
				CRect rectLinker(pCellInfo->m_rectBtn);

				if (rectLinker.PtInRect(point))
				{
					bMouseOnLinker = TRUE;
					::SetCursor(m_hCursorHander);
				}
			}
		}

		if (nSubItem == 0)
		{
			ItemInfo* pItem = GetItemInfo(nItem);
			if (pItem)
			{
				CRect rectLinker(pItem->m_rectBoxArea);
				if (rectLinker.PtInRect(point))
				{
					bMouseOnLinker = TRUE;
				}
			}
		}
	}

	if (m_bCurrentItemBkColorOpened)
	{	
		for (vector<ItemInfo>::iterator itemIter = m_Items.begin();itemIter != m_Items.end();++itemIter)
		{
			itemIter->m_bCursorInSide = false;
		}
		map<int,CRect>::const_iterator iter;
		for ( iter = m_visableItemRectList.begin();iter != m_visableItemRectList.end();++ iter)
		{
			RECT rt ;
			rt.left = iter->second.left;
			rt.top = iter->second.top;
			rt.right = iter->second.right;
			rt.bottom = iter->second.bottom;

			POINT pt;
			pt.x = point.x;
			pt.y = point.y;
			if (PtInRect(&rt,pt))
			{
				break;
			}
		}
		if (m_visableItemRectList.end() == iter)
		{
			return ;
		}
		ItemInfo* pItem = GetItemInfo(iter->first);
		if (pItem)
		{
			if (false == pItem->m_bCursorInSide)
			{
				pItem->m_bCursorInSide = true;
				Draw();
				Invalidate(FALSE);
			}
		}
		

	}

	if (bMouseOnLinker != m_bMouseOnLinker)
	{
		m_bMouseOnLinker = bMouseOnLinker;

		Draw();
		Invalidate(FALSE);
		
	}

	if ( nItem == -1 || nSubItem == -1 || bHideToolTip)
	{
		if( m_hwndTooltip != NULL ) 
		{
			::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
		}

		m_bMouseTracking = FALSE;
	}	
}

void CWndListCtrl::UpdateScrollbars()
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = m_ILParam.m_nTotalRowCnt;
	si.nPage = min(m_ILParam.m_nRowCnt, m_ILParam.m_nTotalRowCnt);
	si.nPos = m_ILParam.m_nCurTopLine;

	CoolSB_SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);

}

void CWndListCtrl::DeleteAllItems(void)
{
	m_Items.clear();

	m_nSelectedItem = -1;

	if (m_bRedraw)
	{
		Draw();
		if (IsWindow(m_hWnd))
		{
			Invalidate();
		}
	}

	m_ptLastMouse= CPoint(0, 0);
	
}

BOOL CWndListCtrl::DeleteItem(int nIdx)
{
	BOOL bRet = FALSE;

	if (nIdx < 0 || nIdx > (int)m_Items.size() - 1)
	{
		return bRet;
	}

	m_Items.erase(m_Items.begin() + nIdx);
	m_nSelectedItem = -1;

	if (nIdx - 1 >= 0)
	{
		SetSelectIndex(nIdx - 1) ;
	}
	else
	{
		if (nIdx < m_Items.size())
		{
			SetSelectIndex(nIdx) ;
		}
	}
	

	Draw();
	Invalidate();

	return TRUE;
}


void CWndListCtrl::SendMessageToParent(UINT nMsg, WPARAM wParam, LPARAM lParam)
{	

	::PostMessage(GetParent()->GetSafeHwnd(),nMsg, wParam, lParam);

	if (nMsg == WM_WND_LIST_ITEM_LBUTTON_CLICK)
	{
		this->SetFocus();
	}
}


void CWndListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (::IsWindow(this->m_hWnd)) {
		if (m_MemDC.GetSafeHdc()) {
			if (m_pMemBmpOld != NULL) {
				m_MemDC.SelectObject(m_pMemBmpOld);
				m_pMemBmpOld = NULL;
			}

			m_MemBmp.DeleteObject();
			m_MemDC.DeleteDC();
		}

		if (m_MemSelectDC.GetSafeHdc()) {
			if (m_pMemSelectBmpOld != NULL) {
				m_MemSelectDC.SelectObject(m_pMemSelectBmpOld);
				m_pMemSelectBmpOld = NULL;
			}

			m_MemSelectBmp.DeleteObject();
			m_MemSelectDC.DeleteDC();
		}

		CDC *pDC = GetDC();

		GetClientRect(&m_rectClient);

		m_rectWork.left = m_rectClient.left + m_nLeftSpan;
		m_rectWork.top = m_rectClient.top + m_nTopSpan + m_nTitleHeight;
		m_rectWork.right = m_rectClient.right - m_nRightSpan;
		m_rectWork.bottom = m_rectClient.bottom - m_nBottomSpan;

		m_MemDC.CreateCompatibleDC(pDC);
		m_MemBmp.CreateCompatibleBitmap(pDC, m_rectClient.Width(), m_rectClient.Height());
		m_pMemBmpOld = m_MemDC.SelectObject(&m_MemBmp);

		m_MemSelectDC.CreateCompatibleDC(pDC);
		m_MemSelectBmp.CreateCompatibleBitmap(pDC, m_rectClient.Width(), m_rectClient.Height());
		m_pMemSelectBmpOld = m_MemSelectDC.SelectObject(&m_MemSelectBmp);


		if(pDC != NULL) {
			ReleaseDC(pDC);
			pDC = NULL;
		}

		Draw();
	}
}

int CWndListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//ShowScrollBar(SB_VERT, TRUE);

	LONG lExStyle = GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE);
	lExStyle &= ~ WS_EX_CLIENTEDGE ;
	BOOL bRet = SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, lExStyle);

	DWORD dwEr = ::GetLastError();


	if (m_pImageScrollBar != NULL)
	{
		m_hdcSkin  = CreateCompatibleDC(GetDC()->GetSafeHdc());
		//m_hSkinBmp = (HBITMAP)LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP1));
		m_hSkinBmp = CreateCompatibleBitmap(GetDC()->GetSafeHdc(),m_pImageScrollBar->GetWidth(),m_pImageScrollBar->GetHeight());
		SelectObject(m_hdcSkin, m_hSkinBmp);

		Graphics graph(m_hdcSkin);
		RectF rfDest(PointF((REAL)0, (REAL)0), SizeF((REAL)m_pImageScrollBar->GetWidth(), (REAL)m_pImageScrollBar->GetHeight()));
		graph.DrawImage(m_pImageScrollBar, rfDest, (REAL)0, (REAL)0, (REAL)m_pImageScrollBar->GetWidth(), (REAL)m_pImageScrollBar->GetHeight(), UnitPixel, NULL);

	}
	

	InitializeCoolSB(m_hWnd);

	CoolSB_SetStyle(m_hWnd, SB_VERT, CSBS_HOTTRACKED);

	CoolSB_SetSize(m_hWnd, SB_VERT, m_nScrollHeight, m_nScrollWidth);
	CoolSB_SetMinThumbSize(m_hWnd, SB_VERT, m_nScrollHeight + 4);

	CoolSB_ShowScrollBar(m_hWnd, SB_VERT, FALSE);
	CoolSB_EnableScrollBar(m_hWnd, SB_VERT,FALSE);

	LOGFONT logFont;
	CFont* pFont = GetParent()->GetFont();

	if (pFont && pFont->GetLogFont(&logFont))
	{	
		m_NormalFont.CreateFontIndirect(&logFont);

		logFont.lfUnderline = TRUE;
		m_LinkerFont.CreateFontIndirect(&logFont);

		logFont.lfUnderline = FALSE;
		logFont.lfWeight = FW_BOLD;
		m_BoldFont.CreateFontIndirect(&logFont);
	}

	return 0;
}

void CWndListCtrl::OnDestroy()
{

	if (m_MemDC.GetSafeHdc()) {
		if (m_pMemBmpOld != NULL) {
			m_MemDC.SelectObject(m_pMemBmpOld);
			m_pMemBmpOld = NULL;
		}

		m_MemBmp.DeleteObject();
		m_MemDC.DeleteDC();
	}

	if (m_MemSelectDC.GetSafeHdc()) {
		if (m_pMemSelectBmpOld != NULL) {
			m_MemSelectDC.SelectObject(m_pMemSelectBmpOld);
			m_pMemSelectBmpOld = NULL;
		}

		m_MemSelectBmp.DeleteObject();
		m_MemSelectDC.DeleteDC();
	}

	CWnd::OnDestroy();

}


void CWndListCtrl::OnMyKeyDown(UINT nChar)
{
	if (m_nSelectedItem == -1)
		return;

	int nSelectdNew = -1;

	if (nChar == VK_LEFT) {
		nSelectdNew = max(0, m_nSelectedItem - 1);
	}
	else if (nChar == VK_RIGHT) {
		nSelectdNew = min(m_nSelectedItem + 1, (int)m_Items.size() - 1);
	}
	else if (nChar == VK_UP) {
		if (m_nSelectedItem - m_ILParam.m_nColCnt < 0) {
			return;
		}

		nSelectdNew = m_nSelectedItem - m_ILParam.m_nColCnt;
	}
	else if (nChar == VK_DOWN) {
		if (m_nSelectedItem + m_ILParam.m_nColCnt > (int)m_Items.size() - 1) {
			return;
		}

		nSelectdNew = m_nSelectedItem + m_ILParam.m_nColCnt;
	}

	if (nSelectdNew == -1)
		return;

	m_Items[m_nSelectedItem].m_bSelected = ~m_Items[m_nSelectedItem].m_bSelected;

	m_Items[nSelectdNew].m_bSelected = ~m_Items[nSelectdNew].m_bSelected;

	if (m_Items[nSelectdNew].m_bSelected) {
		m_nSelectedItem = nSelectdNew;

		SendMessageToParent(WM_WND_LIST_SELECTED_CHANGED, (WPARAM)m_hWnd);
	}
	else {
		m_nSelectedItem = -1;
	}

	m_ILParam.m_nCurTopLine = m_nSelectedItem / m_ILParam.m_nColCnt - m_ILParam.m_nItemCntPerPage / m_ILParam.m_nColCnt + 2;

	if (m_ILParam.m_nCurTopLine < 0)
	{
		m_ILParam.m_nCurTopLine = 0;
	}
	Draw();
	Invalidate(FALSE);
}

void CWndListCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | WS_VSCROLL | SS_NOTIFY);


	if (::IsWindow(this->m_hWnd)) {
		if (m_MemDC.GetSafeHdc()) {
			if (m_pMemBmpOld != NULL) {
				m_MemDC.SelectObject(m_pMemBmpOld);
				m_pMemBmpOld = NULL;
			}

			m_MemBmp.DeleteObject();
			m_MemDC.DeleteDC();
		}

		if (m_MemSelectDC.GetSafeHdc()) {
			if (m_pMemSelectBmpOld != NULL) {
				m_MemSelectDC.SelectObject(m_pMemSelectBmpOld);
				m_pMemSelectBmpOld = NULL;
			}

			m_MemSelectBmp.DeleteObject();
			m_MemSelectDC.DeleteDC();
		}

		CDC *pDC = GetDC();

		GetClientRect(&m_rectClient);

		m_MemDC.CreateCompatibleDC(pDC);
		m_MemBmp.CreateCompatibleBitmap(pDC, m_rectClient.Width(), m_rectClient.Height());
		m_pMemBmpOld = m_MemDC.SelectObject(&m_MemBmp);

		m_MemSelectDC.CreateCompatibleDC(pDC);
		m_MemSelectBmp.CreateCompatibleBitmap(pDC, m_rectClient.Width(), m_rectClient.Height());
		m_pMemSelectBmpOld = m_MemSelectDC.SelectObject(&m_MemSelectBmp);


		m_rectWork.left = m_rectClient.left + m_nLeftSpan;
		m_rectWork.top = m_rectClient.top + m_nTopSpan;
		m_rectWork.right = m_rectClient.right - m_nRightSpan;
		m_rectWork.bottom = m_rectClient.bottom - m_nBottomSpan;


		if(pDC != NULL) {
			ReleaseDC(pDC);
			pDC = NULL;
		}
	}

	CWnd::PreSubclassWindow();
}


BOOL CWndListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
			case VK_LEFT:	  
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
				OnMyKeyDown(pMsg->wParam);
				return TRUE;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

int CWndListCtrl::SetSelectIndex(int nIdx)
{
	if (m_nSelectedItem < 0 || m_nSelectedItem > (int)m_Items.size() - 1)
	{
		m_nSelectedItem = -1;
	}

	if (nIdx >= 0 && nIdx < (int)m_Items.size())
	{
		if (m_nSelectedItem != nIdx)
		{
			if (m_nSelectedItem != -1)
			{
				m_Items[m_nSelectedItem].m_bSelected =  ~m_Items[m_nSelectedItem].m_bSelected;
			}
			m_Items[nIdx].m_bSelected = ~m_Items[nIdx].m_bSelected;


			m_nSelectedItem = nIdx;

			RefreshParam();

			int nPageLineCount = m_ILParam.m_nItemCntPerPage / m_ILParam.m_nColCnt;

			if (m_nSelectedItem >= m_ILParam.m_nCurTopLine && m_nSelectedItem < m_ILParam.m_nCurTopLine + nPageLineCount)
			{

			}
			else
			{
				m_ILParam.m_nCurTopLine = m_nSelectedItem / m_ILParam.m_nColCnt -  nPageLineCount + 1;
			}

			
			if (m_ILParam.m_nCurTopLine < 0)
			{
				m_ILParam.m_nCurTopLine = 0;
			}

			Draw();
			Invalidate(FALSE);
			SendMessageToParent(WM_WND_LIST_SELECTED_CHANGED, (WPARAM)m_hWnd);
		}
	}

	return m_nSelectedItem;
}

void CWndListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	int BorderWidth = 0;
	if (m_colorFrameOutside != INVALID__UICOLOR)
	{
		BorderWidth++;
	}

	if (m_colorFrameInside != INVALID__UICOLOR)
	{
		BorderWidth++;
	}

	if (bCalcValidRects)
	{
		lpncsp->rgrc[0].top += BorderWidth;
		lpncsp->rgrc[0].bottom -= BorderWidth;
		lpncsp->rgrc[0].left += BorderWidth;
		lpncsp->rgrc[0].right -= BorderWidth;
	}

	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CWndListCtrl::OnNcPaint()
{
	// draw frame
	CWindowDC wdc(this);
	CRect rcWindow;
	
	if (m_colorFrameOutside != INVALID__UICOLOR)
	{
		GetWindowRect(&rcWindow);
		rcWindow.bottom = rcWindow.bottom - rcWindow.top - 1;
		rcWindow.top = 0;
		rcWindow.right = rcWindow.right - rcWindow.left - 1;
		rcWindow.left = 0;

		// Draw background
		{
			CPen pen(PS_SOLID, 1, m_colorFrameOutside);
			CPen *pOldPen = wdc.SelectObject(&pen);
			wdc.MoveTo(rcWindow.left, rcWindow.top);
			wdc.LineTo(rcWindow.right, rcWindow.top);
			wdc.LineTo(rcWindow.right, rcWindow.bottom);
			wdc.LineTo(rcWindow.left, rcWindow.bottom);
			wdc.LineTo(rcWindow.left, rcWindow.top);
			wdc.SelectObject(pOldPen);
		}
	}

	if (m_colorFrameInside != INVALID__UICOLOR)
	{
		GetWindowRect(&rcWindow);
		rcWindow.bottom = rcWindow.bottom - rcWindow.top - 2;
		rcWindow.top = 1;
		rcWindow.right = rcWindow.right - rcWindow.left - 2;
		rcWindow.left = 1;

		// Draw background
		{
			CPen pen(PS_SOLID, 1, m_colorFrameInside);
			CPen *pOldPen = wdc.SelectObject(&pen);
			wdc.MoveTo(rcWindow.left, rcWindow.top);
			wdc.LineTo(rcWindow.right, rcWindow.top);
			wdc.LineTo(rcWindow.right, rcWindow.bottom);
			wdc.LineTo(rcWindow.left, rcWindow.bottom);
			wdc.LineTo(rcWindow.left, rcWindow.top);
			wdc.SelectObject(pOldPen);
		}
	}


	Default();
}

BOOL CWndListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}
LRESULT CWndListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	NMHDR *hdr = (NMHDR *)lParam;
	if (message == WM_NOTIFY)
	{			
		if(hdr->code == NM_COOLSB_CUSTOMDRAW)
		{
			return HandleCustomDraw(wParam, (NMCSBCUSTOMDRAW *)lParam);
		}
	}

	return CWnd::DefWindowProc(message, wParam, lParam);
}

void CWndListCtrl::SetRedraw(BOOL bRedraw)
{
	if (bRedraw)
	{
		Draw();
		Invalidate(FALSE);
	}

	m_bRedraw = bRedraw;
}

void CWndListCtrl::SetCanSelected(BOOL bSelected)
{
	m_bCanSelected = bSelected;
}

void CWndListCtrl::SetImagePos(CWndListCtrl::ImagePos nPos)
{
	m_nImagePos = nPos;
}

BOOL CWndListCtrl::SetSpanSize(int nLeftSpan, int nRightSpan, int nTopSpan, int nBottomSpan)
{
	m_nLeftSpan = nLeftSpan;
	m_nRightSpan = nRightSpan;
	m_nTopSpan = nTopSpan;
	m_nBottomSpan = nBottomSpan;

	m_rectWork.left = m_rectClient.left + m_nLeftSpan;
	m_rectWork.top = m_rectClient.top + m_nTopSpan + m_nTitleHeight;
	m_rectWork.right = m_rectClient.right - m_nRightSpan;
	m_rectWork.bottom = m_rectClient.bottom - m_nBottomSpan;

	return TRUE;
}

void CWndListCtrl::SetBKColor(COLORREF color)
{
	m_colorBackGround = color;
	Draw();
}

void CWndListCtrl::SetSelectedItemBKColor(COLORREF colorBk, COLORREF colorBorder)
{
	m_colorItemSelectedBk = colorBk;

	if (colorBorder == INVALID__UICOLOR)
	{
		m_colorItemSelectedBorder = colorBk;
	}
	else
	{
		m_colorItemSelectedBorder = colorBorder;
	}
	
}

void CWndListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	this->SetFocus();
	CWnd::OnLButtonDblClk(nFlags, point);

	int nItem;
	int nSubItem;
	if ( (( nItem = HitTestEx( point, &nSubItem )) != -1) ) {

		CellInfo* pCellInfo = GetCellInfo(nItem, nSubItem);
		if (pCellInfo)
		{
			SendMessageToParent(WM_WND_LIST_ITEM_DOUBLE_CLICK, (WPARAM)m_hWnd, (LPARAM)NULL);
		}
	}
}

void CWndListCtrl::SetScrollBarImage(Image* pImage)
{
	if (pImage)
	{
		m_pImageScrollBar = pImage->Clone();
	}
	else
	{
		m_pImageScrollBar = NULL;
	}
	
}

void CWndListCtrl::SetScrollBarSize(int nWidth, int nHeight)
{
	m_nScrollWidth = nWidth;
	m_nScrollHeight = nHeight;
}

void CWndListCtrl::DoPaintPrettyText(HDC hDC, RECT& rc, LPCTSTR pstrText, COLORREF iTextColor, COLORREF iBackColor, RECT* prcLinks, int& nLinkRects, int& nNeedHeight, int nMinHeight, UINT uStyle)
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

	// The string formatter supports a kind of "mini-html" that consists of various short tags:
	//
	//   Link:             <a>text</a>
	//   Change font:      <f x>        where x = font id
	//   Bold:             <b>text</b>
	//   Indent:           <x i>        where i = indent in pixels
	//   Paragraph:        <p>
	//   Horizontal line:  <h>
	//   Icon:             <i x y>      where x = icon id and (optional) y = size (16/32/50)
	//                     <i x>        where x = icon resource name
	//   Color:            <c #xxxxxx>  where x = RGB in hex
	//                     <c x>        where x = color id
	//
	// In addition the standard string resource formats apply:
	//
	//   %{n}                           where n = resource-string-id
	//

	if( ::IsRectEmpty(&rc) ) return;

	bool bDraw = (uStyle & DT_CALCRECT) == 0;

	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	HRGN hRgn = ::CreateRectRgnIndirect(&rc);
	if( bDraw ) ::ExtSelectClipRgn(hDC, hRgn, RGN_AND);

	CStdString sText = pstrText;
	sText.ProcessResourceTokens();
	pstrText = sText;

	HFONT hOldFont = (HFONT) ::SelectObject(hDC, m_NormalFont);
	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, iTextColor);

	// If the drawstyle includes an alignment, we'll need to first determine the text-size so
	// we can draw it at the correct position...
	if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_VCENTER) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
		RECT rcText = { 0, 0, 9999, 100 };
		int nLinks = 0;
		DoPaintPrettyText(hDC, rcText, pstrText, iTextColor, iBackColor, NULL, nLinks, nNeedHeight, nMinHeight, uStyle | DT_CALCRECT);
		rc.top = rc.top + ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2);
		rc.bottom = rc.top + (rcText.bottom - rcText.top);
	}
	if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_CENTER) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
		RECT rcText = { 0, 0, 9999, 100 };
		int nLinks = 0;
		DoPaintPrettyText(hDC, rcText, pstrText, iTextColor, iBackColor, NULL, nLinks, nNeedHeight, nMinHeight, uStyle | DT_CALCRECT);
		::OffsetRect(&rc, (rc.right - rc.left) / 2 - (rcText.right - rcText.left) / 2, 0);
	}
	if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_RIGHT) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
		RECT rcText = { 0, 0, 9999, 100 };
		int nLinks = 0;
		DoPaintPrettyText(hDC, rcText, pstrText, iTextColor, iBackColor, NULL, nLinks, nNeedHeight, nMinHeight, uStyle | DT_CALCRECT);
		rc.left = rc.right - (rcText.right - rcText.left);
	}

	// Paint backhground
	if( iBackColor != INVALID__UICOLOR ) 
	{
		::SetBkColor(hDC, iBackColor);
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}


	// Determine if we're hovering over a link, because we would like to
	// indicate it by coloring the link text.
	// BUG: This assumes that the prcLink has already been filled once with
	//      link coordinates! That is usually not the case at first repaint. We'll clear
	//      the remanining entries at exit.
	int i;
	bool bHoverLink = false;
	POINT ptMouse = GetLastMousePos();
	for( i = 0; !bHoverLink && i < nLinkRects; i++ ) {
		if( ::PtInRect(prcLinks + i, ptMouse) ) bHoverLink = true;
	}

	nNeedHeight = nMinHeight;

	TEXTMETRIC tm = GetThemeFontInfo(m_NormalFont);

	POINT pt = { rc.left, rc.top + (nMinHeight - tm.tmHeight)/ 2 };
	if ((uStyle & DT_SINGLELINE) != 0)
	{
		 pt.x  =  rc.left;
		 pt.y =  rc.top;
	}
	int iLineIndent = 0;
	int iLinkIndex = 0;
	int cyLine = tm.tmHeight + tm.tmExternalLeading;
	int cyMinHeight = 0;
	POINT ptLinkStart = { 0 };
	bool bInLink = false;

	while( *pstrText != '\0' ) 
	{
		if( pt.x >= rc.right || *pstrText == '\n' ) 
		{
			// A new link was detected/requested. We'll adjust the line height
			// for the next line and expand the link hitbox (if any)
			if( bInLink && iLinkIndex < nLinkRects) ::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, pt.x, pt.y + tm.tmHeight);
			if( (uStyle & DT_SINGLELINE) != 0 ) break;
			if( *pstrText == '\n' ) pstrText++;

			if (pt.y + nMinHeight > nNeedHeight)
			{
				nNeedHeight += nMinHeight;
			}
			
			pt.x = rc.left + iLineIndent;
			pt.y += nMinHeight;
			ptLinkStart = pt;
			cyLine = tm.tmHeight + tm.tmExternalLeading;
			if( pt.x >= rc.right ) break;
			while( *pstrText == ' ' ) pstrText++;
		}
		else if( *pstrText == '<' 
			&& (pstrText[1] >= 'a' && pstrText[1] <= 'z')
			&& (pstrText[2] == ' ' || pstrText[2] == '>') )
		{
			pstrText++;
			switch( *pstrText++ )
			{
			case 'a':  // Link
				{
				
					::SetTextColor(hDC, iTextColor);
					if (bHoverLink)
					{	 SetCursor(::LoadCursor(NULL,IDC_HAND));
						::SelectObject(hDC, m_LinkerFont);
					}

					tm = GetThemeFontInfo(m_LinkerFont);
					cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading);
					ptLinkStart = pt;
					bInLink = true;
				}
				break;
			case 'b':  // Bold text
				{
					::SelectObject(hDC, m_BoldFont);
					tm = GetThemeFontInfo(m_BoldFont);
					cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading);
				}
				break;
			case 'x':  // Indent
				{
					iLineIndent = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
					if( pt.x < rc.left + iLineIndent ) pt.x = rc.left + iLineIndent;
				}
				break;
			case 'p':  // Paragraph
				{
					pt.x = rc.right;
					cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading) + 5;
					iLineIndent = 0;
					::SelectObject(hDC, m_NormalFont);
					::SetTextColor(hDC, iTextColor);
					tm = GetThemeFontInfo(m_NormalFont);
				}
				break;
			case 'i':  // Icon
				{
					int iSize = 16;
					if( *pstrText == ' ' ) pstrText++;
					if( isdigit(*pstrText) ) {
						int iIndex = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						iSize = MAX(16, _ttoi(pstrText));
						if( bDraw && m_pImageCommonIcon != NULL) {
							int nH = m_pImageCommonIcon->GetHeight();

							Graphics graph(hDC);
							RectF rfDest(PointF((REAL)pt.x, (REAL)pt.y), SizeF((REAL)16, (REAL)16));
							graph.DrawImage(m_pImageCommonIcon, rfDest, (REAL)(iIndex * nH), (REAL)0, (REAL)16, (REAL)16, UnitPixel, NULL);
						}
					}
					// A special feature with an icon at the left edge is that it also sets
					// the paragraph indent.
					if( pt.x == rc.left ) iLineIndent = iSize + (iSize / 8); else cyLine = MAX(iSize, cyLine);
					pt.x += iSize + (iSize / 8);
					cyMinHeight = pt.y + iSize;
				}
				break;
			case 'c':  // Color
				{
					if( *pstrText == ' ' ) pstrText++;
					if( *pstrText == '#') {
						pstrText++;
						COLORREF clrColor = _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 16);
						clrColor = RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor));
						::SetTextColor(hDC, clrColor);
					}
				}
				break;
			}
			while( *pstrText != '\0' && *pstrText != '>' ) pstrText++;
			pstrText++;
		}
		else if( *pstrText == '<' && pstrText[1] == '/' )
		{
			pstrText += 2;
			switch( *pstrText++ )
			{
			case 'a':
				if( iLinkIndex < nLinkRects ) ::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, pt.x, pt.y + tm.tmHeight + tm.tmExternalLeading);
				::SetTextColor(hDC, iTextColor);
				::SelectObject(hDC, m_NormalFont);
				tm = GetThemeFontInfo(m_NormalFont);
				bInLink = false;
				break;
			case 'b':
				// TODO: Use a context stack instead
				::SelectObject(hDC, m_NormalFont);
				tm = GetThemeFontInfo(m_NormalFont);
				break;
			case 'c':
				::SetTextColor(hDC, iTextColor);
				break;
			}
			while( *pstrText != '\0' && *pstrText != '>' ) pstrText++;
			pstrText++;
		}
		else if( *pstrText == '&' )
		{
			if( (uStyle & DT_NOPREFIX) == 0 ) {
				if( bDraw  && TRUE ) ::TextOut(hDC, pt.x, pt.y, _T("_"), 1);
			}
			else {
				SIZE szChar = { 0 };
				::GetTextExtentPoint32(hDC, _T("&"), 1, &szChar);
				if( bDraw ) ::TextOut(hDC, pt.x, pt.y, _T("&"), 1);
				pt.x += szChar.cx;
			}
			pstrText++;
		}
		else if( *pstrText == ' ' )
		{
			SIZE szSpace = { 0 };
			::GetTextExtentPoint32(hDC, _T(" "), 1, &szSpace);
			// Still need to paint the space because the font might have
			// underline formatting.
			if( bDraw ) ::TextOut(hDC, pt.x, pt.y, _T(" "), 1);
			pt.x += szSpace.cx;
			pstrText++;
		}
		else
		{
			POINT ptPos = pt;
			int cchChars = 0;
			int cchLastGoodWord = 0;
			LPCTSTR p = pstrText;
			SIZE szText = { 0 };
			if( *p == '<' ) p++, cchChars++;
			while( *p != '\0' && *p != '<' && *p != '\n' && *p != '&' ) {
				// This part makes sure that we're word-wrapping if needed or providing support
				// for DT_END_ELLIPSIS. Unfortunately the GetTextExtentPoint32() call is pretty
				// slow when repeated so often.
				// TODO: Rewrite and use GetTextExtentExPoint() instead!
				cchChars++;
				szText.cx = cchChars * tm.tmMaxCharWidth;
				if( pt.x + szText.cx >= rc.right) {
					::GetTextExtentPoint32(hDC, pstrText, cchChars, &szText);
				}
				if( pt.x + szText.cx >= rc.right) {
					if( (uStyle & DT_WORDBREAK) != 0 && cchLastGoodWord > 0 ) {
						cchChars = cchLastGoodWord;
						pt.x = rc.right;
					}
					if( (uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 2 ) {
						cchChars -= 2;
						pt.x = rc.right;
					}

					if (pt.x + szText.cx > rc.right)
					{
						cchChars--;
						pt.x = rc.right;
					}

					break;
				}
				if( *p == ' ' ) cchLastGoodWord = cchChars;
				p = ::CharNext(p);
			}
			if( cchChars > 0 ) {
				::GetTextExtentPoint32(hDC, pstrText, cchChars, &szText);
				if( bDraw ) {
					::TextOut(hDC, ptPos.x, ptPos.y, pstrText, cchChars);
					if( pt.x == rc.right && (uStyle & DT_END_ELLIPSIS) != 0 )
					{
						::TextOut(hDC, rc.right - 10, ptPos.y, _T("..."), 3);
					}
				}
				pt.x += szText.cx;
				pstrText += cchChars;
			}
		}
		ASSERT(iLinkIndex<=nLinkRects);
	}

	// Clear remaining link rects and return number of used rects
	for( i = iLinkIndex; i < nLinkRects; i++ ) ::ZeroMemory(prcLinks + i, sizeof(RECT));
	nLinkRects = iLinkIndex;

	// Return size of text when requested
	if( (uStyle & DT_CALCRECT) != 0 ) {
		rc.bottom = MAX(cyMinHeight, pt.y + cyLine);
		if( rc.right >= 9999 ) {
			if( _tcslen(pstrText) > 0 ) pt.x += 3;
			rc.right = pt.x;
		}
	}

	if( bDraw ) ::SelectClipRgn(hDC, hOldRgn);
	::DeleteObject(hOldRgn);
	::DeleteObject(hRgn);

	::SelectObject(hDC, hOldFont);
}

const TEXTMETRIC& CWndListCtrl::GetThemeFontInfo(HFONT font) const
{
	TEXTMETRIC tm;
	
	HFONT hOldFont = (HFONT) ::SelectObject(m_MemDC.m_hDC,font);
	::GetTextMetrics(m_MemDC.m_hDC, &tm);
	::SelectObject(m_MemDC.m_hDC, hOldFont);
	
	return tm;
}

POINT CWndListCtrl::GetLastMousePos()
{
	return m_ptLastMouse;
}

void CWndListCtrl::SetCommonIconImage(Image* pImage)
{
	if (pImage)
	{
		m_pImageCommonIcon = pImage->Clone();
	}
	else
	{
		m_pImageCommonIcon = NULL;
	}
	
}

void CWndListCtrl::SetCheckBoxImage(Image* pImage)
{
	if (pImage)
	{
		m_pImageCheckBox = pImage->Clone();
	}
	else
	{
		m_pImageCheckBox = NULL;
	}
	
}

void CWndListCtrl::DoFillRect(HDC hDC, RECT rc, RECT rcImage, Image* pImage)
{
	Graphics graph(hDC);

	if (pImage == NULL)
	{
		return;
	}

	int nWidth = (rcImage.right - rcImage.left) / 3;
	int nHeight = (rcImage.bottom - rcImage.top) / 3;

	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			RectF rfDest;
			RectF rfScr;

			if (x == 2)
			{
				rfDest.X = (REAL)(rc.right - nWidth);
				rfScr.X =  (REAL)(rcImage.right - nWidth);
			}
			else
			{
				rfDest.X = (REAL)(rc.left + nWidth * x);
				rfScr.X = (REAL)(rcImage.left + nWidth * x);
			}

			if (y == 2)
			{
				rfDest.Y = (REAL)(rc.bottom - nHeight);

				rfScr.Y = (REAL)(rcImage.bottom - nHeight);
			}
			else
			{
				rfDest.Y = (REAL)(rc.top + nHeight * y);

				rfScr.Y = (REAL)(rcImage.top + nHeight * y);
			}

			if (x == 1)
			{
				rfDest.Width = (REAL)(rc.right - rc.left - nWidth * 2);

				rfScr.Width = (REAL)(rcImage.right - rcImage.left - nWidth * 2);
			}
			else
			{
				rfDest.Width = (REAL)(nWidth);

				rfScr.Width = (REAL)(nWidth);
			}

			if (y == 1)
			{
				rfDest.Height = (REAL)(rc.bottom - rc.top - nHeight * 2);

				rfScr.Height = (REAL)(rcImage.bottom - rcImage.top - nHeight * 2);
			}
			else
			{
				rfDest.Height = (REAL)(nHeight);

				rfScr.Height = (REAL)(nHeight);
			}


			TextureBrush brush(pImage, WrapModeTile, rfScr);
			brush.TranslateTransform( rfDest.X , rfDest.Y ); 
			graph.FillRectangle(&brush, rfDest);

			// 不用刷子，直接绘图
			//ImageAttributes ImgAtt;
			//ImgAtt.SetWrapMode(WrapModeTileFlipXY);

			//graph.DrawImage(pImage, rfDest, rfScr.X , rfScr.Y, rfScr.Width, rfScr.Height, UnitPixel, &ImgAtt);
		}
	}
}

void CWndListCtrl::SetBackgroundImage(Image* pImage)
{
	if (pImage)
	{
		m_pImageBackground = pImage->Clone();
	}
	else
	{
		m_pImageBackground = NULL;
	}
	
}

void CWndListCtrl::SetSelectedBkImage(Image* pImage)
{
	if (pImage)
	{
		m_pImageSelectedBK = pImage->Clone();
	}
	else
	{
		m_pImageSelectedBK = NULL;
	}
	
}

void CWndListCtrl::SetShowScrollBarAlway(BOOL bAlway)
{
	m_bShowScrollBarAlway = bAlway;
}

HBRUSH CWndListCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
}

void CWndListCtrl::SetButtonImage(Image* pImage, int nStatusCount)
{
	if (pImage)
	{
		m_pImageButton = pImage->Clone();
	}
	else
	{
		m_pImageButton = NULL;
	}
	
	m_nBtnStatusCount = nStatusCount;
}

void CWndListCtrl::DoPaintBorder(HDC hDC, RECT rcItem, COLORREF colorInside, COLORREF colorOutside)
{
	if (colorOutside != INVALID__UICOLOR)
	{
		POINT ptTemp;
		HPEN pen = ::CreatePen(PS_SOLID, 1, colorOutside);
		HPEN oldPen = (HPEN)::SelectObject(hDC, pen);
		::MoveToEx(hDC, rcItem.left, rcItem.bottom - 1, &ptTemp);
		::LineTo(hDC, rcItem.left, rcItem.top);
		::LineTo(hDC, rcItem.right, rcItem.top);
		::LineTo(hDC, rcItem.right, rcItem.bottom);
		::LineTo(hDC, rcItem.left, rcItem.bottom);
		::SelectObject(hDC, oldPen);
	}

	if (colorInside != INVALID__UICOLOR)
	{
		POINT ptTemp;
		HPEN pen = ::CreatePen(PS_SOLID, 1, colorInside);
		HPEN oldPen = (HPEN)::SelectObject(hDC, pen);
		::MoveToEx(hDC, rcItem.left + 1, rcItem.bottom - 1, &ptTemp);
		::LineTo(hDC, rcItem.left + 1, rcItem.top + 1);
		::LineTo(hDC, rcItem.right - 1, rcItem.top + 1);
		::LineTo(hDC, rcItem.right - 1, rcItem.bottom - 1);
		::LineTo(hDC, rcItem.left + 1, rcItem.bottom - 1);
		::SelectObject(hDC, oldPen);
	}

}

void CWndListCtrl::SetImageBorderColor(COLORREF colorOutside, COLORREF colorInside)
{	
	m_colorImageBorderOutside = colorOutside;
	m_colorImageBorderInside = colorInside;
}

void CWndListCtrl::SetItemCountPerRow(int nCount, BOOL bRedraw)
{
	m_nItemCountPerRow = nCount;

	if (bRedraw)
	{
		Draw();
		Invalidate();
	}
}

void CWndListCtrl::DrawGird()
{
	if (m_colorGird == INVALID__UICOLOR)
	{
		return;
	}

	HPEN hPen = ::CreatePen(PS_SOLID, 1, m_colorGird);

	HPEN hOldPen = (HPEN)::SelectObject(m_MemDC.GetSafeHdc(), hPen);

	int colnum = 0;
	
	int nTotal = (int)m_Items.size();

	if (nTotal == 0)
	{
		return;
	}

	// Get the number of columns
	int nColumnCount = GetColumnCount();

	int nCnt = -1;


	POINT pt1 = {m_rectWork.left, m_rectWork.top};
	POINT pt2 = {m_rectWork.right, m_rectWork.top};

	int nItemWindth = (m_rectWork.right - m_rectWork.left) / m_nItemCountPerRow;
	int nItemHeight = 0;
	while (pt1.y <= m_rectWork.bottom)
	{
		nCnt++;
		ItemInfo* pItemInfo = GetItemInfo(GetTopIndex() + nCnt);
		if (pItemInfo != NULL)
		{
			if (nCnt % m_nItemCountPerRow != 0)
			{
				nItemHeight = max(pItemInfo->m_nItemHeight, nItemHeight);
				continue;
			}
			else
			{
				nItemHeight = pItemInfo->m_nItemHeight;
				if (nCnt == 0)
				{
					continue;
				}
			}
		}

		pt1.y = pt2.y = pt1.y + nItemHeight;


		POINT ptTemp = { 0 };

		::MoveToEx(m_MemDC.GetSafeHdc(), pt1.x, pt1.y, &ptTemp);
		::LineTo(m_MemDC.GetSafeHdc(), pt2.x, pt2.y);
	}


	pt1.x = pt2.x = m_rectWork.left;
	pt1.y = m_rectClient.top;
	pt2.y = m_rectClient.bottom;

	//{
	//	POINT ptTemp = { 0 };

	//	::MoveToEx(m_MemDC.GetSafeHdc(), pt1.x, pt1.y, &ptTemp);
	//	::LineTo(m_MemDC.GetSafeHdc(), pt2.x, pt2.y);
	//}

	for (int i = 0; i < GetColumnCount(); i ++)
	{
		int nWidth = GetColumnWidth(i);

		pt1.x = pt2.x = pt1.x + nWidth;

		POINT ptTemp = { 0 };

		::MoveToEx(m_MemDC.GetSafeHdc(), pt1.x, pt1.y, &ptTemp);
		::LineTo(m_MemDC.GetSafeHdc(), pt2.x, pt2.y);

	}


	if (hOldPen)
	{
		::SelectObject(m_MemDC.GetSafeHdc(), hOldPen);
	}
	::DeleteObject(hPen);

	return;
}





void CWndListCtrl::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnMouseHover(nFlags, point);

	int nItem;
	int nSubItem;

	if ( (( nItem = HitTestEx( point, &nSubItem )) != -1) ) 
	{
		CellInfo* pCellInfo = GetCellInfo(nItem, nSubItem);
		if (pCellInfo && pCellInfo->m_strToolTip.GetLength() > 0)
		{
			m_bMouseTracking = FALSE;

			m_ToolTip.cbSize = sizeof(TOOLINFO);
			m_ToolTip.uFlags = TTF_IDISHWND | TTF_TRANSPARENT ;//TTF_CENTERTIP
			m_ToolTip.hwnd = m_hWnd;
			m_ToolTip.uId = (UINT) m_hWnd;
			m_ToolTip.hinst = AfxGetInstanceHandle();
			m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR)pCellInfo->m_strToolTip);
			m_ToolTip.rect = CRect(point.x, point.y, point.x + 100, point.y + 20);
			if( m_hwndTooltip == NULL ) {
				m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX , CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWnd, NULL, AfxGetInstanceHandle(), NULL);
				::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
			}
			::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);

			if (m_nTooltipMaxWidth > 0)
			{
				::SendMessage(m_hwndTooltip, TTM_SETMAXTIPWIDTH, 0, m_nTooltipMaxWidth);
			}
			
			::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
		}
	}

}

void CWndListCtrl::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWnd::OnMouseLeave();

	 if( m_hwndTooltip != NULL ) 
	 {
		 ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
	 }

	 m_bMouseTracking = FALSE;
}

void CWndListCtrl::SetFrameColor(COLORREF colorOutside, COLORREF colorInside)
{	
	m_colorFrameOutside = colorOutside;
	m_colorFrameInside = colorInside;
}

void CWndListCtrl::SetTitleHeight(int nHeight)
{
	m_nTitleHeight = nHeight;
	m_rectWork.top = m_rectClient.top + m_nTopSpan + m_nTitleHeight;
}

void CWndListCtrl::SetTitleBkColor(COLORREF colorTop, COLORREF colorBottom)
{
	m_colorTitleTop = colorTop;
	m_colorTitleBottom = colorBottom;
}

void CWndListCtrl::SetGirdColor(COLORREF color)
{
	m_colorGird = color;
}

void CWndListCtrl::SetTooltipMaxWidth(int nWidth)
{
	m_nTooltipMaxWidth = nWidth;
}

void CWndListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch (nChar)
	{
		case VK_LEFT:	  
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			OnMyKeyDown(nChar);
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CWndListCtrl::SetItemSplitImage(UINT id)
{
	CUIPaintManager::SetImage( &m_pImageItemSplit,id);
	m_iItemSplitHeight = m_pImageItemSplit->GetHeight();
}
void CWndListCtrl::SetItemSplitOpen(BOOL bIsOpen)
{
	m_bItemSplitIsOpen=bIsOpen;	
}
BOOL CWndListCtrl::GetItemSplitIsOpen()
{
	return m_bItemSplitIsOpen;
}
void CWndListCtrl::SetItemSplitImageHeight(int _height)
{
	m_iItemSplitHeight=_height;  //
}
void CWndListCtrl::RemoveItemSplitImage()
{
	delete m_pImageItemSplit;
	m_pImageItemSplit=NULL;
}

/////////////////////

bool CWndListCtrl::SetColumnImagePosCustomMode(int nColumn,int xImage1,int xImage2,int xImage3,bool isOpen)  //设置列图标位置 自定义模式。
{
	if (isOpen)
	{
		CustomSetImagePosColumn columnInfo ;
		columnInfo.m_xImage1Offset = xImage1;
		columnInfo.m_xImage2Offset = xImage2;
		columnInfo.m_xImage3Offset = xImage3;
		m_customSetImagePosColumnList[nColumn] = columnInfo;
		
	}
	else
	{
		m_customSetImagePosColumnList.erase(nColumn);
	}
	return true;
}

bool CWndListCtrl::GetCustomImagePos(int nColumn,CustomSetImagePosColumn& pInfo)
{	
	using namespace std;
	//CustomSetImagePosColumn* tempInfo = new CustomSetImagePosColumn;
    std::map<int,CustomSetImagePosColumn>::iterator iter = m_customSetImagePosColumnList.find(nColumn);
	if (m_customSetImagePosColumnList.end() == iter)
	{		
        return false;
	}
	pInfo.m_xImage1Offset=iter->second.m_xImage1Offset;
	pInfo.m_xImage2Offset=iter->second.m_xImage2Offset;
	pInfo.m_xImage3Offset=iter->second.m_xImage3Offset;

	pInfo.m_columnImage1BottomValueAdd = iter->second.m_columnImage1BottomValueAdd;
	pInfo.m_columnImage1TopValueAdd	=  iter->second.m_columnImage1TopValueAdd;
	pInfo.m_columnImage2BottomValueAdd =  iter->second.m_columnImage2BottomValueAdd;
	pInfo.m_columnImage2TopValueAdd	=  iter->second.m_columnImage2TopValueAdd;
	pInfo.m_columnImage3BottomValueAdd   =   iter->second.m_columnImage3BottomValueAdd;
	pInfo.m_columnImage3TopValueAdd	=  iter->second.m_columnImage3TopValueAdd;

	pInfo.m_columnTextBottomValueAdd =	iter->second.m_columnTextBottomValueAdd;
	pInfo.m_columnTextTopValueAdd =	 iter->second.m_columnTextTopValueAdd;
	
	return true;
}

void CWndListCtrl::SetColumnTextTopValueAdd(int nColumn,int _add)
{
	try
	{
		m_customSetImagePosColumnList[nColumn].m_columnTextTopValueAdd = _add;
	}
	catch (CException* e)
	{

	}	
}
void CWndListCtrl::SetColumnTextBottomValueAdd(int nColumn,int _add)
{
	try
	{
		m_customSetImagePosColumnList[nColumn].m_columnTextBottomValueAdd = _add;
	}
	catch (CException* e)
	{

	}	
}

void CWndListCtrl::SetColumnImage1TopValueAdd(int nColumn,int _add)
{
	try
	{
		m_customSetImagePosColumnList[nColumn].m_columnImage1TopValueAdd = _add;
	}
	catch (CException* e)
	{

	}	
}
void CWndListCtrl::SetColumnImage1BottomValueAdd(int nColumn,int _add)
{
	try
	{
		m_customSetImagePosColumnList[nColumn].m_columnImage1BottomValueAdd = _add;
	}
	catch (CException* e)
	{

	}	
}
void CWndListCtrl::SetColumnImage2TopValueAdd(int nColumn,int _add)
{
	try
	{
		m_customSetImagePosColumnList[nColumn].m_columnImage2TopValueAdd = _add;
	}
	catch (CException* e)
	{

	}	
}
void CWndListCtrl::SetColumnImage2BottomValueAdd(int nColumn,int _add)
{
	try
	{
		m_customSetImagePosColumnList[nColumn].m_columnImage2BottomValueAdd = _add;
	}
	catch (CException* e)
	{

	}	
}
void CWndListCtrl::SetColumnImage3TopValueAdd(int nColumn,int _add)
{
	try
	{
		m_customSetImagePosColumnList[nColumn].m_columnImage3TopValueAdd = _add;
	}
	catch (CException* e)
	{

	}	
}
void CWndListCtrl::SetColumnImage3BottomValueAdd(int nColumn,int _add)
{
	try
	{
		m_customSetImagePosColumnList[nColumn].m_columnImage3BottomValueAdd = _add;
	}
	catch (CException* e)
	{

	}	
}
void CWndListCtrl::AdJustItemText_ImagePosAdded(CRect& rcText,CRect& rtImage1,CRect& rtImage2,CRect& rtImage3,CustomSetImagePosColumn info)
{
	rcText.top += info.m_columnTextTopValueAdd;
	rcText.bottom += info.m_columnTextBottomValueAdd;

	rtImage1.top += info.m_columnImage1TopValueAdd;
	rtImage1.bottom += info.m_columnImage1BottomValueAdd;

	rtImage2.top += info.m_columnImage2TopValueAdd;
	rtImage2.bottom += info.m_columnImage2BottomValueAdd;

	rtImage3.top += info.m_columnImage3TopValueAdd;
	rtImage3.bottom += info.m_columnImage3BottomValueAdd;
    
}
bool CWndListCtrl::SetCurrentItemBkColorEnable(bool _bEnable)
{
    m_bCurrentItemBkColorOpened = _bEnable;
	return true;
}
void CWndListCtrl::SetCurrentItemBkColor(COLORREF _color)
{
	m_currentItemBkColor = _color;
}
