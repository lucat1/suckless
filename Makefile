SUBFOLDERS = dwm dwmstatus st dmenu

all:
	$(foreach FOLDER,$(SUBFOLDERS), \
		$(MAKE) -C $(FOLDER); \
	)

install:
	$(foreach FOLDER,$(SUBFOLDERS), \
		$(MAKE) -C $(FOLDER) install; \
	)

clean:
	$(foreach FOLDER,$(SUBFOLDERS), \
		$(MAKE) -C $(FOLDER) clean; \
	)
