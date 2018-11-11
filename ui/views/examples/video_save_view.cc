// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/examples/video_save_view.h"

#include <set>

#include "base/utf_string_conversions.h"
#include "ui/base/models/simple_menu_model.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/button/menu_button_listener.h"
#include "ui/views/controls/button/image_button.h"
#include "ui/views/controls/menu/menu_model_adapter.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/controls/button/radio_button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/link.h"
#include "ui/views/controls/separator.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/view.h"
#include "ui/views/border.h"
#include "base/stringprintf.h"
#include "ui/base/resource/resource_bundle.h"
#include "grit/ui_resources.h"
#include "ui/gfx/image/image.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/root_view.h"

namespace views {
namespace examples {

namespace {
const int kLayoutSpacing = 10;  // pixels


  struct PluginVideoResource {
    struct Item {
      Item(const std::string& video_url, uint32 len, const std::string& ext) : url(video_url)
        ,length(len)
        ,extension(ext) {
      }

      Item() : length(0) {
      }

      std::string url;
      uint32 length;  // video length
      std::string extension;  // file extension name.
    };

    PluginVideoResource() {
    }

    string16 title;
    std::string referrer;
    std::vector<Item> resources;
  };


  enum ButtonTag {
    BTN_ID_SAVE = 0,
    BTN_ID_COPY_LINK,
    BTN_ID_SAVE_ALL,
    BTN_ID_SETTING,
  };

  string16 GetSizeText( uint32 iContentSize)
  {
    //TCHAR szSize[ MAX_PATH ] = _T( "" );
    //TCHAR szBuffer[ MAX_PATH ] = _T( "" );

    string16 result_text;
    if ( -1 != iContentSize && 0 != iContentSize )
    {
      if ( iContentSize < 1024 )
      {
        //_i64tot_s( iContentSize, szBuffer, _countof(szBuffer), 10 );
        //_stprintf_s( szSize, _T( "%s B" ), szBuffer );
        result_text = StringPrintf(L"%d B", iContentSize);
      }
      else
      {
        uint32 iSizeKB = iContentSize / 1024;
        if ( iSizeKB < 1024 )
        {
          //_i64tot_s( iSizeKB, szBuffer, _countof(szBuffer), 10 );
          //_stprintf_s( szSize, _T( "%s KB" ), szBuffer );
          result_text = StringPrintf(L"%d KB", iSizeKB);
        }
        else
        {
          double dSizeMB = ( double )iSizeKB / 1024.0;
          if (dSizeMB < 1024.0f)
          {
            //_stprintf_s( szSize, _T( "%.2f MB" ), dSizeMB );
            result_text = StringPrintf(L"%.2f MB", dSizeMB);
          }       
          else
          {
            double dSizeGB = ( double )dSizeMB / 1024.0f;
            //_stprintf_s( szSize, _T( "%.2f GB" ), dSizeGB );
            result_text = StringPrintf(L"%.2f GB", dSizeGB);
          }
        }
      }
    }
    else
      result_text = StringPrintf(L"未知大小");

    return result_text;
  }

const int VIEW_HEIGHT_MAX = 420;
const int VIEW_BORDER_SIZE = 1;
const int VIEW_HOR_MARGIN = 12;
const int VIEW_VER_MARGIN = 3;

class TitleBarView : public View {
public:
  TitleBarView() :
      title_(NULL)
        ,btn_save_all_(NULL){

          set_border(Border::CreateSolidBorder(VIEW_BORDER_SIZE, SK_ColorGRAY));

          title_ = new Link(L"资源列表");
          title_->set_id(BTN_ID_SETTING);
          btn_save_all_ = new Link(L"全部保存");
          btn_save_all_->set_id(BTN_ID_SAVE_ALL);
          AddChildView(title_);
          AddChildView(btn_save_all_);
      }

      void set_listener(LinkListener* listener) {
        if (title_)
          title_->set_listener(listener);
        if (btn_save_all_)
          btn_save_all_->set_listener(listener);
      }

