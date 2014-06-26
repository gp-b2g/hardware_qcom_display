/*
 * Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
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
 *    * Neither the name of The Linux Foundation nor the names of its
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

namespace overlay2 {

   inline HdmiPipe::HdmiPipe() {}
   inline HdmiPipe::~HdmiPipe() { close(); }
   inline bool HdmiPipe::open(RotatorBase* rot) {
      LOGE_IF(DEBUG_OVERLAY, "HdmiPipe open");
      return mHdmi.open(rot);
   }
   inline bool HdmiPipe::close() { return mHdmi.close(); }
   inline bool HdmiPipe::commit() { return mHdmi.commit(); }
   inline void HdmiPipe::setId(int id) { mHdmi.setId(id); }
   inline void HdmiPipe::setMemoryId(int id) { mHdmi.setMemoryId(id); }
   inline bool HdmiPipe::queueBuffer(uint32_t offset) { 
      return mHdmi.queueBuffer(offset); }
   inline bool HdmiPipe::dequeueBuffer(void*& buf) {
      return mHdmi.dequeueBuffer(buf); }
   inline bool HdmiPipe::waitForVsync() { 
      return mHdmi.waitForVsync(); }
   inline bool HdmiPipe::setCrop(const utils::Dim& dim) {
      return mHdmi.setCrop(dim); }
   inline bool HdmiPipe::start(const utils::PipeArgs& args) {
      return mHdmi.start(args); }
   inline bool HdmiPipe::setPosition(const utils::Dim& dim)
   {
      utils::Dim d;
      // Need to change dim to aspect ratio
      if (utils::FrameBufferInfo::getInstance()->supportTrueMirroring()) {
         // Use dim info to calculate aspect ratio for true UI mirroring
         d = mHdmi.getAspectRatio(dim);
      } else {
         // Use cached crop data to get aspect ratio
         utils::Dim crop = mHdmi.getCrop();
         utils::Whf whf(crop.w, crop.h, 0);
         d = mHdmi.getAspectRatio(whf);
      }
      LOGE_IF(DEBUG_OVERLAY, "Calculated aspect ratio for HDMI: x=%d, y=%d, w=%d, h=%d, o=%d",
              d.x, d.y, d.w, d.h, d.o);
      return mHdmi.setPosition(d);
   }
   inline bool HdmiPipe::setParameter(const utils::Params& param) {
      return mHdmi.setParameter(param); }
   inline bool HdmiPipe::setSource(const utils::PipeArgs& args) {
      utils::PipeArgs arg(args);
      return mHdmi.setSource(arg);
   }
   inline const utils::PipeArgs& HdmiPipe::getArgs() const {
      return mHdmi.getArgs();
   }
   inline utils::eOverlayPipeType HdmiPipe::getOvPipeType() const {
      return utils::OV_PIPE_TYPE_HDMI;
   }
   inline void HdmiPipe::dump() const {
      LOGE("HDMI Pipe");
      mHdmi.dump();
   }

}// overlay2
