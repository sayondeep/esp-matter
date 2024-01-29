/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// THIS FILE IS GENERATED BY ZAP

#pragma once
void MatterAccessControlPluginServerInitCallback();
void MatterAccountLoginPluginServerInitCallback();
void MatterActionsPluginServerInitCallback();
void MatterActivatedCarbonFilterMonitoringPluginServerInitCallback();
void MatterAdministratorCommissioningPluginServerInitCallback();
void MatterAirQualityPluginServerInitCallback();
void MatterApplicationBasicPluginServerInitCallback();
void MatterApplicationLauncherPluginServerInitCallback();
void MatterAudioOutputPluginServerInitCallback();
void MatterBallastConfigurationPluginServerInitCallback();
void MatterBarrierControlPluginServerInitCallback();
void MatterBasicInformationPluginServerInitCallback();
void MatterBinaryInputBasicPluginServerInitCallback();
void MatterBindingPluginServerInitCallback();
void MatterBooleanStatePluginServerInitCallback();
void MatterCarbonDioxideConcentrationMeasurementPluginServerInitCallback();
void MatterCarbonMonoxideConcentrationMeasurementPluginServerInitCallback();
void MatterChannelPluginServerInitCallback();
void MatterColorControlPluginServerInitCallback();
void MatterContentLauncherPluginServerInitCallback();
void MatterDescriptorPluginServerInitCallback();
void MatterDiagnosticLogsPluginServerInitCallback();
void MatterDishwasherAlarmPluginServerInitCallback();
void MatterDishwasherModePluginServerInitCallback();
void MatterDoorLockPluginServerInitCallback();
void MatterElectricalEnergyMeasurementPluginServerInitCallback();
void MatterElectricalMeasurementPluginServerInitCallback();
void MatterEnergyEvsePluginServerInitCallback();
void MatterEthernetNetworkDiagnosticsPluginServerInitCallback();
void MatterFanControlPluginServerInitCallback();
void MatterFaultInjectionPluginServerInitCallback();
void MatterFixedLabelPluginServerInitCallback();
void MatterFlowMeasurementPluginServerInitCallback();
void MatterFormaldehydeConcentrationMeasurementPluginServerInitCallback();
void MatterGeneralCommissioningPluginServerInitCallback();
void MatterGeneralDiagnosticsPluginServerInitCallback();
void MatterGroupKeyManagementPluginServerInitCallback();
void MatterGroupsPluginServerInitCallback();
void MatterHepaFilterMonitoringPluginServerInitCallback();
void MatterIcdManagementPluginServerInitCallback();
void MatterIdentifyPluginServerInitCallback();
void MatterIlluminanceMeasurementPluginServerInitCallback();
void MatterKeypadInputPluginServerInitCallback();
void MatterLaundryDryerControlsPluginServerInitCallback();
void MatterLaundryWasherControlsPluginServerInitCallback();
void MatterLaundryWasherModePluginServerInitCallback();
void MatterLevelControlPluginServerInitCallback();
void MatterLocalizationConfigurationPluginServerInitCallback();
void MatterLowPowerPluginServerInitCallback();
void MatterMediaInputPluginServerInitCallback();
void MatterMediaPlaybackPluginServerInitCallback();
void MatterMicrowaveOvenModePluginServerInitCallback();
void MatterModeSelectPluginServerInitCallback();
void MatterNetworkCommissioningPluginServerInitCallback();
void MatterNitrogenDioxideConcentrationMeasurementPluginServerInitCallback();
void MatterOccupancySensingPluginServerInitCallback();
void MatterOnOffPluginServerInitCallback();
void MatterOnOffSwitchConfigurationPluginServerInitCallback();
void MatterOperationalCredentialsPluginServerInitCallback();
void MatterOperationalStatePluginServerInitCallback();
void MatterOtaSoftwareUpdateProviderPluginServerInitCallback();
void MatterOtaSoftwareUpdateRequestorPluginServerInitCallback();
void MatterOzoneConcentrationMeasurementPluginServerInitCallback();
void MatterPm10ConcentrationMeasurementPluginServerInitCallback();
void MatterPm1ConcentrationMeasurementPluginServerInitCallback();
void MatterPm25ConcentrationMeasurementPluginServerInitCallback();
void MatterPowerSourcePluginServerInitCallback();
void MatterPowerSourceConfigurationPluginServerInitCallback();
void MatterPressureMeasurementPluginServerInitCallback();
void MatterPumpConfigurationAndControlPluginServerInitCallback();
void MatterRadonConcentrationMeasurementPluginServerInitCallback();
void MatterRefrigeratorAlarmPluginServerInitCallback();
void MatterRefrigeratorAndTemperatureControlledCabinetModePluginServerInitCallback();
void MatterRelativeHumidityMeasurementPluginServerInitCallback();
void MatterRvcCleanModePluginServerInitCallback();
void MatterRvcOperationalStatePluginServerInitCallback();
void MatterRvcRunModePluginServerInitCallback();
void MatterScenesManagementPluginServerInitCallback();
void MatterSmokeCoAlarmPluginServerInitCallback();
void MatterSoftwareDiagnosticsPluginServerInitCallback();
void MatterSwitchPluginServerInitCallback();
void MatterTargetNavigatorPluginServerInitCallback();
void MatterTemperatureControlPluginServerInitCallback();
void MatterTemperatureMeasurementPluginServerInitCallback();
void MatterThermostatPluginServerInitCallback();
void MatterThermostatUserInterfaceConfigurationPluginServerInitCallback();
void MatterThreadNetworkDiagnosticsPluginServerInitCallback();
void MatterTimeFormatLocalizationPluginServerInitCallback();
void MatterTimeSynchronizationPluginServerInitCallback();
void MatterTotalVolatileOrganicCompoundsConcentrationMeasurementPluginServerInitCallback();
void MatterUnitLocalizationPluginServerInitCallback();
void MatterUnitTestingPluginServerInitCallback();
void MatterUserLabelPluginServerInitCallback();
void MatterWakeOnLanPluginServerInitCallback();
void MatterWiFiNetworkDiagnosticsPluginServerInitCallback();
void MatterWindowCoveringPluginServerInitCallback();

#include <esp_matter_cluster.h>

#define MATTER_PLUGINS_INIT esp_matter::cluster::plugin_init_callback_common();
