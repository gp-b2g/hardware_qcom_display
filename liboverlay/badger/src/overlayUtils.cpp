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

#include <stdlib.h>
#include <utils/Log.h>
#include <linux/msm_mdp.h>
#include <cutils/properties.h>
#include "gralloc_priv.h"
#include "fb_priv.h"
#include "overlayFD.h"
#include "overlayRes.h"
#include "mdpWrapper.h"

// just a helper static thingy
namespace {
   struct IOFile {
      IOFile(const char* s, const char* mode) : fp(0) {
         fp = ::fopen(s, mode);
         if(!fp) {
            LOGE("Failed open %s", s);
         }
      }
      template <class T>
      size_t read(T& r, size_t elem) {
         if(fp) {
            return ::fread(&r, sizeof(T), elem, fp);
         }
         return 0;
      }
      size_t write(const char* s, uint32_t val) {
         if(fp) {
            return ::fprintf(fp, s, val);
         }
         return 0;
      }
      bool valid() const { return fp != 0; }
      ~IOFile() {
         if(fp) ::fclose(fp);
         fp=0;
      }
      FILE* fp;
   };
}

namespace overlay2 {

namespace utils {

   FrameBufferInfo::FrameBufferInfo() {
      mFBWidth = 0;
      mFBHeight = 0;
      mBorderFillSupported = false;

      OvFD mFd;

      // Use open defined in overlayFD file to open fd for fb0
      if(!overlay2::open(mFd, 0, Res::devTemplate)) {
         LOGE("FrameBufferInfo: failed to open fd");
         return;
      }

      if (!mFd.valid()) {
         LOGE("FrameBufferInfo: FD not valid");
         return;
      }

      fb_var_screeninfo vinfo;
      if (!mdp_wrapper::getVScreenInfo(mFd.getFD(), vinfo)) {
         LOGE("FrameBufferInfo: failed getVScreenInfo on fb0");
         mFd.close();
         return;
      }

      mdp_overlay ov;
      memset(&ov, 0, sizeof(ov));
      ov.id = 1;
      if (!mdp_wrapper::getOverlay(mFd.getFD(), ov)) {
         LOGE("FrameBufferInfo: failed getOverlay on fb0");
         mFd.close();
         return;
      }

      mFd.close();

      mFBWidth = vinfo.xres;
      mFBHeight = vinfo.yres;
      mBorderFillSupported = (ov.flags & MDP_BORDERFILL_SUPPORTED) ?
                              true : false;
   }

   FrameBufferInfo* FrameBufferInfo::getInstance() {
      if (!sFBInfoInstance) {
         sFBInfoInstance = new FrameBufferInfo;
      }
      return sFBInfoInstance;
   }

   int FrameBufferInfo::getWidth() const {
      return mFBWidth;
   }

   int FrameBufferInfo::getHeight() const {
      return mFBHeight;
   }

   bool FrameBufferInfo::supportTrueMirroring() const {
      return mBorderFillSupported;
   }

   uint32_t getSize(const Whf& whf) {
      int aligned_height=0, pitch=0;

      uint32_t size = whf.w * whf.h;
      switch (whf.format) {
      case MDP_RGBA_8888:
      case MDP_BGRA_8888:
      case MDP_RGBX_8888:
         size *= 4;
         break;
      case MDP_RGB_565:
      case MDP_Y_CBCR_H2V1:
         size *= 2;
         break;
      case MDP_Y_CBCR_H2V2:
      case MDP_Y_CRCB_H2V2:
         size = (size * 3) / 2;
         break;
      case MDP_Y_CRCB_H2V2_TILE:
         aligned_height = align(whf.h , 32);
         pitch = align(whf.w, 128);
         size = pitch * aligned_height;
         size = align(size, 8192);

         aligned_height = align(whf.h >> 1, 32);
         size += pitch * aligned_height;
         size = align(size, 8192);
         break;
      default:
         LOGE("getSize unknown format %d", whf.format);
         return 0;
      }
      return size;
   }

   int getMdpFormat(int format) {
      switch (format) {
      case HAL_PIXEL_FORMAT_RGBA_8888 :
         return MDP_RGBA_8888;
      case HAL_PIXEL_FORMAT_BGRA_8888:
         return MDP_BGRA_8888;
      case HAL_PIXEL_FORMAT_RGB_565:
         return MDP_RGB_565;
      case HAL_PIXEL_FORMAT_RGBX_8888:
         return MDP_RGBX_8888;
      case HAL_PIXEL_FORMAT_YCbCr_422_SP:
         return MDP_Y_CBCR_H2V1;
      case HAL_PIXEL_FORMAT_YCrCb_422_SP:
         return MDP_Y_CRCB_H2V1;
      case HAL_PIXEL_FORMAT_YCbCr_420_SP:
         return MDP_Y_CBCR_H2V2;
      case HAL_PIXEL_FORMAT_YCrCb_420_SP:
         return MDP_Y_CRCB_H2V2;
      case HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED:
         return MDP_Y_CBCR_H2V2_TILE;
      case HAL_PIXEL_FORMAT_YV12:
         return MDP_Y_CR_CB_H2V2;
      default:
         LOGE("Error getMdpFormat format=%d", format);
         return -1;
      }
      // not reached
      return -1;
   }

