# Prototype Digital Schematic

Source: `prototype.pdf`  
Project: SMU-A / PROTOTYPE  
PDF pages: 6  
PDF creation date: 2026-07-05 09:54:47 +02:00

This file summarizes the digital prototype schematic so the PDF does not need
to be opened for every pin or component lookup.

## Pages

| Page | Sheet | Main function |
| --- | --- | --- |
| 1 | RP2040 | Pico 1 / RP2040 GPIO assignment and digital supply decoupling |
| 2 | ISO | Digital-to-analog isolation for SPI and chip-select signals |
| 3 | DAC | DAC and output setpoint buffers for voltage and current limit |
| 4 | ADC | ADC and input scaling for measured voltage and current |
| 5 | VREF | 2.5 V reference and inverted -2.5 V reference |
| 6 | INTERFACE | Power and analog interface connectors |

## Supply Nets

| Net | Domain | Notes |
| --- | --- | --- |
| `+3V3` | Digital | RP2040/Pico side and isolator digital side |
| `DGND` | Digital | Pico and digital-side ground |
| `+5V` | Analog | Isolator analog side, DAC, ADC, references |
| `AGND` | Analog | Analog-side ground |
| `+15V` | Analog | Op-amp positive supply through ferrite beads |
| `-15V` | Analog | Op-amp negative supply through ferrite beads |
| `VREF_2V5` | Analog | Positive 2.5 V reference |
| `-2V5_REF` | Analog | Inverted -2.5 V reference |

## RP2040 / Pico 1 Pin Assignment

Main component: `U36`, Pico 1 RP2040.

| Pico pin | RP2040 pin | Net | Function |
| --- | --- | --- | --- |
| 1 | `GP0` | Unused | No net shown |
| 2 | `GP1` | Unused | No net shown |
| 3 | `GND` | `DGND` | Digital ground |
| 4 | `GP2` | Unused | No net shown |
| 5 | `GP3` | Unused | No net shown |
| 6 | `GP4` | Unused | No net shown |
| 7 | `GP5` | Unused | No net shown |
| 8 | `GND` | `DGND` | Digital ground |
| 9 | `GP6` | `FAN_PWM` | Fan PWM output |
| 10 | `GP7` | `FAN_SENSE` | Fan tach/sense input, pulled up by `R76` 10 k to `+3V3` |
| 11 | `GP8` | Unused | No net shown |
| 12 | `GP9` | `BEEPER` | Beeper output |
| 13 | `GND` | `DGND` | Digital ground |
| 14 | `GP10` | `DAC_CS_N_DIG` | DAC chip-select, digital side |
| 15 | `GP11` | `SPI1_MISO_DIG` | SPI1 MISO, digital side |
| 16 | `GP12` | `ADC_CS_N_DIG` | ADC chip-select, digital side |
| 17 | `GP13` | `SPI1_CLK_DIG` | SPI1 clock, digital side |
| 18 | `GND` | `DGND` | Digital ground |
| 19 | `GP14` | `SPI1_MOSI_DIG` | SPI1 MOSI, digital side |
| 20 | `GP15` | Unused | No net shown |
| 21 | `GP16` | `SPI0_MISO_DIG` | Display SPI0 MISO |
| 22 | `GP17` | `DISP_CS_N_DIG` | Display chip-select |
| 23 | `GND` | `DGND` | Digital ground |
| 24 | `GP18` | `SPI0_CLK_DIG` | Display SPI0 clock |
| 25 | `GP19` | `SPI0_MOSI_DIG` | Display SPI0 MOSI |
| 26 | `GP20` | `DISP_PD_N_DIG` | Display power-down/reset style signal |
| 27 | `GP21` | `DISP_INT_DIG` | Display interrupt |
| 28 | `GND` | `DGND` | Digital ground |
| 29 | `GP22` | Pulled up by `R77` 10 k to `+3V3` | Net name not shown in PDF |
| 30 | `RUN` | Unused | No net shown |
| 31 | `GP26` | Unused | No net shown |
| 32 | `GP27` | Unused | No net shown |
| 33 | `GND` | `DGND` | Digital ground |
| 34 | `GP28` | Unused | No net shown |
| 35 | `ADC_VREF` | Unused | No net shown |
| 36 | `3V3(OUT)` | `+3V3` | Pico 3.3 V output |
| 37 | `3V3_EN` | Unused | No net shown |
| 38 | `GND` | `DGND` | Digital ground |
| 39 | `VSYS` | Unused | No net shown |
| 40 | `VBUS` | Unused | No net shown |
| 41 | `SWCLK` | Test/debug | Test pad shown |
| 42 | `GND` | Test/debug | Test pad shown |
| 43 | `SWDIO` | Test/debug | Test pad shown |

