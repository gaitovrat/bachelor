add_library(Shared STATIC 
    Src/Image.cpp
    Src/PIDData.cpp
    Src/Region.cpp
    Src/Settings.cpp
    Include/Shared/CameraData.h
    Include/Shared/Utils.h
    Include/Shared/Data.h
    Include/Shared/Image.h
    Include/Shared/Mode.h
    Include/Shared/MotorData.h
    Include/Shared/MotorState.h
    Include/Shared/PIDData.h
    Include/Shared/Region.h
    Include/Shared/SensorData.h
    Include/Shared/Settings.h
    Include/Shared/SteerData.h
    Include/Shared/Vec.h)

target_include_directories(Shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
