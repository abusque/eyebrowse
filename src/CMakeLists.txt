#find_library(MGL mgl PATHS /usr/local/lib)

ADD_EXECUTABLE( eyebrowse main.cpp findEyeCenter.cpp findEyeCenter.h findEyeCorner.cpp uiActions.cpp findEyeCorner.h constants.h helpers.cpp helpers.h uiActions.h )
TARGET_LINK_LIBRARIES( eyebrowse ${OpenCV_LIBS} )
TARGET_LINK_LIBRARIES( eyebrowse xdo )
#target_link_libraries( eyebrowse ${MGL})