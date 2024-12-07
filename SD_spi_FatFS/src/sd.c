#include "sd.h"

extern SD_Descriptor_t 			sd;

DMA_InitTypeDef hdma_sd;
DMA_ChannelHandleTypeDef hdma_sdspi_tx_ch;
DMA_ChannelHandleTypeDef hdma_sdspi_rx_ch;

static SD_Status_t SD_InterfaceInit(SD_Descriptor_t* local, SPI_HandleTypeDef* hspi, SPI_TypeDef* instance, uint8_t cs);

//--------------------------------------------------------------------------

SD_Status_t SD_Write(uint32_t addr, uint8_t* buf)  {
	return SD_SingleWrite(&sd, addr, buf);
}
//--------------------------------------------------------------------------

SD_Status_t SD_Read(uint32_t addr, uint8_t* buf) {
	return SD_SingleRead(&sd, addr, buf);
}
//--------------------------------------------------------------------------

SD_Status_t SD_SendCommand(SD_Descriptor_t* local, SD_Commands_enum command, uint32_t operand, uint8_t crc, uint8_t* resp) {
    uint8_t data = 0xFF;
    uint16_t timeout = 0;
    //HAL_SPI_CS_Enable(local->spi, SPI_CS_0);
    do {
        HAL_SPI_Exchange(local->spi, &data, resp, 1, SPI_TIMEOUT_DEFAULT);
        timeout += 1;
        if (timeout > 10000) return SD_CommunicationError;
    } while (*resp != 0xFF);

    data = (uint8_t)command;
    HAL_SPI_Exchange(local->spi, &data, resp, 1, SPI_TIMEOUT_DEFAULT);

    data = ((operand>>24) & 0xFF);
    HAL_SPI_Exchange(local->spi, &data, resp, 1, SPI_TIMEOUT_DEFAULT);

    data = ((operand>>16) & 0xFF);
    HAL_SPI_Exchange(local->spi, &data, resp, 1, SPI_TIMEOUT_DEFAULT);

    data = ((operand>>8) & 0xFF);
    HAL_SPI_Exchange(local->spi, &data, resp, 1, SPI_TIMEOUT_DEFAULT);

    data = (operand & 0xFF);
    HAL_SPI_Exchange(local->spi, &data, resp, 1, SPI_TIMEOUT_DEFAULT);

    data = crc;
    HAL_SPI_Exchange(local->spi, &data, resp, 1, SPI_TIMEOUT_DEFAULT);

    data = 0xFF;
    timeout = 0;

    do {
        HAL_SPI_Exchange(local->spi, &data, resp, 1, SPI_TIMEOUT_DEFAULT);
        timeout += 1;
        if (timeout > 10000) return SD_CommunicationError;
    } while (*resp == 0xFF);
    
    // R7 response
    if ((command == CMD8) || (command == CMD58))
    {
        for (uint8_t i=0; i<4; i++) HAL_SPI_Exchange(local->spi, &data, resp+i+1, 1, SPI_TIMEOUT_DEFAULT);
    }

    //HAL_SPI_CS_Disable(local->spi);
    //xprintf("Com: 0x%X; R1: %08b", command, resp[0]);
    // if ((command == CMD8) || (command == CMD58))
    // {
    //     uint32_t ocr = ((uint32_t)resp[1]<<24 | (uint32_t)resp[2]<<16 |
    //                     (uint32_t)resp[3]<<8 | resp[4]);
    //     xprintf(", extra: %032b", ocr);
    // }
    //xprintf("\n");

    return SD_OK;
}
//-------------------------------------------------------------------------

