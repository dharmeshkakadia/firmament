// The Firmament project
// Copyright (c) 2011-2012 Malte Schwarzkopf <malte.schwarzkopf@cl.cam.ac.uk>
//
// Platform-independent coordinator class definition. This is subclassed by the
// platform-specific coordinator classes.

#ifndef FIRMAMENT_ENGINE_COORDINATOR_H
#define FIRMAMENT_ENGINE_COORDINATOR_H

#include <string>
#include <map>
#include <utility>
#include <vector>

// XXX(malte): Think about the Boost dependency!
#ifdef __PLATFORM_HAS_BOOST__
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/uuid/uuid.hpp>
#else
// Currently this won't build if __PLATFORM_HAS_BOOST__ is not defined.
#error __PLATFORM_HAS_BOOST__ not set, so cannot build coordinator!
#endif

#include "base/common.h"
#include "base/types.h"
#include "base/job_desc.pb.h"
#include "base/task_desc.pb.h"
#include "base/reference_desc.pb.h"
#include "base/resource_desc.pb.h"
#include "base/resource_topology_node_desc.pb.h"
#include "base/task_graph.h"
#include "engine/node.h"
#include "engine/knowledge_base.h"
// XXX(malte): include order dependency
#include "platforms/unix/common.h"
#include "messages/heartbeat_message.pb.h"
#include "messages/registration_message.pb.h"
#include "messages/task_delegation_message.pb.h"
#include "messages/task_heartbeat_message.pb.h"
#include "messages/task_info_message.pb.h"
#include "messages/task_spawn_message.pb.h"
#include "messages/task_state_message.pb.h"
#include "messages/storage_message.pb.h"
#include "misc/messaging_interface.h"
#include "platforms/common.h"
#include "platforms/unix/signal_handler.h"
#include "platforms/unix/stream_sockets_adapter.h"
#include "platforms/unix/stream_sockets_adapter-inl.h"
#include "platforms/unix/procfs_machine.h"
#ifdef __HTTP_UI__
#include "engine/coordinator_http_ui.h"
#endif
#include "scheduling/simple_scheduler.h"
#include "scheduling/quincy_scheduler.h"
#include "storage/object_store_interface.h"
#include "engine/topology_manager.h"
#ifdef __SIMULATE_SYNTHETIC_DTG__
#include "sim/simple_dtg_generator.h"
#endif

