register_concrete (math)
concrete_include (PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
concrete_sources ("*.c")
concrete_require (
        SCOPE PUBLIC
        ABSTRACT reflection
        CONCRETE_INTERFACE container
        INTERFACE api_common
        THIRD_PARTY cglm_headers)
setup_reflected_preprocessing (REFLECTION_GLOB "*.h")
