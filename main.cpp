/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "mbed.h"
#include "ble/BLE.h"

/* Optional: Device Name, add for human read-ability */
const static char     DEVICE_NAME[] = "Line Beacon 001";                                           /* Name it */


const static uint8_t AdvServiceUUIDs[] = { 0x6F, 0xFE };                           
const static uint8_t AdvData[] = { 0x6F, 0xFE, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00 };  /* Replace 0xFF, 0xFF, 0xFF, 0xFF, 0xFF to your bot's HWID */
//const static uint8_t AdvDataFrameType[] = { 0x02 };
//const static uint8_t AdvDataHWID[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
//const static uint8_t AdvDataMeasuredTxPower[] = { 0x7F };
//const static uint8_t AdvDataDeviceMessage[] = { 0x00 };

/* Optional: Restart advertising when peer disconnects */
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising();
}
/**
 * This function is called when the ble initialization process has failed
 */
void onBleInitError(BLE &ble, ble_error_t error)
{
    /* Avoid compiler warnings */
    (void) ble;
    (void) error;
    
    /* Initialization error handling should go here */
}    

/**
 * Callback triggered when the ble initialization process has finished
 */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        /* In case of error, forward the error handling to onBleInitError */
        onBleInitError(ble, error);
        return;
    }

    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }
    
    /* Set device name characteristic data */
    ble.gap().setDeviceName((const uint8_t *) DEVICE_NAME);

    /* Optional: add callback for disconnection */
    ble.gap().onDisconnection(disconnectionCallback);

    /* Sacrifice 3B of 31B to Advertising Flags */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE );
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);

    /* Sacrifice 2B of 31B to AdvType overhead, rest goes to AdvData array you define */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, AdvServiceUUIDs, sizeof(AdvServiceUUIDs));
    
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, AdvServiceUUIDs, sizeof(AdvServiceUUIDs));

    /* Optional: Add name to device */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, AdvData, sizeof(AdvData));
    //ble.gap().setAdvertisingData((const uint8_t *)AdvServiceUUIDs, (const uint8_t *)AdvData);

    /* Set advertising interval. Longer interval == longer battery life */
    ble.gap().setAdvertisingInterval(100); /* 100ms */

    /* Start advertising */
    ble.gap().startAdvertising();
}

int main(void)
{
    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
 
    /* Initialize BLE baselayer, always do this first! */
    ble.init(bleInitComplete);

    /* Infinite loop waiting for BLE events */
    while (true) {
        /* Save power while waiting for callback events */
        ble.waitForEvent();
    }
}
