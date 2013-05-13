#if !defined(AFX_UIEDIT_H__20060218_14AE_7A41_09A2_0080AD509054__INCLUDED_)
#define AFX_UIEDIT_H__20060218_14AE_7A41_09A2_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////////////
//

class CUIEditWnd;


class UILIB_API CUIEdit : public CUIControl
{
friend CUIEditWnd;
public:
   CUIEdit();
   ~CUIEdit();

   LPCTSTR GetClass() const;
   UINT GetControlFlags() const;

   void SetText(LPCTSTR pstrText, bool bNotidyChange = true);

   void SetFocus();
   void KillFocus();

   void SetWantReturn(bool bWantReturn);
   bool IsWantReturn() const;

   void SetReadOnly(bool bReadOnly);
   bool IsReadOnly() const;

   void Event(TEventUI& event);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetPassword(bool bPassword);
   void SetBorderWidth(UINT auWidth);
   UINT GetBorderWidth();

   bool LimitText(int nChars = 0);
   int  GetLimitTextCount();

   void SetMultiLine(bool bMulti);
   bool IsMultiLine() const;

   void Init();
   void SetPos(RECT rc);
   void SetVisible(bool bVisible);
   void SetEnabled(bool bEnabled);

   void SetImage(Image* pImage);
   void SetImage(UINT nResourceID, HMODULE hResHandle=NULL);
   void SetImage(LPCTSTR pstrName);

   Image *GetImage();

   void SetSel(int nStartChar, int nEndChar);

protected:
   void SetEditStyle(UINT uStyle);

   CUIEditWnd* m_pWindow;

   Image *m_pImageOwer;

   bool m_bReadOnly;
   bool m_bPassword;
   UINT m_uEditStyle;
   bool m_bMultiLine;
   bool m_bWantReturn;
   UINT m_uBorderWidth;

   int	m_nLimitTextCount;
};

#endif // !defined(AFX_UIEDIT_H__20060218_14AE_7A41_09A2_0080AD509054__INCLUDED_)

