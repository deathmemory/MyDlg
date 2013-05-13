#if !defined(AFX_SKINDIALOG__20101025_72F5_1B46_6300_0080AD509054__INCLUDED_)
#define AFX_SKINDIALOG__20101025_72F5_1B46_6300_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class CUIWndLayer;

#define WM_SHOW_LAYER  WM_USER + 567

// CSkinDialog 对话框
class CSkinDialog : public CDialog, public INotifyUI
{
	DECLARE_DYNAMIC(CSkinDialog)

public:
	CSkinDialog();   // 标准构造函数
	CSkinDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	
	virtual ~CSkinDialog();

public:
	CUIButton*			RegiserButton(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUILabel*			RegiserLabel(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUIEdit*			RegiserEdit(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUIComboBox*		RegiserComboBox(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUIProgressBar*		RegiserProgressBar(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUIMenuButton*		RegiserMenuButton(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUILabelLine*		RegiserLine(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUISliderctrl*		RegiserSliderCtrl(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUIList*			RegiserList(UINT nId, CRect rect, LPCTSTR lpszText = NULL);

	CUIVScrollBar*		RegiserVScrollBar(UINT nId, CRect rect, LPCTSTR lpszText = NULL);


	CUIPageSheet*		RegiserPageSheet(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUIGroup*			RegiserGroup(UINT nId, CRect rect, LPCTSTR lpszText = NULL);

	virtual void InitControls() = 0;

	virtual void SetupRegion(int nCorner = 6);

	// 所有的点必须按照顺时针方向添加
	virtual void SetupRegion(std::vector<POINT>& ltPt, std::vector<POINT>& rtPt, std::vector<POINT>& rbPt, std::vector<POINT>& lbPt);

	CUIControl*			FindControl(UINT nId);

	void				SetSkinImage(UINT nNormalImage, UINT nFocusImage, UINT nPushedImage, UINT nDisabledImage, HMODULE hResHandle=NULL);
	void				SetSkinImage(Image* pNormalImage, Image* pFocusImage, Image* pPushedImage, Image* pDisabledImage);

	Image*				GetCommonButtonImage();
	Image*				GetCommonCloseButtonImage();

	static bool SetCommonDialogSkin(UINT nResourceID, HMODULE hResHandle=NULL);
	static bool SetCommonMenuSkin(UINT nResourceID, HMODULE hResHandle=NULL);
	static bool SetCommonComboSkin(UINT nResourceID, HMODULE hResHandle=NULL);
	static bool SetCommonButtonSkin(UINT nResourceID, HMODULE hResHandle=NULL);
	static bool SetCommonCloseButtonSkin(UINT nResourceID, HMODULE hResHandle=NULL);
	static bool SetCommonCheckBoxSkin(UINT nResourceID, HMODULE hResHandle=NULL);
	static bool SetCommonRadioBoxSkin(UINT nResourceID, HMODULE hResHandle=NULL);
	static bool SetCommonVScrollSkin(UINT nResourceID, HMODULE hResHandle=NULL);
	static bool SetCommonIcon(UINT nResourceID, UINT nIndex, UINT nWidth, HMODULE hResHandle=NULL);

	void SetEditBoxBKColor_Normal(COLORREF color);
	void SetEditBoxTextColor_Normal(COLORREF color);

	void SetEditBoxBKColor_Disable(COLORREF color);
	void SetEditBoxTextColor_Disable(COLORREF color);


	void SetEditBoxBoderColor_Normal(COLORREF colorLight, COLORREF colorDark = INVALID__UICOLOR);
	void SetEditBoxBoderColor_Focused(COLORREF colorLight, COLORREF colorDark = INVALID__UICOLOR);
	void SetEditBoxBoderColor_Disabled(COLORREF colorLight, COLORREF colorDark = INVALID__UICOLOR);

	int UIMessageBox(LPCTSTR lpText, LPCTSTR lpCaption = NULL,UINT uType = MB_OK | MB_ICONWARNING);


	LRESULT OnShowLayer(WPARAM wParam, LPARAM lParam);

	// 设置无效状态下，蒙在表面窗口的Alpha值，如果为0， 则不显示该窗口；必须在SetupRegion前调用
	void SetLayerAlphaValue(int nValue);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CUIPaintManager m_pm;
	CDialogLayoutUI* m_UI;

	std::vector<CUIGroup*> m_pGroupList;

	CUIWndLayer* m_pLayer;
	bool m_bPendingMask;

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void MakeMask();

	CUIGroup* FindGroupByRect(RECT rect);

public:
	int m_nAlphaValueOfLayer;
};

#endif // !defined(AFX_SKINDIALOG__20101025_72F5_1B46_6300_0080AD509054__INCLUDED_)
