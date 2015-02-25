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