SD_Status_t SD_Init(SD_Descriptor_t* local, SPI_HandleTypeDef* hspi, SPI_TypeDef* instance, uint8_t cs, SCR1_TIMER_HandleTypeDef* hscr1_timer) {
    SD_Status_t res;

    res = SD_InterfaceInit(local, hspi, instance, cs);
    if (res != SD_OK) return res;

    // By default
    local->voltage = SD_Voltage_from_3_2_to_3_3;

    HAL_SPI_CS_Disable(local->spi);

    HAL_DelayMs(hscr1_timer, 200);// 100

    //80 тактов на линии SCK
    uint8_t data = 0xFF;
    uint8_t dummy;

    for (uint8_t i=0; i<10; i++) HAL_SPI_Exchange(local->spi, &data, &dummy, 1, SPI_TIMEOUT_DEFAULT);
    HAL_DelayMs(hscr1_timer, 50);
    uint8_t resp[5] = {0};

    HAL_SPI_CS_Enable(local->spi, SPI_CS_0);

    res = SD_SendCommand(local, CMD0, 0x0, 0x95, resp);
    if (res != SD_OK) return res;

    HAL_SPI_CS_Disable(local->spi);
    HAL_SPI_CS_Enable(local->spi, SPI_CS_0);

    res = SD_SendCommand(local, CMD8, 0x1AA, 0x87, resp);
    if (res != SD_OK) return res;
    
    // It is v1 SD-card or not-SD-card
    if (resp[0] & SD_R1_ILLEGAL_COMMAND_M) {

        res = SD_SendCommand(local, CMD58, 0, 0xFF, resp);

        if (res != SD_OK) return res;
        uint32_t ocr = ((uint32_t)resp[1]<<24 | (uint32_t)resp[2]<<16 |
                        (uint32_t)resp[3]<<8 | resp[4]);

        if (!(local->voltage & ocr)) {
            HAL_SPI_CS_Disable(local->spi);
            return SD_IncorrectVoltage;
        }
        if (resp[0] & SD_R1_ILLEGAL_COMMAND_M) {
            HAL_SPI_CS_Disable(local->spi);
            return SD_UnknownCard;
        }
        uint8_t counter = 200;

        // Trying to send ACMD41
        res = SD_SendCommand(local, CMD55, 0, 0xFF, resp);
        if (res != SD_OK) return res;

        res = SD_SendCommand(local, ACMD41, 0x40000000, 0xFF, resp);
        if (res != SD_OK) return res;

        // It is a MMC
        if (resp[0] & SD_R1_ILLEGAL_COMMAND_M) {
            // >74 clock cycles on SCK
            for (uint8_t i=0; i<10; i++) 
                HAL_SPI_Exchange(local->spi, &data, &dummy, 1, SPI_TIMEOUT_DEFAULT);

            // Go from idle_mode

            while (resp[0] & SD_R1_IDLE_STATE_M) {
                res = SD_SendCommand(local, CMD1, 0, 0xFF, resp);

                if (res != SD_OK) return res;
                counter -= 1;

                if (counter == 0) {
                    HAL_SPI_CS_Disable(local->spi);
                    return SD_TimeoutError;
                }
            }
            local->type = MMC;
            HAL_SPI_CS_Disable(local->spi);

            return SD_OK;
        } else {
        	// It is a SDv1
            // Go from idle_mode
            while (resp[0] & SD_R1_IDLE_STATE_M) {
                res = SD_SendCommand(local, CMD55, 0, 0xFF, resp);
                if (res != SD_OK) return res;

                res = SD_SendCommand(local, ACMD41, 0x40000000, 0xFF, resp);
                if (res != SD_OK) return res;
                counter -= 1;

                if (counter == 0) {
                    HAL_SPI_CS_Disable(local->spi);
                    return SD_TimeoutError;
                }
            }
            local->type = SDv1;
            HAL_SPI_CS_Disable(local->spi);

            return SD_OK;
        }
    } else {
    // It is SD v2, SDHC or SDXC card
    // check the check_pattern //
    	if (resp[4] != 0xAA) {
            HAL_SPI_CS_Disable(local->spi);
            xprintf("Communication error\r\n");
            return SD_CommunicationError;
        }

        // Check the card's valid voltage
        res = SD_SendCommand(local, CMD58, 0, 0xFF, resp);
        if (res != SD_OK) return res;

        uint32_t ocr = ((uint32_t)resp[1]<<24 | (uint32_t)resp[2]<<16 |
                        (uint32_t)resp[3]<<8 | resp[4]);
        if (!(local->voltage & ocr)) {
            HAL_SPI_CS_Disable(local->spi);
            xprintf("Incorrect voltage");
            return SD_IncorrectVoltage;
        }

        // >74 clock cycles on SCK
            for (uint8_t i=0; i<10; i++) 
                HAL_SPI_Exchange(local->spi, &data, &dummy, 1, SPI_TIMEOUT_DEFAULT);

        // Go from idle_mode
        uint8_t counter = 200;
        while (resp[0] & SD_R1_IDLE_STATE_M) {
            res = SD_SendCommand(local, CMD55, 0, 0xFF, resp);
            if (res != SD_OK) return res;

            res = SD_SendCommand(local, ACMD41, 0x40000000, 0xFF, resp);
            if (res != SD_OK) return res;
            counter -= 1;

            if (counter == 0) {
                HAL_SPI_CS_Disable(local->spi);
                return SD_TimeoutError;
            }
        }

        // Read the CCS value
        res = SD_SendCommand(local, CMD58, 0, 0xFF, resp);
        if (res != SD_OK) return res;

        uint8_t ccs = resp[1] & 0b01000000;
        if (ccs == 0) local->type = SDv2;
        else local->type = SDHC;

        HAL_SPI_CS_Disable(local->spi);
        return SD_OK;
    }

    // Increase clocking speed
    local->spi->Init.BaudRateDiv = SPI_BAUDRATE_DIV32;

    if (HAL_SPI_Init(local->spi) != HAL_OK) {
       return SD_CommunicationError;
    }
    return SD_OK;
}
//---------------------------------------------------------------------

