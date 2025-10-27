QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += opengl

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

msvc {
    #使用/utf-8选项（vs 2015 update 2及以后版本支持）
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

SOURCES += \
    1027-main.cpp \
    AnalysisEngine.cpp \
    JsonHandler.cpp \
    inputdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    pclvisualizer.cpp \
    DataFetcher.cpp \
    reportgenerator.cpp \
    taskdockpanel.cpp

HEADERS += \
    AnalysisEngine.h \
    JsonHandler.h \
    inputdialog.h \
    mainwindow.h \
    pclvisualizer.h \
    DataFetcher.h \
    reportgenerator.h \
    taskdockpanel.h


FORMS += \
    inputdialog.ui \
    mainwindow.ui \
    pclvisualizer.ui \
    taskdockpanel.ui

RESOURCES += \
    images.qrc \
    json.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#QMAKE_LFLAGS += /MAP

# 基础路径定义()
PCL_ROOT = "D:\Program Files (x86)\PCL 1.9.1"
QT_ROOT = "D:\Qt\5.14.1\msvc2017_64"

# 包含目录
INCLUDEPATH += $$PCL_ROOT/include/pcl-1.9 \
               $$PCL_ROOT/include/pcl-1.9/pcl \
               $$PCL_ROOT/3rdParty/Boost/include/boost-1_68 \
               $$PCL_ROOT/3rdParty/Boost/include/boost-1_68/boost \
               $$PCL_ROOT/3rdParty/Eigen/eigen3 \
               $$PCL_ROOT/3rdParty/FLANN/include \
               $$PCL_ROOT/3rdParty/FLANN/include/flann \
               $$PCL_ROOT/3rdParty/OpenNI2/Include \
               $$PCL_ROOT/3rdParty/Qhull/include \
               $$PCL_ROOT/3rdParty/VTK2/include/vtk-8.1\
               $$PCL_ROOT/3rdParty/VTK/include/vtk-8.1


# 库目录
LIBS += -L$$PCL_ROOT/lib \
        -L$$PCL_ROOT/3rdParty/Boost/lib \
        -L$$PCL_ROOT/3rdParty/FLANN/lib \
        -L$$PCL_ROOT/3rdParty/OpenNI2/Lib \
        -L$$PCL_ROOT/3rdParty/Qhull/lib \
        -L$$PCL_ROOT/3rdParty/VTK2/lib \
        -L$$QT_ROOT/lib

CONFIG(debug, debug|release) {
    # Debug版本库
    LIBS += -lpcl_common_debug \
            -lpcl_features_debug \
            -lpcl_filters_debug \
            -lpcl_io_debug \
            -lpcl_io_ply_debug \
            -lpcl_kdtree_debug \
            -lpcl_keypoints_debug \
            -lpcl_ml_debug \
            -lpcl_octree_debug \
            -lpcl_outofcore_debug \
            -lpcl_people_debug \
            -lpcl_recognition_debug \
            -lpcl_registration_debug \
            -lpcl_sample_consensus_debug \
            -lpcl_search_debug \
            -lpcl_segmentation_debug \
            -lpcl_stereo_debug \
            -lpcl_surface_debug \
            -lpcl_tracking_debug \
            -lpcl_visualization_debug

    # Boost Debug库
    LIBS += -llibboost_atomic-vc141-mt-gd-x64-1_68\
        -llibboost_bzip2-vc141-mt-gd-x64-1_68\
        -llibboost_chrono-vc141-mt-gd-x64-1_68\
        -llibboost_container-vc141-mt-gd-x64-1_68\
        -llibboost_context-vc141-mt-gd-x64-1_68\
        -llibboost_coroutine-vc141-mt-gd-x64-1_68\
        -llibboost_date_time-vc141-mt-gd-x64-1_68\
        -llibboost_exception-vc141-mt-gd-x64-1_68\
        -llibboost_fiber-vc141-mt-gd-x64-1_68\
        -llibboost_filesystem-vc141-mt-gd-x64-1_68\
        -llibboost_graph-vc141-mt-gd-x64-1_68\
        -llibboost_graph_parallel-vc141-mt-gd-x64-1_68\
        -llibboost_iostreams-vc141-mt-gd-x64-1_68\
        -llibboost_locale-vc141-mt-gd-x64-1_68\
        -llibboost_log-vc141-mt-gd-x64-1_68\
        -llibboost_log_setup-vc141-mt-gd-x64-1_68\
        -llibboost_math_c99-vc141-mt-gd-x64-1_68\
        -llibboost_math_c99f-vc141-mt-gd-x64-1_68\
        -llibboost_math_c99l-vc141-mt-gd-x64-1_68\
        -llibboost_math_tr1-vc141-mt-gd-x64-1_68\
        -llibboost_math_tr1f-vc141-mt-gd-x64-1_68\
        -llibboost_math_tr1l-vc141-mt-gd-x64-1_68\
        -llibboost_mpi-vc141-mt-gd-x64-1_68\
        -llibboost_numpy27-vc141-mt-gd-x64-1_68\
        -llibboost_numpy37-vc141-mt-gd-x64-1_68\
        -llibboost_prg_exec_monitor-vc141-mt-gd-x64-1_68\
        -llibboost_program_options-vc141-mt-gd-x64-1_68\
        -llibboost_python27-vc141-mt-gd-x64-1_68\
        -llibboost_python37-vc141-mt-gd-x64-1_68\
        -llibboost_random-vc141-mt-gd-x64-1_68\
        -llibboost_regex-vc141-mt-gd-x64-1_68\
        -llibboost_serialization-vc141-mt-gd-x64-1_68\
        -llibboost_signals-vc141-mt-gd-x64-1_68\
        -llibboost_system-vc141-mt-gd-x64-1_68\
        -llibboost_test_exec_monitor-vc141-mt-gd-x64-1_68\
        -llibboost_thread-vc141-mt-gd-x64-1_68\
        -llibboost_timer-vc141-mt-gd-x64-1_68\
        -llibboost_type_erasure-vc141-mt-gd-x64-1_68\
        -llibboost_unit_test_framework-vc141-mt-gd-x64-1_68\
        -llibboost_wave-vc141-mt-gd-x64-1_68\
        -llibboost_wserialization-vc141-mt-gd-x64-1_68\
        -llibboost_zlib-vc141-mt-gd-x64-1_68

    # 其他Debug库
    LIBS += -lflann-gd \
            -lflann_cpp-gd \
            -lflann_cpp_s-gd \
            -lflann_s-gd \
            -lOpenNI2 \
            -lqhull_d \
            -lqhullcpp_d \
            -lqhullstatic_d \
            -lqhullstatic_r_d \
            -lqhull_p_d \
            -lqhull_r_d \
            -lQt5OpenGL

    # VTK Debug库
    LIBS += -lvtkalglib-8.1_d \
            -lvtkChartsCore-8.1_d \
            -lvtkCommonColor-8.1_d \
            -lvtkCommonComputationalGeometry-8.1_d \
            -lvtkCommonCore-8.1_d \
            -lvtkCommonDataModel-8.1_d \
            -lvtkCommonExecutionModel-8.1_d \
            -lvtkCommonMath-8.1_d \
            -lvtkCommonMisc-8.1_d \
            -lvtkCommonSystem-8.1_d \
            -lvtkCommonTransforms-8.1_d \
            -lvtkDICOMParser-8.1_d \
            -lvtkDomainsChemistry-8.1_d \
            -lvtkDomainsChemistryOpenGL2-8.1_d \
            -lvtkexoIIc-8.1_d \
            -lvtkexpat-8.1_d \
            -lvtkFiltersAMR-8.1_d \
            -lvtkFiltersCore-8.1_d \
            -lvtkFiltersExtraction-8.1_d \
            -lvtkFiltersFlowPaths-8.1_d \
            -lvtkFiltersGeneral-8.1_d \
            -lvtkFiltersGeneric-8.1_d \
            -lvtkFiltersGeometry-8.1_d \
            -lvtkFiltersHybrid-8.1_d \
            -lvtkFiltersHyperTree-8.1_d \
            -lvtkFiltersImaging-8.1_d \
            -lvtkFiltersModeling-8.1_d \
            -lvtkFiltersParallel-8.1_d \
            -lvtkFiltersParallelImaging-8.1_d \
            -lvtkFiltersPoints-8.1_d \
            -lvtkFiltersProgrammable-8.1_d \
            -lvtkFiltersSelection-8.1_d \
            -lvtkFiltersSMP-8.1_d \
            -lvtkFiltersSources-8.1_d \
            -lvtkFiltersStatistics-8.1_d \
            -lvtkFiltersTexture-8.1_d \
            -lvtkFiltersTopology-8.1_d \
            -lvtkFiltersVerdict-8.1_d \
            -lvtkfreetype-8.1_d \
            -lvtkGeovisCore-8.1_d \
            -lvtkgl2ps-8.1_d \
            -lvtkglew-8.1_d \
            -lvtkGUISupportQt-8.1_d \
            -lvtkGUISupportQtSQL-8.1_d \
            -lvtkhdf5-8.1_d \
            -lvtkhdf5_hl-8.1_d \
            -lvtkImagingColor-8.1_d \
            -lvtkImagingCore-8.1_d \
            -lvtkImagingFourier-8.1_d \
            -lvtkImagingGeneral-8.1_d \
            -lvtkImagingHybrid-8.1_d \
            -lvtkImagingMath-8.1_d \
            -lvtkImagingMorphological-8.1_d \
            -lvtkImagingSources-8.1_d \
            -lvtkImagingStatistics-8.1_d \
            -lvtkImagingStencil-8.1_d \
            -lvtkInfovisCore-8.1_d \
            -lvtkInfovisLayout-8.1_d \
            -lvtkInteractionImage-8.1_d \
            -lvtkInteractionStyle-8.1_d \
            -lvtkInteractionWidgets-8.1_d \
            -lvtkIOAMR-8.1_d \
            -lvtkIOCore-8.1_d \
            -lvtkIOEnSight-8.1_d \
            -lvtkIOExodus-8.1_d \
            -lvtkIOExport-8.1_d \
            -lvtkIOExportOpenGL2-8.1_d \
            -lvtkIOGeometry-8.1_d \
            -lvtkIOImage-8.1_d \
            -lvtkIOImport-8.1_d \
            -lvtkIOInfovis-8.1_d \
            -lvtkIOLegacy-8.1_d \
            -lvtkIOLSDyna-8.1_d \
            -lvtkIOMINC-8.1_d \
            -lvtkIOMovie-8.1_d \
            -lvtkIONetCDF-8.1_d \
            -lvtkIOParallel-8.1_d \
            -lvtkIOParallelXML-8.1_d \
            -lvtkIOPLY-8.1_d \
            -lvtkIOSQL-8.1_d \
            -lvtkIOTecplotTable-8.1_d \
            -lvtkIOVideo-8.1_d \
            -lvtkIOXML-8.1_d \
            -lvtkIOXMLParser-8.1_d \
            -lvtkjpeg-8.1_d \
            -lvtkjsoncpp-8.1_d \
            -lvtklibharu-8.1_d \
            -lvtklibxml2-8.1_d \
            -lvtklz4-8.1_d \
            -lvtkmetaio-8.1_d \
            -lvtkNetCDF-8.1_d \
            -lvtknetcdfcpp-8.1_d \
            -lvtkoggtheora-8.1_d \
            -lvtkParallelCore-8.1_d \
            -lvtkpng-8.1_d \
            -lvtkproj4-8.1_d \
            -lvtkRenderingAnnotation-8.1_d \
            -lvtkRenderingContext2D-8.1_d \
            -lvtkRenderingContextOpenGL2-8.1_d \
            -lvtkRenderingCore-8.1_d \
            -lvtkRenderingFreeType-8.1_d \
            -lvtkRenderingGL2PSOpenGL2-8.1_d \
            -lvtkRenderingImage-8.1_d \
            -lvtkRenderingLabel-8.1_d \
            -lvtkRenderingLOD-8.1_d \
            -lvtkRenderingOpenGL2-8.1_d \
            -lvtkRenderingQt-8.1_d \
            -lvtkRenderingVolume-8.1_d \
            -lvtkRenderingVolumeOpenGL2-8.1_d \
            -lvtksqlite-8.1_d \
            -lvtksys-8.1_d \
            -lvtktiff-8.1_d \
            -lvtkverdict-8.1_d \
            -lvtkViewsContext2D-8.1_d \
            -lvtkViewsCore-8.1_d \
            -lvtkViewsInfovis-8.1_d \
            -lvtkViewsQt-8.1_d \
            -lvtkzlib-8.1_d

} else {
    # Release版本库
    LIBS += -lpcl_common_release \
            -lpcl_features_release \
            -lpcl_filters_release \
            -lpcl_io_ply_release \
            -lpcl_io_release \
            -lpcl_kdtree_release \
            -lpcl_keypoints_release \
            -lpcl_ml_release \
            -lpcl_octree_release \
            -lpcl_outofcore_release \
            -lpcl_people_release \
            -lpcl_recognition_release \
            -lpcl_registration_release \
            -lpcl_sample_consensus_release \
            -lpcl_search_release \
            -lpcl_segmentation_release \
            -lpcl_stereo_release \
            -lpcl_surface_release \
            -lpcl_tracking_release \
            -lpcl_visualization_release

    # Boost Release库

    LIBS += -llibboost_atomic-vc141-mt-x64-1_68 \
            -llibboost_bzip2-vc141-mt-x64-1_68 \
            -llibboost_chrono-vc141-mt-x64-1_68 \
            -llibboost_container-vc141-mt-x64-1_68 \
            -llibboost_context-vc141-mt-x64-1_68 \
            -llibboost_coroutine-vc141-mt-x64-1_68 \
            -llibboost_date_time-vc141-mt-x64-1_68 \
            -llibboost_exception-vc141-mt-x64-1_68 \
            -llibboost_fiber-vc141-mt-x64-1_68 \
            -llibboost_filesystem-vc141-mt-x64-1_68 \
            -llibboost_graph-vc141-mt-x64-1_68 \
            -llibboost_graph_parallel-vc141-mt-x64-1_68 \
            -llibboost_iostreams-vc141-mt-x64-1_68 \
            -llibboost_locale-vc141-mt-x64-1_68 \
            -llibboost_log-vc141-mt-x64-1_68 \
            -llibboost_log_setup-vc141-mt-x64-1_68 \
            -llibboost_math_c99-vc141-mt-x64-1_68 \
            -llibboost_math_c99f-vc141-mt-x64-1_68 \
            -llibboost_math_c99l-vc141-mt-x64-1_68 \
            -llibboost_math_tr1-vc141-mt-x64-1_68 \
            -llibboost_math_tr1f-vc141-mt-x64-1_68 \
            -llibboost_math_tr1l-vc141-mt-x64-1_68 \
            -llibboost_mpi-vc141-mt-x64-1_68 \
            -llibboost_numpy27-vc141-mt-x64-1_68 \
            -llibboost_numpy37-vc141-mt-x64-1_68 \
            -llibboost_prg_exec_monitor-vc141-mt-x64-1_68 \
            -llibboost_program_options-vc141-mt-x64-1_68 \
            -llibboost_python27-vc141-mt-x64-1_68 \
            -llibboost_python37-vc141-mt-x64-1_68 \
            -llibboost_random-vc141-mt-x64-1_68 \
            -llibboost_regex-vc141-mt-x64-1_68 \
            -llibboost_serialization-vc141-mt-x64-1_68 \
            -llibboost_signals-vc141-mt-x64-1_68 \
            -llibboost_system-vc141-mt-x64-1_68 \
            -llibboost_test_exec_monitor-vc141-mt-x64-1_68 \
            -llibboost_thread-vc141-mt-x64-1_68 \
            -llibboost_timer-vc141-mt-x64-1_68 \
            -llibboost_type_erasure-vc141-mt-x64-1_68 \
            -llibboost_unit_test_framework-vc141-mt-x64-1_68 \
            -llibboost_wave-vc141-mt-x64-1_68 \
            -llibboost_wserialization-vc141-mt-x64-1_68 \
            -llibboost_zlib-vc141-mt-x64-1_68

    # 其他Release库
    LIBS += -lflann \
            -lflann_cpp \
            -lflann_cpp_s \
            -lflann_s \
            -lOpenNI2 \
            -lqhull \
            -lqhullcpp \
            -lqhullstatic \
            -lqhullstatic_r \
            -lqhull_p \
            -lqhull_r \
            -lQt5OpenGL

    # VTK Release库
    LIBS += -lvtkalglib-8.1 \
            -lvtkChartsCore-8.1 \
            -lvtkCommonColor-8.1 \
            -lvtkCommonComputationalGeometry-8.1 \
            -lvtkCommonCore-8.1 \
            -lvtkCommonDataModel-8.1 \
            -lvtkCommonExecutionModel-8.1 \
            -lvtkCommonMath-8.1 \
            -lvtkCommonMisc-8.1 \
            -lvtkCommonSystem-8.1 \
            -lvtkCommonTransforms-8.1 \
            -lvtkDICOMParser-8.1 \
            -lvtkDomainsChemistry-8.1 \
            -lvtkDomainsChemistryOpenGL2-8.1 \
            -lvtkexoIIc-8.1 \
            -lvtkexpat-8.1 \
            -lvtkFiltersAMR-8.1 \
            -lvtkFiltersCore-8.1 \
            -lvtkFiltersExtraction-8.1 \
            -lvtkFiltersFlowPaths-8.1 \
            -lvtkFiltersGeneral-8.1 \
            -lvtkFiltersGeneric-8.1 \
            -lvtkFiltersGeometry-8.1 \
            -lvtkFiltersHybrid-8.1 \
            -lvtkFiltersHyperTree-8.1 \
            -lvtkFiltersImaging-8.1 \
            -lvtkFiltersModeling-8.1 \
            -lvtkFiltersParallel-8.1 \
            -lvtkFiltersParallelImaging-8.1 \
            -lvtkFiltersPoints-8.1 \
            -lvtkFiltersProgrammable-8.1 \
            -lvtkFiltersSelection-8.1 \
            -lvtkFiltersSMP-8.1 \
            -lvtkFiltersSources-8.1 \
            -lvtkFiltersStatistics-8.1 \
            -lvtkFiltersTexture-8.1 \
            -lvtkFiltersTopology-8.1 \
            -lvtkFiltersVerdict-8.1 \
            -lvtkfreetype-8.1 \
            -lvtkGeovisCore-8.1 \
            -lvtkgl2ps-8.1 \
            -lvtkglew-8.1 \
            -lvtkGUISupportQt-8.1 \
            -lvtkGUISupportQtSQL-8.1 \
            -lvtkhdf5-8.1 \
            -lvtkhdf5_hl-8.1 \
            -lvtkImagingColor-8.1 \
            -lvtkImagingCore-8.1 \
            -lvtkImagingFourier-8.1 \
            -lvtkImagingGeneral-8.1 \
            -lvtkImagingHybrid-8.1 \
            -lvtkImagingMath-8.1 \
            -lvtkImagingMorphological-8.1 \
            -lvtkImagingSources-8.1 \
            -lvtkImagingStatistics-8.1 \
            -lvtkImagingStencil-8.1 \
            -lvtkInfovisCore-8.1 \
            -lvtkInfovisLayout-8.1 \
            -lvtkInteractionImage-8.1 \
            -lvtkInteractionStyle-8.1 \
            -lvtkInteractionWidgets-8.1 \
            -lvtkIOAMR-8.1 \
            -lvtkIOCore-8.1 \
            -lvtkIOEnSight-8.1 \
            -lvtkIOExodus-8.1 \
            -lvtkIOExport-8.1 \
            -lvtkIOExportOpenGL2-8.1 \
            -lvtkIOGeometry-8.1 \
            -lvtkIOImage-8.1 \
            -lvtkIOImport-8.1 \
            -lvtkIOInfovis-8.1 \
            -lvtkIOLegacy-8.1 \
            -lvtkIOLSDyna-8.1 \
            -lvtkIOMINC-8.1 \
            -lvtkIOMovie-8.1 \
            -lvtkIONetCDF-8.1 \
            -lvtkIOParallel-8.1 \
            -lvtkIOParallelXML-8.1 \
            -lvtkIOPLY-8.1 \
            -lvtkIOSQL-8.1 \
            -lvtkIOTecplotTable-8.1 \
            -lvtkIOVideo-8.1 \
            -lvtkIOXML-8.1 \
            -lvtkIOXMLParser-8.1 \
            -lvtkjpeg-8.1 \
            -lvtkjsoncpp-8.1 \
            -lvtklibharu-8.1 \
            -lvtklibxml2-8.1 \
            -lvtklz4-8.1 \
            -lvtkmetaio-8.1 \
            -lvtkNetCDF-8.1 \
            -lvtknetcdfcpp-8.1 \
            -lvtkoggtheora-8.1 \
            -lvtkParallelCore-8.1 \
            -lvtkpng-8.1 \
            -lvtkproj4-8.1 \
            -lvtkRenderingAnnotation-8.1 \
            -lvtkRenderingContext2D-8.1 \
            -lvtkRenderingContextOpenGL2-8.1 \
            -lvtkRenderingCore-8.1 \
            -lvtkRenderingFreeType-8.1 \
            -lvtkRenderingGL2PSOpenGL2-8.1 \
            -lvtkRenderingImage-8.1 \
            -lvtkRenderingLabel-8.1 \
            -lvtkRenderingLOD-8.1 \
            -lvtkRenderingOpenGL2-8.1 \
            -lvtkRenderingQt-8.1 \
            -lvtkRenderingVolume-8.1 \
            -lvtkRenderingVolumeOpenGL2-8.1 \
            -lvtksqlite-8.1 \
            -lvtksys-8.1 \
            -lvtktiff-8.1 \
            -lvtkverdict-8.1 \
            -lvtkViewsContext2D-8.1 \
            -lvtkViewsCore-8.1 \
            -lvtkViewsInfovis-8.1 \
            -lvtkViewsQt-8.1 \
            -lvtkzlib-8.1
}



# 部署设置（可选）
# 定义源DLL目录和目标目录
DLL_SOURCE_DIR = $$PWD/bin
DLL_TARGET_DIR = $$OUT_PWD/$$CONFIGURATION/debug

# 执行复制操作（Windows）
win32 {
    QMAKE_POST_LINK += $$escape_expand(\\n) xcopy /Y \"$$shell_path($$DLL_SOURCE_DIR)\\*.dll\" \"$$shell_path($$DLL_TARGET_DIR)\"
}

