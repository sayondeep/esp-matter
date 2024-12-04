// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <esp_log.h>
#include <string.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_rainmaker.h>
// #include <esp_rmaker_core.h>
// #include <esp_rmaker_user_mapping.h>
#include <app/util/attribute-storage.h>
#include <app/AttributeAccessInterface.h>
#include <app/AttributeAccessInterfaceRegistry.h>

#define ESP_MATTER_CUSTOMAPI_COMMAND_LIMIT              5 /* This command can be called 5 times per reboot */
#define ESP_MATTER_CUSTOMAPI_MAX_DATA_LEN               40
#define ESP_MATTER_CUSTOMAPI_MAX_CHALLENGE_LEN          40
#define ESP_MATTER_CUSTOMAPI_MAX_NODE_ID_LEN            40
#define ESP_MATTER_CUSTOMAPI_MAX_CHALLENGE_RESPONSE_LEN 150

using namespace chip::app;
using namespace chip::app::Clusters;

#define RAINMAKER_CLUSTER_REVISION 2

static const char *TAG = "esp_matter_rainmaker";

class ForcedSizeBuffer : public chip::app::DataModel::EncodableToTLV
{
public:
    ForcedSizeBuffer(uint32_t size, uint32_t val )
    {
        if (mBuffer.Alloc(size))
        {
            // No significance with using 0x12, just using a value.
            memset(mBuffer.Get(), val, size);
        }
    }

    // No significance with using 0x12 as the CommandId, just using a value.
    static constexpr chip::CommandId GetCommandId() { return 0x12; }
    CHIP_ERROR EncodeTo(chip::TLV::TLVWriter & aWriter, chip::TLV::Tag aTag) const override
    {
        VerifyOrReturnError(mBuffer, CHIP_ERROR_NO_MEMORY);

        chip::TLV::TLVType outerContainerType;
        ReturnErrorOnFailure(aWriter.StartContainer(aTag, chip::TLV::kTLVType_Structure, outerContainerType));
        ReturnErrorOnFailure(chip::app::DataModel::Encode(aWriter, chip::TLV::ContextTag(1), chip::ByteSpan(mBuffer.Get(), mBuffer.AllocatedSize())));
        return aWriter.EndContainer(outerContainerType);
    }

private:
    chip::Platform::ScopedMemoryBufferWithSize<uint8_t> mBuffer;
};

namespace esp_matter {

// RainMaker cluster data model definition
namespace cluster {
        namespace customapi {
            static constexpr chip::EndpointId endpoint_id = 0x00000000; /* Same as root node endpoint. This will always be
                                                                        endpoint_id 0. */
            static constexpr chip::ClusterId Id = 0x0000FC00; /* 0xAA11 == manufacturer code.
                                                                 0xFC00 == start of manufacuturer specific cluster_id */

            namespace attribute {
                namespace status {
                    static constexpr chip::AttributeId Id = 0x0000;
                } /* status */
                namespace node_id {
                    static constexpr chip::AttributeId Id = 0x0001;
                } /* node_id */
                namespace challenge {
                    static constexpr chip::AttributeId Id = 0x0002;
                } /* challenge */
                namespace value {
                    static constexpr chip::AttributeId Id = 0x0003;
                } /* challenge */
            } /* attribute */

            namespace command {
                namespace configuration {
                    static constexpr chip::CommandId Id = 0x0000;
                } /* configuration */
                namespace sign_data {
                    static constexpr chip::CommandId Id = 0x0001;
                } /* sign_data */
                namespace read_status {
                    static constexpr chip::CommandId Id = 0x0002;
                } /* sign_data */

            } /* command */
        } /* customapi */
    }

    namespace customapi {

        static esp_err_t status_attribute_update(bool status)
        {
            uint16_t endpoint_id = cluster::customapi::endpoint_id;
            uint32_t cluster_id = cluster:: customapi::Id;
            uint32_t attribute_id = cluster:: customapi::attribute::status::Id;
            esp_matter_attr_val_t val = esp_matter_bool(status);
            return attribute::update(endpoint_id, cluster_id, attribute_id, &val);
        }

        static esp_err_t node_id_attribute_update(char *node_id)
        {
            if (!node_id)
            {
                return ESP_ERR_INVALID_ARG;
            }
            uint16_t endpoint_id = cluster:: customapi::endpoint_id;
            uint32_t cluster_id = cluster:: customapi::Id;
            uint32_t attribute_id = cluster:: customapi::attribute::node_id::Id;
            esp_matter_attr_val_t val = esp_matter_char_str(node_id, strlen(node_id));
            return attribute::update(endpoint_id, cluster_id, attribute_id, &val);
        }

        static esp_err_t challenge_attribute_update(char *challenge)
        {
            uint16_t endpoint_id = cluster:: customapi::endpoint_id;
            uint32_t cluster_id = cluster:: customapi::Id;
            uint32_t attribute_id = cluster:: customapi::attribute::challenge::Id;
            esp_matter_attr_val_t val = esp_matter_char_str(challenge, strlen(challenge));
            return attribute::update(endpoint_id, cluster_id, attribute_id, &val);
        }

