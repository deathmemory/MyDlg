// SkinDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "SkinDialog.h"

#define ID_PENDINGMAKS_TIMER	60000

class CUIWndLayer : public CWindowWnd
{
public:
	CUIWndLayer();

	void Init(HWND hwndParent, const RECT rc, int nAlpha = 80);

	LPCTSTR GetWindowClassName() const;

	void OnFinalMessage(HWND hWnd);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	int m_nAlphaValue;
};


CUIWndLayer::CUIWndLayer()
{
	m_nAlphaValue = 80;
}

void CUIWndLayer::Init(HWND hwndParent, const RECT rc, int nAlpha)
{	
	Create(hwndParent, NULL, WS_POPUP | WS_DISABLED , WS_EX_WINDOWEDGE | WS_EX_LAYERED, rc);
	m_nAlphaValue = nAlpha;
}

LPCTSTR CUIWndLayer::GetWindowClassName() const
{
	return _T("LayerWnd");
}


void CUIWndLayer::OnFinalMessage(HWND /*hWnd*/)
{
	delete this;
}


LRESULT CUIWndLayer::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	bool bHandled = true;

	if (uMsg == WM_CREATE)
	{
		::SetLayeredWindowAttributes(m_hWnd, NULL, m_nAlphaValue, LWA_ALPHA);
	}
	else
	{
		bHandled = false;
	}

	if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

// CSkinDialog 对话框



IMPLEMENT_DYNAMIC(CSkinDialog, CDialog)

CSkinDialog::CSkinDialog()
{
	m_UI = NULL;
	m_pLayer = NULL;
	m_nAlphaValueOfLayer = 80;
	m_bPendingMask = false;
}

CSkinDialog::CSkinDialog(UINT nIDTemplate, CWnd* pParentWnd)
: CDialog(nIDTemplate, pParentWnd)
{
	m_UI = NULL;
	m_pLayer = NULL;
	m_nAlphaValueOfLayer = 80;
}

CSkinDialog::~CSkinDialog()
{

}

void CSkinDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSkinDialog, CDialog)
	ON_MESSAGE(WM_SHOW_LAYER, OnShowLayer)
END_MESSAGE_MAP()

CUIButton* CSkinDialog::RegiserButton(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIButton* pBtn = new CUIButton;
	if (pBtn == NULL)
	{
		return NULL;
	}

	pBtn->SetId(nId);
	pBtn->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pBtn->SetName(strTemp);

	if (lpszText)
	{
		pBtn->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pBtn);
	}
	else
	{
		m_UI->Add(pBtn);
	}

	return pBtn;
}

CUILabel* CSkinDialog::RegiserLabel(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUILabel* pLabel = new CUILabel;
	if (pLabel == NULL)
	{
		return NULL;
	}

	pLabel->SetId(nId);
	pLabel->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pLabel->SetName(strTemp);

	if (lpszText)
	{
		pLabel->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pLabel);
	}
	else
	{
		m_UI->Add(pLabel);
	}

	return pLabel;
}

CUIEdit* CSkinDialog::RegiserEdit(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIEdit* pEdit = new CUIEdit;
	if (pEdit == NULL)
	{
		return NULL;
	}

	pEdit->SetId(nId);
	pEdit->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pEdit->SetName(strTemp);

	if (lpszText)
	{
		pEdit->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pEdit);
	}
	else
	{
		m_UI->Add(pEdit);
	}

	return pEdit;
}

CUIComboBox* CSkinDialog::RegiserComboBox(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIComboBox* pCombo = new CUIComboBox;
	if (pCombo == NULL)
	{
		return NULL;
	}

	pCombo->SetId(nId);
	pCombo->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pCombo->SetName(strTemp);

	if (lpszText)
	{
		pCombo->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pCombo);
	}
	else
	{
		m_UI->Add(pCombo);
	}

	return pCombo;
}

