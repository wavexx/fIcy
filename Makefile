# Makefile for fIcy (for pmake or gmake)
# Copyright(c) 2004 by wave++ (Yuri D'Elia) <wavexx@users.sf.net>

# configuration
TARGETS := fIcy fResync fPls
FICY_OBJECTS := fIcy.o resolver.o socket.o http.o msg.o match.o \
	urlencode.o urlparse.o hdrparse.o icy.o sanitize.o
FRESYNC_OBJECTS := fResync.o msg.o mpeg.o copy.o
FPLS_OBJECTS := fPls.o msg.o resolver.o http.o socket.o \
	urlencode.o urlparse.o plsparse.o

# parameters
DEPS := Makedepend
#if $(CXX) != "g++"
# This will be overriden when using make of GNU make, so let's assume MIPSPro
DGEN := -MDupdate $(DEPS)
DUPD :=
#else
# Dependencies by side-effect using gcc still require a two-stage make, or
# tweaking the compilation command to get -MDupdate (as we do). I want Jam...
DGEN = -MD -MF $*.d
DUPD = cat *.d <&- > $(DEPS)
#endif


# suffixes, rules
.SUFFIXES: .cc .o
.cc.o:
	$(CXX) $(DGEN) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
	-@$(DUPD)


# targets
all: $(TARGETS)

fIcy: $(FICY_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(FICY_OBJECTS)

fResync: $(FRESYNC_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(FRESYNC_OBJECTS)

fPls: $(FPLS_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(FPLS_OBJECTS)


# stubs
.PHONY: all clean distclean
clean:
	rm -rf $(TARGETS) $(FICY_OBJECTS) $(FRESYNC_OBJECTS) \
		 $(FPLS_OBJECTS) ii_files core $(DEPS) *.d

distclean: clean
	rm -rf *~

sinclude $(DEPS)
