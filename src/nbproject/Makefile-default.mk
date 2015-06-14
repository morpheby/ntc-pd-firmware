#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/________.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/________.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=modbus.c ADC.c main.c D_I_O.c Init_timers.c electrical_params_calc.c izmer_V.c system.c cn_inputs.c list.c mem_pool.c uart_base.c util.c oscillators.c resource.c configuration_bits.c traps.c wdt.c flash_store.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/modbus.o ${OBJECTDIR}/ADC.o ${OBJECTDIR}/main.o ${OBJECTDIR}/D_I_O.o ${OBJECTDIR}/Init_timers.o ${OBJECTDIR}/electrical_params_calc.o ${OBJECTDIR}/izmer_V.o ${OBJECTDIR}/system.o ${OBJECTDIR}/cn_inputs.o ${OBJECTDIR}/list.o ${OBJECTDIR}/mem_pool.o ${OBJECTDIR}/uart_base.o ${OBJECTDIR}/util.o ${OBJECTDIR}/oscillators.o ${OBJECTDIR}/resource.o ${OBJECTDIR}/configuration_bits.o ${OBJECTDIR}/traps.o ${OBJECTDIR}/wdt.o ${OBJECTDIR}/flash_store.o
POSSIBLE_DEPFILES=${OBJECTDIR}/modbus.o.d ${OBJECTDIR}/ADC.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/D_I_O.o.d ${OBJECTDIR}/Init_timers.o.d ${OBJECTDIR}/electrical_params_calc.o.d ${OBJECTDIR}/izmer_V.o.d ${OBJECTDIR}/system.o.d ${OBJECTDIR}/cn_inputs.o.d ${OBJECTDIR}/list.o.d ${OBJECTDIR}/mem_pool.o.d ${OBJECTDIR}/uart_base.o.d ${OBJECTDIR}/util.o.d ${OBJECTDIR}/oscillators.o.d ${OBJECTDIR}/resource.o.d ${OBJECTDIR}/configuration_bits.o.d ${OBJECTDIR}/traps.o.d ${OBJECTDIR}/wdt.o.d ${OBJECTDIR}/flash_store.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/modbus.o ${OBJECTDIR}/ADC.o ${OBJECTDIR}/main.o ${OBJECTDIR}/D_I_O.o ${OBJECTDIR}/Init_timers.o ${OBJECTDIR}/electrical_params_calc.o ${OBJECTDIR}/izmer_V.o ${OBJECTDIR}/system.o ${OBJECTDIR}/cn_inputs.o ${OBJECTDIR}/list.o ${OBJECTDIR}/mem_pool.o ${OBJECTDIR}/uart_base.o ${OBJECTDIR}/util.o ${OBJECTDIR}/oscillators.o ${OBJECTDIR}/resource.o ${OBJECTDIR}/configuration_bits.o ${OBJECTDIR}/traps.o ${OBJECTDIR}/wdt.o ${OBJECTDIR}/flash_store.o

