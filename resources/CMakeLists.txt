juce_add_binary_data(BinaryData SOURCES
    logo.png
    ts_background.jpg
    ../models/TS-M1N3.json
)

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
