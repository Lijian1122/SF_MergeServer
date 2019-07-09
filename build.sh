build(){  
  if [ ! -d libs ]
    then
        mkdir libs
    fi
  cd Base
    cmake .
        make
    if [ $? -eq 0 ]; then
        echo "make Base successed";
    else
        echo "make Base failed";
        exit;
   fi
 cd ../Mutexmp4
   cmake .
        make
    if [ $? -eq 0 ]; then
        echo "make Mutexmp4 successed";
    else
        echo "make Mutexmp4 failed";
        exit;
    fi
 cd ../Monitor
   cmake .
        make
    if [ $? -eq 0 ]; then
        echo "make Monitor successed";
    else
        echo "make Monitor failed";
        exit;
    fi
 cd ..
   cmake .
        make
    if [ $? -eq 0 ]; then
        echo "make mergeServer successed";
    else
        echo "make mergeServer failed";
        exit;
    fi
}

clean(){
   cd Base
   make clean
   rm -rf CMakeCache.txt
   rm -rf CMakeFiles
   rm cmake_install.cmake
   rm Makefile

   cd ../Mutexmp4
   make clean
   rm -rf CMakeCache.txt
   rm -rf CMakeFiles
   rm cmake_install.cmake
   rm Makefile

   cd ../Monitor
   make clean
   rm -rf CMakeCache.txt
   rm -rf CMakeFiles
   rm cmake_install.cmake
   rm Makefile
   
   cd ..
   make clean
   rm -rf CMakeCache.txt
   rm -rf CMakeFiles
   rm cmake_install.cmake
   rm Makefile
}

case $1 in
	clean)
		echo "clean all build..."
		clean
		;;
	*)
		echo "all build..."
                build
		;;
esac

