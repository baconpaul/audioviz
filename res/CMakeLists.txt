project(audioviz-resources)

cmrc_add_resource_library(${PROJECT_NAME}
        NAMESPACE audioviz_resources
        PirateSessions.png
        BeamGradient.png
        BriansYieArKungFu.png
        pixel_operator/PixelOperator.ttf
        pixel_operator/PixelOperatorMono-Bold.ttf
        AnonymousPro/AnonymousPro-Regular.ttf
        )
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD FALSE)
