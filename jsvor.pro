# Copyright 2016 lyobzik
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = jsvor \
          tests_common \
          tests \
          perftests \
          jsvor_validator \


jsvor.file = lib/jsvor.pro
tests_common.file = tests_common/tests_common.pro
tests.file = tests/tests.pro
perftests.file = perftests/perftests.pro
jsvor_validator.file = jsvor_validator/jsvor_validator.pro

tests_common.depends = jsvor
tests.depends = tests_common jsvor
perftests.depends = tests_common tests jsvor
jsvor_validator.depends = jsvor
