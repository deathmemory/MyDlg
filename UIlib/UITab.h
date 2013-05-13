#if !defined(AFX_UITAB_H__20060218_95D6_2F8B_4F7A_0080AD509054__INCLUDED_)
#define AFX_UITAB_H__20060218_95D6_2F8B_4F7A_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIlist.h"
#include <vector>


/////////////////////////////////////////////////////////////////////////////////////
//
class UILIB_API CUIPageOption : public CUIContainer
{
public:
	CUIPageOption();

	LPCTSTR GetClass() const;

	CUIButton*			RegiserButton(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUILabel*			RegiserLabel(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUIEdit*			RegiserEdit(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUIComboBox*		RegiserComboBox(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUIProgressBar*		RegiserProgressBar(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUISliderctrl*		RegiserSliderCtrl(UINT nId, CRect rect, LPCTSTR lpszText = NULL);
	CUIGroup*			RegiserGroup(UINT nId, CRect rect, LPCTSTR lpszText = NULL);

	CUIGroup*			FindGroupByRect(RECT rect);

	void Event(TEventUI& Event);

protected:
	std::vector<CUIGroup*> m_pGroupList;
};


class UILIB_API CUIPageSheet : public CUIContainer, public IUIListOwner
{
public:
   CUIPageSheet();

   virtual ~CUIPageSheet();

   LPCTSTR GetClass() const;

   void Init();

   bool Add(CUIControl* pControl);

   CUIPageOption* AddPageOption(UINT uPageIdx, PCTSTR pstrText);


   int GetCurSel() const;
   bool SetCurSel(int iIndex);

   void Event(TEventUI& Event);

   void SetPos(RECT rc);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

   bool SetHotItem(int iIndex);
   void ResetSubRects();
   void SetTabLeftOffset(UINT auOffset);
   void SetTabOverBoxPixels(UINT auHPixels);

   void SetTabImage(Image* pImage);
   void SetTabImage(UINT nResourceID, HMODULE hResHandle=NULL);
   void SetTabImage(LPCTSTR pstrName);

   void SetBoxImage(Image* pImage);
   void SetBoxImage(UINT nResourceID, HMODULE hResHandle=NULL);
   void SetBoxImage(LPCTSTR pstrName);

    void SetTabTextColor(COLORREF crNormal=RGB(0,0,0), COLORREF crFocused=RGB(0,0,20), COLORREF crPushed=RGB(0,0,20), COLORREF crDisabled=RGB(128,128,128));

   void SetBorderColor(COLORREF colorOutside, COLORREF colorInside = INVALID__UICOLOR);
   void SetBackGroundColor(COLORREF color);

protected:
   int m_iCurSel;
   RECT m_rcPage;//Box内Page的区域
   RECT m_rcBox;//去掉Tab的区域
   UINT m_uTabLeftOffset;
   CUIControl* m_pCurPage;
   CStdValArray m_aTabAreas;

   COLORREF m_colorOutside;
   COLORREF m_colorInside;
   COLORREF m_colorBackGround;

   COLORREF m_crTabTextNormal;
   COLORREF m_crTabTextFocused;
   COLORREF m_crTabTextPushed;
   COLORREF m_crTabTextDisabled;

   UINT m_uTabOverBoxPixels;

   Image*	m_pTabImageOwner;
   Image*	m_pBoxImageOwner;

};

#endif // !defined(AFX_UITAB_H__20060218_95D6_2F8B_4F7A_0080AD509054__INCLUDED_)