#ifndef SD_DRIVER_USE_DMA
SD_Status_t SD_SingleRead(SD_Descriptor_t* local, uint32_t addr, uint8_t* buf) {
    uint8_t resp, dummy = 0xFF;
    SD_Status_t res;

    HAL_SPI_CS_Enable(local->spi, SPI_CS_0);

    res = SD_SendCommand(local, CMD17, addr, 0xff, &resp);
    if (res != SD_OK) return res;

    if (resp != 0) return resp;
    uint16_t counter = 0;

    while ((resp != 0xFE) && (resp != 0xFC)) {
        HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);

        if (counter >= SD_SREAD_WAIT_ATTEMPTS) return SD_TimeoutError;
        counter += 1;

        if (resp == 0xFF) continue;
    }

    for (uint16_t i=0; i<512; i++) HAL_SPI_Exchange(local->spi, &dummy, buf+i, 1, SPI_TIMEOUT_DEFAULT);

    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);

    HAL_SPI_CS_Disable(local->spi);
    return SD_OK;
}
#else
SD_Status_t SD_SingleRead(SD_Descriptor_t* local, uint32_t addr, uint8_t* buf) {
    uint8_t resp, dummy = 0xFF;
    SD_Status_t res;

    HAL_SPI_CS_Enable(local->spi, SPI_CS_0);

    res = SD_SendCommand(local, CMD17, addr, 0xff, &resp);
    if (res != SD_OK) return res;

    if (resp != 0) return resp;
    uint16_t counter = 0;

    while ((resp != 0xFE) && (resp != 0xFC)) {
        HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);

        if (counter >= SD_SREAD_WAIT_ATTEMPTS) return SD_TimeoutError;
        counter += 1;

        if (resp == 0xFF) continue;
    }
    
    // Data receiving & transmitting via DMA
    HAL_SPI_Enable(local->spi);
    dummy = 0xFF;
    hdma_sdspi_tx_ch.ChannelInit.ReadInc = DMA_CHANNEL_INC_DISABLE;
    hdma_sdspi_rx_ch.ChannelInit.WriteInc = DMA_CHANNEL_INC_ENABLE;

    HAL_DMA_Start(&hdma_sdspi_tx_ch, &dummy, (void *)&local->spi->Instance->TXDATA, 511);
    HAL_DMA_Start(&hdma_sdspi_rx_ch, (void *)&local->spi->Instance->RXDATA, buf, 511);

    if (HAL_DMA_Wait(&hdma_sdspi_tx_ch, DMA_TIMEOUT_DEFAULT) != HAL_OK) {
        //xprintf("Timeout CH0\n");
        return SD_CommunicationError;
    }

    if (HAL_DMA_Wait(&hdma_sdspi_rx_ch, DMA_TIMEOUT_DEFAULT) != HAL_OK) {
        //xprintf("Timeout CH1\n");
        return SD_CommunicationError;
    }
    HAL_SPI_Disable(local->spi);

    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_CS_Disable(local->spi);

    return SD_OK;
}
#endif


