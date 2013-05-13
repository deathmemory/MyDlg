#pragma once

#define WM_WND_LIST_SELECTED_CHANGED        WM_USER + 1025
#define WM_WND_LIST_ITEM_LBUTTON_CLICK		WM_USER + 1026
#define WM_WND_LIST_ITEM_RBUTTON_CLICK		WM_USER + 1027
#define WM_WND_LIST_ITEM_DOUBLE_CLICK		WM_USER + 1028
#define WM_WND_LIST_ITEM_BUTTON_CLICK		WM_USER + 1029

#include <vector>
using namespace std;
#include <afxmt.h>

#include "..\UILib\coolscroll.h"

class CWndListCtrl : public CWnd
{
	DECLARE_DYNAMIC(CWndListCtrl)
public:
	typedef struct  _CustomSetImagePosColumn
	{
		//int m_nColumnSetImagePosCustom;
		int m_xImage1Offset;
		int m_xImage2Offset;
		int m_xImage3Offset;


		int m_columnTextTopValueAdd;
		int m_columnTextBottomValueAdd;

		int m_columnImage1TopValueAdd;
		int m_columnImage1BottomValueAdd;

		int m_columnImage2TopValueAdd;
		int m_columnImage2BottomValueAdd;

		int m_columnImage3TopValueAdd;
		int m_columnImage3BottomValueAdd;   

        _CustomSetImagePosColumn()
		{
			/*
			m_xImage1Offset= m_xImage2Offset= m_xImage3Offset= m_columnTextTopValueAdd=	 m_columnTextBottomValueAdd= m_columnImage1TopValueAdd=	 m_columnImage1BottomValueAdd
				= m_columnImage2TopValueAdd= m_columnImage2BottomValueAdd=	 m_columnImage3TopValueAdd=	 m_columnImage3BottomValueAdd=  0;  
			*/
			memset(this,0,sizeof(struct _CustomSetImagePosColumn));
		}
		

	}CustomSetImagePosColumn;

	typedef struct CellInfo
	{	
		int m_nColumn;
		CString m_strText;
		CString m_strToolTip;
		COLORREF m_clrText;	

		int		 m_nOffsetX;
		
		CString m_strLinker[8];
		RECT	m_rectLinker[8];
		int		m_nLinkerCount;
		int		m_nCurrLinkerIdx;

		Image*	m_pImage1;
		Image*	m_pImage2;
		Image*	m_pImage3;

		BOOL	m_bSingleLine;

		BOOL	m_bBtn;
		RECT	m_rectBtn;
		CellInfo(int nColumn);
	}CellInfo;


	typedef struct ItemInfo
	{
		int m_nItem;
		vector<CellInfo> m_aCellInfo;
		COLORREF		 m_clrBack;
		int				 m_nItemHeight;
		int				 m_nMinHeight;
		BOOL			 m_bSelected;

		RECT			 m_rectBoxArea;
		BOOL			 m_bChecked;

		BOOL             m_bCursorInSide;

		DWORD_PTR		 m_dwData;

		ItemInfo();
	}ItemInfo;


	typedef struct ColumnInfo
	{
		int nItem;
		CString m_strText;
		int m_nColumnWidth;
		int m_nFormat;
		ColumnInfo();
	}ColumnInfo;


	typedef struct CtrlParam{
		int m_nCurTopLine;
		int m_nItemAtTop;
		int m_nTotalRowCnt;
		int m_nItemCntPerPage;
		int m_nRowCnt;
		int m_nColCnt;
		int m_nItemW;
		int m_nItemH;
	}CtrlParam;


	typedef enum
	{
		ImagePos_Left = 0,
		ImagePos_Top,
		ImagePos_Bottom
		
	}ImagePos;


public:
	CWndListCtrl();
	virtual ~CWndListCtrl();

	virtual void PreSubclassWindow();

public:
	virtual int         InsertItem(
								int nItem,
								LPCTSTR lpszItem,
								Image* pImage1 = NULL,
								Image* pImage2 = NULL,
								Image* pImage3 = NULL
								);
	virtual void         SetItemText(
								int nItem,
								int nSubItem,
								LPCTSTR lpszItem,
								Image* pImage1 = NULL,
								Image* pImage2 = NULL,
								Image* pImage3 = NULL
		);

	virtual CellInfo* GetCellInfo(int nItem, int nSubItem);
	virtual ItemInfo* GetItemInfo(int nItem);

	virtual int			GetColumnWidth(int nIdx);
	virtual int			GetColumnFormat(int nIdx);
	virtual CString		GetColumnText(int nIdx);
	

