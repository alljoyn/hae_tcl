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

#ifndef CDMCONTROLLEE_H_
#define CDMCONTROLLEE_H_

#include <ajtcl/services/ServicesCommon.h>
#include <ajtcl/alljoyn.h>

typedef enum { false, true } bool;

typedef enum {
    UNDEFINED_INTERFACE             = 0,
    //INPUT
    HID_INTERFACE,

    //Environment
    CURRENT_AIR_QUALITY_INTERFACE,
    CURRENT_AIR_QUALITY_LEVEL_INTERFACE,
    CURRENT_HUMIDITY_INTERFACE,
    CURRENT_TEMPERATURE_INTERFACE,
    TARGET_HUMIDITY_INTERFACE,
    TARGET_TEMPERATURE_INTERFACE,
    TARGET_TEMPERATURE_LEVEL_INTERFACE,
    WATER_LEVEL_INTERFACE,
    WIND_DIRECTION_INTERFACE,

    //Operation
    AIR_RECIRCULATION_MODE_INTERFACE,
    ALERTS_INTERFACE,
    AUDIO_VIDEO_INPUT_INTERFACE,
    AUDIO_VOLUME_INTERFACE,
    BATTERY_STATUS_INTERFACE,
    CHANNEL_INTERFACE,
    CLIMATE_CONTROL_MODE_INTERFACE,
    CLOSED_STATUS_INTERFACE,
    CURRENT_POWER_INTERFACE,
    CYCLE_CONTROL_INTERFACE,
    DISH_WASHING_CYCLE_PHASE_INTERFACE,
    ENERGY_USAGE_INTERFACE,
    FAN_SPEED_LEVEL_INTERFACE,
    FILTER_STATUS_INTERFACE,
    HEATING_ZONE_INTERFACE,
    HVAC_FAN_MODE_INTERFACE,
    LAUNDRY_CYCLE_PHASE_INTERFACE,
    MOISTURE_OUTPUT_LEVEL_INTERFACE,
    ON_OFF_STATUS_INTERFACE,
    OFF_CONTROL_INTERFACE,
    ON_CONTROL_INTERFACE,
    OVEN_CYCLE_PHASE_INTERFACE,
    PLUG_IN_UNITS_INTERFACE,
    RAPID_MODE_INTERFACE,
    RAPID_MODE_TIMED_INTERFACE,
    REMOTE_CONTROLLABILITY_INTERFACE,
    REPEAT_MODE_INTERFACE,
    RESOURCE_SAVING_INTERFACE,
    ROBOT_CLEANING_CYCLE_PHASE_INTERFACE,
    SOIL_LEVEL_INTERFACE,
    SPIN_SPEED_LEVEL_INTERFACE,
    TIMER_INTERFACE,

    //UserInterfaceSettings
    LANGUAGE_DISPLAY_INTERFACE,
    TEMPERATURE_DISPLAY_INTERFACE,
    TIME_DISPLAY_INTERFACE,

    //////////////////////////////
    LAST_BASIC_INTERFACE_TYPE,
    NUMBER_OF_BASIC_INTERFACE_TYPE,
    MAX_BASIC_INTERFACE_TYPE        = NUMBER_OF_BASIC_INTERFACE_TYPE,

    // Vendor Defined Interface
    VENDOR_DEFINED_INTERFACE        = 0x1000,

} CdmInterfaceTypes;

typedef struct {
    void* properties;
    uint32_t member_index_mask;
} CdmPropertiesChangedByMethod;

/**
 * Vendor defined interface handler
 */