CUIProgressBar* CSkinDialog::RegiserProgressBar(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIProgressBar* pBar = new CUIProgressBar;
	if (pBar == NULL)
	{
		return NULL;
	}

	pBar->SetId(nId);

	pBar->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pBar->SetName(strTemp);

	if (lpszText)
	{
		pBar->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pBar);
	}
	else
	{
		m_UI->Add(pBar);
	}

	return pBar;
}

CUIMenuButton* CSkinDialog::RegiserMenuButton(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIMenuButton* pMenuBtn = new CUIMenuButton;
	if (pMenuBtn == NULL)
	{
		return NULL;
	}

	pMenuBtn->SetId(nId);
	pMenuBtn->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pMenuBtn->SetName(strTemp);

	if (lpszText)
	{
		pMenuBtn->SetText(lpszText);
	}

	m_UI->Add(pMenuBtn);

	return pMenuBtn;
}


CUILabelLine* CSkinDialog::RegiserLine(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUILabelLine* pLabel = new CUILabelLine;
	if (pLabel == NULL)
	{
		return NULL;
	}

	pLabel->SetId(nId);
	pLabel->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pLabel->SetName(strTemp);

	if (lpszText)
	{
		pLabel->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pLabel);
	}
	else
	{
		m_UI->Add(pLabel);
	}


	return pLabel;
}

CUISliderctrl* CSkinDialog::RegiserSliderCtrl(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUISliderctrl* pSlider = new CUISliderctrl;
	if (pSlider == NULL)
	{
		return NULL;
	}

	pSlider->SetId(nId);
	pSlider->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pSlider->SetName(strTemp);

	if (lpszText)
	{
		pSlider->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pSlider);
	}
	else
	{
		m_UI->Add(pSlider);
	}

	return pSlider;
}

CUIList* CSkinDialog::RegiserList(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIList* pList = new CUIList;
	if (pList == NULL)
	{
		return NULL;
	}

	pList->SetId(nId);
	pList->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pList->SetName(strTemp);

	if (lpszText)
	{
		pList->SetText(lpszText);
	}

	
	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pList);
	}
	else
	{
		m_UI->Add(pList);
	}

	

	return pList;
}

CUIVScrollBar* CSkinDialog::RegiserVScrollBar(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIVScrollBar* pScrollBar = new CUIVScrollBar;
	if (pScrollBar == NULL)
	{
		return NULL;
	}

	pScrollBar->SetId(nId);
	pScrollBar->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pScrollBar->SetName(strTemp);

	if (lpszText)
	{
		pScrollBar->SetText(lpszText);
	}


	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pScrollBar);
	}
	else
	{
		m_UI->Add(pScrollBar);
	}

	return pScrollBar;
}


CUIGroup* CSkinDialog::RegiserGroup(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIGroup* pGroup = new CUIGroup;
	if (pGroup == NULL)
	{
		return NULL;
	}

	pGroup->SetId(nId);
	pGroup->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pGroup->SetName(strTemp);

	if (lpszText)
	{
		pGroup->SetText(lpszText);
	}

	m_UI->Add(pGroup);

	m_pGroupList.push_back(pGroup);

	return pGroup;
}

CUIPageSheet* CSkinDialog::RegiserPageSheet(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIPageSheet* pSheet = new CUIPageSheet;
	if (pSheet == NULL)
	{
		return NULL;
	}

	m_UI->Add(pSheet);

	pSheet->SetId(nId);
	pSheet->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pSheet->SetName(strTemp);

	if (lpszText)
	{
		pSheet->SetText(lpszText);
	}

	

	return pSheet;
}

CUIGroup* CSkinDialog::FindGroupByRect(RECT rect)
{
	CUIGroup* pGroup = NULL;

	for (int i = 0; i < (int)m_pGroupList.size(); i++)
	{
		pGroup = m_pGroupList[i];
		if (pGroup)
		{
			RECT rcTemp = { 0 };
			RECT rectGroup = pGroup->GetPos();

			if(::IntersectRect(&rcTemp, &rectGroup, &rect))
			{
				if (rect.left == rcTemp.left && rect.top == rcTemp.top && rect.right == rcTemp.right && rect.bottom == rcTemp.bottom)
				{
					break;
				}
				else
				{
					pGroup = NULL;
				}
			}
			else
			{
				pGroup = NULL;
			}
		}
	}

	return pGroup;
}

