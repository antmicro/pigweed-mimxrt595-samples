# Copyright 2023 The Pigweed Authors
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

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

workspace(name = "mimxrt595-examples")

http_archive(
    name = "rules_cc",
    integrity = "sha256-NddP6xi6LzsIHT8bMSVJ2NtoURbN+l3xpjvmIgB6aSg=",
    strip_prefix = "rules_cc-1acf5213b6170f1f0133e273cb85ede0e732048f",
    urls = [
        "https://github.com/bazelbuild/rules_cc/archive/1acf5213b6170f1f0133e273cb85ede0e732048f.zip",
    ],
)

# Load Pigweed's own dependencies that we'll need.
#
# TODO: b/274148833 - Don't require users to spell these out.
http_archive(
    name = "platforms",
    sha256 = "5308fc1d8865406a49427ba24a9ab53087f17f5266a7aabbfc28823f3916e1ca",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.6/platforms-0.0.6.tar.gz",
        "https://github.com/bazelbuild/platforms/releases/download/0.0.6/platforms-0.0.6.tar.gz",
    ],
)

http_archive(
    name = "bazel_skylib",
    sha256 = "aede1b60709ac12b3461ee0bb3fa097b58a86fbfdb88ef7e9f90424a69043167",
    strip_prefix = "bazel-skylib-1.6.1",  # 2024-04-24
    urls = ["https://github.com/bazelbuild/bazel-skylib/archive/refs/tags/1.6.1.tar.gz"],
)

http_archive(
    name = "rules_proto",
    sha256 = "dc3fb206a2cb3441b485eb1e423165b231235a1ea9b031b4433cf7bc1fa460dd",
    strip_prefix = "rules_proto-5.3.0-21.7",
    urls = [
        "https://github.com/bazelbuild/rules_proto/archive/refs/tags/5.3.0-21.7.tar.gz",
    ],
)

http_archive(
    name = "rules_python",
    sha256 = "e3f1cc7a04d9b09635afb3130731ed82b5f58eadc8233d4efb59944d92ffc06f",
    strip_prefix = "rules_python-0.33.2",
    url = "https://github.com/bazelbuild/rules_python/releases/download/0.33.2/rules_python-0.33.2.tar.gz",
)

load("@rules_python//python:repositories.bzl", "py_repositories", "python_register_toolchains")

py_repositories()

