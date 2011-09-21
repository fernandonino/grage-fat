


install:
	make install -C grage-linux-testing-unit
	make install -C grage-linux-testing-integration
	make install -C grage-commons
	make build   -C grage-pfs
	make build   -C grage-praid
	make build   -C grage-ppd

build:
	make install -C grage-linux-testing-unit
	make install -C grage-linux-testing-integration
	make install -C grage-commons
	make build-lib -C grage-ppd
	make build-lib -C grage-praid
	make build-lib -C grage-pfs

clean:
	make clean -C grage-commons
	make clean -C grage-linux-testing-integration
	make clean -C grage-linux-testing-unit
	make clean -C grage-pfs
	make clean -C grage-praid
	make clean -C grage-ppd
