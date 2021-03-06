// The Firmament project
// Copyright (c) 2011-2012 Malte Schwarzkopf <malte.schwarzkopf@cl.cam.ac.uk>
//
// Initialization code for the executor library. This will be linked into the
// task binary, and constitutes the entry point for it. After creating an
// task library instance, setting up watcher threads etc., this will delegate to
// the task's Run() method.

#include "base/common.h"
#include "engine/task_lib.h"
//#include "platforms/common.h"

//#include "platforms/common.pb.h"

DECLARE_string(coordinator_uri);
DECLARE_string(resource_id);
DECLARE_string(cache_name);

using namespace firmament;  // NOLINT

// The main method: initializes, parses arguments and sets up a worker for
// the platform we're running on.
int main(int argc, char *argv[]) {
  // N.B.: We must always call InitFirmament from any main(), since it performs
  // setup work for command line flags, logging, etc.
  VLOG(2) << "Calling common::InitFirmament";
  common::InitFirmament(argc, argv);

  LOG(INFO) << "Firmament task library starting for resource "
            << FLAGS_resource_id;
  TaskLib task_lib;

  task_lib.Run(argc, argv);

  LOG(INFO) << "Executor's Run() method returned; terminating...";
}