namespace firmament {

//using __gnu_cxx::hash_map;

using machine::topology::TopologyManager;
using platform_unix::SignalHandler;
using platform_unix::streamsockets::StreamSocketsChannel;
using platform_unix::streamsockets::StreamSocketsAdapter;
using platform_unix::ProcFSMachine;
using scheduler::SchedulerInterface;
using scheduler::SimpleScheduler;
using scheduler::QuincyScheduler;
using store::ObjectStoreInterface;

#ifdef __HTTP_UI__
// Forward declaration
namespace webui {
class CoordinatorHTTPUI;
}  // namespace webui
using webui::CoordinatorHTTPUI;
#endif

class Coordinator : public Node,
                    public boost::enable_shared_from_this<Coordinator> {
 public:
  explicit Coordinator(PlatformID platform_id);
  virtual ~Coordinator();
  void Run();
  const JobDescriptor* DescriptorForJob(const string& job_id);
  void Shutdown(const string& reason);
  const string SubmitJob(const JobDescriptor& job_descriptor);

  // Gets a pointer to the resource descriptor for an associated resource
  // (including the coordinator itself); returns NULL if the resource is not
  // associated or the coordinator itself.
  ResourceDescriptor* GetResource(ResourceID_t res_id) {
    if (res_id == uuid_)
      return &resource_desc_;
    ResourceStatus** res = FindOrNull(*associated_resources_, res_id);
    return (res ? (*res)->mutable_descriptor() : NULL);
  }

  // Gets a pointer to the resource status for an associated resource.
  // Returns NULL if not associated.
  ResourceStatus* GetResourceStatus(ResourceID_t res_id) {
    ResourceStatus** res = FindOrNull(*associated_resources_, res_id);
    return (res ? *res : NULL);
  }

  // Gets the current location (endpoint) of an associated resource.
  // Returns NULL if not associated.
  const string GetLocationForResource(ResourceID_t res_id) {
    ResourceStatus** res = FindOrNull(*associated_resources_, res_id);
    return (res ? (*res)->location() : NULL);
  }

  // Gets a pointer to the job descriptor for a job known to the coordinator.
  // If the job is not known to the coordinator, we will return NULL.
  JobDescriptor* GetJob(JobID_t job_id) {
    return FindOrNull(*job_table_, job_id);
  }

  // Gets a pointer to the task descriptor for a task known to the coordinator.
  // If the task is not known to the coordinator, we will return NULL.
  TaskDescriptor* GetTask(TaskID_t task_id) {
    TaskDescriptor** result = FindOrNull(*task_table_, task_id);
    return (result ? *result : NULL);
  }
  inline size_t NumResources() { return associated_resources_->size(); }
  inline size_t NumJobs() { return job_table_->size(); }
  inline size_t NumJobsInState(JobDescriptor::JobState state) {
    size_t count = 0;
    if (job_table_->empty())
      return 0;
    for (JobMap_t::const_iterator j_iter = job_table_->begin();
         j_iter != job_table_->end();
         ++j_iter)
      if (j_iter->second.state() == state)
        count++;
    return count;
  }
  inline size_t NumTasks() { return task_table_->size(); }
  inline size_t NumTasksInState(TaskDescriptor::TaskState state) {
    size_t count = 0;
    for (TaskMap_t::const_iterator t_iter = task_table_->begin();
         t_iter != task_table_->end();
         ++t_iter) {
      if (t_iter->second->state() == state)
        count++;
    }
    return count;
  }

  vector<ResourceStatus*> associated_resources() {
    vector<ResourceStatus*> res_vec;
    for (ResourceMap_t::const_iterator res_iter =
         associated_resources_->begin();
         res_iter != associated_resources_->end();
         ++res_iter) {
      res_vec.push_back(res_iter->second);
    }
    return res_vec;
  };
  vector<JobDescriptor> active_jobs() {
    vector<JobDescriptor> jd_vec;
    for (JobMap_t::const_iterator job_iter =
         job_table_->begin();
         job_iter != job_table_->end();
         ++job_iter) {
      jd_vec.push_back(job_iter->second);
    }
    return jd_vec;
  }
  vector<TaskDescriptor*> active_tasks() {
    vector<TaskDescriptor*> td_vec;
    for (TaskMap_t::const_iterator task_iter =
         task_table_->begin();
         task_iter != task_table_->end();
         ++task_iter) {
      td_vec.push_back(task_iter->second);
    }
    return td_vec;
  }
  inline const ResourceTopologyNodeDescriptor& local_resource_topology() {
    CHECK_NOTNULL(local_resource_topology_);
    return *local_resource_topology_;
  }
  shared_ptr<ObjectStoreInterface> get_object_store() {
      return object_store_;
  }
  const KnowledgeBase& knowledge_base() {
    return knowledge_base_;
  }

  void InformStorageEngineNewResource(ResourceDescriptor* rd);
  void KillRunningTask(TaskID_t task_id,
                       TaskKillMessage::TaskKillReason reason);

 protected:
  void AddJobsTasksToTables(TaskDescriptor* td, JobID_t job_id);
  void AddResource(ResourceDescriptor* resource_desc,
                   const string& endpoint_uri,
                   bool local);
  bool RegisterWithCoordinator(StreamSocketsChannel<BaseMessage>* chan);
  void DetectLocalResources();
  void HandleCreateRequest(const CreateRequest& msg,
                           const string& remote_endpoint);
  void HandleIncomingMessage(BaseMessage *bm, const string& remote_endpoint);
  void HandleIncomingReceiveError(const boost::system::error_code& error,
                                  const string& remote_endpoint);
  void HandleHeartbeat(const HeartbeatMessage& msg);
  void HandleLookupRequest(const LookupRequest& msg,
                           const string& remote_endpoint);
  void HandleIONotification(const BaseMessage& msg,
                            const string& remote_uri);
  void HandleRegistrationRequest(const RegistrationMessage& msg);
  void HandleTaskDelegationRequest(const TaskDelegationMessage& msg,
                                   const string& endpoint);
  void HandleTaskHeartbeat(const TaskHeartbeatMessage& msg);
  void HandleTaskInfoRequest(const TaskInfoRequestMessage& msg,
                             const string& remote_endpoint);
  void HandleTaskSpawn(const TaskSpawnMessage& msg);
  void HandleTaskStateChange(const TaskStateMessage& msg);
  void HandleStorageRegistrationRequest(const StorageRegistrationMessage& msg);

  /* Only necessary if storage is not guaranteed to be local*/
  void HandleStorageDiscoverRequest(const StorageDiscoverMessage& msg);
#ifdef __HTTP_UI__
  void InitHTTPUI();
#endif
  void SendHeartbeatToParent();

#ifdef __HTTP_UI__
  scoped_ptr<CoordinatorHTTPUI> c_http_ui_;
#endif
  // A map of resources associated with this coordinator.
  // The key is a resource UUID, the value a pair.
  // The first component of the pair is the resource descriptor, the second is
  // the timestamp when the latest heartbeat or message was received from this
  // resource..
  shared_ptr<ResourceMap_t> associated_resources_;
  // TODO(malte): Figure out the right representation here. Currently, we
  // maintain both associated_resources_ and the topology tree; one may suffice?
  ResourceTopologyNodeDescriptor* local_resource_topology_;
  // A map of all jobs known to this coordinator, indexed by their job ID.
  // Key is the job ID, value a ResourceDescriptor.
  // Currently, this table grows ad infinitum.
  shared_ptr<JobMap_t> job_table_;
  // A map of all tasks that the coordinator currently knows about.
  // TODO(malte): Think about GC'ing this.
  shared_ptr<TaskMap_t> task_table_;
  // TODO(malte): figure out if we need task_table_ and job_table_ in addition
  // to this.
  TaskGraphMap_t task_graph_table_;
  // The topology manager associated with this coordinator; responsible for the
  // local resources.
  shared_ptr<TopologyManager> topology_manager_;
  // The local object store.
  shared_ptr<ObjectStoreInterface> object_store_;
  // The local scheduler object. A coordinator may not have a scheduler, in
  // which case this will be a stub that defers to another scheduler.
  // TODO(malte): Work out the detailed semantics of this.
  SchedulerInterface* scheduler_;
  // Pointer to channel to the parent coordinator
  StreamSocketsChannel<BaseMessage>* parent_chan_;
  // Machine statistics monitor
  ProcFSMachine machine_monitor_;
  // Knowledge base
  KnowledgeBase knowledge_base_;

#ifdef __SIMULATE_SYNTHETIC_DTG__
  shared_ptr<sim::SimpleDTGGenerator> sim_dtg_generator_;
#endif
};

}  // namespace firmament

#endif  // FIRMAMENT_ENGINE_COORDINATOR_H
