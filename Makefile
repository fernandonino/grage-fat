


install:
	make install -C grage-linux-testing-unit
	make install -C grage-linux-testing-integration
	make install -C grage-linux-commons

clean:
	make clean -C grage-linux-commons
	make clean -C grage-linux-testing-integration
	make clean -C grage-linux-testing-unit
	make clean -C grage-pfs
	make clean -C grage-praid
	make clean -C grage-ppd
