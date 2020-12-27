################################### Toolchain ###################################
CC = avr-gcc
DUMP = avr-objdump
COPY = avr-objcopy
SIZE = avr-size
AVR_DUDE = avrdude

MCU = atmega328p

CFLAGS = -Wall -mmcu=$(MCU) $(DEFS)

DFLAGS = -O2

DEFS = -DF_CPU=16000000UL \
		-D__AVR_ATmega328P__

FLASH_MCU = m328p
FLASH_PORT = /dev/ttyUSB0
FLASH_PROG = arduino
FLASH_BAUD = 115200
#################################################################################

################################## Directorios ##################################
# El directorio de trabajo es la raiz del proyecto
PROJDIR := $(realpath .)
# Directorio donde se encuentran los archivos del proyecto
SOURCEDIR := $(PROJDIR)/src
# Directorio donde se compilará el proyecto
BUILDDIR := $(PROJDIR)/build
# Directorio donde se compilarán los objs
OBJDIR := $(BUILDDIR)/obj
#################################################################################

#################################### Archivos ###################################
# Nombre del archivo ejecutable final
TARGET = firmware

# Directorios del proyecto
DIRS = libs \
		libs/hd44780 \
		libs/ticker \
		libs/wdt \
		libs/encoder

SOURCEDIRS = $(foreach dir, $(DIRS), $(addprefix $(SOURCEDIR)/, $(dir)))
OBJDIRS = $(foreach dir, $(DIRS), $(addprefix $(OBJDIR)/, $(dir)))

# Se generan los includes
INCLUDES = $(foreach dir, $(SOURCEDIRS), $(addprefix -I, $(dir)))

# Directorio temporal para buscar los .c
VPATH = $(SOURCEDIRS)

# Se crea un listado con los archivos .c
SOURCES = $(wildcard $(SOURCEDIR)/*.c)
SOURCES += $(foreach dir, $(SOURCEDIRS), $(wildcard $(dir)/*.c))

# Define los objects de los .c
OBJS := $(subst $(SOURCEDIR), $(OBJDIR), $(SOURCES:.c=.o))
#################################################################################

#################################### Comandos ###################################
MKDIR = mkdir -p
RM = rm -rf
#################################################################################

################################### Secuencias ##################################
.PHONY: all \
	clean \
	directories \
	flash

all: directories $(BUILDDIR)/$(TARGET).elf

$(BUILDDIR)/$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(DFLAGS) $(OBJS) -o $@
	$(DUMP) -h -S $@ > $(BUILDDIR)/$(TARGET).list
	$(COPY) -j .text -j .data -O ihex $@ $(BUILDDIR)/$(TARGET).hex
	$(SIZE) --format=avr --mcu=$(MCU) $@

$(OBJS): $(OBJDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) $(DFLAGS) $(INCLUDES) -c $< -o $@

directories:
	$(MKDIR) $(OBJDIRS)

clean:
	$(RM) $(OBJS)
	$(RM) $(OBJDIRS)
	$(RM) $(OBJDIR)
	$(RM) $(BUILDDIR)/*.*

flash: all
	$(AVR_DUDE) -p $(FLASH_MCU) -P $(FLASH_PORT) -c $(FLASH_PROG) -b $(FLASH_BAUD) -D -U flash:w:"$(BUILDDIR)/$(TARGET).hex":i
 #################################################################################
