# Генерация множества Мандельброта на CPU и векторные оптимизации 

### Введение

Эта программа генерирует и визуализирует на экране множество Мандельброта. Алгоритм выполняется на центральном процессоре с применением SIMD инструкций.
Также в работе проводится сравнение эффективности 3 различных реализаций алгоритма рендеринга.

### Скачивание и установка

#### Зависимости

В этой программе для визуализации множества Мандельброта используется графическая библиотека [sfml](https://www.sfml-dev.org/), а для сборки используется система сборки ['make'](https://ru.wikipedia.org/wiki/Make)

#### Скачивание

В первую очередь нужно выполнить клонирование репозитория на свой компьютер выполнив следующую команду:

``` bash
git clone https://github.com/dmitry131131/SIMD_project.git
```

#### Сборка

Программу можно собрать в двух разных версиях
1. Графическая реализация 
2. Графическая реализация + тест производительности

Для сборки **первой** версии выполните команду: 
```bash
make
```
Для сборки **второй** версии выполните команду:
```bash
make test
```

### Использование

Окно приложения представляет собой графическое окно размером *800x600*px, в котором рисуется изображение фрактала Мандельброта. В левом верхнем углу отображается значение текущего FPS.

![Окно](img/win.png)

Взаимодействие с программой производится посредством клавиатуры.

Клавиша|Действие
:-----:|:-------
 ↑     | Сдвиг области видимости вверх
 ↓     | Сдвиг области видимости вниз
 ←     | Сдвиг области видимости влево
 →     | Сдвиг области видимости вправо
\+     | Увеличение изображения
\-     | Уменьшение изображения
1      | Выбор режима попиксельного рендеринга
2      | Выбор режима рендеринга массивом по 8 пикселей
3      | Выбор режима SIMD рендеринга

#### Тестовый режим
В тестовом режиме помимо графического окна в консоль печатается количество тактов процессора необходимое для рендеринга 20 изображений фрактала каждым из способов.
Ниже будут приведены усреднённые результаты многократных измерений в тестовом режиме

### Режимы рендеринга

1) **Попиксельный рендеринг** В этом режиме рендеринг происходит посредством отдельной обработки каждого пикселя в двойном цикле
2) **Оптимизация на массивах** В этом режиме программа за 1 итерацию цикла обрабатывает массив из 8 пикселей, что позволяет компилятору в режиме `-O3` выполнить частичную векторизацию этого процесса.
3) **Оптимизация на SIMD инструкциях** В этом режиме используются AVX2 методы, что позволяет добиться большей оптимизации, чем при неявном использовании AVX2 инструкций компилятором.

### Результаты тестов

В этой таблице представлены средние значения времени выполнения разных методов рендеринга в тиках процессора:

Метод рендеринга | Попиксельный | Массивы    | SIMD 
-----------------|--------------|------------|-
Тики             | 6269939484   | 2626102068 | 924851686

Из этих результатов можно сделать вывод, что оптимизация с массивами работает в `2.4` раза быстрее, чем простейшая реализация. А SIMD оптимизация быстрее в `6.8` раз. 





