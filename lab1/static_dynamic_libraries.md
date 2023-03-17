# static library

## kompilowanie stringlibrary.c i tworzenie archiwum
gcc -c stringlibrary.c [-o stringlibrary.o]
ar crs libstringlibrary.a stringlibrary.o

## kompilowanie main.c z użyciem biblioteki
gcc -c main.c [-o main.o]
gcc main.o libstringlibrary.a [-o main]
lub
gcc main.o –l stringlibrary –L ./ [-o main]


# shared library

## kompilowanie stringlibrary.c i tworzenie obiektu współdzielonego
gcc -fPIC -c stringlibrary.c [-o stringlibrary.o]
gcc -shared stringlibrary.o -o libstringlibrary.so

## kompilowanie main.c z użyciem biblioteki współdzielonej

gcc -L /home/gracjan/Dokumenty/Studia/SysOpy/dynamiclibrary/ main.c [-Wall] -lstringlibrary -o main

## dodanie stworzonej biblioteki do bibliotek współdzielonych
export LD_LIBRARY_PATH=/home/gracjan/Dokumenty/Studia/SysOpy/dynamiclibrary:$LD_LIBRARY_PATH

