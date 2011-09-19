


install:
	make install -C grage-linux-testing-unit
	make install -C grage-linux-testing-integration
	make install -C grage-linux-commons
	make build   -C grage-pfs
	make build   -C grage-praid
	make build   -C grage-ppd
clean:
	make clean -C grage-linux-commons
	make clean -C grage-linux-testing-integration
	make clean -C grage-linux-testing-unit
	make clean -C grage-pfs
	make clean -C grage-praid
	make clean -C grage-ppd
