#!/usr/bin/env bash
# Set up development environment for esp-matter

set -e

# Function to print help message
print_help() {
    echo "Usage: setup-dev-env.sh [OPTIONS]"
    echo "Options:"
    echo "  --help                  Display this help message"
    echo "  -h                      Display this help message"
    echo "  -y                      Use non-interactive mode"
    echo "  -v                      Enable debug outputs"
    echo "  --idf-no-clone          Disable cloning of the esp-idf repo"
    echo "  --idf_path              Specify the path to install esp-idf"
    echo "  --idf_commit            Specify the esp-idf commit to use"
    echo "  --matter-no-install     Disable installation of the esp-matter."
    echo "  --esp_matter_commit     Specify the esp-matter commit to use"
    echo "  --submodule_commit      Specify the connectedhomeip commit to use"
    echo "  --matter_repo           Specify the esp-matter repo url to use"
    echo "  --no-pre-req            Disable installation of esp-matter pre-requisite"
    echo ""
}

ESP_MATTER_PATH=$(readlink -f "$(dirname "$0")")

# Parse arguments
args=()

#Default idf config for esp-matter
option_no_pre_req=false
option_idf_path="$ESP_MATTER_PATH/../esp-idf"
option_esp_matter_path="$ESP_MATTER_PATH"
option_idf_commit="-"
option_esp_matter_commit="-"
option_idf_no_clone=false
option_matter_no_install=false
option_matter_repo="https://github.com/espressif/esp-matter.git"

while [ "$1" != "" ]; do
    case "$1" in
    --help | -h)
        print_help
        exit 1
        ;;
    -y)
        # Use non-interactive mode.
        option_yes=true
        ;;
    -v)
        # Enable debug outputs.
        option_verbose=true
        ;;
    --idf_path)
        # Set esp-idf directory
        option_idf_path="$2"
        shift
        ;;
    --no-pre-req)
        # Set esp-idf is cloned option
        option_no_pre_req=true
        ;;
    --idf-no-clone)
        # Set esp-idf is cloned option
        option_idf_no_clone=true
        ;;
    --idf_commit)
        option_idf_commit="$2"
        shift
        ;;
    --matter_repo)
        option_matter_repo="$2"
        shift
        ;;
    --matter-no-install)
        # Set esp-idf is cloned option
        option_matter_no_install=true
        ;;
    --esp_matter_commit)
        option_esp_matter_commit="$2"
        shift
        ;;
    *)
        args+=("$1")
        ;;
    esac
    shift
done

# Select installation type
target_playbook="esp_matter.dev_env.matter" # default

# Initialize ansible args
ansible_args=()

# Confirm to start installation
if [ "$option_yes" = "true" ]; then
    echo -e "\e[36mRun the setup in non-interactive mode.\e[m"
else
    echo -e "\e[33mSetting up the build environment.\e[m"
    echo -e "\e[33mPassword may be needed for installation of packages.\e[m"

    read -rp ">  Are you sure you want to run setup? [y/N] " answer

    # Check whether to cancel
    if ! [[ ${answer:0:1} =~ y|Y ]]; then
        echo -e "\e[33mCancelled.\e[0m"
        exit 1
    fi

    # Check verbose option
    if [ "$option_no_pre_req" != "true" ]; then
        ansible_args+=("--ask-become-pass")
    fi

fi

# Check verbose option
if [ "$option_verbose" = "true" ]; then
    ansible_args+=("-vvv")
fi

# Check cloning of esp-idf
if [ "$option_idf_no_clone" = "true" ]; then
    ansible_args+=("--extra-vars" "install_idf=false")
fi

# Check module option
if [ "$option_idf_commit" == "-" ]; then
    option_idf_commit="v5.2.3"
fi
echo "Selected IDF commit is: ${option_idf_commit}"

# Check installation of esp-matter
if [ "$option_matter_no_install" = "true" ]; then
    ansible_args+=("--extra-vars" "install_matter=false")
fi

ansible_args+=("--extra-vars" "no_pre_req=$option_no_pre_req")

ansible_args+=("--extra-vars" "idf_path=$option_idf_path")

ansible_args+=("--extra-vars" "esp_matter_path=$option_esp_matter_path")

ansible_args+=("--extra-vars" "idf_commit=$option_idf_commit")

ansible_args+=("--extra-vars" "matter_repo=$option_matter_repo")

ansible_args+=("--extra-vars" "esp_matter_commit=$option_esp_matter_commit")

ansible_args+=("--extra-vars" "esp_matter_no_install=$option_matter_no_install")

ansible_args+=("--extra-vars" "idf_no_clone=$option_idf_no_clone")


# # Check matter commit option
# if [ "$option_esp_matter_commit" != "" ]; then
#     ansible_args+=("--extra-vars" "esp_matter_commit=$option_esp_matter_commit")
# fi

# Install sudo
if ! (command -v sudo >/dev/null 2>&1); then
    apt-get -y update
    apt-get -y install sudo
fi

# Install git
if ! (command -v git >/dev/null 2>&1); then
    sudo apt-get -y update
    sudo apt-get -y install git
fi

# Install pip for ansible
if ! (python3 -m pip --version >/dev/null 2>&1); then
    sudo apt-get -y update
    sudo apt-get -y install python3-pip python3-venv
fi

# Install pipx for ansible
if ! (python3 -m pipx --version >/dev/null 2>&1); then
    sudo apt-get -y update
    # python3 -m pip install --user pipx
    sudo apt-get install pipx
    pipx install pipx
fi

# Install ansible
# python3 -m pipx ensurepath
pipx ensurepath
export PATH="${PIPX_BIN_DIR:=$HOME/.local/bin}:$PATH"
pipx install --include-deps --force "ansible==6.*"

# Install ansible collections
echo -e "\e[36m"ansible-galaxy collection install -f -r "$ESP_MATTER_PATH/ansible-galaxy-requirements.yaml" "\e[m"
ansible-galaxy collection install -f -r "$ESP_MATTER_PATH/ansible-galaxy-requirements.yaml"

# Run ansible
echo -e "\e[36m"ansible-playbook "$target_playbook" "${ansible_args[@]}" "\e[m"
if ansible-playbook "$target_playbook" "${ansible_args[@]}"; then
    echo -e "\e[32mCompleted.\e[0m"
    echo -e "\033[1;32m✔ SUCCESS:\033[0m ESP-Matter installation completed successfully!"
    echo -e "\033[1;32m Export ESP-IDF and ESP-Matter \033[0m"
    echo -e "\033[1;32m RUN: \033[0m source "$option_idf_path"/export.sh"
    echo -e "\033[1;32m RUN:\033[0m source "$option_esp_matter_path"/export.sh"
    exit 0
else
    echo -e "\e[31mFailed.\e[0m"
    exit 1
fi