set(APP_NAME world_radio)

set(PRIVATE_SRC
    main.cpp
    ui_world_radio.cpp
)

add_mayhem_external_app(${APP_NAME} "${PRIVATE_SRC}")
