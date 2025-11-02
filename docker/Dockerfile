
FROM debian:buster-slim
RUN apt update -y && apt upgrade -y
RUN apt -y install wget bzip2 sqlite3 jq
RUN apt -y install python3 python3-pip
RUN python3 -m pip install numpy
RUN python3 -m pip install pandas
COPY deploy-bin.tgz /
RUN cd / && tar -xvzf deploy-bin.tgz && rm deploy-bin.tgz
#COPY genstats.py /home/hyperclique/
RUN cd /home/hyperclique && wget https://github.com/mpelleau/ICTAI-2025/releases/download/v0/experiments-tuwien.json.bz2
RUN cd /home/hyperclique && wget https://github.com/mpelleau/ICTAI-2025/releases/download/v0/tuwien.sqlite.bz2 
RUN bunzip2 /home/hyperclique/tuwien.sqlite.bz2
RUN cd /home/hyperclique && wget https://github.com/mpelleau/ICTAI-2025/releases/download/v0/tuwien-htd.tbz && tar -xvjf tuwien-htd.tbz && rm tuwien-htd.tbz
WORKDIR /home/hyperclique