CUIControl*	CSkinDialog::FindControl(UINT nId)
{
	if (m_UI == NULL)
	{
		return NULL;
	}

	return m_pm.FindControl(nId);
}



// CSkinDialog 消息处理程序

BOOL CSkinDialog::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CUIPaintManager::SetResourceInstance(AfxGetInstanceHandle());

	m_UI = new CDialogLayoutUI;

	m_pm.Init(m_hWnd);
	m_pm.AttachDialog(m_UI);
	m_pm.AddNotifier(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CSkinDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if(pMsg->message==WM_KEYDOWN
		&&(pMsg->wParam == VK_LEFT 
		|| pMsg->wParam == VK_RIGHT 
		|| pMsg->wParam == VK_UP 
		|| pMsg->wParam == VK_DOWN)
		) 
	{
		return __super::PreTranslateMessage(pMsg);
	} 

	if (!CUIPaintManager::TranslateMessage(pMsg))
	{
		return __super::PreTranslateMessage(pMsg);
	}


	return TRUE;
}

LRESULT CSkinDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	LRESULT lRes = 0;
	if(message == WM_ENABLE)
	{
		::SendMessage(m_hWnd, WM_SHOW_LAYER, NULL, NULL);
	}

	if (message == WM_TIMER)
	{
		if (wParam == ID_PENDINGMAKS_TIMER)
		{
			if (m_bPendingMask)
			{
				MakeMask();
			}
			else
			{
				KillTimer(ID_PENDINGMAKS_TIMER);
			}
		}
	}

	if( m_pm.MessageHandler(message, wParam, lParam, lRes) )
	{
		return lRes;
	}



	return __super::WindowProc(message, wParam, lParam);
}

void CSkinDialog::SetupRegion(int nCorner)
{
	std::vector<POINT> ltPt;
	{
		POINT pt = {0, nCorner};
		ltPt.push_back(pt);
	}
	{
		POINT pt = {nCorner, 0};
		ltPt.push_back(pt);
	}


	std::vector<POINT> rtPt;
	{
		POINT pt = {-nCorner, 0};
		rtPt.push_back(pt);
	}
	{
		POINT pt = {0, nCorner};
		rtPt.push_back(pt);
	}
	

	nCorner = nCorner + 1;
	std::vector<POINT> rbPt;
	{
		POINT pt = {0, -nCorner};
		rbPt.push_back(pt);
	}
	{
		POINT pt = {-nCorner, 0};
		rbPt.push_back(pt);
	}


	std::vector<POINT> lbPt;
	{
		POINT pt = {nCorner, 0};
		lbPt.push_back(pt);
	}
	{
		POINT pt = {0, -nCorner};
		lbPt.push_back(pt);
	}


	SetupRegion(ltPt, rtPt, rbPt, lbPt);
	
}
void CSkinDialog::SetSkinImage( UINT nNormalImage, UINT nFocusImage, UINT nPushedImage, UINT nDisabledImage, HMODULE hResHandle/*=NULL*/ )
{
	m_pm.SetNormalSkin(nNormalImage, hResHandle);
	m_pm.SetFocusSkin(nFocusImage, hResHandle);
	m_pm.SetPushedSkin(nPushedImage, hResHandle);
	m_pm.SetDisabledSkin(nDisabledImage, hResHandle);
}
void CSkinDialog::SetSkinImage(Image* pNormalImage, Image* pFocusImage, Image* pPushedImage, Image* pDisabledImage)
{
	m_pm.SetNormalSkin(pNormalImage);
	m_pm.SetFocusSkin(pFocusImage);
	m_pm.SetPushedSkin(pPushedImage);
	m_pm.SetDisabledSkin(pDisabledImage);
}
Image* CSkinDialog::GetCommonButtonImage()
{
	return m_pm.m_pImageSkinCommonButton;
}

