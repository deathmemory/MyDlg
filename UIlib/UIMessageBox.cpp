
#include "StdAfx.h"
#include "UIMessageBox.h"

/////////////////////////////////////////////////////////////////////////////////////
//
//
#define LWA_COLORKEY 0x00000001         //方式
#define WS_EX_LAYERED 0x00080000 

class CMessageBoxWnd : public CWindowWnd, public INotifyUI
{
	typedef enum
	{
		IDC_BTN_CLOSE,

		IDC_STC_TITLE,

		IDC_STC_ICON,
		IDC_STC_INFO,

		IDC_BTN_OK,
		IDC_BTN_CANCEL,

		IDC_BTN_YES,
		IDC_BTN_NO
	};

public:
	void Init(HWND hWnd, 
		LPCTSTR lpText,
		LPCTSTR lpCaption,
		UINT uType);

	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	CUILabel* RegiserLabel(UINT nId, RECT rect, LPCTSTR lpszText = NULL);
	CUIButton* RegiserButton(UINT nId, RECT rect, LPCTSTR lpszText = NULL);

	virtual void Notify(TNotifyUI& msg);

public:
	CUIPaintManager m_pm;

	CDialogLayoutUI* m_pWindow;
	
	HWND			m_hwndParent;
	CStdString		m_sCaption;
	CStdString		m_sText;
	UINT			m_uType;

	HICON			m_hIcon;

	static int		m_nRet;			

};