Decoupling on the RP2040 sheet:

| Reference | Value | Connection |
| --- | --- | --- |
| `C133` | 10 uF | `+3V3` to `DGND` |
| `C134` | 100 nF | `+3V3` to `DGND` |
| `R76` | 10 k | `FAN_SENSE` pull-up to `+3V3` |
| `R77` | 10 k | Pull-up from Pico `GP22` net to `+3V3` |

## Isolation

Digital and analog domains are isolated with two `ADUM1401WSRWZ-RL` devices.

| Reference | Part | Purpose |
| --- | --- | --- |
| `U2` | `ADUM1401WSRWZ-RL` | Isolates DAC SPI signals |
| `U181` | `ADUM1401WSRWZ-RL` | Isolates ADC chip-select signal |

### U2 - DAC SPI Isolation

| Digital-side net | Analog-side net |
| --- | --- |
| `DAC_CS_N_DIG` | `DAC_CS_N` |
| `SPI1_MOSI_DIG` | `SPI1_MOSI` |
| `SPI1_CLK_DIG` | `SPI1_CLK` |
| `SPI1_MISO_DIG` | `SPI1_MISO` |

Supply and decoupling:

| Reference | Value | Connection |
| --- | --- | --- |
| `C131` | 100 nF | `+3V3` to `DGND` |
| `C142` | 100 nF | `+5V` to `AGND` |

### U181 - ADC CS Isolation

| Digital-side net | Analog-side net |
| --- | --- |
| `ADC_CS_N_DIG` | `ADC_CS_N` |

Supply and decoupling:

| Reference | Value | Connection |
| --- | --- | --- |
| `C141` | 100 nF | `+3V3` to `DGND` |
| `C140` | 100 nF | `+5V` to `AGND` |

## DAC

| Reference | Part | Function |
| --- | --- | --- |
| `U3` | `LTC2602CMS8#TRPBF` | Dual DAC |
| `U182` | `OPA2192ID` | Dual op-amp for setpoint outputs |

### U3 - LTC2602

| Pin | Name | Net |
| --- | --- | --- |
| 1 | `CS#/LD` | `DAC_CS_N` |
| 2 | `SCK` | `SPI1_CLK` |
| 3 | `SDI` | `SPI1_MOSI` |
| 4 | `REF` | `VREF_2V5` |
| 5 | `VOUTB` | `DAC_CH1` |
| 6 | `VCC` | `+5V` |
| 7 | `GND` | `AGND` |
| 8 | `VOUTA` | `DAC_CH0` |

DAC output use:

| DAC channel | Buffer | Output net | Intended function |
| --- | --- | --- | --- |
| `DAC_CH0` | `U182.1` | `V_SET` | Voltage setpoint |
| `DAC_CH1` | `U182.2` | `I_LIMIT` | Current-limit setpoint |

DAC-related passives:

| Reference | Value | Connection / note |
| --- | --- | --- |
| `C139` | 100 nF | `+5V` to `AGND` |
| `C1` | 100 nF | `+15V` filtered rail to `AGND` near `U182.1` |
| `C3` | 100 nF | `-15V` filtered rail to `AGND` near `U182.1` |
| `FB4` | 600 R | `+15V` ferrite bead/filter |
| `FB5` | 600 R | `-15V` ferrite bead/filter |
| `R78` | 10 k | Feedback around `U182.1` |
| `R79` | 10 k | `VREF_2V5` input resistor to `U182.1` |
| `R80` | 10 k | `VREF_2V5` input resistor to `U182.2` |
| `R81` | 10 k | Feedback around `U182.2` |

## ADC

| Reference | Part | Function |
| --- | --- | --- |
| `U1` | `LTC2402CMS#TRPBF` | Dual ADC |
| `U183` | `OPA2192IDR` | Dual op-amp for input scaling |

### U1 - LTC2402

