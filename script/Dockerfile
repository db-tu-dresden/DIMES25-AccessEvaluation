FROM ubuntu:latest

WORKDIR /visualize
 
RUN apt-get update && apt-get install -y \
        g++ \
        cmake \
        r-base \
        r-base-dev \
        texlive \ 
        texlive-latex-extra

RUN  R -e 'install.packages(c("magrittr", "dplyr", "ggplot2", "data.table", "collapse", "latex2exp", "tikzDevice"))'