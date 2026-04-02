spdlog: 
	cd 3rdparty/spdlog && mkdir build && cd build && cmake .. && make

json:
	cd 3rdparty && wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz && tar -xf json.tar.xz && rm -f json.tar.xz && cd json && mkdir build && cd build && cmake -D JSON_BuildTests=OFF .. && make


libs:
	make spdlog
	make json

trader:
	cd trader && make

all: libs trader


.PHONY: libs trader

