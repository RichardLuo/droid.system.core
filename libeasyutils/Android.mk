# Copyright 2006 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES +=	\
	external/cppformat

LOCAL_SRC_FILES:= easyutils.cpp

LOCAL_SHARED_LIBRARIES := libcppformat liblog

LOCAL_MODULE:= libeasyutils

LOCAL_MODULE_TAGS := eng

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libstlport libdl
endif

include $(BUILD_SHARED_LIBRARY)
