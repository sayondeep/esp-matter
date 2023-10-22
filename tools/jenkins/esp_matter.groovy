def setup_directories_firmware() {
    sh '''
    echo "repos path: ${REPOS_PATH}"
    ls -l ${REPOS_PATH} 
    cd ${REPOS_PATH}
    cd ..
    ls -l
    mkdir -p ${PACKAGE_PATH}
    mkdir -p ${PACKAGE_PATH}/Firmware
    mkdir -p ${PACKAGE_PATH}/Firmware/Evaluation
    '''
}

def setup_directories_script() {
    sh '''
    cd ${REPOS_PATH}
    mkdir -p ${PACKAGE_PATH}
    mkdir -p ${PACKAGE_PATH}/Script
    '''
}

def setup_directories_tools() {
    sh '''
    cd ${REPOS_PATH}
    mkdir -p ${PACKAGE_PATH}
    mkdir -p ${PACKAGE_PATH}/Tools
    '''
}

def setup_environment() {
    sh '''
    cd ${REPOS_PATH}/esp-idf
    . ./export.sh
    cd ..

    cd ${REPOS_PATH}/esp-matter
    git pull --no-recurse-submodules
    git submodule update --init --depth 1
    cd connectedhomeip/connectedhomeip
    ./scripts/checkout_submodules.py --platform esp32 linux --shallow
    cd ../..
    # ./install.sh
    . ./export.sh
    cd ..

    IDF_BRANCH=`cd ${REPOS_PATH}/esp-idf && git rev-parse --abbrev-ref HEAD`
    IDF_COMMIT_ID=`cd ${REPOS_PATH}/esp-idf && git rev-parse --verify HEAD`
    ESP_MATTER_BRANCH=`cd ${REPOS_PATH}/esp-matter && git rev-parse --abbrev-ref HEAD`
    ESP_MATTER_COMMIT_ID=`cd ${REPOS_PATH}/esp-matter && git rev-parse --verify HEAD`
 
  //  echo "esp-idf: ${IDF_BRANCH}: ${IDF_COMMIT_ID}" >> ${REPOS_PATH}/build_details.txt
  //  echo "esp-matter: ${ESP_MATTER_BRANCH}: ${ESP_MATTER_COMMIT_ID}" >> ${REPOS_PATH}/build_details.txt    
  //  printf "\n\n" >> ${REPOS_PATH}/build_details.txt

  //  echo "product: ${product}" >> ${REPOS_PATH}/build_details.txt
  //  echo "chip: ${chip}" >> ${REPOS_PATH}/build_details.txt
  //  echo "flash_size: 4MB" >> ${REPOS_PATH}/build_details.txt
  //  echo "secure_boot: enabled" >> ${REPOS_PATH}/build_details.txt
    
  //  printf "\n\n" >> ${REPOS_PATH}/build_details.txt
    '''
}

def firmware_build() {
    sh '''
    //printf "\n\n" >> ${REPOS_PATH}/build_details.txt
    //echo "firmware_type: ${FIRMWARE_TYPE}" >> ${REPOS_PATH}/build_details.txt
 
    cd ${ESP_MATTER_PATH}/examples/${product}

    . ${IDF_PATH}/export.sh
    . ${ESP_MATTER_PATH}/export.sh;

    idf.py fullclean
    idf.py set-target ${chip}
    idf.py build

    # Create Merged binary
    cd build
    BINARY_FILES=$(cat flash_args | sed 's/--.*//' | tr '\n' ' ' | sed 's/^ *//;s/ *$//')
    cd ..
    '''
}

// def firmware_build_save() {
//     sh '''
//     PACKAGE_FIRMWARE_PATH=${PACKAGE_PATH}/Firmware/${FIRMWARE_TYPE}
//     cd ${ESP_MATTER_PATH}/examples/${product}

//     # Firmware binary
//     FILE_PREFIX=esp_matter_firmware_${product}_${chip}_${BUILD_NUMBER}
//     cp build/${product}.bin ${PACKAGE_FIRMWARE_PATH}/${FILE_PREFIX}_ota_0.bin
//     cp build/bootloader/bootloader.bin ${PACKAGE_FIRMWARE_PATH}/${FILE_PREFIX}_bootloader.bin
//     cp build/${product}_merged.bin ${PACKAGE_FIRMWARE_PATH}/${FILE_PREFIX}_merged.bin