| Pin | Name | Net |
| --- | --- | --- |
| 1 | `Vcc` | `+5V` |
| 2 | `FSset` | `VREF_2V5` |
| 3 | `CH1` | `ADC_CH1` |
| 4 | `CH0` | `ADC_CH0` |
| 5 | `ZSset` | `AGND` |
| 6 | `GND` | `AGND` |
| 7 | `CS#` | `ADC_CS_N` |
| 8 | `SDO` | `SPI1_MISO` |
| 9 | `SCK` | `SPI1_CLK` |
| 10 | `F0` | `+5V` |

ADC input use:

| Measurement net | Input range shown | Scaling output | ADC channel |
| --- | --- | --- | --- |
| `V_MON` | `+/- 2,5V` | `0..2,5V` | `ADC_CH0` |
| `I_MON` | `+/- 2,5V` | `0..2,5V` | `ADC_CH1` |

ADC-related passives:

| Reference | Value | Connection / note |
| --- | --- | --- |
| `C138` | 100 nF | `+5V` to `AGND` near `U1` |
| `C143` | 100 nF | `+5V` to `AGND` near `U183` |
| `U172.1`, `U172.2`, `172.3`, `172.4`, `U174.5` | 10 k shown | `I_MON` scaling network |
| `U176.1`, `U176.2`, `173.3`, `173.4`, `U177.5` | 10 k shown | `V_MON` scaling network |

The ADC sheet also notes "10k und 5k" for the scaling networks.

## Voltage References

| Reference | Part | Function |
| --- | --- | --- |
| `U38` | `REF03GP` | Generates `VREF_2V5` from `+5V` |
| `U161` | `OP07` | Generates inverted `-2V5_REF` |

### U38 - REF03GP

| Pin | Name | Net |
| --- | --- | --- |
| 1 | `NC` | Not connected |
| 2 | `VIN` | `+5V` |
| 3 | `TEMP` | Not connected |
| 4 | `GND` | `AGND` |
| 5 | `TRIM` | Not connected |
| 6 | `VOUT` | `VREF_2V5` |
| 7 | `NC` | Not connected |
| 8 | `NC` | Not connected |

VREF-related passives:

| Reference | Value | Connection / note |
| --- | --- | --- |
| `C137` | 100 nF | `+5V` to `AGND` |
| `C85` | 100 nF | `+15V` filtered rail to `AGND` |
| `C86` | 100 nF | `-15V` filtered rail to `AGND` |
| `FB2` | 600 R | `+15V` ferrite bead/filter |
| `FB3` | 600 R | `-15V` ferrite bead/filter |
| `U47.7`, `U7.8` | 10 k shown | Reference inversion network |
| `U159`, `U160` | 1-pin connectors/test points | Reference test points shown |

## Interface

### Power Connectors

| Reference | Connector | Pins / nets |
| --- | --- | --- |
| `U46` | `PH-4A` | `+5V`, `+15V`, `AGND`, `-15V` |
| `U44` | 2-pin connector | `+3V3`, `DGND` |

### Analog Connector

| Reference | Connector | Net |
| --- | --- | --- |
| `H5` pin 13 | Analog header | `V_SET` |
| `H5` pin 11 | Analog header | `I_LIMIT` |
| `H5` pin 14 | Analog header | `V_MON` |
| `H5` pin 12 | Analog header | `I_MON` |

Pins `1` through `10` on `H5` are shown as not connected in the prototype PDF.

## Signal Flow Summary

| Direction | Path |
| --- | --- |
| Set voltage | RP2040 `SPI1` -> `U2` isolation -> `U3` DAC channel A -> `U182.1` -> `V_SET` |
| Set current limit | RP2040 `SPI1` -> `U2` isolation -> `U3` DAC channel B -> `U182.2` -> `I_LIMIT` |
| Read voltage | `V_MON` -> `U183.2` scaling -> `ADC_CH0` -> `U1` ADC -> `SPI1_MISO` -> `U2` isolation -> RP2040 |
| Read current | `I_MON` -> `U183.1` scaling -> `ADC_CH1` -> `U1` ADC -> `SPI1_MISO` -> `U2` isolation -> RP2040 |
| Display | RP2040 `SPI0` nets: `SPI0_MISO_DIG`, `SPI0_CLK_DIG`, `SPI0_MOSI_DIG`, `DISP_CS_N_DIG`, `DISP_PD_N_DIG`, `DISP_INT_DIG` |
| Fan | RP2040 `FAN_PWM` output and `FAN_SENSE` input |
| Beeper | RP2040 `BEEPER` output |