# Source Files
SOURCEFILES=modbus.c ADC.c main.c D_I_O.c Init_timers.c electrical_params_calc.c izmer_V.c system.c cn_inputs.c list.c mem_pool.c uart_base.c util.c oscillators.c resource.c configuration_bits.c traps.c wdt.c flash_store.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/________.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ32MC204
MP_LINKER_FILE_OPTION=,--script="p33FJ32MC204.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/modbus.o: modbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/modbus.o.d 
	@${RM} ${OBJECTDIR}/modbus.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  modbus.c  -o ${OBJECTDIR}/modbus.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/modbus.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/modbus.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/ADC.o: ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ADC.c  -o ${OBJECTDIR}/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ADC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/ADC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/D_I_O.o: D_I_O.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/D_I_O.o.d 
	@${RM} ${OBJECTDIR}/D_I_O.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  D_I_O.c  -o ${OBJECTDIR}/D_I_O.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/D_I_O.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/D_I_O.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Init_timers.o: Init_timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Init_timers.o.d 
	@${RM} ${OBJECTDIR}/Init_timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Init_timers.c  -o ${OBJECTDIR}/Init_timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Init_timers.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/Init_timers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/electrical_params_calc.o: electrical_params_calc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/electrical_params_calc.o.d 
	@${RM} ${OBJECTDIR}/electrical_params_calc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  electrical_params_calc.c  -o ${OBJECTDIR}/electrical_params_calc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/electrical_params_calc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/electrical_params_calc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/izmer_V.o: izmer_V.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/izmer_V.o.d 
	@${RM} ${OBJECTDIR}/izmer_V.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  izmer_V.c  -o ${OBJECTDIR}/izmer_V.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/izmer_V.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/izmer_V.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/system.o: system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/system.o.d 
	@${RM} ${OBJECTDIR}/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  system.c  -o ${OBJECTDIR}/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/system.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/system.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/cn_inputs.o: cn_inputs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/cn_inputs.o.d 
	@${RM} ${OBJECTDIR}/cn_inputs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  cn_inputs.c  -o ${OBJECTDIR}/cn_inputs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/cn_inputs.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/cn_inputs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/list.o: list.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/list.o.d 
	@${RM} ${OBJECTDIR}/list.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  list.c  -o ${OBJECTDIR}/list.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/list.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/list.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mem_pool.o: mem_pool.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/mem_pool.o.d 
	@${RM} ${OBJECTDIR}/mem_pool.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mem_pool.c  -o ${OBJECTDIR}/mem_pool.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mem_pool.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/mem_pool.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/uart_base.o: uart_base.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart_base.o.d 
	@${RM} ${OBJECTDIR}/uart_base.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uart_base.c  -o ${OBJECTDIR}/uart_base.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/uart_base.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/uart_base.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/util.o: util.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/util.o.d 
	@${RM} ${OBJECTDIR}/util.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  util.c  -o ${OBJECTDIR}/util.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/util.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/util.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/oscillators.o: oscillators.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/oscillators.o.d 
	@${RM} ${OBJECTDIR}/oscillators.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  oscillators.c  -o ${OBJECTDIR}/oscillators.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/oscillators.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/oscillators.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/resource.o: resource.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/resource.o.d 
	@${RM} ${OBJECTDIR}/resource.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  resource.c  -o ${OBJECTDIR}/resource.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/resource.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/resource.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/configuration_bits.o: configuration_bits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/configuration_bits.o.d 
	@${RM} ${OBJECTDIR}/configuration_bits.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  configuration_bits.c  -o ${OBJECTDIR}/configuration_bits.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/configuration_bits.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/configuration_bits.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/traps.o: traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/traps.o.d 
	@${RM} ${OBJECTDIR}/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  traps.c  -o ${OBJECTDIR}/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/traps.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/wdt.o: wdt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/wdt.o.d 
	@${RM} ${OBJECTDIR}/wdt.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  wdt.c  -o ${OBJECTDIR}/wdt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/wdt.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/wdt.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/flash_store.o: flash_store.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/flash_store.o.d 
	@${RM} ${OBJECTDIR}/flash_store.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  flash_store.c  -o ${OBJECTDIR}/flash_store.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/flash_store.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1    -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/flash_store.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/modbus.o: modbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/modbus.o.d 
	@${RM} ${OBJECTDIR}/modbus.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  modbus.c  -o ${OBJECTDIR}/modbus.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/modbus.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/modbus.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/ADC.o: ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ADC.c  -o ${OBJECTDIR}/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ADC.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/ADC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/D_I_O.o: D_I_O.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/D_I_O.o.d 
	@${RM} ${OBJECTDIR}/D_I_O.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  D_I_O.c  -o ${OBJECTDIR}/D_I_O.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/D_I_O.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/D_I_O.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Init_timers.o: Init_timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Init_timers.o.d 
	@${RM} ${OBJECTDIR}/Init_timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Init_timers.c  -o ${OBJECTDIR}/Init_timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Init_timers.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/Init_timers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/electrical_params_calc.o: electrical_params_calc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/electrical_params_calc.o.d 
	@${RM} ${OBJECTDIR}/electrical_params_calc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  electrical_params_calc.c  -o ${OBJECTDIR}/electrical_params_calc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/electrical_params_calc.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/electrical_params_calc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/izmer_V.o: izmer_V.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/izmer_V.o.d 
	@${RM} ${OBJECTDIR}/izmer_V.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  izmer_V.c  -o ${OBJECTDIR}/izmer_V.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/izmer_V.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/izmer_V.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/system.o: system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/system.o.d 
	@${RM} ${OBJECTDIR}/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  system.c  -o ${OBJECTDIR}/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/system.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/system.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/cn_inputs.o: cn_inputs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/cn_inputs.o.d 
	@${RM} ${OBJECTDIR}/cn_inputs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  cn_inputs.c  -o ${OBJECTDIR}/cn_inputs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/cn_inputs.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/cn_inputs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/list.o: list.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/list.o.d 
	@${RM} ${OBJECTDIR}/list.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  list.c  -o ${OBJECTDIR}/list.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/list.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/list.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mem_pool.o: mem_pool.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/mem_pool.o.d 
	@${RM} ${OBJECTDIR}/mem_pool.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mem_pool.c  -o ${OBJECTDIR}/mem_pool.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mem_pool.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/mem_pool.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/uart_base.o: uart_base.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart_base.o.d 
	@${RM} ${OBJECTDIR}/uart_base.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uart_base.c  -o ${OBJECTDIR}/uart_base.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/uart_base.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/uart_base.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/util.o: util.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/util.o.d 
	@${RM} ${OBJECTDIR}/util.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  util.c  -o ${OBJECTDIR}/util.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/util.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/util.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/oscillators.o: oscillators.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/oscillators.o.d 
	@${RM} ${OBJECTDIR}/oscillators.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  oscillators.c  -o ${OBJECTDIR}/oscillators.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/oscillators.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/oscillators.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/resource.o: resource.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/resource.o.d 
	@${RM} ${OBJECTDIR}/resource.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  resource.c  -o ${OBJECTDIR}/resource.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/resource.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/resource.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/configuration_bits.o: configuration_bits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/configuration_bits.o.d 
	@${RM} ${OBJECTDIR}/configuration_bits.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  configuration_bits.c  -o ${OBJECTDIR}/configuration_bits.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/configuration_bits.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/configuration_bits.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/traps.o: traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/traps.o.d 
	@${RM} ${OBJECTDIR}/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  traps.c  -o ${OBJECTDIR}/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/traps.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/wdt.o: wdt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/wdt.o.d 
	@${RM} ${OBJECTDIR}/wdt.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  wdt.c  -o ${OBJECTDIR}/wdt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/wdt.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/wdt.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/flash_store.o: flash_store.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/flash_store.o.d 
	@${RM} ${OBJECTDIR}/flash_store.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  flash_store.c  -o ${OBJECTDIR}/flash_store.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/flash_store.o.d"        -g -omf=elf -fast-math -menable-large-arrays -O0 -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/flash_store.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/________.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    p33FJ32MC204.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/________.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1  -omf=elf -fast-math  -mreserve=data@0x800:0x822   -Wl,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PICKIT2=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/________.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   p33FJ32MC204.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/________.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -fast-math -Wl,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/________.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
