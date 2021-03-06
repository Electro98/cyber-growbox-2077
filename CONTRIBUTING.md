# Коллективное участие в проекте

Ниже приводится набор рекомендаций по участию в проекте CyberGrowBox :seedling:. 
Это в основном рекомендации, а не правила. Используйте здравый смысл и
не стесняйтесь предлагать изменения в этот документ.

## Как я могу внести свой склад?

### Сообщение об ошибках

В этом разделе представлены инструкции по отправке отчета об ошибке для
CyberGrowBox. Следование этим рекомендациям помогает специалистам по
сопровождению и сообществу понять ваш отчет :clipboard:, воспроизвести
поведение :computer:.

Если вы нашли ошибку не стесняйтесь сообщить об этом.

Чтобы сообщить об ошибке придерживайтесь следующих простых правил :memo::

- Проверьте раздел ```issues```, чтобы не создавать дубликатов
- Задайте понятный заголовок ```issues```, лаконично и исчерпывающе определяющий проблему
    - постарайтесь не допускать двойного смысла, сленга и т.д.
- Опишите сценарий воспроизведения ошибки или место в тексте, где она присутствует
    - скриншоты очень сильно помогают, но не заменяют сценарий
    - добавьте сообщение об ошибке (если это связано с кодом)
- Опишите в чем заключается ошибка по вашему мнению
- Опишите ожидаемое поведение или представление

### Предложения об улучшении

В этом разделе представлены инструкции по отправке предложений по улучшению
CyberGrowBox, включая совершенно новые функции и незначительные улучшения
существующих функций. Следование этим рекомендациям помогает разработчикам
и сообществу понять ваше предложение :clipboard:. Прежде чем создавать
предложения по улучшению, проверьте раздел ```issues```, чтобы не создавать
дубликатов. Когда вы создаете предложение по улучшению, включите как можно
больше деталей.

Для внесения предложения по улучшению следуйте простым шагам:

- Задайте понятный заголовок ```issue```, лаконично и исчерпывающе
определяющий Ваше предложение.
- Укажите, что вы предлагаете: новую функциональность, вариант визуализации и др.
- Опишите суть предложения и обсудите в ```issue``` варианты реализации,
предоставляемые возможности и т.д.
- Реализуйте Ваше предложение и предложите его через ```Pull request```.

## Руководство по стилю

### Сообщения коммитов

- Ограничьте первую строку 72 символами или меньше 
- Подумайте о том, чтобы начать сообщение с подходящего эмодзи: 
    - :art: ```:art:``` при улучшении формата/структуры кода 
    - :memo: ```:memo:``` при написании документации
    - :pushpin: ```:pushpin:``` при добавлении сопутствующих файлов 
    - :wrench: ```:wrench:``` при исправлении чего-либо для Arduino
    - :snake: ```:snake:``` при исправлении чего-либо на сервере
    - :bug: ```:bug:``` при исправлении ошибки 
    - :fire: ```:fire:``` при удалении кода или файлов 
    - :green_heart: ```:green_heart:``` при исправлении сборки CI/CD 
    - :white_check_mark:  ```:white_check_mark:``` при добавлении тестов 
    - :arrow_up: ```:arrow_up:``` при добавлении зависимостей 
    - :arrow_down: ```:arrow_down:``` при удалении зависимостей

### Стиль кодирования Python

Для оформления кода на Python необходимо придерживаться следующих стандартов:

- [PEP 8 - Стиль кодирования на Python](https://www.python.org/dev/peps/pep-0008/)
- [PEP 257 - Соглашение о строках документации](https://www.python.org/dev/peps/pep-0257/)

Рекомендуется добавлять аннотации типов, используя стандарты:

- [PEP 484 -- Type Hints](https://www.python.org/dev/peps/pep-0484/)
- [PEP 526 -- Syntax for Variable Annotations](https://www.python.org/dev/peps/pep-0526/)
- [PEP 3107 -- Function Annotations](https://www.python.org/dev/peps/pep-3107/)

### Стиль строк документации Python

Строки документации в коде на Python рекомендуется оформлять по следующим стандартам:

- [PEP 257 - Соглашение о строках документации](https://www.python.org/dev/peps/pep-0257/)

Следует придерживаться стиля Sphinx, который использует разметку reStructuredText:

- [reStructuredText](https://docutils.sourceforge.io/rst.html)

### Стиль кодирования С/С++

При оформлении кода на С/С++ необходимо придерживаться следующих стандартов:

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Руководство Google по стилю в C++](https://evgenykislov.com/wp-content/custom/cpp_codestyle/cppguide_ru.html)

### Стиль строк документации С/С++

Следует придерживаться стиля Sphinx, который использует разметку reStructuredText:

- [reStructuredText](https://docutils.sourceforge.io/rst.html)