      virtual gfx::Size GetPreferredSize() {
        gfx::Size size_title = title_->GetPreferredSize();
        gfx::Size size_save_all = btn_save_all_->GetPreferredSize();

        int width = size_title.width() + size_save_all.width() + VIEW_HOR_MARGIN*2;
        int height = std::max(size_title.height(), size_save_all.height()) + VIEW_VER_MARGIN*2;
        return gfx::Size(width, height);
      }

      virtual void Layout() {
        const gfx::Rect& bound = bounds();

        gfx::Size btn_size;

        btn_size = title_->GetPreferredSize();
        title_->SetBounds(VIEW_HOR_MARGIN, VIEW_VER_MARGIN, btn_size.width(), btn_size.height());

        btn_size = btn_save_all_->GetPreferredSize();
        btn_save_all_->SetBounds(bound.width()-btn_size.width()-VIEW_HOR_MARGIN, VIEW_VER_MARGIN, btn_size.width(), btn_size.height());
      }

private:

  Link *title_;
  Link* btn_save_all_;

  DISALLOW_COPY_AND_ASSIGN(TitleBarView);
};


class VideoSavePanelView : public View, public ButtonListener, public LinkListener{
public:

  VideoSavePanelView();

  virtual gfx::Size GetPreferredSize();
  virtual void Layout();
private:
  void InitFrame();
  void InitContent();

  // Overridden from ButtonListener:
  virtual void ButtonPressed(Button* sender, const Event& event);
  // Overridden from LinkListener:
  virtual void LinkClicked(Link* source, int event_flags);

  void AddColumnSet(GridLayout *layout, const string16& filename, uint32 length);

  TitleBarView* title_bar_view_;
  ScrollView* scroll_view_;
  View* scrollable_view_;
};


VideoSavePanelView::VideoSavePanelView() :
title_bar_view_(NULL),
scroll_view_(NULL),
scrollable_view_(NULL)
{
  set_background(Background::CreateSolidBackground(255, 255, 255));
  set_border(Border::CreateSolidBorder(VIEW_BORDER_SIZE, SK_ColorGRAY));

  InitFrame();
  InitContent();
}



void VideoSavePanelView::InitFrame() {
  title_bar_view_ = new TitleBarView();
  title_bar_view_->set_listener(this);
  AddChildView(title_bar_view_);

  scroll_view_ = new ScrollView();
  AddChildView(scroll_view_);
}

void VideoSavePanelView::InitContent() {
  if (scrollable_view_) {
    scroll_view_->SetContents(NULL);
    scrollable_view_ = NULL;
  }

  scrollable_view_ = new View();
  scroll_view_->SetContents(scrollable_view_);

  GridLayout* grid_layout = new GridLayout(scrollable_view_);
  scrollable_view_->SetLayoutManager(grid_layout);


  int column_set_id;
  ColumnSet* column_set = NULL;

  // top margin
  grid_layout->AddPaddingRow(0, VIEW_VER_MARGIN);

  // resource item.
  column_set_id = 0;
  column_set = grid_layout->AddColumnSet(column_set_id);
  column_set->AddPaddingColumn(0, 12);  //left padding

  // icon
  column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0,
    GridLayout::FIXED, 19, 0);
  column_set->AddPaddingColumn(0, 3);

  // filename
  column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0,
    GridLayout::FIXED, 120, 0);
  column_set->AddPaddingColumn(0, 12);

  // file size
  column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0,
    GridLayout::USE_PREF, 0, 0);
  column_set->AddPaddingColumn(0, 12);

  // save
  column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0,
    GridLayout::USE_PREF, 0, 0);
  column_set->AddPaddingColumn(0, 12);



  gfx::Rect scrollable_view_bound;
  gfx::Size layout_size;
  scrollable_view_bound = scrollable_view_->GetContentsBounds();

  AddColumnSet(grid_layout, L"条条大路通罗马，条条大路通罗马，条条大路通罗马file1.flv", 3*1024+3);
  AddColumnSet(grid_layout, L"file2.flv", 1024*1024*6.2);
  AddColumnSet(grid_layout, L"https://chromium.googlesource.com/android_tools.git.flv", 300);
  AddColumnSet(grid_layout, L"file4.flv", 1024);
  AddColumnSet(grid_layout, L"file5.flv", 1235);

  for( int i=0; i<3; i++) {
    string16 file_name = StringPrintf(L"filename%d.flv", i);
    AddColumnSet(grid_layout, file_name, i*1024*1024*1024);
  }

  layout_size = grid_layout->GetPreferredSize(scrollable_view_);
  scrollable_view_->SetBounds(0, 0, layout_size.width(), layout_size.height());
}


