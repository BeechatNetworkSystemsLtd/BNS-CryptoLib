

.PHONY: all clear

all: clean
	@mkdir bin
	@javac -d bin network/beechat/*.java
	@jar -cmf manifest.mf sign.jar -C ./bin .

clean:
	@rm -rf bin
	@rm -rf sign.jar
	@rm -rf key.public key.secret

run:
	java -Djava.library.path=../build/Release/ -jar sign.jar