	virtual int         GetItemCount(void);
	virtual	int			GetTopIndex(void);
	virtual int			GetCountPerPage(void);
	virtual int			GetColumnCount(void);

	int HitTestEx(CPoint &point, int *col);

public:
	virtual void        RefreshParam(void);
	virtual void        ClearParam(void);

	virtual void        Draw();

	virtual void        ScrollUPLine(void);
	virtual void        ScrollDWLine(void);

	virtual void        ScrollUPPage(void);
	virtual void        ScrollDWPage(void);


	virtual void		DeleteAllItems(void);
	virtual BOOL		DeleteItem(int nIdx);

	virtual int         InsertColumn(int nItem, LPCTSTR lpszItem, int nWidth, int nFormat);
	


	virtual CString GetItemText(int nItem, int nSubItem);
	virtual int  SetSelectIndex(int nIdx);


	virtual DWORD_PTR GetItemData(int nItem);
	virtual BOOL	  SetItemData(int nItem, DWORD_PTR dwData);

	virtual BOOL	 GetCheck(int nItem);
	virtual void	 SetCheck(int nItem, BOOL bCheck);

	virtual void SetItemHeight(int nItem, int nHeight);

	virtual void SetItemBKColor(int nItem, int nSubItem, COLORREF color);
	virtual void SetItemTextColor(int nItem, int nSubItem, COLORREF color);
	virtual void SetItemLinker(int nItem, int nSubItem, LPCTSTR lpszLinker);
	virtual void SetItemTextOffsetX(int nItem, int nSubItem, int nOffset);
	virtual void SetItemSingleLine(int nItem, int nSubItem, BOOL bSingleLine);

	virtual void SetItemIsButton(int nItem, int nSubItem, BOOL bBtn);

	virtual void SetItemToolTip(int nItem, int nSubItem, LPCTSTR lpszTip);

	virtual LRESULT HandleCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm);

	void SetRedraw(BOOL bRedraw = TRUE);
	void SetCanSelected(BOOL bSelected = TRUE);

	void SetImagePos(CWndListCtrl::ImagePos nPos);
	BOOL SetSpanSize(int nLeftSpan, int nRightSpan, int nTopSpan, int nBottomSpan);

	void SetBKColor(COLORREF color);
	void SetSelectedItemBKColor(COLORREF colorBk, COLORREF colorBorder = INVALID__UICOLOR);

	void SetScrollBarImage(Image* pImage);
	void SetScrollBarSize(int nWidth, int nHeight);

	void SetCommonIconImage(Image* pImage);
	void SetCheckBoxImage(Image* pImage);

	void SetBackgroundImage(Image* pImage);
	void SetSelectedBkImage(Image* pImage);

	void SetButtonImage(Image* pImage, int nStatusCount = 4);

	int GetSelectedIndex()
	{
		return m_nSelectedItem;
	};

	void SetShowScrollBarAlway(BOOL bAlway);

	void SetImageBorderColor(COLORREF colorOutside, COLORREF colorInside);

	void SetFrameColor(COLORREF colorOutside, COLORREF colorInside);

	void SetItemCountPerRow(int nCount, BOOL bRedraw = FALSE);

	void DrawGird();

	void SetTitleHeight(int nHeight);
	void SetTitleBkColor(COLORREF colorTop, COLORREF colorBottom);

	void SetGirdColor(COLORREF color);

	void SetTooltipMaxWidth(int nWidth);
protected:
	CtrlParam				m_ILParam;
	vector<ItemInfo>		m_Items;

	vector<ColumnInfo>		m_Columns;

	int						m_nColCount;

	CFont					m_NormalFont;
	CFont					m_BoldFont;
	CFont					m_LinkerFont;

	CDC						m_MemDC;
	CBitmap					m_MemBmp;
	CBitmap*				m_pMemBmpOld;

	CDC						m_MemSelectDC;
	CBitmap					m_MemSelectBmp;
	CBitmap*				m_pMemSelectBmpOld;

	BOOL					m_bMouseOnLinker;

	int						m_nSelectedItem;
	BOOL					m_bCanSelected;
	BLENDFUNCTION			m_rBlendProps;
	HCURSOR					m_hCursorHander;

	HDC						m_hdcSkin;
	HBITMAP					m_hSkinBmp;

	BOOL					m_bRedraw;

	ULONG_PTR				m_gdiplusToken;

	ImagePos				m_nImagePos;

	int						m_nLeftSpan;
	int						m_nRightSpan;
	int						m_nTopSpan;
	int						m_nBottomSpan;

	int						m_nTitleHeight;
	COLORREF				m_colorTitleTop;
	COLORREF				m_colorTitleBottom;

	CRect					m_rectClient;
	CRect					m_rectWork;

	COLORREF				m_colorBackGround;
	COLORREF				m_colorItemSelectedBk;
	COLORREF				m_colorItemSelectedBorder;

	BOOL					m_bShowScrollBar;

	Image*					m_pImageScrollBar;
	Image*					m_pImageCheckBox;

	Image*					m_pImageCommonIcon;
	Image*					m_pImageBackground;
	Image*					m_pImageSelectedBK;

	Image*					m_pImageButton;
	int						m_nBtnStatusCount;

	int						m_nScrollWidth;
	int						m_nScrollHeight;

	CPoint					m_ptLastMouse;

	BOOL					m_bShowScrollBarAlway;

	BOOL					m_bResetItemHeight;

	COLORREF				m_colorImageBorderInside;
	COLORREF				m_colorImageBorderOutside;

	COLORREF				m_colorFrameInside;
	COLORREF				m_colorFrameOutside;
	COLORREF				m_colorGird;

	int						m_nItemCountPerRow;

	HWND					m_hwndTooltip;
	TOOLINFO				m_ToolTip;
	BOOL					m_bMouseTracking;

	int						m_nTooltipMaxWidth;

