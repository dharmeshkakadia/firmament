ROOT_DIR=../..
SUFFIX=engine

include $(ROOT_DIR)/include/Makefile.config
include $(ROOT_DIR)/include/Makefile.common

TASK_LIB_LIBS := $(LIBS) -lboost_thread -lboost_system -lboost_regex -lboost_date_time -lhwloc -lssl -lcrypto
LIBS += -lboost_thread -lboost_system -lboost_regex -lboost_date_time -lhwloc -lssl -lcrypto
HTTP_UI_LIBS += -lpion-common -lpion-net -llog4cpp $(PB2JSON_LIB) -ljansson -lctemplate

LIB = libfirmament_task_lib.a
BINS = coordinator worker coordinator_sim
OBJS = coordinator_http_ui.o coordinator.o knowledge_base.o local_executor.o node.o remote_executor.o worker.o topology_manager.o
PBS =
TESTS = coordinator_test local_executor_test simple_scheduler_test topology_manager_test worker_test
TESTS_DEPS = $(BUILD_DIR)/misc/libfirmament_misc.a \
	     $(BUILD_DIR)/base/libfirmament_base.a \
	     $(BUILD_DIR)/messages/libfirmament_messages.a \
	     $(BUILD_DIR)/platforms/unix/libfirmament_unix.a \
	     $(BUILD_DIR)/platforms/common.pb.o \
	     $(BUILD_DIR)/storage/libfirmament_storage.a
# Test-specific dependencies; we keep these separate from the others in order
# to avoid cyclic dependencies that will break test compilation.
# E.g. worker_test would depend on coordinator.o if it had coordinator_http_ui.o
# as a dependency.
TESTS_coordinator_DEPS = $(BUILD_DIR)/engine/coordinator_http_ui.o \
	                 $(BUILD_DIR)/engine/node.o \
	                 $(BUILD_DIR)/engine/knowledge_base.o \
	                 $(BUILD_DIR)/platforms/unix/libfirmament_unix.a \
	                 $(BUILD_DIR)/engine/local_executor.o \
	                 $(BUILD_DIR)/engine/remote_executor.o \
	                 $(BUILD_DIR)/engine/topology_manager.o \
	                 $(BUILD_DIR)/scheduling/libfirmament_scheduling.a \
	                 $(BUILD_DIR)/base/libfirmament_base.a \
	                 $(BUILD_DIR)/messages/libfirmament_messages.a \
	                 $(BUILD_DIR)/storage/libfirmament_storage.a \
			 $(HTTP_UI_LIBS)
# N.B.: Note that simple_scheduler_test lives in the engine makefile as it
# depends on engine components that are not yet built when the schedulers (which
# the coordinator depends on) are built.
TESTS_simple_scheduler_DEPS = $(BUILD_DIR)/engine/node.o \
			      $(BUILD_DIR)/engine/local_executor.o \
			      $(BUILD_DIR)/engine/remote_executor.o \
			      $(BUILD_DIR)/engine/topology_manager.o \
			      $(BUILD_DIR)/messages/libfirmament_messages.a \
			      $(BUILD_DIR)/scheduling/libfirmament_scheduling.a \
			      $(BUILD_DIR)/misc/libfirmament_misc.a \
			      $(BUILD_DIR)/base/libfirmament_base.a \
			      $(BUILD_DIR)/platforms/unix/libfirmament_unix.a \
			      $(BUILD_DIR)/storage/libfirmament_storage.a
TESTS_local_executor_DEPS = $(BUILD_DIR)/engine/topology_manager.o \
			    $(BUILD_DIR)/misc/libfirmament_misc.a \
			    $(BUILD_DIR)/base/libfirmament_base.a
TESTS_worker_DEPS = $(BUILD_DIR)/engine/node.o \
                    $(BUILD_DIR)/base/libfirmament_base.a \
		    $(BUILD_DIR)/platforms/unix/libfirmament_unix.a


OBJ_BIN = $(addprefix $(OBJ_DIR)/, $(BINS))
OBJ_LIB = $(addprefix $(OBJ_DIR)/, $(LIB))
#TESTS_OBJS = $(addprefix $(TEST_OBJ_DIR)/, $(TESTS))
TESTS_BIN = $(addprefix $(TEST_OBJ_DIR)/, $(TESTS))

all: protobufs $(OBJ_BIN) $(TESTS_BIN) $(OBJ_LIB) .setup

protobufs: $(addprefix $(OBJ_DIR)/, $(PBS))

# TODO: check if any of the below targets can make use of a generic
# build rule from Makefile.common, or can be converted to do so.

