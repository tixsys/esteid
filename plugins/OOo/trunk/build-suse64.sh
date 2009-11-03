export OO_SDK_HOME=/usr/lib64/ooo3/solver
export OO_SDK_URE_HOME=/usr/lib64/ooo3/basis-link/ure-link
export OFFICE_BASE_PROGRAM_PATH=/usr/lib64/ooo3/basis-link/program
export LD_LIBRARY_PATH=kataloog1:/usr/kaaloog2
cd build
rm -rf *
cmake ..
make
cd ..
