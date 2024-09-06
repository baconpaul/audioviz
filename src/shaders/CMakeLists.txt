project(audioviz-shaders)

cmrc_add_resource_library(${PROJECT_NAME}
        NAMESPACE audioviz_shaders
        valuenoise.frag
        )
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD FALSE)
