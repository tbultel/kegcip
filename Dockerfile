
FROM debian:buster

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
    bash \
    bc \
    binutils \
    bison \
    build-essential \
    bzip2 \
    cpio \
    cmake \
    curl \
    expect \
    flex \
    git \
    gzip \
    jq \
    libbz2-dev \
	libfontconfig \
    libgtk-3-0 \
    libncurses-dev \
    libncurses5 \
    libncursesw5 \
    libusb-1.0-0-dev \
    libssl-dev \
	libsm6 \	
	libxft2 \
	libxtst6 \
	libxxf86vm1 \
    locales \
    make \
    mercurial \
    moreutils \
    patch \
    perl \
    pkg-config \
    python3 \
    python3-pip \
    python-pip \
    rsync \
    sed \
    sudo \
    tar \
    tree \
    unzip \
    vim \
    wget \
    whois \
    zlib1g-dev

RUN rm -rf /var/lib/apt/lists/*

ENV ARDUINO=arduino-1.8.19
ENV ARDUINO_SDK=${ARDUINO}-linux64.tar.xz

RUN cd /opt && wget -q https://downloads.arduino.cc/${ARDUINO_SDK} 
RUN cd /opt && tar xf ${ARDUINO_SDK}

ENV TEENSYDUINO=TeensyduinoInstall.linux64

RUN cd /opt && wget -q https://www.pjrc.com/teensy/td_157/${TEENSYDUINO}
ENV DISPLAY=:0

ENV UDEV=ON

#RUN cd /etc/udev/rules.d && wget https://www.pjrc.com/teensy/00-teensy.rules 

RUN cd /opt && chmod +x ${TEENSYDUINO} && ./${TEENSYDUINO} --dir=/opt/${ARDUINO}
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh


RUN adduser --disabled-login docker_user && \
    echo docker_user:docker_user | chpasswd

RUN groupadd -f wheel
RUN usermod -a -G wheel docker_user
RUN usermod -a -G sudo docker_user

ENV DOCKER_USER_HOME=/home/docker_user

RUN apt install -y libsm6

USER docker_user

RUN /bin/arduino-cli lib install U8g2
RUN /bin/arduino-cli lib install DallasTemperature
RUN /bin/arduino-cli lib install RotaryEncoder
RUN /bin/arduino-cli lib install "Adafruit MCP23017 Arduino Library"
RUN /bin/arduino-cli lib install "Adafruit BusIO"

RUN mkdir -p ${DOCKER_USER_HOME}/.buildroot-ccache

WORKDIR /workspaces/KegCip/kegcip
