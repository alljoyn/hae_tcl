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

#include <stdlib.h>
#include <string.h>
#include <ajtcl/hae/HaeControllee.h>
#include "HaeControlleeImpl.h"
#include "../interfaces/operation/AudioVolumeImpl.h"
#include "../interfaces/operation/AudioVideoInputImpl.h"
#include "../interfaces/operation/ChannelImpl.h"
#include "../interfaces/operation/ClosedStatusImpl.h"
#include "../interfaces/operation/CurrentPowerImpl.h"
#include "../interfaces/operation/CycleControlImpl.h"
#include "../interfaces/operation/DishWashingCyclePhaseImpl.h"
#include "../interfaces/operation/LaundryCyclePhaseImpl.h"
#include "../interfaces/operation/OnControlImpl.h"
#include "../interfaces/operation/OffControlImpl.h"
#include "../interfaces/operation/OnOffStatusImpl.h"
#include "../interfaces/operation/OvenCyclePhaseImpl.h"
#include "../interfaces/operation/FanSpeedLevelImpl.h"
#include "../interfaces/operation/BatteryStatusImpl.h"
#include "../interfaces/operation/ClimateControlModeImpl.h"
#include "../interfaces/operation/EnergyUsageImpl.h"
#include "../interfaces/operation/HeatingZoneImpl.h"
#include "../interfaces/operation/RapidModeImpl.h"
#include "../interfaces/operation/RemoteControllabilityImpl.h"
#include "../interfaces/operation/RepeatModeImpl.h"
#include "../interfaces/operation/ResourceSavingImpl.h"
#include "../interfaces/operation/AirRecirculationModeImpl.h"
#include "../interfaces/operation/RobotCleaningCyclePhaseImpl.h"
#include "../interfaces/operation/SoilLevelImpl.h"
#include "../interfaces/operation/SpinSpeedLevelImpl.h"
#include "../interfaces/operation/TimerImpl.h"
#include "../interfaces/operation/MoistureOutputLevelImpl.h"
#include "../interfaces/operation/FilterStatusImpl.h"
#include "../interfaces/environment/CurrentAirQualityImpl.h"
#include "../interfaces/environment/CurrentAirQualityLevelImpl.h"
#include "../interfaces/environment/CurrentTemperatureImpl.h"
#include "../interfaces/environment/TargetTemperatureImpl.h"
#include "../interfaces/environment/WaterLevelImpl.h"
#include "../interfaces/environment/WindDirectionImpl.h"
#include "../interfaces/input/HidImpl.h"
#include "../interfaces/environment/CurrentHumidityImpl.h"
#include "../interfaces/environment/TargetHumidityImpl.h"
#include "../interfaces/environment/TargetTemperatureLevelImpl.h"
#include "../interfaces/operation/HvacFanModeImpl.h"
#include "../interfaces/operation/PlugInUnitsImpl.h"
#include "../interfaces/operation/RapidModeTimedImpl.h"

#define HAE_OBJECT_LIST_INDEX AJAPP_OBJECTS_LIST_INDEX