/////////////////////////////////////////////////////////////////////////////////////
//
//
int CMessageBoxWnd::m_nRet = 0;
void CMessageBoxWnd::Init(HWND hWnd, 
						  LPCTSTR lpText,
						  LPCTSTR lpCaption,
						  UINT uType)
{
   m_hwndParent = hWnd;
   m_sCaption = lpCaption;
   m_sText= lpText;
   m_uType = uType;

   // Position the popup window in absolute space
 
   RECT rc = {0, 0, 250 ,250};
  
   MapWindowRect(m_hwndParent, HWND_DESKTOP, &rc);
   Create(m_hwndParent, NULL, WS_POPUP, WS_EX_WINDOWEDGE, rc);

   ::ShowWindow(m_hWnd, SW_SHOW);
   ::SendMessage(m_hwndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CMessageBoxWnd::GetWindowClassName() const
{
   return _T("MessageBoxWnd");
}

void CMessageBoxWnd::OnFinalMessage(HWND hWnd)
{
}

LRESULT CMessageBoxWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if( uMsg == WM_CREATE ) 
   {   
      m_pm.Init(m_hWnd);

      m_pWindow = new CDialogLayoutUI;
	  m_pm.AttachDialog(m_pWindow);
	  m_pm.AddNotifier(this);

	  //关闭按钮的大小
	  int nCloseBtnWidth = m_pm.m_pImageSkinCloseButton->GetWidth() / 4;
	  int nCloseBtnHeight = m_pm.m_pImageSkinCloseButton->GetHeight();
     
	  int nLinks = 0;
	  RECT rcTitle = { 0, 0, 9999, 100 };
	  CUIBlueRenderEngine::DoPaintPrettyText(m_pm.GetPaintDC(), &m_pm, rcTitle, m_sText, UICOLOR_CONTROL_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_VCENTER | DT_CALCRECT);

	  RECT rcInfo = { 0, 0, 9999, 100 };
	  CUIBlueRenderEngine::DoPaintPrettyText(m_pm.GetPaintDC(), &m_pm, rcInfo, m_sText, UICOLOR_CONTROL_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks,  DT_CALCRECT);

	  //图标的大小
	  int liIconWidth=0;
	  int liIconHeight=0;
	  Bitmap* lpIconImage = NULL;

	  if ((m_uType & 0X000000F0) == MB_ICONWARNING)
	  {
		  ExtractIconEx(TEXT("user32.dll"), 1, &m_hIcon, NULL, 1);
	  }
	  else if ((m_uType & 0X000000F0) ==  MB_ICONEXCLAMATION)
	  {
		  ExtractIconEx(TEXT("user32.dll"), 4, &m_hIcon, NULL, 1);
	  }

	  else if ((m_uType & 0X000000F0) == MB_ICONQUESTION)
	  {
		  ExtractIconEx(TEXT("user32.dll"), 2, &m_hIcon, NULL, 1);
	  }
	  else if ((m_uType & 0X000000F0) == MB_ICONERROR)
	  {
		  ExtractIconEx(TEXT("user32.dll"), 3, &m_hIcon, NULL, 1);
	  }
	  else
	  {
		  ExtractIconEx(TEXT("user32.dll"), 4, &m_hIcon, NULL, 1);
	  }
	  ICONINFO icInfo = { 0 };
	  if (::GetIconInfo(m_hIcon, &icInfo))
	  {
		  BITMAP bitmap; 
		  GetObject(icInfo.hbmColor, sizeof(BITMAP), &bitmap);

		  Bitmap* pBitmap = NULL;
		  Bitmap* pWrapBitmap = NULL;
		  if (bitmap.bmBitsPixel != 32)
		  {   
			  pBitmap = Bitmap::FromHICON(m_hIcon);
		  } 
		  else
		  {
			  pWrapBitmap = Bitmap::FromHBITMAP(icInfo.hbmColor, NULL);
			  BitmapData bitmapData;
			  Rect rcImage(0,0, pWrapBitmap->GetWidth(), pWrapBitmap->GetHeight());
			  pWrapBitmap->LockBits(&rcImage, ImageLockModeRead, pWrapBitmap->GetPixelFormat(), &bitmapData); 

			  pBitmap = new Bitmap(bitmapData.Width, bitmapData.Height, bitmapData.Stride,  PixelFormat32bppARGB, (BYTE*)bitmapData.Scan0);

			  pWrapBitmap->UnlockBits(&bitmapData);
		  }

		  lpIconImage = pBitmap->Clone(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight(), PixelFormat32bppARGB);

		  DeleteObject(icInfo.hbmColor); 
		  DeleteObject(icInfo.hbmMask); 
	  }
	  if(lpIconImage != NULL)
	  {
		  liIconWidth=lpIconImage->GetWidth();
		  liIconHeight=lpIconImage->GetHeight();
	  }
	  //计算图口的大小
	  int nWinWidth= rcInfo.right - rcInfo.left + CUIMessageBox::m_nSpaceSpan*3+liIconWidth;
	  int nWinHeight = CUIMessageBox::m_nTitleBarHeight+CUIMessageBox::m_nSpaceSpan*3+CUIMessageBox::m_nCommBtnHeight+liIconHeight;

	  int liMinWidth=CUIMessageBox::m_nSpaceSpan*3+CUIMessageBox::m_nCommBtnWidth*2;
	  int liMinHeight=CUIMessageBox::m_nTitleBarHeight+CUIMessageBox::m_nSpaceSpan*3+CUIMessageBox::m_nCommBtnHeight+rcInfo.bottom-rcInfo.top;
		
	  nWinWidth=nWinWidth < liMinWidth ? liMinWidth:nWinWidth;
	  nWinHeight=nWinHeight < liMinHeight ? liMinHeight:nWinHeight;

	  ::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, nWinWidth, nWinHeight, SWP_NOZORDER|SWP_NOMOVE);

	  {
		  int nSpan = CUIMessageBox::m_nCornerSpan;

			POINT pt[9];

			pt[0].x = nSpan;
			pt[0].y = 0;

			pt[1].x = nWinWidth - nSpan - 1;
			pt[1].y = 0;

			pt[2].x = nWinWidth;
			pt[2].y = nSpan;

			pt[3].x = nWinWidth;
			pt[3].y = nWinHeight - nSpan - 1;

			pt[4].x = nWinWidth - nSpan - 1;
			pt[4].y = nWinHeight;

			pt[5].x = nSpan;
			pt[5].y = nWinHeight;

			pt[6].x = 0;
			pt[6].y = nWinHeight - nSpan - 1;

			pt[7].x = 0;
			pt[7].y = nSpan;

			pt[8].x = nSpan;
			pt[8].y = 0;

			HRGN rgn = CreatePolygonRgn(pt, 9, ALTERNATE );

			SetWindowRgn(m_hWnd, rgn, TRUE);
	  }

	  {
		  //COLORREF maskColor=RGB(255,255,255);
		  //SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd,GWL_EXSTYLE) | WS_EX_LAYERED);
		  //SetLayeredWindowAttributes(m_hWnd,  maskColor, 100, LWA_COLORKEY);
	  }

	  CenterWindow();

	  {
		   RECT rect = {0, 0, nWinWidth , nWinHeight};
		   m_pWindow->SetPos(rect);
	  }

	  // 添加标题
	  {
		  RECT rectTitle;

		  if (CUIMessageBox::m_nTitlePosX == 0 && CUIMessageBox::m_nTitlePosY == 0 )
		  {
			  RECT rect ={nCloseBtnWidth, 
				  CUIMessageBox::m_nCloseBtnPosToTopY,
				  nWinWidth -nCloseBtnWidth -  CUIMessageBox::m_nCloseBtnPosToRightX, 
				  CUIMessageBox::m_nCloseBtnPosToTopY + nCloseBtnHeight
			  };

			  rectTitle = rect;
		  }
		  else
		  {
			  RECT rect ={	CUIMessageBox::m_nTitlePosX, 
							CUIMessageBox::m_nTitlePosY,
							nWinWidth -nCloseBtnWidth -  CUIMessageBox::m_nCloseBtnPosToRightX, 
							CUIMessageBox::m_nTitlePosY + nCloseBtnHeight
			  };

			  rectTitle = rect;
		  }

		  CUILabel* pLabel = RegiserLabel(IDC_STC_TITLE, rectTitle, m_sCaption);
		  pLabel->SetTextStyle(CUIMessageBox::m_nTitleTextStyle);
		  pLabel->SetTextColor(CUIMessageBox::m_colorTitleText);
	  }

	  // 关闭按钮
	  {
		  RECT rect = {nWinWidth - nCloseBtnWidth - CUIMessageBox::m_nCloseBtnPosToRightX, 
						CUIMessageBox::m_nCloseBtnPosToTopY, 
						nWinWidth - CUIMessageBox::m_nCloseBtnPosToRightX,
						CUIMessageBox::m_nCloseBtnPosToTopY + nCloseBtnHeight
					};
		  CUIButton* pBtn = RegiserButton(IDC_BTN_CLOSE, rect);
		  if (pBtn)
		  {
			  pBtn->SetImage(m_pm.m_pImageSkinCloseButton);
		  }
	  }

	  // 添加图标
	  int liIconX=CUIMessageBox::m_nSpaceSpan;
	  int liIconY=CUIMessageBox::m_nTitleBarHeight+CUIMessageBox::m_nSpaceSpan;
	  //------------------------------------------------------------------------------------
	  {
		  if(lpIconImage != NULL)
		  {
			  RECT rect = {liIconX, liIconY, liIconX + liIconWidth , liIconY + liIconHeight};
			  CUILabel* pLabel = RegiserLabel(IDC_STC_ICON, rect);
			  pLabel->SetImage(lpIconImage);
		  }
	  }
	  //------------------------------------------------------------------------------------
	  // 添加文字
	  {
		  RECT rect = {liIconX+liIconWidth+CUIMessageBox::m_nSpaceSpan, liIconY+5, nWinWidth-CUIMessageBox::m_nSpaceSpan , liIconY+5+rcInfo.bottom-rcInfo.top+2};
		  CUILabel* pLabel = RegiserLabel(IDC_STC_INFO, rect, m_sText);
		  pLabel->SetTextColor(CUIMessageBox::m_colorInfoText);
		  pLabel->SetTextStyle(DT_LEFT);
	  }
	  //添加普通按钮
	  //------------------------------------------------------------------------------------
	  {
		  int nBtnWidth = CUIMessageBox::m_nCommBtnWidth;
		  int nBtnHeight = CUIMessageBox::m_nCommBtnHeight;
		  int nBtnSpan = CUIMessageBox::m_nCommBtnSpan;
		  int nY = nWinHeight-CUIMessageBox::m_nSpaceSpan-CUIMessageBox::m_nCommBtnHeight;

		  if ((m_uType & 0X0000000F) == MB_OK)
		  {
			  int nX = (nWinWidth - nBtnWidth)/ 2;
			 
			  RECT rect = {nX, nY, nX + nBtnWidth , nY + nBtnHeight};

			  CUIButton* pBtn = RegiserButton(IDC_BTN_OK, rect, _T("确定"));
			  if (pBtn)
			  {
				  pBtn->SetName(_T("ok"));
				  pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
					  CUIMessageBox::m_colorBtnTextFocused,
					  CUIMessageBox::m_colorBtnTextPushed,
					  CUIMessageBox::m_colorBtnTextDisabled);

			  }
		  }
		  else if ((m_uType & 0X0000000F) ==  MB_OKCANCEL)
		  {
			  {
				  int nX = (nWinWidth - nBtnWidth * 2 - nBtnSpan)/ 2;
				  RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				  CUIButton* pBtn = RegiserButton(IDC_BTN_OK, rect, _T("确定"));
				  if (pBtn)
				  {
					  pBtn->SetName(_T("ok"));
					  pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						  CUIMessageBox::m_colorBtnTextFocused,
						  CUIMessageBox::m_colorBtnTextPushed,
						  CUIMessageBox::m_colorBtnTextDisabled);
				  }
			  }

			  {
				  int nX = (nWinWidth - nBtnWidth * 2 - nBtnSpan)/ 2 + nBtnSpan + nBtnWidth;
				  RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				  CUIButton* pBtn = RegiserButton(IDC_BTN_CANCEL, rect, _T("取消"));
				  if (pBtn)
				  {
					  pBtn->SetName(_T("cancel"));
					  pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						  CUIMessageBox::m_colorBtnTextFocused,
						  CUIMessageBox::m_colorBtnTextPushed,
						  CUIMessageBox::m_colorBtnTextDisabled);
				  }
			  }

		  }

		  else if ((m_uType & 0X0000000F) == MB_YESNO)
		  {
			  {
				  int nX = (nWinWidth - nBtnWidth * 2 - nBtnSpan)/ 2;
				  RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				  CUIButton* pBtn = RegiserButton(IDC_BTN_YES, rect, _T("是"));
				  if (pBtn)
				  {
					  pBtn->SetName(_T("ok"));
					  pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						  CUIMessageBox::m_colorBtnTextFocused,
						  CUIMessageBox::m_colorBtnTextPushed,
						  CUIMessageBox::m_colorBtnTextDisabled);
				  }
			  }

			  {
				  int nX = (nWinWidth - nBtnWidth * 2 - nBtnSpan)/ 2 + nBtnSpan + nBtnWidth;
				  RECT rect = {nX, nY, nX + nBtnWidth,  nY + nBtnHeight};

				  CUIButton* pBtn = RegiserButton(IDC_BTN_NO, rect, _T("否"));
				  if (pBtn)
				  {
					  pBtn->SetName(_T("cancel"));
					  pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
						  CUIMessageBox::m_colorBtnTextFocused,
						  CUIMessageBox::m_colorBtnTextPushed,
						  CUIMessageBox::m_colorBtnTextDisabled);
				  }
			  }
		  }
		  else
		  {
			  int nX = (nWinWidth - nBtnWidth)/ 2;
			  RECT rect = {nX, nY, nX + nBtnWidth , nY + nBtnHeight};

			  CUIButton* pBtn = RegiserButton(IDC_BTN_OK, rect, _T("确定"));
			  if (pBtn)
			  {
				  pBtn->SetName(_T("ok"));
				  pBtn->SetTextColor(CUIMessageBox::m_colorBtnTextNormal,
					  CUIMessageBox::m_colorBtnTextFocused,
					  CUIMessageBox::m_colorBtnTextPushed,
					  CUIMessageBox::m_colorBtnTextDisabled);
			  }
		  }
		 
	  }

      return 0;
   }
   else if (uMsg == WM_NCHITTEST)
   {
	   int nCloseBtnWidth = m_pm.m_pImageSkinCloseButton->GetWidth() / 4;
	   RECT rc;
	   ::GetClientRect(m_hWnd, &rc);
	   rc.bottom = rc.top + CUIMessageBox::m_nTitleBarHeight;
	   rc.right = rc.right - nCloseBtnWidth;

	   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	   ::ScreenToClient(m_hWnd, &pt);

	   if (PtInRect(&rc, pt))
	   {
			return HTCAPTION;
	   }
	   else
	   {
		   return HTCLIENT;
	   }
   }
 
   LRESULT lRes = 0;
   if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
   return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CMessageBoxWnd::Notify(TNotifyUI& msg)
{
	if( msg.nType == UI_NOTIFY_CLICK ) 
	{
		if( msg.pSender->GetId() == IDC_BTN_OK )
		{
			Close();
			m_nRet = IDOK;
		}

		if( msg.pSender->GetId() == IDC_BTN_OK )
		{
			Close();
			m_nRet = IDOK;
		}

		if( msg.pSender->GetId() == IDC_BTN_CANCEL )
		{
			Close();
			m_nRet = IDCANCEL;
		}

		if( msg.pSender->GetId() == IDC_BTN_YES )
		{
			Close();
			m_nRet = IDYES;
		}

		if( msg.pSender->GetId() == IDC_BTN_NO )
		{
			Close();
			m_nRet = IDNO;
		}
		if( msg.pSender->GetId() == IDC_BTN_CLOSE )
		{
			Close();
			m_nRet = IDCANCEL;
		}
	}
}

