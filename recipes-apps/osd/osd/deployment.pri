unix {
    isEmpty(target.path) {
        //target.path = /opt/$${TARGET}/bin
        target.path = /usr/bin
        export(target.path)
    }
    INSTALLS += target
}

export(INSTALLS)






