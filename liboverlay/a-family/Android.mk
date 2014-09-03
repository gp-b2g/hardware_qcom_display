# Copyright (C) 2008 The Android Open Source Project
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := qcom/display
LOCAL_COPY_HEADERS := overlayLib.h
LOCAL_COPY_HEADERS += overlayLibUI.h
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_SHARED_LIBRARIES := liblog libcutils libutils libmemalloc
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcom/display
LOCAL_SRC_FILES := \
    overlayLib.cpp \
    overlayLibUI.cpp \
LOCAL_CFLAGS:= -DLOG_TAG=\"OverlayLib\"

ifeq ($(TARGET_USE_HDMI_AS_PRIMARY),true)
LOCAL_CFLAGS += -DHDMI_AS_PRIMARY
endif

ifeq ($(call is-chipset-prefix-in-board-platform,msm7627),true)
LOCAL_CFLAGS += -DDISPLAY_MDP_REV_4_OR_LESS
else ifeq ($(call is-chipset-prefix-in-board-platform,msm7630),true)
LOCAL_CFLAGS += -DDISPLAY_MDP_REV_4_OR_LESS
endif

ifeq ($(TARGET_USES_POST_PROCESSING),true)
LOCAL_CFLAGS += -DUSES_POST_PROCESSING
LOCAL_SHARED_LIBRARIES += libmm-abl
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/pp/inc
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/lib/
endif
LOCAL_MODULE := liboverlay
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
