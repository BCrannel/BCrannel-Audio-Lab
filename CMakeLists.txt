cmake_minimum_required(VERSION 3.22)
project(BCrannelAudioLab VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.6
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

function(add_bcrannel_plugin target plugin_code)
  juce_add_plugin(BCrannel${target}
    COMPANY_NAME "BCrannel"
    BUNDLE_ID "com.bcrannel.audiolab.${target}"
    PLUGIN_MANUFACTURER_CODE BCrn
    PLUGIN_CODE ${plugin_code}
    FORMATS VST3
    PRODUCT_NAME "BCrannel ${target}"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
  )
  target_sources(BCrannel${target} PRIVATE
    Source/PluginProcessor.cpp
  )
  target_compile_definitions(BCrannel${target} PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
  )
  target_link_libraries(BCrannel${target} PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce_recommended_config_flags
    juce_recommended_lto_flags
    juce_recommended_warning_flags
  )
endfunction()

add_bcrannel_plugin(VoiceProcessor BcVP)
add_bcrannel_plugin(Harmony      BcHM)
add_bcrannel_plugin(Tune         BcTN)
add_bcrannel_plugin(Space        BcSP)
add_bcrannel_plugin(Master       BcMS)
add_bcrannel_plugin(Effector     BcEF)