protected:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()

	void OnMyKeyDown(UINT nChar);
	void UpdateScrollbars();
	void SendMessageToParent(UINT nMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);


	POINT GetLastMousePos();
	const TEXTMETRIC& GetThemeFontInfo(HFONT font) const;
	void DoFillRect(HDC hDC, RECT rc, RECT rcImage, Image* pImage);

	void DoPaintPrettyText(HDC hDC, RECT& rc, LPCTSTR pstrText, COLORREF iTextColor, COLORREF iBackColor, RECT* prcLinks, int& nLinkRects, int& nNeedHeight, int nMinHeight, UINT uStyle);
	void DoPaintBorder(HDC hDC, RECT rcItem, COLORREF colorInside, COLORREF colorOutside);
public:
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//增加Items分隔线 功能
	/*使用步骤：
	  1 调用 SetItemSplitImage(UINT id) 设置作为分隔符的图片；之前可调用RemoveItemSplitImage()移除先前的图片
	  2 （可省略）调用SetItemSplitImageHeight(int _height) 设置分隔符所占的高度
	  3 调用SetItemSplitOpen ()启动。

	*/
protected:
	bool m_bItemSplitIsOpen;  //初始化为FALSE
	int  m_iItemSplitHeight;
	Image* m_pImageItemSplit;   
public:

	void SetItemSplitImage(UINT id);
	void SetItemSplitOpen(BOOL bIsOpen = true);
	BOOL GetItemSplitIsOpen();
	void SetItemSplitImageHeight(int _height=5);
	void RemoveItemSplitImage();
	//增加自定义列Image位置功能
	/*
	更新日期：2011.05.25 
	使用说明：
	   此模式目前只支持 每行只有一个Item的情况。
	         
	使用步骤：
	 --  （之前需确定此列存在！）调用SetColumnImagePosCustomMode（）设置此列相应的Items中 3个Image的相对于此Item左边界的偏移。（注意不要使偏移值超过列的宽度）
	*/
protected:
	std::map<int,CustomSetImagePosColumn> m_customSetImagePosColumnList;  //
	
public:
	bool SetColumnImagePosCustomMode(int nColumn,int xImage1,int xImage2,int xImage3,bool isOpen=true); //设置列图标位置 自定义模式。
	bool GetCustomImagePos(int nColumn,CustomSetImagePosColumn& pInfo);

	void SetColumnTextTopValueAdd(int nColumn,int _add);
	void SetColumnTextBottomValueAdd(int nColumn,int _add);

	void SetColumnImage1TopValueAdd(int nColumn,int _add);
	void SetColumnImage1BottomValueAdd(int nColumn,int _add);
	void SetColumnImage2TopValueAdd(int nColumn,int _add);
	void SetColumnImage2BottomValueAdd(int nColumn,int _add);
	void SetColumnImage3TopValueAdd(int nColumn,int _add);
	void SetColumnImage3BottomValueAdd(int nColumn,int _add);

	void AdJustItemText_ImagePosAdded(CRect& rcText,CRect& rtImage1,CRect& rtImage2,CRect& rtImage3,CustomSetImagePosColumn info);

	//当鼠标移动到Item上时为此Item 添加背景色
    /*   */
protected:
	bool m_bCurrentItemBkColorOpened;
	COLORREF m_currentItemBkColor;
	map<int,CRect> m_visableItemRectList;
public:
	bool SetCurrentItemBkColorEnable(bool _bEnable = true);
	void SetCurrentItemBkColor(COLORREF _color);
	

	
};

