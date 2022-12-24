ORANGE='\033[0;33m'
NC='\033[0m' # No Color

# create, populate, move to test directory
echo -e "${ORANGE}[Create test-build directory]${NC}"
rm -r test-build;
mkdir test-build;
cp -r src test-build/src;
cp -r test test-build/test;
cd test-build;
mkdir lib;

# build googletest libraries
echo -e "${ORANGE}[Build googletest library]${NC}"
mkdir googletest_out;
cd googletest_out;
cmake ../../../lib/googletest;
make;
cp *.a ../lib;
cd ..;

# build test file using framework
echo -e "${ORANGE}[Build unit tests]${NC}"
g++ -Isrc -I../../lib/googletest/include/ -pthread ./test/threadsafe_prioqueue_test.cc lib/libgtest_main.a lib/libgtest.a -o./threadsafe_prioqueue_test;

# run unit tests
echo -e "${ORANGE}[Run unit tests]${NC}"
./threadsafe_prioqueue_test;
