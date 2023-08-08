#-------------------------------------------------
#
# Project created by QtCreator 2012-08-22T14:08:28
#
#-------------------------------------------------

QT       += core gui

TARGET = freeadjust
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    CJob.cpp \
    cangleunits.cpp \
    cunits.cpp \
    convert.cpp \
    cxmlparser.cpp \
    adj_imp.cpp \
    job_load_gkf.cpp \
    gnu_gama/version.cpp \
    gnu_gama/utf8.cpp \
    gnu_gama/statan.cpp \
    gnu_gama/rand.cpp \
    gnu_gama/outstream.cpp \
    gnu_gama/latlong.cpp \
    gnu_gama/gon2deg.cpp \
    gnu_gama/ellipsoids.cpp \
    gnu_gama/ellipsoid.cpp \
    gnu_gama/e3.cpp \
    gnu_gama/adj/adj.cpp \
    gnu_gama/g3/g3_write_observation_xml.cpp \
    gnu_gama/g3/g3_point.cpp \
    gnu_gama/g3/g3_model_write_xml_adjustment_results.cpp \
    gnu_gama/g3/g3_model_revision.cpp \
    gnu_gama/g3/g3_model_linearization.cpp \
    gnu_gama/g3/g3_model_init.cpp \
    gnu_gama/g3/g3_model.cpp \
    gnu_gama/g3/g3_cluster.cpp \
    gnu_gama/g3/g3_adjres.cpp \
    gnu_gama/local/sqlitereader.cpp \
    gnu_gama/local/skipcomm.cpp \
    gnu_gama/local/pointid.cpp \
    gnu_gama/local/orientation.cpp \
    gnu_gama/local/observation_local.cpp \
    gnu_gama/local/observation.cpp \
    gnu_gama/local/newnetwork.cpp \
    gnu_gama/local/network.cpp \
    gnu_gama/local/language.cpp \
    gnu_gama/local/gkf2sql.cpp \
    gnu_gama/local/acord.cpp \
    gnu_gama/local/acord/reduce_to_ellipsoid.cpp \
    gnu_gama/local/acord/reduce_observations.cpp \
    gnu_gama/local/acord/approx_vectors.cpp \
    gnu_gama/local/acord/approx_heights.cpp \
    gnu_gama/local/c/api/capi_output_file.cpp \
    gnu_gama/local/c/api/capi_locnet.cpp \
    gnu_gama/local/c/api/capi_gkfparser.cpp \
    gnu_gama/local/c/api/capi_exception.cpp \
    gnu_gama/local/median/g2d_point.cpp \
    gnu_gama/local/median/g2d_helper.cpp \
    gnu_gama/local/median/g2d_coordinates.cpp \
    gnu_gama/local/median/g2d_cogo.cpp \
    gnu_gama/local/observation/write/xyzdiff.cpp \
    gnu_gama/local/observation/write/xyz.cpp \
    gnu_gama/local/observation/write/sdistance.cpp \
    gnu_gama/local/observation/write/hdiff.cpp \
    gnu_gama/local/observation/write/distance.cpp \
    gnu_gama/local/observation/write/direction.cpp \
    gnu_gama/local/observation/write/angle.cpp \
    gnu_gama/local/observation/write/zangle.cpp \
    gnu_gama/local/pobs/format.cpp \
    gnu_gama/local/pobs/write.cpp \
    gnu_gama/local/results/text/underline.cpp \
    gnu_gama/xml/localnetwork_adjustment_results.cpp \
    gnu_gama/xml/localnetwork.cpp \
    gnu_gama/xml/gkfparser.cpp \
    gnu_gama/xml/encoding_unknown_handler.cpp \
    gnu_gama/xml/encoding_cp1251.cpp \
    gnu_gama/xml/encoding.cpp \
    gnu_gama/xml/dataparser_g3adj.cpp \
    gnu_gama/xml/dataparser_g3.cpp \
    gnu_gama/xml/dataparser_adj.cpp \
    gnu_gama/xml/dataparser.cpp \
    gnu_gama/xml/dataobject.cpp \
    gnu_gama/xml/baseparser.cpp \
    expat/xmlparse/xmlparse.c \
    expat/xmlparse/hashtable.c \
    expat/xmltok/xmltok_impl.c \
    expat/xmltok/xmltok.c \
    expat/xmltok/xmlrole.c \
    expat/xmltok/dllmain.c \
    expat/xmltok/xmltok_ns.c \
    expat/xmlwf/xmlfile.c \
    expat/xmlwf/win32filemap.c \
    expat/xmlwf/codepage.c \
    CJob_gamma_local.cpp \
    C2DView.cpp \
    c3dpoint.cpp \
    CJob_adjust.cpp \
    ExportTxt.cpp \
    CJob_ts_import.cpp \
    winnt.cpp \
    serial/qserialdevice/nativeserialengine_win.cpp \
    serial/qserialdevice/nativeserialengine.cpp \
    serial/qserialdevice/abstractserialnotifier.cpp \
    serial/qserialdevice/abstractserialengine.cpp \
    serial/qserialdevice/abstractserial.cpp \
    serial/qserialdevice/nativeserialnotifier_win.cpp \
    serial/qserialdeviceenumerator/serialdeviceenumerator.cpp \
    serial/qserialdeviceenumerator/serialdeviceenumerator_p_win.cpp \
    cserialdeviceimportdlg.cpp \
    cbasepointselect.cpp \
    cndarray.cpp \
    c2dprintview.cpp \
    c2dpageview.cpp \
    clipLine2D.cpp