http_archive(
    name = "com_google_protobuf",
    sha256 = "616bb3536ac1fff3fb1a141450fa28b875e985712170ea7f1bfe5e5fc41e2cd8",
    strip_prefix = "protobuf-24.4",
    url = "https://github.com/protocolbuffers/protobuf/releases/download/v24.4/protobuf-24.4.tar.gz",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

http_archive(
    name = "rules_fuzzing",
    sha256 = "d9002dd3cd6437017f08593124fdd1b13b3473c7b929ceb0e60d317cb9346118",
    strip_prefix = "rules_fuzzing-0.3.2",
    urls = ["https://github.com/bazelbuild/rules_fuzzing/archive/v0.3.2.zip"],
)

load("@rules_fuzzing//fuzzing:repositories.bzl", "rules_fuzzing_dependencies")

rules_fuzzing_dependencies()

# Add Pigweed itself, as a submodule.
#
# TODO: b/300695111 - Support depending on Pigweed as a git_repository, even if
# you use pw_toolchain.
local_repository(
    name = "pigweed",
    path = "third_party/pigweed",
)

local_repository(
    name = "pw_toolchain",
    path = "third_party/pigweed/pw_toolchain_bazel",
)

# Get ready to grab CIPD dependencies. For this minimal example, the only
# dependencies will be the toolchains and OpenOCD (used for flashing).
load(
    "@pigweed//pw_env_setup/bazel/cipd_setup:cipd_rules.bzl",
    "cipd_client_repository",
    "cipd_repository",
)

cipd_client_repository()

load("@pigweed//pw_toolchain:register_toolchains.bzl", "register_pigweed_cxx_toolchains")

register_pigweed_cxx_toolchains()

# Get the OpenOCD binary (we'll use it for flashing).
cipd_repository(
    name = "openocd",
    path = "infra/3pp/tools/openocd/${os}-${arch}",
    tag = "version:2@0.11.0-3",
)

# Set up the Python interpreter we'll need.
python_register_toolchains(
    name = "python3",
    python_version = "3.11",
    ignore_root_user_error = True,
)

load("@python3//:defs.bzl", "interpreter")
load("@rules_python//python:pip.bzl", "pip_parse")

pip_parse(
    name = "python_packages",
    python_interpreter_target = interpreter,
    requirements_darwin = "@pigweed//pw_env_setup/py/pw_env_setup/virtualenv_setup:upstream_requirements_darwin_lock.txt",
    requirements_linux = "@pigweed//pw_env_setup/py/pw_env_setup/virtualenv_setup:upstream_requirements_linux_lock.txt",
    requirements_windows = "@pigweed//pw_env_setup/py/pw_env_setup/virtualenv_setup:upstream_requirements_windows_lock.txt",
)

load("@python_packages//:requirements.bzl", "install_deps")

install_deps()

# Setup Nanopb protoc plugin.
# Required by: Pigweed.
# Used in modules: pw_protobuf.
http_archive(
    name = "com_github_nanopb_nanopb",
    sha256 = "3f78bf63722a810edb6da5ab5f0e76c7db13a961c2aad4ab49296e3095d0d830",
    strip_prefix = "nanopb-0.4.8",
    url = "https://github.com/nanopb/nanopb/archive/refs/tags/0.4.8.tar.gz",
)

load("@com_github_nanopb_nanopb//extra/bazel:nanopb_deps.bzl", "nanopb_deps")

nanopb_deps()

load("@com_github_nanopb_nanopb//extra/bazel:python_deps.bzl", "nanopb_python_deps")

nanopb_python_deps(interpreter)

load("@com_github_nanopb_nanopb//extra/bazel:nanopb_workspace.bzl", "nanopb_workspace")

nanopb_workspace()

http_archive(
    name = "freertos",
    build_file = "@pigweed//third_party/freertos:freertos.BUILD.bazel",
    sha256 = "89af32b7568c504624f712c21fe97f7311c55fccb7ae6163cda7adde1cde7f62",
    strip_prefix = "FreeRTOS-Kernel-10.5.1",
    urls = ["https://github.com/FreeRTOS/FreeRTOS-Kernel/archive/refs/tags/V10.5.1.tar.gz"],
)

git_repository(
    name = "com_google_emboss",
    # LINT.IfChange(emboss)
    remote = "https://pigweed.googlesource.com/third_party/github/google/emboss",
    tag = "v2024.0809.170004",
    # LINT.ThenChange(/pw_package/py/pw_package/packages/emboss.py:emboss)
)

# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
# When you first run this tool, it'll recommend a sha256 hash to put here with a message like: "DEBUG: Rule 'hedron_compile_commands' indicated that a canonical reproducible form can be obtained by modifying arguments sha256 = ..."
http_archive(
    name = "hedron_compile_commands",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/1e08f8e0507b6b6b1f4416a9a22cf5c28beaba93.tar.gz",
    strip_prefix = "bazel-compile-commands-extractor-1e08f8e0507b6b6b1f4416a9a22cf5c28beaba93",
    integrity = "sha256-BEsUixEeF87mHYw6ru1CBp9zJUYDgjQK/KSRn4JlCUw="
)
load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")
hedron_compile_commands_setup()
load("@hedron_compile_commands//:workspace_setup_transitive.bzl", "hedron_compile_commands_setup_transitive")
hedron_compile_commands_setup_transitive()
load("@hedron_compile_commands//:workspace_setup_transitive_transitive.bzl", "hedron_compile_commands_setup_transitive_transitive")
hedron_compile_commands_setup_transitive_transitive()
load("@hedron_compile_commands//:workspace_setup_transitive_transitive_transitive.bzl", "hedron_compile_commands_setup_transitive_transitive_transitive")
hedron_compile_commands_setup_transitive_transitive_transitive()

# Setup Fuchsia SDK.
# Required by: bt-host.
# Used in modules: //pw_bluetooth_sapphire.
# NOTE: These blocks cannot feasibly be moved into a macro.
# See https://github.com/bazelbuild/bazel/issues/1550
git_repository(
    name = "fuchsia_infra",
    # ROLL: Warning: this entry is automatically updated.
    # ROLL: Last updated 2024-08-10.
    # ROLL: By https://cr-buildbucket.appspot.com/build/8739954865728922337.
    commit = "a61ac0c9305860e9d439ee153b5d5bb34c176fcd",
    remote = "https://fuchsia.googlesource.com/fuchsia-infra-bazel-rules",
)

load("@fuchsia_infra//:workspace.bzl", "fuchsia_infra_workspace")

fuchsia_infra_workspace()

FUCHSIA_LINUX_SDK_VERSION = "version:22.20240717.3.1"

# The Fuchsia SDK is no longer released for MacOS, so we need to pin an older
# version, from the halcyon days when this OS was still supported.
FUCHSIA_MAC_SDK_VERSION = "version:20.20240408.3.1"

cipd_repository(
    name = "fuchsia_sdk",
    path = "fuchsia/sdk/core/fuchsia-bazel-rules/${os}-amd64",
    tag_by_os = {
        "linux": FUCHSIA_LINUX_SDK_VERSION,
        "mac": FUCHSIA_MAC_SDK_VERSION,
    },
)

load("@fuchsia_sdk//fuchsia:deps.bzl", "rules_fuchsia_deps")

rules_fuchsia_deps()

register_toolchains("@fuchsia_sdk//:fuchsia_toolchain_sdk")

cipd_repository(
    name = "fuchsia_products_metadata",
    path = "fuchsia/development/product_bundles/v2",
    tag_by_os = {
        "linux": FUCHSIA_LINUX_SDK_VERSION,
        "mac": FUCHSIA_MAC_SDK_VERSION,
    },
)

load("@fuchsia_sdk//fuchsia:products.bzl", "fuchsia_products_repository")

fuchsia_products_repository(
    name = "fuchsia_products",
    metadata_file = "@fuchsia_products_metadata//:product_bundles.json",
)

load("@fuchsia_sdk//fuchsia:clang.bzl", "fuchsia_clang_repository")

fuchsia_clang_repository(
    name = "fuchsia_clang",
    # TODO: https://pwbug.dev/346354914 - Reuse @llvm_toolchain. This currently
    # leads to flaky loading phase errors!
    # from_workspace = "@llvm_toolchain//:BUILD",
    cipd_tag = "git_revision:c58bc24fcf678c55b0bf522be89eff070507a005",
    sdk_root_label = "@fuchsia_sdk",
)

load("@fuchsia_clang//:defs.bzl", "register_clang_toolchains")

register_clang_toolchains()

# Since Fuchsia doesn't release arm64 SDKs, use this to gate Fuchsia targets.
load("@pigweed//pw_env_setup:bazel/host_metadata_repository.bzl", "host_metadata_repository")

host_metadata_repository(
    name = "host_metadata",
)

git_repository(
    name = "boringssl",
    commit = "74a51c6ab3c9c674a62bf02c904f12e5109761b8",
    remote = "https://boringssl.googlesource.com/boringssl",
    build_file = "@pigweed//third_party/boringssl:BUILD.bazel",
)

git_repository(
    name = "mcuxpresso",
    commit = "5d978a8f88d947b8158b21557b7f997e092f79ac",
    remote = "https://github.com/nxp-mcuxpresso/mcux-sdk",
    build_file = "@pigweed//third_party/mcuxpresso:BUILD_sdk.bazel",
)
