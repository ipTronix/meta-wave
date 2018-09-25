DESCRIPTION = "dev Image Frame Buffer Image QT5"

LICENSE = "MIT"

inherit core-image

#IMAGE_FSTYPES = "tar.bz2"
IMAGE_FSTYPES = "tar.gz"

IMAGE_INSTALL_append = " \
    firmware-imx-vpu-imx6q \
    firmware-imx-vpu-imx6d \
"

IMAGE_OVERHEAD_FACTOR = "2.0"

export IMAGE_BASENAME = "dev-fb-qt5"

EXTRA_IMAGE_FEATURES = "debug-tweaks ssh-server-openssh "

IMAGE_INSTALL_append = " libgcc libstdc++ \
    ccache chkconfig glib-networking glibmm \
    packagegroup-core-buildessential pkgconfig  \
    boost zlib glib-2.0 \
    iproute2 \
    cpufrequtils \
    gstreamer1.0 \
    gstreamer1.0-plugins-imx \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-meta-base \
    gstreamer1.0-rtsp-server \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-base \
    openssh-sftp-server \
    imx-vpu \
    qtbase-fonts \
    qtbase-plugins \
    qtbase-tools \
    qtdeclarative \
    qtdeclarative-plugins \
    qtdeclarative-tools \
    qtdeclarative-qmlplugins \
    qtmultimedia \
    qtmultimedia-plugins \
    qtmultimedia-qmlplugins \
    qtsvg \
    qtquickcontrols-qmlplugins \
    qtsvg-plugins \
    qtimageformats-plugins \
    qtsystems \
    qtsystems-tools \
    qtsystems-qmlplugins \
    qtscript \
    qtgraphicaleffects-qmlplugins \
    qtconnectivity-qmlplugins \
    fontconfig freetype pulseaudio dbus \
    alsa-lib alsa-tools alsa-state fsl-alsa-plugins \
    i2c-tools \
    mtd-utils imx-kobs mtd-utils-ubifs \
    e2fsprogs \
    packagegroup-fsl-tools-gpu \
    packagegroup-fsl-tools-gpu-external \
    fwupg \
    osd \
    u-boot-mkimage \
    script-install \
    "

