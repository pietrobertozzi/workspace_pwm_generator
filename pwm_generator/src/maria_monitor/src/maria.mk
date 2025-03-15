
ifndef MARIA_DIR
$(error " MARIA_DIR variable not set!!!")
endif
ifndef OBJDIR
OBJDIR=$(BASE_DIR)/obj
endif

TOBECLEANED_MARIA = $(OBJDIR)/*.tbc $(OBJDIR)/*.ime $(OBJDIR)/*.set $(OBJDIR)/maria_table.c $(OBJDIR)/maria_table.o

######### todo: modify these lines to fit your system
#########
NM				  = nm -C -f posix --defined-only
CODEPARSER        = ./codeparser.exe
ELFPARSER         = ./elfparser.exe

ifeq ($(CROSS_COMPILE), arm-none-eabi-)
$(OBJDIR)/%tbc: $(OBJDIR)/%o
	$(ELFPARSER) -i $< -o $@ -t tbc
else
$(OBJDIR)/%tbc: $(OBJDIR)/%o
	$(NM) $< | $(CODEPARSER) $<
endif

###### remember to define a target "all" in your main Makefile
.PHONY: mkall
mkall: all

###### todo: uncomment this if you want eclipse to compile maria_table.c
######
#maria_table.o: maria_table.c
#	@echo 'Building file: $<'
#	@echo 'Invoking: Cross GCC Compiler'
#	# todo: copy gcc invocation from an eclipse auto-generated .mk file
#	arm-none-eabi-gcc -DBOARD_DWB -D__thumb2__=1 -DPACK_STRUCT_END="__attribute((packed))" -DALIGN_STRUCT_END="__attribute((aligned(4)))" -DTARGET_ARCH_ARM7 -DGCC_ARMCM3 -Dflash -DNOTRACE -Dlpc17xx -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\lpc17lib" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\lpc17lib\Core\CM3\CoreSupport" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\lpc17lib\Core\CM3\DeviceSupport\NXP\lpc17xx" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\lpc17lib\Drivers\include" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\freeRTOS" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\freeRTOS\Source\include" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\freeRTOS\Source\portable\GCC\ARM7_LPC23xx" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\freeRTOS\Common\include" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\freeRTOS\Common\ethernet\uIP\uip-1.0\uip" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\freeRTOS\Common\drivers\LuminaryMicro" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\maria_monitor\src" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\appl_dwb" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\common_rhe_dwb" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\common_mcdrivers" -I"C:\houston\job\imei_catcher\arm-pmc\svn\rh\common_dwbdrivers" -O0 -g -gdwarf-2 -Wall -c -mcpu=cortex-m3 -mthumb -mapcs-frame -msoft-float -mno-sched-prolog -fno-hosted -mtune=cortex-m3 -mfix-cortex-m3-ldrd -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '

MARIATABLEITEMS = \
		$(OBJDIR)/maria_utils.tbc \
		$(OBJDIR)/maria_info.tbc \

MARIAOBJECTS = $(OBJDIR)/maria_info.o \
				$(OBJDIR)/maria_parser.o \
				$(OBJDIR)/maria_utils.o 
# in mariaobjects non c'e' e non ci deve essere maria_table.o, perche' maria_table.o viene fatta dopo che tutti gli .o sono stati parsati
# quindi in un tempo successivo

###### MONITOR ######
$(OBJDIR)/maria_table.c: $(MARIATABLEITEMS)
	@echo "#include <maria_parser.h>" > $@
	@echo "#include <maria_info.h>" >> $@
	@echo " " >> $@
	-cat $(OBJDIR)/*.ime | sort >> $@
	@echo " " >> $@
	-cat $(OBJDIR)/*.use | sort >> $@
	@echo " " >> $@
	@echo "MARIA_TABLE_ATTR MARIA_TABLE_RECORD maria_table[] = {" >> $@
	cat $(OBJDIR)/*.tbc | sort >> $@
	@echo "{\"eol\",MARIA_SYMTYPE_FUNCTION,(GENPTR)0L}," >> $@
	@echo "};" >> $@
	@echo " " >> $@
	@echo "#ifndef NO_MARIA_TABLE_INIT /* maria_table is const */" >> $@
	@echo " " >> $@
	@echo "void init_maria_table(void)" >> $@
	@echo "{" >> $@
	@echo "     int i=0;" >> $@
	@echo " " >> $@
	-cat $(OBJDIR)/*.set | sort >> $@
	@echo "};" >> $@
	@echo " " >> $@
	@echo "#endif " >> $@
	@echo " " >> $@

##		cat $(TABLEITEMS) | sort -t '"' -k 2,2 >> $@

elfparser.exe: $(MARIA_DIR)/srcparser/elfparser.c
	$(HOSTCC) -I$(MARIA_DIR)/srcparser/ -o $@ $<
	
codeparser.exe: $(MARIA_DIR)/srcparser/codeparser.c
	$(HOSTCC) -I$(MARIA_DIR)/srcparser/ -o $@ $<

$(OBJDIR)/maria_table.o: $(OBJDIR)/maria_table.c