static const char* const* intfDescs[MAX_BASIC_INTERFACE_TYPE];
static InterfaceCreator intfCreator[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static InterfaceDestructor intfDestructor[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static OnGetProperty onGetProperty[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static OnSetProperty onSetProperty[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static EmitPropertiesChanged emitPropertiesChanged[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static OnMethodHandler onMethodHandler[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static HaeObjectInfo* objInfoFirst;
static HaeObjectInfo* objInfoLast;
static AJ_Object* ajObjects;
static VendorDefinedInterfaceInfo* vendorDefinedIntfInfoFirst;
static VendorDefinedInterfaceInfo* vendorDefinedIntfInfoLast;

static VendorDefinedInterfaceInfo* GetVendorDefinedInterfaceInfo(HaeInterfaceTypes intfType)
{
    VendorDefinedInterfaceInfo* intfInfo = NULL;

    intfInfo = vendorDefinedIntfInfoFirst;
    while (intfInfo) {
        if (intfInfo->intfType == intfType) {
            break;
        }
        intfInfo = intfInfo->intfNext;
    }

    return intfInfo;
}

AJ_Status Hae_Init()
{
    intfDescs[AUDIO_VOLUME_INTERFACE] = intfDescOperationAudioVolume;
    intfDescs[AUDIO_VIDEO_INPUT_INTERFACE] = intfDescOperationAudioVideoInput;
    intfDescs[CHANNEL_INTERFACE] = intfDescOperationChannel;
    intfDescs[CLOSED_STATUS_INTERFACE] = intfDescOperationClosedStatus;
    intfDescs[CURRENT_POWER_INTERFACE] = intfDescOperationCurrentPower;
    intfDescs[CYCLE_CONTROL_INTERFACE] = intfDescOperationCycleControl;
    intfDescs[DISH_WASHING_CYCLE_PHASE_INTERFACE] = intfDescOperationDishWashingCyclePhase;
    intfDescs[LAUNDRY_CYCLE_PHASE_INTERFACE] = intfDescOperationLaundryCyclePhase;
    intfDescs[ON_CONTROL_INTERFACE] = intfDescOperationOnControl;
    intfDescs[OFF_CONTROL_INTERFACE] = intfDescOperationOffControl;
    intfDescs[ON_OFF_STATUS_INTERFACE] = intfDescOperationOnOffStatus;
    intfDescs[OVEN_CYCLE_PHASE_INTERFACE] = intfDescOperationOvenCyclePhase;
    intfDescs[FAN_SPEED_LEVEL_INTERFACE] = intfDescOperationFanSpeedLevel;
    intfDescs[BATTERY_STATUS_INTERFACE] = intfDescOperationBatteryStatus;
    intfDescs[CLIMATE_CONTROL_MODE_INTERFACE] = intfDescOperationClimateControlMode;
    intfDescs[ENERGY_USAGE_INTERFACE] = intfDescOperationEnergyUsage;
    intfDescs[HEATING_ZONE_INTERFACE] = intfDescOperationHeatingZone;
    intfDescs[RAPID_MODE_INTERFACE] = intfDescOperationRapidMode;
    intfDescs[REMOTE_CONTROLLABILITY_INTERFACE] = intfDescOperationRemoteControllability;
    intfDescs[REPEAT_MODE_INTERFACE] = intfDescOperationRepeatMode;
    intfDescs[RESOURCE_SAVING_INTERFACE] = intfDescOperationResourceSaving;
    intfDescs[AIR_RECIRCULATION_MODE_INTERFACE] = intfDescOperationAirRecirculationMode;
    intfDescs[ROBOT_CLEANING_CYCLE_PHASE_INTERFACE] = intfDescOperationRobotCleaningCyclePhase;
    intfDescs[SOIL_LEVEL_INTERFACE] = intfDescOperationSoilLevel;
    intfDescs[SPIN_SPEED_LEVEL_INTERFACE] = intfDescOperationSpinSpeedLevel;
    intfDescs[TIMER_INTERFACE] = intfDescOperationTimer;
    intfDescs[MOISTURE_OUTPUT_LEVEL_INTERFACE] = intfDescOperationMoistureOutputLevel;
    intfDescs[FILTER_STATUS_INTERFACE] = intfDescOperationFilterStatus;
    intfDescs[CURRENT_AIR_QUALITY_INTERFACE] = intfDescEnvironmentCurrentAirQuality;
    intfDescs[CURRENT_AIR_QUALITY_LEVEL_INTERFACE] = intfDescEnvironmentCurrentAirQualityLevel;
    intfDescs[CURRENT_TEMPERATURE_INTERFACE] = intfDescEnvironmentCurrentTemperature;
    intfDescs[TARGET_TEMPERATURE_INTERFACE] = intfDescEnvironmentTargetTemperature;
    intfDescs[WATER_LEVEL_INTERFACE] = intfDescEnvironmentWaterLevel;
    intfDescs[WIND_DIRECTION_INTERFACE] = intfDescEnvironmentWindDirection;
    intfDescs[HID_INTERFACE] = intfDescInputHid;
    intfDescs[CURRENT_HUMIDITY_INTERFACE] = intfDescEnvironmentCurrentHumidity;
    intfDescs[TARGET_HUMIDITY_INTERFACE] = intfDescEnvironmentTargetHumidity;
    intfDescs[TARGET_TEMPERATURE_LEVEL_INTERFACE] = intfDescEnvironmentTargetTemperatureLevel;
    intfDescs[HVAC_FAN_MODE_INTERFACE] = intfDescOperationHvacFanMode;
    intfDescs[PLUG_IN_UNITS_INTERFACE] = intfDescOperationPlugInUnits;
    intfDescs[RAPID_MODE_TIMED_INTERFACE] = intfDescOperationRapidModeTimed;

    intfCreator[AUDIO_VOLUME_INTERFACE] = CreateAudioVolumeInterface;
    intfCreator[AUDIO_VIDEO_INPUT_INTERFACE] = CreateAudioVideoInputInterface;
    intfCreator[CHANNEL_INTERFACE] = CreateChannelInterface;
    intfCreator[CLOSED_STATUS_INTERFACE] = CreateClosedStatusInterface;
    intfCreator[CURRENT_POWER_INTERFACE] = CreateCurrentPowerInterface;
    intfCreator[CYCLE_CONTROL_INTERFACE] = CreateCycleControlInterface;
    intfCreator[DISH_WASHING_CYCLE_PHASE_INTERFACE] = CreateLaundryCyclePhaseInterface;
    intfCreator[LAUNDRY_CYCLE_PHASE_INTERFACE] = CreateDishWashingCyclePhaseInterface;
    intfCreator[ON_CONTROL_INTERFACE] = CreateOnControlInterface;
    intfCreator[OFF_CONTROL_INTERFACE] = CreateOffControlInterface;
    intfCreator[ON_OFF_STATUS_INTERFACE] = CreateOnOffStatusInterface;
    intfCreator[OVEN_CYCLE_PHASE_INTERFACE] = CreateOvenCyclePhaseInterface;
    intfCreator[FAN_SPEED_LEVEL_INTERFACE] = CreateFanSpeedLevelInterface;
    intfCreator[BATTERY_STATUS_INTERFACE] = CreateBatteryStatusInterface;
    intfCreator[CLIMATE_CONTROL_MODE_INTERFACE] = CreateClimateControlModeInterface;
    intfCreator[ENERGY_USAGE_INTERFACE] = CreateEnergyUsageInterface;
    intfCreator[HEATING_ZONE_INTERFACE] = CreateHeatingZoneInterface;
    intfCreator[RAPID_MODE_INTERFACE] = CreateRapidModeInterface;
    intfCreator[REMOTE_CONTROLLABILITY_INTERFACE] = CreateRemoteControllabilityInterface;
    intfCreator[REPEAT_MODE_INTERFACE] = CreateRepeatModeInterface;
    intfCreator[RESOURCE_SAVING_INTERFACE] = CreateResourceSavingInterface;
    intfCreator[AIR_RECIRCULATION_MODE_INTERFACE] = CreateAirRecirculationModeInterface;
    intfCreator[ROBOT_CLEANING_CYCLE_PHASE_INTERFACE] = CreateRobotCleaningCyclePhaseInterface;
    intfCreator[SOIL_LEVEL_INTERFACE] = CreateSoilLevelInterface;
    intfCreator[SPIN_SPEED_LEVEL_INTERFACE] = CreateSpinSpeedLevelInterface;
    intfCreator[TIMER_INTERFACE] = CreateTimerInterface;
    intfCreator[MOISTURE_OUTPUT_LEVEL_INTERFACE] = CreateMoistureOutputLevelInterface;
    intfCreator[FILTER_STATUS_INTERFACE] = CreateFilterStatusInterface;
    intfCreator[CURRENT_AIR_QUALITY_INTERFACE] = CreateCurrentAirQualityInterface;
    intfCreator[CURRENT_AIR_QUALITY_LEVEL_INTERFACE] = CreateCurrentAirQualityLevelInterface;
    intfCreator[CURRENT_TEMPERATURE_INTERFACE] = CreateCurrentTemperatureInterface;
    intfCreator[TARGET_TEMPERATURE_INTERFACE] = CreateTargetTemperatureInterface;
    intfCreator[WATER_LEVEL_INTERFACE] = CreateWaterLevelInterface;
    intfCreator[WIND_DIRECTION_INTERFACE] = CreateWindDirectionInterface;
    intfCreator[HID_INTERFACE] = CreateHidInterface;
    intfCreator[CURRENT_HUMIDITY_INTERFACE] = CreateCurrentHumidityInterface;
    intfCreator[TARGET_HUMIDITY_INTERFACE] = CreateTargetHumidityInterface;
    intfCreator[TARGET_TEMPERATURE_LEVEL_INTERFACE] = CreateTargetTemperatureLevelInterface;
    intfCreator[HVAC_FAN_MODE_INTERFACE] = CreateHvacFanModeInterface;
    intfCreator[PLUG_IN_UNITS_INTERFACE] = CreatePlugInUnitsInterface;
    intfCreator[RAPID_MODE_TIMED_INTERFACE] = CreateRapidModeTimedInterface;

    intfDestructor[AUDIO_VOLUME_INTERFACE] = DestroyAudioVolumeInterface;
    intfDestructor[AUDIO_VIDEO_INPUT_INTERFACE] = DestroyAudioVideoInputInterface;
    intfDestructor[CHANNEL_INTERFACE] = DestroyChannelInterface;
    intfDestructor[CLOSED_STATUS_INTERFACE] = DestroyClosedStatusInterface;
    intfDestructor[CURRENT_POWER_INTERFACE] = DestroyCurrentPowerInterface;
    intfDestructor[CYCLE_CONTROL_INTERFACE] = DestroyCycleControlInterface;
    intfDestructor[DISH_WASHING_CYCLE_PHASE_INTERFACE] = DestroyDishWashingCyclePhaseInterface;
    intfDestructor[LAUNDRY_CYCLE_PHASE_INTERFACE] = DestroyLaundryCyclePhaseInterface;
    intfDestructor[ON_CONTROL_INTERFACE] = DestroyOnControlInterface;
    intfDestructor[OFF_CONTROL_INTERFACE] = DestroyOffControlInterface;
    intfDestructor[ON_OFF_STATUS_INTERFACE] = DestroyOnOffStatusInterface;
    intfDestructor[OVEN_CYCLE_PHASE_INTERFACE] = DestroyOvenCyclePhaseInterface;
    intfDestructor[FAN_SPEED_LEVEL_INTERFACE] = DestroyFanSpeedLevelInterface;
    intfDestructor[BATTERY_STATUS_INTERFACE] = DestroyBatteryStatusInterface;
    intfDestructor[CLIMATE_CONTROL_MODE_INTERFACE] = DestroyClimateControlModeInterface;
    intfDestructor[ENERGY_USAGE_INTERFACE] = DestroyEnergyUsageInterface;
    intfDestructor[HEATING_ZONE_INTERFACE] = DestroyHeatingZoneInterface;
    intfDestructor[RAPID_MODE_INTERFACE] = DestroyRapidModeInterface;
    intfDestructor[REMOTE_CONTROLLABILITY_INTERFACE] = DestroyRemoteControllabilityInterface;
    intfDestructor[REPEAT_MODE_INTERFACE] = DestroyRepeatModeInterface;
    intfDestructor[RESOURCE_SAVING_INTERFACE] = DestroyResourceSavingInterface;
    intfDestructor[AIR_RECIRCULATION_MODE_INTERFACE] = DestroyAirRecirculationModeInterface;
    intfDestructor[ROBOT_CLEANING_CYCLE_PHASE_INTERFACE] = DestroyRobotCleaningCyclePhaseInterface;
    intfDestructor[SOIL_LEVEL_INTERFACE] = DestroySoilLevelInterface;
    intfDestructor[SPIN_SPEED_LEVEL_INTERFACE] = DestroySpinSpeedLevelInterface;
    intfDestructor[TIMER_INTERFACE] = DestroyTimerInterface;
    intfDestructor[MOISTURE_OUTPUT_LEVEL_INTERFACE] = DestroyMoistureOutputLevelInterface;
    intfDestructor[FILTER_STATUS_INTERFACE] = DestroyFilterStatusInterface;
    intfDestructor[CURRENT_AIR_QUALITY_INTERFACE] = DestroyCurrentAirQualityInterface;
    intfDestructor[CURRENT_AIR_QUALITY_LEVEL_INTERFACE] = DestroyCurrentAirQualityLevelInterface;
    intfDestructor[CURRENT_TEMPERATURE_INTERFACE] = DestroyCurrentTemperatureInterface;
    intfDestructor[TARGET_TEMPERATURE_INTERFACE] = DestroyTargetTemperatureInterface;
    intfDestructor[WATER_LEVEL_INTERFACE] = DestroyWaterLevelInterface;
    intfDestructor[WIND_DIRECTION_INTERFACE] = DestroyWindDirectionInterface;
    intfDestructor[HID_INTERFACE] = DestroyHidInterface;
    intfDestructor[CURRENT_HUMIDITY_INTERFACE] = DestroyCurrentHumidityInterface;
    intfDestructor[TARGET_HUMIDITY_INTERFACE] = DestroyTargetHumidityInterface;
    intfDestructor[TARGET_TEMPERATURE_LEVEL_INTERFACE] = DestroyTargetTemperatureLevelInterface;
    intfDestructor[HVAC_FAN_MODE_INTERFACE] = DestroyHvacFanModeInterface;
    intfDestructor[PLUG_IN_UNITS_INTERFACE] = DestroyPlugInUnitsInterface;
    intfDestructor[RAPID_MODE_TIMED_INTERFACE] = DestroyRapidModeTimedInterface;

    onGetProperty[AUDIO_VOLUME_INTERFACE] = AudioVolumeInterfaceOnGetProperty;
    onGetProperty[AUDIO_VIDEO_INPUT_INTERFACE] = AudioVideoInputInterfaceOnGetProperty;
    onGetProperty[CHANNEL_INTERFACE] = ChannelInterfaceOnGetProperty;
    onGetProperty[CLOSED_STATUS_INTERFACE] = ClosedStatusInterfaceOnGetProperty;
    onGetProperty[CURRENT_POWER_INTERFACE] = CurrentPowerInterfaceOnGetProperty;
    onGetProperty[CYCLE_CONTROL_INTERFACE] = CycleControlInterfaceOnGetProperty;
    onGetProperty[DISH_WASHING_CYCLE_PHASE_INTERFACE] = DishWashingCyclePhaseInterfaceOnGetProperty;
    onGetProperty[LAUNDRY_CYCLE_PHASE_INTERFACE] = LaundryCyclePhaseInterfaceOnGetProperty;
    onGetProperty[ON_OFF_STATUS_INTERFACE] = OnOffStatusInterfaceOnGetProperty;
    onGetProperty[OVEN_CYCLE_PHASE_INTERFACE] = OvenCyclePhaseInterfaceOnGetProperty;
    onGetProperty[FAN_SPEED_LEVEL_INTERFACE] = FanSpeedLevelInterfaceOnGetProperty;
    onGetProperty[BATTERY_STATUS_INTERFACE] = BatteryStatusInterfaceOnGetProperty;
    onGetProperty[CLIMATE_CONTROL_MODE_INTERFACE] = ClimateControlModeInterfaceOnGetProperty;
    onGetProperty[ENERGY_USAGE_INTERFACE] = EnergyUsageInterfaceOnGetProperty;
    onGetProperty[HEATING_ZONE_INTERFACE] = HeatingZoneInterfaceOnGetProperty;
    onGetProperty[RAPID_MODE_INTERFACE] = RapidModeInterfaceOnGetProperty;
    onGetProperty[REMOTE_CONTROLLABILITY_INTERFACE] = RemoteControllabilityInterfaceOnGetProperty;
    onGetProperty[REPEAT_MODE_INTERFACE] = RepeatModeInterfaceOnGetProperty;
    onGetProperty[RESOURCE_SAVING_INTERFACE] = ResourceSavingInterfaceOnGetProperty;
    onGetProperty[AIR_RECIRCULATION_MODE_INTERFACE] = AirRecirculationModeInterfaceOnGetProperty;
    onGetProperty[ROBOT_CLEANING_CYCLE_PHASE_INTERFACE] = RobotCleaningCyclePhaseInterfaceOnGetProperty;
    onGetProperty[SOIL_LEVEL_INTERFACE] = SoilLevelInterfaceOnGetProperty;
    onGetProperty[SPIN_SPEED_LEVEL_INTERFACE] = SpinSpeedLevelInterfaceOnGetProperty;
    onGetProperty[TIMER_INTERFACE] = TimerInterfaceOnGetProperty;
    onGetProperty[MOISTURE_OUTPUT_LEVEL_INTERFACE] = MoistureOutputLevelInterfaceOnGetProperty;
    onGetProperty[FILTER_STATUS_INTERFACE] = FilterStatusInterfaceOnGetProperty;
    onGetProperty[CURRENT_AIR_QUALITY_INTERFACE] = CurrentAirQualityInterfaceOnGetProperty;
    onGetProperty[CURRENT_AIR_QUALITY_LEVEL_INTERFACE] = CurrentAirQualityLevelInterfaceOnGetProperty;
    onGetProperty[CURRENT_TEMPERATURE_INTERFACE] = CurrentTemperatureInterfaceOnGetProperty;
    onGetProperty[TARGET_TEMPERATURE_INTERFACE] = TargetTemperatureInterfaceOnGetProperty;
    onGetProperty[WATER_LEVEL_INTERFACE] = WaterLevelInterfaceOnGetProperty;
    onGetProperty[WIND_DIRECTION_INTERFACE] = WindDirectionInterfaceOnGetProperty;
    onGetProperty[HID_INTERFACE] = HidInterfaceOnGetProperty;
    onGetProperty[CURRENT_HUMIDITY_INTERFACE] = CurrentHumidityInterfaceOnGetProperty;
    onGetProperty[TARGET_HUMIDITY_INTERFACE] = TargetHumidityInterfaceOnGetProperty;
    onGetProperty[TARGET_TEMPERATURE_LEVEL_INTERFACE] = TargetTemperatureLevelInterfaceOnGetProperty;
    onGetProperty[HVAC_FAN_MODE_INTERFACE] = HvacFanModeInterfaceOnGetProperty;
    onGetProperty[PLUG_IN_UNITS_INTERFACE] = PlugInUnitsInterfaceOnGetProperty;
    onGetProperty[RAPID_MODE_TIMED_INTERFACE] = RapidModeTimedInterfaceOnGetProperty;

    onSetProperty[AUDIO_VOLUME_INTERFACE] = AudioVolumeInterfaceOnSetProperty;
    onSetProperty[AUDIO_VIDEO_INPUT_INTERFACE] = AudioVideoInputInterfaceOnSetProperty;
    onSetProperty[CHANNEL_INTERFACE] = ChannelInterfaceOnSetProperty;
    onSetProperty[FAN_SPEED_LEVEL_INTERFACE] = FanSpeedLevelInterfaceOnSetProperty;
    onSetProperty[CLIMATE_CONTROL_MODE_INTERFACE] = ClimateControlModeInterfaceOnSetProperty;
    onSetProperty[RAPID_MODE_INTERFACE] = RapidModeInterfaceOnSetProperty;
    onSetProperty[REPEAT_MODE_INTERFACE] = RepeatModeInterfaceOnSetProperty;
    onSetProperty[RESOURCE_SAVING_INTERFACE] = ResourceSavingInterfaceOnSetProperty;
    onSetProperty[AIR_RECIRCULATION_MODE_INTERFACE] = AirRecirculationModeInterfaceOnSetProperty;
    onSetProperty[SOIL_LEVEL_INTERFACE] = SoilLevelInterfaceOnSetProperty;
    onSetProperty[SPIN_SPEED_LEVEL_INTERFACE] = SpinSpeedLevelInterfaceOnSetProperty;
    onSetProperty[MOISTURE_OUTPUT_LEVEL_INTERFACE] = MoistureOutputLevelInterfaceOnSetProperty;
    onSetProperty[TARGET_TEMPERATURE_INTERFACE] = TargetTemperatureInterfaceOnSetProperty;
    onSetProperty[WIND_DIRECTION_INTERFACE] = WindDirectionInterfaceOnSetProperty;
    onSetProperty[TARGET_HUMIDITY_INTERFACE] = TargetHumidityInterfaceOnSetProperty;
    onSetProperty[TARGET_TEMPERATURE_LEVEL_INTERFACE] = TargetTemperatureLevelInterfaceOnSetProperty;
    onSetProperty[HVAC_FAN_MODE_INTERFACE] = HvacFanModeInterfaceOnSetProperty;
    onSetProperty[RAPID_MODE_TIMED_INTERFACE] = RapidModeTimedInterfaceOnSetProperty;

    emitPropertiesChanged[AUDIO_VOLUME_INTERFACE] = AudioVolumeInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[AUDIO_VIDEO_INPUT_INTERFACE] = AudioVideoInputInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[CHANNEL_INTERFACE] = ChannelInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[FAN_SPEED_LEVEL_INTERFACE] = FanSpeedLevelInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[CLIMATE_CONTROL_MODE_INTERFACE] = ClimateControlModeInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[ENERGY_USAGE_INTERFACE] = EnergyUsageInterfaceEmitPropertiesChanged; //There is no writable property, but there is a property which can be changed by method call
    emitPropertiesChanged[RAPID_MODE_INTERFACE] = RapidModeInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[REPEAT_MODE_INTERFACE] = RepeatModeInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[RESOURCE_SAVING_INTERFACE] = ResourceSavingInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[AIR_RECIRCULATION_MODE_INTERFACE] = AirRecirculationModeInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[SOIL_LEVEL_INTERFACE] = SoilLevelInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[SPIN_SPEED_LEVEL_INTERFACE] = SpinSpeedLevelInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[MOISTURE_OUTPUT_LEVEL_INTERFACE] = MoistureOutputLevelInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[TARGET_TEMPERATURE_INTERFACE] = TargetTemperatureInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[WIND_DIRECTION_INTERFACE] = WindDirectionInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[TARGET_HUMIDITY_INTERFACE] = TargetHumidityInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[TARGET_TEMPERATURE_LEVEL_INTERFACE] = TargetTemperatureLevelInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[HVAC_FAN_MODE_INTERFACE] = HvacFanModeInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[RAPID_MODE_TIMED_INTERFACE] = RapidModeTimedInterfaceEmitPropertiesChanged;

    onMethodHandler[CHANNEL_INTERFACE] = ChannelInterfaceOnMethodHandler;
    onMethodHandler[CYCLE_CONTROL_INTERFACE] = CycleControlInterfaceOnMethodHandler;
    onMethodHandler[DISH_WASHING_CYCLE_PHASE_INTERFACE] = DishWashingCyclePhaseInterfaceOnMethodHandler;
    onMethodHandler[LAUNDRY_CYCLE_PHASE_INTERFACE] = LaundryCyclePhaseInterfaceOnMethodHandler;
    onMethodHandler[ON_CONTROL_INTERFACE] = OnControlInterfaceOnMethodHandler;
    onMethodHandler[OFF_CONTROL_INTERFACE] = OffControlInterfaceOnMethodHandler;
    onMethodHandler[OVEN_CYCLE_PHASE_INTERFACE] = OvenCyclePhaseInterfaceOnMethodHandler;
    onMethodHandler[ENERGY_USAGE_INTERFACE] = EnergyUsageInterfaceOnMethodHandler;
    onMethodHandler[ROBOT_CLEANING_CYCLE_PHASE_INTERFACE] = RobotCleaningCyclePhaseInterfaceOnMethodHandler;
    onMethodHandler[TIMER_INTERFACE] = TimerInterfaceOnMethodHandler;
    onMethodHandler[HID_INTERFACE] = HidInterfaceOnMethodHandler;

    objInfoFirst = NULL;
    objInfoLast = NULL;

    vendorDefinedIntfInfoFirst = NULL;
    vendorDefinedIntfInfoLast = NULL;

    return AJ_OK;
}

void Hae_Deinit()
{
    HaeObjectInfo* objInfo = objInfoFirst;
    HaeObjectInfo* tempObjInfo = NULL;
    HaeInterfaceInfo* intfInfo = NULL;
    HaeInterfaceInfo* tempIntfInfo = NULL;
    VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = NULL;
    VendorDefinedInterfaceInfo* tempVendorDefinedIntfInfo = NULL;

    while (objInfo) {
        intfInfo = objInfo->intfFirst;

        while (intfInfo) {
            if (intfInfo->intfType > UNDEFINED_INTERFACE && intfInfo->intfType < VENDOR_DEFINED_INTERFACE) {
                if (intfDestructor[intfInfo->intfType]) {
                    intfDestructor[intfInfo->intfType](intfInfo->properties);
                }
            } else if (intfInfo->intfType > VENDOR_DEFINED_INTERFACE) {
                vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfInfo->intfType);
                if (vendorDefinedIntfInfo) {
                    if (vendorDefinedIntfInfo->handler) {
                        if (vendorDefinedIntfInfo->handler->InterfaceDestructor) {
                            vendorDefinedIntfInfo->handler->InterfaceDestructor(intfInfo->properties);
                        }
                    }
                }
            }

            tempIntfInfo = intfInfo;
            intfInfo = intfInfo->intfNext;
            free(tempIntfInfo);
        }

        if (objInfo->ajIntfDesc) {
            free(objInfo->ajIntfDesc);
        }
        tempObjInfo = objInfo;
        objInfo = objInfo->objNext;
        free(tempObjInfo);
    }

    vendorDefinedIntfInfo = vendorDefinedIntfInfoFirst;
    while (vendorDefinedIntfInfo) {
        tempVendorDefinedIntfInfo = vendorDefinedIntfInfo;
        vendorDefinedIntfInfo = vendorDefinedIntfInfo->intfNext;
        free(tempVendorDefinedIntfInfo);
    }

    if (ajObjects) {
        free(ajObjects);
    }
}

static HaeObjectInfo* FindObject(const char* objPath)
{
    HaeObjectInfo* objInfo = objInfoFirst;

    if (!objPath) {
        return NULL;
    }

    while (objInfo) {
        if (!strcmp(objPath, objInfo->path)) {
            break;
        } else {
            objInfo = objInfo->objNext;
        }
    }

    return objInfo;
}

AJ_Status Hae_CreateInterface(HaeInterfaceTypes intfType, const char* objPath, void* listener)
{
    AJ_Status status = AJ_OK;
    HaeObjectInfo* objInfo = NULL;
    VendorDefinedInterfaceHandler* vendorDefinedIntfHandler = NULL;

    if (!objPath) {
        return AJ_ERR_INVALID;
    }

    if (intfType > UNDEFINED_INTERFACE && intfType < VENDOR_DEFINED_INTERFACE) {
        if (!intfCreator[intfType]) {
            return AJ_ERR_NULL;
        }
    } else if (intfType > VENDOR_DEFINED_INTERFACE) {
        VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfType);
        if (!vendorDefinedIntfInfo) {
            return AJ_ERR_NULL;
        }

        vendorDefinedIntfHandler = vendorDefinedIntfInfo->handler;
        if (!vendorDefinedIntfHandler) {
            return AJ_ERR_NULL;
        }
    } else {
        return AJ_ERR_INVALID;
    }

    HaeInterfaceInfo* intfInfo = (HaeInterfaceInfo*)malloc(sizeof(HaeInterfaceInfo));
    if (!intfInfo) {
        return AJ_ERR_RESOURCES;
    }
    intfInfo->intfType = intfType;
    intfInfo->listener = listener;

    if (!vendorDefinedIntfHandler) {
        status = intfCreator[intfType](&intfInfo->properties);
    } else {
        status = vendorDefinedIntfHandler->InterfaceCreator(&intfInfo->properties);
    }
    if (status != AJ_OK) {
        return status;
    }

    objInfo = FindObject(objPath);
    if (!objInfo) {
        objInfo = (HaeObjectInfo*)malloc(sizeof(HaeObjectInfo));
        if (!objInfo) {
            free(intfInfo);
            return AJ_ERR_RESOURCES;
        }

        {
            HaeObjectInfo tempObj = { objPath, intfInfo, intfInfo, NULL, NULL };
            memcpy(objInfo, &tempObj, sizeof(tempObj));
        }
        if (!objInfoFirst) {
            objInfoFirst = objInfo;
        }
        if (objInfoLast) {
            objInfoLast->objNext = objInfo;
        }
        objInfoLast = objInfo;
    } else {
        objInfo->intfLast->intfNext = intfInfo;
        objInfo->intfLast = intfInfo;
    }
    return AJ_OK;
}

static void CleanInterfaceDescs()
{
    HaeObjectInfo* objInfo = objInfoFirst;

    while (objInfo) {
        if (objInfo->ajIntfDesc) {
            free(objInfo->ajIntfDesc);
        }

        objInfo = objInfo->objNext;
    }
}

AJ_Status Hae_Start()
{
    int i=0, j=1;
    HaeObjectInfo* objInfo = NULL;
    int numOfObjs = 1;
    HaeInterfaceInfo* intfInfo = NULL;
    int numOfIntfs = 0;

    if (!objInfoFirst || !objInfoLast) {
        return AJ_ERR_UNEXPECTED;
    }

    objInfo = objInfoFirst;
    while (objInfo != objInfoLast) {
        objInfo = objInfo->objNext;
        numOfObjs++;
    }

    ajObjects = (AJ_Object*)malloc(sizeof(AJ_Object) * (numOfObjs+1)); //+1 is for last NULL
    if (!ajObjects) {
        return AJ_ERR_RESOURCES;
    }

    objInfo = objInfoFirst;
    while (objInfo) {
        intfInfo = objInfo->intfFirst;
        numOfIntfs = 1;
        while (intfInfo != objInfo->intfLast) {
            intfInfo = intfInfo->intfNext;
            numOfIntfs++;
        }

        objInfo->ajIntfDesc = (AJ_InterfaceDescription*)malloc(sizeof(AJ_InterfaceDescription) * (numOfIntfs+2)); //+2 are for AJ_PropertiesIface and last NULL
        if (!objInfo->ajIntfDesc) {
            CleanInterfaceDescs();
            free(ajObjects);
            return AJ_ERR_RESOURCES;
        }

        objInfo->ajIntfDesc[0] = AJ_PropertiesIface;
        intfInfo = objInfo->intfFirst;
        j=1;
        while (intfInfo) {
            if (intfInfo->intfType > UNDEFINED_INTERFACE && intfInfo->intfType < VENDOR_DEFINED_INTERFACE) {
                objInfo->ajIntfDesc[j] = intfDescs[intfInfo->intfType];
            } else if (intfInfo->intfType > VENDOR_DEFINED_INTERFACE) {
                VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfInfo->intfType);
                if (vendorDefinedIntfInfo) {
                    objInfo->ajIntfDesc[j] = vendorDefinedIntfInfo->intfDesc;
                }
            }
            intfInfo = intfInfo->intfNext;
            j++;
        }
        objInfo->ajIntfDesc[j] = NULL;

        {
            AJ_Object temp_obj = { objInfo->path, objInfo->ajIntfDesc, AJ_OBJ_FLAG_ANNOUNCED };
            memcpy(&ajObjects[i], &temp_obj, sizeof(temp_obj));
        }
        objInfo = objInfo->objNext;
        i++;
    }

    AJ_PrintXML(ajObjects);

    AJ_RegisterObjectList(ajObjects, HAE_OBJECT_LIST_INDEX);

    return AJ_OK;
}

AJ_Status Hae_EnableSecurity(AJ_BusAttachment* busAttachment, const uint32_t* suites, const size_t numOfSuites,
                             AJ_AuthListenerFunc authListenerCallback)
{
    AJ_Status status = AJ_OK;

    status = AJ_BusEnableSecurity(busAttachment, suites, numOfSuites);
    if (status == AJ_OK) {
        AJ_BusSetAuthListenerCallback(busAttachment, authListenerCallback);
    }

    return status;
}

static bool IsHaeMsg(uint32_t msgId)
{
    if ((msgId >> 24) == HAE_OBJECT_LIST_INDEX) {
        return true;
    }

    return false;
}

static uint8_t GetObjectIndex(uint32_t msgId)
{
    return (uint8_t)((msgId & 0x00FF0000) >> 16);
}

static uint8_t GetInterfaceIndex(uint32_t msgId)
{
    return (uint8_t)((msgId & 0x0000FF00) >> 8);
}

static uint8_t GetMemberIndex(uint32_t msgId)
{
    return (uint8_t)(msgId & 0x0000FF);
}

static HaeObjectInfo* GetObjectInfo(uint8_t objIndex)
{
    HaeObjectInfo* objInfo = objInfoFirst;
    uint8_t i = 0;

    for (i=0; i<objIndex; i++) {
        if (!objInfo) {
            break;
        }

        objInfo = objInfo->objNext;
    }

    return objInfo;
}

static HaeInterfaceInfo* GetInterfaceInfoOfObject(HaeObjectInfo* objInfo, uint8_t intfIndex)
{
    uint8_t i=1;

    if (!objInfo) {
        return NULL;
    }

    if (intfIndex < 1) { //0 is org.freedesktop.DBus.Properties
        return NULL;
    }

    HaeInterfaceInfo* intfInfo = objInfo->intfFirst;

    for (i=1; i<intfIndex; i++) {
        intfInfo = intfInfo->intfNext;
        if (!intfInfo) {
            break;
        }
    }

    return intfInfo;
}

static void EmitPropChangedByMethod(HaeInterfaceTypes intfType, AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint32_t mask)
{
    int i = 1;
    uint8_t check = 0;

    for (i = 1; i < 32; i++) {
        mask >>= 1;
        if (!mask) {
            break;
        }
        check = mask & (uint32_t)1;
        if (check) {
            if (emitPropertiesChanged[intfType]) {
                emitPropertiesChanged[intfType](busAttachment, objPath, properties, i);
            }
        }
    }
}

static AJ_Status PropGetHandler(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    uint8_t objIndex = GetObjectIndex(propId);
    uint8_t intfIndex = GetInterfaceIndex(propId);
    uint8_t memberIndex = GetMemberIndex(propId);
    AJ_Status status = AJ_OK;

    HaeObjectInfo* objInfo = GetObjectInfo(objIndex);
    if (objInfo) {
        HaeInterfaceInfo* intfInfo = GetInterfaceInfoOfObject(objInfo, intfIndex);
        if (intfInfo) {
            if (intfInfo->intfType > UNDEFINED_INTERFACE && intfInfo->intfType < VENDOR_DEFINED_INTERFACE) {
                if (onGetProperty[intfInfo->intfType]) {
                    status = onGetProperty[intfInfo->intfType](replyMsg, objInfo->path, intfInfo->properties, memberIndex, intfInfo->listener);
                }
            } else if (intfInfo->intfType > VENDOR_DEFINED_INTERFACE) {
                VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfInfo->intfType);
                if (!vendorDefinedIntfInfo) {
                    status = AJ_ERR_NULL;
                }

                if (vendorDefinedIntfInfo->handler) {
                    if (vendorDefinedIntfInfo->handler->OnGetProperty) {
                        status = vendorDefinedIntfInfo->handler->OnGetProperty(replyMsg, objInfo->path, intfInfo->properties, memberIndex, intfInfo->listener);
                    } else {
                        status = AJ_ERR_NULL;
                    }
                } else {
                    status = AJ_ERR_NULL;
                }
            } else {
                status = AJ_ERR_INVALID;
            }
        } else {
            status = AJ_ERR_NULL;
        }
    } else {
        status = AJ_ERR_NULL;
    }

    return status;
}

static AJ_Status PropSetHandler(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    uint8_t objIndex = GetObjectIndex(propId);
    uint8_t intfIndex = GetInterfaceIndex(propId);
    uint8_t memberIndex = GetMemberIndex(propId);
    HaePropertiesChanged* propChanged = (HaePropertiesChanged*)context;
    AJ_Status status = AJ_OK;

    HaeObjectInfo* objInfo = GetObjectInfo(objIndex);
    if (objInfo) {
        HaeInterfaceInfo* intfInfo = GetInterfaceInfoOfObject(objInfo, intfIndex);
        if (intfInfo) {
            propChanged->intfType = intfInfo->intfType;
            propChanged->properties = intfInfo->properties;
            propChanged->member_index = memberIndex;
            if (intfInfo->intfType > UNDEFINED_INTERFACE && intfInfo->intfType < VENDOR_DEFINED_INTERFACE) {
                if (onSetProperty[intfInfo->intfType]) {
                    status = onSetProperty[intfInfo->intfType](replyMsg, objInfo->path, intfInfo->properties, memberIndex, intfInfo->listener, &propChanged->changed);
                }
            } else if (intfInfo->intfType > VENDOR_DEFINED_INTERFACE) {
                VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfInfo->intfType);
                if (!vendorDefinedIntfInfo) {
                    status = AJ_ERR_NULL;
                }

                if (vendorDefinedIntfInfo->handler) {
                    if (vendorDefinedIntfInfo->handler->OnSetProperty) {
                        status = vendorDefinedIntfInfo->handler->OnSetProperty(replyMsg, objInfo->path, intfInfo->properties, memberIndex, intfInfo->listener, &propChanged->changed);
                    } else {
                        status = AJ_ERR_NULL;
                    }
                } else {
                    status = AJ_ERR_NULL;
                }
            } else {
                status = AJ_ERR_INVALID;
            }
        } else {
            status = AJ_ERR_NULL;
        }
    } else {
        status = AJ_ERR_NULL;
    }

    return status;
}

AJSVC_ServiceStatus Hae_MessageProcessor(AJ_BusAttachment* busAttachment, AJ_Message* msg, AJ_Status* status)
{
    AJSVC_ServiceStatus serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;

    if (IsHaeMsg(msg->msgId)) {
        uint8_t objIndex = GetObjectIndex(msg->msgId);
        uint8_t intfIndex = GetInterfaceIndex(msg->msgId);
        uint8_t memberIndex = GetMemberIndex(msg->msgId);
        HaeObjectInfo* objInfo = GetObjectInfo(objIndex);

        if (objInfo) {
            if (intfIndex == (uint8_t)0) { //org.freedesktop.DBus.Properties handling
                if (memberIndex == AJ_PROP_GET) {
                    *status = AJ_BusPropGet(msg, PropGetHandler, NULL);
                } else if (memberIndex == AJ_PROP_SET) {
                    HaePropertiesChanged propChanged;
                    *status = AJ_BusPropSet(msg, PropSetHandler, &propChanged);
                    if (*status == AJ_OK && propChanged.changed) {
                        if (propChanged.intfType > UNDEFINED_INTERFACE && propChanged.intfType < VENDOR_DEFINED_INTERFACE) {
                            if (emitPropertiesChanged[propChanged.intfType]) {
                                emitPropertiesChanged[propChanged.intfType](busAttachment, objInfo->path, propChanged.properties, propChanged.member_index);
                            }
                        } else if (propChanged.intfType > VENDOR_DEFINED_INTERFACE) {
                            VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(propChanged.intfType);
                            if (vendorDefinedIntfInfo) {
                                if (vendorDefinedIntfInfo->handler) {
                                    if (vendorDefinedIntfInfo->handler->EmitPropertiesChanged) {
                                        vendorDefinedIntfInfo->handler->EmitPropertiesChanged(busAttachment, objInfo->path, propChanged.properties, propChanged.member_index);
                                    }
                                }
                            }
                        }
                    }
                } else {
                    *status = AJ_ERR_INVALID;
                }
            } else { //method
                HaeInterfaceInfo* intfInfo = GetInterfaceInfoOfObject(objInfo, intfIndex);
                if (intfInfo) {
                    HaePropertiesChangedByMethod propChangedByMethod;
                    propChangedByMethod.properties = intfInfo->properties;
                    propChangedByMethod.member_index_mask = 0;
                    if (intfInfo->intfType > UNDEFINED_INTERFACE && intfInfo->intfType < VENDOR_DEFINED_INTERFACE) {
                        if (onMethodHandler[intfInfo->intfType]) {
                            *status = onMethodHandler[intfInfo->intfType](msg, objInfo->path, memberIndex, intfInfo->listener, &propChangedByMethod);
                            if (*status == AJ_OK && propChangedByMethod.member_index_mask != 0) {
                                EmitPropChangedByMethod(intfInfo->intfType, busAttachment, objInfo->path, propChangedByMethod.properties, propChangedByMethod.member_index_mask);
                            }
                        } else {
                            *status = AJ_ERR_INVALID;
                        }
                    } else if (intfInfo->intfType > VENDOR_DEFINED_INTERFACE) {
                        VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfInfo->intfType);
                        if (!vendorDefinedIntfInfo) {
                            *status = AJ_ERR_NULL;
                        }

                        if (vendorDefinedIntfInfo->handler) {
                            if (vendorDefinedIntfInfo->handler->OnMethodHandler) {
                                *status = vendorDefinedIntfInfo->handler->OnMethodHandler(msg, objInfo->path, memberIndex, intfInfo->listener, &propChangedByMethod);
                                if (*status == AJ_OK && propChangedByMethod.member_index_mask != 0) {
                                    EmitPropChangedByMethod(intfInfo->intfType, busAttachment, objInfo->path, propChangedByMethod.properties, propChangedByMethod.member_index_mask);
                                }
                            } else {
                                *status = AJ_ERR_NULL;
                            }
                        } else {
                            *status = AJ_ERR_NULL;
                        }
                    } else {
                        *status = AJ_ERR_INVALID;
                    }
                } else {
                    *status = AJ_ERR_NULL;
                }
            }
        } else {
            *status = AJ_ERR_NULL;
        }
    } else {
        serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;
    }

    return serviceStatus;
}

