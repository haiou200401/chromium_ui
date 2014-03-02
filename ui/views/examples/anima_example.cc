// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/examples/anima_example.h"

#include "base/utf_string_conversions.h"
#include "grit/ui_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/point.h"
#include "ui/gfx/rect.h"
#include "ui/views/view.h"
#include "ui/views/controls/button/image_button.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/view.h"
#include "ui/base/animation/slide_animation.h"
#include "ui/views/animation/bounds_animator.h"

using namespace views;

namespace {
const int kLayoutSpacing = 10;  // pixels

class MyView : public View {
public:
  MyView() {
  }

private:
  // views::View overrides:
  virtual gfx::Size GetPreferredSize() OVERRIDE;
  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;

  /*virtual void Layout() OVERRIDE;*/
};

gfx::Size MyView::GetPreferredSize() {

  return gfx::Size(30, 20);
}

void MyView::OnPaint(gfx::Canvas* canvas) {
  gfx::Rect content = GetContentsBounds();
  canvas->FillRect(content, SkColorSetRGB(100, 255, 100));
  View::OnPaint(canvas);
}
//
//void MyView::Layout() {
//
//}

class AnimaView : public View {
public:
  AnimaView() :
  my_view_(new MyView())
  ,bounds_animation_(new BoundsAnimator(this)) {
    //SetLayoutManager( new FillLayout());
    //Label *label = new Label(L"move lable");
    //AddChildView(label);
    AddChildView(my_view_);

    //bounds_animation_->AnimateViewTo(label, gfx::Rect(100, 300, 100, 50));
    //this->AddChildView(new Label(L"Anima view"));
  }

  BoundsAnimator* animator() { return bounds_animation_.get();}

private:
  virtual void Layout() {
    const gfx::Rect& bound = bounds();

    my_view_->SetBounds(10, 100, bound.width()-100, 30);
  }
  // Fade animation for bubble.
  scoped_ptr<BoundsAnimator> bounds_animation_;

  MyView *my_view_;
};


}  // namespace

namespace views {
namespace examples {

AnimaExample::AnimaExample()
    : ExampleBase("Button")
{
}

AnimaExample::~AnimaExample() {
}

void AnimaExample::CreateExampleView(View* container) {
  //container->SetLayoutManager(new BoxLayout(BoxLayout::kVertical, 0, 0, kLayoutSpacing));
  container->SetLayoutManager(new FillLayout());


  AnimaView *anim_view = new AnimaView();
  //anim_view->animator()->AnimateViewTo(anim_view, gfx::Rect(300, 300, 100, 100));
  container->AddChildView( anim_view);

}

void AnimaExample::ButtonPressed(Button* sender, const Event& event) {

  example_view()->GetLayoutManager()->Layout(example_view());
}

}  // namespace examples
}  // namespace views
