###############################################################################
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################

# Nom du fichier cible (sans extension).
TARGET = mbpoll

# Chemin relatif du répertoire racine du projet de l'utilisateur
PROJECT_TOPDIR = ../../..

# Architecture du système cible
ARCH = ARCH_ARM_RASPBERRYPI
#ARCH = ARCH_GENERIC_LINUX

# Permet de générer un fichier version-git.h permettant de récupérer les informations sur la version
GIT_VERSION = ON

# Niveau d'optimisation de GCC =  [0, 1, 2, 3, s].
#     0 = pas d'optimisation (pour debug).
#     s = optimisation de la taille du code (pour release).
#     (Note: 3 n'est pas toujours le meilleur niveau. Voir la FAQ avr-libc.)
OPT = s

# Format informations Debug
#     Les formats natifs pour AVR-GCC -g sont dwarf-2 [default] ou stabs.
#     AVR Studio 4.10 nécessite dwarf-2.
DEBUG_FORMAT = dwarf-2

# Niveau d'optimisation de GCC =  [0, 1, 2, 3, s] pour le debug
#     0 = pas d'optimisation (pour debug).
#     s = optimisation de la taille du code (pour release).
#     (Note: 3 n'est pas toujours le meilleur niveau. Voir la FAQ avr-libc.)
DEBUG_OPT = 0

# Activation des informations Debug (ON/OFF)
# Si défini sur ON, aucune information de debug ne sera générée
# DEBUG = ON

# Affiche la ligne de compilation GCC ou non (ON/OFF)
VIEW_GCC_LINE = ON

# Désactive la suppression des variables et fonctions "inutiles"
# Le linker vérifie d'une fonction ou une variable est appellée, si ce n'est pas
# le cas, il supprime la variable ou la fonction
# Cela peut être problèmatique dans certains cas (bootloarder !)
DISABLE_DELETE_UNUSED_SECTIONS = OFF

# Liste des fichiers source C. (Les dépendances sont automatiquement générées.)
# Le chemin d'accès des fichiers sources systèmes a été ajouté au chemin de
# recherche du compilateur, il n'est donc pas nécessaire de préciser le chemin
# d'accès complet du fichier mais seulement le nom du projet
SRC  = $(TARGET).c

# Liste des fichiers source C++ (Les dépendances sont automatiquement générées.)
# Le chemin d'accès des fichiers sources systèmes a été ajouté au chemin de
# recherche du compilateur, il n'est donc pas nécessaire de préciser le chemin
# d'accès complet du fichier mais seulement le nom du projet (avrio, avrx, ...)
CPPSRC =

# Liste des fichiers source assembleur
#   L'extenson doit toujours être .S (en majuscule). En effet, les fichiers .s
#   ne sont pas consédérés comme des fichiers sources mais comme des fichiers
#   générés par le compilateur et seront supprimés lors d'un make clean.
#   Cela est valable aussi sous DOS/Windows (bien que le système d'exploitation
#   ne soit pas sensible à la casse).
ASRC =

# Place -D or -U options here for C sources
CDEFS +=

# Place -D or -U options here for ASM sources
ADEFS +=

# Place -D or -U options here for C++ sources
CPPDEFS +=

# Enable gcc warning (without -W)
WARNINGS = all strict-prototypes no-unused-but-set-variable

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRA_INCDIRS =

#---------------- Library Options ----------------

# Enable static link
STATIC_LINKER = OFF

# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRA_LIBDIRS =

# List any extra libraries here (without lib prefix).
#     Each library must be seperated by a space.
EXTRA_LIBS = modbus

# Enable link with  mathematics library (ON/OFF)
MATH_LIB_ENABLE = ON

# Compiler flag to set the C Standard level.
#     c89   = "ANSI" C
#     gnu89 = c89 plus GCC extensions
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99

#---------------- Install Options ----------------
prefix=/usr/local
INSTALL_BINDIR=$(prefix)/bin
VERSION=1.0.0