void* GetProperties(const char* objPath, HaeInterfaceTypes intfType)
{
    HaeObjectInfo* objInfo = NULL;

    if (!objPath) {
        return NULL;
    }

    objInfo = FindObject(objPath);
    if (objInfo) {
        HaeInterfaceInfo* intfInfo = objInfo->intfFirst;

        while (intfInfo) {
            if (intfInfo->intfType == intfType) {
                break;
            }

            intfInfo = intfInfo->intfNext;
        }

        if (intfInfo) {
            return intfInfo->properties;
        }
    }

    return NULL;
}

AJ_Status MakeMsgId(const char* objPath, HaeInterfaceTypes intfType, uint8_t memberIndex, uint32_t* msgId)
{
    HaeObjectInfo* objInfo = objInfoFirst;
    HaeInterfaceInfo* intfInfo = NULL;
    uint8_t objIndex = -1;
    uint8_t intfIndex = 0;

    if (!objPath) {
        return AJ_ERR_INVALID;
    }

    if (!msgId) {
        return AJ_ERR_INVALID;
    }

    *msgId = HAE_OBJECT_LIST_INDEX << 24;

    while (objInfo) {
        objIndex++;

        if (!strcmp(objInfo->path, objPath)) {
            break;
        } else {
            objInfo = objInfo->objNext;
        }
    }

    if (!objInfo) {
        return AJ_ERR_NO_MATCH;
    }

    *msgId |= (objIndex << 16);

    intfInfo = objInfo->intfFirst;
    while (intfInfo) {
        intfIndex++;
        if (intfInfo->intfType == intfType) {
            break;
        } else {
            intfInfo = intfInfo->intfNext;
        }
    }

    if (!intfInfo) {
        return AJ_ERR_NO_MATCH;
    }

    *msgId |= (intfIndex << 8);
    *msgId |= memberIndex;

    return AJ_OK;
}