HEADERS  += mainwindow.h \
    cangleunits.h \
    cunits.h \
    convert.h \
    CJob.h \
    cxmlparser.h \
    gnu_gama/visitor.h \
    gnu_gama/version.h \
    gnu_gama/utf8.h \
    gnu_gama/statan.h \
    gnu_gama/rand.h \
    gnu_gama/radian.h \
    gnu_gama/pointbase.h \
    gnu_gama/outstream.h \
    gnu_gama/obsdata.h \
    gnu_gama/movetofront.h \
    gnu_gama/model.h \
    gnu_gama/list.h \
    gnu_gama/latlong.h \
    gnu_gama/intfloat.h \
    gnu_gama/gon2deg.h \
    gnu_gama/exception.h \
    gnu_gama/ellipsoids.h \
    gnu_gama/ellipsoid.h \
    gnu_gama/e3.h \
    gnu_gama/xml_expat.h \
    gnu_gama/adj/envelope.h \
    gnu_gama/adj/adj_svd.h \
    gnu_gama/adj/adj_gso.h \
    gnu_gama/adj/adj_envelope_implementation.h \
    gnu_gama/adj/adj_envelope.h \
    gnu_gama/adj/adj_chol_implementation.h \
    gnu_gama/adj/adj_chol.h \
    gnu_gama/adj/adj_basesparse.h \
    gnu_gama/adj/adj_basefull.h \
    gnu_gama/adj/adj_base.h \
    gnu_gama/adj/adj.h \
    gnu_gama/adj/homogenization.h \
    gnu_gama/g3/g3_point.h \
    gnu_gama/g3/g3_parameter.h \
    gnu_gama/g3/g3_observation.h \
    gnu_gama/g3/g3_model.h \
    gnu_gama/g3/g3_cluster.h \
    gnu_gama/g3/g3_adjres.h \
    gnu_gama/g3/g3_write_observation_xml.h \
    gnu_gama/local/skipcomm.h \
    gnu_gama/local/revision.h \
    gnu_gama/local/readsabw.h \
    gnu_gama/local/pointid.h \
    gnu_gama/local/orientation.h \
    gnu_gama/local/observation.h \
    gnu_gama/local/newnetwork.h \
    gnu_gama/local/network_svd.h \
    gnu_gama/local/network_gso.h \
    gnu_gama/local/network_env.h \
    gnu_gama/local/network_chol.h \
    gnu_gama/local/network.h \
    gnu_gama/local/medianf.h \
    gnu_gama/local/matvec.h \
    gnu_gama/local/lpoint.h \
    gnu_gama/local/local_revision.h \
    gnu_gama/local/local_linearization.h \
    gnu_gama/local/linearization.h \
    gnu_gama/local/lcoords.h \
    gnu_gama/local/language.h \
    gnu_gama/local/itstream.h \
    gnu_gama/local/gkf2sql.h \
    gnu_gama/local/gamadata.h \
    gnu_gama/local/float.h \
    gnu_gama/local/exception.h \
    gnu_gama/local/cluster.h \
    gnu_gama/local/c_api.h \
    gnu_gama/local/bpoint.h \
    gnu_gama/local/angobs.h \
    gnu_gama/local/acord.h \
    gnu_gama/local/sqlitereader.h \
    gnu_gama/local/acord/reduce_observations.h \
    gnu_gama/local/acord/approx_vectors.h \
    gnu_gama/local/acord/approx_heights.h \
    gnu_gama/local/acord/reduce_to_ellipsoid.h \
    gnu_gama/local/c/api/capi_output_file.h \
    gnu_gama/local/c/api/capi_locnet.h \
    gnu_gama/local/c/api/capi_gkfparser.h \
    gnu_gama/local/c/api/capi_exception.h \
    gnu_gama/local/c/api/capi_private_exception.h \
    gnu_gama/local/median/g2d_helper.h \
    gnu_gama/local/median/g2d_exception.h \
    gnu_gama/local/median/g2d_coordinates.h \
    gnu_gama/local/median/g2d_cogo.h \
    gnu_gama/local/median/g2d_point.h \
    gnu_gama/local/observation/write/writevisitor.h \
    gnu_gama/local/pobs/format.h \
    gnu_gama/local/pobs/bearing.h \
    gnu_gama/local/results/text/test_linearization.h \
    gnu_gama/local/results/text/residuals_observations.h \
    gnu_gama/local/results/text/reduced_observations_to_ellipsoid.h \
    gnu_gama/local/results/text/reduced_observations.h \
    gnu_gama/local/results/text/outlying_abs_terms.h \
    gnu_gama/local/results/text/network_description.h \
    gnu_gama/local/results/text/general_parameters.h \
    gnu_gama/local/results/text/fixed_points.h \
    gnu_gama/local/results/text/error_ellipses.h \
    gnu_gama/local/results/text/approximate_coordinates.h \
    gnu_gama/local/results/text/adjusted_unknowns.h \
    gnu_gama/local/results/text/adjusted_observations.h \
    gnu_gama/local/results/text/underline.h \
    gnu_gama/sparse/smatrix_ordering.h \
    gnu_gama/sparse/smatrix_graph_connected.h \
    gnu_gama/sparse/smatrix_graph.h \
    gnu_gama/sparse/smatrix.h \
    gnu_gama/sparse/sbdiagonal.h \
    gnu_gama/sparse/intlist.h \
    gnu_gama/sparse/svector.h \
    gnu_gama/xml/localnetwork.h \
    gnu_gama/xml/gkfparser.h \
    gnu_gama/xml/encoding.h \
    gnu_gama/xml/dataparser.h \
    gnu_gama/xml/dataobject.h \
    gnu_gama/xml/baseparser.h \
    gnu_gama/xml/localnetwork_adjustment_results.h \
    expat/xmlparse/hashtable.h \
    expat/xmlparse/xmlparse.h \
    expat/xmltok/xmltok_impl.h \
    expat/xmltok/xmltok.h \
    expat/xmltok/xmlrole.h \
    expat/xmltok/xmldef.h \
    expat/xmltok/utf8tab.h \
    expat/xmltok/nametab.h \
    expat/xmltok/latin1tab.h \
    expat/xmltok/iasciitab.h \
    expat/xmltok/asciitab.h \
    expat/xmlwf/xmltchar.h \
    expat/xmlwf/xmlfile.h \
    expat/xmlwf/filemap.h \
    expat/xmlwf/codepage.h \
    c3dpoint.h \
    CStationTI.h \
    CNodeTI.h \
    CObservTI.h \
    serial/qserialdevice/nativeserialnotifier.h \
    serial/qserialdevice/nativeserialengine_p.h \
    serial/qserialdevice/nativeserialengine.h \
    serial/qserialdevice/abstractserialnotifier.h \
    serial/qserialdevice/abstractserialengine_p.h \
    serial/qserialdevice/abstractserialengine.h \
    serial/qserialdevice/abstractserial_p.h \
    serial/qserialdevice/abstractserial.h \
    serial/qserialdeviceenumerator/serialdeviceenumerator_p.h \
    serial/qserialdeviceenumerator/serialdeviceenumerator.h \
    cserialdeviceimportdlg.h \
    cbasepointselect.h \
    cndarray.h \
    chcarray.h \
    chclass.h \
    c2dprintview.h \
    CViewportTI.h \
    CViewportPropertyTI.h \
    c2dpageview.h \
    c2dviewporttranslator.h \
    clipLine2D.h

FORMS    += mainwindow.ui \
    cserialdeviceimportdlg.ui \
    cbasepointselect.ui

win32 {
    LIBS += -lsetupapi -luuid -ladvapi32
}

TRANSLATIONS = fadjust_ru_RU.ts

RESOURCES += \
    fadjust.qrc

RC_FILE = images\icons\myicon.rc

















