#ifndef SD_DRIVER_USE_DMA
SD_Status_t SD_SingleWrite(SD_Descriptor_t* local, uint32_t addr, uint8_t* buf) {
    uint8_t resp, dummy = 0xFF;
    SD_Status_t res;

    HAL_SPI_CS_Enable(local->spi, SPI_CS_0);

    res = SD_SendCommand(local, CMD24, addr, 0xff, &resp);
    if (res != SD_OK) return res;
    if (resp != 0) return resp;

    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    dummy = 0xFE;

    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);

    for (uint16_t i=0; i<512; i++) HAL_SPI_Exchange(local->spi, buf+i, &resp, 1, SPI_TIMEOUT_DEFAULT);

    dummy = 0xFF;

    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);

    HAL_SPI_CS_Disable(local->spi);
    return SD_OK;
}
#else
SD_Status_t SD_SingleWrite(SD_Descriptor_t* local, uint32_t addr, uint8_t* buf) {
    uint8_t resp, dummy = 0xFF;
    SD_Status_t res;

    HAL_SPI_CS_Enable(local->spi, SPI_CS_0);

    res = SD_SendCommand(local, CMD24, addr, 0xff, &resp);
    if (res != SD_OK) return res;
    if (resp != 0) return resp;

    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    dummy = 0xFE;
    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    
    // Data receiving & transmitting via DMA
    HAL_SPI_Enable(local->spi);

    hdma_sdspi_tx_ch.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_sdspi_rx_ch.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;

    HAL_DMA_Start(&hdma_sdspi_tx_ch, buf, (void *)&local->spi->Instance->TXDATA, 511);
    HAL_DMA_Start(&hdma_sdspi_rx_ch, (void *)&local->spi->Instance->RXDATA, &dummy, 511);

    if (HAL_DMA_Wait(&hdma_sdspi_tx_ch, DMA_TIMEOUT_DEFAULT) != HAL_OK) {
        //xprintf("Timeout CH0\n");
        return SD_CommunicationError;
    }
    if (HAL_DMA_Wait(&hdma_sdspi_rx_ch, DMA_TIMEOUT_DEFAULT) != HAL_OK) {
        //xprintf("Timeout CH1\n");
        return SD_CommunicationError;
    }
    HAL_SPI_Disable(local->spi);

    dummy = 0xFF;

    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);
    HAL_SPI_Exchange(local->spi, &dummy, &resp, 1, SPI_TIMEOUT_DEFAULT);

    HAL_SPI_CS_Disable(local->spi);
    return SD_OK;
}
#endif


