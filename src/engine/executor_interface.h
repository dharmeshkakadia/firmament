// The Firmament project
// Copyright (c) 2011-2012 Malte Schwarzkopf <malte.schwarzkopf@cl.cam.ac.uk>
//
// The executor interface assumed by the engine.

#ifndef FIRMAMENT_ENGINE_EXECUTOR_INTERFACE_H
#define FIRMAMENT_ENGINE_EXECUTOR_INTERFACE_H

#include "base/common.h"
#include "base/types.h"
#include "base/task_final_report.pb.h"
#include "misc/printable_interface.h"

namespace firmament {
namespace executor {

class ExecutorInterface : public PrintableInterface {
 public:
  virtual ostream& ToString(ostream* stream) const = 0;
  virtual void RunTask(TaskDescriptor* td,
                       bool firmament_binary) = 0;
  virtual void HandleTaskCompletion(const TaskDescriptor& td,
                                    TaskFinalReport* report) = 0;

 protected:
};

}  // namespace executor
}  // namespace firmament

#endif  // FIRMAMENT_ENGINE_EXECUTOR_INTERFACE_H
