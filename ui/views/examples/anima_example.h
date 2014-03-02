// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_EXAMPLES_ANIMA_EXAMPLE_H_
#define UI_VIEWS_EXAMPLES_ANIMA_EXAMPLE_H_
#pragma once

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/examples/example_base.h"

namespace views {

class ImageButton;
class View;

namespace examples {


class AnimaExample : public ExampleBase, public ButtonListener {
 public:
  AnimaExample();
  virtual ~AnimaExample();

  // Overridden from ExampleBase:
  virtual void CreateExampleView(View* container) OVERRIDE;

 private:
  // Overridden from ButtonListener:
  virtual void ButtonPressed(Button* sender, const Event& event) OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(AnimaExample);
};

}  // namespace examples
}  // namespace views

#endif  // UI_VIEWS_EXAMPLES_ANIMA_EXAMPLE_H_
