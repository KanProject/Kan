#pragma once

#include <test_routine_api.h>

#include <kan/context/all_system_names.h>
#include <kan/context/application_framework_system.h>
#include <kan/context/application_system.h>
#include <kan/context/render_backend_system.h>
#include <kan/log/logging.h>
#include <kan/precise_time/precise_time.h>
#include <kan/resource_pipeline/meta.h>
#include <kan/universe/macro.h>
#include <kan/universe/universe.h>
#include <kan/universe_resource_provider/provider.h>

enum test_routine_manual_result_t
{
    TEST_ROUTINE_MANUAL_RESULT_NOT_SET = 0u,
    TEST_ROUTINE_MANUAL_RESULT_PASSED,
    TEST_ROUTINE_MANUAL_RESULT_FAILED,
};

struct test_routine_singleton_t
{
    bool test_mode_enabled;

    kan_render_buffer_t expectation_read_back_buffer;

    KAN_REFLECTION_DYNAMIC_ARRAY_TYPE (kan_render_read_back_status_t)
    struct kan_dynamic_array_t expectation_read_back_statuses;

    enum test_routine_manual_result_t manual_result;
};

TEST_ROUTINE_API void test_routine_singleton_init (struct test_routine_singleton_t *instance);

TEST_ROUTINE_API void test_routine_singleton_shutdown (struct test_routine_singleton_t *instance);
