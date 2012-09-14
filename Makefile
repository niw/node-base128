TARGET=build/Release/base128.node

all: $(TARGET)

$(TARGET): src/base128.cc
	node-gyp rebuild

clean:
	node-gyp clean

distclean: clean
	rm -rf node_modules

node_modules: package.json
	npm install

test: node_modules $(TARGET)
	./node_modules/.bin/mocha

.PHONY: test base128
