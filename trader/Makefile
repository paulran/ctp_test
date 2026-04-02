trader:
	g++ -o ../bin/trader -O0 -g \
		-I../api -I../3rdparty/spdlog/include -I../3rdparty/json/include \
		-L../lib -L../3rdparty/spdlog/build \
		-lspdlog \
		main.cpp logger.cpp traderspi.cpp \
		../lib/thosttraderapi_se.so 