SD_Status_t SD_SingleErase(SD_Descriptor_t* local, uint32_t addr) {
    uint8_t resp;
    SD_Status_t res;

    HAL_SPI_CS_Enable(local->spi, SPI_CS_0);

    res = SD_SendCommand(local, CMD32, addr, 0xFF, &resp);
    if (res != SD_OK) return res;
    if (resp != 0) return resp;

    res = SD_SendCommand(local, CMD33, addr, 0xFF, &resp);
    if (res != SD_OK) return res;
    if (resp != 0) return resp;

    res = SD_SendCommand(local, CMD38, 0, 0xFF, &resp);
    if (res != SD_OK) return res;
    HAL_SPI_CS_Disable(local->spi);
    return SD_OK;
}




 SD_Status_t SD_InterfaceInit(SD_Descriptor_t* local, SPI_HandleTypeDef* hspi, SPI_TypeDef* instance, uint8_t cs) {
    local->spi = hspi;

    // SPI init
    local->spi->Instance = instance;
    local->spi->Init.SPI_Mode = HAL_SPI_MODE_MASTER;
    local->spi->Init.CLKPhase = SPI_PHASE_ON;
    local->spi->Init.CLKPolarity = SPI_POLARITY_HIGH;
    local->spi->Init.ThresholdTX = 4;
    local->spi->Init.BaudRateDiv = SPI_BAUDRATE_DIV256;
    local->spi->Init.Decoder = SPI_DECODER_NONE;
    local->spi->Init.ManualCS = SPI_MANUALCS_ON;
    local->spi->Init.ChipSelect = cs;
 if (HAL_SPI_Init(local->spi) != HAL_OK) {
        xprintf("SPI_Init_Error\n");
        return SD_CommunicationError;
    }

    // DMA init
#ifdef SD_DRIVER_USE_DMA
    hdma_sd.Instance = DMA_CONFIG;
    hdma_sd.CurrentValue = DMA_CURRENT_VALUE_ENABLE;
    if (HAL_DMA_Init(&hdma_sd) != HAL_OK) {
        //xprintf("DMA_Init Error\n");
        return SD_CommunicationError;
    }
    // Channel init
    hdma_sdspi_tx_ch.dma = &hdma_sd;
    hdma_sdspi_tx_ch.ChannelInit.Channel = DMA_CHANNEL_0;
    hdma_sdspi_tx_ch.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;
    hdma_sdspi_tx_ch.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_sdspi_tx_ch.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_sdspi_tx_ch.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;
    hdma_sdspi_tx_ch.ChannelInit.ReadBurstSize = 0;
    hdma_sdspi_tx_ch.ChannelInit.ReadRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_sdspi_tx_ch.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;
    hdma_sdspi_tx_ch.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_sdspi_tx_ch.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_sdspi_tx_ch.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;
    hdma_sdspi_tx_ch.ChannelInit.WriteBurstSize = 0;
    hdma_sdspi_tx_ch.ChannelInit.WriteRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_sdspi_tx_ch.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
    hdma_sdspi_rx_ch.dma = &hdma_sd;
    hdma_sdspi_rx_ch.ChannelInit.Channel = DMA_CHANNEL_1;
    hdma_sdspi_rx_ch.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;
    hdma_sdspi_rx_ch.ChannelInit.ReadMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_sdspi_rx_ch.ChannelInit.ReadInc = DMA_CHANNEL_INC_DISABLE;
    hdma_sdspi_rx_ch.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;
    hdma_sdspi_rx_ch.ChannelInit.ReadBurstSize = 0;
    hdma_sdspi_rx_ch.ChannelInit.ReadRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_sdspi_rx_ch.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;
    hdma_sdspi_rx_ch.ChannelInit.WriteMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_sdspi_rx_ch.ChannelInit.WriteInc = DMA_CHANNEL_INC_ENABLE;
    hdma_sdspi_rx_ch.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;
    hdma_sdspi_rx_ch.ChannelInit.WriteBurstSize = 0;
    hdma_sdspi_rx_ch.ChannelInit.WriteRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_sdspi_rx_ch.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
#endif
    return SD_OK;
}
