### Репозиторий типовых примеров для мироконтроллера MIK32 Amur 
Примеры иллюстрируют базовые способы использования периферии Mik32 Amur,  библиотека HAL  по возможности используется в минимальном объеме. 

> Данные проекты проверены на работоспособность с MikronIDE_v0_0_9.
> В некоторых проектах отдельные файлы библиотеки HAL исправлены и отредактированы. 

Список примеров:
- [Bootloader](https://github.com/ppryaz/mik32_Amur_example/tree/main/Bootloader) - Загрузчик
- [GPIO_PIN_IT](https://github.com/ppryaz/mik32_Amur_example/tree/main/GPIO_PIN_IT) - Использование 3-х кнопок с прерыванием. 	При получении прерывания от кнопки,  по UART в терминале выводится сообщения о сработавшей кнопке
- [HAL_EEPROM](https://github.com/ppryaz/mik32_Amur_example/tree/main/HAL_EEPROM) - использование встроенной EEPROM
- [HAL_i2c_scanner](https://github.com/ppryaz/mik32_Amur_example/tree/main/HAL_i2c_scanner) - сканер I2C устройств на шине
- [Lab3_UART_DAC](https://github.com/ppryaz/mik32_Amur_example/tree/main/Lab3_UART_DAC) - работа с ЦАП
- [Lab7_ADC](https://github.com/ppryaz/mik32_Amur_example/tree/main/Lab7_ADC) - работа с АЦП
- [RTC](https://github.com/ppryaz/mik32_Amur_example/tree/main/RTC) - Пример использования встроенного модуля RTC
- [SD_spi_test]( https://github.com/ppryaz/mik32_Amur_example/tree/main/SD_spi_test) - Работа драйвера SD карт через интерфейс SPI. Используется файловая система MIK32FAT. 
- [SPI_board_selftest](https://github.com/ppryaz/mik32_Amur_example/tree/main/SPI_board_selftest) - эхо по SPI
- [TIM16_Encoder](https://github.com/ppryaz/mik32_Amur_example/tree/main/TIM16_Encoder) - работа с инкрементальным энкодером 
- [UART_DAC_manager](https://github.com/ppryaz/mik32_Amur_example/tree/main/UART_DAC_manager) - управление DAC через команды с терминала
- [UART_IT_RX_TX](https://github.com/ppryaz/mik32_Amur_example/tree/main/UART_IT_RX_TX) - UART echo with interrupt (эхо по UART с прерываниями)
- [UART_TX_RX](https://github.com/ppryaz/mik32_Amur_example/tree/main/UART_TX_RX) - UART echo ( эхо по UART в основном цикле)
- [UART_TX_RX_xprintf](https://github.com/ppryaz/mik32_Amur_example/tree/main/UART_TX_RX_xprintf) - Пример настройки вывода отладочных сообщений по UART
- [lcd2004a_i2c](https://github.com/ppryaz/mik32_Amur_example/tree/main/lcd2004a_i2c) - Управление дисплеем LCD2004 через I2C
- [lcd2004a_i2c_RusText](https://github.com/ppryaz/mik32_Amur_example/tree/main/lcd2004a_i2c_RusText)- Работа с дисплеем LCD2004 модель WH2004A_YGH_CT с поддержкой русского языка.
- [W5500_ethernet](https://github.com/ppryaz/mik32_Amur_example/tree/main/W5500_ethernet) - Пример использования модуля Ethernet W5500
- [SD_spi_FatFS](https://github.com/ppryaz/mik32_Amur_example/tree/main/SD_spi_FatFS) - Использование FatFs для работы с SD картой


#### Различия в разных версиях MikronIDE
В разных версиях MikronIDE файл memory_map  назван по-разному. Эта версия использует mcu32_memory_map.h.

Существенное отличие в файле mik32_hal_gpio.h. В нем структура __HAL_PinsTypeDef определяющяя маску выводов заменена на __HAL_PinMapNewTypeDef.
Отличие заключается в том, что в последней версии  порт и pin определяются одновременно (PORT0_0).

#### Полезные ссылки:
- [Сайт производителя](https://mikron.ru/products/mikrokontrollery/mk32-amur/?ysclid=m433334y8n259691795)
- [Техническое описание MIK32](https://nc.mikron.ru/s/aXSRc8HdLAM2LLg/download)
- [Wiki по mik32](https://wiki.mik32.ru/Заглавная_страница)
- [Официальный репозиторий микроконтроллера Mikron MIK32 Амур](https://github.com/MikronMIK32?ysclid=m434su1vbn219021505)
- [VK Сообщество MIK32 Амур - К1948ВК018](https://vk.com/mik32_amur?ysclid=m4339m5olf94696113)
- [Форма обращения в техподдержку](https://bugreport.mik32.ru/)
