# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

# Set your project's banner and color.
export PW_BRANDING_BANNER="banner.txt"
export PW_BRANDING_BANNER_COLOR=cyan

# First, activate the Pigweed development environment.
_project_abspath () {
  python -c "import os.path; print(os.path.abspath('$@'))"
}

# Default to third_party/pigweed/bootstrap.sh
PW_SETUP_SCRIPT_PATH=$(_project_abspath third_party/pigweed/bootstrap.sh)

# Set third_party/pigweed/{activate.sh,bootstrap.sh}
# depending on which name was sourced.

# Shell: bash.
if test -n "$BASH"; then
  PW_SETUP_SCRIPT_PATH="$(_project_abspath third_party/pigweed/"$BASH_SOURCE")"
# Shell: zsh.
elif test -n "$ZSH_NAME"; then
  PW_SETUP_SCRIPT_PATH="$(_project_abspath third_party/pigweed/"${(%):-%N}")"
# Shell: dash.
elif test ${0##*/} = dash; then
  PW_SETUP_SCRIPT_PATH="$(_project_abspath \
    third_party/pigweed/"$(lsof -p $$ -Fn0 | tail -1 | sed 's#^[^/]*##;')")"
# If everything else fails, try $0. It could work.
else
  PW_SETUP_SCRIPT_PATH="$(_project_abspath third_party/pigweed/"$0")"
fi

if [ ! -e ${PW_SETUP_SCRIPT_PATH} ]; then
  echo "Error: ${PW_SETUP_SCRIPT_PATH} not found."
  echo "Did you forget to initialize the git submodules?"
  echo "To setup the git submodules run:"
  echo "  git submodule init"
  echo "  git submodule update"
  return
fi

export SAMPLE_PROJECT_ROOT=$(_project_abspath)

source $PW_SETUP_SCRIPT_PATH

# Add user-defined environment setup here.
