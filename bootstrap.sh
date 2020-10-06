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

PW_BOOTSTRAP_SCRIPT=$(_project_abspath)/third_party/pigweed/bootstrap.sh

if [ ! -e ${PW_BOOTSTRAP_SCRIPT} ]; then
  echo "Error: ${PW_BOOTSTRAP_SCRIPT} not found."
  echo "Did you forget to initialize the git submodules?"
  echo "To setup the git submodules run:"
  echo "  git submodule init"
  echo "  git submodule update"
  return
fi

export SAMPLE_PROJECT_ROOT=$(_project_abspath)

source $PW_BOOTSTRAP_SCRIPT

# Add user-defined environment setup here.