Image* CSkinDialog::GetCommonCloseButtonImage()
{
	return m_pm.m_pImageSkinCloseButton;
}
bool CSkinDialog::SetCommonMenuSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return CUIPaintManager::SetCommonMenuSkin(nResourceID, hResHandle);
}
bool CSkinDialog::SetCommonComboSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return CUIPaintManager::SetCommonComboSkin(nResourceID, hResHandle);
}
bool CSkinDialog::SetCommonDialogSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return CUIPaintManager::SetCommonDialogSkin(nResourceID, hResHandle);
}
bool CSkinDialog::SetCommonButtonSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return CUIPaintManager::SetCommonButtonSkin(nResourceID, hResHandle);
}
bool CSkinDialog::SetCommonCheckBoxSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return CUIPaintManager::SetCommonCheckBoxSkin(nResourceID, hResHandle);
}
bool CSkinDialog::SetCommonRadioBoxSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return CUIPaintManager::SetCommonRadioBoxSkin(nResourceID, hResHandle);
}
bool CSkinDialog::SetCommonVScrollSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return CUIPaintManager::SetCommonVScrollSkin(nResourceID, hResHandle);
}
bool CSkinDialog::SetCommonIcon( UINT nResourceID, UINT nIndex, UINT nWidth, HMODULE hResHandle/*=NULL*/ )
{
	return CUIPaintManager::SetCommonIcon(nResourceID, nIndex, nWidth, hResHandle);
}
bool CSkinDialog::SetCommonCloseButtonSkin( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	return CUIPaintManager::SetCommonCloseButtonSkin(nResourceID, hResHandle);
}
void CSkinDialog::SetEditBoxBKColor_Normal(COLORREF color)
{
	m_pm.SetEditBoxBKColor_Normal(color);
}

void CSkinDialog::SetEditBoxTextColor_Normal(COLORREF color)
{
	m_pm.SetEditBoxTextColor_Normal(color);
}

void CSkinDialog::SetEditBoxBKColor_Disable(COLORREF color)
{
	m_pm.SetEditBoxBKColor_Disable(color);
}
void CSkinDialog::SetEditBoxTextColor_Disable(COLORREF color)
{
	m_pm.SetEditBoxTextColor_Disable(color);
}

int CSkinDialog::UIMessageBox(LPCTSTR lpText, LPCTSTR lpCaption,UINT uType)
{
	CString strCaption;
	if (lpCaption == NULL)
	{
		GetWindowText(strCaption);
	}
	else
	{
		strCaption = lpCaption;
	}
	return CUIMessageBox::Show(m_hWnd, lpText, strCaption, uType);
}

void CSkinDialog::SetEditBoxBoderColor_Normal(COLORREF colorLight, COLORREF colorDark)
{
	m_pm.SetEditBoxBoderColor_Normal(colorLight, colorDark);
}
void CSkinDialog::SetEditBoxBoderColor_Focused(COLORREF colorLight, COLORREF colorDark)
{
	m_pm.SetEditBoxBoderColor_Focused(colorLight, colorDark);
}
void CSkinDialog::SetEditBoxBoderColor_Disabled(COLORREF colorLight, COLORREF colorDark)
{
	m_pm.SetEditBoxBoderColor_Disabled(colorLight, colorDark);
}

