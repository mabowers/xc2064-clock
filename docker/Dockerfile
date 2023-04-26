# Based on Ubuntu 22.04 LTS base
FROM ubuntu:22.04

# update packages
RUN apt-get update

# install DOSBox
RUN apt-get -y install dosbox

# install stuff for language and timezone
RUN apt-get -y install locales tzdata

# set locale to English (US) UTF-8, timezone to Detroit
RUN locale-gen "en_US.UTF-8"
ENV LC_ALL   en_US.UTF-8
ENV LANG     en_US.UTF-8
ENV LANGUAGE en_US.UTF-8
ENV TZ="America/Detroit"
