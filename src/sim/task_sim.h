// TODO: header

#ifndef FIRMAMENT_SIM_TASK_SIM_H
#define FIRMAMENT_SIM_TASK_SIM_H

#include "base/task.h"
#include "sim/resource_sim.h"

namespace firmament {

class TaskSim : public Task {
 public:
  TaskSim(const string& name, double runtime);
  double runtime() { return runtime_; }
  void set_runtime(double runtime) { runtime_ = runtime; }
  void BindToResource(Resource *resource) {
    CHECK_NOTNULL(resource);
    bound_resource_ = resource;
  }
  void RelinquishBoundResource() {
    CHECK_NOTNULL(bound_resource_);
    bound_resource_->TaskExited();
    bound_resource_ = NULL;
  }
 private:
  double runtime_;
  Resource *bound_resource_;
};

}

#endif  // FIRMAMENT_SIM_TASK_SIM_H
