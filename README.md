класс Algo реализует алгоритм Эрли для КС грамматик.

Описание некоторых классов:

Configuration - класс ситуации, left - нетерминал до стрелки в правиле, 
right - слово после стрелки. dot - индекс буквы, перед которой стоит "точка".

Grammar - грамматика, для быстрого поиска правил по первому нетерминалу, 
организована как хеш таблица.

Множества D организованы следующим образом: D[j][A] - можество ситуаций, в которых
после "точки" стоит символ A, для быстрой работы базовых операций. То есть 
множество D[j] реализовано через хеш таблицу.