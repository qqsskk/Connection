MAKE := make
SUBDIR := src demo/server demo/client

all: $(SUBDIR)
	@for sdir in ${SUBDIR}; do (cd $$sdir && $(MAKE)) || exit 1; done
clean: $(SUBDIR)
	@for sdir in ${SUBDIR}; do (cd $$sdir && $(MAKE) clean) || exit 1; done

