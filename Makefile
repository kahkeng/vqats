OPTIONS_SAMPLING=-D SAMPLING_SIZE=100 -D SAMPLING_WIN_X=11 -D SAMPLING_WIN_Y=11 -D SAMPLING_LUMINANCE_WEIGHTING
OPTIONS_FIB=-D FH_STATS
OPTIONS_A=-D CACHE_SIZE=100 $(OPTIONS_SAMPLING) $(OPTIONS_FIB)
OPTIONS_B=-D CACHE_SIZE=20 $(OPTIONS_SAMPLING) $(OPTIONS_FIB)
OPTIONS_D=-D CACHE_SIZE=100 $(OPTIONS_SAMPLING)
OPTIONS_DR=-D CACHE_SIZE=100 $(OPTIONS_SAMPLING)
OPTIONS_L=-D CACHE_SIZE=100 $(OPTIONS_SAMPLING)

all: pkg vqatsA vqatsB vqatsD vqatsDR vqatsL fib

pkg:
	@PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/
	@export PKG_CONFIG_PATH

fib:
	@gcc -Wall $(OPTIONS_FIB) -I. -c fib.c -o fib.o

vqats%: fib
	@$(MAKE) -s _$@ ID=$(subst vqats,,$@)

_vqats%:
	gcc -Wall $(OPTIONS_$(ID)) vqats.cc algo$(ID).cc tool.cc fib.o `pkg-config --cflags opencv` `pkg-config --libs opencv` -o vqats$(ID)x
	gcc -Wall -g -D DEBUG $(OPTIONS_$(ID)) vqats.cc algo$(ID).cc tool.cc fib.o `pkg-config --cflags opencv` `pkg-config --libs opencv` -o vqats$(ID)d