AJ_Status MakePropChangedId(const char* objPath, uint32_t* msgId)
{
    HaeObjectInfo* objInfo = objInfoFirst;
    uint8_t objIndex = -1;

    if (!objPath) {
        return AJ_ERR_INVALID;
    }

    if (!msgId) {
        return AJ_ERR_INVALID;
    }

    *msgId = HAE_OBJECT_LIST_INDEX << 24;

    while (objInfo) {
        objIndex++;

        if (!strcmp(objInfo->path, objPath)) {
            break;
        } else {
            objInfo = objInfo->objNext;
        }
    }

    if (!objInfo) {
        return AJ_ERR_NO_MATCH;
    }

    *msgId |= (objIndex << 16);
    *msgId |= AJ_PROP_CHANGED;

    return AJ_OK;
}

static bool CheckAlreadyRegistered(const char* intfName)
{
    VendorDefinedInterfaceInfo* intfInfo = NULL;

    intfInfo = vendorDefinedIntfInfoFirst;
    while (intfInfo) {
        if (!strcmp(intfName, intfInfo->intfName)) {
            break;
        }
        intfInfo = intfInfo->intfNext;
    }

    if (intfInfo) {
        return true;
    } else {
        return false;
    }
}

static int GetNumOfVendorInterfaces()
{
    VendorDefinedInterfaceInfo* intfInfo = NULL;
    int numOfIntfs = 1;

    intfInfo = vendorDefinedIntfInfoFirst;
    while (intfInfo != vendorDefinedIntfInfoLast) {
        intfInfo = intfInfo->intfNext;
        numOfIntfs++;
    }

    return numOfIntfs;
}

