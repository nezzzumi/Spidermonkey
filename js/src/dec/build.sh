g++ -std=c++11 dec.cpp \
  -I../build_OPT.OBJ/dist/include \
  -L../build_OPT.OBJ/dist/lib \
  -lmozjs \
  -Wl,-rpath=../build_OPT.OBJ/dist/lib \
  -o dec
