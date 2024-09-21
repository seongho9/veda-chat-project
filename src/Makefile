SUBDIRS = client server data parser

.PHONY: clean obj $(SUBDIRS)

all: clean_client client clean_server server

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

clean_server:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

clean_client:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

client:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir CC=gcc obj; \
	done
	$(MAKE) -C client CC=gcc
	mv client/output ./client_exec

server:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir CC=aarch64-linux-gnu-gcc-12 obj; \
	done
	$(MAKE) -C server CC=aarch64-linux-gnu-gcc-12
	mv server/output ./server_exec