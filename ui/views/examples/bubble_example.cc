// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/examples/bubble_example.h"

#include "base/utf_string_conversions.h"
#include "ui/views/bubble/bubble_delegate.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/widget/widget.h"
#include "ui/views/bubble/bubble_frame_view.h"
#include "ui/base/animation/slide_animation.h"
#include "ui/views/animation/bounds_animator.h"

namespace views {
namespace examples {

struct BubbleConfig {
  string16 label;
  SkColor color;
  View* anchor_view;
  BubbleBorder::ArrowLocation arrow;
  bool fade_in;
  bool fade_out;
};

// Create four types of bubbles, one without arrow, one with an arrow, one
// that fades in, and another that fades out and won't close on the escape key.
BubbleConfig kRoundConfig = { ASCIIToUTF16("Round"), 0xFFC1B1E1, NULL,
                              BubbleBorder::NONE, false, false };
BubbleConfig kArrowConfig = { ASCIIToUTF16("Arrow"), SK_ColorGRAY, NULL,
                              BubbleBorder::TOP_LEFT, false, false };
BubbleConfig kFadeInConfig = { ASCIIToUTF16("FadeIn"), SK_ColorYELLOW, NULL,
                               BubbleBorder::BOTTOM_RIGHT, true, false };
BubbleConfig kFadeOutConfig = { ASCIIToUTF16("FadeOut"), SK_ColorWHITE, NULL,
                                BubbleBorder::LEFT_TOP, false, true };

static gfx::Rect g_kRect(0, 0, 640, 480);
class ExampleBubbleDelegateView : public BubbleDelegateView {
 public:
  explicit ExampleBubbleDelegateView(const BubbleConfig& config)
      : BubbleDelegateView(config.anchor_view, config.arrow),
        label_(config.label){
    set_color(config.color);
	this->set_close_on_deactivate(false);
  }
  ~ExampleBubbleDelegateView(){
  }
  // View overrides:
  virtual gfx::Size GetPreferredSize(){
	  return g_kRect.size(); 
  }

  // ui::AnimationDelegate overrides:
  virtual void AnimationEnded(const ui::Animation* animation){

  }
  virtual void AnimationProgressed(const ui::Animation* animation){

  }
public:
  void SetBubbleBounds(int x, int y, int width, int height){
	  GetBubbleFrameView()->SetBounds( x, y, width, height);
  }

 protected:
  virtual void Init() OVERRIDE {
    SetLayoutManager(new FillLayout());
    Label* label = new Label(label_);
	label->SetSize(gfx::Size(320, 240));
	AddChildView(label);

	SetSize(gfx::Size(320, 240));
  }

 private:
  string16 label_;
};

BubbleExample::BubbleExample() 
: ExampleBase("Bubble"),
bounds_animator_(NULL)
{
}





BubbleExample::~BubbleExample() {}

void BubbleExample::CreateExampleView(View* container) {
  container->SetLayoutManager(
      new BoxLayout(BoxLayout::kHorizontal, 0, 0, 1));
  round_ = new TextButton(this, kRoundConfig.label);
  arrow_ = new TextButton(this, kArrowConfig.label);
  fade_in_ = new TextButton(this, kFadeInConfig.label);
  fade_out_ = new TextButton(this, kFadeOutConfig.label);
  container->AddChildView(round_);
  container->AddChildView(arrow_);
  container->AddChildView(fade_in_);
  container->AddChildView(fade_out_);
}

void BubbleExample::ButtonPressed(Button* sender, const Event& event) {
  BubbleConfig config;
  if (sender == round_)
    config = kRoundConfig;
  else if (sender == arrow_)
    config = kArrowConfig;
  else if (sender == fade_in_)
    config = kFadeInConfig;
  else if (sender == fade_out_)
    config = kFadeOutConfig;

  config.anchor_view = sender;
  ExampleBubbleDelegateView* bubble_delegate =
	  new ExampleBubbleDelegateView(config );

  bubble_delegate->set_use_focusless(true);
  Widget *wid = BubbleDelegateView::CreateBubble(bubble_delegate);

#if 1 //test anima
  if (bounds_animator_)
	  delete bounds_animator_;
  bounds_animator_ = new views::BoundsAnimator(bubble_delegate->parent());
  bounds_animator_->SetAnimationDuration(4000);

  gfx::Rect rcBound(10, 10, 200, 200);
  //wid->SetBounds(rcBound);
  //bubble_delegate->SetBoundsRect(rcBound);
  //bubble_delegate->SetBounds(0, 0, 100, 200);
  //bubble_delegate->SetSize(gfx::Size(320, 240));
  //bubble_delegate->SetBubbleBounds( 200, 200, 400, 320);
  //bubble_delegate->SizeToContents();
#endif

  if (config.fade_in)
    bubble_delegate->StartFade(true);
  else{
    bubble_delegate->Show();
#if 1 //test anima
	bounds_animator_->AnimateViewTo(bubble_delegate, gfx::Rect(10, 10, 800, 600));
#endif
  }

  if (config.fade_out) {
    bubble_delegate->set_close_on_esc(false);
    bubble_delegate->StartFade(false);
  }
}

}  // namespace examples
}  // namespace views
