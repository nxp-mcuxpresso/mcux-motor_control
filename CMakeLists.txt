
if(CONFIG_MCUX_COMPONENT_middleware.mc_pmsm_snsless.float)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DPMSM_SNSLESS"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/pmsm_control.c
                pmsm/pmsm_float/mc_algorithms/pmsm_control.h
                pmsm/pmsm_float/state_machine/state_machine.c
                pmsm/pmsm_float/state_machine/state_machine.h
                pmsm/pmsm_float/mc_state_machine/m1_sm_snsless.c
                pmsm/pmsm_float/mc_state_machine/m1_sm_snsless.h
                pmsm/pmsm_float/mc_state_machine/sm_common.h
                freemaster/pmsm_float.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_algorithms
                 pmsm/pmsm_float/state_machine
                 pmsm/pmsm_float/mc_state_machine
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.mc_pmsm_enc.float)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DPMSM_SNSLESS -DPMSM_SNSLESS_ENC"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/pmsm_control.c
                pmsm/pmsm_float/mc_algorithms/pmsm_control.h
                pmsm/pmsm_float/state_machine/state_machine.c
                pmsm/pmsm_float/state_machine/state_machine.h
                pmsm/pmsm_float/mc_state_machine/m1_sm_snsless_enc.c
                pmsm/pmsm_float/mc_state_machine/m1_sm_snsless_enc.h
                pmsm/pmsm_float/mc_state_machine/sm_common.h
                freemaster/pmsm_float_enc.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_algorithms
                 pmsm/pmsm_float/state_machine
                 pmsm/pmsm_float/mc_state_machine
                 pmsm/pmsm_float
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.mc_pmsm_enc_dual.float)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DPMSM_SNSLESS -DPMSM_SNSLESS_ENC -DDUAL_MOTOR"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/pmsm_control.c
                pmsm/pmsm_float/mc_algorithms/pmsm_control.h
                pmsm/pmsm_float/state_machine/state_machine.c
                pmsm/pmsm_float/state_machine/state_machine.h
                pmsm/pmsm_float/mc_state_machine/m1_sm_snsless_enc.c
                pmsm/pmsm_float/mc_state_machine/m1_sm_snsless_enc.h
                pmsm/pmsm_float/mc_state_machine/m2_sm_snsless_enc.c
                pmsm/pmsm_float/mc_state_machine/m2_sm_snsless_enc.h
                pmsm/pmsm_float/mc_state_machine/sm_common.h
                freemaster/pmsm_float_enc_dual.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_algorithms
                 pmsm/pmsm_float/state_machine
                 pmsm/pmsm_float/mc_state_machine
                 pmsm/pmsm_float
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.mc_pmsm_servo.float)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DPMSM_SNSLESS -DPMSM_SNSLESS_ENC -DPMSM_SERVO"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/pmsm_control.c
                pmsm/pmsm_float/mc_algorithms/pmsm_control.h
                pmsm/pmsm_float/state_machine/state_machine.c
                pmsm/pmsm_float/state_machine/state_machine.h
                pmsm/pmsm_float/mc_state_machine/m1_sm_servo.c
                pmsm/pmsm_float/mc_state_machine/m1_sm_servo.h
                pmsm/pmsm_float/mc_state_machine/sm_servo_common.h
                freemaster/pmsm_float_servo_dual.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_algorithms
                 pmsm/pmsm_float/state_machine
                 pmsm/pmsm_float/mc_state_machine
                 pmsm/pmsm_float
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.mc_pmsm_servo_dual.float)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DPMSM_SNSLESS -DPMSM_SNSLESS_ENC -DDUAL_MOTOR -DPMSM_SERVO"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/pmsm_control.c
                pmsm/pmsm_float/mc_algorithms/pmsm_control.h
                pmsm/pmsm_float/state_machine/state_machine.c
                pmsm/pmsm_float/state_machine/state_machine.h
                pmsm/pmsm_float/mc_state_machine/m1_sm_servo.c
                pmsm/pmsm_float/mc_state_machine/m1_sm_servo.h
                pmsm/pmsm_float/mc_state_machine/m2_sm_servo.c
                pmsm/pmsm_float/mc_state_machine/m2_sm_servo.h
                pmsm/pmsm_float/mc_state_machine/sm_servo_common.h
                freemaster/pmsm_float_servo_dual.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_algorithms
                 pmsm/pmsm_float/state_machine
                 pmsm/pmsm_float/mc_state_machine
                 pmsm/pmsm_float
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.mc_bldc.frac)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DBLDC_SNSLESS"
    )
    mcux_add_source(
        SOURCES bldc/mc_algorithms/bldc_control.c
                bldc/mc_algorithms/bldc_control.h
                bldc/state_machine/state_machine.c
                bldc/state_machine/state_machine.h
                bldc/mc_state_machine/m1_sm_snsless.c
                bldc/mc_state_machine/m1_sm_snsless.h
                freemaster/bldc.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES bldc/mc_algorithms
                 bldc/state_machine
                 bldc/mc_state_machine
                 bldc
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_bldc_frac.tsa)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES bldc/fm_tsa_bldc.c
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES bldc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.pmsm_mid.float)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DMID_EN"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_identification/mid_auxiliary.c
                pmsm/pmsm_float/mc_identification/mid_auxiliary.h
                pmsm/pmsm_float/mc_identification/mid_def.h
                pmsm/pmsm_float/mc_identification/mid_sm_states.c
                pmsm/pmsm_float/mc_identification/mid_sm_states.h
                pmsm/pmsm_float/mc_identification/pp_measure.c
                pmsm/pmsm_float/mc_identification/pp_measure.h
                pmsm/pmsm_float/mc_algorithms/pmsm_control.c
                pmsm/pmsm_float/mc_algorithms/pmsm_control.h
                freemaster/pmsm_float_enc.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_identification
                 pmsm/pmsm_float/mc_algorithms
                 pmsm/pmsm_float
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.mc_pmsm_snsless.frac)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DPMSM_SNSLESS"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/pmsm_control.c
                pmsm/pmsm_frac/mc_algorithms/pmsm_control.h
                pmsm/pmsm_frac/state_machine/state_machine.c
                pmsm/pmsm_frac/state_machine/state_machine.h
                pmsm/pmsm_frac/mc_state_machine/m1_sm_snsless.c
                pmsm/pmsm_frac/mc_state_machine/m1_sm_snsless.h
                pmsm/pmsm_frac/mc_state_machine/sm_common.h
                freemaster/pmsm_frac.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_algorithms
                 pmsm/pmsm_frac/state_machine
                 pmsm/pmsm_frac/mc_state_machine
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.mc_pmsm_enc.frac)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DPMSM_SNSLESS -DPMSM_SNSLESS_ENC"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/pmsm_control.c
                pmsm/pmsm_frac/mc_algorithms/pmsm_control.h
                pmsm/pmsm_frac/state_machine/state_machine.c
                pmsm/pmsm_frac/state_machine/state_machine.h
                pmsm/pmsm_frac/mc_state_machine/m1_sm_snsless_enc.c
                pmsm/pmsm_frac/mc_state_machine/m1_sm_snsless_enc.h
                pmsm/pmsm_frac/mc_state_machine/sm_common_enc.h
                freemaster/pmsm_frac_enc.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_algorithms
                 pmsm/pmsm_frac/state_machine
                 pmsm/pmsm_frac/mc_state_machine
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.pmsm_mid.frac)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DMID_EN"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_identification/mid_def.h
                pmsm/pmsm_frac/mc_identification/mid_sm_states.c
                pmsm/pmsm_frac/mc_identification/mid_sm_states.h
                pmsm/pmsm_frac/mc_identification/pp_measure.c
                pmsm/pmsm_frac/mc_identification/pp_measure.h
                pmsm/pmsm_frac/mc_identification/m1_mid_switch.c
                pmsm/pmsm_frac/mc_identification/m1_mid_switch.h
                pmsm/pmsm_frac/mc_algorithms/pmsm_control.c
                pmsm/pmsm_frac/mc_algorithms/pmsm_control.h
                freemaster/pmsm_frac.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_identification
                 pmsm/pmsm_frac/mc_algorithms
                 pmsm/pmsm_frac
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.pmsm_mid.frac_enc)
    mcux_component_version(3.2.0)
    mcux_add_configuration(
        CC  "-DMID_EN"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_identification/mid_def.h
                pmsm/pmsm_frac/mc_identification/mid_sm_states.c
                pmsm/pmsm_frac/mc_identification/mid_sm_states.h
                pmsm/pmsm_frac/mc_identification/pp_measure.c
                pmsm/pmsm_frac/mc_identification/pp_measure.h
                pmsm/pmsm_frac/mc_identification/m1_mid_switch.c
                pmsm/pmsm_frac/mc_identification/m1_mid_switch.h
                pmsm/pmsm_frac/mc_algorithms/pmsm_control.c
                pmsm/pmsm_frac/mc_algorithms/pmsm_control.h
                freemaster/pmsm_frac_enc.pmpx
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_identification
                 pmsm/pmsm_frac/mc_algorithms
                 pmsm/pmsm_frac
                 freemaster
                 docs
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_pmsm_float.template)
    mcux_component_version(3.2.0)

    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_cfg_template
        TARGET_FILES m1_pmsm_appconfig.h m2_pmsm_appconfig.h mc_periph_init.h
    )

    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_cfg_template/m1_pmsm_appconfig.h
                pmsm/pmsm_float/mc_cfg_template/m2_pmsm_appconfig.h
                pmsm/pmsm_float/mc_cfg_template/mc_periph_init.c
                pmsm/pmsm_float/mc_cfg_template/mc_periph_init.h
        CONFIG true
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_bldc_frac.template)
    mcux_component_version(3.2.0)

    mcux_add_include(
        INCLUDES bldc/mc_cfg_template
        TARGET_FILES m1_bldc_appconfig.h mc_periph_init.h
    )

    mcux_add_source(
        SOURCES bldc/mc_cfg_template/m1_bldc_appconfig.h
                bldc/mc_cfg_template/mc_periph_init.c
                bldc/mc_cfg_template/mc_periph_init.h
        CONFIG true
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_pmsm_frac.template)
    mcux_component_version(3.2.0)

    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_cfg_template
        TARGET_FILES m1_pmsm_appconfig.h mc_periph_init.h
    )

    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_cfg_template/m1_pmsm_appconfig.h
                pmsm/pmsm_frac/mc_cfg_template/mc_periph_init.c
                pmsm/pmsm_frac/mc_cfg_template/mc_periph_init.h
        CONFIG true
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_pmsm_float.tsa)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/fm_tsa_pmsm.c
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_pmsm_servo_float.tsa)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/fm_tsa_pmsm_servo.c
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_pmsm_frac.tsa)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/fm_tsa_pmsm.c
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_pmsm_frac_enc.tsa)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/fm_tsa_pmsm.c
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adc_mcx_epwm_frac)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_drivers/mcdrv_adc_mcxa.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_adc_mcxa.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_epwm.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_epwm.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_enc_eqd.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_enc_eqd.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adc_mcxa_epwm_float)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adc_mcxa.c
                pmsm/pmsm_float/mc_drivers/mcdrv_adc_mcxa.h
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_epwm.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_epwm.h
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_eqd.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_eqd.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adc_mcxa20_epwm_float)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adc_mcxa20.c
                pmsm/pmsm_float/mc_drivers/mcdrv_adc_mcxa20.h
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_epwm.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_epwm.h
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_eqd.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_eqd.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adclpc_ftm_frac)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_drivers/mcdrv_adc_lpc860.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_adc_lpc860.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_ftm.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_ftm.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adc_mcxe_ftm_float)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adc_mcxe.c
                pmsm/pmsm_float/mc_drivers/mcdrv_adc_mcxe.h
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_ftm.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_ftm.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adc12_pwma_frac)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_drivers/mcdrv_adc_adc12.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_adc_adc12.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_pwma.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adc12_pwma_qdc_frac)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_drivers/mcdrv_adc_adc12.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_adc_adc12.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_pwma.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_enc_qd.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_enc_qd.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adc12_pwma_qspi_frac)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_drivers/mcdrv_adc_adc12.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_adc_adc12.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_pwma.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_mosfet_predrv_qspi.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_mosfet_predrv_qspi.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adc12_pwma_qspi_frac_enc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_drivers/mcdrv_adc_adc12.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_adc_adc12.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_pwma.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_mosfet_predrv_qspi.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_mosfet_predrv_qspi.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_enc_qd.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_enc_qd.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adc12_pwma_frac_enc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_drivers/mcdrv_adc_adc12.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_adc_adc12.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_pwm3ph_pwma.h
                pmsm/pmsm_frac/mc_drivers/mcdrv_enc_qtmr.c
                pmsm/pmsm_frac/mc_drivers/mcdrv_enc_qtmr.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_lpcadc_sctimer)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adc_lpc.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adc_lpc.c
                pmsm/pmsm_float/mc_drivers/mcdrv_sctimer_lpc.c
                pmsm/pmsm_float/mc_drivers/mcdrv_sctimer_lpc.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_lpcadc_pwma_enc)
    mcux_component_version(3.2.0)
	mcux_add_configuration(
		CC  "-DMCDRV_FLASH_LPC55S36"
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adc_lpc55s36.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adc_lpc55s36.c
                pmsm/pmsm_float/mc_drivers/mcdrv_eflexpwm_lpc.c
                pmsm/pmsm_float/mc_drivers/mcdrv_eflexpwm_lpc.h
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qd.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qd.h
                pmsm/pmsm_float/mc_drivers/mcdrv_flash_lpc55s36.h
                pmsm/pmsm_float/mc_drivers/mcdrv_flash_lpc55s36.c
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_lpcadc_pwma_bldc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES bldc/mc_drivers/mcdrv_adc_lpc55s36.h
                bldc/mc_drivers/mcdrv_adc_lpc55s36.c
                bldc/mc_drivers/mcdrv_eflexpwm_lpc.c
                bldc/mc_drivers/mcdrv_eflexpwm_lpc.h
                bldc/mc_drivers/mcdrv_cmt_eflexpwm_lpc55s36.c
                bldc/mc_drivers/mcdrv_cmt_eflexpwm_lpc55s36.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES bldc/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcncxn_pwma_enc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adc_mcxn.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adc_mcxn.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qd.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qd.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcncxn_pwma_qdc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adc_mcxn.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adc_mcxn.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qdc.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qdc.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_mcxe3xx)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_sar_adc_mcxe.h
                pmsm/pmsm_float/mc_drivers/mcdrv_sar_adc_mcxe.c
                pmsm/pmsm_float/mc_drivers/mcdrv_lcu_mcxe.c
                pmsm/pmsm_float/mc_drivers/mcdrv_lcu_mcxe.h
				pmsm/pmsm_float/mc_drivers/mcdrv_emios_mcxe.c
                pmsm/pmsm_float/mc_drivers/mcdrv_emios_mcxe.h
				pmsm/pmsm_float/mc_drivers/mcdrv_enc_emios.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_emios.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcimxrt1010_pwma)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adcetc1010_imxrt.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adcetc1010_imxrt.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcimxrt1010_pwma_enc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adcetc1010_imxrt.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adcetc1010_imxrt.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qd.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qd.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcimxrt_pwma)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adcetc_imxrt.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adcetc_imxrt.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcimxrt_pwma_enc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adcetc_imxrt.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adcetc_imxrt.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qd.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qd.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcimxrt11xx_pwma)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adcetc_imxrt11xx.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adcetc_imxrt11xx.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcimxrt11xx_pwma_enc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adcetc_imxrt11xx.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adcetc_imxrt11xx.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qd.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_qd.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcimxrt11xx_pwma_bldc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES bldc/mc_drivers/mcdrv_pwm3ph_pwma.c
                bldc/mc_drivers/mcdrv_pwm3ph_pwma.h
                bldc/mc_drivers/mcdrv_adcetc_imxrt11xx.c
                bldc/mc_drivers/mcdrv_adcetc_imxrt11xx.h
                bldc/mc_drivers/mcdrv_cmt_pwma.c
                bldc/mc_drivers/mcdrv_cmt_pwma.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES bldc/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcimxrt1180_pwma_enc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adc_imxrt118x.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adc_imxrt118x.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_eqd2.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_eqd2.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_adcimxrt1186_pwma_enc)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adc_imxrt1186.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adc_imxrt1186.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
				pmsm/pmsm_float/mc_drivers/mcdrv_enc_eqd2.c
                pmsm/pmsm_float/mc_drivers/mcdrv_enc_eqd2.h
				pmsm/pmsm_float/mc_drivers/mcdrv_sinc.c
				pmsm/pmsm_float/mc_drivers/mcdrv_sinc.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_imxrt1186_pwma_sinc_endat)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_drivers/mcdrv_adc_imxrt1186.h
                pmsm/pmsm_float/mc_drivers/mcdrv_adc_imxrt1186.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
                pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
				pmsm/pmsm_float/mc_drivers/mcdrv_sinc2.c
				pmsm/pmsm_float/mc_drivers/mcdrv_sinc2.h			
				pmsm/pmsm_float/mc_drivers/fsl_flexio_endat2p2.c
				pmsm/pmsm_float/mc_drivers/fsl_flexio_endat2p2.h				
				pmsm/pmsm_float/mc_drivers/mcdrv_flexio_endat2p2.c
				pmsm/pmsm_float/mc_drivers/mcdrv_flexio_endat2p2.h
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if (CONFIG_MCUX_COMPONENT_middleware.motor_control.mc_drivers_imx943evk)
	mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES
            pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.c
            pmsm/pmsm_float/mc_drivers/mcdrv_pwm3ph_pwma.h
			pmsm/pmsm_float/mc_drivers/mcdrv_bissc.c
			pmsm/pmsm_float/mc_drivers/mcdrv_bissc.h
			pmsm/pmsm_float/mc_drivers/mcdrv_sinc.c
			pmsm/pmsm_float/mc_drivers/mcdrv_sinc.h
			pmsm/pmsm_float/mc_drivers/mcdrv_endat2p2.c
			pmsm/pmsm_float/mc_drivers/mcdrv_endat2p2.h
			pmsm/pmsm_float/mc_drivers/mcdrv_endat3.c
			pmsm/pmsm_float/mc_drivers/mcdrv_endat3.h
		BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
	
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_drivers
		BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_pmsm_float_imcat)
    mcux_component_version(3.2.0)
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_pmsm_frac_imcat)
    mcux_component_version(3.2.0)
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control_bldc_frac_imcat)
    mcux_component_version(3.2.0)
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mcaa_cm33f)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_iar/Include/MCAA_EstimRL_FLT.h
				pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_iar/Include/MCAA_EstimBJ_FLT.h
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_iar/Include/mcaa_lib_fp.h
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_mdk/Include/MCAA_EstimRL_FLT.h
				pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_mdk/Include/MCAA_EstimBJ_FLT.h
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_mdk/Include/mcaa_lib_fp.h
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_mcux/Include/mcaa_lib_fp.h
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_mcux/Include/MCAA_EstimRL_FLT.h
				pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_mcux/Include/MCAA_EstimBJ_FLT.h
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_iar/mcaa_lib.a
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_mdk/mcaa_lib.lib
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_mcux/libmcaa_lib.a
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS iar
        INCLUDES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_iar/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS mdk
        INCLUDES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_mdk/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS mcux armgcc
        INCLUDES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm33f_mcux/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mcaa_cm33nodsp)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_iar/Include/MCAA_EstimRL.h
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_iar/Include/mcaa_lib.h
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_mdk/Include/MCAA_EstimRL.h
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_mdk/Include/mcaa_lib.h
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_mcux/Include/mcaa_lib.h
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_mcux/Include/MCAA_EstimRL.h
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_iar/mcaa_lib.a
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_mdk/mcaa_lib.lib
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_mcux/libmcaa_lib.a
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS iar
        INCLUDES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_iar/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS mdk
        INCLUDES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_mdk/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS mcux armgcc
        INCLUDES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm33nodsp_mcux/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mcaa_cm7f)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_iar/Include/MCAA_EstimRL_FLT.h
				pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_iar/Include/MCAA_EstimBJ_FLT.h
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_iar/Include/mcaa_lib_fp.h
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_mdk/Include/MCAA_EstimRL_FLT.h
				pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_mdk/Include/MCAA_EstimBJ_FLT.h
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_mdk/Include/mcaa_lib_fp.h
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_mcux/Include/MCAA_EstimRL_FLT.h
				pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_mcux/Include/MCAA_EstimBJ_FLT.h
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_mcux/Include/mcaa_lib_fp.h
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_iar/mcaa_lib.a
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_mdk/mcaa_lib.lib
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_mcux/libmcaa_lib.a
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS iar
        INCLUDES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_iar/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS mdk
        INCLUDES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_mdk/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS mcux armgcc
        INCLUDES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm7f_mcux/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mcaa_cm4f)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_iar/Include/MCAA_EstimRL_FLT.h
				pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_iar/Include/MCAA_EstimBJ_FLT.h
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_iar/Include/mcaa_lib_fp.h
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_mdk/Include/MCAA_EstimRL_FLT.h
				pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_mdk/Include/MCAA_EstimBJ_FLT.h
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_mdk/Include/mcaa_lib_fp.h
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_mcux/Include/MCAA_EstimRL_FLT.h
				pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_mcux/Include/MCAA_EstimBJ_FLT.h
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_mcux/Include/mcaa_lib_fp.h
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_iar/mcaa_lib.a
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_mdk/mcaa_lib.lib
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_mcux/libmcaa_lib.a
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS iar
        INCLUDES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_iar/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS mdk
        INCLUDES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_mdk/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS mcux armgcc
        INCLUDES pmsm/pmsm_float/mc_algorithms/mcaa_lib/cm4f_mcux/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mcaa_cm0)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_iar/Include/MCAA_EstimRL.h
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_iar/Include/mcaa_lib.h
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_mdk/Include/MCAA_EstimRL.h
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_mdk/Include/mcaa_lib.h
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_mcux/Include/MCAA_EstimRL.h
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_mcux/Include/mcaa_lib.h
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_iar/mcaa_lib.a
        TOOLCHAINS iar
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_mdk/mcaa_lib.lib
        TOOLCHAINS mdk
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_mcux/libmcaa_lib.a
        TOOLCHAINS mcux armgcc
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS iar
        INCLUDES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_iar/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS mdk
        INCLUDES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_mdk/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        TOOLCHAINS mcux armgcc
        INCLUDES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/cm0_mcux/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mcaa_dsp56800ex)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/DSP56800EX/Include/mcaa_lib.h
        TOOLCHAINS codewarrior
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/DSP56800EX/Include/MCAA_EstimRL.h
        TOOLCHAINS codewarrior
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_frac/mc_algorithms/mcaa_lib/DSP56800EX/MCAA_LIB_SDM.lib
        TOOLCHAINS codewarrior
        TARGETS flash_sdm_lpm_debug flash_sdm_lpm_release
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_frac/mc_algorithms/mcaa_lib/DSP56800EX/MCAA_LIB_LDM.lib
        TOOLCHAINS codewarrior
        TARGETS flash_ldm_lpm_debug flash_ldm_lpm_release
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_frac/mc_algorithms/mcaa_lib/DSP56800EX/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.motor_control.mcaa_dsp56800ef)
    mcux_component_version(3.2.0)
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/dsp56800ef/Include/mcaa_lib.h
        TOOLCHAINS codewarrior
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/dsp56800ef/Include/mcaa_lib_fp.h
        TOOLCHAINS codewarrior
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/dsp56800ef/Include/MCAA_EstimRL.h
        TOOLCHAINS codewarrior
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_source(
        SOURCES pmsm/pmsm_float/mc_algorithms/mcaa_lib/dsp56800ef/Include/MCAA_EstimRL_FLT.h
        TOOLCHAINS codewarrior
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/dsp56800ef/MCAA_LIB_SDM.lib
        TOOLCHAINS codewarrior
        TARGETS flash_sdm_lpm_debug flash_sdm_lpm_release
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_library(
        LIBS pmsm/pmsm_float/mc_algorithms/mcaa_lib/dsp56800ef/MCAA_LIB_LDM.lib
        TOOLCHAINS codewarrior
        TARGETS flash_ldm_lpm_debug flash_ldm_lpm_release
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
    mcux_add_include(
        INCLUDES pmsm/pmsm_float/mc_algorithms/mcaa_lib/dsp56800ef/Include
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/
    )
endif()