   bool isHDMIConnected () {
      char value[PROPERTY_VALUE_MAX] = {0};
      property_get("hw.hdmiON", value, "0");
      int isHDMI = atoi(value);
      return isHDMI ? true : false;
   }

   bool is3DTV() {
      char is3DTV = '0';
      IOFile fp(Res::edid3dInfoFile, "r");
      (void)fp.read(is3DTV, 1);
      LOGI("3DTV EDID flag: %d", is3DTV);
      return (is3DTV == '0') ? false : true;
   }

   bool isPanel3D() {
      OvFD fd;
      if(!overlay2::open(fd, 0 /*fb*/, Res::devTemplate)){
         LOGE("isPanel3D Can't open framebuffer 0");
         return false;
      }
      fb_fix_screeninfo finfo;
      if(!mdp_wrapper::getFScreenInfo(fd.getFD(), finfo)) {
         LOGE("isPanel3D read fb0 failed");
      }
      fd.close();
      return (FB_TYPE_3D_PANEL == finfo.type) ? true : false;
   }

   bool usePanel3D() {
      if(!isPanel3D())
         return false;
      char value[PROPERTY_VALUE_MAX];
      property_get("persist.user.panel3D", value, "0");
      int usePanel3D = atoi(value);
      return usePanel3D ? true : false;
   }

   bool send3DInfoPacket (uint32_t format3D) {
      IOFile fp(Res::format3DFile, "wb");
      (void)fp.write("%d", format3D);
      if(!fp.valid()) {
         LOGE("send3DInfoPacket: no sysfs entry for setting 3d mode");
         return false;
      }
      return true;
   }

   bool enableBarrier (uint32_t orientation) {
      IOFile fp(Res::barrierFile, "wb");
      (void)fp.write("%d", orientation);
      if(!fp.valid()) {
         LOGE("enableBarrier no sysfs entry for "
              "enabling barriers on 3D panel");
         return false;
      }
      return true;
   }

   uint32_t getS3DFormat(uint32_t fmt) {
      // The S3D is part of the HAL_PIXEL_FORMAT_YV12 value. Add
      // an explicit check for the format
      if (fmt == HAL_PIXEL_FORMAT_YV12) {
         return 0;
      }
      uint32_t fmt3D = format3D(fmt);
      uint32_t fIn3D = format3DInput(fmt3D); // MSB 2 bytes - inp
      uint32_t fOut3D = format3DOutput(fmt3D); // LSB 2 bytes - out
      fmt3D = fIn3D | fOut3D;
      if (!fIn3D) {
         fmt3D |= fOut3D << SHIFT_TOT_3D; //Set the input format
      }
      if (!fOut3D) {
         switch (fIn3D) {
         case HAL_3D_IN_SIDE_BY_SIDE_L_R:
         case HAL_3D_IN_SIDE_BY_SIDE_R_L:
            // For all side by side formats, set the output
            // format as Side-by-Side i.e 0x1
            fmt3D |= HAL_3D_IN_SIDE_BY_SIDE_L_R >> SHIFT_TOT_3D;
            break;
         default:
            fmt3D |= fIn3D >> SHIFT_TOT_3D; //Set the output format
         }
      }
      return fmt3D;
   }

   void normalizeCrop(uint32_t& xy, uint32_t& wh) {
      if (xy & 0x0001) {
         // x or y is odd, increment it's value
         xy += 1;
         // Since we've incremented x(y), we need to decrement
         // w(h) accordingly
         if (wh & 0x0001) {
            // w or h is odd, decrement it by 1, to make it even
            even_out(wh);
         } else {
            // w(h) is already even, hence we decrement by 2
            wh -=2;
         }
      } else {
         even_out(wh);
      }
   }

   void scale(mdp_overlay& ov)
   {
      /* Scaling of upto a max of 8 times supported */
      overlay2::utils::Dim dst(overlay2::utils::getDstRectDim(ov));
      overlay2::utils::Dim src(overlay2::utils::getSrcRectDim(ov));
      if(dst.w >(src.w * overlay2::utils::HW_OV_MAGNIFICATION_LIMIT)) {
         dst.w = overlay2::utils::HW_OV_MAGNIFICATION_LIMIT * src.w;
      }
      if(dst.h >(src.h * overlay2::utils::HW_OV_MAGNIFICATION_LIMIT)) {
         dst.h = overlay2::utils::HW_OV_MAGNIFICATION_LIMIT * src.h;
      }

      setDstRectDim(dst, ov);
   }

} // utils

} // overlay2
