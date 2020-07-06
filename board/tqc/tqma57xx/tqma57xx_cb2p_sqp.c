/*
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com
 *
 * Author: Felipe Balbi <balbi@ti.com>
 *
 * Based on board/ti/dra7xx/evm.c
 *
 * Copyright (C) 2017 TQ Systems (ported AM57xx IDK to TQMa57xx)
 *
 * Author: Stefan Lange <s.lange@gateware.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/omap_common.h>
#include <asm/arch/omap.h>
#include <asm/arch/dra7xx_iodelay.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/mux_dra7xx.h>
#include <spl.h>
#include <i2c.h>
#include <pca953x.h>
#include <asm/gpio.h>

#ifdef CONFIG_DRIVER_TI_CPSW
#include <cpsw.h>
#include <miiphy.h>
#endif

#define WAKEUP_ENA	(1 << 24)

const struct pad_conf_entry core_padconf_array_bb_tqma57xx[] = {
    /* swissQprint CB2+ */ 
        /* Triggers */
        {VOUT1_D6,     (M10 | PIN_INPUT_PULLUP)},                   /* vout1_d6.pr2_edc_latch1_in       F8   X1-019    TriggerIn1V8_0 */
        {VOUT1_D5,     (M10 | PIN_INPUT_PULLUP)},                   /* vout1_d5.pr2_edc_latch0_in       F9   X1-034    TriggerIn1V8_1 */
        {VIN2A_D5,     (M14 | PIN_INPUT_PULLUP)},                   /* vin2a_d5.gpio4_6                 F4   X1-004    TriggerIn0     */
        {VIN1A_D3,     (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d3.gpio3_7                 AH6  X1-112    TriggerIn1     */
        
        /* Safety */
        {VIN2A_D2,     (M14 | PIN_INPUT_PULLUP)},                   /* vin2a_d2.gpio4_3                 D1   X1-006    SafetyIn0      */
        {VIN1A_D0,     (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d0.gpio3_4                 AE8  X1-114    SafetyIn1      */
        {UART1_RXD,    (M14 | PIN_INPUT_PULLUP)},                   /* uart1_rxd.gpio7_22               B27  X1-119    SafetyIn2      */
        {UART2_RXD,    (M14 | PIN_INPUT_PULLUP)},                   /* uart2_rxd.gpio7_26               D28  X1-103    SafetyIn3      */
        {VIN1A_D4,     (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d2.gpio3_8                 AH3  X4-101    SafetyIn0      */
        {VIN1A_D5,     (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d5.gpio3_9                 AH5  X4-103    SafetyIn1      */
        {VIN1A_D6,     (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d6.gpio3_10                AG6  X4-107    SafetyIn2      */
        {VIN1A_D7,     (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d7.gpio3_11                AH4  X4-109    SafetyIn3      */
        
        /* DataOut */
        {UART1_TXD,    (M14 | PIN_INPUT_PULLUP)},                   /* uart1_txd.gpio7_23               C26  X1-111    DataOut0       */
        {VIN1A_D8,     (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d8.gpio3_12                AG4  X4-096    DataOut0       */
        {UART2_TXD,    (M14 | PIN_INPUT_PULLUP)},                   /* uart2_txd.gpio7_27               D26  X1-105    DataOut1       */
        {VIN1A_D9,     (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d9.gpio3_13                AG2  X4-100    DataOut1       */

        /* CB2+ Detection */
        {VIN1A_D2,     (M14 | PIN_INPUT)},                          /* vin1a_d2.gpio3_6                 AG7  X4-099    LinkDetect     */
        
        /* Interface_nOE */
        {VIN1A_D20,    (M14 | PIN_OUTPUT)},                         /* vin1a_d20.gpio3_24               AE2  X4-088    Interface_nOE  */

        /* HW-Version */
        {MCASP1_AXR4,  (M14 | PIN_INPUT_PULLUP)},                   /* mcasp1_axr4.gpio5_6              E12  X1-061    HW_Version0    */
        {MCASP1_AXR5,  (M14 | PIN_INPUT_PULLUP)},                   /* mcasp1_axr5.gpio5_7              F13  X1-063    HW_Version1    */
        {VIN2A_D4,     (M14 | PIN_INPUT_PULLUP)},                   /* vin2a_d4.gpio4_5                 D2   X1-050    HW_Version2    */
        {MCASP1_FSR,   (M14 | PIN_INPUT_PULLUP)},                   /* mcasp1_fsr.gpio5_1               J14  X1-052    HW_Version3    */
        
        /* PRUSS2 MII0 */
        {MCASP1_AXR6,  (M14 | PIN_INPUT_PULLUP)},                   /* mcasp1_axr6.gpio5_8              C12  X1-065    PR2_MII0_RST#  */
        {VIN2A_FLD0,   (M14 | PIN_INPUT_PULLUP)},                   /* vin2a_fld0.gpio3_30              H7   X2-042    PR2_MII0_INT#  */
        
        /* PRUSS2 MII1 */
        {MCASP1_AXR7,  (M14 | PIN_INPUT_PULLUP)},                   /* mcasp1_axr7.gpio5_9              D12  X1-067    PR2_MII1_RST#  */
        {VIN2A_HSYNC0, (M14 | PIN_INPUT_PULLUP)},                   /* vin2a_hsync0.gpio3_31            G1   X3-040    PR2_MII1_INT#  */
        
        /* SPI */
        {SPI1_SCLK,    (M14 | PIN_INPUT_PULLUP)},                   /* spi1_sclk.gpio7_7                A25  X2-077    SPI1_SCLK      */
        {SPI1_CS0,     (M14 | PIN_INPUT_PULLUP)},                   /* spi1_cs0.gpio7_10                A24  X2-079    SPI1_CS0       */
        {SPI1_D0,      (M14 | PIN_INPUT_PULLUP)},                   /* spi1_d0.gpio7_9                  B25  X2-071    SPI1_D0_MISO   */
        {SPI1_D1,      (M14 | PIN_INPUT_PULLUP)},                   /* spi1_d1.gpio7_8                  F16  X2-073    SPI1_D1_MOSI   */
        {SPI2_SCLK,    (M14 | PIN_INPUT_PULLUP)},                   /* spi2_sclk.gpio7_14               A26  X1-100    SPI2_SCLK      */
        {SPI2_CS0,     (M14 | PIN_INPUT_PULLUP)},                   /* spi2_cs0.gpio7_17                B24  X1-101    SPI2_CS0       */
        {SPI2_D1,      (M14 | PIN_INPUT_PULLUP)},                   /* spi2_d1.gpio7_15                 B22  X1-102    SPI2_D1_MOSI   */
        {SPI2_D0,      (M14 | PIN_INPUT_PULLUP)},                   /* spi2_d0.gpio7_16                 G17  X1-099    SPI2_D0_MISO   */
        
        /* I2C */
        {MCASP1_ACLKR, (M10 | PIN_INPUT_PULLUP)},                   /* mcasp1_aclkr.i2c4_sda            B14  X1-088    I2C0_SDA       */
        {MCASP4_FSX,   (M4  | PIN_INPUT_PULLUP)},                   /* mcasp4_fsx.i2c4_scl              A21  X1-090    I2C0_SCL       */
        
        /* Console */ /* see earlypadconf */
        {MCASP4_AXR0,  (M4  | PIN_INPUT)},                          /* mcasp4_axr0.uart4_rxd            G16  X1-106    RS232_RX       */
        {MCASP4_AXR1,  (M4  | PIN_OUTPUT)},                         /* mcasp4_axr1.uart4_txd            D17  X1-108    RS232_TX       */
        
        /* Led's */
        {VIN1A_HSYNC0, (M14 | PIN_OUTPUT)},                         /* vin1a_hsync0.gpio3_2             AE9  X4-085    LED_Out0       */
        {VIN1A_VSYNC0, (M14 | PIN_OUTPUT)},                         /* vin1a_vsync.gpio3_3              AF8  X4-087    LED_Out1       */
        {VIN1A_CLK0,   (M14 | PIN_OUTPUT)},                         /* vin1a_clk0.gpio2_30              AG8  X4-110    LED_Out2       */
        {VIN1B_CLK1,   (M14 | PIN_OUTPUT)},                         /* vin1a_clk1.gpio2_31              AH7  X4-112    LED_Out3       */
        
        /* Power Supply Error */
        {VIN1A_D13,    (M14 | PIN_INPUT)},                          /* vin1a_d13.gpio3_17               AF6  X4-091    nSupplyError   */
        
        /* GMAC RGMII0 */
        {VIN1A_D16,    (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d16.gpio3_20               AF1  X4-095    RGMII0_INT#    */
        {VIN1A_D18,    (M14 | PIN_OUTPUT)},                         /* vin1a_d18.gpio3_22               AE5  X4-084    RGMII0_RST#    */

        /* GMAC RGMII1 */
        {VIN1A_D17,    (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d17.gpio3_21               AE3  X4-097    RGMII1_INT#    */
        {VIN1A_D19,    (M14 | PIN_OUTPUT)},                         /* vin1a_d19.gpio3_23               AE1  X4-086    RGMII1_RST#    */
        
        /* Test */
        {VIN1A_D10,    (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d10.gpio3_14               AG3  X4-102    Test_IO0        */
        {VIN1A_D11,    (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d11.gpio3_15               AG5  X4-104    Test_IO1        */
        {VIN1A_D12,    (M14 | PIN_INPUT_PULLUP)},                   /* vin1a_d12.gpio3_16               AF2  X4-106    Test_IO2        */
        {GPMC_WAIT0,   (M14 | PIN_INPUT_PULLUP)},                   /* gpmc_wait0.gpio2_28              N2   X4-108    Test_IO3        */
        
    /* END: swissQprint CB2+ */ 
    
    /* TQM MBa57xx */
        /* GMAC MDIO */
        {VIN2A_D10,    (M3 | PIN_OUTPUT_PULLUP | SLEWCONTROL)},     /* vin2a_d10.mdio_mclk              D3   X2-019    GMAC_MDIO_MCLK */
        {VIN2A_D11,    (M3 | PIN_INPUT_PULLUP | SLEWCONTROL)},      /* vin2a_d11.mdio_d                 F6   X2-017    GMAC_MDIO_DATA */

        /* GMAC SW0 RGMII0 */
        {RGMII0_TXC,   (M0 | PIN_OUTPUT | MANUAL_MODE)},            /* rgmii0_txc.rgmii0_txc            W9   X3-061    RGMII0_TXC     */
        {RGMII0_TXCTL, (M0 | PIN_OUTPUT | MANUAL_MODE)},            /* rgmii0_txctl.rgmii0_txctl        V9   X3-063    RGMII0_TXCTL   */
        {RGMII0_TXD0,  (M0 | PIN_OUTPUT | MANUAL_MODE)},            /* rgmii0_txd0.rgmii0_txd0          U6   X3-055    RGMII0_TXD0    */
        {RGMII0_TXD1,  (M0 | PIN_OUTPUT | MANUAL_MODE)},            /* rgmii0_txd1.rgmii0_txd1          V6   x3-057    RGMII0_TXD1    */
        {RGMII0_TXD2,  (M0 | PIN_OUTPUT | MANUAL_MODE)},            /* rgmii0_txd2.rgmii0_txd2          U7   X3-051    RGMII0_TXD2    */
        {RGMII0_TXD3,  (M0 | PIN_OUTPUT | MANUAL_MODE)},            /* rgmii0_txd3.rgmii0_txd3          V7   X3-053    RGMII0_TXD3    */
        {RGMII0_RXC,   (M0 | PIN_INPUT | MANUAL_MODE)},             /* rgmii0_rxc.rgmii0_rxc            U5   X3-044    RGMII0_RXC     */
        {RGMII0_RXCTL, (M0 | PIN_INPUT | MANUAL_MODE)},             /* rgmii0_rxctl.rgmii0_rxctl        V5   X3-048    RGMII0_RXCTL   */
        {RGMII0_RXD0,  (M0 | PIN_INPUT | MANUAL_MODE)},             /* rgmii0_rxd0.rgmii0_rxd0          W2   X3-052    RGMII0_RXD0    */
        {RGMII0_RXD1,  (M0 | PIN_INPUT | MANUAL_MODE)},             /* rgmii0_rxd1.rgmii0_rxd1          Y2   X3-054    RGMII0_RXD1    */
        {RGMII0_RXD2,  (M0 | PIN_INPUT | MANUAL_MODE)},             /* rgmii0_rxd2.rgmii0_rxd2          V3   X3-058    RGMII0_RXD2    */
        {RGMII0_RXD3,  (M0 | PIN_INPUT | MANUAL_MODE)},             /* rgmii0_rxd3.rgmii0_rxd3          V4   X3-056    RGMII0_RXD3    */
        
        /* GMAC SW1 RGMII1 */
        {VIN2A_D12,    (M3 | PIN_OUTPUT | MANUAL_MODE)},            /* vin2a_d12.rgmii1_txc             D5   X2-028    RGMII1_TXC     */
        {VIN2A_D13,    (M3 | PIN_OUTPUT | MANUAL_MODE)},            /* vin2a_d13.rgmii1_txctl           C2   X2-030    RGMII1_TXCTL   */
        {VIN2A_D17,    (M3 | PIN_OUTPUT | MANUAL_MODE)},            /* vin2a_d17.rgmii1_txd0            D6   X2-033    RGMII1_TXD0    */
        {VIN2A_D16,    (M3 | PIN_OUTPUT | MANUAL_MODE)},            /* vin2a_d16.rgmii1_txd1            B2   X2-035    RGMII1_TXD1    */
        {VIN2A_D15,    (M3 | PIN_OUTPUT | MANUAL_MODE)},            /* vin2a_d15.rgmii1_txd2            C4   X2-037    RGMII1_TXD2    */
        {VIN2A_D14,    (M3 | PIN_OUTPUT | MANUAL_MODE)},            /* vin2a_d14.rgmii1_txd3            C3   X2-039    RGMII1_TXD3    */
        {VIN2A_D18,    (M3 | PIN_INPUT | MANUAL_MODE)},             /* vin2a_d18.rgmii1_rxc             C5   X2-038    RGMII1_RXC     */
        {VIN2A_D19,    (M3 | PIN_INPUT | MANUAL_MODE)},             /* vin2a_d19.rgmii1_rxctl           A3   X2-036    RGMII1_RXCTL   */
        {VIN2A_D23,    (M3 | PIN_INPUT | MANUAL_MODE)},             /* vin2a_d23.rgmii1_rxd0            A4   X2-023    RGMII1_RXD0    */
        {VIN2A_D22,    (M3 | PIN_INPUT | MANUAL_MODE)},             /* vin2a_d22.rgmii1_rxd1            B5   X2-025    RGMII1_RXD1    */
        {VIN2A_D21,    (M3 | PIN_INPUT | MANUAL_MODE)},             /* vin2a_d21.rgmii1_rxd2            B4   X2-027    RGMII1_RXD2    */
        {VIN2A_D20,    (M3 | PIN_INPUT | MANUAL_MODE)},             /* vin2a_d20.rgmii1_rxd3            B3   X2-029    RGMII1_RXD3    */
        
        /* PRUSS2 MDIO */
        {MCASP1_ACLKX, (M11 | PIN_OUTPUT)},                         /* mcasp1_aclkx.pr2_mdio_mdclk      C14  X2-046   PR2_MDIO_MDCLK */
        {MCASP1_FSX,   (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp1_fsx.pr2_mdio_data         D14  X2-044   PR2_MDIO_DATA  */

        /* PRUSS2 MII0 */
        {MCASP1_AXR13, (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp1_axr13.pr2_mii_mr0_clk     A13  X2-049   PR2_MII0_MR_CLK */
        {MCASP1_AXR14, (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp1_axr14.pr2_mii0_rxdv       G14  X2-063   PR2_MII0_RXDV   */
        {MCASP1_AXR0,  (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp1_axr0.pr2_mii0_rxer        G12  X2-051   PR2_MII0_RXER   */
        {MCASP2_AXR2,  (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp2_axr2.pr2_mii0_rxd0        C15  X2-053   PR2_MII0_RXD0   */
        {MCASP2_FSX,   (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp2_fsx.pr2_mii0_rxd1         A18  X2-055   PR2_MII0_RXD1   */
        {MCASP2_ACLKX, (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp2_aclkx.pr2_mii0_rxd2       A19  X2-057   PR2_MII0_RXD2   */
        {MCASP1_AXR15, (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp1_axr15.pr2_mii0_rxd3       F14  X2-059   PR2_MII0_RXD3   */
        {MCASP1_AXR1,  (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp1_axr1.pr2_mii_mt0_clk      F12  X2-050   PR2_MII0_MT_CLK */
        {MCASP1_AXR8,  (M11 | PIN_OUTPUT | SLEWCONTROL)},           /* mcasp1_axr8.pr2_mii0_txen        B12  X2-052   PR2_MII0_TXEN   */
        {MCASP1_AXR12, (M11 | PIN_OUTPUT | SLEWCONTROL)},           /* mcasp1_axr12.pr2_mii0_txd0       E14  X2-054   PR2_MII0_TXD0   */
        {MCASP1_AXR11, (M11 | PIN_OUTPUT | SLEWCONTROL)},           /* mcasp1_axr11.pr2_mii0_txd1       A12  X2-058   PR2_MII0_TXD1   */
        {MCASP1_AXR10, (M11 | PIN_OUTPUT | SLEWCONTROL)},           /* mcasp1_axr10.pr2_mii0_txd2       B13  X2-060   PR2_MII0_TXD2   */
        {MCASP1_AXR9,  (M11 | PIN_OUTPUT | SLEWCONTROL)},           /* mcasp1_axr9.pr2_mii0_txd3        A11  X2-062   PR2_MII0_TXD3   */
        {MCASP3_ACLKX, (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp3_aclkx.pr2_mii0_crs        B18  X2-066   PR2_MII0_CRS    */
        {MCASP3_FSX,   (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp3_fsx.pr2_mii0_col          F15  X2-064   PR2_MII0_COL    */
        
        /* PRUSS2 MII1 */
        {MMC3_DAT2,    (M11 | PIN_INPUT | SLEWCONTROL)},            /* mmc3_dat2.pr2_mii_mr1_clk        AC9  X3-073   PR2_MII1_MR_CLK */
        {MMC3_DAT3,    (M11 | PIN_INPUT | SLEWCONTROL)},            /* mmc3_dat3.pr2_mii1_rxdv          AC3  X3-075   PR2_MII1_RXDV   */
        {MCASP3_AXR0,  (M11 | PIN_INPUT | SLEWCONTROL)},            /* mcasp3_axr0.pr2_mii1_rxer        B19  X3-043   PR2_MII1_RXER   */
        {MMC3_DAT7,    (M11 | PIN_INPUT | SLEWCONTROL)},            /* mmc3_dat7.pr2_mii1_rxd0          AB5  X3-062   PR2_MII1_RXD0   */
        {MMC3_DAT6,    (M11 | PIN_INPUT | SLEWCONTROL)},            /* mmc3_dat6.pr2_mii1_rxd1          AB8  X3-064   PR2_MII1_RXD1   */
        {MMC3_DAT5,    (M11 | PIN_INPUT | SLEWCONTROL)},            /* mmc3_dat5.pr2_mii1_rxd2          AD6  X3-066   PR2_MII1_RXD2   */
        {MMC3_DAT4,    (M11 | PIN_INPUT | SLEWCONTROL)},            /* mmc3_dat4.pr2_mii1_rxd3          AC8  X3-068   PR2_MII1_RXD3   */
        {GPIO6_10,     (M11 | PIN_INPUT | SLEWCONTROL)},            /* gpio6_10.pr2_mii_mt1_clk         AC5  X3-077   PR2_MII1_MT_CLK */
        {GPIO6_11,     (M11 | PIN_OUTPUT | SLEWCONTROL)},           /* gpio6_11.pr2_mii1_txen           AB4  X3-070   PR2_MII1_TXEN   */
        {MMC3_DAT1,    (M11 | PIN_OUTPUT | SLEWCONTROL)},           /* mmc3_dat1.pr2_mii1_txd0          AC6  X3-072   PR2_MII1_TXD0   */
        {MMC3_DAT0,    (M11 | PIN_OUTPUT | SLEWCONTROL)},           /* mmc3_dat0.pr2_mii1_txd1          AC7  X3-074   PR2_MII1_TXD1   */
        {MMC3_CMD,     (M11 | PIN_OUTPUT | SLEWCONTROL)},           /* mmc3_cmd.pr2_mii1_txd2           AC4  X3-076   PR2_MII1_TXD2   */
        {MMC3_CLK,     (M11 | PIN_OUTPUT | SLEWCONTROL)},           /* mmc3_clk.pr2_mii1_txd3           AD4  X3-078   PR2_MII1_TXD3   */
        {XREF_CLK1,    (M11 | PIN_INPUT | SLEWCONTROL)},            /* xref_clk1.pr2_mii1_crs           E17  X2-072   PR2_MII1_CRS    */
        {XREF_CLK0,    (M11 | PIN_INPUT | SLEWCONTROL)},            /* xref_clk0.pr2_mii1_col           D18  X2-074   PR2_MII1_COL    */

        /* MMC1: sd card */
        {MMC1_CLK,     (M0  | PIN_INPUT_PULLUP)},                   /* mmc1_clk.mmc1_clk                W6   X4-069   MMC1_CLK        */
        {MMC1_CMD,     (M0  | PIN_INPUT_PULLUP)},                   /* mmc1_cmd.mmc1_cmd                Y6   X4-071   MMC1_CMD        */
        {MMC1_DAT0,    (M0  | PIN_INPUT_PULLUP)},                   /* mmc1_dat0.mmc1_dat0              AA6  X4-073   MMC1_DAT0       */
        {MMC1_DAT1,    (M0  | PIN_INPUT_PULLUP)},                   /* mmc1_dat1.mmc1_dat1              Y4   X4-075   MMC1_DAT1       */
        {MMC1_DAT2,    (M0  | PIN_INPUT_PULLUP)},                   /* mmc1_dat2.mmc1_dat2              AA5  X4-077   MMC1_DAT2       */ 
        {MMC1_DAT3,    (M0  | PIN_INPUT_PULLUP)},                   /* mmc1_dat3.mmc1_dat3              Y3   X4-079   MMC1_DAT3       */
        {MMC1_SDCD,    (M15 | PIN_INPUT | SLEWCONTROL)},            /* mmc1_sdcd.gpio6_27               W7   X4-081   MMC1_CD#        */  // ???? Why not M0
        {MMC1_SDWP,    (M15 | PIN_INPUT | SLEWCONTROL)},            /* mmc1_sdwp.gpio6_28               Y9   X4-065   MMC1_WP         */  // ???? Why not M0
    /* END: TQM MBa57xx */

        
//    {MCASP3_AXR1, (M11 | PIN_INPUT | SLEWCONTROL)},             /* mcasp3_axr1.pr2_mii1_rxlink */
//    {RMII_MHZ_50_CLK, (M0 | PIN_OUTPUT | MANUAL_MODE)},         /* RMII_MHZ_50_CLK.rmii_mhz_50_clk */
//    {MCASP2_AXR3, (M11 | PIN_INPUT | SLEWCONTROL)},             /* mcasp2_axr3.pr2_mii0_rxlink */

    /* UART */
//	{VIN1A_D1, (M5 | PIN_OUTPUT)},	/* vin1a_d1.uart8_txd */
//	{VIN2A_D3, (M8 | PIN_OUTPUT)},	/* vin2a_d3.uart10_txd */

	/* DCAN */
//	{GPIO6_14, (M15 | PULL_UP)},  /* gpio6_14.safe for dcan2_tx */
//	{GPIO6_15, (M15 | PULL_UP)},  /* gpio6_15.safe for dcan2_rx */
//	{DCAN1_TX, (M15 | PULL_UP)},    /* dcan1_tx.safe for dcan1_tx */
//	{DCAN1_RX, (M15 | PULL_UP)},    /* dcan1_rx.safe for dcan1_rx */

//	/* I2C */
//	{MCASP5_ACLKX, (M4 | PIN_INPUT)},       /* mcasp5_aclkx.i2c5_sda */
//	{MCASP5_FSX, (M4 | PIN_INPUT)},  /* mcasp5_fsx.i2c5_scl */
	/* I2C1: see earlypadconf */

	/* multichannel audio serial port */
//	{XREF_CLK3, (M4 | PIN_INPUT)},        /* xref_clk3.mcasp8_ahclkx */
//	{MCASP2_ACLKR, (M1 | PIN_INPUT)},     /* mcasp2_aclkr.mcasp8_axr2 */
//	{MCASP2_FSR, (M1 | PIN_INPUT)},     /* mcasp2_fsr.mcasp8_axr3 */
//	{MCASP2_AXR0, (M0 | PIN_INPUT)},     /* mcasp2_axr0.mcasp2_axr0 */
//	{MCASP2_AXR4, (M1 | PIN_INPUT)},      /* mcasp2_axr4.mcasp8_axr0 */
//	{MCASP2_AXR5, (M1 | PIN_INPUT)},      /* mcasp2_axr5.mcasp8_axr1 */
//	{MCASP2_AXR6, (M1 | PIN_INPUT)},      /* mcasp2_axr6.mcasp8_aclkx */
//	{MCASP2_AXR7, (M1 | PIN_INPUT)},      /* mcasp2_axr7.mcasp8_fsx */

	/* SPI: to display connector */
//	{MCASP4_ACLKX, (M2 | PIN_INPUT)},       /* mcasp4_aclkx.spi3_sclk */
//	{MCASP4_AXR1, (M2 | PIN_INPUT_PULLUP)}, /* mcasp4_axr1.spi3_cs0 */

	/* HDMI */
//	{SPI1_CS2, (M6 | PIN_INPUT_PULLUP)},     /* spi1_cs2.hdmi1_hpd */
//	{SPI1_CS3, (M6 | PIN_INPUT_PULLUP)},      /* spi1_cs3.hdmi1_cec */
//	{I2C2_SDA, (M1 | PIN_INPUT_PULLUP)},   /* i2c2_sda.hdmi1_ddc_scl */
//	{I2C2_SCL, (M1 | PIN_INPUT_PULLUP)},   /* i2c2_scl.hdmi1_ddc_sda */

	/* MMC4: to pin header */
//	{UART2_CTSN, (M3 | PIN_INPUT_PULLUP | MANUAL_MODE)},        /* uart2_ctsn.mmc4_dat2 */
//	{UART2_RTSN, (M3 | PIN_INPUT_PULLUP | MANUAL_MODE)},        /* uart2_rtsn.mmc4_dat3 */
//	{UART1_CTSN, (M3 | PIN_INPUT_PULLUP | MANUAL_MODE)},        /* uart1_ctsn.mmc4_clk */
//	{UART1_RTSN, (M3 | PIN_INPUT_PULLUP | MANUAL_MODE)},        /* uart1_rtsn.mmc4_cmd */

    /* swissQprint */
	/* video out */
//	{VOUT1_CLK, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_clk.vout1_clk */
//	{VOUT1_DE, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_de.vout1_de */
//	{VOUT1_FLD, (M14 | PIN_OUTPUT)},        /* vout1_fld.gpio4_21 */
//	{VOUT1_HSYNC, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},   /* vout1_hsync.vout1_hsync */
//	{VOUT1_VSYNC, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},   /* vout1_vsync.vout1_vsync */
//	{VOUT1_D0, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_d0.vout1_d0 */
//	{VOUT1_D1, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_d1.vout1_d1 */
//	{VOUT1_D2, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_d2.vout1_d2 */
//	{VOUT1_D3, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_d3.vout1_d3 */
//	{VOUT1_D4, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_d4.vout1_d4 */
//	{VOUT1_D5, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_d5.vout1_d5 */
//	{VOUT1_D6, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_d6.vout1_d6 */
//	{VOUT1_D7, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_d7.vout1_d7 */
//	{VOUT1_D8, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_d8.vout1_d8 */
//	{VOUT1_D9, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},      /* vout1_d9.vout1_d9 */
//	{VOUT1_D10, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d10.vout1_d10 */
//	{VOUT1_D11, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d11.vout1_d11 */
//	{VOUT1_D12, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d12.vout1_d12 */
//	{VOUT1_D13, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d13.vout1_d13 */
//	{VOUT1_D14, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d14.vout1_d14 */
//	{VOUT1_D15, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d15.vout1_d15 */
//	{VOUT1_D16, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d16.vout1_d16 */
//	{VOUT1_D17, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d17.vout1_d17 */
//	{VOUT1_D18, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d18.vout1_d18 */
//	{VOUT1_D19, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d19.vout1_d19 */
//	{VOUT1_D20, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d20.vout1_d20 */
//	{VOUT1_D21, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d21.vout1_d21 */
//	{VOUT1_D22, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d22.vout1_d22 */
//	{VOUT1_D23, (M0 | PIN_OUTPUT | SLEWCONTROL | MANUAL_MODE)},     /* vout1_d23.vout1_d23 */

	/* div */
//	{NMIN_DSP, (M0 | PIN_INPUT)},   /* nmin_dsp.nmin_dsp */

	/* USB */
//	{USB1_DRVVBUS, (M0 | PIN_OUTPUT | SLEWCONTROL)},        /* usb1_drvvbus.usb1_drvvbus */
//	{USB2_DRVVBUS, (M0 | PIN_OUTPUT | SLEWCONTROL)},        /* usb2_drvvbus.usb2_drvvbus */

	/* GPIO */
//	{VIN1A_D14, (M14 | PIN_INPUT)}, /* vin1a_d14.gpio3_18 */
//	{VIN1A_D15, (M14 | PIN_INPUT)}, /* vin1a_d15.gpio3_19 */
//	{VIN1A_D21, (M14 | PIN_INPUT)}, /* vin1a_d21.gpio3_25 */
//	{VIN1A_D22, (M14 | PIN_INPUT)}, /* vin1a_d22.gpio3_26 */
//	{VIN1A_D23, (M14 | PIN_INPUT)}, /* vin1a_d23.gpio3_27 */
//	{VIN1A_DE0, (M14 | PIN_INPUT_PULLUP)}, /* vin1a_de0.gpio3_0 */
//	{VIN1A_FLD0, (M14 | PIN_INPUT)},	/* vin1a_fld0.gpio3_1 */
//	{VIN2A_CLK0, (M14 | PIN_INPUT)},	/* vin2a_clk0.gpio3_28 */
//	{VIN2A_DE0, (M14 | PIN_INPUT)},		/* vin2a_de0.gpio3_29 */
//	{VIN2A_VSYNC0, (M14 | PIN_INPUT)},      /* vin2a_vsync0.gpio4_0 */
//	{VIN2A_D0, (M14 | PIN_INPUT)},  /* vin2a_d0.gpio4_1 */
//	{VIN2A_D1, (M11 | PIN_INPUT)}, /* vin2a_d1.gpio4_2 */
//	{VIN2A_D6, (M14 | PIN_INPUT)},   /* vin2a_d6.gpio4_7 */
//	{VIN2A_D7, (M14 | PIN_INPUT)},  /* vin2a_d7.gpio4_8 */
//	{VIN2A_D8, (M14 | PIN_INPUT)},  /* vin2a_d8.gpio4_9_ */
//	{VIN2A_D9, (M14 | PIN_INPUT)},  /* vin2a_d9.gpio4_10 */
//	{GPIO6_16, (M0 | PIN_INPUT)},    /* gpio6_16.gpio6_16 */
//	{XREF_CLK2, (M14 | PIN_INPUT)},        /* xref_clk2.gpio6_19 */
//	{MCASP1_AXR2, (M14 | PIN_INPUT)},       /* mcasp1_axr2.gpio5_4 */
//	{MCASP1_AXR3, (M14 | PIN_INPUT)},       /* mcasp1_axr3.gpio5_5 */
//	{WAKEUP1, (M14 | PIN_INPUT)},    /* Wakeup1.gpio1_1 */
//	{WAKEUP2, (M14 | PIN_INPUT)},    /* Wakeup2.gpio1_2 */
//	{WAKEUP3, (M14 | PIN_INPUT)},    /* Wakeup3.gpio1_3 */
//	{GPMC_AD0, (M14 | PIN_INPUT)},    /* gpmc_ad0.gpio1_6 */
//	{GPMC_AD1, (M14 | PIN_INPUT)},    /* gpmc_ad1.gpio1_7 */
//	{GPMC_AD2, (M14 | PIN_INPUT)},    /* gpmc_ad2.gpio1_8 */
//	{GPMC_AD3, (M14 | PIN_INPUT)},    /* gpmc_ad3.gpio1_9 */
//	{GPMC_AD4, (M14 | PIN_INPUT)},    /* gpmc_ad4.gpio1_10 */
//	{GPMC_AD5, (M14 | PIN_INPUT)},    /* gpmc_ad5.gpio1_11*/
//	{GPMC_AD6, (M14 | PIN_INPUT)},    /* gpmc_ad6.gpio1_12 */
//	{GPMC_AD7, (M14 | PIN_INPUT)},    /* gpmc_ad7.gpio1_13 */
//	{GPMC_AD8, (M14 | PIN_INPUT)},    /* gpmc_ad8.gpio7_18 */
//	{GPMC_AD9, (M14 | PIN_INPUT)},    /* gpmc_ad9.gpio7_19 */
//	{GPMC_AD10, (M14 | PIN_INPUT)},    /* gpmc_ad10.gpio7_28 */
//	{GPMC_AD11, (M14 | PIN_INPUT)},    /* gpmc_ad11.gpio7_29 */
//	{GPMC_AD12, (M14 | PIN_INPUT)},    /* gpmc_ad12.gpio1_18 */
//	{GPMC_AD13, (M14 | PIN_INPUT)},    /* gpmc_ad13.gpio1_19 */
//	{GPMC_AD14, (M14 | PIN_INPUT)},    /* gpmc_ad14.gpio1_20 */
//	{GPMC_AD15, (M14 | PIN_INPUT)},    /* gpmc_ad15.gpio1_21 */
//	{GPMC_A0, (M14 | PIN_INPUT)},      /* gpmc_a0.gpio7_3 */
//	{GPMC_A1, (M14 | PIN_INPUT)},      /* gpmc_a1.gpio7_4 */
//	{GPMC_A2, (M14 | PIN_INPUT)},      /* gpmc_a2.gpio7_5 */
//	{GPMC_A3, (M14 | PIN_INPUT)},      /* gpmc_a3.gpio7_6 */
//	{GPMC_A4, (M14 | PIN_INPUT)},    /* gpmc_a4.gpio1_26 */
//	{GPMC_A5, (M14 | PIN_INPUT)},    /* gpmc_a5.gpio1_27 */
//	{GPMC_A6, (M14 | PIN_INPUT)},    /* gpmc_a6.gpio1_28 */
//	{GPMC_A7, (M14 | PIN_INPUT)},    /* gpmc_a7.gpio1_29 */
//	{GPMC_A8, (M14 | PIN_INPUT)},    /* gpmc_a8.gpio1_30 */
//	{GPMC_A9, (M14 | PIN_INPUT)},    /* gpmc_a9.gpio1_31 */
//	{GPMC_A10, (M14 | PIN_INPUT)},    /* gpmc_a10.gpio2_0 */
//	{GPMC_A11, (M14 | PIN_INPUT)},    /* gpmc_a11.gpio2_1 */
//	{GPMC_A12, (M14 | PIN_INPUT | WAKEUP_ENA)},    /* gpmc_a12.gpio2_2 I2C GPIO exp int*/
//	{GPMC_CS0, (M14 | PIN_INPUT)},    /* gpmc_cs0.gpio2_19 */
//	{GPMC_CLK, (M14 | PIN_INPUT)},    /* gpmc_clk.gpio2_22 */
//	{GPMC_ADVN_ALE, (M14 | PIN_INPUT)},    /* gpmc_advn_ale.gpio2_23 */
//	{GPMC_OEN_REN, (M14 | PIN_INPUT)},    /* gpmc_oen_ren.gpio2_24 */
//	{GPMC_WEN, (M14 | PIN_INPUT)},    /* gpmc_wen.gpio2_25 */
//	{GPMC_BEN0, (M14 | PIN_INPUT)},    /* gpmc_ben0.gpio2_26 */
//	{GPMC_BEN1, (M14 | PIN_INPUT)},    /* gpmc_ben1.gpio2_27 */
//	{MDIO_MCLK, (M14 | PIN_INPUT_SLEW)},     /* mdio_mclk.gpio5_15 */
//	{MDIO_D, (M14 | PIN_INPUT_SLEW)},       /* mdio_d.gpio5_16 */
//	{UART3_RXD, (M14 | PIN_INPUT_SLEW)},    /* uart3_rxd.gpio5_18 */
//	{UART3_TXD, (M14 | PIN_INPUT_SLEW)},    /* uart3_txd.gpio5_19 */

	/* PRU GPIO */
//	{MCASP5_AXR0, (M13 | PIN_OUTPUT | MANUAL_MODE)},/* mcasp5_axr0.pr2_pru1_gpo3 */
//	{MCASP5_AXR1, (M13 | PIN_OUTPUT | MANUAL_MODE)},/* mcasp5_axr1.pr2_pru1_gpo4 */
};

#ifdef CONFIG_IODELAY_RECALIBRATION
const struct iodelay_cfg_entry iodelay_cfg_array_bb_tqma57xx[] = {
	{0x0030, 0,	0     }, /* CFG_RMII_MHZ_50_CLK_IN  U3 */
	{0x059C, 1000,	3100  }, /* CFG_MCASP5_AXR0_OUT       AB3 */
	{0x05A8, 1000,	2700  }, /* CFG_MCASP5_AXR1_OUT       AA4 */
	{0x0714, 139,	1081  }, /* CFG_RGMII0_RXD1_IN        Y2 */
	{0x0720, 195,	1100  }, /* CFG_RGMII0_RXD2_IN        V3 */
	{0x072C, 239,	1216  }, /* CFG_RGMII0_RXD3_IN        V4 */
	{0x0708, 123,	1047  }, /* CFG_RGMII0_RXD0_IN        W2 */
	{0x0758, 339,	162   }, /* CFG_RGMII0_TXD0_OUT       U6 */
	{0x0764, 146,	94    }, /* CFG_RGMII0_TXD1_OUT       V6 */
	{0x0770, 0,	27    }, /* CFG_RGMII0_TXD2_OUT       U7 */
	{0x077C, 291,	205   }, /* CFG_RGMII0_TXD3_OUT       V7 */
	{0x0740, 89,	0     }, /* CFG_RGMII0_TXC_OUT        W9 */
	{0x074C, 15,	125   }, /* CFG_RGMII0_TXCTL_OUT      V9 */
	{0x06F0, 260,	0     }, /* CFG_RGMII0_RXC_IN U5 */
	{0x06FC, 0,	1412  }, /* CFG_RGMII0_RXCTL_IN       V5 */
	{0x0AEC, 294,	669   }, /* CFG_VIN2A_D22_IN  B5 */
	{0x0AE0, 192,	836   }, /* CFG_VIN2A_D21_IN  B4 */
	{0x0AD4, 320,	750   }, /* CFG_VIN2A_D20_IN  B3 */
	{0x0AF8, 50,	700   }, /* CFG_VIN2A_D23_IN  A4 */
	{0x0AAC, 78,	27    }, /* CFG_VIN2A_D17_OUT D6 */
	{0x0AA0, 154,	101   }, /* CFG_VIN2A_D16_OUT B2 */
	{0x0A94, 135,	100   }, /* CFG_VIN2A_D15_OUT C4 */
	{0x0A88, 92,	58    }, /* CFG_VIN2A_D14_OUT C3 */
	{0x0A70, 0,	0     }, /* CFG_VIN2A_D12_OUT D5 */
	{0x0A7C, 219,	101   }, /* CFG_VIN2A_D13_OUT C2 */
	{0x0AB0, 411,	0     }, /* CFG_VIN2A_D18_IN  C5 */
	{0x0ABC, 0,	382   }, /* CFG_VIN2A_D19_IN  A3 */
	{0x0840, 0,	0     }, /* CFG_UART1_CTSN_IN E25 */
	{0x0848, 0,	0     }, /* CFG_UART1_CTSN_OUT        E25 */
	{0x084C, 307,	0     }, /* CFG_UART1_RTSN_IN C27 */
	{0x0850, 0,	0     }, /* CFG_UART1_RTSN_OEN        C27 */
	{0x0854, 0,	0     }, /* CFG_UART1_RTSN_OUT        C27 */
	{0x0888, 683,	0     }, /* CFG_UART2_RXD_IN  D28 */
	{0x088C, 0,	0     }, /* CFG_UART2_RXD_OEN D28 */
	{0x0890, 0,	0     }, /* CFG_UART2_RXD_OUT D28 */
	{0x0894, 835,	0     }, /* CFG_UART2_TXD_IN  D26 */
	{0x0898, 0,	0     }, /* CFG_UART2_TXD_OEN D26 */
	{0x089C, 0,	0     }, /* CFG_UART2_TXD_OUT D26 */
	{0x0870, 785,	0     }, /* CFG_UART2_CTSN_IN D27 */
	{0x0874, 0,	0     }, /* CFG_UART2_CTSN_OEN        D27 */
	{0x0878, 0,	0     }, /* CFG_UART2_CTSN_OUT        D27 */
	{0x087C, 613,	0     }, /* CFG_UART2_RTSN_IN C28 */
	{0x0880, 0,	0     }, /* CFG_UART2_RTSN_OEN        C28 */
	{0x0884, 0,	0     }, /* CFG_UART2_RTSN_OUT        C28 */
//	{0x0CEC, 139,	701   }, /* CFG_VOUT1_VSYNC_OUT       E11 */
//	{0x0CE0, 0,	0     }, /* CFG_VOUT1_HSYNC_OUT       C11 */
//	{0x0B9C, 1126,	751   }, /* CFG_VOUT1_CLK_OUT D11 */
//	{0x0CC8, 0,	0     }, /* CFG_VOUT1_DE_OUT  B10 */
//	{0x0BA8, 395,	0     }, /* CFG_VOUT1_D0_OUT  F11 */
//	{0x0C2C, 521,	0     }, /* CFG_VOUT1_D1_OUT  G10 */
//	{0x0C68, 282,	0     }, /* CFG_VOUT1_D2_OUT  F10 */
//	{0x0C74, 438,	0     }, /* CFG_VOUT1_D3_OUT  G11 */
//	{0x0C80, 1298,	0     }, /* CFG_VOUT1_D4_OUT  E9 */
//	{0x0C8C, 397,	0     }, /* CFG_VOUT1_D5_OUT  F9 */
//	{0x0C98, 321,	0     }, /* CFG_VOUT1_D6_OUT  F8 */
//	{0x0CA4, 155,	309   }, /* CFG_VOUT1_D7_OUT  E7 */
//	{0x0CB0, 212,	0     }, /* CFG_VOUT1_D8_OUT  E8 */
//	{0x0CBC, 466,	0     }, /* CFG_VOUT1_D9_OUT  D9 */
//	{0x0BB4, 282,	0     }, /* CFG_VOUT1_D10_OUT D7 */
//	{0x0BC0, 348,	0     }, /* CFG_VOUT1_D11_OUT D8 */
//	{0x0BCC, 1240,	0     }, /* CFG_VOUT1_D12_OUT A5 */
//	{0x0BD8, 182,	0     }, /* CFG_VOUT1_D13_OUT C6 */
//	{0x0BE4, 311,	0     }, /* CFG_VOUT1_D14_OUT C8 */
//	{0x0BF0, 285,	0     }, /* CFG_VOUT1_D15_OUT C7 */
//	{0x0BFC, 166,	0     }, /* CFG_VOUT1_D16_OUT B7 */
//	{0x0C08, 278,	0     }, /* CFG_VOUT1_D17_OUT B8 */
//	{0x0C14, 425,	0     }, /* CFG_VOUT1_D18_OUT A7 */
//	{0x0C20, 516,	0     }, /* CFG_VOUT1_D19_OUT A8 */
//	{0x0C38, 386,	0     }, /* CFG_VOUT1_D20_OUT C9 */
//	{0x0C44, 111,	0     }, /* CFG_VOUT1_D21_OUT A9 */
//	{0x0C50, 227,	0     }, /* CFG_VOUT1_D22_OUT B9 */
//	{0x0C5C, 0,	0     }, /* CFG_VOUT1_D23_OUT A10 */
//	{0x09EC, 0,	0     }, /* CFG_VIN1A_D8_OUT  AG4 */
//	{0x09F8, 0,	0     }, /* CFG_VIN1A_D9_OUT  AG2 */
//	{0x08F0, 0,	600   }, /* CFG_VIN1A_D10_OUT AG3 */
//	{0x08FC, 0,	0     }, /* CFG_VIN1A_D11_OUT AG5 */
//	{0x0908, 0,	2700  }, /* CFG_VIN1A_D12_OUT AF2 */
//	{0x09B4, 0,	0     }, /* CFG_VIN1A_D4_IN   AH3 */
//	{0x09C0, 0,	900   }, /* CFG_VIN1A_D5_IN   AH5 */
//	{0x09CC, 0,	400   }, /* CFG_VIN1A_D6_IN   AG6 */
//	{0x09D8, 0,	500   }, /* CFG_VIN1A_D7_IN   AH4 */
};
#endif

const char *tqma57xx_bb_get_boardname(void)
{
	return "CB2+";
}

#ifdef CONFIG_IODELAY_RECALIBRATION
int tqma57xx_bb_recalibrate_iodelay(void)
{
	const struct pad_conf_entry *bb_pconf;
	const struct iodelay_cfg_entry *bb_iod;
	int bb_pconf_sz, bb_iod_sz;
	int ret;

	bb_pconf = core_padconf_array_bb_tqma57xx;
	bb_pconf_sz = ARRAY_SIZE(core_padconf_array_bb_tqma57xx);
	bb_iod = iodelay_cfg_array_bb_tqma57xx;
	bb_iod_sz = ARRAY_SIZE(iodelay_cfg_array_bb_tqma57xx);

	/* setup pad configuration and muxing */
	do_set_mux32((*ctrl)->control_padconf_core_base, bb_pconf, bb_pconf_sz);

	/* setup IOdelay configuration */
	ret = do_set_iodelay((*ctrl)->iodelay_config_base, bb_iod, bb_iod_sz);

	return ret;
}
#endif

#if defined(CONFIG_MMC)
int tqma57xx_bb_board_mmc_init(bd_t *bis)
{
	/* MMC1: sd card */
	omap_mmc_init(0, 0, 0, -1, -1);
	return 0;
}
#endif /* CONFIG_MMC */

/*Define for building port exp gpio, pin starts from 0*/
#define PORTEXP_IO_NR(chip, pin) \
	((chip << 5) + pin)

/*Get the chip addr from a ioexp gpio*/
#define PORTEXP_IO_TO_CHIP(gpio_nr) \
	(gpio_nr >> 5)

/*Get the pin number from a ioexp gpio*/
#define PORTEXP_IO_TO_PIN(gpio_nr) \
	(gpio_nr & 0x1f)

#define PORTEXP_I2C_BUS_NR	3

static int port_exp_direction_output(unsigned int gpio, int value)
{
	int ret;

	i2c_set_bus_num(PORTEXP_I2C_BUS_NR);

	ret = i2c_probe(PORTEXP_IO_TO_CHIP(gpio));
	if (ret)
		return ret;

	ret = pca953x_set_dir(PORTEXP_IO_TO_CHIP(gpio),
			      (1 << PORTEXP_IO_TO_PIN(gpio)),
			      (PCA953X_DIR_OUT << PORTEXP_IO_TO_PIN(gpio)));

	if (ret)
		return ret;

	ret = pca953x_set_val(PORTEXP_IO_TO_CHIP(gpio),
			      (1 << PORTEXP_IO_TO_PIN(gpio)),
			      (value << PORTEXP_IO_TO_PIN(gpio)));

	if (ret)
		return ret;

	return 0;
}

#if defined(CONFIG_USB_DWC3) || defined(CONFIG_USB_XHCI_OMAP)
#define PWR_EN_1V1     PORTEXP_IO_NR(0x21, 2)

int tqma57xx_bb_board_usb_init(void)
{
	int ret;

	/* enable 1V1 voltage rail for usb hub */
	ret = port_exp_direction_output(PWR_EN_1V1, 1);

	if (ret)
		printf("Error %d enabling 1V1 voltage rail\n", ret);

	return ret;
}
#endif

#ifdef CONFIG_DRIVER_TI_CPSW
static void cpsw_control(int enabled)
{
	/* VTP can be added here */
}

static struct cpsw_slave_data tqma57xx_cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,
		.phy_addr	= 2,
		.phy_if         = PHY_INTERFACE_MODE_RGMII,
	},
	{
		.slave_reg_ofs	= 0x308,
		.sliver_reg_ofs	= 0xdc0,
		.phy_addr	= 3,
		.phy_if         = PHY_INTERFACE_MODE_RGMII,
	},
};

static struct cpsw_platform_data tqma57xx_cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x800,
	.slaves			= 2,
	.slave_data		= tqma57xx_cpsw_slaves,
	.ale_reg_ofs		= 0xd00,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x108,
	.hw_stats_reg_ofs	= 0x900,
	.bd_ram_ofs		= 0x2000,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_2,
};

#define CB2p_AM57XX_GPIO_GBIT_PHY0_RST GPIO_TO_PIN(3, 22)
#define CB2p_AM57XX_GPIO_GBIT_PHY1_RST GPIO_TO_PIN(3, 23)
#define CB2p_AM57XX_GPIO_PRU_PHY0_RST GPIO_TO_PIN(5, 8)
#define CB2p_AM57XX_GPIO_PRU_PHY1_RST GPIO_TO_PIN(5, 9)

int tqma57xx_bb_board_eth_init(bd_t *bis)
{
	int ret;
	u32 ctrl_val;

	/* set GMII 1,2 selection setting to RGMII */
	ctrl_val = readl((*ctrl)->control_core_control_io1) & (~0x33);
	ctrl_val |= 0x22;
	writel(ctrl_val, (*ctrl)->control_core_control_io1);
	mdelay(10);
    
//	gpio_request( CB2p_AM57XX_GPIO_GBIT_PHY0_RST, "RGMII0_RST" );
//	gpio_request( CB2p_AM57XX_GPIO_GBIT_PHY1_RST, "RGMII1_RST" );
//	gpio_direction_output( CB2p_AM57XX_GPIO_GBIT_PHY0_RST, 0 );
//	gpio_direction_output(C B2p_AM57XX_GPIO_GBIT_PHY1_RST, 0 );
//	mdelay(20);
//
//	gpio_set_value( CB2p_AM57XX_GPIO_GBIT_PHY0_RST, 1 );
//	gpio_set_value( CB2p_AM57XX_GPIO_GBIT_PHY1_RST, 1 );
//	mdelay(20);


	ret = cpsw_register(&tqma57xx_cpsw_data);
	if (ret < 0)
		printf("Error %d registering CPSW switch\n", ret);

	const char *devname;

	devname = miiphy_get_current_dev();

	/* emac0 phy: set rgmii driverstrength to max. value */
	miiphy_write(devname, 0x2, 0xd, 0x1f);
	miiphy_write(devname, 0x2, 0xe, 0x0170);
	miiphy_write(devname, 0x2, 0xd, 0x401f);
	miiphy_write(devname, 0x2, 0xe, 0x001f);

	/* emac0 phy: activate rgmii delay */
	miiphy_write(devname, 0x2, 0xd, 0x1f);
	miiphy_write(devname, 0x2, 0xe, 0x32);
	miiphy_write(devname, 0x2, 0xd, 0x401f);
	miiphy_write(devname, 0x2, 0xe, 0xd3);

	/* emac0 phy: set rgmii delay
	 * 0.25ns tx delay, 2.75ns rs delay
	 */
	miiphy_write(devname, 0x2, 0xd, 0x1f);
	miiphy_write(devname, 0x2, 0xe, 0x86);
	miiphy_write(devname, 0x2, 0xd, 0x401f);
	miiphy_write(devname, 0x2, 0xe, 0x000a);

	/* emac1 phy: set rgmii driverstrength to max. value */
	miiphy_write(devname, 0x3, 0xd, 0x1f);
	miiphy_write(devname, 0x3, 0xe, 0x0170);
	miiphy_write(devname, 0x3, 0xd, 0x401f);
	miiphy_write(devname, 0x3, 0xe, 0x001f);

	/* emac1 phy: activate rgmii delay */
	miiphy_write(devname, 0x3, 0xd, 0x1f);
	miiphy_write(devname, 0x3, 0xe, 0x32);
	miiphy_write(devname, 0x3, 0xd, 0x401f);
	miiphy_write(devname, 0x3, 0xe, 0xd3);

	/* emac1 phy: set rgmii delay
	 * 0.25ns tx daley, 2.75ns rs delay
	 */
	miiphy_write(devname, 0x3, 0xd, 0x1f);
	miiphy_write(devname, 0x3, 0xe, 0x86);
	miiphy_write(devname, 0x3, 0xd, 0x401f);
	miiphy_write(devname, 0x3, 0xe, 0x000a);

	return ret;
}
#endif /* CONFIG_DRIVER_TI_CPSW */

#define CB2p_AM57XX_GPIO_nInterfaceOE GPIO_TO_PIN(3, 24)

void tqma57xx_bb_board_late_init(void)
{
    gpio_request( CB2p_AM57XX_GPIO_nInterfaceOE, "nInterfaceOE" );
    gpio_direction_output( CB2p_AM57XX_GPIO_nInterfaceOE, 0 );
    mdelay(20);

	if (!env_get("fdtfile")) {
		switch(omap_revision()) {
		case DRA752_ES2_0:
			env_set("fdtfile", "am572x-cb2p-swissQprint.dtb");
			break;
		default:
			env_set("fdtfile", "unknown");
		}
		printf("Setting fdtfile to %s\n", env_get("fdtfile"));
	}
}