CUILabel* CMessageBoxWnd::RegiserLabel(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CUILabel* pLabel = new CUILabel;
	if (pLabel == NULL)
	{
		return NULL;
	}

	pLabel->SetId(nId);
	pLabel->SetPos(rect);

	CStdString strTemp;
	strTemp.Format(_T("%d"), nId);

	pLabel->SetName(strTemp);

	if (lpszText)
	{
		pLabel->SetText(lpszText);
	}

	m_pWindow->Add(pLabel);

	return pLabel;
}

CUIButton* CMessageBoxWnd::RegiserButton(UINT nId, RECT rect, LPCTSTR lpszText)
{
	CUIButton* pBtn = new CUIButton;
	if (pBtn == NULL)
	{
		return NULL;
	}

	pBtn->SetId(nId);
	pBtn->SetPos(rect);

	CStdString strTemp;
	strTemp.Format(_T("%d"), nId);

	pBtn->SetName(strTemp);

	if (lpszText)
	{
		pBtn->SetText(lpszText);
	}

	m_pWindow->Add(pBtn);

	pBtn->SetImage(m_pm.m_pImageSkinCommonButton);

	return pBtn;
}


////////////////////////////////////////////////////////


int CUIMessageBox::m_nCloseBtnPosToRightX = 0;
int CUIMessageBox::m_nCloseBtnPosToTopY = 0;

