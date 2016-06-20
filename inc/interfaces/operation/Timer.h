/**
 * @file
 */
/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>
#include <ajtcl/hae/interfaces/HaeInterfaceErrors.h>

#define TIMER_FEATURE_NOT_APPLIED 0x80000000
#define TIMER_NOT_SUPPORTED 0x80000001


/**
 * Timer interface listener
 */
typedef struct {

    /**
     * Handler for getting OnGetReferenceTimer
     * @param[in] objPath object path
     * @param[out] referenceTimer reference timer
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetReferenceTimer) (const char* objPath, int32_t* referenceTimer);

    /**
     * Handler for getting OnGetTargetTimeToStart
     * @param[in] objPath object path
     * @param[out] targetTimeToStart target time to start
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetTargetTimeToStart) (const char* objPath, int32_t* targetTimeToStart);

    /**
     * Handler for getting OnGetTargetTimeToStop
     * @param[in] objPath object path
     * @param[out] targetTimeToStop target time to stop
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetTargetTimeToStop) (const char* objPath, int32_t* targetTimeToStop);

    /**
     * Handler for getting OnGetEstimatedTimeToEnd
     * @param[in] objPath object path
     * @param[out] estimatedTimeToEnd estimated time to end
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetEstimatedTimeToEnd) (const char* objPath, int32_t* estimatedTimeToEnd);

    /**
     * Handler for getting OnGetRunningTime
     * @param[in] objPath object path
     * @param[out] runningTime running time
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetRunningTime) (const char* objPath, int32_t* runningTime);

    /**
     * Handler for getting OnGetTargetDuration
     * @param[in] objPath object path
     * @param[out] targetDuration target duration
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetTargetDuration) (const char* objPath, int32_t* targetDuration);

    /**
     * Handler for SetTargetTimeToStart method
     * @param[in] objPath object path
     * @param[in] targetTimeToStart target time to start
     * @param[out] errorCode error code
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetTargetTimeToStart) (const char* objPath, const int32_t targetTimeToStart, ErrorCode* errorCode);

    /**
     * Handler for SetTargetTimeToStop method
     * @param[in] objPath object path
     * @param[in] targetTimeToStop target time to stop
     * @param[out] errorCode error code
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetTargetTimeToStop) (const char* objPath, const int32_t targetTimeToStop, ErrorCode* errorCode);

} TimerListener;

/**
 * Get ReferenceTimer of Timer interface
 * @param[in] objPath the object path including the interface
 * @param[out] referenceTimer reference timer
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceGetReferenceTimer(const char* objPath, int32_t* referenceTimer);

/**
 * Set ReferenceTimer of Timer interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] referenceTimer reference timer
 * @return AJ_OK on success

 */
AJ_Status Hae_TimerInterfaceSetReferenceTimer(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t referenceTimer);

/**
 * Get TargetTimeToStart of Timer interface
 * @param[in] objPath the object path including the interface
 * @param[out] targetTimeToStart target time to start
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceGetTargetTimeToStart(const char* objPath, int32_t* targetTimeToStart);

/**
 * Set TargetTimeToStart of Timer interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] targetTimeToStart target time to start
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceSetTargetTimeToStart(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t targetTimeToStart);

/**
 * Get TargetTimeToStop of Timer interface
 * @param[in] objPath the object path including the interface
 * @param[out] targetTimeToStop target time to stop
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceGetTargetTimeToStop(const char* objPath, int32_t* targetTimeToStop);

/**
 * Set TargetTimeToStop of Timer interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] targetTimeToStop target time to stop
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceSetTargetTimeToStop(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t targetTimeToStop);

/**
 * Get EstimatedTimeToEnd of Timer interface
 * @param[in] objPath the object path including the interface
 * @param[out] estimatedTimeToEnd estimated time to end
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceGetEstimatedTimeToEnd(const char* objPath, int32_t* estimatedTimeToEnd);

/**
 * Set EstimatedTimeToEnd of Timer interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] estimatedTimeToEnd estimated time to end
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceSetEstimatedTimeToEnd(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t estimatedTimeToEnd);

/**
 * Get RunningTime of Timer interface
 * @param[in] objPath the object path including the interface
 * @param[out] runningTime running time
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceGetRunningTime(const char* objPath, int32_t* runningTime);

/**
 * Set RunningTime of Timer interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] runningTime running time
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceSetRunningTime(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t runningTime);

/**
 * Get TargetDuration of Timer interface
 * @param[in] objPath the object path including the interface
 * @param[out] targetDuration target duration
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceGetTargetDuration(const char* objPath, int32_t* targetDuration);

/**
 * Set TargetDuration of Timer interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] targetDuration target duration
 * @return AJ_OK on success
 */
AJ_Status Hae_TimerInterfaceSetTargetDuration(AJ_BusAttachment* busAttachment, const char* objPath, const int32_t targetDuration);

#endif /* TIMER_H_ */
