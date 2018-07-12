cppcheck --enable=all --inconclusive --language=c++ -i external/ -i build/ \
--project=$1/build/compile_commands.json -I$1/include/ $1
