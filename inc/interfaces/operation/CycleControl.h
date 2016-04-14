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

#ifndef CYCLECONTROL_H_
#define CYCLECONTROL_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>
#include <ajtcl/hae/interfaces/HaeInterfaceErrors.h>

/**
 * CycleControl interface listener
 */
typedef struct {
    /**
     * Handler for getting OperationalState property
     * @param[in] objPath object path
     * @param[out] operationalState operational state
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetOperationalState) (const char* objPath, uint8_t* operationalState);

    /**
     * Handler for getting SupportedOperationalStates property
     * @param[in] objPath object path
     * @param[out] supportedOperationalStates supported operational states
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetSupportedOperationalStates) (const char* objPath, uint8_t* supportedOperationalStates);

    /**
     * Handler for getting SupportedOperationalCommands property
     * @param[in] objPath object path
     * @param[out] supportedOperationalCommands supported operational commands
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetSupportedOperationalCommands) (const char* objPath, uint8_t* supportedOperationalCommands);

    /**
     * Handler for ExecuteOperationalCommand method
     * @param[in] objPath object path
     * @param[in] command command
     * @param[out] errorCode error code
     * @return AJ_OK on success
     */
    AJ_Status (*OnExecuteOperationalCommand) (const char* objPath, const uint8_t command, ErrorCode* errorCode);
} CycleControlListener;

/**
 * Get OperationalState of CycleControl interface
 * @param[in] objPath the object path including the interface
 * @param[out] operationalState operational state
 * @return AJ_OK on success
 */
AJ_Status Hae_CycleControlInterfaceGetOperationalState(const char* objPath, uint8_t* operationalState);

/**
 * Set OperationalState of CycleControl interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] operationalState number of heating zones
 * @return AJ_OK on success
 */
AJ_Status Hae_CycleControlInterfaceSetOperationalState(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t operationalState);

/**
 * Get SupportedOperationalStates of CycleControl interface
 * @param[in] objPath the object path including the interface
 * @param[out] supportedOperationalStates supported operational states
 * @return AJ_OK on success
 */
AJ_Status Hae_CycleControlInterfaceGetSupportedOperationalStates(const char* objPath, uint8_t* supportedOperationalStates);

/**
 * Set SupportedOperationalStates of CycleControl interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] supportedOperationalStates supported operational states
 * @param[in] supportedOperationalStatesSize supported operational states size
 * @return AJ_OK on success
 */
AJ_Status Hae_CycleControlInterfaceSetSupportedOperationalStates(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* supportedOperationalStates, const size_t supportedOperationalStatesSize);

/**
 * Get SupportedOperationalCommands of CycleControl interface
 * @param[in] objPath the object path including the interface
 * @param[out] supportedOperationalCommands supported operational commands
 * @return AJ_OK on success
 */
AJ_Status Hae_CycleControlInterfaceGetSupportedOperationalCommands(const char* objPath, uint8_t* supportedOperationalCommands);

/**
 * Set SupportedOperationalCommands of CycleControl interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] supportedOperationalCommands supported operational commands
 * @param[in] supportedOperationalCommandsSize supported operational commands size
 * @return AJ_OK on success
 */
AJ_Status Hae_CycleControlInterfaceSetSupportedOperationalCommands(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* supportedOperationalCommands, const size_t supportedOperationalCommandsSize);

/**
 * Emit ChannelListChanged signal of Channel interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] sessionId session id
 * @return AJ_OK on success
 */
AJ_Status Hae_ChannelInterfaceEmitEndOfCycle(AJ_BusAttachment* busAttachment, const char* objPath, const uint32_t sessionId);

#endif /* CYCLECONTROL_H_ */