#---------------- SysIO Options ----------------
# Active le debug d'un test SysIO (ON/OFF)
# Si défini sur ON, la cible n'est pas liée à la lib sysio et les sources
# de SysIO sont recompilées. SYSIO_ROOT et ARCH doivent être définis
#SYSIO_DEBUG_TEST = ON

ifeq ($(SYSIO_ROOT),)
SYSIO_ROOT = $(PROJECT_TOPDIR)/../sysio
endif
#-----------------------------------------------

#-------------------------------------------------------------------------------
# Define programs and commands.
CC = gcc
OBJCOPY = objcopy
OBJDUMP = objdump
AR = ar rcs
NM = nm
SIZE = size
SHELL = sh
MAKEDIR = mkdir -p
REMOVE = rm -f
REMOVEDIR = rm -rf
COPY = cp

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
# !!!!!!!!!!!!!!!!!         DO NOT EDIT BELOW THIS LINE        !!!!!!!!!!!!!!!!!
#-------------------------------------------------------------------------------
CDEFS += -D_REENTRANT -D$(ARCH)
CPPDEFS += -D_REENTRANT -D$(ARCH)

EXTRA_LIBS += pthread rt
LDFLAGS += -pthread

ifeq ($(SYSIO_DEBUG_TEST),ON)
ifeq ($(SYSIO_ROOT),)
$(error SYSIO_DEBUG_TEST On and SYSIO_ROOT not defined, double-check that !)
else
include $(SYSIO_ROOT)/sysio.mk
endif
else
EXTRA_LIBS += sysio
endif

ifeq ($(PROJECT_TOPDIR),)
else
VPATH+=:$(PROJECT_TOPDIR)
EXTRA_INCDIRS += $(PROJECT_TOPDIR)
endif

#-------------------------------------------------------------------------------
# Destination files directory
DESTDIR = .

# Object files directory
OBJDIR = $(DESTDIR)/obj

# Full Path of TARGET
TARGET_PATH = $(DESTDIR)/$(TARGET)
TARGET_LIB_PATH = $(DESTDIR)/lib$(TARGET)

#---------------- Compiler Options C ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
ifeq ($(DEBUG),ON)
CFLAGS += -g$(DEBUG_FORMAT) -O$(DEBUG_OPT) -DDEBUG
else
CFLAGS += -O$(OPT) -DNDEBUG
endif
CFLAGS += $(CDEFS)
CFLAGS += -Wa,-adhlns=$(addprefix $(OBJDIR)/, $*.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRA_INCDIRS))
CFLAGS += $(patsubst %,-W%,$(WARNINGS))
CFLAGS += $(CSTANDARD)
ifeq ($(DISABLE_DELETE_UNUSED_SECTIONS),OFF)
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
endif

#---------------- Compiler Options C++ ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
ifeq ($(DEBUG),ON)
CPPFLAGS += -g$(DEBUG_FORMAT) -O$(DEBUG_OPT) -DDEBUG
else
CPPFLAGS += -O$(OPT) -DNDEBUG
endif

CPPFLAGS += $(CPPDEFS)
CPPFLAGS += -Wall
CPPFLAGS += -Wa,-adhlns=$(addprefix $(OBJDIR)/, $*.lst)
CPPFLAGS += $(patsubst %,-I%,$(EXTRA_INCDIRS))
CPPFLAGS += $(patsubst %,-W%,$(WARNINGS))
ifeq ($(DISABLE_DELETE_UNUSED_SECTIONS),OFF)
CPPFLAGS += -ffunction-sections
CPPFLAGS += -fdata-sections
endif

#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -adhlns:   create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see libc docs [FIXME: not yet described there]
#  -listing-cont-lines: Sets the maximum number of continuation lines of hex
#       dump that will be displayed for a given single line of source input.
ASFLAGS += $(ADEFS)
ASFLAGS += -ffunction-sections
ASFLAGS += -fdata-sections
ASFLAGS +=  -Wa,-adhlns=$(addprefix $(OBJDIR)/, $*.lst),-gstabs+
ASFLAGS += $(patsubst %,-I%,$(EXTRA_INCDIRS))