//     # Helper files
//     echo -n "${FILE_PREFIX}" >> ${PACKAGE_FIRMWARE_PATH}/file_prefix_ota_0.txt
//     echo -n "${FILE_PREFIX}" >> ${PACKAGE_FIRMWARE_PATH}/file_prefix_bootloader.txt

//     # json
//     cp build/project_description.json ${PACKAGE_FIRMWARE_PATH}/project_description.json
//     '''
// }

// def script_artifacts_create() {
//     sh '''
//     PACKAGE_SCRIPT_PATH=${PACKAGE_PATH}/Script
//     SCRIPTS_DIRECTORY_NAME=manufacturing_scripts
//     SCRIPTS_PATH=${PACKAGE_SCRIPT_PATH}/${SCRIPTS_DIRECTORY_NAME}
//     mkdir -p ${SCRIPTS_PATH}

//     # esp-idf
//     mkdir -p ${SCRIPTS_PATH}/esp-idf
//     mkdir -p ${SCRIPTS_PATH}/esp-idf/components
//     mkdir -p ${SCRIPTS_PATH}/esp-idf/components/nvs_flash
//     cp -r ${IDF_PATH}/components/nvs_flash/nvs_partition_generator ${SCRIPTS_PATH}/esp-idf/components/nvs_flash/

//     mkdir -p ${SCRIPTS_PATH}/esp-idf/tools
//     cp -r ${IDF_PATH}/tools/mass_mfg ${SCRIPTS_PATH}/esp-idf/tools/

//     # esp-matter
//     mkdir -p ${SCRIPTS_PATH}/esp-matter
//     mkdir -p ${SCRIPTS_PATH}/esp-matter/tools
//     cp -r ${ESP_MATTER_PATH}/tools/mfg_tool ${SCRIPTS_PATH}/esp-matter/tools/

//     mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip
//     mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip
//     mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/scripts
//     mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/scripts/tools
//     cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/scripts/tools/spake2p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/scripts/tools

//     mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/src
//     mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/setup_payload
//     cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/setup_payload/python ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/setup_payload

//     # script_description
//     mkdir -p ${SCRIPTS_PATH}/script_description
//     cp ${SCRIPTS_PATH}/esp-matter/tools/mfg_tool/requirements.txt ${SCRIPTS_PATH}/script_description/
//     cp ${SCRIPTS_PATH}/esp-matter/tools/mfg_tool/README.md ${SCRIPTS_PATH}/script_description/

//     # Create manufacturing_scripts.zip folder
//     cd ${PACKAGE_SCRIPT_PATH}
//     zip ${SCRIPTS_PATH}.zip -r ${SCRIPTS_DIRECTORY_NAME}/
//     '''
// }

// def tools_artifacts_create() {
//     sh '''
//     PACKAGE_TOOLS_PATH=${PACKAGE_PATH}/Tools
//     TOOL_DIRECTORY_NAME=chip-tool
//     TOOL_PATH=${PACKAGE_TOOLS_PATH}/${TOOL_DIRECTORY_NAME}
//     mkdir -p ${TOOL_PATH}

//     cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host/chip-tool ${TOOL_PATH}/

//     TOOL_DIRECTORY_NAME=chip-cert
//     TOOL_PATH=${PACKAGE_TOOLS_PATH}/${TOOL_DIRECTORY_NAME}
//     mkdir -p ${TOOL_PATH}

//     cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host/chip-cert ${TOOL_PATH}/

//     TOOL_DIRECTORY_NAME=chip-ota-provider-app
//     TOOL_PATH=${PACKAGE_TOOLS_PATH}/${TOOL_DIRECTORY_NAME}
//     mkdir -p ${TOOL_PATH}

//     cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/debug/chip-ota-provider-app ${TOOL_PATH}/

//     # esp-matter
//     mkdir -p ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/app
//     cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/app/ota_image_tool.py ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/app

//     mkdir -p ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/controller
//     cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/controller/python ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/controller
//     '''
// }

// def artifacts_save() {
//     sh '''
//     cd ${REPOS_PATH}
//     cp build_details.txt ${PACKAGE_NAME}/

//     mkdir ${WORKSPACE}/artifacts
//     cp build_details.txt ${WORKSPACE}/artifacts/
//     tar -zcvf ${WORKSPACE}/artifacts/${PACKAGE_NAME}.tar.gz ${PACKAGE_NAME}
//     '''
// }

return this