int CUIMessageBox::m_nTitlePosX = 0;
int CUIMessageBox::m_nTitlePosY = 0;

int CUIMessageBox::m_nTitleBarHeight=20;
int CUIMessageBox::m_nSpaceSpan=20;

int CUIMessageBox::m_nCommBtnWidth=80;
int CUIMessageBox::m_nCommBtnHeight=20;
int CUIMessageBox::m_nCommBtnSpan=10;

UINT CUIMessageBox::m_nTitleTextStyle = DT_CENTER;

COLORREF CUIMessageBox::m_colorBtnTextNormal=RGB(255, 255, 255);
COLORREF CUIMessageBox::m_colorBtnTextFocused=RGB(255, 255, 255);
COLORREF CUIMessageBox::m_colorBtnTextPushed=RGB(255, 255, 255);
COLORREF CUIMessageBox::m_colorBtnTextDisabled=RGB(255, 255, 255);
COLORREF CUIMessageBox::m_colorInfoText = RGB(255, 255, 255);
COLORREF CUIMessageBox::m_colorTitleText = RGB(255, 255, 255);
int CUIMessageBox::m_nCornerSpan = 5;


void CUIMessageBox::SetCloseBtnPos(int aiXToRight, int aiYToTop)
{
	m_nCloseBtnPosToRightX = aiXToRight;
	m_nCloseBtnPosToTopY = aiYToTop;
}

