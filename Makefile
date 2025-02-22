XBE_TITLE = XBMC4Xbox

NXDK_CXX = y
#DEBUG = y

ifeq ($(DEBUG),y)
OUTPUT_DIR = $(CURDIR)/DEBUG
else
OUTPUT_DIR = $(CURDIR)/RELEASE
endif

GEN_XISO = $(OUTPUT_DIR)/$(XBE_TITLE).iso

SRCS += \
	$(CURDIR)/xbmc/unimplemented.cpp \
	$(CURDIR)/xbmc/platform/xbox/main.cpp \
	$(CURDIR)/xbmc/SystemGlobals.cpp

NXDK_CXXFLAGS +=	-Ilib \
									-Ixbmc \
									-Ixbmc/platform/xbox \
									-Wno-deprecated-declarations \
									-D_XBOX

include $(CURDIR)/xbmc/Makefile

include $(NXDK_DIR)/Makefile

clean_project:
	find . -type f \( -name "*.d" -o -name "*.obj" -o -name "*.pdb" -o -name "*.exe" -o -name "*.lib" \) -delete

