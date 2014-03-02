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
#include "ui/views/widget/widget.h"
#include "base/stringprintf.h"
#include "ui/base/resource/resource_bundle.h"
#include "grit/ui_resources.h"
#include "ui/gfx/image/image.h"
#include "ui/views/widget/widget.h"

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

  class ScrollableView : public View {
  public:
    ScrollableView() {
      SetColor(SK_ColorRED, SK_ColorCYAN);
      AddChildView(new TextButton(NULL, ASCIIToUTF16("Button")));
      AddChildView(new RadioButton(ASCIIToUTF16("Radio Button"), 0));
    }

    virtual gfx::Size GetPreferredSize() {
      return gfx::Size(width(), height());
    }

    void SetColor(SkColor from, SkColor to) {
      set_background(Background::CreateVerticalGradientBackground(from, to));
    }

    void PlaceChildY(int index, int y) {
      View* view = child_at(index);
      gfx::Size size = view->GetPreferredSize();
      view->SetBounds(0, y, size.width(), size.height());
    }

    virtual void Layout() {
      PlaceChildY(0, 0);
      PlaceChildY(1, height() / 2);
      SizeToPreferredSize();
    }

  private:
    DISALLOW_COPY_AND_ASSIGN(ScrollableView);
  };


  const int VIEW_HEIGHT_MAX = 420;
  const int VIEW_BORDER_SIZE = 1;
  class VideoSaveContentView : public View, public ButtonListener, public LinkListener{
  public:
    VideoSaveContentView() {
      //Border *border = Border::CreateSolidBorder(1, SkColorSetRGB(60, 60, 60));
      //set_border(border);
    }

    void InitContent() {

      content_view_ = new View(); //ScrollableView();

      //scroll_view_ = new VideoItemScrollView();
      scroll_view_ = new ScrollView();
      scroll_view_->SetContents(content_view_);
      //scroll_view_->SetContents(new Label(L"test label"));
      gfx::Rect rc = scroll_view_->GetVisibleRect();


      set_background(Background::CreateSolidBackground(0, 255, 0));
      content_view_->set_background(Background::CreateSolidBackground(255, 255, 255));


      //BoxLayout* box_layout = new BoxLayout(BoxLayout::kVertical, 0, 0, 3);
      //this->SetLayoutManager(box_layout);
      //this->SetLayoutManager(new FillLayout);

      title_ = new Label(L"标题");
      this->AddChildView(title_);
      this->AddChildView(scroll_view_);


      GridLayout* grid_layout = new GridLayout(content_view_);
      content_view_->SetLayoutManager(grid_layout);


      int column_set_id;
      ColumnSet* column_set = NULL;

      //tooltip
      column_set_id = 0;
      column_set = grid_layout->AddColumnSet(column_set_id);
      column_set->AddPaddingColumn(0, 12);  //left padding
      //save all
      column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0.5f,
        GridLayout::USE_PREF, 0, 0);
      //setting
      column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0.5f,
        GridLayout::USE_PREF, 0, 0);

      grid_layout->StartRow(0, column_set_id);
      grid_layout->AddView(new Label(L"保存全部"));
      grid_layout->AddView(new Label(L"设置"));



      //Separator
      column_set_id = 1;
      column_set = grid_layout->AddColumnSet(column_set_id);
      column_set->AddPaddingColumn(0, 5);  //left padding
      column_set->AddColumn(GridLayout::FILL, GridLayout::CENTER, 0.5f,
        GridLayout::USE_PREF, 0, 0);

      grid_layout->AddPaddingRow(0, 5);
      grid_layout->StartRow(0, column_set_id);
      grid_layout->AddView(new Separator());
      grid_layout->AddPaddingRow(0, 5);


      column_set_id = 2;
      column_set = grid_layout->AddColumnSet(column_set_id);

      column_set->AddPaddingColumn(0, 12);  //left padding

      // icon
      column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0, //0.4f,
        GridLayout::FIXED, 19, 0);
      column_set->AddPaddingColumn(0, 3);

      // filename
      column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0, //0.4f,
        GridLayout::FIXED, 120, 0);
      column_set->AddPaddingColumn(0, 15);

      // save
      column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0, //0.2f,
        GridLayout::USE_PREF, 0, 0);
      column_set->AddPaddingColumn(0, 5);

      // copy link
      column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0, //0.2f,
        GridLayout::USE_PREF, 0, 0);
      column_set->AddPaddingColumn(0, 15);

      // file size
      column_set->AddColumn(GridLayout::LEADING, GridLayout::FILL, 0, //0.2f,
        GridLayout::USE_PREF, 0, 0);

      column_set->AddPaddingColumn(0, 12); //right padding


      gfx::Rect content_bound;
      gfx::Size layout_size;
      content_bound = content_view_->GetContentsBounds();

      AddColumnSet(grid_layout, L"条条大路通罗马，条条大路通罗马，条条大路通罗马file1.flv", 3*1024+3);
      AddColumnSet(grid_layout, L"file2.flv", 1024*1024*6.2);
      AddColumnSet(grid_layout, L"https://chromium.googlesource.com/android_tools.git.flv", 300);
      AddColumnSet(grid_layout, L"file4.flv", 1024);
      AddColumnSet(grid_layout, L"file5.flv", 1235);

      for( int i=0; i<3; i++) {
        string16 file_name = StringPrintf(L"filename%d.flv", i);
        AddColumnSet(grid_layout, file_name, i*1024*1024*1024);
      }


      layout_size = grid_layout->GetPreferredSize(content_view_);
      content_view_->SetBounds(0, 0, layout_size.width(), layout_size.height());

      gfx::Rect view_rect(0, 0, 320, 480);
      //scroll_view_->ScrollContentsRegionToBeVisible(view_rect);

      Border *border = Border::CreateSolidBorder(VIEW_BORDER_SIZE, SkColorSetRGB(169, 169, 169));
      scroll_view_->set_border(border);
    }
    gfx::Size GetViewSize() const {
      GridLayout* grid_layout = (GridLayout*)content_view_->GetLayoutManager();
      gfx::Size content_size = grid_layout->GetPreferredSize(content_view_);
      content_size.set_height(content_size.height()+50);

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
  private:
    virtual void Layout() {
      const gfx::Rect& bound = bounds();

      title_->SetBounds(0, 0, bound.width(), 50);
      scroll_view_->SetBounds(0, 50, bound.width(), bound.height()-50);
    }

    // Overridden from ButtonListener:
    virtual void ButtonPressed(Button* sender, const Event& event) {
    }
    // Overridden from LinkListener:
    virtual void LinkClicked(Link* source, int event_flags) {
      GridLayout* grid_layout = (GridLayout*)content_view_->GetLayoutManager();
      string16 file_name = StringPrintf(L"filename%d.flv", 333);
      AddColumnSet(grid_layout, file_name, 1024*1024);

      gfx::Size layout_size = grid_layout->GetPreferredSize(content_view_);
      content_view_->SetBounds(0, 0, layout_size.width(), layout_size.height());

      gfx::Size view_size = GetViewSize();
      scroll_view_->GetWidget()->SetSize(view_size); //gfx::Rect(0, 0, view_size.width(), view_size.height()));
      scroll_view_->Layout();

    }

    void AddColumnSet(GridLayout *layout, const string16& filename, uint32 length) {
      ImageView *image_view = NULL;
      Label* lable = NULL;
      Link* link = NULL;
      //TextButton *link = NULL;
      static int group_id = 0;


      ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
      //static int column_set_id = 0;

      layout->StartRow(0, 2); //column_set_id);
      image_view = new ImageView();
      image_view->SetImage(rb.GetImageNamed(IDR_FOLDER_OPEN).ToImageSkia());
      layout->AddView(image_view);

      lable = new Label(filename);
      lable->SetElideInMiddle(true);
      lable->SetEnabledColor(SkColorSetRGB(0, 0, 128));
      layout->AddView(lable);

      link = new Link(L"保存");
      link->set_listener(this);
      //link = new TextButton(this, L"保存");
      link->set_focusable(false);
      link->SetEnabledColor(SkColorSetRGB(0, 0, 128));
      link->set_id(BTN_ID_SAVE);
      link->SetGroup(group_id);
      layout->AddView(link);

      link = new Link(L"复制链接");
      link->set_listener(this);
      //link = new TextButton(this, L"复制链接");
      link->set_focusable(false);
      link->set_id(BTN_ID_COPY_LINK);
      link->SetGroup(group_id);
      layout->AddView(link);

      string16 file_size = GetSizeText(length); //StringPrintf(L"%dB", length);
      lable = new Label(file_size);
      layout->AddView(lable);
      //column_set_id ++;

      layout->AddPaddingRow(0, 5);

      group_id++;
    }

    Label* title_;
    ScrollView* scroll_view_;
    //ScrollableView* content_view_;
    View* content_view_;
  };

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
,content_view_(NULL)
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
  container->SetLayoutManager( new BoxLayout(BoxLayout::kVertical, 0, 0, 10));
  //VideoSaveButton* save_btn = new VideoSaveButton(L"下载");

  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  save_button_ = new ImageButton(this);
  save_button_->SetImage(ImageButton::BS_NORMAL, rb.GetImageNamed(IDR_CLOSE).ToImageSkia());
  save_button_->SetImage(ImageButton::BS_HOT, rb.GetImageNamed(IDR_CLOSE_H).ToImageSkia());
  save_button_->SetImage(ImageButton::BS_PUSHED, rb.GetImageNamed(IDR_CLOSE_P).ToImageSkia());
  container->AddChildView(save_button_);
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

    // Initialize the popup widget with the computed bounds.
    Widget::InitParams params(Widget::InitParams::TYPE_WINDOW_FRAMELESS);
    params.ownership = Widget::InitParams::WIDGET_OWNS_NATIVE_WIDGET;
    params.parent_widget =  parent->GetWidget(); //browser_view_->GetWidget();
    params.transparent = false; //true;
    params.can_activate = false;

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

  VideoSaveContentView* content_view = new VideoSaveContentView();
  content_view->InitContent();
  //content_view->SetBounds(0, 0, 320, 480);
  widget_->SetContentsView(content_view); //widget_container);
  gfx::Rect widget_bound(point.x(), point.y(), 250, 680);
  gfx::Size view_size = content_view->GetViewSize();
  widget_bound.set_width(view_size.width());
  widget_bound.set_height(view_size.height());
  widget_->SetBounds(widget_bound);

  //if (true) { //!transparent) {
  //  widget_container->set_background(
  //    Background::CreateStandardPanelBackground());
  //}

  // Show the widget.
  widget_->Show();
}
  
  
}  // namespace examples
}  // namespace views