void CUIMessageBox::SetTitlePos(int x, int y)
{
	m_nTitlePosX = x;
	m_nTitlePosY = y;
}

void CUIMessageBox::SetTitleBarHeight( int aiHeight )
{
	m_nTitleBarHeight=aiHeight;
}
void CUIMessageBox::SetSpaceSpan( int aiSpan )
{
	m_nSpaceSpan=aiSpan;
}
void CUIMessageBox::SetCornerSpan(int nSpan)
{
	m_nCornerSpan = nSpan;
}

void CUIMessageBox::SetTitleTextStyle(UINT nStyle)
{
	m_nTitleTextStyle = nStyle;
}

void CUIMessageBox::SetBtnTextColor(COLORREF crNormal, COLORREF crFocused, COLORREF crPushed, COLORREF crDisabled)
{
	m_colorBtnTextNormal=crNormal;
	m_colorBtnTextFocused=crFocused;
	m_colorBtnTextPushed=crFocused;
	m_colorBtnTextDisabled=crDisabled;
}

void CUIMessageBox::SetTitleTextColor(COLORREF color)
{
	m_colorTitleText = color;
}

void CUIMessageBox::SetInfoTextColor(COLORREF color)
{
	m_colorInfoText = color;
}
void CUIMessageBox::SetCommBtnSize( int aiWidth, int aiHeight )
{
	m_nCommBtnWidth=aiWidth;
	m_nCommBtnHeight=aiHeight;
}

int CUIMessageBox::Show(HWND hWnd, 
				 LPCTSTR lpText,
				 LPCTSTR lpCaption,
				 UINT uType
				 )
{
	//CMessageBoxWnd* pMsgWnd = new CMessageBoxWnd;
	//if (pMsgWnd == NULL)
	//{
	//	return IDOK;
	//}

	//
	//pMsgWnd->Init(hWnd, lpText, lpCaption, uType);

	//pMsgWnd->ShowModal();

	//if (pMsgWnd)
	//{
	////	delete pMsgWnd;//临时关闭
	//}
	CMessageBoxWnd pMsgWnd;							//使用局部函数替换掉指针，临时解决内存泄露问题
	pMsgWnd.Init(hWnd, lpText, lpCaption, uType);
	pMsgWnd.ShowModal();
	int nRetValue = CMessageBoxWnd::m_nRet;

	return nRetValue;
}
int  UIMessageBox(HWND hWnd, 
			 LPCTSTR lpText,
			 LPCTSTR lpCaption,
			 UINT uType
			 )
{
	return CUIMessageBox::Show(hWnd, lpText, lpCaption, uType);
}