#find_library(MGL mgl PATHS /usr/local/lib)

ADD_EXECUTABLE( eyebrowse main.cpp findEyeCenter.cpp findEyeCenter.h eyeTracker.cpp uiActions.cpp constants.h helpers.cpp helpers.h eyeTracker.h uiActions.h )
TARGET_LINK_LIBRARIES( eyebrowse ${OpenCV_LIBS} )
TARGET_LINK_LIBRARIES( eyebrowse xdo )
TARGET_LINK_LIBRARIES( eyebrowse config++ )
#target_link_libraries( eyebrowse ${MGL})