void CSkinDialog::SetupRegion(std::vector<POINT>& ltPt, std::vector<POINT>& rtPt, std::vector<POINT>& rbPt, std::vector<POINT>& lbPt)
{
	CRect rectClient;
	GetClientRect(&rectClient);

	int nPtIndex = 0;
	POINT ptStd = {0, 0};

	int nPtCount = lbPt.size() + rtPt.size() + rbPt.size() + lbPt.size();
	if (nPtCount <= 4)
	{
		return;
	}

	POINT* ptArray = new POINT[nPtCount];
	if (ptArray == NULL)
	{
		return;
	}

	ptStd.x = 0;
	ptStd.y = 0;
	for (int i = 0; i < ltPt.size(); i++)
	{
		POINT ptTemp = ltPt[i];

		ptArray[nPtIndex].x = ptStd.x + ptTemp.x;
		ptArray[nPtIndex].y = ptStd.y + ptTemp.y;

		nPtIndex++;
	}

	ptStd.x = rectClient.right;
	ptStd.y = 0;
	for (int i = 0; i < rtPt.size(); i++)
	{
		POINT ptTemp = rtPt[i];

		ptArray[nPtIndex].x = ptStd.x + ptTemp.x;
		ptArray[nPtIndex].y = ptStd.y + ptTemp.y;

		nPtIndex++;
	}

	ptStd.x = rectClient.right;
	ptStd.y = rectClient.bottom;
	for (int i = 0; i < rbPt.size(); i++)
	{
		POINT ptTemp = rbPt[i];

		ptArray[nPtIndex].x = ptStd.x + ptTemp.x;
		ptArray[nPtIndex].y = ptStd.y + ptTemp.y;

		nPtIndex++;
	}

	ptStd.x = 0;
	ptStd.y = rectClient.bottom;
	for (int i = 0; i < lbPt.size(); i++)
	{
		POINT ptTemp = lbPt[i];

		ptArray[nPtIndex].x = ptStd.x + ptTemp.x;
		ptArray[nPtIndex].y = ptStd.y + ptTemp.y;

		nPtIndex++;
	}

	HRGN rgn = CreatePolygonRgn(ptArray, nPtCount, WINDING);

	::SetWindowRgn(m_hWnd, rgn, TRUE);


	if (m_nAlphaValueOfLayer > 0)
	{
		if (m_pLayer != NULL)
		{
			m_pLayer->Close();
			m_pLayer = NULL;
		}

		CRect rectWindow;
		GetWindowRect(&rectWindow);
		m_pLayer = new CUIWndLayer;
		if (m_pLayer)
		{
			m_pLayer->Init(m_hWnd, rectWindow, m_nAlphaValueOfLayer);
		}

		if (m_pLayer)
		{
			if (::IsWindow(m_pLayer->GetHWND()))
			{
				HRGN rgn1 = CreatePolygonRgn(ptArray, nPtCount, WINDING);
				::SetWindowRgn(m_pLayer->GetHWND(), rgn1, TRUE);
			}
		}
	}





	if (ptArray)
	{
		delete[] ptArray;
		ptArray = NULL;
	}
}

void CSkinDialog::MakeMask()
{
	BOOL bEnabled = ::IsWindowEnabled(m_hWnd);

	// 如果是Disabled时，先要检查是否还是当前窗体，如果还是当前窗体，延后蒙版操作
	if (!bEnabled && GetForegroundWindow() == this)
	{
		m_bPendingMask = true;

		SetTimer(ID_PENDINGMAKS_TIMER, 10, NULL);
		
		return;
	}

	m_bPendingMask = false;

	if (!bEnabled && ::IsWindowVisible(m_hWnd) )
	{
		if (m_pLayer)
		{
			
			CRect rectClient;
			GetWindowRect(&rectClient);
			if (::IsWindow(m_pLayer->GetHWND()))
			{
				CWnd *pTopWnd = GetForegroundWindow();
				::MoveWindow(m_pLayer->GetHWND(), rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), FALSE);
				m_pLayer->ShowWindow(true, true);
				if (pTopWnd != NULL && ::IsWindow(pTopWnd->GetSafeHwnd()) && pTopWnd != this)
				{
					pTopWnd->SetForegroundWindow();
				}
			}
	
		}
	}
	else
	{
		if (m_pLayer)
		{
			m_pLayer->ShowWindow(false);;
		}
	}
}

LRESULT CSkinDialog::OnShowLayer(WPARAM wParam, LPARAM lParam)
{
	m_bPendingMask = false;
	KillTimer(ID_PENDINGMAKS_TIMER);

	MakeMask();

	return 0;
}

void CSkinDialog::SetLayerAlphaValue(int nValue)
{
	m_nAlphaValueOfLayer = nValue;
}