AJ_Status Hae_RegisterVendorDefinedInterface(const char* intfName, const char* const* intfDesc, VendorDefinedInterfaceHandler* handler, HaeInterfaceTypes* intfType)
{
    if (!intfName || !intfDesc) {
        return AJ_ERR_INVALID;
    }

    if (!handler || !handler->InterfaceRegistered || !handler->InterfaceCreator || !handler->InterfaceDestructor) {
        return AJ_ERR_INVALID;
    }

    if (!intfType) {
        return AJ_ERR_INVALID;
    }

    if (CheckAlreadyRegistered(intfName)) {
        return AJ_ERR_DISALLOWED;
    }

    VendorDefinedInterfaceInfo* intfInfo = (VendorDefinedInterfaceInfo*)malloc(sizeof(VendorDefinedInterfaceInfo));
    if (!intfInfo) {
        return AJ_ERR_RESOURCES;
    }

    {
        VendorDefinedInterfaceInfo tempInfo = { intfName, UNDEFINED_INTERFACE, intfDesc, handler, NULL };
        memcpy(intfInfo, &tempInfo, sizeof(tempInfo));
    }
    if (!vendorDefinedIntfInfoFirst) {
        vendorDefinedIntfInfoFirst = intfInfo;
    }
    if (vendorDefinedIntfInfoLast) {
        vendorDefinedIntfInfoLast->intfNext = intfInfo;
    }
    vendorDefinedIntfInfoLast = intfInfo;
    intfInfo->intfType = VENDOR_DEFINED_INTERFACE + GetNumOfVendorInterfaces();
    handler->InterfaceRegistered(intfInfo->intfType);
    *intfType = intfInfo->intfType;

    return AJ_OK;
}