gfx::Size VideoSavePanelView::GetPreferredSize() {
  GridLayout* grid_layout = (GridLayout*)scrollable_view_->GetLayoutManager();
  gfx::Size toobar_size = title_bar_view_->GetPreferredSize();
  gfx::Size content_size = grid_layout->GetPreferredSize(scrollable_view_);
  content_size.set_height(content_size.height()+toobar_size.height());

  gfx::Size view_size;
  if (content_size.height() > VIEW_HEIGHT_MAX) {
    view_size.set_height(VIEW_HEIGHT_MAX);
    view_size.set_width(content_size.width() + scroll_view_->GetScrollBarWidth());
  } else {
    view_size = content_size;
  }

  view_size.Enlarge(VIEW_BORDER_SIZE*2, VIEW_BORDER_SIZE*2);
  return view_size;
}

void VideoSavePanelView::Layout() {
  const gfx::Rect& bound = bounds();

  gfx::Size toobar_size = title_bar_view_->GetPreferredSize();
  title_bar_view_->SetBounds(0, 0, bound.width(), toobar_size.height());
  scroll_view_->SetBounds(VIEW_BORDER_SIZE, toobar_size.height()+VIEW_BORDER_SIZE,
    bound.width()-VIEW_BORDER_SIZE*2, bound.height()-toobar_size.height()-VIEW_BORDER_SIZE*2);
}

// Overridden from ButtonListener:
void VideoSavePanelView::ButtonPressed(Button* sender, const Event& event) {
}

// Overridden from LinkListener:
void VideoSavePanelView::LinkClicked(Link* source, int event_flags) {
  GridLayout* grid_layout = (GridLayout*)scrollable_view_->GetLayoutManager();

  if (BTN_ID_SAVE_ALL == source->id()) {
    //scrollable_view_->RemoveAllChildViews(true);
    InitContent();
  } else {
    int org_scoll_pos = 0;
    ScrollBar* ver_scroll_bar = scroll_view_->vertical_scroll_bar();
    if (ver_scroll_bar)
      org_scoll_pos = ver_scroll_bar->GetPosition();


    string16 file_name = StringPrintf(L"filename%d.flv", 333);
    AddColumnSet(grid_layout, file_name, 1024*1024);

    gfx::Size layout_size = grid_layout->GetPreferredSize(scrollable_view_);
    scrollable_view_->SetBounds(0, 0, layout_size.width(), layout_size.height());

    gfx::Size view_size = GetPreferredSize();
    scroll_view_->GetWidget()->SetSize(view_size);
    scroll_view_->Layout();

    // resotre scroll position.
    ver_scroll_bar = scroll_view_->vertical_scroll_bar();
    if (0 != org_scoll_pos) {
      const gfx::Rect& scroll_view_bounds = scroll_view_->bounds();
      gfx::Rect scroll_to_rect(0, org_scoll_pos, scroll_view_bounds.width(), scroll_view_bounds.height());
      scroll_view_->ScrollContentsRegionToBeVisible(scroll_to_rect);
    }
  }

}