#---------------- Library Options ----------------
ifeq ($(MATH_LIB_ENABLE),ON)
MATH_LIB = -lm
endif

#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
ifeq ($(STATIC_LINKER),ON)
LDFLAGS += -static
endif
LDFLAGS += $(patsubst %,-L%,$(EXTRA_LIBDIRS))
LDFLAGS += $(patsubst %,-l%,$(EXTRA_LIBS))
LDFLAGS += $(MATH_LIB)
LDFLAGS += -Wl,-Map=$(TARGET_PATH).map,--cref
LDFLAGS += $(EXTMEMOPTS)
ifeq ($(DISABLE_DELETE_UNUSED_SECTIONS),OFF)
LDFLAGS += -Wl,--gc-sections
endif
LDFLAGS += -Wl,--relax
ifeq ($(DEBUG),ON)
LD_CFLAGS += -g$(DEBUG_FORMAT)
endif


# Define Messages
# English
MSG_COMPILING = [CC]\t\t
MSG_COMPILING_CPP = [CPP]\t\t
MSG_ASSEMBLING = [ASM]\t\t
MSG_LINKING = [LINK]\t\t
MSG_CREATING_LIBRARY = [LIB]\t\t
MSG_CLEANING = [CLEAN]\t\t
MSG_EXTENDED_LISTING = [LISTING]\t
MSG_SYMBOL_TABLE = [SYMBOL]\t
MSG_SIZE = [SIZE]
MSG_INSTALL = [INSTALL]
MSG_UNINSTALL = [UNINSTALL]

# Define all object files.
OBJ = $(addprefix $(OBJDIR)/, $(SRC:%.c=%.o) $(CPPSRC:%.cpp=%.o) $(ASRC:%.S=%.o))

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP -MF $(@D)/.dep/$(@F).d

# Generate the list of directories for object files
OBJDIRS := $(sort $(dir $(OBJ)))
DEPDIRS := $(addsuffix .dep, $(OBJDIRS))

# Combine all necessary flags and optional flags.
ALL_CFLAGS = -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_CPPFLAGS = -I. -x c++ $(CPPFLAGS)  $(GENDEPFLAGS)
ALL_ASFLAGS = -I. -x assembler-with-cpp $(ASFLAGS)
#

ifeq ($(VIEW_GCC_LINE),ON)
else
CC := @$(CC)
OBJCOPY := @$(OBJCOPY)
OBJDUMP := @$(OBJDUMP)
endif


# Default target.
all: build sizeafter cleanver
build: elf lss sym
rebuild: sizebefore clean_list build sizeafter
clean: clean_list
distclean: distclean_list clean_list

install: uninstall build
	@echo "$(MSG_INSTALL) $(TARGET)"
	-install -m 0755 $(TARGET) $(INSTALL_BINDIR)
	-chmod u+s $(INSTALL_BINDIR)/$(TARGET)

uninstall:
	@echo "$(MSG_UNINSTALL) $(TARGET)"
	-rm -f $(INSTALL_BINDIR)/$(TARGET)

elf: version-git.h $(TARGET)
lss: $(TARGET_PATH).lss
sym: $(TARGET_PATH).sym

lib: version-git.h $(TARGET_LIB_PATH).a
cleanlib: clean_list_lib
rebuildlib: clean_list_lib $(TARGET_LIB_PATH).a
distcleanlib: distclean_list clean_list_lib

# Include the dependency files.
DEPFILES := $(foreach dep,$(OBJ:.o=.o.d),$(dir $(dep)).dep/$(notdir $(dep)))
-include $(DEPFILES)

# Create the list of directories for object and dependencies files
$(OBJ): | $(OBJDIRS) $(DEPDIRS)

