# Based on Ubuntu 22.04 LTS base
FROM ubuntu:22.04

# update packages
RUN apt-get update

# install DOSBox
RUN apt-get -y install dosbox

# install stuff for language and timezone
RUN apt-get -y install locales tzdata

# install Atmel AVR compiler and tools
RUN apt-get -y install gcc-avr avr-libc make unzip patch

# install stuff for newer ATtiny parts
# See guide: http://leonerds-code.blogspot.com/2019/06/building-for-new-attiny-1-series-chips.html
ADD atmel_setup /tmp/atmel_setup
RUN unzip /tmp/atmel_setup/Atmel.ATtiny_DFP.2.0.368.atpack -d /tmp/atmel_setup
RUN bash -c 'cp /tmp/atmel_setup/include/avr/iotn?*1[2467].h /usr/lib/avr/include/avr/'
RUN bash -c 'cp /tmp/atmel_setup/gcc/dev/attiny?*1[2467]/avrxmega3/*.{o,a} /usr/lib/avr/lib/avrxmega3/'
RUN bash -c 'cp /tmp/atmel_setup/gcc/dev/attiny?*1[2467]/avrxmega3/short-calls/*.{o,a} /usr/lib/avr/lib/avrxmega3/short-calls/'
RUN patch /usr/lib/avr/include/avr/io.h < /tmp/atmel_setup/io.h.patch
RUN rm -rf /tmp/atmel_setup

# set locale to English (US) UTF-8, timezone to Detroit
RUN locale-gen "en_US.UTF-8"
ENV LC_ALL   en_US.UTF-8
ENV LANG     en_US.UTF-8
ENV LANGUAGE en_US.UTF-8
ENV TZ="America/Detroit"
