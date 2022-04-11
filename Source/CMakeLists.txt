#add_subdirectory(headless)

target_sources(TS-M1N3 PRIVATE
	PluginProcessor.cpp
	PluginProcessor.h
	RTNeuralLSTM.cpp
	RTNeuralLSTM.h
)

#target_precompile_headers(TS-M1N3 PRIVATE pch.h)
