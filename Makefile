test_xtc:
	make -C app/core/xtc test

setup_xtc:
	make -C app/core/xtc setup

test_py:
	python3 -m unittest tests

test: test_xtc setup_xtc test_py
	@echo "running all tests..."

setup: setup_xtc
	python3 -m pip install -r requirements.txt

# pipreqs:
#	python3 -m pip install pipreqs

reqs:
	pipreqs senet