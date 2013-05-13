#if !defined(AFX_UIPROGRESSBAR_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)
#define AFX_UIPROGRESSBAR_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIProgressBar : public CUIControl
{
public:
   CUIProgressBar();
   virtual ~CUIProgressBar();

   LPCTSTR GetClass() const;

    void SetText(LPCTSTR pstrText);
   SIZE EstimateSize(SIZE szAvailable);

   void SetProgressPos(UINT uPos);
   void SetProgressImage(Image*	pImage);
   void SetProgressImage(UINT nResourceID);
   void SetProgressImage(LPCTSTR pstrName);

   void SetWidth(int cxWidth);

   void GetRange(int& nMin, int& nMax ) const;
   void SetRange(int nMin, int nMax, bool bRedraw = true);

   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetDrawBackGround(bool bDraw);

protected:
	UINT	m_uPos;
	int		m_cxWidth;
	Image*  m_pImageOwner;
	UINT	m_uTextStyle;

	int m_nPosMax;
	int m_nPosMin;

	bool	m_bDrawBackGround;
};


#endif // !defined(AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_)

