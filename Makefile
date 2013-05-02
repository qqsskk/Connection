MAKE := make
SUBDIR := src \
		  demo/echo/server \
		  demo/echo/client \
		  demo/echo/select_server \
		  demo/echo/poll_server \
		  demo/echo/epoll_server \
		  demo/chatroom/server \
		  demo/chatroom/client

all: $(SUBDIR)
	@for sdir in ${SUBDIR}; do (cd $$sdir && $(MAKE)) || exit 1; done
clean: $(SUBDIR)
	@for sdir in ${SUBDIR}; do (cd $$sdir && $(MAKE) clean) || exit 1; done

