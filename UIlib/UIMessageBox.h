#if !defined(AFX_UIMESSAGEBOX_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)
#define AFX_UIMESSAGEBOX_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////////////
//
class CMessageBoxWnd;

class CUIMessageBox
{
	friend CMessageBoxWnd;
private:
	static int m_nCloseBtnPosToRightX;
	static int m_nCloseBtnPosToTopY;
	static UINT m_nTitleTextStyle;
	static COLORREF m_colorBtnTextNormal;
	static COLORREF m_colorBtnTextFocused;
	static COLORREF m_colorBtnTextPushed;
	static COLORREF m_colorBtnTextDisabled;
	static COLORREF m_colorInfoText;
	static COLORREF m_colorTitleText;
	static int m_nCornerSpan;

	static int m_nTitlePosX;
	static int m_nTitlePosY;
	static int m_nTitleBarHeight;
	static int m_nSpaceSpan;
	static int m_nCommBtnWidth;
	static int m_nCommBtnHeight;
	static int m_nCommBtnSpan;
public:
	static void SetCloseBtnPos(int aiXToRight, int aiYToTop);//设置相对于右上角的坐标
	static void SetTitlePos(int x, int y);
	static void SetTitleBarHeight(int aiHeight);
	static void SetSpaceSpan(int aiSpan);
	static void SetCornerSpan(int nSpan);
	static void SetTitleTextStyle(UINT nStyle);
	static void SetBtnTextColor(COLORREF crNormal, COLORREF crFocused, COLORREF crPushed, COLORREF crDisabled);
	static void SetTitleTextColor(COLORREF color);
	static void SetInfoTextColor(COLORREF color);
	static void SetCommBtnSize(int aiWidth, int aiHeight);

	static int Show(HWND hWnd, 
		LPCTSTR lpText,
		LPCTSTR lpCaption = NULL,
		UINT uType = MB_OK | MB_ICONWARNING
		);
};


int UIMessageBox(HWND hWnd, 
			 LPCTSTR lpText,
			 LPCTSTR lpCaption = NULL,
			 UINT uType = MB_OK | MB_ICONWARNING
			 );
//#define  UIMessageBox CUIMessageBox::Show 


#endif // !defined(AFX_UIMESSAGEBOX_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)

