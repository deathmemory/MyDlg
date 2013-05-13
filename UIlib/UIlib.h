
//#ifdef UILIB_EXPORTS
//#define UILIB_API __declspec(dllexport)
//#else
//#define UILIB_API __declspec(dllimport)
//#endif

#define UILIB_API 

// 定义一种不会再UI中出现的颜色，该颜色用作无效颜色
#define INVALID__UICOLOR RGB(1, 1, 1)

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>

#include "UIBase.h"
#include "UIManager.h"
#include "UIBlue.h"
#include "UIContainer.h"
#include "UILabel.h"
#include "UIButton.h"

#include "UIList.h"
#include "UICombo.h"
#include "UIProgressBar.h"
#include "UIMenuButton.h"

#include "UIMessageBox.h"
#include "UIEdit.h"
#include "UISliderCtrl.h"
#include "UITab.h"
#include "UIVScrollBar.h"

#include "SkinDialog.h"
#include "WndListCtrl.h"