typedef struct {
    /**
     * Handler called when the interface is registered.
     * @param[in] intfType registered interface type
     */
    void (*InterfaceRegistered)(CdmInterfaceTypes intfType);

    /**
     * Handler called when the interface is created.
     * @param[out] properties pointer to properties belonged to the interface. Memory allocation is needed if property is exist.
     * @return AJ_OK on success
     */
    AJ_Status (*InterfaceCreator)(void** properties);

    /**
     * Handler called when the interface is destroyed.
     * @param[out] properties pointer to properties belonged to the interface. Memory deallocation is needed if property is exist.
     */
    void (*InterfaceDestructor)(void* properties);

    /**
     * Handler called when getting property value is requested.
     * @param[in] replyMsg reply message
     * @param[in] objPath the object path including the interface
     * @param[in] properties pointer to properties belonged to the interface
     * @param[in] memberIndex index of the property in the interface description
     * @param[in] listener interface listener
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetProperty)(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener);

    /**
     * Handler called when setting property value is requested.
     * @param[in] replyMsg reply message
     * @param[in] objPath the object path including the interface
     * @param[in] properties pointer to properties belonged to the interface
     * @param[in] memberIndex index of the property in the interface description
     * @param[in] listener interface listener
     * @param[out] propChanged whether the property value is changed
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetProperty)(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged);

    /**
     * Emit PropertiesChanged signal.
     * @param[in] busAttachment bus attachment
     * @param[in] objPath the object path including the interface
     * @param[in] properties pointer to properties belonged to the interface
     * @param[in] memberIndex index of the property in the interface description
     * @return AJ_OK on success
     */
    AJ_Status (*EmitPropertiesChanged)(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex);

    /**
     * Handler called when method is called.
     * @param[in] msg message
     * @param[in] objPath the object path including the interface
     * @param[in] memberIndex index of the method in the interface description
     * @param[in] listener interface listener
     * @param[in] propChangedByMethod includes pointer to properties belonged to the interface. The change of properties values shall be recorded to this.
     * @return AJ_OK on success
     */
    AJ_Status (*OnMethodHandler)(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, CdmPropertiesChangedByMethod* propChangedByMethod);
} VendorDefinedInterfaceHandler;

/**
 * Initialize CDM service framework.
 * @return AJ_OK on success.
 */
AJ_Status Cdm_Init();

/**
 * Deinitialize CDM service framework.
 */
void Cdm_Deinit();

/**
 * Create interface.
 * @param[in] intfType interface type
 * @param[in] objPath the object path including the interface
 * @param[in] listener interface listener
 * @return AJ_OK on success
 */
AJ_Status Cdm_CreateInterface(CdmInterfaceTypes intfType, const char* objPath, void* listener);

/**
 * Start CDM service framework.
 * Objects with interfaces are registered.
 * @return AJ_OK on success
 */
AJ_Status Cdm_Start();

/**
 * Enable security.
 * @param[in] busAttachment bus attachment
 * @param[in] suites the authentication suites to enable
 * @param[in] numOfSuites the number of authentication suites
 * @param[in] authListenerCallback the auth listener callback function
 * @return AJ_OK on success
 */
AJ_Status Cdm_EnableSecurity(AJ_BusAttachment* busAttachment, const uint32_t* suites, const size_t numOfSuites,
                             AJ_AuthListenerFunc authListenerCallback);

/**
 * Function used to process CDM messages.
 * @param[in] busAttachment bus attachment
 * @param[in] msg message
 * @param[out] status status
 * @return service status
 */
AJSVC_ServiceStatus Cdm_MessageProcessor(AJ_BusAttachment* busAttachment, AJ_Message* msg, AJ_Status* status);

/**
 * Register vendor defined interface.
 * @param[in] intfName vendor defined interface name
 * @param[in] intfDesc vendor defined interface description
 * @param[in] handler vendor defined interface handler
 * @param[out] intfType registered interface type
 * @return AJ_OK on success
 */
AJ_Status Cdm_RegisterVendorDefinedInterface(const char* intfName, const char* const* intfDesc, VendorDefinedInterfaceHandler* handler, CdmInterfaceTypes* intfType);

/**
 * Get pointer to properties belonged to the interface
 * @param[in] objPath the object path including the interface
 * @param[in] intfType interface type
 * @return pointer to properties belonged to the interface
 */
void* GetProperties(const char* objPath, CdmInterfaceTypes intfType);

/**
 * Make message identifier (mainly used for emitting signal)
 * @param[in] objPath the object path including the interface
 * @param[in] intfType interface type
 * @param[in] memberIndex index of the member in the interface description
 * @param[out] msgId message identifier made
 * @return AJ_OK on success
 */
AJ_Status MakeMsgId(const char* objPath, CdmInterfaceTypes intfType, uint8_t memberIndex, uint32_t* msgId);

/**
 * Make message identifier for PropertiesChanged signal
 * @param[in] objPath the object path including the interface
 * @param[out] msgId message identifier made
 * @return AJ_OK on success
 */
AJ_Status MakePropChangedId(const char* objPath, uint32_t* msgId);

#endif // CDMCONTROLLEE_H_
