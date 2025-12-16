#pragma once

#include <resource_test_expectation_api.h>

#include <kan/api_common/core_types.h>
#include <kan/container/dynamic_array.h>
#include <kan/reflection/markup.h>

KAN_C_HEADER_BEGIN

struct test_expectation_t
{
    kan_instance_size_t width;
    kan_instance_size_t height;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (uint32_t)
    struct kan_dynamic_array_t rgba_data;
};

RESOURCE_TEST_EXPECTATION_API void test_expectation_init (struct test_expectation_t *instance);

RESOURCE_TEST_EXPECTATION_API void test_expectation_shutdown (struct test_expectation_t *instance);

struct test_setup_t
{
    kan_interned_string_t screenshot_base_name;
    kan_instance_size_t frames_to_wait;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_interned_string_t)
    struct kan_dynamic_array_t expectations;
};

RESOURCE_TEST_EXPECTATION_API void test_setup_init (struct test_setup_t *instance);

RESOURCE_TEST_EXPECTATION_API void test_setup_shutdown (struct test_setup_t *instance);

KAN_C_HEADER_END
