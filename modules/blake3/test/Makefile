
.PHONY: all clear

all: ../build clean
	@cp ../src/java/Blake3.java network/beechat/
	@mkdir bin
	@javac -d bin network/beechat/*.java
	@jar -cmf manifest.mf asap.jar -C ./bin .
	@rm network/beechat/Blake3.java

clean:
	@rm -rf bin
	@rm -rf test_results.csv
	@rm -rf diagram.png
	@rm -rf asap.jar

run:
	@rm -rf test_results.csv
	@rm -rf diagram.png
	java -Djava.library.path=../build/Release/x86 -jar asap.jar
	@pip3 install numpy
	@pip3 install pandas
	@pip3 install matplotlib
	@python3 diagram.py

../build:
	@mkdir ../build
	@cd ../build && cmake .. && make
