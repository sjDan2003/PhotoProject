find_package(Doxygen
             REQUIRED dot)
if (DOXYGEN_FOUND)

    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)
    set(DOXYGEN_DISABLE_INDEX NO)
    set(DOXYGEN_FULL_SIDEBAR NO)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET doxygen-awesome-css/doxygen-awesome.css;doxygen-awesome-css/doxygen-awesome-sidebar-only.css)
    set(DOXYGEN_HTML_COLORSTYLE LIGHT)
    doxygen_add_docs(doxygen
                    src;README.md
                    COMMENT "Generate man pages")
endif()