# --- cmake -------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.3)
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif()


# --- INCLUDES ----------------------------------------------------------------------------
find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

if( VTK_VERSION_MAJOR EQUAL 6 )
  set(Slicer_VTK_COMPONENTS
    vtkCommonColor
    vtkCommonComputationalGeometry
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkCommonMath
    vtkCommonMisc
    vtkCommonSystem
    vtkCommonTransforms
    vtkFiltersCore
    vtkFiltersExtraction
    vtkFiltersGeneral
    vtkFiltersHybrid
    vtkFiltersImaging
    vtkFiltersModeling
    vtkFiltersSources
    vtkFiltersStatistics
    vtkGUISupportQt
    vtkIOCore
    vtkIOGeometry
    vtkIOImage
    vtkIOInfovis
    vtkIOXML
    vtkIOXMLParser
    vtkImagingColor
    vtkImagingCore
    vtkImagingFourier
    vtkImagingGeneral
    vtkImagingHybrid
    vtkImagingSources
    vtkInfovisCore
    vtkInfovisLayout
    vtkInteractionStyle
    vtkInteractionWidgets
    vtkRenderingAnnotation
    vtkRenderingCore
    vtkRenderingVolumeOpenGL
    vtkRenderingLabel
    vtkRenderingOpenGL
    vtkRenderingVolume
    vtkRenderingFreeTypeOpenGL
    vtkRenderingFreeType
    vtkViewsCore
    vtkViewsInfovis
    vtkViewsQt
    vtkalglib
    vtkfreetype
    vtkzlib
    vtkftgl
    vtkChartsCore 
  )
  if( NOT ShapePopulationViewer_BUILD_SLICER_EXTENSION )
    list( APPEND Slicer_VTK_COMPONENTS vtkIOExport )
  endif()
endif()


find_package(VTK REQUIRED COMPONENTS ${Slicer_VTK_COMPONENTS} )
include(${VTK_USE_FILE})
if( ${LOCAL_PROJECT_NAME}_BUILD_SLICER_EXTENSION )
    find_package(GenerateCLP REQUIRED)
    include(${GenerateCLP_USE_FILE})
endif()

# --- SRCS --------------------------------------------------------------------------------
file(GLOB CXX_FILES *.cxx)
file(GLOB UI_FILES *.ui)
file(GLOB QT_WRAP *QT.h)
file(GLOB QT_QRC_FILES *.qrc)
QT4_WRAP_UI(UISrcs ${UI_FILES})
QT4_WRAP_CPP(MOCSrcs ${QT_WRAP})
QT4_ADD_RESOURCES(QRCrcs ${QT_QRC_FILES})

# --- LIBRARIES ---------------------------------------------------------------------------
if(VTK_LIBRARIES)
  if(${VTK_VERSION} VERSION_LESS "6")
    set(ShapePopulationLIBS ${VTK_LIBRARIES} QVTK)
  else()
    set(ShapePopulationLIBS ${VTK_LIBRARIES})
  endif()
else()
  set(ShapePopulationLIBS vtkHybrid QVTK vtkViews ${QT_LIBRARIES})
endif()

# --- SEMMacro ----------------------------------------------------------------------------

if( NOT ${LOCAL_PROJECT_NAME}_BUILD_SLICER_EXTENSION )
    set(INSTALL_RUNTIME_DEST bin)
else()
    set(INSTALL_RUNTIME_DEST ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION}/../hidden-cli-modules)

    generateCLP(SPV_SRCS_CLP ShapePopulationViewer.xml )
    add_executable( Launcher Launcher/Launcher.cxx ${SPV_SRCS_CLP} )
    install(TARGETS Launcher DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION} )
endif()

SEMMacroBuildCLI(
    NAME ${LOCAL_PROJECT_NAME}
    EXECUTABLE_ONLY
    ADDITIONAL_SRCS ${CXX_FILES} ${UISrcs} ${MOCSrcs} ${QRCrcs}
    TARGET_LIBRARIES ${ShapePopulationLIBS}
    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    INSTALL_RUNTIME_DESTINATION ${INSTALL_RUNTIME_DEST}
    INSTALL_LIBRARY_DESTINATION lib
    INSTALL_ARCHIVE_DESTINATION lib
)


# --- CTest -------------------------------------------------------------------------------
set(BUILD_TESTING ON CACHE BOOL "Build, configure and copy testing files")

if( BUILD_TESTING )
  include(CTest)
#    add_subdirectory( Testing )
endif()