$(OBJDIRS):
	@-$(MAKEDIR) $@

$(DEPDIRS):
	@-$(MAKEDIR) $@

version-git.h:
ifeq ($(GIT_VERSION),ON)
	@sysio-ver $@
endif

version-git.mk:
ifeq ($(GIT_VERSION),ON)
	@sysio-ver $@
endif

sizebefore:
	@if test -f $(TARGET); then echo "$(MSG_SIZE)"; $(SIZE) $(TARGET); 2>/dev/null; fi

sizeafter:
	@if test -f $(TARGET); then echo "$(MSG_SIZE)"; $(SIZE) $(TARGET); 2>/dev/null; fi

size: sizebefore

cleanver:
ifeq ($(GIT_VERSION),ON)
	@test -s .version || $(REMOVE) version-git.h .version
endif

# Create extended listing file from ELF output file.
%.lss: $(TARGET)
	@echo "$(MSG_EXTENDED_LISTING) $@"
	@$(OBJDUMP) -h -S -z $< > $@

# Create a symbol table from ELF output file.
%.sym: $(TARGET)
	@echo "$(MSG_SYMBOL_TABLE) $@"
	@$(NM) -n $< > $@

# Create library from object files.
.SECONDARY : $(TARGET_LIB_PATH).a $(TARGET_LIB_PATH).so
.PRECIOUS : $(OBJ)
%.a: $(OBJ)
	@echo "$(MSG_CREATING_LIBRARY) $@"
	@$(AR) $@ $(OBJ)

%.so: $(OBJ)
	@echo "$(MSG_CREATING_LIBRARY) $@"
	$(CC) -shared $^ -o $@

# Link: create ELF output file from object files.
$(TARGET): $(OBJ)
	@echo "$(MSG_LINKING) $@"
	$(CC) $(LD_CFLAGS) $^ --output $@ $(LDFLAGS)

# Compile: create object files from C source files.
$(OBJDIR)/%.o : %.c Makefile
	@echo "$(MSG_COMPILING) $<"
	$(CC) -c $(ALL_CFLAGS) -fPIC $< -o $@


# Compile: create object files from C++ source files.
$(OBJDIR)/%.o : %.cpp Makefile
	@echo "$(MSG_COMPILING_CPP) $<"
	$(CC) -c $(ALL_CPPFLAGS) $< -o $@


# Compile: create assembler files from C source files.
%.s : %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C++ source files.
%.s : %.cpp
	$(CC) -S $(ALL_CPPFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o : %.S Makefile
	@echo "$(MSG_ASSEMBLING) $<"
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Create preprocessed source for use in sending a bug report.
%.i : %.c
	$(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $@

clean_list_lib:
	@echo "$(MSG_CLEANING) $(TARGET)"
	@$(REMOVE) $(TARGET_LIB_PATH).a

clean_list :
	@echo "$(MSG_CLEANING) $(TARGET)"
	@$(REMOVE) $(TARGET)
	@$(REMOVE) $(TARGET_PATH).map
	@$(REMOVE) $(TARGET_PATH).sym
	@$(REMOVE) $(TARGET_PATH).lss
	@$(REMOVEDIR) $(DEPDIRS)
	@$(REMOVEDIR) $(OBJDIRS)

distclean_list :
	@$(REMOVE) *.bak
	@$(REMOVE) *~
ifeq ($(GIT_VERSION),ON)
	@$(REMOVE) version-git.h version-git.mk .version
endif

# Listing of phony targets.
.PHONY : all size sizebefore sizeafter build rebuild lib elf \
lss sym clean distclean cleanlib clean_list clean_list_lib

# Make docs pictures
FIG2DEV                 = fig2dev

dox: eps png pdf

eps: $(TARGET_PATH).eps
png: $(TARGET_PATH).png
pdf: $(TARGET_PATH).pdf

%.eps: %.fig
	@$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	@$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	@$(FIG2DEV) -L png $< $@
