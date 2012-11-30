# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := qcom/display
LOCAL_COPY_HEADERS := gralloc_priv.h
LOCAL_COPY_HEADERS += gr.h
LOCAL_COPY_HEADERS += alloc_controller.h
LOCAL_COPY_HEADERS += memalloc.h
ifeq ($(call is-board-platform-in-list,copper),true)
LOCAL_COPY_HEADERS += badger/fb_priv.h
else
LOCAL_COPY_HEADERS += a-family/fb_priv.h
endif
include $(BUILD_COPY_HEADERS)

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES := liblog libcutils libGLESv1_CM libutils libmemalloc libQcomUI
LOCAL_SHARED_LIBRARIES += libgenlock

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcom/display
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_MODULE := gralloc.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS:= -DLOG_TAG=\"$(TARGET_BOARD_PLATFORM).gralloc\" -DHOST -DDEBUG_CALC_FPS

LOCAL_SRC_FILES :=  gpu.cpp         \
                    gralloc.cpp     \
                    mapper.cpp

ifeq ($(call is-board-platform-in-list,copper),true)
    LOCAL_SRC_FILES += badger/framebuffer.cpp
    LOCAL_C_INCLUDES += hardware/qcom/display/liboverlay/badger/src
    LOCAL_CFLAGS += -DUSE_OVERLAY2 #XXX: Remove later
else
    LOCAL_SRC_FILES += a-family/framebuffer.cpp
    ifeq ($(TARGET_USES_POST_PROCESSING),true)
    LOCAL_CFLAGS += -DUSES_POST_PROCESSING
    LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/pp/inc
    endif
endif


ifeq ($(call is-board-platform,msm7627_surf msm7627_6x),true)
    LOCAL_CFLAGS += -DTARGET_MSM7x27
endif
ifeq ($(TARGET_USES_MDP3), true)
    LOCAL_CFLAGS += -DUSE_MDP3
endif
ifeq ($(TARGET_HAVE_HDMI_OUT),true)
    LOCAL_CFLAGS += -DHDMI_DUAL_DISPLAY
    LOCAL_SHARED_LIBRARIES += liboverlay
endif

include $(BUILD_SHARED_LIBRARY)

#MemAlloc Library
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcom/display
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_SHARED_LIBRARIES := liblog libcutils libutils
LOCAL_SRC_FILES :=  ionalloc.cpp \
                    ashmemalloc.cpp \
                    pmemalloc.cpp \
                    pmem_bestfit_alloc.cpp \
                    alloc_controller.cpp
LOCAL_CFLAGS:= -DLOG_TAG=\"memalloc\"

ifeq ($(TARGET_USES_ION),true)
    LOCAL_CFLAGS += -DUSE_ION
endif

LOCAL_MODULE := libmemalloc
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
