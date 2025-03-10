#include "mik32_hal_crypto.h"


__attribute__((weak)) void HAL_CRYPTO_MspInit(Crypto_HandleTypeDef* hcrypto)
{
    __HAL_PCC_CRYPTO_CLK_ENABLE();
}

void HAL_Crypto_CounterReset(Crypto_HandleTypeDef *hcrypto)
{
    hcrypto->Instance->CONFIG |= CRYPTO_CONFIG_C_RESET_M;
}

void HAL_Crypto_WaitReady(Crypto_HandleTypeDef *hcrypto)
{
    while (!(hcrypto->Instance->CONFIG & CRYPTO_CONFIG_READY_M));
}

void HAL_Crypto_SetAlgorithm(Crypto_HandleTypeDef *hcrypto, uint8_t Algorithm)
{
    hcrypto->Algorithm = Algorithm; 

    uint32_t ConfigTemp = hcrypto->Instance->CONFIG;
    
    ConfigTemp &= ~CRYPTO_CONFIG_DECODE_M; /* Обнуление DECODE */ 

    ConfigTemp |= Algorithm << CRYPTO_CONFIG_DECODE_S;

    hcrypto->Instance->CONFIG = ConfigTemp;
}

void HAL_Crypto_SetCipherMode(Crypto_HandleTypeDef *hcrypto, uint8_t CipherMode)
{
    hcrypto->CipherMode = CipherMode;
    
    uint32_t ConfigTemp = hcrypto->Instance->CONFIG;

    ConfigTemp &= ~CRYPTO_CONFIG_MODE_SEL_M; /* Обнуление MODE_SEL */

    ConfigTemp |= CipherMode << CRYPTO_CONFIG_MODE_SEL_S;

    hcrypto->Instance->CONFIG = ConfigTemp;
}

void HAL_Crypto_SetSwapMode(Crypto_HandleTypeDef *hcrypto, uint8_t SwapMode)
{
    hcrypto->SwapMode = SwapMode;
    
    uint32_t ConfigTemp = hcrypto->Instance->CONFIG;

    ConfigTemp &= ~CRYPTO_CONFIG_SWAP_MODE_M; /* Обнуление SWAP_MODE */ 

    ConfigTemp |= SwapMode << CRYPTO_CONFIG_SWAP_MODE_S;

    hcrypto->Instance->CONFIG = ConfigTemp;
}

void HAL_Crypto_SetOrderMode(Crypto_HandleTypeDef *hcrypto, uint8_t OrderMode)
{
    hcrypto->OrderMode = OrderMode;
    
    uint32_t ConfigTemp = hcrypto->Instance->CONFIG;

    ConfigTemp &= ~CRYPTO_CONFIG_ORDER_MODE_M; /* Обнуление ORDER_MODE */ 

    ConfigTemp |= OrderMode << CRYPTO_CONFIG_ORDER_MODE_S;

    hcrypto->Instance->CONFIG = ConfigTemp;
}

void HAL_Crypto_SetIV(Crypto_HandleTypeDef *hcrypto, uint32_t InitVector[], uint32_t IvLength)
{

    for (uint32_t i = 0; i < IvLength; i++)
    {
        hcrypto->Instance->INIT = InitVector[i];
    } 

    /* В режиме шифрования CTR длина вектора инициализации равна половине блока и такое же количество нулей */
    if(hcrypto->CipherMode == CRYPTO_CIPHER_MODE_CTR)
    {
        for (uint32_t i = 0; i < IvLength; i++)
        {
            hcrypto->Instance->INIT = 0;
        }   
    }

}

void HAL_Crypto_SetKey(Crypto_HandleTypeDef *hcrypto, uint32_t crypto_key[])
{
    uint32_t key_length = 0;

    switch (hcrypto->Algorithm)
    {
    case CRYPTO_ALG_KUZNECHIK:
        key_length = CRYPTO_KEY_KUZNECHIK;
        break;
    case CRYPTO_ALG_MAGMA:
        key_length = CRYPTO_KEY_MAGMA;
        break;
    case CRYPTO_ALG_AES:
        key_length = CRYPTO_KEY_AES;
        break;
    }

    /* Ключ должен быть инициализирован в режиме шифрования */
    hcrypto->Instance->CONFIG &= ~CRYPTO_CONFIG_DECODE_M;

    for (uint32_t i = 0; i < key_length; i++)
    {
        hcrypto->Instance->KEY = crypto_key[i];
    }

    HAL_Crypto_WaitReady(hcrypto);
}

