if(APPLE)
    set(opentrack-hier-pfx ".")
    set(opentrack-hier-path "/")
    set(opentrack-hier-doc "/")
    set(opentrack-hier-str RUNTIME DESTINATION . LIBRARY DESTINATION .)
    set(opentrack-doc-pfx ".")
elseif(WIN32)
    set(opentrack-hier-pfx "./modules")
    set(opentrack-hier-path "/modules/")
    set(opentrack-hier-doc "/doc/")
    set(opentrack-doc-pfx "./doc")
    set(opentrack-hier-str RUNTIME DESTINATION ./modules/ LIBRARY DESTINATION ./modules/)
else()
    set(opentrack-hier-pfx "libexec/opentrack")
    set(opentrack-hier-path "/../libexec/opentrack/")
    set(opentrack-hier-doc "/share/doc/opentrack/")
    set(opentrack-doc-pfx "./share/doc/opentrack")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${opentrack-hier-pfx}")
    set(opentrack-hier-str ARCHIVE DESTINATION lib/opentrack LIBRARY DESTINATION ${opentrack-hier-pfx} RUNTIME DESTINATION bin)
endif()

set(opentrack-contrib-pfx "${opentrack-doc-pfx}/contrib")

set(opentrack-binary-suffix "")
if(APPLE)
    set(opentrack-binary-suffix ".bin")
elseif(WIN32)
    set(opentrack-binary-suffix ".exe")
endif()
