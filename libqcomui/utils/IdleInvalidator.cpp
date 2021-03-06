/*
 * Copyright (c) 2012, Code Aurora Forum. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#include "IdleInvalidator.h"
#include <unistd.h>

#define II_DEBUG 0

static const char *threadName = "Invalidator";
InvalidatorHandler IdleInvalidator::mHandler = NULL;
android::sp<IdleInvalidator> IdleInvalidator::sInstance(0);

IdleInvalidator::IdleInvalidator(): Thread(false), mHwcContext(0),
    mSleepAgain(false), mSleepTime(0) {
    LOGE_IF(II_DEBUG, "%s", __func__);
}

int IdleInvalidator::init(InvalidatorHandler reg_handler, void* user_data,
    unsigned int idleSleepTime) {
    LOGE_IF(II_DEBUG, "%s", __func__);

    /* store registered handler */
    mHandler = reg_handler;
    mHwcContext = user_data;
    mSleepTime = idleSleepTime; //Time in millis
    return 0;
}

bool IdleInvalidator::threadLoop() {
    LOGE_IF(II_DEBUG, "%s", __func__);
    usleep(mSleepTime * 1000);
    //If there are continuous mdp-comp updates, all we need to do is sleep over
    //and over again.
    if(mSleepAgain) {
        //We need to sleep again!
        mSleepAgain = false;
        return true;
    }

    //The handler is not a part of this class just because hwc_context_t
    //definition exists in hwc. Else there is no reason for it to be defined
    //there.
    mHandler((void*)mHwcContext);
    return false;
}

int IdleInvalidator::readyToRun() {
    LOGE_IF(II_DEBUG, "%s", __func__);
    return 0; /*NO_ERROR*/
}

void IdleInvalidator::onFirstRef() {
    LOGE_IF(II_DEBUG, "%s", __func__);
}

void IdleInvalidator::markForSleep() {
    //There was an mdp comp update, so keep sleeping.
    mSleepAgain = true;
    //Triggers the threadLoop to run, if not already running.
    run(threadName, android::PRIORITY_AUDIO);
}

IdleInvalidator *IdleInvalidator::getInstance() {
    LOGE_IF(II_DEBUG, "%s", __func__);
    if(sInstance.get() == NULL)
        sInstance = new IdleInvalidator();
    return sInstance.get();
}
