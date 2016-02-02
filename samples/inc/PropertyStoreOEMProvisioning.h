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

#ifndef _PROPERTYSTOREOEMPROVISIONING_H_
#define _PROPERTYSTOREOEMPROVISIONING_H_

/** @defgroup PropertyStoreOEMProvisioning
 *
 *  @{
 */

#include <ajtcl/alljoyn.h>
#include <ajtcl/aj_config.h>
#include <ajtcl/services/ServicesCommon.h>
#include <ajtcl/services/PropertyStore.h>

/**
 * Device manufacture name
 */
extern const char* deviceManufactureName;

/**
 * Device product name
 */
extern const char* deviceProductName;

#define LANG_VALUE_LENGTH 7
#define KEY_VALUE_LENGTH 10
#define MACHINE_ID_LENGTH (UUID_LENGTH * 2)
#define DEVICE_NAME_VALUE_LENGTH 32
#define PASSWORD_VALUE_LENGTH (AJ_ADHOC_LEN * 2)

extern const char* const* propertyStoreDefaultLanguages;   // A NULL termminated list of language strings

/**
 * A convenience macro for the number of languages
 */
#define AJSVC_PROPERTY_STORE_NUMBER_OF_LANGUAGES (AJSVC_PropertyStore_GetNumberOfLanguages()) // The number of language strings calculated upon PropertyStore initialization

/**
 * property structure
 */
typedef struct _PropertyStoreEntry {
    const char* keyName; // The property key name as shown in About and Config documentation

    // msb=public/private; bit number 3 - initialise once; bit number 2 - multi-language value; bit number 1 - announce; bit number 0 - read/write
    uint8_t mode0Write : 1;
    uint8_t mode1Announce : 1;
    uint8_t mode2MultiLng : 1;
    uint8_t mode3Init : 1;
    uint8_t mode4 : 1;
    uint8_t mode5 : 1;
    uint8_t mode6 : 1;
    uint8_t mode7Public : 1;
} PropertyStoreEntry;

/**
 * properties array variable with property definitions
 */
extern const PropertyStoreEntry propertyStoreProperties[AJSVC_PROPERTY_STORE_NUMBER_OF_KEYS+5];

/**
 * properties array variable with default values
 */
extern const char** propertyStoreDefaultValues[AJSVC_PROPERTY_STORE_NUMBER_OF_KEYS+5]; // Array of Array of size 1 or AJSVC_PROPERTY_STORE_NUMBER_OF_LANGUAGES constant buffers depending on whether the property is multilingual

/**
 * properties container for runtime values
 */
typedef struct _PropertyStoreRuntimeEntry {
    char** value; // An array of size 1 or AJSVC_PROPERTY_STORE_NUMBER_OF_LANGUAGES mutable buffers depending on whether the property is multilingual
    uint8_t size; // The size of the value buffer(s)
} PropertyStoreConfigEntry;

/**
 * properties runtime array variable with runtime values of dynamically initialized and configurable properties
 */
extern PropertyStoreConfigEntry propertyStoreRuntimeValues[AJSVC_PROPERTY_STORE_NUMBER_OF_RUNTIME_KEYS];

#define AJ_PROPERTIES_NV_ID_END   (AJ_PROPERTIES_NV_ID_BEGIN + (int)AJSVC_PROPERTY_STORE_NUMBER_OF_RUNTIME_KEYS * (int)AJSVC_PROPERTY_STORE_NUMBER_OF_LANGUAGES - 1)

AJ_Status PropertyStore_Init();

/**
 * DeviceType
 */
typedef enum {
    ROOT                 = 0,
    OTHER                = 1,
    REFRIGERATOR         = 2,
    FREEZER              = 3,
    ICE_MAKER            = 4,
    AIR_CONDIRIONER      = 5,
    THERMOSTAT           = 6,
    HUMIDIFIER           = 7,
    DEHUMIDIFIER         = 8,
    AIR_PURIFIER         = 9,
    ELECTRIC_FAN         = 10,
    AIR_QUALITY_MONITOR  = 11,
    CLOTHES_WASHER       = 12,
    CLOTHES_DRYER        = 13,
    CLOTHES_WASHER_DRYER = 14,
    DISH_WASHER          = 15,
    ROBOT_CLEANER        = 16,
    OVEN                 = 17,
    COOKER_HOOD          = 18,
    COOKTOP              = 19,
    FOOD_PROBE           = 20,
    TELEVISION           = 21,
    SET_TOP_BOX          = 22,

    NUMBER_OF_DEVICE_TYPE,
    MAX_DEVICE_TYPE      = NUMBER_OF_DEVICE_TYPE - 1
} DeviceType;

/**
 * DeviceTypeDescription
 */
typedef struct _DeviceTypeDescription {
    DeviceType type;
    char* objectpath;
} DeviceTypeDescription;

extern DeviceTypeDescription deviceTypeDescription[];



/** @} */
 #endif //_PROPERTYSTOREOEMPROVISIONING_H_