        static esp_err_t command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
        {
            /* Get ids */
            uint16_t endpoint_id = command_path.mEndpointId;
            uint32_t cluster_id = command_path.mClusterId;
            uint32_t command_id = command_path.mCommandId;

            /* Return if this is not the customapi configuration command */
            if (endpoint_id != cluster:: customapi::endpoint_id || cluster_id != cluster:: customapi::Id || command_id != cluster:: customapi::command::configuration::Id)
            {
                ESP_LOGE(TAG, "Got customapi command callback for some other command. This should not happen.");
                return ESP_FAIL;
            }
            ESP_LOGI(TAG, "customapi configuration command callback");
            static int command_count = ESP_MATTER_CUSTOMAPI_COMMAND_LIMIT;
            if (command_count <= 0)
            {
                ESP_LOGE(TAG, "This command has reached a limit. Please reboot to try again.");
                return ESP_FAIL;
            }
            command_count--;

            return ESP_OK;
        }

        static esp_err_t read_status_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
        {
            /* Get ids */
            uint16_t endpoint_id = command_path.mEndpointId;
            uint32_t cluster_id = command_path.mClusterId;
            uint32_t command_id = command_path.mCommandId;

            /* Return if this is not the correct cluster and command*/
            if (/*endpoint_id != light_endpoint_id || */cluster_id != cluster::customapi::Id || command_id != cluster::customapi::command::read_status::Id)
            {
                ESP_LOGE(TAG, "ERROR! read_usb_status_callback has incorrect ids for endpoint, cluster or command");
                return ESP_FAIL;
            }

            ESP_LOGI(TAG, "About to read the USB voltage...");

            chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;
            if (!command_obj)
            {
                ESP_LOGE(TAG, "Command Object cannot be NULL");
                return ESP_FAIL;
            }

            // Test value initially
            int test_value = 55;

            ForcedSizeBuffer responseData(sizeof(uint32_t), test_value);

            ESP_LOGI(TAG, "About to send response data back to controller for endpoint : 0x%x , cluster ID 0x%lx and command ID 0x%lx", endpoint_id, cluster_id, command_id);

            chip::ChipError chip_err = command_obj->AddResponseData(command_path, (chip::CommandId)command_id, responseData);
            if (chip_err != CHIP_NO_ERROR)
            {
                    ESP_LOGE(TAG, "Unable to send response data from read_usb_status_callback err:%" CHIP_ERROR_FORMAT, chip_err.Format());
                return ESP_FAIL;
            }

            ESP_LOGI(TAG, "Sending success status back to Server for endpoint : 0x%x , cluster ID 0x%lx and command ID 0x%lx", endpoint_id, cluster_id, command_id);

            command_obj->AddStatus(command_path, chip::Protocols::InteractionModel::Status::Success);

                return ESP_OK;
        }

        static esp_err_t custom_cluster_create()
        {
            /* Get the endpoint */
            node_t *node = node::get();
            endpoint_t *endpoint = endpoint::get(node, cluster:: customapi::endpoint_id);

            /* Create custom api cluster */
            cluster_t *cluster = esp_matter::cluster::create(endpoint, cluster:: customapi::Id, CLUSTER_FLAG_SERVER);
            attribute::create(cluster, Globals::Attributes::ClusterRevision::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint16(1));

            /* Create custom status attribute */
            /* Update the value of the attribute */
            attribute::create(cluster, cluster:: customapi::attribute::status::Id, ATTRIBUTE_FLAG_NONE, esp_matter_bool(false));

            /* Create custom node_id attribute */
            /* Update the value of the attribute */
            char node_id[ESP_MATTER_CUSTOMAPI_MAX_NODE_ID_LEN] = {0};
            attribute::create(cluster, cluster:: customapi::attribute::node_id::Id, ATTRIBUTE_FLAG_NONE, esp_matter_char_str(node_id, sizeof(node_id)));

            /* Create custom challenge attribute */
            /* Update the value of the attribute */
            char challenge[ESP_MATTER_CUSTOMAPI_MAX_CHALLENGE_LEN] = {0};
            attribute::create(cluster, cluster:: customapi::attribute::challenge::Id, ATTRIBUTE_FLAG_NONE, esp_matter_char_str(challenge, sizeof(challenge)));

            /* Create custom configuration command */
            command::create(cluster, cluster:: customapi::command::configuration::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM, command_callback);

            command_t *read_status_command = command::create(cluster, cluster:: customapi::command::read_status::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM, read_status_callback);
            if (read_status_command == NULL)
            {
                    ESP_LOGE(TAG, "ERROR! Unable to create read_status_command");
                    return ESP_FAIL;
            }
            return ESP_OK;
        }

        esp_err_t init()
        {
            return custom_cluster_create();
        }

        esp_err_t start()
        {
            return status_attribute_update(true);
        }
    } /* customapi */
} /* esp_matter */