void VideoSavePanelView::AddColumnSet(GridLayout *layout, const string16& filename, uint32 length) {
  ImageView *image_view = NULL;
  Label* lable = NULL;
  Link* link = NULL;
  //TextButton *link = NULL;
  static int group_id = 0;


  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  static int column_set_id = 0;

  layout->StartRow(0, column_set_id);
  image_view = new ImageView();
  image_view->SetImage(rb.GetImageNamed(IDR_FOLDER_OPEN).ToImageSkia());
  layout->AddView(image_view);

  lable = new Label(filename);
  lable->SetElideInMiddle(true);
  //lable->SetEnabledColor(SkColorSetRGB(0, 0, 128));
  layout->AddView(lable);

  string16 file_size = GetSizeText(length);
  lable = new Label(file_size);
  layout->AddView(lable);

  link = new Link(L"保存");
  link->set_listener(this);
  //link = new TextButton(this, L"保存");
  link->set_focusable(false);
  link->SetEnabledColor(SkColorSetRGB(0, 0, 128));
  link->set_id(BTN_ID_SAVE);
  link->SetGroup(group_id);
  layout->AddView(link);

  layout->AddPaddingRow(0, VIEW_VER_MARGIN);

  group_id++;
}



  class VideoItemScrollView : public ScrollView {
  public:
    VideoItemScrollView() {
    }

    // overridden from
    // View overrides:
    virtual gfx::Size GetPreferredSize(){
      return gfx::Size(320, 240);
    }

    DISALLOW_COPY_AND_ASSIGN(VideoItemScrollView);
  };

  // A layout manager that layouts a single child at
  // the center of the host view.
  class CenterLayout : public LayoutManager {
  public:
    CenterLayout() {}
    virtual ~CenterLayout() {}

    // Overridden from LayoutManager:
    virtual void Layout(View* host) {
      View* child = host->child_at(0);
      gfx::Size size = child->GetPreferredSize();
      child->SetBounds((host->width() - size.width()) / 2,
        (host->height() - size.height()) / 2,
        size.width(), size.height());
    }

    virtual gfx::Size GetPreferredSize(View* host) {
      return gfx::Size();
    }

  private:
    DISALLOW_COPY_AND_ASSIGN(CenterLayout);
  };

  // VideoSaveButton -----------------------------------------------------------
  class VideoSaveButton : public MenuButton, public MenuButtonListener {
  public:
    explicit VideoSaveButton(const string16& test);
    virtual ~VideoSaveButton();

  private:
    // Overridden from MenuButtonListener:
    virtual void OnMenuButtonClicked(View* source,
      const gfx::Point& point) OVERRIDE;

    DISALLOW_COPY_AND_ASSIGN(VideoSaveButton);
  };

  VideoSaveButton::VideoSaveButton(const string16& test)
    : ALLOW_THIS_IN_INITIALIZER_LIST(MenuButton(NULL, test, this, true)) {
  }

  VideoSaveButton::~VideoSaveButton() {
  }

  void VideoSaveButton::OnMenuButtonClicked(View* source,
    const gfx::Point& point) {

  }


}  // namespace




VideoBarSaveExample::VideoBarSaveExample() : ExampleBase("VideoBarSave")
,scroll_view_(NULL)
,widget_(NULL){
}

VideoBarSaveExample::~VideoBarSaveExample() {
}


void VideoBarSaveExample::CreateExampleView(View* container) {
  //// We add a button to open a menu.
  //VideoSaveButton* menu_button = new VideoSaveButton(
  //  ASCIIToUTF16("Open a menu"));
  //container->SetLayoutManager(new FillLayout);
  //container->AddChildView(menu_button);
  //container->SetLayoutManager(
  //  new BoxLayout(BoxLayout::kVertical, 0, 0, kLayoutSpacing));
  //VideoSaveButton* save_btn = new VideoSaveButton(L"下载");

  //container->SetLayoutManager( new BoxLayout(BoxLayout::kVertical, 0, 0, 10));

#if 0 //gloam:
  //container->SetLayoutManager( new BoxLayout(BoxLayout::kVertical, 0, 0, 10));
  VideoSavePanelView* content_view = new VideoSavePanelView();
  //content_view->InitContent();
  container->AddChildView(content_view);
  content_view->SizeToPreferredSize();
  //container->SizeToPreferredSize();
#else
  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  save_button_ = new ImageButton(this);
  save_button_->SetImage(ImageButton::BS_NORMAL, rb.GetImageNamed(IDR_CLOSE).ToImageSkia());
  save_button_->SetImage(ImageButton::BS_HOT, rb.GetImageNamed(IDR_CLOSE_H).ToImageSkia());
  save_button_->SetImage(ImageButton::BS_PUSHED, rb.GetImageNamed(IDR_CLOSE_P).ToImageSkia());

  gfx::Size btn_size = save_button_->GetPreferredSize();
  //save_button_->SetBounds(10, 10, btn_size.width(), btn_size.height());
  save_button_->SizeToPreferredSize();
  container->AddChildView(save_button_);
#endif
}

