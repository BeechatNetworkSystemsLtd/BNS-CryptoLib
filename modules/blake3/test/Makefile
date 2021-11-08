
.PHONY: all clear

all: clean
	@cp ../src/java/Blake3.java network/beechat/
	@mkdir bin
	@javac -d bin network/beechat/*.java
	@jar -cmf manifest.mf asap.jar -C ./bin .
	@rm network/beechat/Blake3.java

clean:
	@rm -rf bin

run:
	java -Djava.library.path=../build/Release/x86 -jar asap.jar