$(OBJ_DIR)/coordinator: $(addprefix $(OBJ_DIR)/, $(OBJS)) \
	$(BUILD_DIR)/base/libfirmament_base.a \
	$(BUILD_DIR)/misc/libfirmament_misc.a \
	$(BUILD_DIR)/messages/libfirmament_messages.a \
	$(BUILD_DIR)/messages/libfirmament_scheduling.a \
	$(BUILD_DIR)/messages/libfirmament_storage.a \
	$(BUILD_DIR)/platforms/unix/libfirmament_unix.a
	$(call quiet-command, \
		$(CXX) $(CPPFLAGS) \
		$(SRC_ROOT_DIR)/engine/coordinator_main.cc \
		$(OPTFLAGS) $(DEFINES) \
		$(BUILD_DIR)/engine/coordinator.o \
		$(BUILD_DIR)/engine/knowledge_base.o \
		$(BUILD_DIR)/engine/node.o \
		$(BUILD_DIR)/engine/coordinator_http_ui.o \
		$(BUILD_DIR)/engine/local_executor.o \
		$(BUILD_DIR)/engine/remote_executor.o \
		$(BUILD_DIR)/engine/topology_manager.o \
		$(BUILD_DIR)/platforms/common.pb.o \
		$(BUILD_DIR)/messages/libfirmament_messages.a \
		$(BUILD_DIR)/scheduling/libfirmament_scheduling.a \
		$(BUILD_DIR)/base/libfirmament_base.a \
		$(BUILD_DIR)/misc/libfirmament_misc.a \
		$(BUILD_DIR)/platforms/unix/libfirmament_unix.a \
		$(BUILD_DIR)/storage/libfirmament_storage.a \
		$(LIBS) $(HTTP_UI_LIBS) \
		-o $(BUILD_DIR)/engine/coordinator, \
		"  DYNLNK  $(BUILD_DIR)/engine/coordinator")

# XXX(malte): temporary hack
$(OBJ_DIR)/coordinator_sim: $(BUILD_DIR)/sim/simple_dtg_generator.o \
	$(BUILD_DIR)/base/libfirmament_base.a \
	$(BUILD_DIR)/misc/libfirmament_misc.a \
	$(BUILD_DIR)/messages/libfirmament_messages.a \
	$(BUILD_DIR)/messages/libfirmament_scheduling.a \
	$(BUILD_DIR)/messages/libfirmament_storage.a \
	$(BUILD_DIR)/platforms/unix/libfirmament_unix.a
	$(call quiet-command, \
		$(CXX) $(CPPFLAGS) \
		$(SRC_ROOT_DIR)/engine/coordinator_main.cc \
		$(OPTFLAGS) $(DEFINES) \
		$(SRC_ROOT_DIR)/engine/coordinator.cc \
		$(BUILD_DIR)/engine/node.o \
		$(BUILD_DIR)/engine/knowledge_base.o \
		$(BUILD_DIR)/engine/coordinator_http_ui.o \
		$(BUILD_DIR)/engine/local_executor.o \
		$(BUILD_DIR)/engine/remote_executor.o \
		$(BUILD_DIR)/engine/topology_manager.o \
		$(BUILD_DIR)/platforms/common.pb.o \
		$(BUILD_DIR)/sim/simple_dtg_generator.o \
		$(BUILD_DIR)/messages/libfirmament_messages.a \
		$(BUILD_DIR)/scheduling/libfirmament_scheduling.a \
		$(BUILD_DIR)/base/libfirmament_base.a \
		$(BUILD_DIR)/misc/libfirmament_misc.a \
		$(BUILD_DIR)/platforms/unix/libfirmament_unix.a \
		$(BUILD_DIR)/storage/libfirmament_storage.a \
		$(LIBS) $(HTTP_UI_LIBS) \
		-o $(BUILD_DIR)/engine/coordinator_sim \
		-D__SIMULATE_SYNTHETIC_DTG__, \
		"  DYNLNK  $(BUILD_DIR)/engine/coordinator_sim")

$(OBJ_DIR)/worker: $(addprefix $(OBJ_DIR)/, $(OBJS)) \
	$(BUILD_DIR)/platforms/unix/libfirmament_unix.a \
	$(BUILD_DIR)/base/libfirmament_base.a
	$(call quiet-command, \
		$(CXX) $(CPPFLAGS) \
		$(SRC_ROOT_DIR)/engine/worker_main.cc \
		$(OPTFLAGS) $(DEFINES) \
		$(BUILD_DIR)/engine/node.o \
		$(BUILD_DIR)/engine/worker.o \
		$(BUILD_DIR)/messages/libfirmament_messages.a \
		$(BUILD_DIR)/base/libfirmament_base.a \
		$(BUILD_DIR)/platforms/common.pb.o \
		$(BUILD_DIR)/platforms/unix/libfirmament_unix.a \
		$(BUILD_DIR)/storage/libfirmament_storage.a \
		$(LIBS) -o $(BUILD_DIR)/engine/worker, \
		"  DYNLNK  $(BUILD_DIR)/engine/worker")
#		$(BUILD_DIR)/engine/local_executor.o \


# Task interface library
# ---------------------------
# This needs to be linked into task binaries.
$(OBJ_LIB): $(addprefix $(OBJ_DIR)/, task_lib.o)
