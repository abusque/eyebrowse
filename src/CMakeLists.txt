#find_library(MGL mgl PATHS /usr/local/lib)

ADD_EXECUTABLE( eyebrowse main.cpp findEyeCenter.cpp findEyeCenter.h findEyeCorner.cpp findEyeCorner.h constants.h helpers.cpp helpers.h uiActions.h -l xdo)
TARGET_LINK_LIBRARIES( eyebrowse ${OpenCV_LIBS} )
#target_link_libraries( eyebrowse ${MGL})