#ifndef CHROME_BROWSER_UI_VIEWS_VIDEOBAR_VIDEO_BAR_SAVE_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_VIDEOBAR_VIDEO_BAR_SAVE_VIEW_H_
#pragma once

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/examples/example_base.h"
#include "ui/views/controls/link_listener.h"
#include "ui/views/controls/scroll_view.h"
#include "ui/views/controls/button/image_button.h"


namespace views {
  namespace examples {
namespace {
  class ScrollableView;
}
    // MenuExample demonstrates how to use the MenuModelAdapter and MenuRunner
    // classes.
    class VideoBarSaveExample : public ExampleBase, public ButtonListener, public LinkListener {
    public:
      VideoBarSaveExample();
      virtual ~VideoBarSaveExample();

      // Overridden from ExampleBase:
      virtual void CreateExampleView(View* container) OVERRIDE;

    private:
      // Overridden from ButtonListener:
      virtual void ButtonPressed(Button* sender, const Event& event) OVERRIDE;
      // Overridden from LinkListener:
      virtual void LinkClicked(Link* source, int event_flags) OVERRIDE;

      void PopupSavePanel(View*);

      // Example buttons.
      TextButton* text_button_;
      ImageButton* save_button_;
      Widget* widget_;

      //
      ScrollView* scroll_view_;
      //ScrollableView* content_view_;
      View* content_view_;

      DISALLOW_COPY_AND_ASSIGN(VideoBarSaveExample);
    };

  }  // namespace examples
}  // namespace views











#endif //end CHROME_BROWSER_UI_VIEWS_VIDEOBAR_VIDEO_BAR_SAVE_VIEW_H_