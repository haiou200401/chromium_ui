// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/at_exit.h"
#include "base/basictypes.h"
#include "base/command_line.h"
#include "base/debug/trace_event.h"
#include "base/environment.h"
#include "base/event_recorder.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/i18n/icu_util.h"
#include "base/message_loop.h"
#include "base/metrics/stats_table.h"
#include "base/path_service.h"
#include "base/process_util.h"
#include "base/rand_util.h"
#include "base/string_number_conversions.h"
#include "base/sys_info.h"
#include "base/utf_string_conversions.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_switches.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"
#include "ui/views/examples/examples_window.h"
#include "base/win/scoped_com_initializer.h"
#include "ui/base/win/scoped_ole_initializer.h"



int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, wchar_t*, int) {
  char *argv[] = {"name.exe"};
  int argc = 1;

  base::EnableInProcessStackDumping();
  base::EnableTerminationOnHeapCorruption();

  // Some tests may use base::Singleton<>, thus we need to instanciate
  // the AtExitManager or else we will leak objects.
  base::AtExitManager at_exit_manager;

  CommandLine::Init(argc, argv);
  const CommandLine& parsed_command_line = *CommandLine::ForCurrentProcess();

  // Load ICU data tables
  icu_util::Initialize();

  ui::ScopedOleInitializer ole_initializer;

  ui::ResourceBundle::InitSharedInstanceWithLocale("en-US", NULL);

  // Allocate a message loop for this thread.  Although it is not used
  // directly, its constructor sets up some necessary state.
  MessageLoopForUI main_message_loop;


  ShowExamplesWindow(views::examples::QUIT_ON_CLOSE, NULL);

  main_message_loop.Run();

  return 0;
}
