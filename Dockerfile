From ubuntu:latest

RUN apt-get update && apt-get install -y \
	gcc\
	clang \
	cmake \
	make \
	g++ 

ENV LD_LIBRARY_PATH=/libs
ENV CPLUS_INCLUDE_PATH=/libs/include

COPY . .
RUN cmake .
RUN make work
RUN ls

WORKDIR .

CMD ./work 

