﻿# spi_uart_gateway

Tестовое задание:


Аппаратная платформа: STM32
Программная платформа: FreeRTOS

Устройство имеет USART и SPI в режиме master. Необходимо реализовать двунаправленный шлюз USART ↔️ SPI. 

- Данные передаются сообщениями, протокол можно определить произвольно
- Сообщения на USART и SPI могут приходить асинхронно (у SPI slave устройства может возникнуть необходимость передать сообщение в любой момент)
- Работа с USART и SPI должна быть реализована в разных задачах

1) размер фрэйма динамический (значение длины 16 бит)
2) формат фрейма - строка (оканчиваются NULL), в SPI slave устройстве нет никаких внутренних регистров и прочих адресаций. Начиная с первого же байта оно начинает слать строку. Если устройству нечего передавать, то начиная с первого же байта оно шлет NULL. И также начиная с первого же байта оно принимает строку.
Со стороны уарта также приходят просто строки с символом \0 на  конце.
3) размер очереди - можете установить по 10 в обе стороны.
4) скорости - на ваше усмотрение

Результат - в виде Github репозитория.
