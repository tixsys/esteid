all: esteid.jar

esteid.jar: content/* skin/*
	zip -p $@ $+

clean:
	rm -f esteid.jar
