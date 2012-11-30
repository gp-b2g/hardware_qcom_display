/*
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OVERLAY_HDMI_PIPE_H
#define OVERLAY_HDMI_PIPE_H

#include "overlayGenPipe.h"
#include "overlayUtils.h"
#include "overlayCtrl.h"
#include "overlayData.h"
#include "overlayMdp.h"
#include "overlayRotator.h"

namespace overlay2 {

/* A specific impl of GenericPipe
 * Whenever needed to have a pass through - we do it.
 * If there is a special need for a different behavior - do it here */
class HdmiPipe : utils::NoCopy {
public:
   /* Please look at overlayGenPipe.h for info */
   explicit HdmiPipe();
   ~HdmiPipe();
   bool open(RotatorBase* rot);
   bool close();
   bool commit();
   void setId(int id);
   void setMemoryId(int id);
   bool queueBuffer(uint32_t offset);
   bool dequeueBuffer(void*& buf);
   bool waitForVsync();
   bool setCrop(const utils::Dim& dim);
   bool start(const utils::PipeArgs& args);
   bool setPosition(const utils::Dim& dim);
   bool setParameter(const utils::Params& param);
   bool setSource(const utils::PipeArgs& args);
   const utils::PipeArgs& getArgs() const;
   utils::eOverlayPipeType getOvPipeType() const;
   void dump() const;
private:
   overlay2::GenericPipe<ovutils::FB1> mHdmi;
};

} // overlay2

#include "overlayHdmiPipe.inl"

#endif // OVERLAY_HDMI_PIPE_H