void HAL_Crypto_Init(Crypto_HandleTypeDef *hcrypto)
{
    HAL_CRYPTO_MspInit(hcrypto);
    
    HAL_Crypto_SetAlgorithm(hcrypto, hcrypto->Algorithm);       /* Настройка алгоритма шифрования */
    HAL_Crypto_SetCipherMode(hcrypto, hcrypto->CipherMode);     /* Настройка режима шифрования */
    HAL_Crypto_SetSwapMode(hcrypto, hcrypto->SwapMode);         /* Настройка перестановки слова */
    HAL_Crypto_SetOrderMode(hcrypto, hcrypto->OrderMode);       /* Настройка порядка загрузки/выгрузки */


    #ifdef MIK32_CRYPTO_DEBUG

    switch (hcrypto->Algorithm)
    {
    case CRYPTO_ALG_KUZNECHIK:
        xprintf("KUZNECHIK- ");
        break;
    case CRYPTO_ALG_MAGMA:
        xprintf("MAGMA - ");
        break;
    case CRYPTO_ALG_AES:
        xprintf("AES - ");
        break;
    }

    switch (hcrypto->CipherMode)
    {
    case CRYPTO_CIPHER_MODE_ECB:
        xprintf("ECB\n");
        break;
    case CRYPTO_CIPHER_MODE_CBC:
        xprintf("CBC\n");
        break;
    case CRYPTO_CIPHER_MODE_CTR:
        xprintf("CTR\n");
        break;
    }

    #endif
}

void HAL_Crypto_Encode(Crypto_HandleTypeDef *hcrypto, uint32_t plain_text[], uint32_t cipher_text[], uint32_t text_length)
{
    uint8_t block_size = 0;

    switch (hcrypto->Algorithm)
    {
    case CRYPTO_ALG_KUZNECHIK:
        block_size = CRYPTO_BLOCK_KUZNECHIK;
        break;
    case CRYPTO_ALG_MAGMA:
        block_size = CRYPTO_BLOCK_MAGMA;
        break;
    case CRYPTO_ALG_AES:
        block_size = CRYPTO_BLOCK_AES;
        break;
    }
    
    if(((text_length % block_size) != 0)/* && (hcrypto->CipherMode != CRYPTO_CIPHER_MODE_CTR) */)
    {
        #ifdef MIK32_CRYPTO_DEBUG
        xprintf("Длина текста не кратна длине блока\n");
        #endif
        
        return;
    }

    /* Режим шифрования */
    hcrypto->Instance->CONFIG &= ~CRYPTO_CONFIG_DECODE_M;

    for (volatile uint32_t block_index = 0; block_index < text_length; block_index += block_size)
    {
        for (volatile uint32_t word_index = block_index; word_index < (block_index + block_size); word_index++)
        {
            if (word_index >= text_length)
            {
                break;
            }
            
            hcrypto->Instance->BLOCK = plain_text[word_index];
            
        }

        HAL_Crypto_WaitReady(hcrypto);

        for (volatile uint32_t word_index = block_index; word_index < (block_index + block_size); word_index++)
        {
            if (word_index >= text_length)
            {
                break;
            }
            
            cipher_text[word_index] = hcrypto->Instance->BLOCK;
        }
    }
     
}

void HAL_Crypto_Decode(Crypto_HandleTypeDef *hcrypto, uint32_t cipher_text[], uint32_t plain_text[], uint32_t text_length)
{
    uint8_t block_size = 0;

    switch (hcrypto->Algorithm)
    {
    case CRYPTO_ALG_KUZNECHIK:
        block_size = CRYPTO_BLOCK_KUZNECHIK;
        break;
    case CRYPTO_ALG_MAGMA:
        block_size = CRYPTO_BLOCK_MAGMA;
        break;
    case CRYPTO_ALG_AES:
        block_size = CRYPTO_BLOCK_AES;
        break;
    }

    if(((text_length % block_size) != 0)/* && (hcrypto->CipherMode != CRYPTO_CIPHER_MODE_CTR)*/)
    {
        #ifdef MIK32_CRYPTO_DEBUG
        xprintf("Длина текста не кратна длине блока\n");
        #endif
        
        return;
    }
    
    /* Режим расшифровки */
    hcrypto->Instance->CONFIG |= CRYPTO_CONFIG_DECODE_M;

    for (volatile uint32_t block_index = 0; block_index < text_length; block_index += block_size)
    {
        for (volatile uint32_t word_index = block_index; word_index < (block_index + block_size); word_index++)
        {
            if (word_index >= text_length)
            {
                break;
            }
            hcrypto->Instance->BLOCK = cipher_text[word_index];
        }

        HAL_Crypto_WaitReady(hcrypto);

        for (volatile uint32_t word_index = block_index; word_index < (block_index + block_size); word_index++)
        {
            if (word_index >= text_length)
            {
                break;
            }
            plain_text[word_index] = hcrypto->Instance->BLOCK;
        }
    }

}

