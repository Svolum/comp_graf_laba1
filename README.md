![углы эйлера](https://rcsearch.ru/w/images/b/be/1fTbyJICl8g.jpg)

WASD, SPACE, CTRL + мышь - управление камерой
IJKL, U, O - управление позицией источника света, U - вверх и O - вниз
1-8 управление частями модели

# запуск
запуск производится через **run.bat**, если избегать microsof visual studio

если не запускается можно попытаться сделать так
``` bash
cmake .. -G "Visual Studio 17 2022" -A x64
```

для получения исполняемого файла. // после того как проект собран
``` bash
cmake --build . --config Release
```
