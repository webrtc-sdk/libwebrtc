all:
	g++ -O2 -fPIC -c libwebrtc_wrap.cxx -I/opt/homebrew/opt/python@3.12/Frameworks/Python.framework/Versions/3.12/include/python3.12 -I. -std=c++17 
	g++ -shared libwebrtc_wrap.o -o _libwebrtc.so -L/opt/homebrew/opt/python@3.12/Frameworks/Python.framework/Versions/3.12/lib -lpython3.12 -L. -lwebrtc
swig:
	swig -c++ -python libwebrtc.i