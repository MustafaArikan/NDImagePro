cmake_minimum_required(VERSION 2.8.11)

PROJECT(imageProcess)

add_definitions( -DBOOST_ALL_NO_LIB )
set( Boost_USE_STATIC_LIBS ON )

SET(QT_USE_QTSQL TRUE)

FIND_PACKAGE (Qt5 COMPONENTS Widgets Xml Network Test OpenGL PrintSupport REQUIRED)

INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR})
ADD_DEFINITIONS (${QT_DEFINITIONS})

FIND_PACKAGE(OpenCV REQUIRED )
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})

FIND_PACKAGE(ITK)
INCLUDE(${ITK_USE_FILE})

FIND_PACKAGE(VTK)
INCLUDE(${VTK_USE_FILE})

SET (imageProcess_HEADERS  
geometryTransform.h
itkFilter.h
morphology.h
pointOperation.h
visualization.h
registration.h
orthogonalTransform.h
edge_contour.h
segmentation.h
vtkCustomCommands.h
dicomProcess.h
mainwindow.h 
mainwindowInterator.h 
)

SET(imageProcess_RESOURCES imageprocess.qrc)

SET (imageProcess_UIS 
imageProcess.ui 
edge_contour.ui 
itkFilter.ui 
morphology.ui
orthogonalTransform.ui
pointOperation.ui
visualization.ui
registration.ui
segmentation.ui
geometryTransform.ui
dicomProcess.ui
)

SET (imageProcess_SRCS  

geometryTransform.h
itkFilter.h
morphology.h
pointOperation.h
visualization.h
registration.h
orthogonalTransform.h
edge_contour.h
segmentation.h
vtkCustomCommands.h
dicomProcess.h
mainwindow.h 
mainwindowInterator.h 

geometryTransform.cpp
itkFilter.cpp
morphology.cpp
pointOperation.cpp
visualization.cpp
registration.cpp
orthogonalTransform.cpp
edge_contour.cpp
segmentation.cpp
vtkCustomCommands.cpp
dicomProcess.cpp
main.cpp 
mainwindow.cpp     
mainwindowInterator.cpp

imageprocess.qrc                  
imageProcess.rc
)

QT5_WRAP_CPP      (imageProcess_HEADERS_MOC ${imageProcess_HEADERS})

QT5_ADD_RESOURCES (imageProcess_RESOURCES_RCC ${imageProcess_RESOURCES})

QT5_WRAP_UI       (imageProcess_UIS_UIC     ${imageProcess_UIS})

FIND_PACKAGE( Boost COMPONENTS program_options filesystem system REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )


ADD_EXECUTABLE (imageProcess ${imageProcess_SRCS}
                            
                            ${imageProcess_HEADERS_MOC}

                            ${imageProcess_RESOURCES_RCC}

                            ${imageProcess_UIS_UIC})


TARGET_LINK_LIBRARIES (imageProcess ${QT_LIBRARIES})

TARGET_LINK_LIBRARIES(imageProcess ${OpenCV_LIBS})

TARGET_LINK_LIBRARIES(imageProcess ${ITK_LIBRARIES})

TARGET_LINK_LIBRARIES(imageProcess ${VTK_LIBRARIES})

TARGET_LINK_LIBRARIES(imageProcess ${Boost_LIBRARIES})