void VideoBarSaveExample::ButtonPressed(Button* sender, const Event& event) {

  if (sender == save_button_) {
    PopupSavePanel(sender);
  } else {
    sender->GetWidget()->Close();
    widget_ = NULL;
  }
}

void VideoBarSaveExample::LinkClicked(Link* source, int event_flags) {

}

void VideoBarSaveExample::PopupSavePanel(View* parent) {
  if (!widget_) {
    widget_ = new Widget();

#if 1 //se
    views::Widget::InitParams params(views::Widget::InitParams::TYPE_WINDOW_FRAMELESS); //TYPE_CONTROL);
    params.ownership = views::Widget::InitParams::WIDGET_OWNS_NATIVE_WIDGET;
    params.parent_widget =  parent->GetWidget();
    params.transparent = false; //true;
    params.can_activate = false;
#else
    // Initialize the popup widget with the computed bounds.
    Widget::InitParams params(Widget::InitParams::TYPE_WINDOW_FRAMELESS);
    params.ownership = Widget::InitParams::WIDGET_OWNS_NATIVE_WIDGET;
    params.parent_widget =  parent->GetWidget(); //browser_view_->GetWidget();
    params.transparent = false; //true;
    params.can_activate = false;
#endif

    //params.bounds = gfx::Rect(point.x(), point.y(), 200, 300);
    widget_->Init(params);
  }
  // Compute where to place the popup widget.
  // We'll place it right below the create button.
  gfx::Point point = parent->GetMirroredPosition();
  // The position in point is relative to the parent. Make it absolute.
  View::ConvertPointToScreen(parent, &point);
  // Add the height of create_button_.
  point.Offset(0, parent->size().height());

  //DWORD swp_flags = SWP_NOOWNERZORDER | SWP_NOACTIVATE;
  //SetWindowPos(widget_->GetNativeView(), HWND_TOP, point.x(), point.y(), 340, 480, swp_flags);

  VideoSavePanelView* content_view = new VideoSavePanelView();
  //content_view->InitContent();
  //content_view->SetBounds(0, 0, 320, 480);
  widget_->SetContentsView(content_view); //widget_container);
  gfx::Rect widget_bound(point.x(), point.y(), 250, 680);
  gfx::Size view_size = content_view->GetPreferredSize();
  widget_bound.set_width(view_size.width());
  widget_bound.set_height(view_size.height());

#if 0 //gloam: focus
  View* root_view = widget_->GetRootView();
  View* focus_view = NULL;
  if (root_view) {
    FocusManager* fm = root_view->GetFocusManager();
    if (fm) {
      fm->SetFocusedView(content_view);
      focus_view = fm->GetFocusedView();
    }
  }
#endif
  //if (true) { //!transparent) {
  //  widget_container->set_background(
  //    Background::CreateStandardPanelBackground());
  //}

  // Show the widget.
#if 0 //se


  DWORD swp_flags = SWP_NOOWNERZORDER | SWP_NOACTIVATE;
  //if (no_redraw)
  //  swp_flags |= SWP_NOREDRAW;
  if (!widget_->IsVisible())
    swp_flags |= SWP_SHOWWINDOW;

  ::SetWindowPos(widget_->GetNativeView(), HWND_TOP, widget_bound.x(), widget_bound.y(),
    widget_bound.width(), widget_bound.height(), swp_flags);

  if (widget_->GetContentsView())
    widget_->GetContentsView()->SchedulePaint();
#else
  widget_->SetBounds(widget_bound);
  widget_->Show();
#endif
}
  
  
}  // namespace examples
}  // namespace views
